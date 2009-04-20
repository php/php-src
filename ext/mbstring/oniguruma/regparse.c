/**********************************************************************
  regparse.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2007  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "regparse.h"

#define WARN_BUFSIZE    256

OnigSyntaxType OnigSyntaxRuby = {
  (( SYN_GNU_REGEX_OP | ONIG_SYN_OP_QMARK_NON_GREEDY |
     ONIG_SYN_OP_ESC_OCTAL3 | ONIG_SYN_OP_ESC_X_HEX2 |
     ONIG_SYN_OP_ESC_X_BRACE_HEX8 | ONIG_SYN_OP_ESC_CONTROL_CHARS |
     ONIG_SYN_OP_ESC_C_CONTROL )
   & ~ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END )
  , ( ONIG_SYN_OP2_QMARK_GROUP_EFFECT |
      ONIG_SYN_OP2_OPTION_RUBY |
      ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP | ONIG_SYN_OP2_ESC_K_NAMED_BACKREF |
      ONIG_SYN_OP2_ESC_G_SUBEXP_CALL |
      ONIG_SYN_OP2_PLUS_POSSESSIVE_REPEAT |
      ONIG_SYN_OP2_CCLASS_SET_OP | ONIG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL |
      ONIG_SYN_OP2_ESC_CAPITAL_M_BAR_META | ONIG_SYN_OP2_ESC_V_VTAB |
      ONIG_SYN_OP2_ESC_H_XDIGIT )
  , ( SYN_GNU_REGEX_BV | 
      ONIG_SYN_ALLOW_INTERVAL_LOW_ABBREV |
      ONIG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND |
      ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP |
      ONIG_SYN_ALLOW_MULTIPLEX_DEFINITION_NAME |
      ONIG_SYN_FIXED_INTERVAL_IS_GREEDY_ONLY |
      ONIG_SYN_WARN_CC_OP_NOT_ESCAPED |
      ONIG_SYN_WARN_REDUNDANT_NESTED_REPEAT )
  , ONIG_OPTION_NONE
};

OnigSyntaxType*  OnigDefaultSyntax = ONIG_SYNTAX_RUBY;

extern void onig_null_warn(const char* s) { }

#ifdef RUBY_PLATFORM
extern void
onig_rb_warn(const char* s)
{
  rb_warn("%s", s);
}

extern void
onig_rb_warning(const char* s)
{
  rb_warning("%s", s);
}
#endif

#ifdef DEFAULT_WARN_FUNCTION
static OnigWarnFunc onig_warn = (OnigWarnFunc )DEFAULT_WARN_FUNCTION;
#else
static OnigWarnFunc onig_warn = onig_null_warn;
#endif

#ifdef DEFAULT_VERB_WARN_FUNCTION
static OnigWarnFunc onig_verb_warn = (OnigWarnFunc )DEFAULT_VERB_WARN_FUNCTION;
#else
static OnigWarnFunc onig_verb_warn = onig_null_warn;
#endif

extern void onig_set_warn_func(OnigWarnFunc f)
{
  onig_warn = f;
}

extern void onig_set_verb_warn_func(OnigWarnFunc f)
{
  onig_verb_warn = f;
}

static void
bbuf_free(BBuf* bbuf)
{
  if (IS_NOT_NULL(bbuf)) {
    if (IS_NOT_NULL(bbuf->p)) xfree(bbuf->p);
    xfree(bbuf);
  }
}

static int
bbuf_clone(BBuf** rto, BBuf* from)
{
  int r;
  BBuf *to;

  *rto = to = (BBuf* )xmalloc(sizeof(BBuf));
  CHECK_NULL_RETURN_VAL(to, ONIGERR_MEMORY);
  r = BBUF_INIT(to, from->alloc);
  if (r != 0) return r;
  to->used = from->used;
  xmemcpy(to->p, from->p, from->used);
  return 0;
}

#define ONOFF(v,f,negative)    (negative) ? ((v) &= ~(f)) : ((v) |= (f))

#define MBCODE_START_POS(enc) \
  (OnigCodePoint )(ONIGENC_MBC_MINLEN(enc) > 1 ? 0 : 0x80)

#define SET_ALL_MULTI_BYTE_RANGE(enc, pbuf) \
  add_code_range_to_buf(pbuf, MBCODE_START_POS(enc), ~((OnigCodePoint )0))

#define ADD_ALL_MULTI_BYTE_RANGE(enc, mbuf) do {\
  if (! ONIGENC_IS_SINGLEBYTE(enc)) {\
    r = SET_ALL_MULTI_BYTE_RANGE(enc, &(mbuf));\
    if (r) return r;\
  }\
} while (0)


#define BITSET_IS_EMPTY(bs,empty) do {\
  int i;\
  empty = 1;\
  for (i = 0; i < BITSET_SIZE; i++) {\
    if ((bs)[i] != 0) {\
      empty = 0; break;\
    }\
  }\
} while (0)

static void
bitset_set_range(BitSetRef bs, int from, int to)
{
  int i;
  for (i = from; i <= to && i < SINGLE_BYTE_SIZE; i++) {
    BITSET_SET_BIT(bs, i);
  }
}

#if 0
static void
bitset_set_all(BitSetRef bs)
{
  int i;
  for (i = 0; i < BITSET_SIZE; i++) {
    bs[i] = ~((Bits )0);
  }
}
#endif

static void
bitset_invert(BitSetRef bs)
{
  int i;
  for (i = 0; i < BITSET_SIZE; i++) {
    bs[i] = ~(bs[i]);
  }
}

static void
bitset_invert_to(BitSetRef from, BitSetRef to)
{
  int i;
  for (i = 0; i < BITSET_SIZE; i++) {
    to[i] = ~(from[i]);
  }
}

static void
bitset_and(BitSetRef dest, BitSetRef bs)
{
  int i;
  for (i = 0; i < BITSET_SIZE; i++) {
    dest[i] &= bs[i];
  }
}

static void
bitset_or(BitSetRef dest, BitSetRef bs)
{
  int i;
  for (i = 0; i < BITSET_SIZE; i++) {
    dest[i] |= bs[i];
  }
}

static void
bitset_copy(BitSetRef dest, BitSetRef bs)
{
  int i;
  for (i = 0; i < BITSET_SIZE; i++) {
    dest[i] = bs[i];
  }
}

extern int
onig_strncmp(const UChar* s1, const UChar* s2, int n)
{
  int x;

  while (n-- > 0) {
    x = *s2++ - *s1++;
    if (x) return x;
  }
  return 0;
}

static void
k_strcpy(UChar* dest, const UChar* src, const UChar* end)
{
  int len = end - src;
  if (len > 0) {
    xmemcpy(dest, src, len);
    dest[len] = (UChar )0;
  }
}

static UChar*
strdup_with_null(OnigEncoding enc, UChar* s, UChar* end)
{
  int slen, term_len, i;
  UChar *r;

  slen = end - s;
  term_len = ONIGENC_MBC_MINLEN(enc);

  r = (UChar* )xmalloc(slen + term_len);
  CHECK_NULL_RETURN(r);
  xmemcpy(r, s, slen);

  for (i = 0; i < term_len; i++)
    r[slen + i] = (UChar )0;

  return r;
}


/* scan pattern methods */
#define PEND_VALUE   0

#define PFETCH_READY  UChar* pfetch_prev
#define PEND         (p < end ?  0 : 1)
#define PUNFETCH     p = pfetch_prev
#define PINC       do { \
  pfetch_prev = p; \
  p += ONIGENC_MBC_ENC_LEN(enc, p); \
} while (0)
#define PFETCH(c)  do { \
  c = ONIGENC_MBC_TO_CODE(enc, p, end); \
  pfetch_prev = p; \
  p += ONIGENC_MBC_ENC_LEN(enc, p); \
} while (0)

#define PPEEK        (p < end ? ONIGENC_MBC_TO_CODE(enc, p, end) : PEND_VALUE)
#define PPEEK_IS(c)  (PPEEK == (OnigCodePoint )c)

static UChar*
k_strcat_capa(UChar* dest, UChar* dest_end, const UChar* src, const UChar* src_end,
	      int capa)
{
  UChar* r;

  if (dest)
    r = (UChar* )xrealloc(dest, capa + 1);
  else
    r = (UChar* )xmalloc(capa + 1);

  CHECK_NULL_RETURN(r);
  k_strcpy(r + (dest_end - dest), src, src_end);
  return r;
}

/* dest on static area */
static UChar*
strcat_capa_from_static(UChar* dest, UChar* dest_end,
			const UChar* src, const UChar* src_end, int capa)
{
  UChar* r;

  r = (UChar* )xmalloc(capa + 1);
  CHECK_NULL_RETURN(r);
  k_strcpy(r, dest, dest_end);
  k_strcpy(r + (dest_end - dest), src, src_end);
  return r;
}

#ifdef USE_NAMED_GROUP

#define INIT_NAME_BACKREFS_ALLOC_NUM   8

typedef struct {
  UChar* name;
  int    name_len;   /* byte length */
  int    back_num;   /* number of backrefs */
  int    back_alloc;
  int    back_ref1;
  int*   back_refs;
} NameEntry;

#ifdef USE_ST_HASH_TABLE

#include "st.h"

typedef struct {
  unsigned char* s;
  unsigned char* end;
} st_strend_key;

static int strend_cmp(st_strend_key*, st_strend_key*);
static int strend_hash(st_strend_key*);

static struct st_hash_type type_strend_hash = {
  strend_cmp,
  strend_hash,
};

static st_table*
onig_st_init_strend_table_with_size(int size)
{
    return onig_st_init_table_with_size(&type_strend_hash, size);
}

static int
onig_st_lookup_strend(st_table *table, const UChar* str_key, const UChar* end_key, st_data_t *value)
{
    st_strend_key key;

    key.s   = (unsigned char* )str_key;
    key.end = (unsigned char* )end_key;

    return onig_st_lookup(table, (st_data_t )(&key), value);
}

static int
onig_st_insert_strend(st_table *table, const UChar* str_key, const UChar* end_key, st_data_t value)
{
  st_strend_key* key;
  int result;

  key = (st_strend_key* )xmalloc(sizeof(st_strend_key));
  key->s   = (unsigned char* )str_key;
  key->end = (unsigned char* )end_key;
  result = onig_st_insert(table, (st_data_t )key, value);
  if (result) {
    xfree(key);
  }
  return result;
}

static int
strend_cmp(st_strend_key* x, st_strend_key* y)
{
  unsigned char *p, *q;
  int c;

  if ((x->end - x->s) != (y->end - y->s))
    return 1;

  p = x->s;
  q = y->s;
  while (p < x->end) {
    c = (int )*p - (int )*q;
    if (c != 0) return c;

    p++; q++;
  }

  return 0;
}

static int
strend_hash(st_strend_key* x)
{
  int val;
  unsigned char *p;

  val = 0;
  p = x->s;
  while (p < x->end) {
    val = val * 997 + (int )*p++;
  }

  return val + (val >> 5);
}

typedef st_table  NameTable;
typedef st_data_t HashDataType;   /* 1.6 st.h doesn't define st_data_t type */

#define NAMEBUF_SIZE    24
#define NAMEBUF_SIZE_1  25

#ifdef ONIG_DEBUG
static int
i_print_name_entry(UChar* key, NameEntry* e, void* arg)
{
  int i;
  FILE* fp = (FILE* )arg;

  fprintf(fp, "%s: ", e->name);
  if (e->back_num == 0)
    fputs("-", fp);
  else if (e->back_num == 1)
    fprintf(fp, "%d", e->back_ref1);
  else {
    for (i = 0; i < e->back_num; i++) {
      if (i > 0) fprintf(fp, ", ");
      fprintf(fp, "%d", e->back_refs[i]);
    }
  }
  fputs("\n", fp);
  return ST_CONTINUE;
}

extern int
onig_print_names(FILE* fp, regex_t* reg)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    fprintf(fp, "name table\n");
    onig_st_foreach(t, i_print_name_entry, (HashDataType )fp);
    fputs("\n", fp);
  }
  return 0;
}
#endif

static int
i_free_name_entry(UChar* key, NameEntry* e, void* arg)
{
  xfree(e->name);
  if (IS_NOT_NULL(e->back_refs)) xfree(e->back_refs);
  xfree(key);
  xfree(e);
  return ST_DELETE;
}

static int
names_clear(regex_t* reg)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    onig_st_foreach(t, i_free_name_entry, 0);
  }
  return 0;
}

extern int
onig_names_free(regex_t* reg)
{
  int r;
  NameTable* t;

  r = names_clear(reg);
  if (r) return r;

  t = (NameTable* )reg->name_table;
  if (IS_NOT_NULL(t)) onig_st_free_table(t);
  reg->name_table = (void* )NULL;
  return 0;
}

static NameEntry*
name_find(regex_t* reg, const UChar* name, const UChar* name_end)
{
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  e = (NameEntry* )NULL;
  if (IS_NOT_NULL(t)) {
    onig_st_lookup_strend(t, name, name_end, (HashDataType* )((void* )(&e)));
  }
  return e;
}

typedef struct {
  int (*func)(const UChar*, const UChar*,int,int*,regex_t*,void*);
  regex_t* reg;
  void* arg;
  int ret;
  OnigEncoding enc;
} INamesArg;

static int
i_names(UChar* key, NameEntry* e, INamesArg* arg)
{
  int r = (*(arg->func))(e->name,
                   /*e->name + onigenc_str_bytelen_null(arg->enc, e->name), */
                         e->name + e->name_len,
                         e->back_num,
			 (e->back_num > 1 ? e->back_refs : &(e->back_ref1)),
			 arg->reg, arg->arg);
  if (r != 0) {
    arg->ret = r;
    return ST_STOP;
  }
  return ST_CONTINUE;
}

extern int
onig_foreach_name(regex_t* reg,
	   int (*func)(const UChar*, const UChar*,int,int*,regex_t*,void*),
	   void* arg)
{
  INamesArg narg;
  NameTable* t = (NameTable* )reg->name_table;

  narg.ret = 0;
  if (IS_NOT_NULL(t)) {
    narg.func = func;
    narg.reg  = reg;
    narg.arg  = arg;
    narg.enc  = reg->enc; /* should be pattern encoding. */
    onig_st_foreach(t, i_names, (HashDataType )&narg);
  }
  return narg.ret;
}

static int
i_renumber_name(UChar* key, NameEntry* e, GroupNumRemap* map)
{
  int i;

  if (e->back_num > 1) {
    for (i = 0; i < e->back_num; i++) {
      e->back_refs[i] = map[e->back_refs[i]].new_val;
    }
  }
  else if (e->back_num == 1) {
    e->back_ref1 = map[e->back_ref1].new_val;
  }

  return ST_CONTINUE;
}

extern int
onig_renumber_name_table(regex_t* reg, GroupNumRemap* map)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    onig_st_foreach(t, i_renumber_name, (HashDataType )map);
  }
  return 0;
}


extern int
onig_number_of_names(regex_t* reg)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t))
    return t->num_entries;
  else
    return 0;
}

#else  /* USE_ST_HASH_TABLE */

#define INIT_NAMES_ALLOC_NUM    8

typedef struct {
  NameEntry* e;
  int        num;
  int        alloc;
} NameTable;


#ifdef ONIG_DEBUG
extern int
onig_print_names(FILE* fp, regex_t* reg)
{
  int i, j;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t) && t->num > 0) {
    fprintf(fp, "name table\n");
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      fprintf(fp, "%s: ", e->name);
      if (e->back_num == 0) {
	fputs("-", fp);
      }
      else if (e->back_num == 1) {
	fprintf(fp, "%d", e->back_ref1);
      }
      else {
	for (j = 0; j < e->back_num; j++) {
	  if (j > 0) fprintf(fp, ", ");
	  fprintf(fp, "%d", e->back_refs[j]);
	}
      }
      fputs("\n", fp);
    }
    fputs("\n", fp);
  }
  return 0;
}
#endif

static int
names_clear(regex_t* reg)
{
  int i;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      if (IS_NOT_NULL(e->name)) {
	xfree(e->name);
	e->name       = NULL;
	e->name_len   = 0;
	e->back_num   = 0;
	e->back_alloc = 0;
	if (IS_NOT_NULL(e->back_refs)) xfree(e->back_refs);
	e->back_refs = (int* )NULL;
      }
    }
    if (IS_NOT_NULL(t->e)) {
      xfree(t->e);
      t->e = NULL;
    }
    t->num = 0;
  }
  return 0;
}

extern int
onig_names_free(regex_t* reg)
{
  int r;
  NameTable* t;

  r = names_clear(reg);
  if (r) return r;

  t = (NameTable* )reg->name_table;
  if (IS_NOT_NULL(t)) xfree(t);
  reg->name_table = NULL;
  return 0;
}

static NameEntry*
name_find(regex_t* reg, UChar* name, UChar* name_end)
{
  int i, len;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    len = name_end - name;
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      if (len == e->name_len && onig_strncmp(name, e->name, len) == 0)
	return e;
    }
  }
  return (NameEntry* )NULL;
}

extern int
onig_foreach_name(regex_t* reg,
	   int (*func)(const UChar*, const UChar*,int,int*,regex_t*,void*),
	   void* arg)
{
  int i, r;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      r = (*func)(e->name, e->name + e->name_len, e->back_num,
		  (e->back_num > 1 ? e->back_refs : &(e->back_ref1)),
		  reg, arg);
      if (r != 0) return r;
    }
  }
  return 0;
}

extern int
onig_number_of_names(regex_t* reg)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t))
    return t->num;
  else
    return 0;
}

#endif /* else USE_ST_HASH_TABLE */

static int
name_add(regex_t* reg, UChar* name, UChar* name_end, int backref, ScanEnv* env)
{
  int alloc;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (name_end - name <= 0)
    return ONIGERR_EMPTY_GROUP_NAME;

  e = name_find(reg, name, name_end);
  if (IS_NULL(e)) {
#ifdef USE_ST_HASH_TABLE
    if (IS_NULL(t)) {
      t = onig_st_init_strend_table_with_size(5);
      reg->name_table = (void* )t;
    }
    e = (NameEntry* )xmalloc(sizeof(NameEntry));
    CHECK_NULL_RETURN_VAL(e, ONIGERR_MEMORY);

    e->name = strdup_with_null(reg->enc, name, name_end);
    if (IS_NULL(e->name)) return ONIGERR_MEMORY;
    onig_st_insert_strend(t, e->name, (e->name + (name_end - name)),
                          (HashDataType )e);

    e->name_len   = name_end - name;
    e->back_num   = 0;
    e->back_alloc = 0;
    e->back_refs  = (int* )NULL;

#else

    if (IS_NULL(t)) {
      alloc = INIT_NAMES_ALLOC_NUM;
      t = (NameTable* )xmalloc(sizeof(NameTable));
      CHECK_NULL_RETURN_VAL(t, ONIGERR_MEMORY);
      t->e     = NULL;
      t->alloc = 0;
      t->num   = 0;

      t->e = (NameEntry* )xmalloc(sizeof(NameEntry) * alloc);
      if (IS_NULL(t->e)) {
	xfree(t);
	return ONIGERR_MEMORY;
      }
      t->alloc = alloc;
      reg->name_table = t;
      goto clear;
    }
    else if (t->num == t->alloc) {
      int i;

      alloc = t->alloc * 2;
      t->e = (NameEntry* )xrealloc(t->e, sizeof(NameEntry) * alloc);
      CHECK_NULL_RETURN_VAL(t->e, ONIGERR_MEMORY);
      t->alloc = alloc;

    clear:
      for (i = t->num; i < t->alloc; i++) {
	t->e[i].name       = NULL;
	t->e[i].name_len   = 0;
	t->e[i].back_num   = 0;
	t->e[i].back_alloc = 0;
	t->e[i].back_refs  = (int* )NULL;
      }
    }
    e = &(t->e[t->num]);
    t->num++;
    e->name = strdup_with_null(reg->enc, name, name_end);
    e->name_len = name_end - name;
#endif
  }

  if (e->back_num >= 1 &&
      ! IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_MULTIPLEX_DEFINITION_NAME)) {
    onig_scan_env_set_error_string(env, ONIGERR_MULTIPLEX_DEFINED_NAME,
				    name, name_end);
    return ONIGERR_MULTIPLEX_DEFINED_NAME;
  }

  e->back_num++;
  if (e->back_num == 1) {
    e->back_ref1 = backref;
  }
  else {
    if (e->back_num == 2) {
      alloc = INIT_NAME_BACKREFS_ALLOC_NUM;
      e->back_refs = (int* )xmalloc(sizeof(int) * alloc);
      CHECK_NULL_RETURN_VAL(e->back_refs, ONIGERR_MEMORY);
      e->back_alloc = alloc;
      e->back_refs[0] = e->back_ref1;
      e->back_refs[1] = backref;
    }
    else {
      if (e->back_num > e->back_alloc) {
	alloc = e->back_alloc * 2;
	e->back_refs = (int* )xrealloc(e->back_refs, sizeof(int) * alloc);
	CHECK_NULL_RETURN_VAL(e->back_refs, ONIGERR_MEMORY);
	e->back_alloc = alloc;
      }
      e->back_refs[e->back_num - 1] = backref;
    }
  }

  return 0;
}

extern int
onig_name_to_group_numbers(regex_t* reg, const UChar* name,
			   const UChar* name_end, int** nums)
{
  NameEntry* e;

  e = name_find(reg, name, name_end);
  if (IS_NULL(e)) return ONIGERR_UNDEFINED_NAME_REFERENCE;

  switch (e->back_num) {
  case 0:
    break;
  case 1:
    *nums = &(e->back_ref1);
    break;
  default:
    *nums = e->back_refs;
    break;
  }
  return e->back_num;
}

extern int
onig_name_to_backref_number(regex_t* reg, const UChar* name,
			    const UChar* name_end, OnigRegion *region)
{
  int i, n, *nums;

  n = onig_name_to_group_numbers(reg, name, name_end, &nums);
  if (n < 0)
    return n;
  else if (n == 0)
    return ONIGERR_PARSER_BUG;
  else if (n == 1)
    return nums[0];
  else {
    if (IS_NOT_NULL(region)) {
      for (i = n - 1; i >= 0; i--) {
	if (region->beg[nums[i]] != ONIG_REGION_NOTPOS)
	  return nums[i];
      }
    }
    return nums[n - 1];
  }
}

#else /* USE_NAMED_GROUP */

extern int
onig_name_to_group_numbers(regex_t* reg, const UChar* name,
			   const UChar* name_end, int** nums)
{
  return ONIG_NO_SUPPORT_CONFIG;
}

extern int
onig_name_to_backref_number(regex_t* reg, const UChar* name,
			    const UChar* name_end, OnigRegion* region)
{
  return ONIG_NO_SUPPORT_CONFIG;
}

extern int
onig_foreach_name(regex_t* reg,
	   int (*func)(const UChar*, const UChar*,int,int*,regex_t*,void*),
	   void* arg)
{
  return ONIG_NO_SUPPORT_CONFIG;
}

extern int
onig_number_of_names(regex_t* reg)
{
  return 0;
}
#endif /* else USE_NAMED_GROUP */

extern int
onig_noname_group_capture_is_active(regex_t* reg)
{
  if (ONIG_IS_OPTION_ON(reg->options, ONIG_OPTION_DONT_CAPTURE_GROUP))
    return 0;

#ifdef USE_NAMED_GROUP
  if (onig_number_of_names(reg) > 0 &&
      IS_SYNTAX_BV(reg->syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
      !ONIG_IS_OPTION_ON(reg->options, ONIG_OPTION_CAPTURE_GROUP)) {
    return 0;
  }
#endif

  return 1;
}


#define INIT_SCANENV_MEMNODES_ALLOC_SIZE   16

static void
scan_env_clear(ScanEnv* env)
{
  int i;

  BIT_STATUS_CLEAR(env->capture_history);
  BIT_STATUS_CLEAR(env->bt_mem_start);
  BIT_STATUS_CLEAR(env->bt_mem_end);
  BIT_STATUS_CLEAR(env->backrefed_mem);
  env->error             = (UChar* )NULL;
  env->error_end         = (UChar* )NULL;
  env->num_call          = 0;
  env->num_mem           = 0;
#ifdef USE_NAMED_GROUP
  env->num_named         = 0;
#endif
  env->mem_alloc         = 0;
  env->mem_nodes_dynamic = (Node** )NULL;

  for (i = 0; i < SCANENV_MEMNODES_SIZE; i++)
    env->mem_nodes_static[i] = NULL_NODE;

#ifdef USE_COMBINATION_EXPLOSION_CHECK
  env->num_comb_exp_check  = 0;
  env->comb_exp_max_regnum = 0;
  env->curr_max_regnum     = 0;
  env->has_recursion       = 0;
#endif
}

static int
scan_env_add_mem_entry(ScanEnv* env)
{
  int i, need, alloc;
  Node** p;

  need = env->num_mem + 1;
  if (need >= SCANENV_MEMNODES_SIZE) {
    if (env->mem_alloc <= need) {
      if (IS_NULL(env->mem_nodes_dynamic)) {
	alloc = INIT_SCANENV_MEMNODES_ALLOC_SIZE;
	p = (Node** )xmalloc(sizeof(Node*) * alloc);
	xmemcpy(p, env->mem_nodes_static,
		sizeof(Node*) * SCANENV_MEMNODES_SIZE);
      }
      else {
	alloc = env->mem_alloc * 2;
	p = (Node** )xrealloc(env->mem_nodes_dynamic, sizeof(Node*) * alloc);
      }
      CHECK_NULL_RETURN_VAL(p, ONIGERR_MEMORY);

      for (i = env->num_mem + 1; i < alloc; i++)
	p[i] = NULL_NODE;

      env->mem_nodes_dynamic = p;
      env->mem_alloc = alloc;
    }
  }

  env->num_mem++;
  return env->num_mem;
}

static int
scan_env_set_mem_node(ScanEnv* env, int num, Node* node)
{
  if (env->num_mem >= num)
    SCANENV_MEM_NODES(env)[num] = node;
  else
    return ONIGERR_PARSER_BUG;
  return 0;
}


#ifdef USE_RECYCLE_NODE
typedef struct _FreeNode {
  struct _FreeNode* next;
} FreeNode;

static FreeNode* FreeNodeList = (FreeNode* )NULL;
#endif

extern void
onig_node_free(Node* node)
{
 start:
  if (IS_NULL(node)) return ;

  switch (NTYPE(node)) {
  case N_STRING:
    if (IS_NOT_NULL(NSTRING(node).s) && NSTRING(node).s != NSTRING(node).buf) {
      xfree(NSTRING(node).s);
    }
    break;

  case N_LIST:
  case N_ALT:
    onig_node_free(NCONS(node).left);
    /* onig_node_free(NCONS(node).right); */
    {
      Node* next_node = NCONS(node).right;

#ifdef USE_RECYCLE_NODE
      {
	FreeNode* n = (FreeNode* )node;

        THREAD_ATOMIC_START;
	n->next = FreeNodeList;
	FreeNodeList = n;
        THREAD_ATOMIC_END;
      }
#else
      xfree(node);
#endif

      node = next_node;
      goto start;
    }
    break;

  case N_CCLASS:
    {
      CClassNode* cc = &(NCCLASS(node));

      if (IS_CCLASS_SHARE(cc))
        return ;

      if (cc->mbuf)
        bbuf_free(cc->mbuf);
    }
    break;

  case N_QUANTIFIER:
    if (NQUANTIFIER(node).target)
      onig_node_free(NQUANTIFIER(node).target);
    break;

  case N_EFFECT:
    if (NEFFECT(node).target)
      onig_node_free(NEFFECT(node).target);
    break;

  case N_BACKREF:
    if (IS_NOT_NULL(NBACKREF(node).back_dynamic))
      xfree(NBACKREF(node).back_dynamic);
    break;

  case N_ANCHOR:
    if (NANCHOR(node).target)
      onig_node_free(NANCHOR(node).target);
    break;
  }

#ifdef USE_RECYCLE_NODE
  {
    FreeNode* n = (FreeNode* )node;

    THREAD_ATOMIC_START;
    n->next = FreeNodeList;
    FreeNodeList = n;
    THREAD_ATOMIC_END;
  }
#else
  xfree(node);
#endif
}

#ifdef USE_RECYCLE_NODE
extern int
onig_free_node_list(void)
{
  FreeNode* n;

  /* THREAD_ATOMIC_START; */
  while (IS_NOT_NULL(FreeNodeList)) {
    n = FreeNodeList;
    FreeNodeList = FreeNodeList->next;
    xfree(n);
  }
  /* THREAD_ATOMIC_END; */
  return 0;
}
#endif

static Node*
node_new(void)
{
  Node* node;

#ifdef USE_RECYCLE_NODE
  THREAD_ATOMIC_START;
  if (IS_NOT_NULL(FreeNodeList)) {
    node = (Node* )FreeNodeList;
    FreeNodeList = FreeNodeList->next;
    THREAD_ATOMIC_END;
    return node;
  }
  THREAD_ATOMIC_END;
#endif

  node = (Node* )xmalloc(sizeof(Node));
  return node;
}


static void
initialize_cclass(CClassNode* cc)
{
  BITSET_CLEAR(cc->bs);
  cc->flags = 0;
  cc->mbuf  = NULL;
}

static Node*
node_new_cclass(void)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_CCLASS;

  initialize_cclass(&(NCCLASS(node)));
  return node;
}

static Node*
node_new_cclass_by_codepoint_range(int not,
                   const OnigCodePoint sbr[], const OnigCodePoint mbr[])
{
  CClassNode* cc;
  int n, i, j;

  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_CCLASS;

  cc = &(NCCLASS(node));
  cc->flags = 0;
  if (not != 0) CCLASS_SET_NOT(cc);

  BITSET_CLEAR(cc->bs);
  if (IS_NOT_NULL(sbr)) {
    n = ONIGENC_CODE_RANGE_NUM(sbr);
    for (i = 0; i < n; i++) {
      for (j  = ONIGENC_CODE_RANGE_FROM(sbr, i);
           j <= (int )ONIGENC_CODE_RANGE_TO(sbr, i); j++) {
        BITSET_SET_BIT(cc->bs, j);
      }
    }
  }

  if (IS_NULL(mbr)) {
  is_null:
    cc->mbuf = NULL;
  }
  else {
    BBuf* bbuf;

    n = ONIGENC_CODE_RANGE_NUM(mbr);
    if (n == 0) goto is_null;

    bbuf = (BBuf* )xmalloc(sizeof(BBuf));
    CHECK_NULL_RETURN_VAL(bbuf, NULL);
    bbuf->alloc = n + 1;
    bbuf->used  = n + 1;
    bbuf->p     = (UChar* )((void* )mbr);

    cc->mbuf = bbuf;
  }

  return node;
}

static Node*
node_new_ctype(int type)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_CTYPE;
  NCTYPE(node).type = type;
  return node;
}

static Node*
node_new_anychar(void)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_ANYCHAR;
  return node;
}

static Node*
node_new_list(Node* left, Node* right)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_LIST;
  NCONS(node).left  = left;
  NCONS(node).right = right;
  return node;
}

extern Node*
onig_node_new_list(Node* left, Node* right)
{
  return node_new_list(left, right);
}

static Node*
node_new_alt(Node* left, Node* right)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_ALT;
  NCONS(node).left  = left;
  NCONS(node).right = right;
  return node;
}

extern Node*
onig_node_new_anchor(int type)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_ANCHOR;
  NANCHOR(node).type     = type;
  NANCHOR(node).target   = NULL;
  NANCHOR(node).char_len = -1;
  return node;
}

static Node*
node_new_backref(int back_num, int* backrefs, int by_name,
#ifdef USE_BACKREF_AT_LEVEL
		 int exist_level, int nest_level,
#endif
		 ScanEnv* env)
{
  int i;
  Node* node = node_new();

  CHECK_NULL_RETURN(node);
  node->type = N_BACKREF;
  NBACKREF(node).state    = 0;
  NBACKREF(node).back_num = back_num;
  NBACKREF(node).back_dynamic = (int* )NULL;
  if (by_name != 0)
    NBACKREF(node).state |= NST_NAME_REF;

#ifdef USE_BACKREF_AT_LEVEL
  if (exist_level != 0) {
    NBACKREF(node).state |= NST_NEST_LEVEL;
    NBACKREF(node).nest_level  = nest_level;
  }
#endif

  for (i = 0; i < back_num; i++) {
    if (backrefs[i] <= env->num_mem &&
	IS_NULL(SCANENV_MEM_NODES(env)[backrefs[i]])) {
      NBACKREF(node).state |= NST_RECURSION;   /* /...(\1).../ */
      break;
    }
  }

  if (back_num <= NODE_BACKREFS_SIZE) {
    for (i = 0; i < back_num; i++)
      NBACKREF(node).back_static[i] = backrefs[i];
  }
  else {
    int* p = (int* )xmalloc(sizeof(int) * back_num);
    if (IS_NULL(p)) {
      onig_node_free(node);
      return NULL;
    }
    NBACKREF(node).back_dynamic = p;
    for (i = 0; i < back_num; i++)
      p[i] = backrefs[i];
  }
  return node;
}

#ifdef USE_SUBEXP_CALL
static Node*
node_new_call(UChar* name, UChar* name_end)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  node->type = N_CALL;
  NCALL(node).state    = 0;
  NCALL(node).ref_num  = CALLNODE_REFNUM_UNDEF;
  NCALL(node).target   = NULL_NODE;
  NCALL(node).name     = name;
  NCALL(node).name_end = name_end;
  return node;
}
#endif

static Node*
node_new_quantifier(int lower, int upper, int by_number)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_QUANTIFIER;
  NQUANTIFIER(node).state  = 0;
  NQUANTIFIER(node).target = NULL;
  NQUANTIFIER(node).lower  = lower;
  NQUANTIFIER(node).upper  = upper;
  NQUANTIFIER(node).greedy = 1;
  NQUANTIFIER(node).target_empty_info = NQ_TARGET_ISNOT_EMPTY;
  NQUANTIFIER(node).head_exact        = NULL_NODE;
  NQUANTIFIER(node).next_head_exact   = NULL_NODE;
  NQUANTIFIER(node).is_refered        = 0;
  if (by_number != 0)
    NQUANTIFIER(node).state |= NST_BY_NUMBER;

#ifdef USE_COMBINATION_EXPLOSION_CHECK
  NQUANTIFIER(node).comb_exp_check_num = 0;
#endif

  return node;
}

static Node*
node_new_effect(int type)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_EFFECT;
  NEFFECT(node).type      = type;
  NEFFECT(node).state     =  0;
  NEFFECT(node).regnum    =  0;
  NEFFECT(node).option    =  0;
  NEFFECT(node).target    = NULL;
  NEFFECT(node).call_addr = -1;
  NEFFECT(node).opt_count =  0;
  return node;
}

extern Node*
onig_node_new_effect(int type)
{
  return node_new_effect(type);
}

static Node*
node_new_effect_memory(OnigOptionType option, int is_named)
{
  Node* node = node_new_effect(EFFECT_MEMORY);
  CHECK_NULL_RETURN(node);
  if (is_named != 0)
    SET_EFFECT_STATUS(node, NST_NAMED_GROUP);

#ifdef USE_SUBEXP_CALL
  NEFFECT(node).option = option;
#endif
  return node;
}

static Node*
node_new_option(OnigOptionType option)
{
  Node* node = node_new_effect(EFFECT_OPTION);
  CHECK_NULL_RETURN(node);
  NEFFECT(node).option = option;
  return node;
}

extern int
onig_node_str_cat(Node* node, const UChar* s, const UChar* end)
{
  int addlen = end - s;

  if (addlen > 0) {
    int len  = NSTRING(node).end - NSTRING(node).s;

    if (NSTRING(node).capa > 0 || (len + addlen > NODE_STR_BUF_SIZE - 1)) {
      UChar* p;
      int capa = len + addlen + NODE_STR_MARGIN;

      if (capa <= NSTRING(node).capa) {
	k_strcpy(NSTRING(node).s + len, s, end);
      }
      else {
	if (NSTRING(node).s == NSTRING(node).buf)
	  p = strcat_capa_from_static(NSTRING(node).s, NSTRING(node).end,
				      s, end, capa);
	else
	  p = k_strcat_capa(NSTRING(node).s, NSTRING(node).end, s, end, capa);

	CHECK_NULL_RETURN_VAL(p, ONIGERR_MEMORY);
	NSTRING(node).s    = p;
	NSTRING(node).capa = capa;
      }
    }
    else {
      k_strcpy(NSTRING(node).s + len, s, end);
    }
    NSTRING(node).end = NSTRING(node).s + len + addlen;
  }

  return 0;
}

static int
node_str_cat_char(Node* node, UChar c)
{
  UChar s[1];

  s[0] = c;
  return onig_node_str_cat(node, s, s + 1);
}

extern void
onig_node_conv_to_str_node(Node* node, int flag)
{
  node->type = N_STRING;

  NSTRING(node).flag = flag;
  NSTRING(node).capa = 0;
  NSTRING(node).s    = NSTRING(node).buf;
  NSTRING(node).end  = NSTRING(node).buf;
}

extern void
onig_node_str_clear(Node* node)
{
  if (NSTRING(node).capa != 0 &&
      IS_NOT_NULL(NSTRING(node).s) && NSTRING(node).s != NSTRING(node).buf) {
    xfree(NSTRING(node).s);
  }

  NSTRING(node).capa = 0;
  NSTRING(node).flag = 0;
  NSTRING(node).s    = NSTRING(node).buf;
  NSTRING(node).end  = NSTRING(node).buf;
}

static Node*
node_new_str(const UChar* s, const UChar* end)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  node->type = N_STRING;
  NSTRING(node).capa = 0;
  NSTRING(node).flag = 0;
  NSTRING(node).s    = NSTRING(node).buf;
  NSTRING(node).end  = NSTRING(node).buf;
  if (onig_node_str_cat(node, s, end)) {
    onig_node_free(node);
    return NULL;
  }
  return node;
}

extern Node*
onig_node_new_str(const UChar* s, const UChar* end)
{
  return node_new_str(s, end);
}

#ifdef NUMBERED_CHAR_IS_NOT_CASE_AMBIG
static Node*
node_new_str_raw(UChar* s, UChar* end)
{
  Node* node = node_new_str(s, end);
  NSTRING_SET_RAW(node);
  return node;
}
#endif

static Node*
node_new_empty(void)
{
  return node_new_str(NULL, NULL);
}

static Node*
node_new_str_char(UChar c)
{
  UChar p[1];

  p[0] = c;
  return node_new_str(p, p + 1);
}

static Node*
str_node_split_last_char(StrNode* sn, OnigEncoding enc)
{
  const UChar *p;
  Node* n = NULL_NODE;

  if (sn->end > sn->s) {
    p = onigenc_get_prev_char_head(enc, sn->s, sn->end);
    if (p && p > sn->s) { /* can be splitted. */
      n = node_new_str(p, sn->end);
      if ((sn->flag & NSTR_RAW) != 0)
	NSTRING_SET_RAW(n);
      sn->end = (UChar* )p;
    }
  }
  return n;
}

static int
str_node_can_be_split(StrNode* sn, OnigEncoding enc)
{
  if (sn->end > sn->s) {
    return ((enc_len(enc, sn->s) < sn->end - sn->s)  ?  1 : 0);
  }
  return 0;
}

#ifdef USE_PAD_TO_SHORT_BYTE_CHAR
static int
node_str_head_pad(StrNode* sn, int num, UChar val)
{
  UChar buf[NODE_STR_BUF_SIZE];
  int i, len;

  len = sn->end - sn->s;
  onig_strcpy(buf, sn->s, sn->end);
  onig_strcpy(&(sn->s[num]), buf, buf + len);
  sn->end += num;

  for (i = 0; i < num; i++) {
    sn->s[i] = val;
  }
}
#endif

extern int
onig_scan_unsigned_number(UChar** src, const UChar* end, OnigEncoding enc)
{
  unsigned int num, val;
  OnigCodePoint c;
  UChar* p = *src;
  PFETCH_READY;

  num = 0;
  while (!PEND) {
    PFETCH(c);
    if (ONIGENC_IS_CODE_DIGIT(enc, c)) {
      val = (unsigned int )DIGITVAL(c);
      if ((INT_MAX_LIMIT - val) / 10UL < num)
	return -1;  /* overflow */

      num = num * 10 + val;
    }
    else {
      PUNFETCH;
      break;
    }
  }
  *src = p;
  return num;
}

static int
scan_unsigned_hexadecimal_number(UChar** src, UChar* end, int maxlen,
				 OnigEncoding enc)
{
  OnigCodePoint c;
  unsigned int num, val;
  UChar* p = *src;
  PFETCH_READY;

  num = 0;
  while (!PEND && maxlen-- != 0) {
    PFETCH(c);
    if (ONIGENC_IS_CODE_XDIGIT(enc, c)) {
      val = (unsigned int )XDIGITVAL(enc,c);
      if ((INT_MAX_LIMIT - val) / 16UL < num)
	return -1;  /* overflow */

      num = (num << 4) + XDIGITVAL(enc,c);
    }
    else {
      PUNFETCH;
      break;
    }
  }
  *src = p;
  return num;
}

static int
scan_unsigned_octal_number(UChar** src, UChar* end, int maxlen,
			   OnigEncoding enc)
{
  OnigCodePoint c;
  unsigned int num, val;
  UChar* p = *src;
  PFETCH_READY;

  num = 0;
  while (!PEND && maxlen-- != 0) {
    PFETCH(c);
    if (ONIGENC_IS_CODE_DIGIT(enc, c) && c < '8') {
      val = ODIGITVAL(c);
      if ((INT_MAX_LIMIT - val) / 8UL < num)
	return -1;  /* overflow */

      num = (num << 3) + val;
    }
    else {
      PUNFETCH;
      break;
    }
  }
  *src = p;
  return num;
}


#define BBUF_WRITE_CODE_POINT(bbuf,pos,code) \
    BBUF_WRITE(bbuf, pos, &(code), SIZE_CODE_POINT)

/* data format:
     [n][from-1][to-1][from-2][to-2] ... [from-n][to-n]
     (all data size is OnigCodePoint)
 */
static int
new_code_range(BBuf** pbuf)
{
#define INIT_MULTI_BYTE_RANGE_SIZE  (SIZE_CODE_POINT * 5)
  int r;
  OnigCodePoint n;
  BBuf* bbuf;

  bbuf = *pbuf = (BBuf* )xmalloc(sizeof(BBuf));
  CHECK_NULL_RETURN_VAL(*pbuf, ONIGERR_MEMORY);
  r = BBUF_INIT(*pbuf, INIT_MULTI_BYTE_RANGE_SIZE);
  if (r) return r;

  n = 0;
  BBUF_WRITE_CODE_POINT(bbuf, 0, n);
  return 0;
}

static int
add_code_range_to_buf(BBuf** pbuf, OnigCodePoint from, OnigCodePoint to)
{
  int r, inc_n, pos;
  int low, high, bound, x;
  OnigCodePoint n, *data;
  BBuf* bbuf;

  if (from > to) {
    n = from; from = to; to = n;
  }

  if (IS_NULL(*pbuf)) {
    r = new_code_range(pbuf);
    if (r) return r;
    bbuf = *pbuf;
    n = 0;
  }
  else {
    bbuf = *pbuf;
    GET_CODE_POINT(n, bbuf->p);
  }
  data = (OnigCodePoint* )(bbuf->p);
  data++;

  for (low = 0, bound = n; low < bound; ) {
    x = (low + bound) >> 1;
    if (from > data[x*2 + 1])
      low = x + 1;
    else
      bound = x;
  }

  for (high = low, bound = n; high < bound; ) {
    x = (high + bound) >> 1;
    if (to >= data[x*2] - 1)
      high = x + 1;
    else
      bound = x;
  }

  inc_n = low + 1 - high;
  if (n + inc_n > ONIG_MAX_MULTI_BYTE_RANGES_NUM)
    return ONIGERR_TOO_MANY_MULTI_BYTE_RANGES;

  if (inc_n != 1) {
    if (from > data[low*2])
      from = data[low*2];
    if (to < data[(high - 1)*2 + 1])
      to = data[(high - 1)*2 + 1];
  }

  if (inc_n != 0 && (OnigCodePoint )high < n) {
    int from_pos = SIZE_CODE_POINT * (1 + high * 2);
    int to_pos   = SIZE_CODE_POINT * (1 + (low + 1) * 2);
    int size = (n - high) * 2 * SIZE_CODE_POINT;

    if (inc_n > 0) {
      BBUF_MOVE_RIGHT(bbuf, from_pos, to_pos, size);
    }
    else {
      BBUF_MOVE_LEFT_REDUCE(bbuf, from_pos, to_pos);
    }
  }

  pos = SIZE_CODE_POINT * (1 + low * 2);
  BBUF_ENSURE_SIZE(bbuf, pos + SIZE_CODE_POINT * 2);
  BBUF_WRITE_CODE_POINT(bbuf, pos, from);
  BBUF_WRITE_CODE_POINT(bbuf, pos + SIZE_CODE_POINT, to);
  n += inc_n;
  BBUF_WRITE_CODE_POINT(bbuf, 0, n);

  return 0;
}

static int
add_code_range(BBuf** pbuf, ScanEnv* env, OnigCodePoint from, OnigCodePoint to)
{
  if (from > to) {
    if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
      return 0;
    else
      return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
  }

  return add_code_range_to_buf(pbuf, from, to);
}

static int
not_code_range_buf(OnigEncoding enc, BBuf* bbuf, BBuf** pbuf)
{
  int r, i, n;
  OnigCodePoint pre, from, *data, to = 0;

  *pbuf = (BBuf* )NULL;
  if (IS_NULL(bbuf)) {
  set_all:
    return SET_ALL_MULTI_BYTE_RANGE(enc, pbuf);
  }

  data = (OnigCodePoint* )(bbuf->p);
  GET_CODE_POINT(n, data);
  data++;
  if (n <= 0) goto set_all;

  r = 0;
  pre = MBCODE_START_POS(enc);
  for (i = 0; i < n; i++) {
    from = data[i*2];
    to   = data[i*2+1];
    if (pre <= from - 1) {
      r = add_code_range_to_buf(pbuf, pre, from - 1);
      if (r != 0) return r;
    }
    if (to == ~((OnigCodePoint )0)) break;
    pre = to + 1;
  }
  if (to < ~((OnigCodePoint )0)) {
    r = add_code_range_to_buf(pbuf, to + 1, ~((OnigCodePoint )0));
  }
  return r;
}

#define SWAP_BBUF_NOT(bbuf1, not1, bbuf2, not2) do {\
  BBuf *tbuf; \
  int  tnot; \
  tnot = not1;  not1  = not2;  not2  = tnot; \
  tbuf = bbuf1; bbuf1 = bbuf2; bbuf2 = tbuf; \
} while (0)

static int
or_code_range_buf(OnigEncoding enc, BBuf* bbuf1, int not1,
                  BBuf* bbuf2, int not2, BBuf** pbuf)
{
  int r;
  OnigCodePoint i, n1, *data1;
  OnigCodePoint from, to;

  *pbuf = (BBuf* )NULL;
  if (IS_NULL(bbuf1) && IS_NULL(bbuf2)) {
    if (not1 != 0 || not2 != 0)
      return SET_ALL_MULTI_BYTE_RANGE(enc, pbuf);
    return 0;
  }

  r = 0;
  if (IS_NULL(bbuf2))
    SWAP_BBUF_NOT(bbuf1, not1, bbuf2, not2);

  if (IS_NULL(bbuf1)) {
    if (not1 != 0) {
      return SET_ALL_MULTI_BYTE_RANGE(enc, pbuf);
    }
    else {
      if (not2 == 0) {
	return bbuf_clone(pbuf, bbuf2);
      }
      else {
	return not_code_range_buf(enc, bbuf2, pbuf);
      }
    }
  }

  if (not1 != 0)
    SWAP_BBUF_NOT(bbuf1, not1, bbuf2, not2);

  data1 = (OnigCodePoint* )(bbuf1->p);
  GET_CODE_POINT(n1, data1);
  data1++;

  if (not2 == 0 && not1 == 0) { /* 1 OR 2 */
    r = bbuf_clone(pbuf, bbuf2);
  }
  else if (not1 == 0) { /* 1 OR (not 2) */
    r = not_code_range_buf(enc, bbuf2, pbuf);
  }
  if (r != 0) return r;

  for (i = 0; i < n1; i++) {
    from = data1[i*2];
    to   = data1[i*2+1];
    r = add_code_range_to_buf(pbuf, from, to);
    if (r != 0) return r;
  }
  return 0;
}

static int
and_code_range1(BBuf** pbuf, OnigCodePoint from1, OnigCodePoint to1,
	        OnigCodePoint* data, int n)
{
  int i, r;
  OnigCodePoint from2, to2;

  for (i = 0; i < n; i++) {
    from2 = data[i*2];
    to2   = data[i*2+1];
    if (from2 < from1) {
      if (to2 < from1) continue;
      else {
	from1 = to2 + 1;
      }
    }
    else if (from2 <= to1) {
      if (to2 < to1) {
	if (from1 <= from2 - 1) {
	  r = add_code_range_to_buf(pbuf, from1, from2-1);
	  if (r != 0) return r;
	}
	from1 = to2 + 1;
      }
      else {
	to1 = from2 - 1;
      }
    }
    else {
      from1 = from2;
    }
    if (from1 > to1) break;
  }
  if (from1 <= to1) {
    r = add_code_range_to_buf(pbuf, from1, to1);
    if (r != 0) return r;
  }
  return 0;
}

static int
and_code_range_buf(BBuf* bbuf1, int not1, BBuf* bbuf2, int not2, BBuf** pbuf)
{
  int r;
  OnigCodePoint i, j, n1, n2, *data1, *data2;
  OnigCodePoint from, to, from1, to1, from2, to2;

  *pbuf = (BBuf* )NULL;
  if (IS_NULL(bbuf1)) {
    if (not1 != 0 && IS_NOT_NULL(bbuf2)) /* not1 != 0 -> not2 == 0 */
      return bbuf_clone(pbuf, bbuf2);
    return 0;
  }
  else if (IS_NULL(bbuf2)) {
    if (not2 != 0)
      return bbuf_clone(pbuf, bbuf1);
    return 0;
  }

  if (not1 != 0)
    SWAP_BBUF_NOT(bbuf1, not1, bbuf2, not2);

  data1 = (OnigCodePoint* )(bbuf1->p);
  data2 = (OnigCodePoint* )(bbuf2->p);
  GET_CODE_POINT(n1, data1);
  GET_CODE_POINT(n2, data2);
  data1++;
  data2++;

  if (not2 == 0 && not1 == 0) { /* 1 AND 2 */
    for (i = 0; i < n1; i++) {
      from1 = data1[i*2];
      to1   = data1[i*2+1];
      for (j = 0; j < n2; j++) {
	from2 = data2[j*2];
	to2   = data2[j*2+1];
	if (from2 > to1) break;
	if (to2 < from1) continue;
	from = MAX(from1, from2);
	to   = MIN(to1, to2);
	r = add_code_range_to_buf(pbuf, from, to);
	if (r != 0) return r;
      }
    }
  }
  else if (not1 == 0) { /* 1 AND (not 2) */
    for (i = 0; i < n1; i++) {
      from1 = data1[i*2];
      to1   = data1[i*2+1];
      r = and_code_range1(pbuf, from1, to1, data2, n2);
      if (r != 0) return r;
    }
  }

  return 0;
}

static int
and_cclass(CClassNode* dest, CClassNode* cc, OnigEncoding enc)
{
  int r, not1, not2;
  BBuf *buf1, *buf2, *pbuf;
  BitSetRef bsr1, bsr2;
  BitSet bs1, bs2;

  not1 = IS_CCLASS_NOT(dest);
  bsr1 = dest->bs;
  buf1 = dest->mbuf;
  not2 = IS_CCLASS_NOT(cc);
  bsr2 = cc->bs;
  buf2 = cc->mbuf;

  if (not1 != 0) {
    bitset_invert_to(bsr1, bs1);
    bsr1 = bs1;
  }
  if (not2 != 0) {
    bitset_invert_to(bsr2, bs2);
    bsr2 = bs2;
  }
  bitset_and(bsr1, bsr2);
  if (bsr1 != dest->bs) {
    bitset_copy(dest->bs, bsr1);
    bsr1 = dest->bs;
  }
  if (not1 != 0) {
    bitset_invert(dest->bs);
  }

  if (! ONIGENC_IS_SINGLEBYTE(enc)) {
    if (not1 != 0 && not2 != 0) {
      r = or_code_range_buf(enc, buf1, 0, buf2, 0, &pbuf);
    }
    else {
      r = and_code_range_buf(buf1, not1, buf2, not2, &pbuf);
      if (r == 0 && not1 != 0) {
	BBuf *tbuf;
	r = not_code_range_buf(enc, pbuf, &tbuf);
	if (r != 0) {
	  bbuf_free(pbuf);
	  return r;
	}
	bbuf_free(pbuf);
	pbuf = tbuf;
      }
    }
    if (r != 0) return r;

    dest->mbuf = pbuf;
    bbuf_free(buf1);
    return r;
  }
  return 0;
}

static int
or_cclass(CClassNode* dest, CClassNode* cc, OnigEncoding enc)
{
  int r, not1, not2;
  BBuf *buf1, *buf2, *pbuf;
  BitSetRef bsr1, bsr2;
  BitSet bs1, bs2;

  not1 = IS_CCLASS_NOT(dest);
  bsr1 = dest->bs;
  buf1 = dest->mbuf;
  not2 = IS_CCLASS_NOT(cc);
  bsr2 = cc->bs;
  buf2 = cc->mbuf;

  if (not1 != 0) {
    bitset_invert_to(bsr1, bs1);
    bsr1 = bs1;
  }
  if (not2 != 0) {
    bitset_invert_to(bsr2, bs2);
    bsr2 = bs2;
  }
  bitset_or(bsr1, bsr2);
  if (bsr1 != dest->bs) {
    bitset_copy(dest->bs, bsr1);
    bsr1 = dest->bs;
  }
  if (not1 != 0) {
    bitset_invert(dest->bs);
  }

  if (! ONIGENC_IS_SINGLEBYTE(enc)) {
    if (not1 != 0 && not2 != 0) {
      r = and_code_range_buf(buf1, 0, buf2, 0, &pbuf);
    }
    else {
      r = or_code_range_buf(enc, buf1, not1, buf2, not2, &pbuf);
      if (r == 0 && not1 != 0) {
	BBuf *tbuf;
	r = not_code_range_buf(enc, pbuf, &tbuf);
	if (r != 0) {
	  bbuf_free(pbuf);
	  return r;
	}
	bbuf_free(pbuf);
	pbuf = tbuf;
      }
    }
    if (r != 0) return r;

    dest->mbuf = pbuf;
    bbuf_free(buf1);
    return r;
  }
  else
    return 0;
}

static int
conv_backslash_value(int c, ScanEnv* env)
{
  if (IS_SYNTAX_OP(env->syntax, ONIG_SYN_OP_ESC_CONTROL_CHARS)) {
    switch (c) {
    case 'n':  return '\n';
    case 't':  return '\t';
    case 'r':  return '\r';
    case 'f':  return '\f';
    case 'a':  return '\007';
    case 'b':  return '\010';
    case 'e':  return '\033';
    case 'v':
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ESC_V_VTAB))
	return '\v';
      break;

    default:
      break;
    }
  }
  return c;
}

static int
is_invalid_quantifier_target(Node* node)
{
  switch (NTYPE(node)) {
  case N_ANCHOR:
    return 1;
    break;

  case N_EFFECT:
    if (NEFFECT(node).type == EFFECT_OPTION)
      return is_invalid_quantifier_target(NEFFECT(node).target);
    break;

  case N_LIST: /* ex. (?:\G\A)* */
    do {
      if (! is_invalid_quantifier_target(NCONS(node).left)) return 0;
    } while (IS_NOT_NULL(node = NCONS(node).right));
    return 0;
    break;

  case N_ALT:  /* ex. (?:abc|\A)* */
    do {
      if (is_invalid_quantifier_target(NCONS(node).left)) return 1;
    } while (IS_NOT_NULL(node = NCONS(node).right));
    break;

  default:
    break;
  }
  return 0;
}

/* ?:0, *:1, +:2, ??:3, *?:4, +?:5 */
static int
popular_quantifier_num(QuantifierNode* qf)
{
  if (qf->greedy) {
    if (qf->lower == 0) {
      if (qf->upper == 1) return 0;
      else if (IS_REPEAT_INFINITE(qf->upper)) return 1;
    }
    else if (qf->lower == 1) {
      if (IS_REPEAT_INFINITE(qf->upper)) return 2;
    }
  }
  else {
    if (qf->lower == 0) {
      if (qf->upper == 1) return 3;
      else if (IS_REPEAT_INFINITE(qf->upper)) return 4;
    }
    else if (qf->lower == 1) {
      if (IS_REPEAT_INFINITE(qf->upper)) return 5;
    }
  }
  return -1;
}


enum ReduceType {
  RQ_ASIS = 0, /* as is */
  RQ_DEL  = 1, /* delete parent */
  RQ_A,        /* to '*'    */
  RQ_AQ,       /* to '*?'   */
  RQ_QQ,       /* to '??'   */
  RQ_P_QQ,     /* to '+)??' */
  RQ_PQ_Q      /* to '+?)?' */
};

static enum ReduceType ReduceTypeTable[6][6] = {
  {RQ_DEL,  RQ_A,    RQ_A,   RQ_QQ,   RQ_AQ,   RQ_ASIS}, /* '?'  */
  {RQ_DEL,  RQ_DEL,  RQ_DEL, RQ_P_QQ, RQ_P_QQ, RQ_DEL},  /* '*'  */
  {RQ_A,    RQ_A,    RQ_DEL, RQ_ASIS, RQ_P_QQ, RQ_DEL},  /* '+'  */
  {RQ_DEL,  RQ_AQ,   RQ_AQ,  RQ_DEL,  RQ_AQ,   RQ_AQ},   /* '??' */
  {RQ_DEL,  RQ_DEL,  RQ_DEL, RQ_DEL,  RQ_DEL,  RQ_DEL},  /* '*?' */
  {RQ_ASIS, RQ_PQ_Q, RQ_DEL, RQ_AQ,   RQ_AQ,   RQ_DEL}   /* '+?' */
};

extern void
onig_reduce_nested_quantifier(Node* pnode, Node* cnode)
{
  int pnum, cnum;
  QuantifierNode *p, *c;

  p = &(NQUANTIFIER(pnode));
  c = &(NQUANTIFIER(cnode));
  pnum = popular_quantifier_num(p);
  cnum = popular_quantifier_num(c);

  switch(ReduceTypeTable[cnum][pnum]) {
  case RQ_DEL:
    *p = *c;
    break;
  case RQ_A:
    p->target = c->target;
    p->lower  = 0;  p->upper = REPEAT_INFINITE;  p->greedy = 1;
    break;
  case RQ_AQ:
    p->target = c->target;
    p->lower  = 0;  p->upper = REPEAT_INFINITE;  p->greedy = 0;
    break;
  case RQ_QQ:
    p->target = c->target;
    p->lower  = 0;  p->upper = 1;  p->greedy = 0;
    break;
  case RQ_P_QQ:
    p->target = cnode;
    p->lower  = 0;  p->upper = 1;  p->greedy = 0;
    c->lower  = 1;  c->upper = REPEAT_INFINITE;  c->greedy = 1;
    return ;
    break;
  case RQ_PQ_Q:
    p->target = cnode;
    p->lower  = 0;  p->upper = 1;  p->greedy = 1;
    c->lower  = 1;  c->upper = REPEAT_INFINITE;  c->greedy = 0;
    return ;
    break;
  case RQ_ASIS:
    p->target = cnode;
    return ;
    break;
  }

  c->target = NULL_NODE;
  onig_node_free(cnode);
}


enum TokenSyms {
  TK_EOT      = 0,   /* end of token */
  TK_RAW_BYTE = 1,
  TK_CHAR,
  TK_STRING,
  TK_CODE_POINT,
  TK_ANYCHAR,
  TK_CHAR_TYPE,
  TK_BACKREF,
  TK_CALL,
  TK_ANCHOR,
  TK_OP_REPEAT,
  TK_INTERVAL,
  TK_ANYCHAR_ANYTIME,  /* SQL '%' == .* */
  TK_ALT,
  TK_SUBEXP_OPEN,
  TK_SUBEXP_CLOSE,
  TK_CC_OPEN,
  TK_QUOTE_OPEN,
  TK_CHAR_PROPERTY,    /* \p{...}, \P{...} */
  /* in cc */
  TK_CC_CLOSE,
  TK_CC_RANGE,
  TK_POSIX_BRACKET_OPEN,
  TK_CC_AND,             /* && */
  TK_CC_CC_OPEN          /* [ */
};

typedef struct {
  enum TokenSyms type;
  int escaped;
  int base;   /* is number: 8, 16 (used in [....]) */
  UChar* backp;
  union {
    UChar* s;
    int   c;
    OnigCodePoint code;
    int   anchor;
    int   subtype;
    struct {
      int lower;
      int upper;
      int greedy;
      int possessive;
    } repeat;
    struct {
      int  num;
      int  ref1;
      int* refs;
      int  by_name;
#ifdef USE_BACKREF_AT_LEVEL
      int  exist_level;
      int  level;   /* \k<name+n> */
#endif
    } backref;
    struct {
      UChar* name;
      UChar* name_end;
    } call;
    struct {
      int not;
    } prop;
  } u;
} OnigToken;


static int
fetch_range_quantifier(UChar** src, UChar* end, OnigToken* tok, ScanEnv* env)
{
  int low, up, syn_allow, non_low = 0;
  int r = 0;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar* p = *src;
  PFETCH_READY;

  syn_allow = IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_INVALID_INTERVAL);

  if (PEND) {
    if (syn_allow)
      return 1;  /* "....{" : OK! */
    else
      return ONIGERR_END_PATTERN_AT_LEFT_BRACE;  /* "....{" syntax error */
  }

  if (! syn_allow) {
    c = PPEEK;
    if (c == ')' || c == '(' || c == '|') {
      return ONIGERR_END_PATTERN_AT_LEFT_BRACE;
    }
  }

  low = onig_scan_unsigned_number(&p, end, env->enc);
  if (low < 0) return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;
  if (low > ONIG_MAX_REPEAT_NUM)
    return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;

  if (p == *src) { /* can't read low */
    if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_INTERVAL_LOW_ABBREV)) {
      /* allow {,n} as {0,n} */
      low = 0;
      non_low = 1;
    }
    else
      goto invalid;
  }

  if (PEND) goto invalid;
  PFETCH(c);
  if (c == ',') {
    UChar* prev = p;
    up = onig_scan_unsigned_number(&p, end, env->enc);
    if (up < 0) return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;
    if (up > ONIG_MAX_REPEAT_NUM)
      return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;

    if (p == prev) {
      if (non_low != 0)
	goto invalid;
      up = REPEAT_INFINITE;  /* {n,} : {n,infinite} */
    }
  }
  else {
    if (non_low != 0)
      goto invalid;

    PUNFETCH;
    up = low;  /* {n} : exact n times */
    r = 2;     /* fixed */
  }

  if (PEND) goto invalid;
  PFETCH(c);
  if (IS_SYNTAX_OP(env->syntax, ONIG_SYN_OP_ESC_BRACE_INTERVAL)) {
    if (c != MC_ESC(enc)) goto invalid;
    PFETCH(c);
  }
  if (c != '}') goto invalid;

  if (!IS_REPEAT_INFINITE(up) && low > up) {
    return ONIGERR_UPPER_SMALLER_THAN_LOWER_IN_REPEAT_RANGE;
  }

  tok->type = TK_INTERVAL;
  tok->u.repeat.lower = low;
  tok->u.repeat.upper = up;
  *src = p;
  return r; /* 0: normal {n,m}, 2: fixed {n} */

 invalid:
  if (syn_allow)
    return 1;  /* OK */
  else
    return ONIGERR_INVALID_REPEAT_RANGE_PATTERN;
}

/* \M-, \C-, \c, or \... */
static int
fetch_escaped_value(UChar** src, UChar* end, ScanEnv* env)
{
  int v;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar* p = *src;
  PFETCH_READY;

  if (PEND) return ONIGERR_END_PATTERN_AT_ESCAPE;

  PFETCH(c);
  switch (c) {
  case 'M':
    if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ESC_CAPITAL_M_BAR_META)) {
      if (PEND) return ONIGERR_END_PATTERN_AT_META;
      PFETCH(c);
      if (c != '-') return ONIGERR_META_CODE_SYNTAX;
      if (PEND) return ONIGERR_END_PATTERN_AT_META;
      PFETCH(c);
      if (c == MC_ESC(enc)) {
	v = fetch_escaped_value(&p, end, env);
	if (v < 0) return v;
        c = (OnigCodePoint )v;
      }
      c = ((c & 0xff) | 0x80);
    }
    else
      goto backslash;
    break;

  case 'C':
    if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL)) {
      if (PEND) return ONIGERR_END_PATTERN_AT_CONTROL;
      PFETCH(c);
      if (c != '-') return ONIGERR_CONTROL_CODE_SYNTAX;
      goto control;
    }
    else
      goto backslash;

  case 'c':
    if (IS_SYNTAX_OP(env->syntax, ONIG_SYN_OP_ESC_C_CONTROL)) {
    control:
      if (PEND) return ONIGERR_END_PATTERN_AT_CONTROL;
      PFETCH(c);
      if (c == '?') {
	c = 0177;
      }
      else {
        if (c == MC_ESC(enc)) {
          v = fetch_escaped_value(&p, end, env);
          if (v < 0) return v;
          c = (OnigCodePoint )v;
        }
	c &= 0x9f;
      }
      break;
    }
    /* fall through */

  default:
    {
    backslash:
      c = conv_backslash_value(c, env);
    }
    break;
  }

  *src = p;
  return c;
}

static int fetch_token(OnigToken* tok, UChar** src, UChar* end, ScanEnv* env);

#ifdef USE_NAMED_GROUP
#ifdef USE_BACKREF_AT_LEVEL
/*
   \k<name+n>, \k<name-n>
*/
static int
fetch_name_with_level(UChar** src, UChar* end, UChar** rname_end
		      , ScanEnv* env, int* level)
{
  int r, exist_level = 0;
  OnigCodePoint c = 0;
  OnigCodePoint first_code;
  OnigEncoding enc = env->enc;
  UChar *name_end;
  UChar *p = *src;
  PFETCH_READY;

  name_end = end;
  r = 0;
  if (PEND) {
    return ONIGERR_EMPTY_GROUP_NAME;
  }
  else {
    PFETCH(c);
    first_code = c;
    if (c == '>')
      return ONIGERR_EMPTY_GROUP_NAME;

    if (!ONIGENC_IS_CODE_WORD(enc, c)) {
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
    }
  }

  while (!PEND) {
    name_end = p;
    PFETCH(c);
    if (c == '>' || c == ')' || c == '+' || c == '-') break;

    if (!ONIGENC_IS_CODE_WORD(enc, c)) {
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
    }
  }

  if (c != '>') {
    if (c == '+' || c == '-') {
      int num;
      int flag = (c == '-' ? -1 : 1);

      PFETCH(c);
      if (! ONIGENC_IS_CODE_DIGIT(enc, c)) goto err;
      PUNFETCH;
      num = onig_scan_unsigned_number(&p, end, enc);
      if (num < 0) return ONIGERR_TOO_BIG_NUMBER;
      *level = (num * flag);
      exist_level = 1;

      PFETCH(c);
      if (c == '>')
	goto first_check;
    }

  err:
    r = ONIGERR_INVALID_GROUP_NAME;
    name_end = end;
  }
  else {
  first_check:
    if (ONIGENC_IS_CODE_ASCII(first_code) &&
        ONIGENC_IS_CODE_UPPER(enc, first_code))
      r = ONIGERR_INVALID_GROUP_NAME;
  }

  if (r == 0) {
    *rname_end = name_end;
    *src = p;
    return (exist_level ? 1 : 0);
  }
  else {
    onig_scan_env_set_error_string(env, r, *src, name_end);
    return r;
  }
}
#endif /* USE_BACKREF_AT_LEVEL */

/*
  def: 0 -> define name    (don't allow number name)
       1 -> reference name (allow number name)
*/
static int
fetch_name(UChar** src, UChar* end, UChar** rname_end, ScanEnv* env, int ref)
{
  int r, is_num;
  OnigCodePoint c = 0;
  OnigCodePoint first_code;
  OnigEncoding enc = env->enc;
  UChar *name_end;
  UChar *p = *src;
  PFETCH_READY;

  name_end = end;
  r = 0;
  is_num = 0;
  if (PEND) {
    return ONIGERR_EMPTY_GROUP_NAME;
  }
  else {
    PFETCH(c);
    first_code = c;
    if (c == '>')
      return ONIGERR_EMPTY_GROUP_NAME;

    if (ONIGENC_IS_CODE_DIGIT(enc, c)) {
      if (ref == 1)
	is_num = 1;
      else {
	r = ONIGERR_INVALID_GROUP_NAME;
      }
    }
    else if (!ONIGENC_IS_CODE_WORD(enc, c)) {
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
    }
  }

  while (!PEND) {
    name_end = p;
    PFETCH(c);
    if (c == '>' || c == ')') break;

    if (is_num == 1) {
      if (! ONIGENC_IS_CODE_DIGIT(enc, c)) {
	if (!ONIGENC_IS_CODE_WORD(enc, c))
	  r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
	else
	  r = ONIGERR_INVALID_GROUP_NAME;
      }
    }
    else {
      if (!ONIGENC_IS_CODE_WORD(enc, c)) {
        r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
      }
    }
  }

  if (c != '>') {
    r = ONIGERR_INVALID_GROUP_NAME;
    name_end = end;
  }
  else {
    if (ONIGENC_IS_CODE_ASCII(first_code) &&
        ONIGENC_IS_CODE_UPPER(enc, first_code))
      r = ONIGERR_INVALID_GROUP_NAME;
  }

  if (r == 0) {
    *rname_end = name_end;
    *src = p;
    return 0;
  }
  else {
    onig_scan_env_set_error_string(env, r, *src, name_end);
    return r;
  }
}
#else
static int
fetch_name(UChar** src, UChar* end, UChar** rname_end, ScanEnv* env, int ref)
{
  int r, len;
  OnigCodePoint c = 0;
  UChar *name_end;
  OnigEncoding enc = env->enc;
  UChar *p = *src;
  PFETCH_READY;

  r = 0;
  while (!PEND) {
    name_end = p;
    if (enc_len(enc, p) > 1)
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;

    PFETCH(c);
    if (c == '>' || c == ')') break;
    if (! ONIGENC_IS_CODE_DIGIT(enc, c))
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
  }
  if (c != '>') {
    r = ONIGERR_INVALID_GROUP_NAME;
    name_end = end;
  }

  if (r == 0) {
    *rname_end = name_end;
    *src = p;
    return 0;
  }
  else {
  err:
    onig_scan_env_set_error_string(env, r, *src, name_end);
    return r;
  }
}
#endif

static void
CC_ESC_WARN(ScanEnv* env, UChar *c)
{
  if (onig_warn == onig_null_warn) return ;

  if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_WARN_CC_OP_NOT_ESCAPED) &&
      IS_SYNTAX_BV(env->syntax, ONIG_SYN_BACKSLASH_ESCAPE_IN_CC)) {
    UChar buf[WARN_BUFSIZE];
    onig_snprintf_with_pattern(buf, WARN_BUFSIZE, env->enc,
		env->pattern, env->pattern_end,
                (UChar* )"character class has '%s' without escape", c);
    (*onig_warn)((char* )buf);
  }
}

static void
CCEND_ESC_WARN(ScanEnv* env, UChar* c)
{
  if (onig_warn == onig_null_warn) return ;

  if (IS_SYNTAX_BV((env)->syntax, ONIG_SYN_WARN_CC_OP_NOT_ESCAPED)) {
    UChar buf[WARN_BUFSIZE];
    onig_snprintf_with_pattern(buf, WARN_BUFSIZE, (env)->enc,
		(env)->pattern, (env)->pattern_end,
		(UChar* )"regular expression has '%s' without escape", c);
    (*onig_warn)((char* )buf);
  }
}

static UChar*
find_str_position(OnigCodePoint s[], int n, UChar* from, UChar* to,
		  UChar **next, OnigEncoding enc)
{
  int i;
  OnigCodePoint x;
  UChar *q;
  UChar *p = from;
  
  while (p < to) {
    x = ONIGENC_MBC_TO_CODE(enc, p, to);
    q = p + enc_len(enc, p);
    if (x == s[0]) {
      for (i = 1; i < n && q < to; i++) {
	x = ONIGENC_MBC_TO_CODE(enc, q, to);
	if (x != s[i]) break;
	q += enc_len(enc, q);
      }
      if (i >= n) {
	if (IS_NOT_NULL(next))
	  *next = q;
	return p;
      }
    }
    p = q;
  }
  return NULL_UCHARP;
}

static int
str_exist_check_with_esc(OnigCodePoint s[], int n, UChar* from, UChar* to,
			 OnigCodePoint bad, OnigEncoding enc)
{
  int i, in_esc;
  OnigCodePoint x;
  UChar *q;
  UChar *p = from;

  in_esc = 0;
  while (p < to) {
    if (in_esc) {
      in_esc = 0;
      p += enc_len(enc, p);
    }
    else {
      x = ONIGENC_MBC_TO_CODE(enc, p, to);
      q = p + enc_len(enc, p);
      if (x == s[0]) {
	for (i = 1; i < n && q < to; i++) {
	  x = ONIGENC_MBC_TO_CODE(enc, q, to);
	  if (x != s[i]) break;
	  q += enc_len(enc, q);
	}
	if (i >= n) return 1;
	p += enc_len(enc, p);
      }
      else {
	x = ONIGENC_MBC_TO_CODE(enc, p, to);
	if (x == bad) return 0;
	else if (x == MC_ESC(enc)) in_esc = 1;
	p = q;
      }
    }
  }
  return 0;
}

static int
fetch_token_in_cc(OnigToken* tok, UChar** src, UChar* end, ScanEnv* env)
{
  int num;
  OnigCodePoint c, c2;
  OnigSyntaxType* syn = env->syntax;
  OnigEncoding enc = env->enc;
  UChar* prev;
  UChar* p = *src;
  PFETCH_READY;

  if (PEND) {
    tok->type = TK_EOT;
    return tok->type;
  }

  PFETCH(c);
  tok->type = TK_CHAR;
  tok->base = 0;
  tok->u.c  = c;
  tok->escaped = 0;

  if (c == ']') {
    tok->type = TK_CC_CLOSE;
  }
  else if (c == '-') {
    tok->type = TK_CC_RANGE;
  }
  else if (c == MC_ESC(enc)) {
    if (! IS_SYNTAX_BV(syn, ONIG_SYN_BACKSLASH_ESCAPE_IN_CC))
      goto end;

    if (PEND) return ONIGERR_END_PATTERN_AT_ESCAPE;

    PFETCH(c);
    tok->escaped = 1;
    tok->u.c = c;
    switch (c) {
    case 'w':
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_WORD;
      break;
    case 'W':
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_WORD;
      break;
    case 'd':
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_DIGIT;
      break;
    case 'D':
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_DIGIT;
      break;
    case 's':
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_WHITE_SPACE;
      break;
    case 'S':
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_WHITE_SPACE;
      break;
    case 'h':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_XDIGIT;
      break;
    case 'H':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_XDIGIT;
      break;

    case 'p':
    case 'P':
      c2 = PPEEK;
      if (c2 == '{' &&
	  IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY)) {
	PINC;
	tok->type = TK_CHAR_PROPERTY;
	tok->u.prop.not = (c == 'P' ? 1 : 0);

	if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT)) {
	  PFETCH(c2);
	  if (c2 == '^') {
	    tok->u.prop.not = (tok->u.prop.not == 0 ? 1 : 0);
	  }
	  else
	    PUNFETCH;
	}
      }
      break;

    case 'x':
      if (PEND) break;

      prev = p;
      if (PPEEK_IS('{') && IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_X_BRACE_HEX8)) {
	PINC;
	num = scan_unsigned_hexadecimal_number(&p, end, 8, enc);
	if (num < 0) return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
	if (!PEND) {
          c2 = PPEEK;
          if (ONIGENC_IS_CODE_XDIGIT(enc, c2))
            return ONIGERR_TOO_LONG_WIDE_CHAR_VALUE;
        }

	if (p > prev + enc_len(enc, prev) && !PEND && (PPEEK_IS('}'))) {
	  PINC;
	  tok->type   = TK_CODE_POINT;
	  tok->base   = 16;
	  tok->u.code = (OnigCodePoint )num;
	}
	else {
	  /* can't read nothing or invalid format */
	  p = prev;
	}
      }
      else if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_X_HEX2)) {
	num = scan_unsigned_hexadecimal_number(&p, end, 2, enc);
	if (num < 0) return ONIGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type = TK_RAW_BYTE;
	tok->base = 16;
	tok->u.c  = num;
      }
      break;

    case 'u':
      if (PEND) break;

      prev = p;
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_U_HEX4)) {
	num = scan_unsigned_hexadecimal_number(&p, end, 4, enc);
	if (num < 0) return ONIGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type   = TK_CODE_POINT;
	tok->base   = 16;
	tok->u.code = (OnigCodePoint )num;
      }
      break;

    case '0':
    case '1': case '2': case '3': case '4': case '5': case '6': case '7':
      if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_OCTAL3)) {
	PUNFETCH;
	prev = p;
	num = scan_unsigned_octal_number(&p, end, 3, enc);
	if (num < 0) return ONIGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type = TK_RAW_BYTE;
	tok->base = 8;
	tok->u.c  = num;
      }
      break;

    default:
      PUNFETCH;
      num = fetch_escaped_value(&p, end, env);
      if (num < 0) return num;
      if (tok->u.c != num) {
	tok->u.code = (OnigCodePoint )num;
	tok->type   = TK_CODE_POINT;
      }
      break;
    }
  }
  else if (c == '[') {
    if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_POSIX_BRACKET) && (PPEEK_IS(':'))) {
      OnigCodePoint send[] = { (OnigCodePoint )':', (OnigCodePoint )']' };
      tok->backp = p; /* point at '[' is readed */
      PINC;
      if (str_exist_check_with_esc(send, 2, p, end,
                                   (OnigCodePoint )']', enc)) {
	tok->type = TK_POSIX_BRACKET_OPEN;
      }
      else {
	PUNFETCH;
	goto cc_in_cc;
      }
    }
    else {
    cc_in_cc:
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_CCLASS_SET_OP)) {
	tok->type = TK_CC_CC_OPEN;
      }
      else {
	CC_ESC_WARN(env, (UChar* )"[");
      }
    }
  }
  else if (c == '&') {
    if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_CCLASS_SET_OP) &&
	!PEND && (PPEEK_IS('&'))) {
      PINC;
      tok->type = TK_CC_AND;
    }
  }

 end:
  *src = p;
  return tok->type;
}

static int
fetch_token(OnigToken* tok, UChar** src, UChar* end, ScanEnv* env)
{
  int r, num;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  OnigSyntaxType* syn = env->syntax;
  UChar* prev;
  UChar* p = *src;
  PFETCH_READY;

 start:
  if (PEND) {
    tok->type = TK_EOT;
    return tok->type;
  }

  tok->type  = TK_STRING;
  tok->base  = 0;
  tok->backp = p;

  PFETCH(c);
  if (IS_MC_ESC_CODE(c, enc, syn)) {
    if (PEND) return ONIGERR_END_PATTERN_AT_ESCAPE;

    tok->backp = p;
    PFETCH(c);

    tok->u.c = c;
    tok->escaped = 1;
    switch (c) {
    case '*':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_ASTERISK_ZERO_INF)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '+':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_PLUS_ONE_INF)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 1;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '?':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_QMARK_ZERO_ONE)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = 1;
    greedy_check:
      if (!PEND && PPEEK_IS('?') &&
	  IS_SYNTAX_OP(syn, ONIG_SYN_OP_QMARK_NON_GREEDY)) {
	PFETCH(c);
	tok->u.repeat.greedy     = 0;
	tok->u.repeat.possessive = 0;
      }
      else {
      possessive_check:
	if (!PEND && PPEEK_IS('+') &&
	    ((IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_PLUS_POSSESSIVE_REPEAT) &&
	      tok->type != TK_INTERVAL)  ||
	     (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_PLUS_POSSESSIVE_INTERVAL) &&
	      tok->type == TK_INTERVAL))) {
	  PFETCH(c);
	  tok->u.repeat.greedy     = 1;
	  tok->u.repeat.possessive = 1;
	}
	else {
	  tok->u.repeat.greedy     = 1;
	  tok->u.repeat.possessive = 0;
	}
      }
      break;

    case '{':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_BRACE_INTERVAL)) break;
      r = fetch_range_quantifier(&p, end, tok, env);
      if (r < 0) return r;  /* error */
      if (r == 0) goto greedy_check;
      else if (r == 2) { /* {n} */
	if (IS_SYNTAX_BV(syn, ONIG_SYN_FIXED_INTERVAL_IS_GREEDY_ONLY))
	  goto possessive_check;

	goto greedy_check;
      }
      /* r == 1 : normal char */
      break;

    case '|':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_VBAR_ALT)) break;
      tok->type = TK_ALT;
      break;

    case '(':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_LPAREN_SUBEXP)) break;
      tok->type = TK_SUBEXP_OPEN;
      break;

    case ')':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_LPAREN_SUBEXP)) break;
      tok->type = TK_SUBEXP_CLOSE;
      break;

    case 'w':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_W_WORD)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_WORD;
      break;

    case 'W':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_W_WORD)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_WORD;
      break;

    case 'b':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_B_WORD_BOUND)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_WORD_BOUND;
      break;

    case 'B':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_B_WORD_BOUND)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_NOT_WORD_BOUND;
      break;

#ifdef USE_WORD_BEGIN_END
    case '<':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_WORD_BEGIN;
      break;

    case '>':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_WORD_END;
      break;
#endif

    case 's':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_S_WHITE_SPACE)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_WHITE_SPACE;
      break;

    case 'S':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_S_WHITE_SPACE)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_WHITE_SPACE;
      break;

    case 'd':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_D_DIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_DIGIT;
      break;

    case 'D':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_D_DIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_DIGIT;
      break;

    case 'h':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_XDIGIT;
      break;

    case 'H':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_XDIGIT;
      break;

    case 'A':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR)) break;
    begin_buf:
      tok->type = TK_ANCHOR;
      tok->u.subtype = ANCHOR_BEGIN_BUF;
      break;

    case 'Z':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.subtype = ANCHOR_SEMI_END_BUF;
      break;

    case 'z':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR)) break;
    end_buf:
      tok->type = TK_ANCHOR;
      tok->u.subtype = ANCHOR_END_BUF;
      break;

    case 'G':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_CAPITAL_G_BEGIN_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.subtype = ANCHOR_BEGIN_POSITION;
      break;

    case '`':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_GNU_BUF_ANCHOR)) break;
      goto begin_buf;
      break;

    case '\'':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_GNU_BUF_ANCHOR)) break;
      goto end_buf;
      break;

    case 'x':
      if (PEND) break;

      prev = p;
      if (PPEEK_IS('{') && IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_X_BRACE_HEX8)) {
	PINC;
	num = scan_unsigned_hexadecimal_number(&p, end, 8, enc);
	if (num < 0) return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
	if (!PEND) {
          if (ONIGENC_IS_CODE_XDIGIT(enc, PPEEK))
            return ONIGERR_TOO_LONG_WIDE_CHAR_VALUE;
        }

	if ((p > prev + enc_len(enc, prev)) && !PEND && PPEEK_IS('}')) {
	  PINC;
	  tok->type   = TK_CODE_POINT;
	  tok->u.code = (OnigCodePoint )num;
	}
	else {
	  /* can't read nothing or invalid format */
	  p = prev;
	}
      }
      else if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_X_HEX2)) {
	num = scan_unsigned_hexadecimal_number(&p, end, 2, enc);
	if (num < 0) return ONIGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type = TK_RAW_BYTE;
	tok->base = 16;
	tok->u.c  = num;
      }
      break;

    case 'u':
      if (PEND) break;

      prev = p;
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_U_HEX4)) {
	num = scan_unsigned_hexadecimal_number(&p, end, 4, enc);
	if (num < 0) return ONIGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type   = TK_CODE_POINT;
	tok->base   = 16;
	tok->u.code = (OnigCodePoint )num;
      }
      break;

    case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      PUNFETCH;
      prev = p;
      num = onig_scan_unsigned_number(&p, end, enc);
      if (num < 0 || num > ONIG_MAX_BACKREF_NUM) {
        goto skip_backref;
      }

      if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_DECIMAL_BACKREF) && 
	  (num <= env->num_mem || num <= 9)) { /* This spec. from GNU regex */
	if (IS_SYNTAX_BV(syn, ONIG_SYN_STRICT_CHECK_BACKREF)) {
	  if (num > env->num_mem || IS_NULL(SCANENV_MEM_NODES(env)[num]))
	    return ONIGERR_INVALID_BACKREF;
	}

	tok->type = TK_BACKREF;
	tok->u.backref.num     = 1;
	tok->u.backref.ref1    = num;
	tok->u.backref.by_name = 0;
#ifdef USE_BACKREF_AT_LEVEL
	tok->u.backref.exist_level = 0;
#endif
	break;
      }

    skip_backref:
      if (c == '8' || c == '9') {
	/* normal char */
	p = prev; PINC;
	break;
      }

      p = prev;
      /* fall through */
    case '0':
      if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_OCTAL3)) {
	prev = p;
	num = scan_unsigned_octal_number(&p, end, (c == '0' ? 2:3), enc);
	if (num < 0) return ONIGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type = TK_RAW_BYTE;
	tok->base = 8;
	tok->u.c  = num;
      }
      else if (c != '0') {
	PINC;
      }
      break;

#ifdef USE_NAMED_GROUP
    case 'k':
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_K_NAMED_BACKREF)) {
	PFETCH(c);
	if (c == '<') {
	  UChar* name_end;
	  int* backs;

	  prev = p;

#ifdef USE_BACKREF_AT_LEVEL
	  name_end = NULL_UCHARP; /* no need. escape gcc warning. */
	  r = fetch_name_with_level(&p, end, &name_end, env, &tok->u.backref.level);
	  if (r == 1) tok->u.backref.exist_level = 1;
	  else        tok->u.backref.exist_level = 0;
#else
	  r = fetch_name(&p, end, &name_end, env, 1);
#endif
	  if (r < 0) return r;

	  num = onig_name_to_group_numbers(env->reg, prev, name_end, &backs);
	  if (num <= 0) {
	    onig_scan_env_set_error_string(env,
			    ONIGERR_UNDEFINED_NAME_REFERENCE, prev, name_end);
	    return ONIGERR_UNDEFINED_NAME_REFERENCE;
	  }
	  if (IS_SYNTAX_BV(syn, ONIG_SYN_STRICT_CHECK_BACKREF)) {
	    int i;
	    for (i = 0; i < num; i++) {
	      if (backs[i] > env->num_mem ||
		  IS_NULL(SCANENV_MEM_NODES(env)[backs[i]]))
		return ONIGERR_INVALID_BACKREF;
	    }
	  }

	  tok->type = TK_BACKREF;
	  tok->u.backref.by_name = 1;
	  if (num == 1) {
	    tok->u.backref.num  = 1;
	    tok->u.backref.ref1 = backs[0];
	  }
	  else {
	    tok->u.backref.num  = num;
	    tok->u.backref.refs = backs;
	  }
	}
	else
	  PUNFETCH;
      }
      break;
#endif

#ifdef USE_SUBEXP_CALL
    case 'g':
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_G_SUBEXP_CALL)) {
	PFETCH(c);
	if (c == '<') {
	  UChar* name_end;

	  prev = p;
	  r = fetch_name(&p, end, &name_end, env, 1);
	  if (r < 0) return r;

	  tok->type = TK_CALL;
	  tok->u.call.name     = prev;
	  tok->u.call.name_end = name_end;
	}
	else
	  PUNFETCH;
      }
      break;
#endif

    case 'Q':
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_CAPITAL_Q_QUOTE)) {
	tok->type = TK_QUOTE_OPEN;
      }
      break;

    case 'p':
    case 'P':
      if (PPEEK_IS('{') &&
	  IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY)) {
	PINC;
	tok->type = TK_CHAR_PROPERTY;
	tok->u.prop.not = (c == 'P' ? 1 : 0);

	if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT)) {
	  PFETCH(c);
	  if (c == '^') {
	    tok->u.prop.not = (tok->u.prop.not == 0 ? 1 : 0);
	  }
	  else
	    PUNFETCH;
	}
      }
      break;

    default:
      PUNFETCH;
      num = fetch_escaped_value(&p, end, env);
      if (num < 0) return num;
      /* set_raw: */
      if (tok->u.c != num) {
	tok->type = TK_CODE_POINT;
	tok->u.code = (OnigCodePoint )num;
      }
      else { /* string */
	p = tok->backp + enc_len(enc, tok->backp);
      }
      break;
    }
  }
  else {
    tok->u.c = c;
    tok->escaped = 0;

#ifdef USE_VARIABLE_META_CHARS
    if ((c != ONIG_INEFFECTIVE_META_CHAR) &&
	IS_SYNTAX_OP(syn, ONIG_SYN_OP_VARIABLE_META_CHARACTERS)) {
      if (c == MC_ANYCHAR(enc))
	goto any_char;
      else if (c == MC_ANYTIME(enc))
	goto anytime;
      else if (c == MC_ZERO_OR_ONE_TIME(enc))
	goto zero_or_one_time;
      else if (c == MC_ONE_OR_MORE_TIME(enc))
	goto one_or_more_time;
      else if (c == MC_ANYCHAR_ANYTIME(enc)) {
	tok->type = TK_ANYCHAR_ANYTIME;
	goto out;
      }
    }
#endif

    switch (c) {
    case '.':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_DOT_ANYCHAR)) break;
#ifdef USE_VARIABLE_META_CHARS
    any_char:
#endif
      tok->type = TK_ANYCHAR;
      break;

    case '*':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ASTERISK_ZERO_INF)) break;
#ifdef USE_VARIABLE_META_CHARS
    anytime:
#endif
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '+':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_PLUS_ONE_INF)) break;
#ifdef USE_VARIABLE_META_CHARS
    one_or_more_time:
#endif
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 1;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '?':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_QMARK_ZERO_ONE)) break;
#ifdef USE_VARIABLE_META_CHARS
    zero_or_one_time:
#endif
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = 1;
      goto greedy_check;
      break;

    case '{':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_BRACE_INTERVAL)) break;
      r = fetch_range_quantifier(&p, end, tok, env);
      if (r < 0) return r;  /* error */
      if (r == 0) goto greedy_check;
      else if (r == 2) { /* {n} */
	if (IS_SYNTAX_BV(syn, ONIG_SYN_FIXED_INTERVAL_IS_GREEDY_ONLY))
	  goto possessive_check;

	goto greedy_check;
      }
      /* r == 1 : normal char */
      break;

    case '|':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_VBAR_ALT)) break;
      tok->type = TK_ALT;
      break;

    case '(':
      if (PPEEK_IS('?') &&
          IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_QMARK_GROUP_EFFECT)) {
        PINC;
        if (PPEEK_IS('#')) {
          PFETCH(c);
          while (1) {
            if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
            PFETCH(c);
            if (c == MC_ESC(enc)) {
              if (!PEND) PFETCH(c);
            }
            else {
              if (c == ')') break;
            }
          }
          goto start;
        }
        PUNFETCH;
      }

      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_LPAREN_SUBEXP)) break;
      tok->type = TK_SUBEXP_OPEN;
      break;

    case ')':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_LPAREN_SUBEXP)) break;
      tok->type = TK_SUBEXP_CLOSE;
      break;

    case '^':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_LINE_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.subtype = (IS_SINGLELINE(env->option)
			? ANCHOR_BEGIN_BUF : ANCHOR_BEGIN_LINE);
      break;

    case '$':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_LINE_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.subtype = (IS_SINGLELINE(env->option)
			? ANCHOR_SEMI_END_BUF : ANCHOR_END_LINE);
      break;

    case '[':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_BRACKET_CC)) break;
      tok->type = TK_CC_OPEN;
      break;

    case ']':
      if (*src > env->pattern)   /* /].../ is allowed. */
	CCEND_ESC_WARN(env, (UChar* )"]");
      break;

    case '#':
      if (IS_EXTEND(env->option)) {
	while (!PEND) {
	  PFETCH(c);
	  if (ONIGENC_IS_CODE_NEWLINE(enc, c))
	    break;
	}
	goto start;
	break;
      }
      break;

    case ' ': case '\t': case '\n': case '\r': case '\f':
      if (IS_EXTEND(env->option))
	goto start;
      break;

    default:
      /* string */
      break;
    }
  }

#ifdef USE_VARIABLE_META_CHARS
 out:
#endif
  *src = p;
  return tok->type;
}

static int
add_ctype_to_cc_by_range(CClassNode* cc, int ctype, int not, OnigEncoding enc,
                         const OnigCodePoint sbr[], const OnigCodePoint mbr[])
{
  int i, r;
  OnigCodePoint j;

  int nsb = ONIGENC_CODE_RANGE_NUM(sbr);
  int nmb = ONIGENC_CODE_RANGE_NUM(mbr);

  if (not == 0) {
    for (i = 0; i < nsb; i++) {
      for (j  = ONIGENC_CODE_RANGE_FROM(sbr, i);
           j <= ONIGENC_CODE_RANGE_TO(sbr, i); j++) {
        BITSET_SET_BIT(cc->bs, j);
      }
    }

    for (i = 0; i < nmb; i++) {
      r = add_code_range_to_buf(&(cc->mbuf),
                                ONIGENC_CODE_RANGE_FROM(mbr, i),
                                ONIGENC_CODE_RANGE_TO(mbr, i));
      if (r != 0) return r;
    }
  }
  else {
    OnigCodePoint prev = 0;

    if (ONIGENC_MBC_MINLEN(enc) == 1) {
      for (i = 0; i < nsb; i++) {
        for (j = prev;
             j < ONIGENC_CODE_RANGE_FROM(sbr, i); j++) {
          BITSET_SET_BIT(cc->bs, j);
        }
        prev = ONIGENC_CODE_RANGE_TO(sbr, i) + 1;
      }
      if (prev < 0x7f) {
        for (j = prev; j < 0x7f; j++) {
          BITSET_SET_BIT(cc->bs, j);
        }
      }

      prev = 0x80;
    }

    for (i = 0; i < nmb; i++) {
      if (prev < ONIGENC_CODE_RANGE_FROM(mbr, i)) {
	r = add_code_range_to_buf(&(cc->mbuf), prev,
                                  ONIGENC_CODE_RANGE_FROM(mbr, i) - 1);
	if (r != 0) return r;
      }
      prev = ONIGENC_CODE_RANGE_TO(mbr, i) + 1;
    }
    if (prev < 0x7fffffff) {
      r = add_code_range_to_buf(&(cc->mbuf), prev, 0x7fffffff);
      if (r != 0) return r;
    }
  }

  return 0;
}

static int
add_ctype_to_cc(CClassNode* cc, int ctype, int not, ScanEnv* env)
{
  int c, r;
  const OnigCodePoint *sbr, *mbr;
  OnigEncoding enc = env->enc;

  r = ONIGENC_GET_CTYPE_CODE_RANGE(enc, ctype, &sbr, &mbr);
  if (r == 0) {
    return add_ctype_to_cc_by_range(cc, ctype, not, env->enc, sbr, mbr);
  }
  else if (r != ONIG_NO_SUPPORT_CONFIG) {
    return r;
  }

  r = 0;
  switch (ctype) {
  case ONIGENC_CTYPE_ALPHA:
  case ONIGENC_CTYPE_BLANK:
  case ONIGENC_CTYPE_CNTRL:
  case ONIGENC_CTYPE_DIGIT:
  case ONIGENC_CTYPE_LOWER:
  case ONIGENC_CTYPE_PUNCT:
  case ONIGENC_CTYPE_SPACE:
  case ONIGENC_CTYPE_UPPER:
  case ONIGENC_CTYPE_XDIGIT:
  case ONIGENC_CTYPE_ASCII:
  case ONIGENC_CTYPE_ALNUM:
    if (not != 0) {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    else {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT(cc->bs, c);
      }
    }
    break;

  case ONIGENC_CTYPE_GRAPH:
  case ONIGENC_CTYPE_PRINT:
    if (not != 0) {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT(cc->bs, c);
      }
    }
    else {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    break;

  case ONIGENC_CTYPE_WORD:
    if (not == 0) {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (ONIGENC_IS_CODE_SB_WORD(enc, c)) BITSET_SET_BIT(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    else {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
        if ((ONIGENC_CODE_TO_MBCLEN(enc, c) > 0)  /* 0: invalid code point */
	    && ! ONIGENC_IS_CODE_WORD(enc, c))
	  BITSET_SET_BIT(cc->bs, c);
      }
    }
    break;

  default:
    return ONIGERR_PARSER_BUG;
    break;
  }

  return r;
}

static int
parse_ctype_to_enc_ctype(int pctype, int* not)
{
  int ctype;

  switch (pctype) {
  case CTYPE_WORD:
    ctype = ONIGENC_CTYPE_WORD;
    *not = 0;
    break;
  case CTYPE_NOT_WORD:
    ctype = ONIGENC_CTYPE_WORD;
    *not = 1;
    break;
  case CTYPE_WHITE_SPACE:
    ctype = ONIGENC_CTYPE_SPACE;
    *not = 0;
    break;
  case CTYPE_NOT_WHITE_SPACE:
    ctype = ONIGENC_CTYPE_SPACE;
    *not = 1;
    break;
  case CTYPE_DIGIT:
    ctype = ONIGENC_CTYPE_DIGIT;
    *not = 0;
    break;
  case CTYPE_NOT_DIGIT:
    ctype = ONIGENC_CTYPE_DIGIT;
    *not = 1;
    break;
  case CTYPE_XDIGIT:
    ctype = ONIGENC_CTYPE_XDIGIT;
    *not = 0;
    break;
  case CTYPE_NOT_XDIGIT:
    ctype = ONIGENC_CTYPE_XDIGIT;
    *not = 1;
    break;
  default:
    return ONIGERR_PARSER_BUG;
    break;
  }
  return ctype;
}

typedef struct {
  UChar    *name;
  int       ctype;
  short int len;
} PosixBracketEntryType;

static int
parse_posix_bracket(CClassNode* cc, UChar** src, UChar* end, ScanEnv* env)
{
#define POSIX_BRACKET_CHECK_LIMIT_LENGTH  20
#define POSIX_BRACKET_NAME_MAX_LEN         6

  static PosixBracketEntryType PBS[] = {
    { (UChar* )"alnum",  ONIGENC_CTYPE_ALNUM,  5 },
    { (UChar* )"alpha",  ONIGENC_CTYPE_ALPHA,  5 },
    { (UChar* )"blank",  ONIGENC_CTYPE_BLANK,  5 },
    { (UChar* )"cntrl",  ONIGENC_CTYPE_CNTRL,  5 },
    { (UChar* )"digit",  ONIGENC_CTYPE_DIGIT,  5 },
    { (UChar* )"graph",  ONIGENC_CTYPE_GRAPH,  5 },
    { (UChar* )"lower",  ONIGENC_CTYPE_LOWER,  5 },
    { (UChar* )"print",  ONIGENC_CTYPE_PRINT,  5 },
    { (UChar* )"punct",  ONIGENC_CTYPE_PUNCT,  5 },
    { (UChar* )"space",  ONIGENC_CTYPE_SPACE,  5 },
    { (UChar* )"upper",  ONIGENC_CTYPE_UPPER,  5 },
    { (UChar* )"xdigit", ONIGENC_CTYPE_XDIGIT, 6 },
    { (UChar* )"ascii",  ONIGENC_CTYPE_ASCII,  5 },
    { (UChar* )NULL, -1, 0 }
  };

  PosixBracketEntryType *pb;
  int not, i, r;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar *p = *src;
  PFETCH_READY;

  if (PPEEK_IS('^')) {
    PINC;
    not = 1;
  }
  else
    not = 0;

  if (onigenc_strlen(enc, p, end) < POSIX_BRACKET_NAME_MAX_LEN + 2)
    goto not_posix_bracket;

  for (pb = PBS; IS_NOT_NULL(pb->name); pb++) {
    if (onigenc_with_ascii_strncmp(enc, p, end, pb->name, pb->len) == 0) {
      p = (UChar* )onigenc_step(enc, p, end, pb->len);
      if (onigenc_with_ascii_strncmp(enc, p, end, (UChar* )":]", 2) != 0)
	return ONIGERR_INVALID_POSIX_BRACKET_TYPE;

      r = add_ctype_to_cc(cc, pb->ctype, not, env);
      if (r != 0) return r;

      PINC; PINC;
      *src = p;
      return 0;
    }
  }

 not_posix_bracket:
  c = 0;
  i = 0;
  while (!PEND && ((c = PPEEK) != ':') && c != ']') {
    PINC;
    if (++i > POSIX_BRACKET_CHECK_LIMIT_LENGTH) break;
  }
  if (c == ':' && ! PEND) {
    PINC;
    if (! PEND) {
      PFETCH(c);
      if (c == ']')
	return ONIGERR_INVALID_POSIX_BRACKET_TYPE;
    }
  }

  return 1;   /* 1: is not POSIX bracket, but no error. */
}

static int
property_name_to_ctype(UChar* p, UChar* end, OnigEncoding enc)
{
  static PosixBracketEntryType PBS[] = {
    { (UChar* )"Alnum",  ONIGENC_CTYPE_ALNUM,  5 },
    { (UChar* )"Alpha",  ONIGENC_CTYPE_ALPHA,  5 },
    { (UChar* )"Blank",  ONIGENC_CTYPE_BLANK,  5 },
    { (UChar* )"Cntrl",  ONIGENC_CTYPE_CNTRL,  5 },
    { (UChar* )"Digit",  ONIGENC_CTYPE_DIGIT,  5 },
    { (UChar* )"Graph",  ONIGENC_CTYPE_GRAPH,  5 },
    { (UChar* )"Lower",  ONIGENC_CTYPE_LOWER,  5 },
    { (UChar* )"Print",  ONIGENC_CTYPE_PRINT,  5 },
    { (UChar* )"Punct",  ONIGENC_CTYPE_PUNCT,  5 },
    { (UChar* )"Space",  ONIGENC_CTYPE_SPACE,  5 },
    { (UChar* )"Upper",  ONIGENC_CTYPE_UPPER,  5 },
    { (UChar* )"XDigit", ONIGENC_CTYPE_XDIGIT, 6 },
    { (UChar* )"ASCII",  ONIGENC_CTYPE_ASCII,  5 },
    { (UChar* )NULL, -1, 0 }
  };

  PosixBracketEntryType *pb;
  int len;

  len = onigenc_strlen(enc, p, end);
  for (pb = PBS; IS_NOT_NULL(pb->name); pb++) {
    if (len == pb->len &&
        onigenc_with_ascii_strncmp(enc, p, end, pb->name, pb->len) == 0)
      return pb->ctype;
  }

  return -1;
}

static int
fetch_char_property_to_ctype(UChar** src, UChar* end, ScanEnv* env)
{
  int ctype;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar *prev, *start, *p = *src;
  PFETCH_READY;

  /* 'IsXXXX' => 'XXXX' */
  if (!PEND &&
      IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_CHAR_PROPERTY_PREFIX_IS)) {
    c = PPEEK;
    if (c == 'I') {
      PINC;
      if (! PEND) {
	c = PPEEK;
	if (c == 's')
	  PINC;
	else
	  PUNFETCH;
      }
    }
  }

  start = prev = p;

  while (!PEND) {
    prev = p;
    PFETCH(c);
    if (c == '}') {
      ctype = property_name_to_ctype(start, prev, enc);
      if (ctype < 0) break;

      *src = p;
      return ctype;
    }
    else if (c == '(' || c == ')' || c == '{' || c == '|')
      break;
  }

  onig_scan_env_set_error_string(env, ONIGERR_INVALID_CHAR_PROPERTY_NAME,
				 *src, prev);
  return ONIGERR_INVALID_CHAR_PROPERTY_NAME;
}

static int
parse_char_property(Node** np, OnigToken* tok, UChar** src, UChar* end,
		    ScanEnv* env)
{
  int r, ctype;
  CClassNode* cc;

  ctype = fetch_char_property_to_ctype(src, end, env);
  if (ctype < 0) return ctype;

  *np = node_new_cclass();
  CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
  cc = &(NCCLASS(*np));
  r = add_ctype_to_cc(cc, ctype, 0, env);
  if (r != 0) return r;
  if (tok->u.prop.not != 0) CCLASS_SET_NOT(cc);

  return 0;
}


enum CCSTATE {
  CCS_VALUE,
  CCS_RANGE,
  CCS_COMPLETE,
  CCS_START
};

enum CCVALTYPE {
  CCV_SB,
  CCV_CODE_POINT,
  CCV_CLASS
};

static int
next_state_class(CClassNode* cc, OnigCodePoint* vs, enum CCVALTYPE* type,
		 enum CCSTATE* state, ScanEnv* env)
{
  int r;

  if (*state == CCS_RANGE)
    return ONIGERR_CHAR_CLASS_VALUE_AT_END_OF_RANGE;

  if (*state == CCS_VALUE && *type != CCV_CLASS) {
    if (*type == CCV_SB)
      BITSET_SET_BIT(cc->bs, (int )(*vs));
    else if (*type == CCV_CODE_POINT) {
      r = add_code_range(&(cc->mbuf), env, *vs, *vs);
      if (r < 0) return r;
    }
  }

  *state = CCS_VALUE;
  *type  = CCV_CLASS;
  return 0;
}

static int
next_state_val(CClassNode* cc, OnigCodePoint *vs, OnigCodePoint v,
	       int* vs_israw, int v_israw,
	       enum CCVALTYPE intype, enum CCVALTYPE* type,
	       enum CCSTATE* state, ScanEnv* env)
{
  int r;

  switch (*state) {
  case CCS_VALUE:
    if (*type == CCV_SB)
      BITSET_SET_BIT(cc->bs, (int )(*vs));
    else if (*type == CCV_CODE_POINT) {
      r = add_code_range(&(cc->mbuf), env, *vs, *vs);
      if (r < 0) return r;
    }
    break;

  case CCS_RANGE:
    if (intype == *type) {
      if (intype == CCV_SB) {
        if (*vs > 0xff || v > 0xff)
          return ONIGERR_INVALID_WIDE_CHAR_VALUE;

	if (*vs > v) {
	  if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
	    goto ccs_range_end;
	  else
	    return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
	}
	bitset_set_range(cc->bs, (int )*vs, (int )v);
      }
      else {
	r = add_code_range(&(cc->mbuf), env, *vs, v);
	if (r < 0) return r;
      }
    }
    else {
#if 0
      if (intype == CCV_CODE_POINT && *type == CCV_SB) {
#endif
	if (*vs > v) {
	  if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
	    goto ccs_range_end;
	  else
	    return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
	}
	bitset_set_range(cc->bs, (int )*vs, (int )(v < 0xff ? v : 0xff));
	r = add_code_range(&(cc->mbuf), env, (OnigCodePoint )*vs, v);
	if (r < 0) return r;
#if 0
      }
      else
	return ONIGERR_MISMATCH_CODE_LENGTH_IN_CLASS_RANGE;
#endif
    }
  ccs_range_end:
    *state = CCS_COMPLETE;
    break;

  case CCS_COMPLETE:
  case CCS_START:
    *state = CCS_VALUE;
    break;

  default:
    break;
  }

  *vs_israw = v_israw;
  *vs       = v;
  *type     = intype;
  return 0;
}

static int
code_exist_check(OnigCodePoint c, UChar* from, UChar* end, int ignore_escaped,
		 OnigEncoding enc)
{
  int in_esc;
  OnigCodePoint code;
  UChar* p = from;
  PFETCH_READY;

  in_esc = 0;
  while (! PEND) {
    if (ignore_escaped && in_esc) {
      in_esc = 0;
    }
    else {
      PFETCH(code);
      if (code == c) return 1;
      if (code == MC_ESC(enc)) in_esc = 1;
    }
  }
  return 0;
}

static int
parse_char_class(Node** np, OnigToken* tok, UChar** src, UChar* end,
		 ScanEnv* env)
{
  int r, neg, len, fetched, and_start;
  OnigCodePoint v, vs;
  UChar *p;
  Node* node;
  CClassNode *cc, *prev_cc;
  CClassNode work_cc;

  enum CCSTATE state;
  enum CCVALTYPE val_type, in_type;
  int val_israw, in_israw;

  prev_cc = (CClassNode* )NULL;
  *np = NULL_NODE;
  r = fetch_token_in_cc(tok, src, end, env);
  if (r == TK_CHAR && tok->u.c == '^' && tok->escaped == 0) {
    neg = 1;
    r = fetch_token_in_cc(tok, src, end, env);
  }
  else {
    neg = 0;
  }

  if (r < 0) return r;
  if (r == TK_CC_CLOSE) {
    if (! code_exist_check((OnigCodePoint )']',
                           *src, env->pattern_end, 1, env->enc))
      return ONIGERR_EMPTY_CHAR_CLASS;

    CC_ESC_WARN(env, (UChar* )"]");
    r = tok->type = TK_CHAR;  /* allow []...] */
  }

  *np = node = node_new_cclass();
  CHECK_NULL_RETURN_VAL(node, ONIGERR_MEMORY);
  cc = &(NCCLASS(node));

  and_start = 0;
  state = CCS_START;
  p = *src;
  while (r != TK_CC_CLOSE) {
    fetched = 0;
    switch (r) {
    case TK_CHAR:
      len = ONIGENC_CODE_TO_MBCLEN(env->enc, tok->u.c);
      if (len > 1) {
	in_type = CCV_CODE_POINT;
      }
      else {
      sb_char:
	in_type = CCV_SB;
      }
      v = (OnigCodePoint )tok->u.c;
      in_israw = 0;
      goto val_entry2;
      break;

    case TK_RAW_BYTE:
      /* tok->base != 0 : octal or hexadec. */
      if (! ONIGENC_IS_SINGLEBYTE(env->enc) && tok->base != 0) {
	UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
	UChar* bufe = buf + ONIGENC_CODE_TO_MBC_MAXLEN;
	UChar* psave = p;
	int i, base = tok->base;

	buf[0] = tok->u.c;
	for (i = 1; i < ONIGENC_MBC_MAXLEN(env->enc); i++) {
	  r = fetch_token_in_cc(tok, &p, end, env);
	  if (r < 0) goto err;
	  if (r != TK_RAW_BYTE || tok->base != base) {
	    fetched = 1;
	    break;
	  }
	  buf[i] = tok->u.c;
	}

	if (i < ONIGENC_MBC_MINLEN(env->enc)) {
	  r = ONIGERR_TOO_SHORT_MULTI_BYTE_STRING;
	  goto err;
	}

	len = enc_len(env->enc, buf);
	if (i < len) {
	  r = ONIGERR_TOO_SHORT_MULTI_BYTE_STRING;
	  goto err;
	}
	else if (i > len) { /* fetch back */
	  p = psave;
	  for (i = 1; i < len; i++) {
	    r = fetch_token_in_cc(tok, &p, end, env);
	  }
	  fetched = 0;
	}

	if (i == 1) {
	  v = (OnigCodePoint )buf[0];
	  goto raw_single;
	}
	else {
	  v = ONIGENC_MBC_TO_CODE(env->enc, buf, bufe);
	  in_type = CCV_CODE_POINT;
	}
      }
      else {
	v = (OnigCodePoint )tok->u.c;
      raw_single:
	in_type = CCV_SB;
      }
      in_israw = 1;
      goto val_entry2;
      break;

    case TK_CODE_POINT:
      v = tok->u.code;
      in_israw = 1;
    val_entry:
      len = ONIGENC_CODE_TO_MBCLEN(env->enc, v);
      if (len < 0) {
	r = len;
	goto err;
      }
      in_type = (len == 1 ? CCV_SB : CCV_CODE_POINT);
    val_entry2:
      r = next_state_val(cc, &vs, v, &val_israw, in_israw, in_type, &val_type,
			 &state, env);
      if (r != 0) goto err;
      break;

    case TK_POSIX_BRACKET_OPEN:
      r = parse_posix_bracket(cc, &p, end, env);
      if (r < 0) goto err;
      if (r == 1) {  /* is not POSIX bracket */
	CC_ESC_WARN(env, (UChar* )"[");
	p = tok->backp;
	v = (OnigCodePoint )tok->u.c;
	in_israw = 0;
	goto val_entry;
      }
      goto next_class;
      break;

    case TK_CHAR_TYPE:
      {
	int ctype, not;
	ctype = parse_ctype_to_enc_ctype(tok->u.subtype, &not);
	r = add_ctype_to_cc(cc, ctype, not, env);
	if (r != 0) return r;
      }

    next_class:
      r = next_state_class(cc, &vs, &val_type, &state, env);
      if (r != 0) goto err;
      break;

    case TK_CHAR_PROPERTY:
      {
	int ctype;

	ctype = fetch_char_property_to_ctype(&p, end, env);
	if (ctype < 0) return ctype;
	r = add_ctype_to_cc(cc, ctype, tok->u.prop.not, env);
	if (r != 0) return r;
	goto next_class;
      }
      break;

    case TK_CC_RANGE:
      if (state == CCS_VALUE) {
	r = fetch_token_in_cc(tok, &p, end, env);
	if (r < 0) goto err;
	fetched = 1;
	if (r == TK_CC_CLOSE) { /* allow [x-] */
	range_end_val:
	  v = (OnigCodePoint )'-';
	  in_israw = 0;
	  goto val_entry;
	}
	else if (r == TK_CC_AND) {
	  CC_ESC_WARN(env, (UChar* )"-");
	  goto range_end_val;
	}
	state = CCS_RANGE;
      }
      else if (state == CCS_START) {
	/* [-xa] is allowed */
	v = (OnigCodePoint )tok->u.c;
	in_israw = 0;

	r = fetch_token_in_cc(tok, &p, end, env);
	if (r < 0) goto err;
	fetched = 1;
	/* [--x] or [a&&-x] is warned. */
	if (r == TK_CC_RANGE || and_start != 0)
	  CC_ESC_WARN(env, (UChar* )"-");

	goto val_entry;
      }
      else if (state == CCS_RANGE) {
	CC_ESC_WARN(env, (UChar* )"-");
	goto sb_char;  /* [!--x] is allowed */
      }
      else { /* CCS_COMPLETE */
	r = fetch_token_in_cc(tok, &p, end, env);
	if (r < 0) goto err;
	fetched = 1;
	if (r == TK_CC_CLOSE) goto range_end_val; /* allow [a-b-] */
	else if (r == TK_CC_AND) {
	  CC_ESC_WARN(env, (UChar* )"-");
	  goto range_end_val;
	}
	
	if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_DOUBLE_RANGE_OP_IN_CC)) {
	  CC_ESC_WARN(env, (UChar* )"-");
	  goto sb_char;   /* [0-9-a] is allowed as [0-9\-a] */
	}
	r = ONIGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS;
	goto err;
      }
      break;

    case TK_CC_CC_OPEN: /* [ */
      {
	Node *anode;
	CClassNode* acc;

	r = parse_char_class(&anode, tok, &p, end, env);
	if (r != 0) goto cc_open_err;
	acc = &(NCCLASS(anode));
	r = or_cclass(cc, acc, env->enc);

	onig_node_free(anode);
      cc_open_err:
	if (r != 0) goto err;
      }
      break;

    case TK_CC_AND: /* && */
      {
	if (state == CCS_VALUE) {
	  r = next_state_val(cc, &vs, 0, &val_israw, 0, val_type,
			     &val_type, &state, env);
	  if (r != 0) goto err;
	}
	/* initialize local variables */
	and_start = 1;
	state = CCS_START;

	if (IS_NOT_NULL(prev_cc)) {
	  r = and_cclass(prev_cc, cc, env->enc);
	  if (r != 0) goto err;
	  bbuf_free(cc->mbuf);
	}
	else {
	  prev_cc = cc;
	  cc = &work_cc;
	}
	initialize_cclass(cc);
      }
      break;

    case TK_EOT:
      r = ONIGERR_PREMATURE_END_OF_CHAR_CLASS;
      goto err;
      break;
    default:
      r = ONIGERR_PARSER_BUG;
      goto err;
      break;
    }

    if (fetched)
      r = tok->type;
    else {
      r = fetch_token_in_cc(tok, &p, end, env);
      if (r < 0) goto err;
    }
  }

  if (state == CCS_VALUE) {
    r = next_state_val(cc, &vs, 0, &val_israw, 0, val_type,
		       &val_type, &state, env);
    if (r != 0) goto err;
  }

  if (IS_NOT_NULL(prev_cc)) {
    r = and_cclass(prev_cc, cc, env->enc);
    if (r != 0) goto err;
    bbuf_free(cc->mbuf);
    cc = prev_cc;
  }

  if (neg != 0)
    CCLASS_SET_NOT(cc);
  else
    CCLASS_CLEAR_NOT(cc);
  if (IS_CCLASS_NOT(cc) &&
      IS_SYNTAX_BV(env->syntax, ONIG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC)) {
    int is_empty;

    is_empty = (IS_NULL(cc->mbuf) ? 1 : 0);
    if (is_empty != 0)
      BITSET_IS_EMPTY(cc->bs, is_empty);

    if (is_empty == 0) {
#define NEWLINE_CODE    0x0a

      if (ONIGENC_IS_CODE_NEWLINE(env->enc, NEWLINE_CODE)) {
        if (ONIGENC_CODE_TO_MBCLEN(env->enc, NEWLINE_CODE) == 1)
          BITSET_SET_BIT(cc->bs, NEWLINE_CODE);
        else
          add_code_range(&(cc->mbuf), env, NEWLINE_CODE, NEWLINE_CODE);
      }
    }
  }
  *src = p;
  return 0;

 err:
  if (cc != &(NCCLASS(*np)))
    bbuf_free(cc->mbuf);
  onig_node_free(*np);
  return r;
}

static int parse_subexp(Node** top, OnigToken* tok, int term,
			UChar** src, UChar* end, ScanEnv* env);

static int
parse_effect(Node** np, OnigToken* tok, int term, UChar** src, UChar* end,
	     ScanEnv* env)
{
  int r, num;
  int list_capture;
  Node *target;
  OnigOptionType option;
  OnigEncoding enc = env->enc;
  OnigCodePoint c;
  UChar* p = *src;
  PFETCH_READY;

  *np = NULL;
  if (PEND) return ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS;

  option = env->option;
  if (PPEEK_IS('?') &&
      IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_GROUP_EFFECT)) {
    PINC;
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

    PFETCH(c);
    switch (c) {
    case ':':   /* (?:...) grouping only */
    group:
      r = fetch_token(tok, &p, end, env);
      if (r < 0) return r;
      r = parse_subexp(np, tok, term, &p, end, env);
      if (r < 0) return r;
      *src = p;
      return 1; /* group */
      break;

    case '=':
      *np = onig_node_new_anchor(ANCHOR_PREC_READ);
      break;
    case '!':  /*         preceding read */
      *np = onig_node_new_anchor(ANCHOR_PREC_READ_NOT);
      break;
    case '>':            /* (?>...) stop backtrack */
      *np = node_new_effect(EFFECT_STOP_BACKTRACK);
      break;

    case '<':   /* look behind (?<=...), (?<!...) */
      PFETCH(c);
      if (c == '=')
	*np = onig_node_new_anchor(ANCHOR_LOOK_BEHIND);
      else if (c == '!')
	*np = onig_node_new_anchor(ANCHOR_LOOK_BEHIND_NOT);
#ifdef USE_NAMED_GROUP
      else if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP)) {
	UChar *name;
	UChar *name_end;

	PUNFETCH;
	list_capture = 0;

      named_group:
	name = p;
	r = fetch_name(&p, end, &name_end, env, 0);
	if (r < 0) return r;

	num = scan_env_add_mem_entry(env);
	if (num < 0) return num;
	if (list_capture != 0 && num >= BIT_STATUS_BITS_NUM)
	  return ONIGERR_GROUP_NUMBER_OVER_FOR_CAPTURE_HISTORY;

	r = name_add(env->reg, name, name_end, num, env);
	if (r != 0) return r;
	*np = node_new_effect_memory(env->option, 1);
	CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
	NEFFECT(*np).regnum = num;
	if (list_capture != 0)
	  BIT_STATUS_ON_AT_SIMPLE(env->capture_history, num);
	env->num_named++;
      }
#endif
      else
	return ONIGERR_UNDEFINED_GROUP_OPTION;
      break;

    case '@':
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ATMARK_CAPTURE_HISTORY)) {
#ifdef USE_NAMED_GROUP
	if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP)) {
	  PFETCH(c);
	  if (c == '<') {
	    list_capture = 1;
	    goto named_group; /* (?@<name>...) */
	  }
	  PUNFETCH;
	}
#endif
	*np = node_new_effect_memory(env->option, 0);
	CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
	num = scan_env_add_mem_entry(env);
	if (num < 0) {
	  onig_node_free(*np);
	  return num;
	}
	else if (num >= BIT_STATUS_BITS_NUM) {
	  onig_node_free(*np);
	  return ONIGERR_GROUP_NUMBER_OVER_FOR_CAPTURE_HISTORY;
	}
	NEFFECT(*np).regnum = num;
	BIT_STATUS_ON_AT_SIMPLE(env->capture_history, num);
      }
      else {
	return ONIGERR_UNDEFINED_GROUP_OPTION;
      }
      break;

#ifdef USE_POSIXLINE_OPTION
    case 'p':
#endif
    case '-': case 'i': case 'm': case 's': case 'x':
      {
	int neg = 0;

	while (1) {
	  switch (c) {
	  case ':':
	  case ')':
	  break;

	  case '-':  neg = 1; break;
	  case 'x':  ONOFF(option, ONIG_OPTION_EXTEND,     neg); break;
	  case 'i':  ONOFF(option, ONIG_OPTION_IGNORECASE, neg); break;
	  case 's':
	    if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_OPTION_PERL)) {
	      ONOFF(option, ONIG_OPTION_MULTILINE,  neg);
	    }
	    else
	      return ONIGERR_UNDEFINED_GROUP_OPTION;
	    break;

	  case 'm':
	    if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_OPTION_PERL)) {
	      ONOFF(option, ONIG_OPTION_SINGLELINE, (neg == 0 ? 1 : 0));
	    }
	    else if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_OPTION_RUBY)) {
	      ONOFF(option, ONIG_OPTION_MULTILINE,  neg);
	    }
	    else
	      return ONIGERR_UNDEFINED_GROUP_OPTION;
	    break;
#ifdef USE_POSIXLINE_OPTION
	  case 'p':
	    ONOFF(option, ONIG_OPTION_MULTILINE|ONIG_OPTION_SINGLELINE, neg);
	    break;
#endif
	  default:
	    return ONIGERR_UNDEFINED_GROUP_OPTION;
	  }

	  if (c == ')') {
	    *np = node_new_option(option);
	    CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
	    *src = p;
	    return 2; /* option only */
	  }
	  else if (c == ':') {
	    OnigOptionType prev = env->option;

	    env->option     = option;
	    r = fetch_token(tok, &p, end, env);
	    if (r < 0) return r;
	    r = parse_subexp(&target, tok, term, &p, end, env);
	    env->option = prev;
	    if (r < 0) return r;
	    *np = node_new_option(option);
	    CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
	    NEFFECT(*np).target = target;
	    *src = p;
	    return 0;
	  }

	  if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
	  PFETCH(c);
	}
      }
      break;

    default:
      return ONIGERR_UNDEFINED_GROUP_OPTION;
    }
  }
  else {
    if (ONIG_IS_OPTION_ON(env->option, ONIG_OPTION_DONT_CAPTURE_GROUP))
      goto group;

    *np = node_new_effect_memory(env->option, 0);
    CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
    num = scan_env_add_mem_entry(env);
    if (num < 0) return num;
    NEFFECT(*np).regnum = num;
  }

  CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
  r = fetch_token(tok, &p, end, env);
  if (r < 0) return r;
  r = parse_subexp(&target, tok, term, &p, end, env);
  if (r < 0) return r;

  if (NTYPE(*np) == N_ANCHOR)
    NANCHOR(*np).target = target;
  else {
    NEFFECT(*np).target = target;
    if (NEFFECT(*np).type == EFFECT_MEMORY) {
      /* Don't move this to previous of parse_subexp() */
      r = scan_env_set_mem_node(env, NEFFECT(*np).regnum, *np);
      if (r != 0) return r;
    }
  }

  *src = p;
  return 0;
}

static const char* PopularQStr[] = {
  "?", "*", "+", "??", "*?", "+?"
};

static const char* ReduceQStr[] = {
  "", "", "*", "*?", "??", "+ and ??", "+? and ?"
};

static int
set_quantifier(Node* qnode, Node* target, int group, ScanEnv* env)
{
  QuantifierNode* qn;

  qn = &(NQUANTIFIER(qnode));
  if (qn->lower == 1 && qn->upper == 1) {
    return 1;
  }

  switch (NTYPE(target)) {
  case N_STRING:
    if (! group) {
      StrNode* sn = &(NSTRING(target));
      if (str_node_can_be_split(sn, env->enc)) {
	Node* n = str_node_split_last_char(sn, env->enc);
	if (IS_NOT_NULL(n)) {
	  qn->target = n;
	  return 2;
	}
      }
    }
    break;

  case N_QUANTIFIER:
    { /* check redundant double repeat. */
      /* verbose warn (?:.?)? etc... but not warn (.?)? etc... */
      QuantifierNode* qnt = &(NQUANTIFIER(target));
      int nestq_num   = popular_quantifier_num(qn);
      int targetq_num = popular_quantifier_num(qnt);

#ifdef USE_WARNING_REDUNDANT_NESTED_REPEAT_OPERATOR
      if (!IS_QUANTIFIER_BY_NUMBER(qn) && !IS_QUANTIFIER_BY_NUMBER(qnt) &&
	  IS_SYNTAX_BV(env->syntax, ONIG_SYN_WARN_REDUNDANT_NESTED_REPEAT)) {
        UChar buf[WARN_BUFSIZE];

        switch(ReduceTypeTable[targetq_num][nestq_num]) {
        case RQ_ASIS:
          break;

        case RQ_DEL:
          if (onig_verb_warn != onig_null_warn) {
            onig_snprintf_with_pattern(buf, WARN_BUFSIZE, env->enc,
                                 env->pattern, env->pattern_end,
                                 (UChar* )"redundant nested repeat operator");
            (*onig_verb_warn)((char* )buf);
          }
          goto warn_exit;
          break;

        default:
          if (onig_verb_warn != onig_null_warn) {
            onig_snprintf_with_pattern(buf, WARN_BUFSIZE, env->enc,
                                       env->pattern, env->pattern_end,
            (UChar* )"nested repeat operator %s and %s was replaced with '%s'",
            PopularQStr[targetq_num], PopularQStr[nestq_num],
            ReduceQStr[ReduceTypeTable[targetq_num][nestq_num]]);
            (*onig_verb_warn)((char* )buf);
          }
          goto warn_exit;
          break;
        }
      }

    warn_exit:
#endif
      if (targetq_num >= 0) {
	if (nestq_num >= 0) {
	  onig_reduce_nested_quantifier(qnode, target);
	  goto q_exit;
	}
	else if (targetq_num == 1 || targetq_num == 2) { /* * or + */
	  /* (?:a*){n,m}, (?:a+){n,m} => (?:a*){n,n}, (?:a+){n,n} */
	  if (! IS_REPEAT_INFINITE(qn->upper) && qn->upper > 1 && qn->greedy) {
	    qn->upper = (qn->lower == 0 ? 1 : qn->lower);
	  }
	}
      }
    }
    break;

  default:
    break;
  }

  qn->target = target;
 q_exit:
  return 0;
}

#ifdef USE_SHARED_CCLASS_TABLE

#define THRESHOLD_RANGE_NUM_FOR_SHARE_CCLASS     8

/* for ctype node hash table */

typedef struct {
  OnigEncoding enc;
  int not;
  int type;
} type_cclass_key;

static int type_cclass_cmp(type_cclass_key* x, type_cclass_key* y)
{
  if (x->type != y->type) return 1;
  if (x->enc  != y->enc)  return 1;
  if (x->not  != y->not)  return 1;
  return 0;
}

static int type_cclass_hash(type_cclass_key* key)
{
  int i, val;
  unsigned char *p;

  val = 0;

  p = (unsigned char* )&(key->enc);
  for (i = 0; i < sizeof(key->enc); i++) {
    val = val * 997 + (int )*p++;
  }

  p = (unsigned char* )(&key->type);
  for (i = 0; i < sizeof(key->type); i++) {
    val = val * 997 + (int )*p++;
  }

  val += key->not;
  return val + (val >> 5);
}

static struct st_hash_type type_type_cclass_hash = {
    type_cclass_cmp,
    type_cclass_hash,
};

static st_table* OnigTypeCClassTable;


static int
i_free_shared_class(type_cclass_key* key, Node* node, void* arg)
{
  if (IS_NOT_NULL(node)) {
    CClassNode* cc = &(NCCLASS(node));
    if (IS_NOT_NULL(cc->mbuf)) xfree(cc->mbuf);
    xfree(node);
  }

  if (IS_NOT_NULL(key)) xfree(key);
  return ST_DELETE;
}

extern int
onig_free_shared_cclass_table(void)
{
  if (IS_NOT_NULL(OnigTypeCClassTable)) {
    onig_st_foreach(OnigTypeCClassTable, i_free_shared_class, 0);
    onig_st_free_table(OnigTypeCClassTable);
    OnigTypeCClassTable = NULL;
  }

  return 0;
}

#endif /* USE_SHARED_CCLASS_TABLE */


static int
parse_exp(Node** np, OnigToken* tok, int term,
	  UChar** src, UChar* end, ScanEnv* env)
{
  int r, len, group = 0;
  Node* qn;
  Node** targetp;

  *np = NULL;
  if (tok->type == term)
    goto end_of_token;

  switch (tok->type) {
  case TK_ALT:
  case TK_EOT:
  end_of_token:
  *np = node_new_empty();
  return tok->type;
  break;

  case TK_SUBEXP_OPEN:
    r = parse_effect(np, tok, TK_SUBEXP_CLOSE, src, end, env);
    if (r < 0) return r;
    if (r == 1) group = 1;
    else if (r == 2) { /* option only */
      Node* target;
      OnigOptionType prev = env->option;

      env->option = NEFFECT(*np).option;
      r = fetch_token(tok, src, end, env);
      if (r < 0) return r;
      r = parse_subexp(&target, tok, term, src, end, env);
      env->option = prev;
      if (r < 0) return r;
      NEFFECT(*np).target = target;	
      return tok->type;
    }
    break;

  case TK_SUBEXP_CLOSE:
    if (! IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_UNMATCHED_CLOSE_SUBEXP))
      return ONIGERR_UNMATCHED_CLOSE_PARENTHESIS;

    if (tok->escaped) goto tk_raw_byte;
    else goto tk_byte;
    break;

  case TK_STRING:
  tk_byte:
    {
      *np = node_new_str(tok->backp, *src);
      CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);

      while (1) {
	r = fetch_token(tok, src, end, env);
	if (r < 0) return r;
	if (r != TK_STRING) break;

	r = onig_node_str_cat(*np, tok->backp, *src);
	if (r < 0) return r;
      }

    string_end:
      targetp = np;
      goto repeat;
    }
    break;

  case TK_RAW_BYTE:
  tk_raw_byte:
    {
      *np = node_new_str_char((UChar )tok->u.c);
      CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
      len = 1;
      while (1) {
	if (len >= ONIGENC_MBC_MINLEN(env->enc)) {
	  if (len == enc_len(env->enc, NSTRING(*np).s)) {
	    r = fetch_token(tok, src, end, env);
	    goto string_end;
	  }
	}

	r = fetch_token(tok, src, end, env);
	if (r < 0) return r;
	if (r != TK_RAW_BYTE) {
#ifdef USE_PAD_TO_SHORT_BYTE_CHAR
	  int rem;
	  if (len < ONIGENC_MBC_MINLEN(env->enc)) {
	    rem = ONIGENC_MBC_MINLEN(env->enc) - len;
	    (void )node_str_head_pad(&NSTRING(*np), rem, (UChar )0);
	    if (len + rem == enc_len(env->enc, NSTRING(*np).s)) {
	      goto string_end;
	    }
	  }
#endif
	  return ONIGERR_TOO_SHORT_MULTI_BYTE_STRING;
	}

	r = node_str_cat_char(*np, (UChar )tok->u.c);
	if (r < 0) return r;

	len++;
      }
    }
    break;

  case TK_CODE_POINT:
    {
      UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
      int num = ONIGENC_CODE_TO_MBC(env->enc, tok->u.code, buf);
      if (num < 0) return num;
#ifdef NUMBERED_CHAR_IS_NOT_CASE_AMBIG
      *np = node_new_str_raw(buf, buf + num);
#else
      *np = node_new_str(buf, buf + num);
#endif
      CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
    }
    break;

  case TK_QUOTE_OPEN:
    {
      OnigCodePoint end_op[2];
      UChar *qstart, *qend, *nextp;

      end_op[0] = (OnigCodePoint )MC_ESC(env->enc);
      end_op[1] = (OnigCodePoint )'E';
      qstart = *src;
      qend = find_str_position(end_op, 2, qstart, end, &nextp, env->enc);
      if (IS_NULL(qend)) {
	nextp = qend = end;
      }
      *np = node_new_str(qstart, qend);
      CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
      *src = nextp;
    }
    break;

  case TK_CHAR_TYPE:
    {
      switch (tok->u.subtype) {
      case CTYPE_WORD:
      case CTYPE_NOT_WORD:
	*np = node_new_ctype(tok->u.subtype);
	CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
	break;

      case CTYPE_WHITE_SPACE:
      case CTYPE_NOT_WHITE_SPACE:
      case CTYPE_DIGIT:
      case CTYPE_NOT_DIGIT:
      case CTYPE_XDIGIT:
      case CTYPE_NOT_XDIGIT:
	{
	  CClassNode* cc;
	  int ctype, not;

#ifdef USE_SHARED_CCLASS_TABLE
          const OnigCodePoint *sbr, *mbr;

	  ctype = parse_ctype_to_enc_ctype(tok->u.subtype, &not);
          r = ONIGENC_GET_CTYPE_CODE_RANGE(env->enc, ctype, &sbr, &mbr);
          if (r == 0 &&
              ONIGENC_CODE_RANGE_NUM(mbr)
              >= THRESHOLD_RANGE_NUM_FOR_SHARE_CCLASS) {
            type_cclass_key  key;
            type_cclass_key* new_key;

            key.enc  = env->enc;
            key.not  = not;
            key.type = ctype;

            THREAD_ATOMIC_START;

            if (IS_NULL(OnigTypeCClassTable)) {
              OnigTypeCClassTable
                = onig_st_init_table_with_size(&type_type_cclass_hash, 10);
              if (IS_NULL(OnigTypeCClassTable)) {
                THREAD_ATOMIC_END;
                return ONIGERR_MEMORY;
              }
            }
            else {
              if (onig_st_lookup(OnigTypeCClassTable, (st_data_t )&key,
                                 (st_data_t* )np)) {
                THREAD_ATOMIC_END;
                break;
              }
            }

            *np = node_new_cclass_by_codepoint_range(not, sbr, mbr);
            if (IS_NULL(*np)) {
              THREAD_ATOMIC_END;
              return ONIGERR_MEMORY;
            }

            CCLASS_SET_SHARE(&(NCCLASS(*np)));
            new_key = (type_cclass_key* )xmalloc(sizeof(type_cclass_key));
            onig_st_add_direct(OnigTypeCClassTable, (st_data_t )new_key,
                               (st_data_t )*np);
            
            THREAD_ATOMIC_END;
          }
          else {
#endif
            ctype = parse_ctype_to_enc_ctype(tok->u.subtype, &not);
            *np = node_new_cclass();
            CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
            cc = &(NCCLASS(*np));
            add_ctype_to_cc(cc, ctype, 0, env);
            if (not != 0) CCLASS_SET_NOT(cc);
#ifdef USE_SHARED_CCLASS_TABLE
          }
#endif
	}
	break;

      default:
	return ONIGERR_PARSER_BUG;
	break;
      }
    }
    break;

  case TK_CHAR_PROPERTY:
    r = parse_char_property(np, tok, src, end, env);
    if (r != 0) return r;
    break;

  case TK_CC_OPEN:
    {
      CClassNode* cc;

      r = parse_char_class(np, tok, src, end, env);
      if (r != 0) return r;

      cc = &(NCCLASS(*np));

      if (IS_IGNORECASE(env->option)) {
        int i, n, in_cc;
        const OnigPairAmbigCodes* ccs;
        BitSetRef bs = cc->bs;
        OnigAmbigType amb;

        for (amb = 0x01; amb <= ONIGENC_AMBIGUOUS_MATCH_LIMIT; amb <<= 1) {
          if ((amb & env->ambig_flag) == 0)  continue;

          n = ONIGENC_GET_ALL_PAIR_AMBIG_CODES(env->enc, amb, &ccs);
          for (i = 0; i < n; i++) {
            in_cc = onig_is_code_in_cc(env->enc, ccs[i].from, cc);

            if ((in_cc != 0 && !IS_CCLASS_NOT(cc)) ||
                (in_cc == 0 && IS_CCLASS_NOT(cc))) {
              if (ONIGENC_MBC_MINLEN(env->enc) > 1 ||
                  ccs[i].from >= SINGLE_BYTE_SIZE) {
                /* if (cc->not) clear_not_flag_cclass(cc, env->enc); */
                add_code_range(&(cc->mbuf), env, ccs[i].to, ccs[i].to);
              }
              else {
                if (BITSET_AT(bs, ccs[i].from)) {
                  /* /(?i:[^A-C])/.match("a") ==> fail. */
                  BITSET_SET_BIT(bs, ccs[i].to);
                }
                if (BITSET_AT(bs, ccs[i].to)) {
                  BITSET_SET_BIT(bs, ccs[i].from);
                }
              }
            }
          }
        }
      }
    }
    break;

  case TK_ANYCHAR:
    *np = node_new_anychar();
    CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
    break;

  case TK_ANYCHAR_ANYTIME:
    *np = node_new_anychar();
    CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
    qn = node_new_quantifier(0, REPEAT_INFINITE, 0);
    CHECK_NULL_RETURN_VAL(qn, ONIGERR_MEMORY);
    NQUANTIFIER(qn).target = *np;
    *np = qn;
    break;

  case TK_BACKREF:
    len = tok->u.backref.num;
    *np = node_new_backref(len,
		   (len > 1 ? tok->u.backref.refs : &(tok->u.backref.ref1)),
			   tok->u.backref.by_name,
#ifdef USE_BACKREF_AT_LEVEL
			   tok->u.backref.exist_level,
			   tok->u.backref.level,
#endif
			   env);
    CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
    break;

#ifdef USE_SUBEXP_CALL
  case TK_CALL:
    *np = node_new_call(tok->u.call.name, tok->u.call.name_end);
    CHECK_NULL_RETURN_VAL(*np, ONIGERR_MEMORY);
    env->num_call++;
    break;
#endif

  case TK_ANCHOR:
    *np = onig_node_new_anchor(tok->u.anchor);
    break;

  case TK_OP_REPEAT:
  case TK_INTERVAL:
    if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_CONTEXT_INDEP_REPEAT_OPS)) {
      if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_CONTEXT_INVALID_REPEAT_OPS))
	return ONIGERR_TARGET_OF_REPEAT_OPERATOR_NOT_SPECIFIED;
      else
	*np = node_new_empty();
    }
    else {
      goto tk_byte;
    }
    break;

  default:
    return ONIGERR_PARSER_BUG;
    break;
  }

  {
    targetp = np;

  re_entry:
    r = fetch_token(tok, src, end, env);
    if (r < 0) return r;

  repeat:
    if (r == TK_OP_REPEAT || r == TK_INTERVAL) {
      if (is_invalid_quantifier_target(*targetp))
	return ONIGERR_TARGET_OF_REPEAT_OPERATOR_INVALID;

      qn = node_new_quantifier(tok->u.repeat.lower, tok->u.repeat.upper,
			      (r == TK_INTERVAL ? 1 : 0));
      CHECK_NULL_RETURN_VAL(qn, ONIGERR_MEMORY);
      NQUANTIFIER(qn).greedy = tok->u.repeat.greedy;
      r = set_quantifier(qn, *targetp, group, env);
      if (r < 0) return r;
      
      if (tok->u.repeat.possessive != 0) {
	Node* en;
	en = node_new_effect(EFFECT_STOP_BACKTRACK);
	CHECK_NULL_RETURN_VAL(en, ONIGERR_MEMORY);
	NEFFECT(en).target = qn;
	qn = en;
      }

      if (r == 0) {
	*targetp = qn;
      }
      else if (r == 2) { /* split case: /abc+/ */
	Node *tmp;

	*targetp = node_new_list(*targetp, NULL);
	CHECK_NULL_RETURN_VAL(*targetp, ONIGERR_MEMORY);
	tmp = NCONS(*targetp).right = node_new_list(qn, NULL);
	CHECK_NULL_RETURN_VAL(tmp, ONIGERR_MEMORY);
	targetp = &(NCONS(tmp).left);
      }
      goto re_entry;
    }
  }

  return r;
}

static int
parse_branch(Node** top, OnigToken* tok, int term,
	     UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  Node *node, **headp;

  *top = NULL;
  r = parse_exp(&node, tok, term, src, end, env);
  if (r < 0) return r;

  if (r == TK_EOT || r == term || r == TK_ALT) {
    *top = node;
  }
  else {
    *top  = node_new_list(node, NULL);
    headp = &(NCONS(*top).right);
    while (r != TK_EOT && r != term && r != TK_ALT) {
      r = parse_exp(&node, tok, term, src, end, env);
      if (r < 0) return r;

      if (NTYPE(node) == N_LIST) {
	*headp = node;
	while (IS_NOT_NULL(NCONS(node).right)) node = NCONS(node).right;
	headp = &(NCONS(node).right);
      }
      else {
	*headp = node_new_list(node, NULL);
	headp = &(NCONS(*headp).right);
      }
    }
  }

  return r;
}

/* term_tok: TK_EOT or TK_SUBEXP_CLOSE */
static int
parse_subexp(Node** top, OnigToken* tok, int term,
	     UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  Node *node, **headp;

  *top = NULL;
  r = parse_branch(&node, tok, term, src, end, env);
  if (r < 0) {
    onig_node_free(node);
    return r;
  }

  if (r == term) {
    *top = node;
  }
  else if (r == TK_ALT) {
    *top  = node_new_alt(node, NULL);
    headp = &(NCONS(*top).right);
    while (r == TK_ALT) {
      r = fetch_token(tok, src, end, env);
      if (r < 0) return r;
      r = parse_branch(&node, tok, term, src, end, env);
      if (r < 0) return r;

      *headp = node_new_alt(node, NULL);
      headp = &(NCONS(*headp).right);
    }

    if (tok->type != term)
      goto err;
  }
  else {
  err:
    if (term == TK_SUBEXP_CLOSE)
      return ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS;
    else
      return ONIGERR_PARSER_BUG;
  }

  return r;
}

static int
parse_regexp(Node** top, UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  OnigToken tok;

  r = fetch_token(&tok, src, end, env);
  if (r < 0) return r;
  r = parse_subexp(top, &tok, TK_EOT, src, end, env);
  if (r < 0) return r;
  return 0;
}

extern int
onig_parse_make_tree(Node** root, const UChar* pattern, const UChar* end, regex_t* reg,
		      ScanEnv* env)
{
  int r;
  UChar* p;

#ifdef USE_NAMED_GROUP
  names_clear(reg);
#endif

  scan_env_clear(env);
  env->option      = reg->options;
  env->ambig_flag  = reg->ambig_flag;
  env->enc         = reg->enc;
  env->syntax      = reg->syntax;
  env->pattern     = (UChar* )pattern;
  env->pattern_end = (UChar* )end;
  env->reg         = reg;

  *root = NULL;
  p = (UChar* )pattern;
  r = parse_regexp(root, &p, (UChar* )end, env);
  reg->num_mem = env->num_mem;
  return r;
}

extern void
onig_scan_env_set_error_string(ScanEnv* env, int ecode,
				UChar* arg, UChar* arg_end)
{
  env->error     = arg;
  env->error_end = arg_end;
}
