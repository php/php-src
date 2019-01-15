/**********************************************************************
  regparse.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2018  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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
#include "st.h"

#ifdef DEBUG_NODE_FREE
#include <stdio.h>
#endif

#define INIT_TAG_NAMES_ALLOC_NUM   5

#define WARN_BUFSIZE    256

#define CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS

#define IS_ALLOWED_CODE_IN_CALLOUT_NAME(c) \
  ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' /* || c == '!' */)
#define IS_ALLOWED_CODE_IN_CALLOUT_TAG_NAME(c) \
  ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')


OnigSyntaxType OnigSyntaxOniguruma = {
  (( SYN_GNU_REGEX_OP | ONIG_SYN_OP_QMARK_NON_GREEDY |
     ONIG_SYN_OP_ESC_OCTAL3 | ONIG_SYN_OP_ESC_X_HEX2 |
     ONIG_SYN_OP_ESC_X_BRACE_HEX8 | ONIG_SYN_OP_ESC_O_BRACE_OCTAL |
     ONIG_SYN_OP_ESC_CONTROL_CHARS |
     ONIG_SYN_OP_ESC_C_CONTROL )
   & ~ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END )
  , ( ONIG_SYN_OP2_QMARK_GROUP_EFFECT |
      ONIG_SYN_OP2_OPTION_RUBY |
      ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP | ONIG_SYN_OP2_ESC_K_NAMED_BACKREF |
      ONIG_SYN_OP2_QMARK_LPAREN_IF_ELSE |
      ONIG_SYN_OP2_QMARK_TILDE_ABSENT_GROUP |
      ONIG_SYN_OP2_QMARK_BRACE_CALLOUT_CONTENTS |
      ONIG_SYN_OP2_ASTERISK_CALLOUT_NAME    |
      ONIG_SYN_OP2_ESC_X_Y_GRAPHEME_CLUSTER |
      ONIG_SYN_OP2_ESC_CAPITAL_R_GENERAL_NEWLINE |
      ONIG_SYN_OP2_ESC_CAPITAL_N_O_SUPER_DOT |
      ONIG_SYN_OP2_ESC_CAPITAL_K_KEEP |
      ONIG_SYN_OP2_ESC_G_SUBEXP_CALL |
      ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY  |
      ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT |
      ONIG_SYN_OP2_PLUS_POSSESSIVE_REPEAT |
      ONIG_SYN_OP2_CCLASS_SET_OP | ONIG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL |
      ONIG_SYN_OP2_ESC_CAPITAL_M_BAR_META | ONIG_SYN_OP2_ESC_V_VTAB |
      ONIG_SYN_OP2_ESC_H_XDIGIT | ONIG_SYN_OP2_ESC_U_HEX4 )
  , ( SYN_GNU_REGEX_BV | 
      ONIG_SYN_ALLOW_INTERVAL_LOW_ABBREV |
      ONIG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND |
      ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP |
      ONIG_SYN_ALLOW_MULTIPLEX_DEFINITION_NAME |
      ONIG_SYN_FIXED_INTERVAL_IS_GREEDY_ONLY |
      ONIG_SYN_WARN_CC_OP_NOT_ESCAPED |
      ONIG_SYN_WARN_REDUNDANT_NESTED_REPEAT )
  , ONIG_OPTION_NONE
  ,
  {
      (OnigCodePoint )'\\'                       /* esc */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar '.'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anytime '*'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* zero or one time '?' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* one or more time '+' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar anytime */
  }
};

OnigSyntaxType OnigSyntaxRuby = {
  (( SYN_GNU_REGEX_OP | ONIG_SYN_OP_QMARK_NON_GREEDY |
     ONIG_SYN_OP_ESC_OCTAL3 | ONIG_SYN_OP_ESC_X_HEX2 |
     ONIG_SYN_OP_ESC_X_BRACE_HEX8 | ONIG_SYN_OP_ESC_O_BRACE_OCTAL |
     ONIG_SYN_OP_ESC_CONTROL_CHARS |
     ONIG_SYN_OP_ESC_C_CONTROL )
   & ~ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END )
  , ( ONIG_SYN_OP2_QMARK_GROUP_EFFECT |
      ONIG_SYN_OP2_OPTION_RUBY |
      ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP | ONIG_SYN_OP2_ESC_K_NAMED_BACKREF |
      ONIG_SYN_OP2_QMARK_LPAREN_IF_ELSE |
      ONIG_SYN_OP2_QMARK_TILDE_ABSENT_GROUP |
      ONIG_SYN_OP2_ESC_X_Y_GRAPHEME_CLUSTER |
      ONIG_SYN_OP2_ESC_CAPITAL_R_GENERAL_NEWLINE |
      ONIG_SYN_OP2_ESC_CAPITAL_K_KEEP |
      ONIG_SYN_OP2_ESC_G_SUBEXP_CALL |
      ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY  |
      ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT |
      ONIG_SYN_OP2_PLUS_POSSESSIVE_REPEAT |
      ONIG_SYN_OP2_CCLASS_SET_OP | ONIG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL |
      ONIG_SYN_OP2_ESC_CAPITAL_M_BAR_META | ONIG_SYN_OP2_ESC_V_VTAB |
      ONIG_SYN_OP2_ESC_H_XDIGIT | ONIG_SYN_OP2_ESC_U_HEX4 )
  , ( SYN_GNU_REGEX_BV | 
      ONIG_SYN_ALLOW_INTERVAL_LOW_ABBREV |
      ONIG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND |
      ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP |
      ONIG_SYN_ALLOW_MULTIPLEX_DEFINITION_NAME |
      ONIG_SYN_FIXED_INTERVAL_IS_GREEDY_ONLY |
      ONIG_SYN_WARN_CC_OP_NOT_ESCAPED |
      ONIG_SYN_WARN_REDUNDANT_NESTED_REPEAT )
  , ONIG_OPTION_NONE
  ,
  {
      (OnigCodePoint )'\\'                       /* esc */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar '.'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anytime '*'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* zero or one time '?' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* one or more time '+' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar anytime */
  }
};

OnigSyntaxType*  OnigDefaultSyntax = ONIG_SYNTAX_ONIGURUMA;

extern void onig_null_warn(const char* s ARG_UNUSED) { }

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

extern void
onig_warning(const char* s)
{
  if (onig_warn == onig_null_warn) return ;

  (*onig_warn)(s);
}

#define DEFAULT_MAX_CAPTURE_NUM   32767

static int MaxCaptureNum = DEFAULT_MAX_CAPTURE_NUM;

extern int
onig_set_capture_num_limit(int num)
{
  if (num < 0) return -1;

  MaxCaptureNum = num;
  return 0;
}

static unsigned int ParseDepthLimit = DEFAULT_PARSE_DEPTH_LIMIT;

extern unsigned int
onig_get_parse_depth_limit(void)
{
  return ParseDepthLimit;
}

extern int
onig_set_parse_depth_limit(unsigned int depth)
{
  if (depth == 0)
    ParseDepthLimit = DEFAULT_PARSE_DEPTH_LIMIT;
  else
    ParseDepthLimit = depth;
  return 0;
}

static int
positive_int_multiply(int x, int y)
{
  if (x == 0 || y == 0) return 0;

  if (x < INT_MAX / y)
    return x * y;
  else
    return -1;
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
  CHECK_NULL_RETURN_MEMERR(to);
  r = BB_INIT(to, from->alloc);
  if (r != 0) {
    xfree(to->p);
    *rto = 0;
    return r;
  }
  to->used = from->used;
  xmemcpy(to->p, from->p, from->used);
  return 0;
}

static int backref_rel_to_abs(int rel_no, ScanEnv* env)
{
  if (rel_no > 0) {
    return env->num_mem + rel_no;
  }
  else {
    return env->num_mem + 1 + rel_no;
  }
}

#define OPTION_ON(v,f)     ((v) |= (f))
#define OPTION_OFF(v,f)    ((v) &= ~(f))

#define OPTION_NEGATE(v,f,negative)    (negative) ? ((v) &= ~(f)) : ((v) |= (f))

#define MBCODE_START_POS(enc) \
  (OnigCodePoint )(ONIGENC_MBC_MINLEN(enc) > 1 ? 0 : 0x80)

#define SET_ALL_MULTI_BYTE_RANGE(enc, pbuf) \
  add_code_range_to_buf(pbuf, MBCODE_START_POS(enc), ~((OnigCodePoint )0))

#define ADD_ALL_MULTI_BYTE_RANGE(enc, mbuf) do {\
  if (! ONIGENC_IS_SINGLEBYTE(enc)) {\
    r = SET_ALL_MULTI_BYTE_RANGE(enc, &(mbuf));\
    if (r != 0) return r;\
  }\
} while (0)


#define BITSET_IS_EMPTY(bs,empty) do {\
  int i;\
  empty = 1;\
  for (i = 0; i < (int )BITSET_SIZE; i++) {\
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
  for (i = 0; i < BITSET_SIZE; i++) { bs[i] = ~((Bits )0); }
}
#endif

static void
bitset_invert(BitSetRef bs)
{
  int i;
  for (i = 0; i < (int )BITSET_SIZE; i++) { bs[i] = ~(bs[i]); }
}

static void
bitset_invert_to(BitSetRef from, BitSetRef to)
{
  int i;
  for (i = 0; i < (int )BITSET_SIZE; i++) { to[i] = ~(from[i]); }
}

static void
bitset_and(BitSetRef dest, BitSetRef bs)
{
  int i;
  for (i = 0; i < (int )BITSET_SIZE; i++) { dest[i] &= bs[i]; }
}

static void
bitset_or(BitSetRef dest, BitSetRef bs)
{
  int i;
  for (i = 0; i < (int )BITSET_SIZE; i++) { dest[i] |= bs[i]; }
}

static void
bitset_copy(BitSetRef dest, BitSetRef bs)
{
  int i;
  for (i = 0; i < (int )BITSET_SIZE; i++) { dest[i] = bs[i]; }
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

extern void
onig_strcpy(UChar* dest, const UChar* src, const UChar* end)
{
  int len = (int )(end - src);
  if (len > 0) {
    xmemcpy(dest, src, len);
    dest[len] = (UChar )0;
  }
}

static int
save_entry(ScanEnv* env, enum SaveType type, int* id)
{
  int nid = env->save_num;

#if 0
  if (IS_NULL(env->saves)) {
    int n = 10;
    env->saves = (SaveItem* )xmalloc(sizeof(SaveItem) * n);
    CHECK_NULL_RETURN_MEMERR(env->saves);
    env->save_alloc_num = n;
  }
  else if (env->save_alloc_num <= nid) {
    int n = env->save_alloc_num * 2;
    SaveItem* p = (SaveItem* )xrealloc(env->saves, sizeof(SaveItem) * n);
    CHECK_NULL_RETURN_MEMERR(p);
    env->saves = p;
    env->save_alloc_num = n;
  }

  env->saves[nid].type = type;
#endif

  env->save_num++;
  *id = nid;
  return 0;
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
  if(UNEXPECTED(p > end)) p = end; \
} while (0)

#define PINC_S     do { \
  p += ONIGENC_MBC_ENC_LEN(enc, p); \
  if(UNEXPECTED(p > end)) p = end; \
} while (0)
#define PFETCH_S(c) do { \
  c = ONIGENC_MBC_TO_CODE(enc, p, end); \
  p += ONIGENC_MBC_ENC_LEN(enc, p); \
  if(UNEXPECTED(p > end)) p = end; \
} while (0)

#define PPEEK        (p < end ? ONIGENC_MBC_TO_CODE(enc, p, end) : PEND_VALUE)
#define PPEEK_IS(c)  (PPEEK == (OnigCodePoint )c)

static UChar*
strcat_capa(UChar* dest, UChar* dest_end, const UChar* src, const UChar* src_end,
            int capa)
{
  UChar* r;

  if (dest)
    r = (UChar* )xrealloc(dest, capa + 1);
  else
    r = (UChar* )xmalloc(capa + 1);

  CHECK_NULL_RETURN(r);
  onig_strcpy(r + (dest_end - dest), src, src_end);
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
  onig_strcpy(r, dest, dest_end);
  onig_strcpy(r + (dest_end - dest), src, src_end);
  return r;
}


#ifdef USE_ST_LIBRARY

typedef struct {
  UChar* s;
  UChar* end;
} st_str_end_key;

static int
str_end_cmp(st_str_end_key* x, st_str_end_key* y)
{
  UChar *p, *q;
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
str_end_hash(st_str_end_key* x)
{
  UChar *p;
  int val = 0;

  p = x->s;
  while (p < x->end) {
    val = val * 997 + (int )*p++;
  }

  return val + (val >> 5);
}

extern hash_table_type*
onig_st_init_strend_table_with_size(int size)
{
  static struct st_hash_type hashType = {
    str_end_cmp,
    str_end_hash,
  };

  return (hash_table_type* )
           onig_st_init_table_with_size(&hashType, size);
}

extern int
onig_st_lookup_strend(hash_table_type* table, const UChar* str_key,
                      const UChar* end_key, hash_data_type *value)
{
  st_str_end_key key;

  key.s   = (UChar* )str_key;
  key.end = (UChar* )end_key;

  return onig_st_lookup(table, (st_data_t )(&key), value);
}

extern int
onig_st_insert_strend(hash_table_type* table, const UChar* str_key,
                      const UChar* end_key, hash_data_type value)
{
  st_str_end_key* key;
  int result;

  key = (st_str_end_key* )xmalloc(sizeof(st_str_end_key));
  CHECK_NULL_RETURN_MEMERR(key);

  key->s   = (UChar* )str_key;
  key->end = (UChar* )end_key;
  result = onig_st_insert(table, (st_data_t )key, value);
  if (result) {
    xfree(key);
  }
  return result;
}


typedef struct {
  OnigEncoding enc;
  int    type; /* callout type: single or not */
  UChar* s;
  UChar* end;
} st_callout_name_key;

static int
callout_name_table_cmp(st_callout_name_key* x, st_callout_name_key* y)
{
  UChar *p, *q;
  int c;

  if (x->enc  != y->enc)  return 1;
  if (x->type != y->type) return 1;
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
callout_name_table_hash(st_callout_name_key* x)
{
  UChar *p;
  int val = 0;

  p = x->s;
  while (p < x->end) {
    val = val * 997 + (int )*p++;
  }

  /* use intptr_t for escape warning in Windows */
  return val + (val >> 5) + ((intptr_t )x->enc & 0xffff) + x->type;
}

extern hash_table_type*
onig_st_init_callout_name_table_with_size(int size)
{
  static struct st_hash_type hashType = {
    callout_name_table_cmp,
    callout_name_table_hash,
  };

  return (hash_table_type* )
           onig_st_init_table_with_size(&hashType, size);
}

extern int
onig_st_lookup_callout_name_table(hash_table_type* table,
                                  OnigEncoding enc,
                                  int type,
                                  const UChar* str_key,
                                  const UChar* end_key,
                                  hash_data_type *value)
{
  st_callout_name_key key;

  key.enc  = enc;
  key.type = type;
  key.s    = (UChar* )str_key;
  key.end  = (UChar* )end_key;

  return onig_st_lookup(table, (st_data_t )(&key), value);
}

static int
st_insert_callout_name_table(hash_table_type* table,
                             OnigEncoding enc, int type,
                             UChar* str_key, UChar* end_key,
                             hash_data_type value)
{
  st_callout_name_key* key;
  int result;

  key = (st_callout_name_key* )xmalloc(sizeof(st_callout_name_key));
  CHECK_NULL_RETURN_MEMERR(key);

  /* key->s: don't duplicate, because str_key is duped in callout_name_entry() */
  key->enc  = enc;
  key->type = type;
  key->s    = str_key;
  key->end  = end_key;
  result = onig_st_insert(table, (st_data_t )key, value);
  if (result) {
    xfree(key);
  }
  return result;
}

#endif /* USE_ST_LIBRARY */


#define INIT_NAME_BACKREFS_ALLOC_NUM   8

typedef struct {
  UChar* name;
  int    name_len;   /* byte length */
  int    back_num;   /* number of backrefs */
  int    back_alloc;
  int    back_ref1;
  int*   back_refs;
} NameEntry;

#ifdef USE_ST_LIBRARY

#define INIT_NAMES_ALLOC_NUM    5

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
#endif /* ONIG_DEBUG */

static int
i_free_name_entry(UChar* key, NameEntry* e, void* arg ARG_UNUSED)
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
  if (r != 0) return r;

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
i_names(UChar* key ARG_UNUSED, NameEntry* e, INamesArg* arg)
{
  int r = (*(arg->func))(e->name,
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
  int (*func)(const UChar*, const UChar*,int,int*,regex_t*,void*), void* arg)
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
i_renumber_name(UChar* key ARG_UNUSED, NameEntry* e, GroupNumRemap* map)
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

#else  /* USE_ST_LIBRARY */

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
  if (r != 0) return r;

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
  int (*func)(const UChar*, const UChar*,int,int*,regex_t*,void*), void* arg)
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

#endif /* else USE_ST_LIBRARY */

static int
name_add(regex_t* reg, UChar* name, UChar* name_end, int backref, ScanEnv* env)
{
  int r;
  int alloc;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (name_end - name <= 0)
    return ONIGERR_EMPTY_GROUP_NAME;

  e = name_find(reg, name, name_end);
  if (IS_NULL(e)) {
#ifdef USE_ST_LIBRARY
    if (IS_NULL(t)) {
      t = onig_st_init_strend_table_with_size(INIT_NAMES_ALLOC_NUM);
      reg->name_table = (void* )t;
    }
    e = (NameEntry* )xmalloc(sizeof(NameEntry));
    CHECK_NULL_RETURN_MEMERR(e);

    e->name = onigenc_strdup(reg->enc, name, name_end);
    if (IS_NULL(e->name)) {
      xfree(e);  return ONIGERR_MEMORY;
    }
    r = onig_st_insert_strend(t, e->name, (e->name + (name_end - name)),
                              (HashDataType )e);
    if (r < 0) return r;

    e->name_len   = (int )(name_end - name);
    e->back_num   = 0;
    e->back_alloc = 0;
    e->back_refs  = (int* )NULL;

#else

    if (IS_NULL(t)) {
      alloc = INIT_NAMES_ALLOC_NUM;
      t = (NameTable* )xmalloc(sizeof(NameTable));
      CHECK_NULL_RETURN_MEMERR(t);
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
      CHECK_NULL_RETURN_MEMERR(t->e);
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
    e->name = onigenc_strdup(reg->enc, name, name_end);
    if (IS_NULL(e->name)) return ONIGERR_MEMORY;
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
      CHECK_NULL_RETURN_MEMERR(e->back_refs);
      e->back_alloc = alloc;
      e->back_refs[0] = e->back_ref1;
      e->back_refs[1] = backref;
    }
    else {
      if (e->back_num > e->back_alloc) {
        alloc = e->back_alloc * 2;
        e->back_refs = (int* )xrealloc(e->back_refs, sizeof(int) * alloc);
        CHECK_NULL_RETURN_MEMERR(e->back_refs);
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
  NameEntry* e = name_find(reg, name, name_end);

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

extern int
onig_noname_group_capture_is_active(regex_t* reg)
{
  if (ONIG_IS_OPTION_ON(reg->options, ONIG_OPTION_DONT_CAPTURE_GROUP))
    return 0;

  if (onig_number_of_names(reg) > 0 &&
      IS_SYNTAX_BV(reg->syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
      !ONIG_IS_OPTION_ON(reg->options, ONIG_OPTION_CAPTURE_GROUP)) {
    return 0;
  }

  return 1;
}

#ifdef USE_CALLOUT

typedef struct {
  OnigCalloutType type;
  int             in;
  OnigCalloutFunc start_func;
  OnigCalloutFunc end_func;
  int             arg_num;
  int             opt_arg_num;
  unsigned int    arg_types[ONIG_CALLOUT_MAX_ARGS_NUM];
  OnigValue       opt_defaults[ONIG_CALLOUT_MAX_ARGS_NUM];
  UChar*          name; /* reference to GlobalCalloutNameTable entry: e->name */
} CalloutNameListEntry;

typedef struct {
  int  n;
  int  alloc;
  CalloutNameListEntry* v;
} CalloutNameListType;

static CalloutNameListType* GlobalCalloutNameList;

static int
make_callout_func_list(CalloutNameListType** rs, int init_size)
{
  CalloutNameListType* s;
  CalloutNameListEntry* v;

  *rs = 0;

  s = xmalloc(sizeof(*s));
  if (IS_NULL(s)) return ONIGERR_MEMORY;

  v = (CalloutNameListEntry* )xmalloc(sizeof(CalloutNameListEntry) * init_size);
  if (IS_NULL(v)) {
    xfree(s);
    return ONIGERR_MEMORY;
  }

  s->n = 0;
  s->alloc = init_size;
  s->v = v;

  *rs = s;
  return ONIG_NORMAL;
}

static void
free_callout_func_list(CalloutNameListType* s)
{
  if (IS_NOT_NULL(s)) {
    if (IS_NOT_NULL(s->v)) {
      int i, j;

      for (i = 0; i < s->n; i++) {
        CalloutNameListEntry* e = s->v + i;
        for (j = e->arg_num - e->opt_arg_num; j < e->arg_num; j++) {
          if (e->arg_types[j] == ONIG_TYPE_STRING) {
            UChar* p = e->opt_defaults[j].s.start;
            if (IS_NOT_NULL(p)) xfree(p);
          }
        }
      }
      xfree(s->v);
    }
    xfree(s);
  }
}

static int
callout_func_list_add(CalloutNameListType* s, int* rid)
{
  if (s->n >= s->alloc) {
    int new_size = s->alloc * 2;
    CalloutNameListEntry* nv = (CalloutNameListEntry* )
      xrealloc(s->v, sizeof(CalloutNameListEntry) * new_size);
    if (IS_NULL(nv)) return ONIGERR_MEMORY;

    s->alloc = new_size;
    s->v = nv;
  }

  *rid = s->n;

  xmemset(&(s->v[s->n]), 0, sizeof(*(s->v)));
  s->n++;
  return ONIG_NORMAL;
}


typedef struct {
  UChar* name;
  int    name_len;   /* byte length */
  int    id;
} CalloutNameEntry;

#ifdef USE_ST_LIBRARY
typedef st_table  CalloutNameTable;
#else
typedef struct {
  CalloutNameEntry* e;
  int               num;
  int               alloc;
} CalloutNameTable;
#endif

static CalloutNameTable* GlobalCalloutNameTable;
static int CalloutNameIDCounter;

#ifdef USE_ST_LIBRARY

static int
i_free_callout_name_entry(st_callout_name_key* key, CalloutNameEntry* e,
                          void* arg ARG_UNUSED)
{
  xfree(e->name);
  /*xfree(key->s); */ /* is same as e->name */
  xfree(key);
  xfree(e);
  return ST_DELETE;
}

static int
callout_name_table_clear(CalloutNameTable* t)
{
  if (IS_NOT_NULL(t)) {
    onig_st_foreach(t, i_free_callout_name_entry, 0);
  }
  return 0;
}

static int
global_callout_name_table_free(void)
{
  if (IS_NOT_NULL(GlobalCalloutNameTable)) {
    int r = callout_name_table_clear(GlobalCalloutNameTable);
    if (r != 0) return r;

    onig_st_free_table(GlobalCalloutNameTable);
    GlobalCalloutNameTable = 0;
    CalloutNameIDCounter = 0;
  }

  return 0;
}

static CalloutNameEntry*
callout_name_find(OnigEncoding enc, int is_not_single,
                  const UChar* name, const UChar* name_end)
{
  int r;
  CalloutNameEntry* e;
  CalloutNameTable* t = GlobalCalloutNameTable;

  e = (CalloutNameEntry* )NULL;
  if (IS_NOT_NULL(t)) {
    r = onig_st_lookup_callout_name_table(t, enc, is_not_single, name, name_end,
                                          (HashDataType* )((void* )(&e)));
    if (r == 0) { /* not found */
      if (enc != ONIG_ENCODING_ASCII &&
          ONIGENC_IS_ASCII_COMPATIBLE_ENCODING(enc)) {
        enc = ONIG_ENCODING_ASCII;
        onig_st_lookup_callout_name_table(t, enc, is_not_single, name, name_end,
                                          (HashDataType* )((void* )(&e)));
      }
    }
  }
  return e;
}

#else

static int
callout_name_table_clear(CalloutNameTable* t)
{
  int i;
  CalloutNameEntry* e;

  if (IS_NOT_NULL(t)) {
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      if (IS_NOT_NULL(e->name)) {
        xfree(e->name);
        e->name     = NULL;
        e->name_len = 0;
        e->id       = 0;
        e->func     = 0;
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

static int
global_callout_name_table_free(void)
{
  if (IS_NOT_NULL(GlobalCalloutNameTable)) {
    int r = callout_name_table_clear(GlobalCalloutNameTable);
    if (r != 0) return r;

    xfree(GlobalCalloutNameTable);
    GlobalCalloutNameTable = 0;
    CalloutNameIDCounter = 0;
  }
  return 0;
}

static CalloutNameEntry*
callout_name_find(UChar* name, UChar* name_end)
{
  int i, len;
  CalloutNameEntry* e;
  CalloutNameTable* t = Calloutnames;

  if (IS_NOT_NULL(t)) {
    len = name_end - name;
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      if (len == e->name_len && onig_strncmp(name, e->name, len) == 0)
        return e;
    }
  }
  return (CalloutNameEntry* )NULL;
}

#endif

/* name string must be single byte char string. */
static int
callout_name_entry(CalloutNameEntry** rentry, OnigEncoding enc,
                   int is_not_single, UChar* name, UChar* name_end)
{
  int r;
  CalloutNameEntry* e;
  CalloutNameTable* t = GlobalCalloutNameTable;

  *rentry = 0;
  if (name_end - name <= 0)
    return ONIGERR_INVALID_CALLOUT_NAME;

  e = callout_name_find(enc, is_not_single, name, name_end);
  if (IS_NULL(e)) {
#ifdef USE_ST_LIBRARY
    if (IS_NULL(t)) {
      t = onig_st_init_callout_name_table_with_size(INIT_NAMES_ALLOC_NUM);
      GlobalCalloutNameTable = t;
    }
    e = (CalloutNameEntry* )xmalloc(sizeof(CalloutNameEntry));
    CHECK_NULL_RETURN_MEMERR(e);

    e->name = onigenc_strdup(enc, name, name_end);
    if (IS_NULL(e->name)) {
      xfree(e);  return ONIGERR_MEMORY;
    }

    r = st_insert_callout_name_table(t, enc, is_not_single,
                                     e->name, (e->name + (name_end - name)),
                                     (HashDataType )e);
    if (r < 0) return r;

#else

    int alloc;

    if (IS_NULL(t)) {
      alloc = INIT_NAMES_ALLOC_NUM;
      t = (CalloutNameTable* )xmalloc(sizeof(CalloutNameTable));
      CHECK_NULL_RETURN_MEMERR(t);
      t->e     = NULL;
      t->alloc = 0;
      t->num   = 0;

      t->e = (CalloutNameEntry* )xmalloc(sizeof(CalloutNameEntry) * alloc);
      if (IS_NULL(t->e)) {
        xfree(t);
        return ONIGERR_MEMORY;
      }
      t->alloc = alloc;
      GlobalCalloutNameTable = t;
      goto clear;
    }
    else if (t->num == t->alloc) {
      int i;

      alloc = t->alloc * 2;
      t->e = (CalloutNameEntry* )xrealloc(t->e, sizeof(CalloutNameEntry) * alloc);
      CHECK_NULL_RETURN_MEMERR(t->e);
      t->alloc = alloc;

    clear:
      for (i = t->num; i < t->alloc; i++) {
        t->e[i].name       = NULL;
        t->e[i].name_len   = 0;
        t->e[i].id         = 0;
      }
    }
    e = &(t->e[t->num]);
    t->num++;
    e->name = onigenc_strdup(enc, name, name_end);
    if (IS_NULL(e->name)) return ONIGERR_MEMORY;
#endif

    CalloutNameIDCounter++;
    e->id = CalloutNameIDCounter;
    e->name_len = (int )(name_end - name);
  }

  *rentry = e;
  return e->id;
}

static int
is_allowed_callout_name(OnigEncoding enc, UChar* name, UChar* name_end)
{
  UChar* p;
  OnigCodePoint c;

  if (name >= name_end) return 0;

  p = name;
  while (p < name_end) {
    c = ONIGENC_MBC_TO_CODE(enc, p, name_end);
    if (! IS_ALLOWED_CODE_IN_CALLOUT_NAME(c))
      return 0;

    if (p == name) {
      if (c >= '0' && c <= '9') return 0;
    }

    p += ONIGENC_MBC_ENC_LEN(enc, p);
  }

  return 1;
}

static int
is_allowed_callout_tag_name(OnigEncoding enc, UChar* name, UChar* name_end)
{
  UChar* p;
  OnigCodePoint c;

  if (name >= name_end) return 0;

  p = name;
  while (p < name_end) {
    c = ONIGENC_MBC_TO_CODE(enc, p, name_end);
    if (! IS_ALLOWED_CODE_IN_CALLOUT_TAG_NAME(c))
      return 0;

    if (p == name) {
      if (c >= '0' && c <= '9') return 0;
    }

    p += ONIGENC_MBC_ENC_LEN(enc, p);
  }

  return 1;
}

extern int
onig_set_callout_of_name(OnigEncoding enc, OnigCalloutType callout_type,
                         UChar* name, UChar* name_end, int in,
                         OnigCalloutFunc start_func,
                         OnigCalloutFunc end_func,
                         int arg_num, unsigned int arg_types[],
                         int opt_arg_num, OnigValue opt_defaults[])
{
  int r;
  int i;
  int j;
  int id;
  int is_not_single;
  CalloutNameEntry* e;
  CalloutNameListEntry* fe;

  if (callout_type != ONIG_CALLOUT_TYPE_SINGLE)
    return ONIGERR_INVALID_ARGUMENT;

  if (arg_num < 0 || arg_num > ONIG_CALLOUT_MAX_ARGS_NUM)
    return ONIGERR_INVALID_CALLOUT_ARG;

  if (opt_arg_num < 0 || opt_arg_num > arg_num)
    return ONIGERR_INVALID_CALLOUT_ARG;

  if (start_func == 0 && end_func == 0)
    return ONIGERR_INVALID_CALLOUT_ARG;

  if ((in & ONIG_CALLOUT_IN_PROGRESS) == 0 && (in & ONIG_CALLOUT_IN_RETRACTION) == 0)
    return ONIGERR_INVALID_CALLOUT_ARG;

  for (i = 0; i < arg_num; i++) {
    unsigned int t = arg_types[i];
    if (t == ONIG_TYPE_VOID)
      return ONIGERR_INVALID_CALLOUT_ARG;
    else {
      if (i >= arg_num - opt_arg_num) {
        if (t != ONIG_TYPE_LONG && t != ONIG_TYPE_CHAR && t != ONIG_TYPE_STRING &&
            t != ONIG_TYPE_TAG)
          return ONIGERR_INVALID_CALLOUT_ARG;
      }
      else {
        if (t != ONIG_TYPE_LONG) {
          t = t & ~ONIG_TYPE_LONG;
          if (t != ONIG_TYPE_CHAR && t != ONIG_TYPE_STRING && t != ONIG_TYPE_TAG)
            return ONIGERR_INVALID_CALLOUT_ARG;
        }
      }
    }
  }

  if (! is_allowed_callout_name(enc, name, name_end)) {
    return ONIGERR_INVALID_CALLOUT_NAME;
  }

  is_not_single = (callout_type != ONIG_CALLOUT_TYPE_SINGLE);
  id = callout_name_entry(&e, enc, is_not_single, name, name_end);
  if (id < 0) return id;

  r = ONIG_NORMAL;
  if (IS_NULL(GlobalCalloutNameList)) {
    r = make_callout_func_list(&GlobalCalloutNameList, 10);
    if (r != ONIG_NORMAL) return r;
  }

  while (id >= GlobalCalloutNameList->n) {
    int rid;
    r = callout_func_list_add(GlobalCalloutNameList, &rid);
    if (r != ONIG_NORMAL) return r;
  }

  fe = GlobalCalloutNameList->v + id;
  fe->type         = callout_type;
  fe->in           = in;
  fe->start_func   = start_func;
  fe->end_func     = end_func;
  fe->arg_num      = arg_num;
  fe->opt_arg_num  = opt_arg_num;
  fe->name         = e->name;

  for (i = 0; i < arg_num; i++) {
    fe->arg_types[i] = arg_types[i];
  }
  for (i = arg_num - opt_arg_num, j = 0; i < arg_num; i++, j++) {
    if (fe->arg_types[i] == ONIG_TYPE_STRING) {
      OnigValue* val = opt_defaults + j;
      UChar* ds = onigenc_strdup(enc, val->s.start, val->s.end);
      CHECK_NULL_RETURN_MEMERR(ds);

      fe->opt_defaults[i].s.start = ds;
      fe->opt_defaults[i].s.end   = ds + (val->s.end - val->s.start);
    }
    else {
      fe->opt_defaults[i] = opt_defaults[j];
    }
  }

  r = id;
  return r;
}

static int
get_callout_name_id_by_name(OnigEncoding enc, int is_not_single,
                            UChar* name, UChar* name_end, int* rid)
{
  int r;
  CalloutNameEntry* e;

  if (! is_allowed_callout_name(enc, name, name_end)) {
    return ONIGERR_INVALID_CALLOUT_NAME;
  }

  e = callout_name_find(enc, is_not_single, name, name_end);
  if (IS_NULL(e)) {
    return ONIGERR_UNDEFINED_CALLOUT_NAME;
  }

  r = ONIG_NORMAL;
  *rid = e->id;

  return r;
}

extern OnigCalloutFunc
onig_get_callout_start_func(regex_t* reg, int callout_num)
{
  /* If used for callouts of contents, return 0. */
  CalloutListEntry* e;

  e = onig_reg_callout_list_at(reg, callout_num);
  return e->start_func;
}

extern const UChar*
onig_get_callout_tag_start(regex_t* reg, int callout_num)
{
  CalloutListEntry* e = onig_reg_callout_list_at(reg, callout_num);
  return e->tag_start;
}

extern const UChar*
onig_get_callout_tag_end(regex_t* reg, int callout_num)
{
  CalloutListEntry* e = onig_reg_callout_list_at(reg, callout_num);
  return e->tag_end;
}


extern OnigCalloutType
onig_get_callout_type_by_name_id(int name_id)
{
  if (name_id < 0 || name_id >= GlobalCalloutNameList->n)
    return 0;

  return GlobalCalloutNameList->v[name_id].type;
}

extern OnigCalloutFunc
onig_get_callout_start_func_by_name_id(int name_id)
{
  if (name_id < 0 || name_id >= GlobalCalloutNameList->n)
    return 0;

  return GlobalCalloutNameList->v[name_id].start_func;
}

extern OnigCalloutFunc
onig_get_callout_end_func_by_name_id(int name_id)
{
  if (name_id < 0 || name_id >= GlobalCalloutNameList->n)
    return 0;

  return GlobalCalloutNameList->v[name_id].end_func;
}

extern int
onig_get_callout_in_by_name_id(int name_id)
{
  if (name_id < 0 || name_id >= GlobalCalloutNameList->n)
    return 0;

  return GlobalCalloutNameList->v[name_id].in;
}

static int
get_callout_arg_num_by_name_id(int name_id)
{
  return GlobalCalloutNameList->v[name_id].arg_num;
}

static int
get_callout_opt_arg_num_by_name_id(int name_id)
{
  return GlobalCalloutNameList->v[name_id].opt_arg_num;
}

static unsigned int
get_callout_arg_type_by_name_id(int name_id, int index)
{
  return GlobalCalloutNameList->v[name_id].arg_types[index];
}

static OnigValue
get_callout_opt_default_by_name_id(int name_id, int index)
{
  return GlobalCalloutNameList->v[name_id].opt_defaults[index];
}

extern UChar*
onig_get_callout_name_by_name_id(int name_id)
{
  if (name_id < 0 || name_id >= GlobalCalloutNameList->n)
    return 0;

  return GlobalCalloutNameList->v[name_id].name;
}

extern int
onig_global_callout_names_free(void)
{
  free_callout_func_list(GlobalCalloutNameList);
  GlobalCalloutNameList = 0;

  global_callout_name_table_free();
  return ONIG_NORMAL;
}


typedef st_table   CalloutTagTable;
typedef intptr_t   CalloutTagVal;

#define CALLOUT_TAG_LIST_FLAG_TAG_EXIST     (1<<0)

static int
i_callout_callout_list_set(UChar* key, CalloutTagVal e, void* arg)
{
  int num;
  RegexExt* ext = (RegexExt* )arg;

  num = (int )e - 1;
  ext->callout_list[num].flag |= CALLOUT_TAG_LIST_FLAG_TAG_EXIST;
  return ST_CONTINUE;
}

static int
setup_ext_callout_list_values(regex_t* reg)
{
  int i, j;
  RegexExt* ext;

  ext = REG_EXTP(reg);
  if (IS_NOT_NULL(ext->tag_table)) {
    onig_st_foreach((CalloutTagTable *)ext->tag_table, i_callout_callout_list_set,
                    (st_data_t )ext);
  }

  for (i = 0; i < ext->callout_num; i++) {
    CalloutListEntry* e = ext->callout_list + i;
    if (e->of == ONIG_CALLOUT_OF_NAME) {
      for (j = 0; j < e->u.arg.num; j++) {
        if (e->u.arg.types[j] == ONIG_TYPE_TAG) {
          UChar* start;
          UChar* end;
          int num;
          start = e->u.arg.vals[j].s.start;
          end   = e->u.arg.vals[j].s.end;
          num = onig_get_callout_num_by_tag(reg, start, end);
          if (num < 0) return num;
          e->u.arg.vals[j].tag = num;
        }
      }
    }
  }

  return ONIG_NORMAL;
}

extern int
onig_callout_tag_is_exist_at_callout_num(regex_t* reg, int callout_num)
{
  RegexExt* ext = REG_EXTP(reg);

  if (IS_NULL(ext) || IS_NULL(ext->callout_list)) return 0;
  if (callout_num > ext->callout_num) return 0;

  return (ext->callout_list[callout_num].flag &
          CALLOUT_TAG_LIST_FLAG_TAG_EXIST) != 0  ? 1 : 0;
}

static int
i_free_callout_tag_entry(UChar* key, CalloutTagVal e, void* arg ARG_UNUSED)
{
  xfree(key);
  return ST_DELETE;
}

static int
callout_tag_table_clear(CalloutTagTable* t)
{
  if (IS_NOT_NULL(t)) {
    onig_st_foreach(t, i_free_callout_tag_entry, 0);
  }
  return 0;
}

extern int
onig_callout_tag_table_free(void* table)
{
  CalloutTagTable* t = (CalloutTagTable* )table;

  if (IS_NOT_NULL(t)) {
    int r = callout_tag_table_clear(t);
    if (r != 0) return r;

    onig_st_free_table(t);
  }

  return 0;
}

extern int
onig_get_callout_num_by_tag(regex_t* reg,
                            const UChar* tag, const UChar* tag_end)
{
  int r;
  RegexExt* ext;
  CalloutTagVal e;

  ext = REG_EXTP(reg);
  if (IS_NULL(ext) || IS_NULL(ext->tag_table))
    return ONIGERR_INVALID_CALLOUT_TAG_NAME;

  r = onig_st_lookup_strend(ext->tag_table, tag, tag_end,
                            (HashDataType* )((void* )(&e)));
  if (r == 0) return ONIGERR_INVALID_CALLOUT_TAG_NAME;
  return (int )e;
}

static CalloutTagVal
callout_tag_find(CalloutTagTable* t, const UChar* name, const UChar* name_end)
{
  CalloutTagVal e;

  e = -1;
  if (IS_NOT_NULL(t)) {
    onig_st_lookup_strend(t, name, name_end, (HashDataType* )((void* )(&e)));
  }
  return e;
}

static int
callout_tag_table_new(CalloutTagTable** rt)
{
  CalloutTagTable* t;

  *rt = 0;
  t = onig_st_init_strend_table_with_size(INIT_TAG_NAMES_ALLOC_NUM);
  CHECK_NULL_RETURN_MEMERR(t);

  *rt = t;
  return ONIG_NORMAL;
}

static int
callout_tag_entry_raw(CalloutTagTable* t, UChar* name, UChar* name_end,
                      CalloutTagVal entry_val)
{
  int r;
  CalloutTagVal val;

  if (name_end - name <= 0)
    return ONIGERR_INVALID_CALLOUT_TAG_NAME;

  val = callout_tag_find(t, name, name_end);
  if (val >= 0)
    return ONIGERR_MULTIPLEX_DEFINED_NAME;

  r = onig_st_insert_strend(t, name, name_end, (HashDataType )entry_val);
  if (r < 0) return r;

  return ONIG_NORMAL;
}

static int
ext_ensure_tag_table(regex_t* reg)
{
  int r;
  RegexExt* ext;
  CalloutTagTable* t;

  ext = onig_get_regex_ext(reg);
  CHECK_NULL_RETURN_MEMERR(ext);

  if (IS_NULL(ext->tag_table)) {
    r = callout_tag_table_new(&t);
    if (r != ONIG_NORMAL) return r;

    ext->tag_table = t;
  }

  return ONIG_NORMAL;
}

static int
callout_tag_entry(regex_t* reg, UChar* name, UChar* name_end,
                  CalloutTagVal entry_val)
{
  int r;
  RegexExt* ext;
  CalloutListEntry* e;

  r = ext_ensure_tag_table(reg);
  if (r != ONIG_NORMAL) return r;

  ext = onig_get_regex_ext(reg);
  r = callout_tag_entry_raw(ext->tag_table, name, name_end, entry_val);

  e = onig_reg_callout_list_at(reg, (int )entry_val);
  e->tag_start = name;
  e->tag_end   = name_end;

  return r;
}

#endif /* USE_CALLOUT */


#define INIT_SCANENV_MEMENV_ALLOC_SIZE   16

static void
scan_env_clear(ScanEnv* env)
{
  MEM_STATUS_CLEAR(env->capture_history);
  MEM_STATUS_CLEAR(env->bt_mem_start);
  MEM_STATUS_CLEAR(env->bt_mem_end);
  MEM_STATUS_CLEAR(env->backrefed_mem);
  env->error      = (UChar* )NULL;
  env->error_end  = (UChar* )NULL;
  env->num_call   = 0;

#ifdef USE_CALL
  env->unset_addr_list = NULL;
  env->has_call_zero   = 0;
#endif

  env->num_mem    = 0;
  env->num_named  = 0;
  env->mem_alloc  = 0;
  env->mem_env_dynamic = (MemEnv* )NULL;

  xmemset(env->mem_env_static, 0, sizeof(env->mem_env_static));

  env->parse_depth         = 0;
  env->keep_num            = 0;
  env->save_num            = 0;
  env->save_alloc_num      = 0;
  env->saves               = 0;
}

static int
scan_env_add_mem_entry(ScanEnv* env)
{
  int i, need, alloc;
  MemEnv* p;

  need = env->num_mem + 1;
  if (need > MaxCaptureNum && MaxCaptureNum != 0)
    return ONIGERR_TOO_MANY_CAPTURES;

  if (need >= SCANENV_MEMENV_SIZE) {
    if (env->mem_alloc <= need) {
      if (IS_NULL(env->mem_env_dynamic)) {
        alloc = INIT_SCANENV_MEMENV_ALLOC_SIZE;
        p = (MemEnv* )xmalloc(sizeof(MemEnv) * alloc);
        CHECK_NULL_RETURN_MEMERR(p);
        xmemcpy(p, env->mem_env_static, sizeof(env->mem_env_static));
      }
      else {
        alloc = env->mem_alloc * 2;
        p = (MemEnv* )xrealloc(env->mem_env_dynamic, sizeof(MemEnv) * alloc);
        CHECK_NULL_RETURN_MEMERR(p);
      }

      for (i = env->num_mem + 1; i < alloc; i++) {
        p[i].node = NULL_NODE;
#if 0
        p[i].in   = 0;
        p[i].recursion = 0;
#endif
      }

      env->mem_env_dynamic = p;
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
    SCANENV_MEMENV(env)[num].node = node;
  else
    return ONIGERR_PARSER_BUG;
  return 0;
}

extern void
onig_node_free(Node* node)
{
 start:
  if (IS_NULL(node)) return ;

#ifdef DEBUG_NODE_FREE
  fprintf(stderr, "onig_node_free: %p\n", node);
#endif

  switch (NODE_TYPE(node)) {
  case NODE_STRING:
    if (STR_(node)->capa != 0 &&
        IS_NOT_NULL(STR_(node)->s) && STR_(node)->s != STR_(node)->buf) {
      xfree(STR_(node)->s);
    }
    break;

  case NODE_LIST:
  case NODE_ALT:
    onig_node_free(NODE_CAR(node));
    {
      Node* next_node = NODE_CDR(node);

      xfree(node);
      node = next_node;
      goto start;
    }
    break;

  case NODE_CCLASS:
    {
      CClassNode* cc = CCLASS_(node);

      if (cc->mbuf)
        bbuf_free(cc->mbuf);
    }
    break;

  case NODE_BACKREF:
    if (IS_NOT_NULL(BACKREF_(node)->back_dynamic))
      xfree(BACKREF_(node)->back_dynamic);
    break;

  case NODE_ENCLOSURE:
    if (NODE_BODY(node))
      onig_node_free(NODE_BODY(node));

    {
      EnclosureNode* en = ENCLOSURE_(node);
      if (en->type == ENCLOSURE_IF_ELSE) {
        onig_node_free(en->te.Then);
        onig_node_free(en->te.Else);
      }
    }
    break;

  case NODE_QUANT:
  case NODE_ANCHOR:
    if (NODE_BODY(node))
      onig_node_free(NODE_BODY(node));
    break;

  case NODE_CTYPE:
  case NODE_CALL:
  case NODE_GIMMICK:
    break;
  }

  xfree(node);
}

static void
cons_node_free_alone(Node* node)
{
  NODE_CAR(node) = 0;
  NODE_CDR(node) = 0;
  onig_node_free(node);
}

static Node*
node_new(void)
{
  Node* node;

  node = (Node* )xmalloc(sizeof(Node));
  xmemset(node, 0, sizeof(*node));

#ifdef DEBUG_NODE_FREE
  fprintf(stderr, "node_new: %p\n", node);
#endif
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

  NODE_SET_TYPE(node, NODE_CCLASS);
  initialize_cclass(CCLASS_(node));
  return node;
}

static Node*
node_new_ctype(int type, int not, OnigOptionType options)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_CTYPE);
  CTYPE_(node)->ctype   = type;
  CTYPE_(node)->not     = not;
  CTYPE_(node)->options = options;
  CTYPE_(node)->ascii_mode = IS_ASCII_MODE_CTYPE_OPTION(type, options);
  return node;
}

static Node*
node_new_anychar(void)
{
  Node* node = node_new_ctype(CTYPE_ANYCHAR, 0, ONIG_OPTION_NONE);
  return node;
}

static Node*
node_new_anychar_with_fixed_option(OnigOptionType option)
{
  CtypeNode* ct;
  Node* node;

  node = node_new_anychar();
  ct = CTYPE_(node);
  ct->options = option;
  NODE_STATUS_ADD(node, FIXED_OPTION);
  return node;
}

static int
node_new_no_newline(Node** node, ScanEnv* env)
{
  Node* n;

  n = node_new_anychar_with_fixed_option(ONIG_OPTION_NONE);
  CHECK_NULL_RETURN_MEMERR(n);
  *node = n;
  return 0;
}

static int
node_new_true_anychar(Node** node, ScanEnv* env)
{
  Node* n;

  n = node_new_anychar_with_fixed_option(ONIG_OPTION_MULTILINE);
  CHECK_NULL_RETURN_MEMERR(n);
  *node = n;
  return 0;
}

static Node*
node_new_list(Node* left, Node* right)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_LIST);
  NODE_CAR(node)  = left;
  NODE_CDR(node) = right;
  return node;
}

extern Node*
onig_node_new_list(Node* left, Node* right)
{
  return node_new_list(left, right);
}

extern Node*
onig_node_list_add(Node* list, Node* x)
{
  Node *n;

  n = onig_node_new_list(x, NULL);
  if (IS_NULL(n)) return NULL_NODE;

  if (IS_NOT_NULL(list)) {
    while (IS_NOT_NULL(NODE_CDR(list)))
      list = NODE_CDR(list);

    NODE_CDR(list) = n;
  }

  return n;
}

extern Node*
onig_node_new_alt(Node* left, Node* right)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_ALT);
  NODE_CAR(node)  = left;
  NODE_CDR(node) = right;
  return node;
}

static Node*
make_list_or_alt(NodeType type, int n, Node* ns[])
{
  Node* r;

  if (n <= 0) return NULL_NODE;

  if (n == 1) {
    r = node_new();
    CHECK_NULL_RETURN(r);
    NODE_SET_TYPE(r, type);
    NODE_CAR(r) = ns[0];
    NODE_CDR(r) = NULL_NODE;
  }
  else {
    Node* right;

    r = node_new();
    CHECK_NULL_RETURN(r);

    right = make_list_or_alt(type, n - 1, ns + 1);
    if (IS_NULL(right)) {
      onig_node_free(r);
      return NULL_NODE;
    }

    NODE_SET_TYPE(r, type);
    NODE_CAR(r) = ns[0];
    NODE_CDR(r) = right;
  }

  return r;
}

static Node*
make_list(int n, Node* ns[])
{
  return make_list_or_alt(NODE_LIST, n, ns);
}

static Node*
make_alt(int n, Node* ns[])
{
  return make_list_or_alt(NODE_ALT, n, ns);
}

extern Node*
onig_node_new_anchor(int type, int ascii_mode)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_ANCHOR);
  ANCHOR_(node)->type       = type;
  ANCHOR_(node)->char_len   = -1;
  ANCHOR_(node)->ascii_mode = ascii_mode;
  return node;
}

static Node*
node_new_backref(int back_num, int* backrefs, int by_name,
#ifdef USE_BACKREF_WITH_LEVEL
                 int exist_level, int nest_level,
#endif
                 ScanEnv* env)
{
  int i;
  Node* node = node_new();

  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_BACKREF);
  BACKREF_(node)->back_num = back_num;
  BACKREF_(node)->back_dynamic = (int* )NULL;
  if (by_name != 0)
    NODE_STATUS_ADD(node, BY_NAME);

#ifdef USE_BACKREF_WITH_LEVEL
  if (exist_level != 0) {
    NODE_STATUS_ADD(node, NEST_LEVEL);
    BACKREF_(node)->nest_level  = nest_level;
  }
#endif

  for (i = 0; i < back_num; i++) {
    if (backrefs[i] <= env->num_mem &&
        IS_NULL(SCANENV_MEMENV(env)[backrefs[i]].node)) {
      NODE_STATUS_ADD(node, RECURSION);   /* /...(\1).../ */
      break;
    }
  }

  if (back_num <= NODE_BACKREFS_SIZE) {
    for (i = 0; i < back_num; i++)
      BACKREF_(node)->back_static[i] = backrefs[i];
  }
  else {
    int* p = (int* )xmalloc(sizeof(int) * back_num);
    if (IS_NULL(p)) {
      onig_node_free(node);
      return NULL;
    }
    BACKREF_(node)->back_dynamic = p;
    for (i = 0; i < back_num; i++)
      p[i] = backrefs[i];
  }
  return node;
}

static Node*
node_new_backref_checker(int back_num, int* backrefs, int by_name,
#ifdef USE_BACKREF_WITH_LEVEL
                         int exist_level, int nest_level,
#endif
                         ScanEnv* env)
{
  Node* node;

  node = node_new_backref(back_num, backrefs, by_name,
#ifdef USE_BACKREF_WITH_LEVEL
                          exist_level, nest_level,
#endif
                          env);
  CHECK_NULL_RETURN(node);

  NODE_STATUS_ADD(node, CHECKER);
  return node;
}

#ifdef USE_CALL
static Node*
node_new_call(UChar* name, UChar* name_end, int gnum, int by_number)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_CALL);
  CALL_(node)->by_number   = by_number;
  CALL_(node)->name        = name;
  CALL_(node)->name_end    = name_end;
  CALL_(node)->group_num   = gnum;
  CALL_(node)->entry_count = 1;
  return node;
}
#endif

static Node*
node_new_quantifier(int lower, int upper, int by_number)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_QUANT);
  QUANT_(node)->lower  = lower;
  QUANT_(node)->upper  = upper;
  QUANT_(node)->greedy = 1;
  QUANT_(node)->body_empty_info = QUANT_BODY_IS_NOT_EMPTY;
  QUANT_(node)->head_exact      = NULL_NODE;
  QUANT_(node)->next_head_exact = NULL_NODE;
  QUANT_(node)->is_refered      = 0;
  if (by_number != 0)
    NODE_STATUS_ADD(node, BY_NUMBER);

  return node;
}

static Node*
node_new_enclosure(enum EnclosureType type)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_ENCLOSURE);
  ENCLOSURE_(node)->type = type;

  switch (type) {
  case ENCLOSURE_MEMORY:
    ENCLOSURE_(node)->m.regnum       =  0;
    ENCLOSURE_(node)->m.called_addr  = -1;
    ENCLOSURE_(node)->m.entry_count  =  1;
    ENCLOSURE_(node)->m.called_state =  0;
    break;

  case ENCLOSURE_OPTION:
    ENCLOSURE_(node)->o.options =  0;
    break;

  case ENCLOSURE_STOP_BACKTRACK:
    break;

  case ENCLOSURE_IF_ELSE:
    ENCLOSURE_(node)->te.Then = 0;
    ENCLOSURE_(node)->te.Else = 0;
    break;
  }

  ENCLOSURE_(node)->opt_count = 0;
  return node;
}

extern Node*
onig_node_new_enclosure(int type)
{
  return node_new_enclosure(type);
}

static Node*
node_new_enclosure_if_else(Node* cond, Node* Then, Node* Else)
{
  Node* n;
  n = node_new_enclosure(ENCLOSURE_IF_ELSE);
  CHECK_NULL_RETURN(n);

  NODE_BODY(n) = cond;
  ENCLOSURE_(n)->te.Then = Then;
  ENCLOSURE_(n)->te.Else = Else;
  return n;
}

static Node*
node_new_memory(int is_named)
{
  Node* node = node_new_enclosure(ENCLOSURE_MEMORY);
  CHECK_NULL_RETURN(node);
  if (is_named != 0)
    NODE_STATUS_ADD(node, NAMED_GROUP);

  return node;
}

static Node*
node_new_option(OnigOptionType option)
{
  Node* node = node_new_enclosure(ENCLOSURE_OPTION);
  CHECK_NULL_RETURN(node);
  ENCLOSURE_(node)->o.options = option;
  return node;
}

static int
node_new_fail(Node** node, ScanEnv* env)
{
  *node = node_new();
  CHECK_NULL_RETURN_MEMERR(*node);

  NODE_SET_TYPE(*node, NODE_GIMMICK);
  GIMMICK_(*node)->type = GIMMICK_FAIL;
  return ONIG_NORMAL;
}

static int
node_new_save_gimmick(Node** node, enum SaveType save_type, ScanEnv* env)
{
  int id;
  int r;

  r = save_entry(env, save_type, &id);
  if (r != ONIG_NORMAL) return r;

  *node = node_new();
  CHECK_NULL_RETURN_MEMERR(*node);

  NODE_SET_TYPE(*node, NODE_GIMMICK);
  GIMMICK_(*node)->id   = id;
  GIMMICK_(*node)->type = GIMMICK_SAVE;
  GIMMICK_(*node)->detail_type = (int )save_type;

  return ONIG_NORMAL;
}

static int
node_new_update_var_gimmick(Node** node, enum UpdateVarType update_var_type,
                            int id, ScanEnv* env)
{
  *node = node_new();
  CHECK_NULL_RETURN_MEMERR(*node);

  NODE_SET_TYPE(*node, NODE_GIMMICK);
  GIMMICK_(*node)->id   = id;
  GIMMICK_(*node)->type = GIMMICK_UPDATE_VAR;
  GIMMICK_(*node)->detail_type = (int )update_var_type;

  return ONIG_NORMAL;
}

static int
node_new_keep(Node** node, ScanEnv* env)
{
  int r;

  r = node_new_save_gimmick(node, SAVE_KEEP, env);
  if (r != 0) return r;

  env->keep_num++;
  return ONIG_NORMAL;
}

#ifdef USE_CALLOUT

extern void
onig_free_reg_callout_list(int n, CalloutListEntry* list)
{
  int i;
  int j;

  if (IS_NULL(list)) return ;

  for (i = 0; i < n; i++) {
    if (list[i].of == ONIG_CALLOUT_OF_NAME) {
      for (j = 0; j < list[i].u.arg.passed_num; j++) {
        if (list[i].u.arg.types[j] == ONIG_TYPE_STRING) {
          if (IS_NOT_NULL(list[i].u.arg.vals[j].s.start))
            xfree(list[i].u.arg.vals[j].s.start);
        }
      }
    }
    else { /* ONIG_CALLOUT_OF_CONTENTS */
      if (IS_NOT_NULL(list[i].u.content.start)) {
        xfree((void* )list[i].u.content.start);
      }
    }
  }

  xfree(list);
}

extern CalloutListEntry*
onig_reg_callout_list_at(regex_t* reg, int num)
{
  RegexExt* ext = REG_EXTP(reg);
  CHECK_NULL_RETURN(ext);

  if (num <= 0 || num > ext->callout_num)
    return 0;

  num--;
  return ext->callout_list + num;
}

static int
reg_callout_list_entry(ScanEnv* env, int* rnum)
{
#define INIT_CALLOUT_LIST_NUM  3

  int num;
  CalloutListEntry* list;
  CalloutListEntry* e;
  RegexExt* ext;

  ext = onig_get_regex_ext(env->reg);
  CHECK_NULL_RETURN_MEMERR(ext);

  if (IS_NULL(ext->callout_list)) {
    list = (CalloutListEntry* )xmalloc(sizeof(*list) * INIT_CALLOUT_LIST_NUM);
    CHECK_NULL_RETURN_MEMERR(list);

    ext->callout_list = list;
    ext->callout_list_alloc = INIT_CALLOUT_LIST_NUM;
    ext->callout_num = 0;
  }

  num = ext->callout_num + 1;
  if (num > ext->callout_list_alloc) {
    int alloc = ext->callout_list_alloc * 2;
    list = (CalloutListEntry* )xrealloc(ext->callout_list,
                                        sizeof(CalloutListEntry) * alloc);
    CHECK_NULL_RETURN_MEMERR(list);

    ext->callout_list       = list;
    ext->callout_list_alloc = alloc;
  }

  e = ext->callout_list + (num - 1);

  e->flag             = 0;
  e->of               = 0;
  e->in               = ONIG_CALLOUT_OF_CONTENTS;
  e->type             = 0;
  e->tag_start        = 0;
  e->tag_end          = 0;
  e->start_func       = 0;
  e->end_func         = 0;
  e->u.arg.num        = 0;
  e->u.arg.passed_num = 0;

  ext->callout_num = num;
  *rnum = num;
  return ONIG_NORMAL;
}

static int
node_new_callout(Node** node, OnigCalloutOf callout_of, int num, int id,
                 ScanEnv* env)
{
  *node = node_new();
  CHECK_NULL_RETURN_MEMERR(*node);

  NODE_SET_TYPE(*node, NODE_GIMMICK);
  GIMMICK_(*node)->id          = id;
  GIMMICK_(*node)->num         = num;
  GIMMICK_(*node)->type        = GIMMICK_CALLOUT;
  GIMMICK_(*node)->detail_type = (int )callout_of;

  return ONIG_NORMAL;
}
#endif

static int
make_extended_grapheme_cluster(Node** node, ScanEnv* env)
{
  int r;
  int i;
  Node* x;
  Node* ns[2];

  /* \X == (?>\O(?:\Y\O)*) */

  ns[1] = NULL_NODE;

  r = ONIGERR_MEMORY;
  ns[0] = onig_node_new_anchor(ANCHOR_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY, 0);
  if (IS_NULL(ns[0])) goto err;

  r = node_new_true_anychar(&ns[1], env);
  if (r != 0) goto err1;

  x = make_list(2, ns);
  if (IS_NULL(x)) goto err;
  ns[0] = x;
  ns[1] = NULL_NODE;

  x = node_new_quantifier(0, REPEAT_INFINITE, 1);
  if (IS_NULL(x)) goto err;

  NODE_BODY(x) = ns[0];
  ns[0] = NULL_NODE;
  ns[1] = x;

  r = node_new_true_anychar(&ns[0], env);
  if (r != 0) goto err1;

  x = make_list(2, ns);
  if (IS_NULL(x)) goto err;

  ns[0] = x;
  ns[1] = NULL_NODE;

  x = node_new_enclosure(ENCLOSURE_STOP_BACKTRACK);
  if (IS_NULL(x)) goto err;

  NODE_BODY(x) = ns[0];

  *node = x;
  return ONIG_NORMAL;

 err:
  r = ONIGERR_MEMORY;
 err1:
  for (i = 0; i < 2; i++) onig_node_free(ns[i]);
  return r;
}

static int
make_absent_engine(Node** node, int pre_save_right_id, Node* absent,
                   Node* step_one, int lower, int upper, int possessive,
                   int is_range_cutter, ScanEnv* env)
{
  int r;
  int i;
  int id;
  Node* x;
  Node* ns[4];

  for (i = 0; i < 4; i++) ns[i] = NULL_NODE;

  ns[1] = absent;
  ns[3] = step_one; /* for err */
  r = node_new_save_gimmick(&ns[0], SAVE_S, env);
  if (r != 0) goto err;

  id = GIMMICK_(ns[0])->id;
  r = node_new_update_var_gimmick(&ns[2], UPDATE_VAR_RIGHT_RANGE_FROM_S_STACK,
                                  id, env);
  if (r != 0) goto err;

  r = node_new_fail(&ns[3], env);
  if (r != 0) goto err;

  x = make_list(4, ns);
  if (IS_NULL(x)) goto err0;

  ns[0] = x;
  ns[1] = step_one;
  ns[2] = ns[3] = NULL_NODE;

  x = make_alt(2, ns);
  if (IS_NULL(x)) goto err0;

  ns[0] = x;

  x = node_new_quantifier(lower, upper, 0);
  if (IS_NULL(x)) goto err0;

  NODE_BODY(x) = ns[0];
  ns[0] = x;

  if (possessive != 0) {
    x = node_new_enclosure(ENCLOSURE_STOP_BACKTRACK);
    if (IS_NULL(x)) goto err0;

    NODE_BODY(x) = ns[0];
    ns[0] = x;
  }

  r = node_new_update_var_gimmick(&ns[1], UPDATE_VAR_RIGHT_RANGE_FROM_STACK,
                                  pre_save_right_id, env);
  if (r != 0) goto err;

  r = node_new_fail(&ns[2], env);
  if (r != 0) goto err;

  x = make_list(2, ns + 1);
  if (IS_NULL(x)) goto err0;

  ns[1] = x; ns[2] = NULL_NODE;

  x = make_alt(2, ns);
  if (IS_NULL(x)) goto err0;

  if (is_range_cutter != 0)
    NODE_STATUS_ADD(x, SUPER);

  *node = x;
  return ONIG_NORMAL;

 err0:
  r = ONIGERR_MEMORY;
 err:
  for (i = 0; i < 4; i++) onig_node_free(ns[i]);
  return r;
}

static int
make_absent_tail(Node** node1, Node** node2, int pre_save_right_id,
                 ScanEnv* env)
{
  int r;
  int id;
  Node* save;
  Node* x;
  Node* ns[2];

  *node1 = *node2 = NULL_NODE;
  save = ns[0] = ns[1] = NULL_NODE;

  r = node_new_save_gimmick(&save, SAVE_RIGHT_RANGE, env);
  if (r != 0) goto err;

  id = GIMMICK_(save)->id;
  r = node_new_update_var_gimmick(&ns[0], UPDATE_VAR_RIGHT_RANGE_FROM_STACK,
                                  id, env);
  if (r != 0) goto err;

  r = node_new_fail(&ns[1], env);
  if (r != 0) goto err;

  x = make_list(2, ns);
  if (IS_NULL(x)) goto err0;

  ns[0] = NULL_NODE; ns[1] = x;

  r = node_new_update_var_gimmick(&ns[0], UPDATE_VAR_RIGHT_RANGE_FROM_STACK,
                                  pre_save_right_id, env);
  if (r != 0) goto err;

  x = make_alt(2, ns);
  if (IS_NULL(x)) goto err0;

  *node1 = save;
  *node2 = x;
  return ONIG_NORMAL;

 err0:
  r = ONIGERR_MEMORY;
 err:
  onig_node_free(save);
  onig_node_free(ns[0]);
  onig_node_free(ns[1]);
  return r;
}

static int
make_range_clear(Node** node, ScanEnv* env)
{
  int r;
  int id;
  Node* save;
  Node* x;
  Node* ns[2];

  *node = NULL_NODE;
  save = ns[0] = ns[1] = NULL_NODE;

  r = node_new_save_gimmick(&save, SAVE_RIGHT_RANGE, env);
  if (r != 0) goto err;

  id = GIMMICK_(save)->id;
  r = node_new_update_var_gimmick(&ns[0], UPDATE_VAR_RIGHT_RANGE_FROM_STACK,
                                  id, env);
  if (r != 0) goto err;

  r = node_new_fail(&ns[1], env);
  if (r != 0) goto err;

  x = make_list(2, ns);
  if (IS_NULL(x)) goto err0;

  ns[0] = NULL_NODE; ns[1] = x;

  r = node_new_update_var_gimmick(&ns[0], UPDATE_VAR_RIGHT_RANGE_INIT, 0, env);
  if (r != 0) goto err;

  x = make_alt(2, ns);
  if (IS_NULL(x)) goto err0;

  NODE_STATUS_ADD(x, SUPER);

  ns[0] = save;
  ns[1] = x;
  save = NULL_NODE;
  x = make_list(2, ns);
  if (IS_NULL(x)) goto err0;

  *node = x;
  return ONIG_NORMAL;

 err0:
  r = ONIGERR_MEMORY;
 err:
  onig_node_free(save);
  onig_node_free(ns[0]);
  onig_node_free(ns[1]);
  return r;
}

static int
is_simple_one_char_repeat(Node* node, Node** rquant, Node** rbody,
                          int* is_possessive, ScanEnv* env)
{
  Node* quant;
  Node* body;

  *rquant = *rbody = 0;
  *is_possessive = 0;

  if (NODE_TYPE(node) == NODE_QUANT) {
    quant = node;
  }
  else {
    if (NODE_TYPE(node) == NODE_ENCLOSURE) {
      EnclosureNode* en = ENCLOSURE_(node);
      if (en->type == ENCLOSURE_STOP_BACKTRACK) {
        *is_possessive = 1;
        quant = NODE_ENCLOSURE_BODY(en);
        if (NODE_TYPE(quant) != NODE_QUANT)
          return 0;
      }
      else
        return 0;
    }
    else
      return 0;
  }

  if (QUANT_(quant)->greedy == 0)
    return 0;

  body = NODE_BODY(quant);
  switch (NODE_TYPE(body)) {
  case NODE_STRING:
    {
      int len;
      StrNode* sn = STR_(body);
      UChar *s = sn->s;

      len = 0;
      while (s < sn->end) {
        s += enclen(env->enc, s);
        len++;
      }
      if (len != 1)
        return 0;
    }

  case NODE_CCLASS:
    break;

  default:
    return 0;
    break;
  }

  if (node != quant) {
    NODE_BODY(node) = 0;
    onig_node_free(node);
  }
  NODE_BODY(quant) = NULL_NODE;
  *rquant = quant;
  *rbody  = body;
  return 1;
}

static int
make_absent_tree_for_simple_one_char_repeat(Node** node, Node* absent, Node* quant,
                                            Node* body, int possessive, ScanEnv* env)
{
  int r;
  int i;
  int id1;
  int lower, upper;
  Node* x;
  Node* ns[4];

  *node = NULL_NODE;
  r = ONIGERR_MEMORY;
  ns[0] = ns[1] = NULL_NODE;
  ns[2] = body, ns[3] = absent;

  lower = QUANT_(quant)->lower;
  upper = QUANT_(quant)->upper;
  onig_node_free(quant);

  r = node_new_save_gimmick(&ns[0], SAVE_RIGHT_RANGE, env);
  if (r != 0) goto err;

  id1 = GIMMICK_(ns[0])->id;

  r = make_absent_engine(&ns[1], id1, absent, body, lower, upper, possessive,
                         0, env);
  if (r != 0) goto err;

  ns[2] = ns[3] = NULL_NODE;

  r = node_new_update_var_gimmick(&ns[2], UPDATE_VAR_RIGHT_RANGE_FROM_STACK,
                                  id1, env);
  if (r != 0) goto err;

  x = make_list(3, ns);
  if (IS_NULL(x)) goto err0;

  *node = x;
  return ONIG_NORMAL;

 err0:
  r = ONIGERR_MEMORY;
 err:
  for (i = 0; i < 4; i++) onig_node_free(ns[i]);
  return r;
}

static int
make_absent_tree(Node** node, Node* absent, Node* expr, int is_range_cutter,
                 ScanEnv* env)
{
  int r;
  int i;
  int id1, id2;
  int possessive;
  Node* x;
  Node* ns[7];

  r = ONIGERR_MEMORY;
  for (i = 0; i < 7; i++) ns[i] = NULL_NODE;
  ns[4] = expr; ns[5] = absent;

  if (is_range_cutter == 0) {
    Node* quant;
    Node* body;

    if (expr == NULL_NODE) {
      /* default expr \O* */
      quant = node_new_quantifier(0, REPEAT_INFINITE, 0);
      if (IS_NULL(quant)) goto err0;

      r = node_new_true_anychar(&body, env);
      if (r != 0) {
        onig_node_free(quant);
        goto err;
      }
      possessive = 0;
      goto simple;
    }
    else {
      if (is_simple_one_char_repeat(expr, &quant, &body, &possessive, env)) {
      simple:
        r = make_absent_tree_for_simple_one_char_repeat(node, absent, quant,
                                                        body, possessive, env);
        if (r != 0) {
          ns[4] = NULL_NODE;
          onig_node_free(quant);
          onig_node_free(body);
          goto err;
        }

        return ONIG_NORMAL;
      }
    }
  }

  r = node_new_save_gimmick(&ns[0], SAVE_RIGHT_RANGE, env);
  if (r != 0) goto err;

  id1 = GIMMICK_(ns[0])->id;

  r = node_new_save_gimmick(&ns[1], SAVE_S, env);
  if (r != 0) goto err;

  id2 = GIMMICK_(ns[1])->id;

  r = node_new_true_anychar(&ns[3], env);
  if (r != 0) goto err;

  possessive = 1;
  r = make_absent_engine(&ns[2], id1, absent, ns[3], 0, REPEAT_INFINITE,
                         possessive, is_range_cutter, env);
  if (r != 0) goto err;

  ns[3] = NULL_NODE;
  ns[5] = NULL_NODE;

  r = node_new_update_var_gimmick(&ns[3], UPDATE_VAR_S_FROM_STACK, id2, env);
  if (r != 0) goto err;

  if (is_range_cutter != 0) {
    x = make_list(4, ns);
    if (IS_NULL(x)) goto err0;
  }
  else {
    r = make_absent_tail(&ns[5], &ns[6], id1, env);
    if (r != 0) goto err;
  
    x = make_list(7, ns);
    if (IS_NULL(x)) goto err0;
  }

  *node = x;
  return ONIG_NORMAL;

 err0:
  r = ONIGERR_MEMORY;
 err:
  for (i = 0; i < 7; i++) onig_node_free(ns[i]);
  return r;  
}

extern int
onig_node_str_cat(Node* node, const UChar* s, const UChar* end)
{
  int addlen = (int )(end - s);

  if (addlen > 0) {
    int len  = (int )(STR_(node)->end - STR_(node)->s);

    if (STR_(node)->capa > 0 || (len + addlen > NODE_STRING_BUF_SIZE - 1)) {
      UChar* p;
      int capa = len + addlen + NODE_STRING_MARGIN;

      if (capa <= STR_(node)->capa) {
        onig_strcpy(STR_(node)->s + len, s, end);
      }
      else {
        if (STR_(node)->s == STR_(node)->buf)
          p = strcat_capa_from_static(STR_(node)->s, STR_(node)->end,
                                      s, end, capa);
        else
          p = strcat_capa(STR_(node)->s, STR_(node)->end, s, end, capa);

        CHECK_NULL_RETURN_MEMERR(p);
        STR_(node)->s    = p;
        STR_(node)->capa = capa;
      }
    }
    else {
      onig_strcpy(STR_(node)->s + len, s, end);
    }
    STR_(node)->end = STR_(node)->s + len + addlen;
  }

  return 0;
}

extern int
onig_node_str_set(Node* node, const UChar* s, const UChar* end)
{
  onig_node_str_clear(node);
  return onig_node_str_cat(node, s, end);
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
  NODE_SET_TYPE(node, NODE_STRING);
  STR_(node)->flag = flag;
  STR_(node)->capa = 0;
  STR_(node)->s    = STR_(node)->buf;
  STR_(node)->end  = STR_(node)->buf;
}

extern void
onig_node_str_clear(Node* node)
{
  if (STR_(node)->capa != 0 &&
      IS_NOT_NULL(STR_(node)->s) && STR_(node)->s != STR_(node)->buf) {
    xfree(STR_(node)->s);
  }

  STR_(node)->capa = 0;
  STR_(node)->flag = 0;
  STR_(node)->s    = STR_(node)->buf;
  STR_(node)->end  = STR_(node)->buf;
}

static Node*
node_new_str(const UChar* s, const UChar* end)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_STRING);
  STR_(node)->capa = 0;
  STR_(node)->flag = 0;
  STR_(node)->s    = STR_(node)->buf;
  STR_(node)->end  = STR_(node)->buf;
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

static Node*
node_new_str_raw(UChar* s, UChar* end)
{
  Node* node = node_new_str(s, end);
  NODE_STRING_SET_RAW(node);
  return node;
}

static Node*
node_new_empty(void)
{
  return node_new_str(NULL, NULL);
}

static Node*
node_new_str_raw_char(UChar c)
{
  UChar p[1];

  p[0] = c;
  return node_new_str_raw(p, p + 1);
}

static Node*
str_node_split_last_char(Node* node, OnigEncoding enc)
{
  const UChar *p;
  Node* rn;
  StrNode* sn;

  sn = STR_(node);
  rn = NULL_NODE;
  if (sn->end > sn->s) {
    p = onigenc_get_prev_char_head(enc, sn->s, sn->end);
    if (p && p > sn->s) { /* can be split. */
      rn = node_new_str(p, sn->end);
      if (NODE_STRING_IS_RAW(node))
        NODE_STRING_SET_RAW(rn);

      sn->end = (UChar* )p;
    }
  }
  return rn;
}

static int
str_node_can_be_split(Node* node, OnigEncoding enc)
{
  StrNode* sn = STR_(node);
  if (sn->end > sn->s) {
    return ((enclen(enc, sn->s) < sn->end - sn->s)  ?  1 : 0);
  }
  return 0;
}

#ifdef USE_PAD_TO_SHORT_BYTE_CHAR
static int
node_str_head_pad(StrNode* sn, int num, UChar val)
{
  UChar buf[NODE_STRING_BUF_SIZE];
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
  while (! PEND) {
    PFETCH(c);
    if (IS_CODE_DIGIT_ASCII(enc, c)) {
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
scan_unsigned_hexadecimal_number(UChar** src, UChar* end, int minlen,
                                 int maxlen, OnigEncoding enc)
{
  OnigCodePoint c;
  unsigned int num, val;
  int n;
  UChar* p = *src;
  PFETCH_READY;

  num = 0;
  n = 0;
  while (! PEND && n < maxlen) {
    PFETCH(c);
    if (IS_CODE_XDIGIT_ASCII(enc, c)) {
      n++;
      val = (unsigned int )XDIGITVAL(enc,c);
      if ((INT_MAX_LIMIT - val) / 16UL < num)
        return ONIGERR_TOO_BIG_NUMBER; /* overflow */

      num = (num << 4) + XDIGITVAL(enc,c);
    }
    else {
      PUNFETCH;
      break;
    }
  }

  if (n < minlen)
    return ONIGERR_INVALID_CODE_POINT_VALUE;

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
  while (! PEND && maxlen-- != 0) {
    PFETCH(c);
    if (IS_CODE_DIGIT_ASCII(enc, c) && c < '8') {
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


#define BB_WRITE_CODE_POINT(bbuf,pos,code) \
    BB_WRITE(bbuf, pos, &(code), SIZE_CODE_POINT)

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
  CHECK_NULL_RETURN_MEMERR(bbuf);
  r = BB_INIT(bbuf, INIT_MULTI_BYTE_RANGE_SIZE);
  if (r != 0) {
    xfree(bbuf);
    *pbuf = 0;
    return r;
  }

  n = 0;
  BB_WRITE_CODE_POINT(bbuf, 0, n);
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
    if (r != 0) return r;
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

  high = (to == ~((OnigCodePoint )0)) ? n : low;
  for (bound = n; high < bound; ) {
    x = (high + bound) >> 1;
    if (to + 1 >= data[x*2])
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
      BB_MOVE_RIGHT(bbuf, from_pos, to_pos, size);
    }
    else {
      BB_MOVE_LEFT_REDUCE(bbuf, from_pos, to_pos);
    }
  }

  pos = SIZE_CODE_POINT * (1 + low * 2);
  BB_ENSURE_SIZE(bbuf, pos + SIZE_CODE_POINT * 2);
  BB_WRITE_CODE_POINT(bbuf, pos, from);
  BB_WRITE_CODE_POINT(bbuf, pos + SIZE_CODE_POINT, to);
  n += inc_n;
  BB_WRITE_CODE_POINT(bbuf, 0, n);

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

#define SWAP_BB_NOT(bbuf1, not1, bbuf2, not2) do {\
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
    SWAP_BB_NOT(bbuf1, not1, bbuf2, not2);

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
    SWAP_BB_NOT(bbuf1, not1, bbuf2, not2);

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
    SWAP_BB_NOT(bbuf1, not1, bbuf2, not2);

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

  not1 = IS_NCCLASS_NOT(dest);
  bsr1 = dest->bs;
  buf1 = dest->mbuf;
  not2 = IS_NCCLASS_NOT(cc);
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

  not1 = IS_NCCLASS_NOT(dest);
  bsr1 = dest->bs;
  buf1 = dest->mbuf;
  not2 = IS_NCCLASS_NOT(cc);
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

static OnigCodePoint
conv_backslash_value(OnigCodePoint c, ScanEnv* env)
{
  if (IS_SYNTAX_OP(env->syntax, ONIG_SYN_OP_ESC_CONTROL_CHARS)) {
    switch (c) {
    case 'n': return '\n';
    case 't': return '\t';
    case 'r': return '\r';
    case 'f': return '\f';
    case 'a': return '\007';
    case 'b': return '\010';
    case 'e': return '\033';
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
  switch (NODE_TYPE(node)) {
  case NODE_ANCHOR:
  case NODE_GIMMICK:
    return 1;
    break;

  case NODE_ENCLOSURE:
    /* allow enclosed elements */
    /* return is_invalid_quantifier_target(NODE_BODY(node)); */
    break;

  case NODE_LIST:
    do {
      if (! is_invalid_quantifier_target(NODE_CAR(node))) return 0;
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    return 0;
    break;

  case NODE_ALT:
    do {
      if (is_invalid_quantifier_target(NODE_CAR(node))) return 1;
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  default:
    break;
  }
  return 0;
}

/* ?:0, *:1, +:2, ??:3, *?:4, +?:5 */
static int
quantifier_type_num(QuantNode* q)
{
  if (q->greedy) {
    if (q->lower == 0) {
      if (q->upper == 1) return 0;
      else if (IS_REPEAT_INFINITE(q->upper)) return 1;
    }
    else if (q->lower == 1) {
      if (IS_REPEAT_INFINITE(q->upper)) return 2;
    }
  }
  else {
    if (q->lower == 0) {
      if (q->upper == 1) return 3;
      else if (IS_REPEAT_INFINITE(q->upper)) return 4;
    }
    else if (q->lower == 1) {
      if (IS_REPEAT_INFINITE(q->upper)) return 5;
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
  QuantNode *p, *c;

  p = QUANT_(pnode);
  c = QUANT_(cnode);
  pnum = quantifier_type_num(p);
  cnum = quantifier_type_num(c);
  if (pnum < 0 || cnum < 0) {
    if ((p->lower == p->upper) && ! IS_REPEAT_INFINITE(p->upper)) {
      if ((c->lower == c->upper) && ! IS_REPEAT_INFINITE(c->upper)) {
        int n = positive_int_multiply(p->lower, c->lower);
        if (n >= 0) {
          p->lower = p->upper = n;
          NODE_BODY(pnode) = NODE_BODY(cnode);
          goto remove_cnode;
        }
      }
    }

    return ;
  }

  switch(ReduceTypeTable[cnum][pnum]) {
  case RQ_DEL:
    *pnode = *cnode;
    break;
  case RQ_A:
    NODE_BODY(pnode) = NODE_BODY(cnode);
    p->lower  = 0;  p->upper = REPEAT_INFINITE;  p->greedy = 1;
    break;
  case RQ_AQ:
    NODE_BODY(pnode) = NODE_BODY(cnode);
    p->lower  = 0;  p->upper = REPEAT_INFINITE;  p->greedy = 0;
    break;
  case RQ_QQ:
    NODE_BODY(pnode) = NODE_BODY(cnode);
    p->lower  = 0;  p->upper = 1;  p->greedy = 0;
    break;
  case RQ_P_QQ:
    NODE_BODY(pnode) = cnode;
    p->lower  = 0;  p->upper = 1;  p->greedy = 0;
    c->lower  = 1;  c->upper = REPEAT_INFINITE;  c->greedy = 1;
    return ;
    break;
  case RQ_PQ_Q:
    NODE_BODY(pnode) = cnode;
    p->lower  = 0;  p->upper = 1;  p->greedy = 1;
    c->lower  = 1;  c->upper = REPEAT_INFINITE;  c->greedy = 0;
    return ;
    break;
  case RQ_ASIS:
    NODE_BODY(pnode) = cnode;
    return ;
    break;
  }

 remove_cnode:
  NODE_BODY(cnode) = NULL_NODE;
  onig_node_free(cnode);
}

static int
node_new_general_newline(Node** node, ScanEnv* env)
{
  int r;
  int dlen, alen;
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN * 2];
  Node* crnl;
  Node* ncc;
  Node* x;
  CClassNode* cc;

  dlen = ONIGENC_CODE_TO_MBC(env->enc, 0x0d, buf);
  if (dlen < 0) return dlen;
  alen = ONIGENC_CODE_TO_MBC(env->enc, 0x0a, buf + dlen);
  if (alen < 0) return alen;

  crnl = node_new_str_raw(buf, buf + dlen + alen);
  CHECK_NULL_RETURN_MEMERR(crnl);

  ncc = node_new_cclass();
  if (IS_NULL(ncc)) goto err2;

  cc = CCLASS_(ncc);
  if (dlen == 1) {
    bitset_set_range(cc->bs, 0x0a, 0x0d);
  }
  else {
    r = add_code_range(&(cc->mbuf), env, 0x0a, 0x0d);
    if (r != 0) {
    err1:
      onig_node_free(ncc);
    err2:
      onig_node_free(crnl);
      return ONIGERR_MEMORY;
    }
  }

  if (ONIGENC_IS_UNICODE_ENCODING(env->enc)) {
    r = add_code_range(&(cc->mbuf), env, 0x85, 0x85);
    if (r != 0) goto err1;
    r = add_code_range(&(cc->mbuf), env, 0x2028, 0x2029);
    if (r != 0) goto err1;
  }

  x = node_new_enclosure_if_else(crnl, 0, ncc);
  if (IS_NULL(x)) goto err1;

  *node = x;
  return 0;
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
  TK_KEEP,             /* \K */
  TK_GENERAL_NEWLINE,  /* \R */
  TK_NO_NEWLINE,       /* \N */
  TK_TRUE_ANYCHAR,     /* \O */
  TK_EXTENDED_GRAPHEME_CLUSTER, /* \X */

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
#ifdef USE_BACKREF_WITH_LEVEL
      int  exist_level;
      int  level;   /* \k<name+n> */
#endif
    } backref;
    struct {
      UChar* name;
      UChar* name_end;
      int    gnum;
      int    by_number;
    } call;
    struct {
      int ctype;
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
    if (c != MC_ESC(env->syntax)) goto invalid;
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
  if (syn_allow) {
    /* *src = p; */ /* !!! Don't do this line !!! */
    return 1;  /* OK */
  }
  else
    return ONIGERR_INVALID_REPEAT_RANGE_PATTERN;
}

/* \M-, \C-, \c, or \... */
static int
fetch_escaped_value(UChar** src, UChar* end, ScanEnv* env, OnigCodePoint* val)
{
  int v;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar* p = *src;

  if (PEND) return ONIGERR_END_PATTERN_AT_ESCAPE;

  PFETCH_S(c);
  switch (c) {
  case 'M':
    if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ESC_CAPITAL_M_BAR_META)) {
      if (PEND) return ONIGERR_END_PATTERN_AT_META;
      PFETCH_S(c);
      if (c != '-') return ONIGERR_META_CODE_SYNTAX;
      if (PEND) return ONIGERR_END_PATTERN_AT_META;
      PFETCH_S(c);
      if (c == MC_ESC(env->syntax)) {
        v = fetch_escaped_value(&p, end, env, &c);
        if (v < 0) return v;
      }
      c = ((c & 0xff) | 0x80);
    }
    else
      goto backslash;
    break;

  case 'C':
    if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL)) {
      if (PEND) return ONIGERR_END_PATTERN_AT_CONTROL;
      PFETCH_S(c);
      if (c != '-') return ONIGERR_CONTROL_CODE_SYNTAX;
      goto control;
    }
    else
      goto backslash;

  case 'c':
    if (IS_SYNTAX_OP(env->syntax, ONIG_SYN_OP_ESC_C_CONTROL)) {
    control:
      if (PEND) return ONIGERR_END_PATTERN_AT_CONTROL;
      PFETCH_S(c);
      if (c == '?') {
        c = 0177;
      }
      else {
        if (c == MC_ESC(env->syntax)) {
          v = fetch_escaped_value(&p, end, env, &c);
          if (v < 0) return v;
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
  *val = c;
  return 0;
}

static int fetch_token(OnigToken* tok, UChar** src, UChar* end, ScanEnv* env);

static OnigCodePoint
get_name_end_code_point(OnigCodePoint start)
{
  switch (start) {
  case '<':  return (OnigCodePoint )'>';  break;
  case '\'': return (OnigCodePoint )'\''; break;
  case '(':  return (OnigCodePoint )')';  break;
  default:
    break;
  }

  return (OnigCodePoint )0;
}

enum REF_NUM {
  IS_NOT_NUM = 0,
  IS_ABS_NUM = 1,
  IS_REL_NUM = 2
};

#ifdef USE_BACKREF_WITH_LEVEL
/*
   \k<name+n>, \k<name-n>
   \k<num+n>,  \k<num-n>
   \k<-num+n>, \k<-num-n>
   \k<+num+n>, \k<+num-n>
*/
static int
fetch_name_with_level(OnigCodePoint start_code, UChar** src, UChar* end,
                      UChar** rname_end, ScanEnv* env,
                      int* rback_num, int* rlevel, enum REF_NUM* num_type)
{
  int r, sign, exist_level;
  int digit_count;
  OnigCodePoint end_code;
  OnigCodePoint c = 0;
  OnigEncoding enc = env->enc;
  UChar *name_end;
  UChar *pnum_head;
  UChar *p = *src;
  PFETCH_READY;

  *rback_num = 0;
  exist_level = 0;
  *num_type = IS_NOT_NUM;
  sign = 1;
  pnum_head = *src;

  end_code = get_name_end_code_point(start_code);

  digit_count = 0;
  name_end = end;
  r = 0;
  if (PEND) {
    return ONIGERR_EMPTY_GROUP_NAME;
  }
  else {
    PFETCH(c);
    if (c == end_code)
      return ONIGERR_EMPTY_GROUP_NAME;

    if (IS_CODE_DIGIT_ASCII(enc, c)) {
      *num_type = IS_ABS_NUM;
      digit_count++;
    }
    else if (c == '-') {
      *num_type = IS_REL_NUM;
      sign = -1;
      pnum_head = p;
    }
    else if (c == '+') {
      *num_type = IS_REL_NUM;
      sign = 1;
      pnum_head = p;
    }
    else if (!ONIGENC_IS_CODE_WORD(enc, c)) {
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
    }
  }

  while (!PEND) {
    name_end = p;
    PFETCH(c);
    if (c == end_code || c == ')' || c == '+' || c == '-') {
      if (*num_type != IS_NOT_NUM && digit_count == 0)
        r = ONIGERR_INVALID_GROUP_NAME;
      break;
    }

    if (*num_type != IS_NOT_NUM) {
      if (IS_CODE_DIGIT_ASCII(enc, c)) {
        digit_count++;
      }
      else {
        r = ONIGERR_INVALID_GROUP_NAME;
        *num_type = IS_NOT_NUM;
      }
    }
    else if (!ONIGENC_IS_CODE_WORD(enc, c)) {
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
    }
  }

  if (r == 0 && c != end_code) {
    if (c == '+' || c == '-') {
      int level;
      int flag = (c == '-' ? -1 : 1);

      if (PEND) {
        r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
        goto end;
      }
      PFETCH(c);
      if (! IS_CODE_DIGIT_ASCII(enc, c)) goto err;
      PUNFETCH;
      level = onig_scan_unsigned_number(&p, end, enc);
      if (level < 0) return ONIGERR_TOO_BIG_NUMBER;
      *rlevel = (level * flag);
      exist_level = 1;

      if (!PEND) {
        PFETCH(c);
        if (c == end_code)
          goto end;
      }
    }

  err:
    name_end = end;
  err2:
    r = ONIGERR_INVALID_GROUP_NAME;
  }

 end:
  if (r == 0) {
    if (*num_type != IS_NOT_NUM) {
      *rback_num = onig_scan_unsigned_number(&pnum_head, name_end, enc);
      if (*rback_num < 0) return ONIGERR_TOO_BIG_NUMBER;
      else if (*rback_num == 0) {
        if (*num_type == IS_REL_NUM)
          goto err2;
      }

      *rback_num *= sign;
    }

    *rname_end = name_end;
    *src = p;
    return (exist_level ? 1 : 0);
  }
  else {
    onig_scan_env_set_error_string(env, r, *src, name_end);
    return r;
  }
}
#endif /* USE_BACKREF_WITH_LEVEL */

/*
  ref: 0 -> define name    (don't allow number name)
       1 -> reference name (allow number name)
*/
static int
fetch_name(OnigCodePoint start_code, UChar** src, UChar* end,
           UChar** rname_end, ScanEnv* env, int* rback_num,
           enum REF_NUM* num_type, int ref)
{
  int r, sign;
  int digit_count;
  OnigCodePoint end_code;
  OnigCodePoint c = 0;
  OnigEncoding enc = env->enc;
  UChar *name_end;
  UChar *pnum_head;
  UChar *p = *src;

  *rback_num = 0;

  end_code = get_name_end_code_point(start_code);

  digit_count = 0;
  name_end = end;
  pnum_head = *src;
  r = 0;
  *num_type = IS_NOT_NUM;
  sign = 1;
  if (PEND) {
    return ONIGERR_EMPTY_GROUP_NAME;
  }
  else {
    PFETCH_S(c);
    if (c == end_code)
      return ONIGERR_EMPTY_GROUP_NAME;

    if (IS_CODE_DIGIT_ASCII(enc, c)) {
      if (ref == 1)
        *num_type = IS_ABS_NUM;
      else {
        r = ONIGERR_INVALID_GROUP_NAME;
      }
      digit_count++;
    }
    else if (c == '-') {
      if (ref == 1) {
        *num_type = IS_REL_NUM;
        sign = -1;
        pnum_head = p;
      }
      else {
        r = ONIGERR_INVALID_GROUP_NAME;
      }
    }
    else if (c == '+') {
      if (ref == 1) {
        *num_type = IS_REL_NUM;
        sign = 1;
        pnum_head = p;
      }
      else {
        r = ONIGERR_INVALID_GROUP_NAME;
      }
    }
    else if (!ONIGENC_IS_CODE_WORD(enc, c)) {
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
    }
  }

  if (r == 0) {
    while (!PEND) {
      name_end = p;
      PFETCH_S(c);
      if (c == end_code || c == ')') {
        if (*num_type != IS_NOT_NUM && digit_count == 0)
          r = ONIGERR_INVALID_GROUP_NAME;
        break;
      }

      if (*num_type != IS_NOT_NUM) {
        if (IS_CODE_DIGIT_ASCII(enc, c)) {
          digit_count++;
        }
        else {
          if (!ONIGENC_IS_CODE_WORD(enc, c))
            r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
          else
            r = ONIGERR_INVALID_GROUP_NAME;

          *num_type = IS_NOT_NUM;
        }
      }
      else {
        if (!ONIGENC_IS_CODE_WORD(enc, c)) {
          r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
        }
      }
    }

    if (c != end_code) {
      r = ONIGERR_INVALID_GROUP_NAME;
      goto err;
    }

    if (*num_type != IS_NOT_NUM) {
      *rback_num = onig_scan_unsigned_number(&pnum_head, name_end, enc);
      if (*rback_num < 0) return ONIGERR_TOO_BIG_NUMBER;
      else if (*rback_num == 0) {
        if (*num_type == IS_REL_NUM) {
          r = ONIGERR_INVALID_GROUP_NAME;
          goto err;
        }
      }

      *rback_num *= sign;
    }

    *rname_end = name_end;
    *src = p;
    return 0;
  }
  else {
    while (!PEND) {
      name_end = p;
      PFETCH_S(c);
      if (c == end_code || c == ')')
        break;
    }
    if (PEND)
      name_end = end;

  err:
    onig_scan_env_set_error_string(env, r, *src, name_end);
    return r;
  }
}

static void
CC_ESC_WARN(ScanEnv* env, UChar *c)
{
  if (onig_warn == onig_null_warn) return ;

  if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_WARN_CC_OP_NOT_ESCAPED) &&
      IS_SYNTAX_BV(env->syntax, ONIG_SYN_BACKSLASH_ESCAPE_IN_CC)) {
    UChar buf[WARN_BUFSIZE];
    onig_snprintf_with_pattern(buf, WARN_BUFSIZE, env->enc,
                               env->pattern, env->pattern_end,
                               (UChar* )"character class has '%s' without escape",
                               c);
    (*onig_warn)((char* )buf);
  }
}

static void
CLOSE_BRACKET_WITHOUT_ESC_WARN(ScanEnv* env, UChar* c)
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
    q = p + enclen(enc, p);
    if (x == s[0]) {
      for (i = 1; i < n && q < to; i++) {
        x = ONIGENC_MBC_TO_CODE(enc, q, to);
        if (x != s[i]) break;
        q += enclen(enc, q);
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
                         OnigCodePoint bad, OnigEncoding enc, OnigSyntaxType* syn)
{
  int i, in_esc;
  OnigCodePoint x;
  UChar *q;
  UChar *p = from;

  in_esc = 0;
  while (p < to) {
    if (in_esc) {
      in_esc = 0;
      p += enclen(enc, p);
    }
    else {
      x = ONIGENC_MBC_TO_CODE(enc, p, to);
      q = p + enclen(enc, p);
      if (x == s[0]) {
        for (i = 1; i < n && q < to; i++) {
          x = ONIGENC_MBC_TO_CODE(enc, q, to);
          if (x != s[i]) break;
          q += enclen(enc, q);
        }
        if (i >= n) return 1;
        p += enclen(enc, p);
      }
      else {
        x = ONIGENC_MBC_TO_CODE(enc, p, to);
        if (x == bad) return 0;
        else if (x == MC_ESC(syn)) in_esc = 1;
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
  else if (c == MC_ESC(syn)) {
    if (! IS_SYNTAX_BV(syn, ONIG_SYN_BACKSLASH_ESCAPE_IN_CC))
      goto end;

    if (PEND) return ONIGERR_END_PATTERN_AT_ESCAPE;

    PFETCH(c);
    tok->escaped = 1;
    tok->u.c = c;
    switch (c) {
    case 'w':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_WORD;
      tok->u.prop.not   = 0;
      break;
    case 'W':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_WORD;
      tok->u.prop.not   = 1;
      break;
    case 'd':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_DIGIT;
      tok->u.prop.not   = 0;
      break;
    case 'D':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_DIGIT;
      tok->u.prop.not   = 1;
      break;
    case 's':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_SPACE;
      tok->u.prop.not   = 0;
      break;
    case 'S':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_SPACE;
      tok->u.prop.not   = 1;
      break;
    case 'h':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_XDIGIT;
      tok->u.prop.not   = 0;
      break;
    case 'H':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_XDIGIT;
      tok->u.prop.not   = 1;
      break;

    case 'p':
    case 'P':
      if (PEND) break;

      c2 = PPEEK;
      if (c2 == '{' &&
          IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY)) {
        PINC;
        tok->type = TK_CHAR_PROPERTY;
        tok->u.prop.not = (c == 'P' ? 1 : 0);

        if (!PEND && IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT)) {
          PFETCH(c2);
          if (c2 == '^') {
            tok->u.prop.not = (tok->u.prop.not == 0 ? 1 : 0);
          }
          else
            PUNFETCH;
        }
      }
      break;

    case 'o':
      if (PEND) break;

      prev = p;
      if (PPEEK_IS('{') && IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_O_BRACE_OCTAL)) {
        PINC;
        num = scan_unsigned_octal_number(&p, end, 11, enc);
        if (num < 0) return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
        if (!PEND) {
          c2 = PPEEK;
          if (IS_CODE_DIGIT_ASCII(enc, c2))
            return ONIGERR_TOO_LONG_WIDE_CHAR_VALUE;
        }

        if (p > prev + enclen(enc, prev) && !PEND && (PPEEK_IS('}'))) {
          PINC;
          tok->type   = TK_CODE_POINT;
          tok->base   = 8;
          tok->u.code = (OnigCodePoint )num;
        }
        else {
          /* can't read nothing or invalid format */
          p = prev;
        }
      }
      break;

    case 'x':
      if (PEND) break;

      prev = p;
      if (PPEEK_IS('{') && IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_X_BRACE_HEX8)) {
        PINC;
        num = scan_unsigned_hexadecimal_number(&p, end, 0, 8, enc);
        if (num < 0) {
          if (num == ONIGERR_TOO_BIG_NUMBER)
            return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
          else
            return num;
        }
        if (!PEND) {
          c2 = PPEEK;
          if (IS_CODE_XDIGIT_ASCII(enc, c2))
            return ONIGERR_TOO_LONG_WIDE_CHAR_VALUE;
        }

        if (p > prev + enclen(enc, prev) && !PEND && (PPEEK_IS('}'))) {
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
        num = scan_unsigned_hexadecimal_number(&p, end, 0, 2, enc);
        if (num < 0) return num;
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
        num = scan_unsigned_hexadecimal_number(&p, end, 4, 4, enc);
        if (num < 0) return num;
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
        if (num < 0 || num >= 256) return ONIGERR_TOO_BIG_NUMBER;
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
      num = fetch_escaped_value(&p, end, env, &c2);
      if (num < 0) return num;
      if (tok->u.c != c2) {
        tok->u.code = c2;
        tok->type   = TK_CODE_POINT;
      }
      break;
    }
  }
  else if (c == '[') {
    if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_POSIX_BRACKET) && (PPEEK_IS(':'))) {
      OnigCodePoint send[] = { (OnigCodePoint )':', (OnigCodePoint )']' };
      tok->backp = p; /* point at '[' is read */
      PINC;
      if (str_exist_check_with_esc(send, 2, p, end,
                                   (OnigCodePoint )']', enc, syn)) {
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
  if (IS_MC_ESC_CODE(c, syn)) {
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
      tok->u.prop.ctype = ONIGENC_CTYPE_WORD;
      tok->u.prop.not   = 0;
      break;

    case 'W':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_W_WORD)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_WORD;
      tok->u.prop.not   = 1;
      break;

    case 'b':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_B_WORD_BOUND)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_WORD_BOUNDARY;
      break;

    case 'B':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_B_WORD_BOUND)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_NO_WORD_BOUNDARY;
      break;

    case 'y':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP2_ESC_X_Y_GRAPHEME_CLUSTER)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY;
      break;

    case 'Y':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP2_ESC_X_Y_GRAPHEME_CLUSTER)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY;
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
      tok->u.prop.ctype = ONIGENC_CTYPE_SPACE;
      tok->u.prop.not   = 0;
      break;

    case 'S':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_S_WHITE_SPACE)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_SPACE;
      tok->u.prop.not   = 1;
      break;

    case 'd':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_D_DIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_DIGIT;
      tok->u.prop.not   = 0;
      break;

    case 'D':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_D_DIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_DIGIT;
      tok->u.prop.not   = 1;
      break;

    case 'h':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_XDIGIT;
      tok->u.prop.not   = 0;
      break;

    case 'H':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_XDIGIT;
      tok->u.prop.not   = 1;
      break;

    case 'K':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_CAPITAL_K_KEEP)) break;
      tok->type = TK_KEEP;
      break;

    case 'R':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_CAPITAL_R_GENERAL_NEWLINE)) break;
      tok->type = TK_GENERAL_NEWLINE;
      break;

    case 'N':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_CAPITAL_N_O_SUPER_DOT)) break;
      tok->type = TK_NO_NEWLINE;
      break;

    case 'O':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_CAPITAL_N_O_SUPER_DOT)) break;
      tok->type = TK_TRUE_ANYCHAR;
      break;

    case 'X':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_X_Y_GRAPHEME_CLUSTER)) break;
      tok->type = TK_EXTENDED_GRAPHEME_CLUSTER;
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

    case 'o':
      if (PEND) break;

      prev = p;
      if (PPEEK_IS('{') && IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_O_BRACE_OCTAL)) {
        PINC;
        num = scan_unsigned_octal_number(&p, end, 11, enc);
        if (num < 0) return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
        if (!PEND) {
          if (IS_CODE_DIGIT_ASCII(enc, PPEEK))
            return ONIGERR_TOO_LONG_WIDE_CHAR_VALUE;
        }

        if ((p > prev + enclen(enc, prev)) && !PEND && PPEEK_IS('}')) {
          PINC;
          tok->type   = TK_CODE_POINT;
          tok->u.code = (OnigCodePoint )num;
        }
        else {
          /* can't read nothing or invalid format */
          p = prev;
        }
      }
      break;

    case 'x':
      if (PEND) break;

      prev = p;
      if (PPEEK_IS('{') && IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_X_BRACE_HEX8)) {
        PINC;
        num = scan_unsigned_hexadecimal_number(&p, end, 0, 8, enc);
        if (num < 0) {
          if (num == ONIGERR_TOO_BIG_NUMBER)
            return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
          else
            return num;
        }
        if (!PEND) {
          if (IS_CODE_XDIGIT_ASCII(enc, PPEEK))
            return ONIGERR_TOO_LONG_WIDE_CHAR_VALUE;
        }

        if ((p > prev + enclen(enc, prev)) && !PEND && PPEEK_IS('}')) {
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
        num = scan_unsigned_hexadecimal_number(&p, end, 0, 2, enc);
        if (num < 0) return num;
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
        num = scan_unsigned_hexadecimal_number(&p, end, 4, 4, enc);
        if (num < 0) return num;
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
          if (num > env->num_mem || IS_NULL(SCANENV_MEMENV(env)[num].node))
            return ONIGERR_INVALID_BACKREF;
        }

        tok->type = TK_BACKREF;
        tok->u.backref.num     = 1;
        tok->u.backref.ref1    = num;
        tok->u.backref.by_name = 0;
#ifdef USE_BACKREF_WITH_LEVEL
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
        if (num < 0 || num >= 256) return ONIGERR_TOO_BIG_NUMBER;
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

    case 'k':
      if (!PEND && IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_K_NAMED_BACKREF)) {
        PFETCH(c);
        if (c == '<' || c == '\'') {
          UChar* name_end;
          int* backs;
          int back_num;
          enum REF_NUM num_type;

          prev = p;

#ifdef USE_BACKREF_WITH_LEVEL
          name_end = NULL_UCHARP; /* no need. escape gcc warning. */
          r = fetch_name_with_level((OnigCodePoint )c, &p, end, &name_end,
                                 env, &back_num, &tok->u.backref.level, &num_type);
          if (r == 1) tok->u.backref.exist_level = 1;
          else        tok->u.backref.exist_level = 0;
#else
          r = fetch_name(c, &p, end, &name_end, env, &back_num, &num_type, 1);
#endif
          if (r < 0) return r;

          if (num_type != IS_NOT_NUM) {
            if (num_type == IS_REL_NUM) {
              back_num = backref_rel_to_abs(back_num, env);
            }
            if (back_num <= 0)
              return ONIGERR_INVALID_BACKREF;

            if (IS_SYNTAX_BV(syn, ONIG_SYN_STRICT_CHECK_BACKREF)) {
              if (back_num > env->num_mem ||
                  IS_NULL(SCANENV_MEMENV(env)[back_num].node))
                return ONIGERR_INVALID_BACKREF;
            }
            tok->type = TK_BACKREF;
            tok->u.backref.by_name = 0;
            tok->u.backref.num  = 1;
            tok->u.backref.ref1 = back_num;
          }
          else {
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
                    IS_NULL(SCANENV_MEMENV(env)[backs[i]].node))
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
        }
        else
          PUNFETCH;
      }
      break;

#ifdef USE_CALL
    case 'g':
      if (!PEND && IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_G_SUBEXP_CALL)) {
        PFETCH(c);
        if (c == '<' || c == '\'') {
          int gnum;
          UChar* name_end;
          enum REF_NUM num_type;

          prev = p;
          r = fetch_name((OnigCodePoint )c, &p, end, &name_end, env,
                         &gnum, &num_type, 1);
          if (r < 0) return r;

          if (num_type != IS_NOT_NUM) {
            if (num_type == IS_REL_NUM) {
              gnum = backref_rel_to_abs(gnum, env);
              if (gnum < 0) {
                onig_scan_env_set_error_string(env, ONIGERR_UNDEFINED_NAME_REFERENCE,
                                               prev, name_end);
                return ONIGERR_UNDEFINED_GROUP_REFERENCE;
              }
            }
            tok->u.call.by_number = 1;
            tok->u.call.gnum      = gnum;
          }
          else {
            tok->u.call.by_number = 0;
            tok->u.call.gnum      = 0;
          }

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
      if (!PEND && PPEEK_IS('{') &&
          IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY)) {
        PINC;
        tok->type = TK_CHAR_PROPERTY;
        tok->u.prop.not = (c == 'P' ? 1 : 0);

        if (!PEND &&
            IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT)) {
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
      {
        OnigCodePoint c2;

        PUNFETCH;
        num = fetch_escaped_value(&p, end, env, &c2);
        if (num < 0) return num;
        /* set_raw: */
        if (tok->u.c != c2) {
          tok->type = TK_CODE_POINT;
          tok->u.code = c2;
        }
        else { /* string */
          int len;
          SAFE_ENC_LEN(enc, tok->backp, end, len);
          p = tok->backp + len;
        }
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
      if (c == MC_ANYCHAR(syn))
        goto any_char;
      else if (c == MC_ANYTIME(syn))
        goto anytime;
      else if (c == MC_ZERO_OR_ONE_TIME(syn))
        goto zero_or_one_time;
      else if (c == MC_ONE_OR_MORE_TIME(syn))
        goto one_or_more_time;
      else if (c == MC_ANYCHAR_ANYTIME(syn)) {
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
      if (!PEND && PPEEK_IS('?') &&
          IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_QMARK_GROUP_EFFECT)) {
        PINC;
        if (! PEND) {
          c = PPEEK;
          if (c == '#') {
            PFETCH(c);
            while (1) {
              if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
              PFETCH(c);
              if (c == MC_ESC(syn)) {
                if (! PEND) PFETCH(c);
              }
              else {
                if (c == ')') break;
              }
            }
            goto start;
          }
          else if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_QMARK_PERL_SUBEXP_CALL)) {
            int gnum;
            UChar* name;
            UChar* name_end;
            enum REF_NUM num_type;

            switch (c) {
            case '&':
              {
                PINC;
                name = p;
                r = fetch_name((OnigCodePoint )'(', &p, end, &name_end, env, &gnum,
                               &num_type, 0);
                if (r < 0) return r;

                tok->type = TK_CALL;
                tok->u.call.by_number = 0;
                tok->u.call.gnum      = 0;
                tok->u.call.name      = name;
                tok->u.call.name_end  = name_end;
              }
              break;

            case 'R':
              tok->type = TK_CALL;
              tok->u.call.by_number = 1;
              tok->u.call.gnum      = 0;
              tok->u.call.name      = p;
              PINC;
              if (! PPEEK_IS(')')) return ONIGERR_INVALID_GROUP_NAME;
              tok->u.call.name_end  = p;
              break;

            case '-':
            case '+':
              goto lparen_qmark_num;
              break;
            default:
              if (! ONIGENC_IS_CODE_DIGIT(enc, c)) goto lparen_qmark_end;

            lparen_qmark_num:
              {
                name = p;
                r = fetch_name((OnigCodePoint )'(', &p, end, &name_end, env,
                               &gnum, &num_type, 1);
                if (r < 0) return r;

                if (num_type == IS_NOT_NUM) {
                  return ONIGERR_INVALID_GROUP_NAME;
                }
                else {
                  if (num_type == IS_REL_NUM) {
                    gnum = backref_rel_to_abs(gnum, env);
                    if (gnum < 0) {
                      onig_scan_env_set_error_string(env,
                             ONIGERR_UNDEFINED_NAME_REFERENCE, name, name_end);
                      return ONIGERR_UNDEFINED_GROUP_REFERENCE;
                    }
                  }
                  tok->u.call.by_number = 1;
                  tok->u.call.gnum      = gnum;
                }

                tok->type = TK_CALL;
                tok->u.call.name     = name;
                tok->u.call.name_end = name_end;
              }
              break;
            }
          }
        }
      lparen_qmark_end:
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
      tok->u.subtype = (IS_SINGLELINE(env->options)
                        ? ANCHOR_BEGIN_BUF : ANCHOR_BEGIN_LINE);
      break;

    case '$':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_LINE_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.subtype = (IS_SINGLELINE(env->options)
                        ? ANCHOR_SEMI_END_BUF : ANCHOR_END_LINE);
      break;

    case '[':
      if (! IS_SYNTAX_OP(syn, ONIG_SYN_OP_BRACKET_CC)) break;
      tok->type = TK_CC_OPEN;
      break;

    case ']':
      if (*src > env->pattern)   /* /].../ is allowed. */
        CLOSE_BRACKET_WITHOUT_ESC_WARN(env, (UChar* )"]");
      break;

    case '#':
      if (IS_EXTEND(env->options)) {
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
      if (IS_EXTEND(env->options))
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
add_ctype_to_cc_by_range(CClassNode* cc, int ctype ARG_UNUSED, int not,
                         OnigEncoding enc ARG_UNUSED, OnigCodePoint sb_out,
                         const OnigCodePoint mbr[])
{
  int i, r;
  OnigCodePoint j;

  int n = ONIGENC_CODE_RANGE_NUM(mbr);

  if (not == 0) {
    for (i = 0; i < n; i++) {
      for (j  = ONIGENC_CODE_RANGE_FROM(mbr, i);
           j <= ONIGENC_CODE_RANGE_TO(mbr, i); j++) {
        if (j >= sb_out) {
          if (j > ONIGENC_CODE_RANGE_FROM(mbr, i)) {
            r = add_code_range_to_buf(&(cc->mbuf), j,
                                      ONIGENC_CODE_RANGE_TO(mbr, i));
            if (r != 0) return r;
            i++;
          }

          goto sb_end;
        }
        BITSET_SET_BIT(cc->bs, j);
      }
    }

  sb_end:
    for ( ; i < n; i++) {
      r = add_code_range_to_buf(&(cc->mbuf),
                                ONIGENC_CODE_RANGE_FROM(mbr, i),
                                ONIGENC_CODE_RANGE_TO(mbr, i));
      if (r != 0) return r;
    }
  }
  else {
    OnigCodePoint prev = 0;

    for (i = 0; i < n; i++) {
      for (j = prev; j < ONIGENC_CODE_RANGE_FROM(mbr, i); j++) {
        if (j >= sb_out) {
          goto sb_end2;
        }
        BITSET_SET_BIT(cc->bs, j);
      }
      prev = ONIGENC_CODE_RANGE_TO(mbr, i) + 1;
    }
    for (j = prev; j < sb_out; j++) {
      BITSET_SET_BIT(cc->bs, j);
    }

  sb_end2:
    prev = sb_out;

    for (i = 0; i < n; i++) {
      if (prev < ONIGENC_CODE_RANGE_FROM(mbr, i)) {
        r = add_code_range_to_buf(&(cc->mbuf), prev,
                                  ONIGENC_CODE_RANGE_FROM(mbr, i) - 1);
        if (r != 0) return r;
      }
      prev = ONIGENC_CODE_RANGE_TO(mbr, i) + 1;
      if (prev == 0) goto end;
    }

    r = add_code_range_to_buf(&(cc->mbuf), prev, MAX_CODE_POINT);
    if (r != 0) return r;
  }

 end:
  return 0;
}

static int
add_ctype_to_cc_by_range_limit(CClassNode* cc, int ctype ARG_UNUSED, int not,
                               OnigEncoding enc ARG_UNUSED,
                               OnigCodePoint sb_out,
                               const OnigCodePoint mbr[], OnigCodePoint limit)
{
  int i, r;
  OnigCodePoint j;
  OnigCodePoint from;
  OnigCodePoint to;

  int n = ONIGENC_CODE_RANGE_NUM(mbr);

  if (not == 0) {
    for (i = 0; i < n; i++) {
      for (j  = ONIGENC_CODE_RANGE_FROM(mbr, i);
           j <= ONIGENC_CODE_RANGE_TO(mbr, i); j++) {
        if (j > limit) goto end;
        if (j >= sb_out) {
          if (j > ONIGENC_CODE_RANGE_FROM(mbr, i)) {
            to = ONIGENC_CODE_RANGE_TO(mbr, i);
            if (to > limit) to = limit;
            r = add_code_range_to_buf(&(cc->mbuf), j, to);
            if (r != 0) return r;
            i++;
          }

          goto sb_end;
        }
        BITSET_SET_BIT(cc->bs, j);
      }
    }

  sb_end:
    for ( ; i < n; i++) {
      from = ONIGENC_CODE_RANGE_FROM(mbr, i);
      to   = ONIGENC_CODE_RANGE_TO(mbr, i);
      if (from > limit) break;
      if (to   > limit) to = limit;
      r = add_code_range_to_buf(&(cc->mbuf), from, to);
      if (r != 0) return r;
    }
  }
  else {
    OnigCodePoint prev = 0;

    for (i = 0; i < n; i++) {
      from = ONIGENC_CODE_RANGE_FROM(mbr, i);
      if (from > limit) {
        for (j = prev; j < sb_out; j++) {
          BITSET_SET_BIT(cc->bs, j);
        }
        goto sb_end2;
      }
      for (j = prev; j < from; j++) {
        if (j >= sb_out) goto sb_end2;
        BITSET_SET_BIT(cc->bs, j);
      }
      prev = ONIGENC_CODE_RANGE_TO(mbr, i);
      if (prev > limit) prev = limit;
      prev++;
      if (prev == 0) goto end;
    }
    for (j = prev; j < sb_out; j++) {
      BITSET_SET_BIT(cc->bs, j);
    }

  sb_end2:
    prev = sb_out;

    for (i = 0; i < n; i++) {
      from = ONIGENC_CODE_RANGE_FROM(mbr, i);
      if (from > limit) goto last;

      if (prev < from) {
        r = add_code_range_to_buf(&(cc->mbuf), prev, from - 1);
        if (r != 0) return r;
      }
      prev = ONIGENC_CODE_RANGE_TO(mbr, i);
      if (prev > limit) prev = limit;
      prev++;
      if (prev == 0) goto end;
    }

  last:
    r = add_code_range_to_buf(&(cc->mbuf), prev, MAX_CODE_POINT);
    if (r != 0) return r;
  }

 end:
  return 0;
}

static int
add_ctype_to_cc(CClassNode* cc, int ctype, int not, ScanEnv* env)
{
#define ASCII_LIMIT    127

  int c, r;
  int ascii_mode;
  const OnigCodePoint *ranges;
  OnigCodePoint limit;
  OnigCodePoint sb_out;
  OnigEncoding enc = env->enc;

  ascii_mode = IS_ASCII_MODE_CTYPE_OPTION(ctype, env->options);

  r = ONIGENC_GET_CTYPE_CODE_RANGE(enc, ctype, &sb_out, &ranges);
  if (r == 0) {
    if (ascii_mode == 0)
      r = add_ctype_to_cc_by_range(cc, ctype, not, env->enc, sb_out, ranges);
    else
      r = add_ctype_to_cc_by_range_limit(cc, ctype, not, env->enc, sb_out,
                                         ranges, ASCII_LIMIT);
    return r;
  }
  else if (r != ONIG_NO_SUPPORT_CONFIG) {
    return r;
  }

  r = 0;
  limit = ascii_mode ? ASCII_LIMIT : SINGLE_BYTE_SIZE;

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
      for (c = 0; c < (int )limit; c++) {
        if (! ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
          BITSET_SET_BIT(cc->bs, c);
      }
      for (c = limit; c < SINGLE_BYTE_SIZE; c++) {
        BITSET_SET_BIT(cc->bs, c);
      }

      ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    else {
      for (c = 0; c < (int )limit; c++) {
        if (ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
          BITSET_SET_BIT(cc->bs, c);
      }
    }
    break;

  case ONIGENC_CTYPE_GRAPH:
  case ONIGENC_CTYPE_PRINT:
  case ONIGENC_CTYPE_WORD:
    if (not != 0) {
      for (c = 0; c < (int )limit; c++) {
        if (ONIGENC_CODE_TO_MBCLEN(enc, c) > 0 /* check invalid code point */
            && ! ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
          BITSET_SET_BIT(cc->bs, c);
      }
      for (c = limit; c < SINGLE_BYTE_SIZE; c++) {
        if (ONIGENC_CODE_TO_MBCLEN(enc, c) > 0)
          BITSET_SET_BIT(cc->bs, c);
      }
    }
    else {
      for (c = 0; c < (int )limit; c++) {
        if (ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
          BITSET_SET_BIT(cc->bs, c);
      }
      if (ascii_mode == 0)
        ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    break;

  default:
    return ONIGERR_PARSER_BUG;
    break;
  }

  return r;
}

static int
parse_posix_bracket(CClassNode* cc, UChar** src, UChar* end, ScanEnv* env)
{
#define POSIX_BRACKET_CHECK_LIMIT_LENGTH  20
#define POSIX_BRACKET_NAME_MIN_LEN         4

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
    { (UChar* )"word",   ONIGENC_CTYPE_WORD,   4 },
    { (UChar* )NULL,     -1, 0 }
  };

  PosixBracketEntryType *pb;
  int not, i, r;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar *p = *src;

  if (PPEEK_IS('^')) {
    PINC_S;
    not = 1;
  }
  else
    not = 0;

  if (onigenc_strlen(enc, p, end) < POSIX_BRACKET_NAME_MIN_LEN + 3)
    goto not_posix_bracket;

  for (pb = PBS; IS_NOT_NULL(pb->name); pb++) {
    if (onigenc_with_ascii_strncmp(enc, p, end, pb->name, pb->len) == 0) {
      p = (UChar* )onigenc_step(enc, p, end, pb->len);
      if (onigenc_with_ascii_strncmp(enc, p, end, (UChar* )":]", 2) != 0)
        return ONIGERR_INVALID_POSIX_BRACKET_TYPE;

      r = add_ctype_to_cc(cc, pb->ctype, not, env);
      if (r != 0) return r;

      PINC_S; PINC_S;
      *src = p;
      return 0;
    }
  }

 not_posix_bracket:
  c = 0;
  i = 0;
  while (!PEND && ((c = PPEEK) != ':') && c != ']') {
    PINC_S;
    if (++i > POSIX_BRACKET_CHECK_LIMIT_LENGTH) break;
  }
  if (c == ':' && ! PEND) {
    PINC_S;
    if (! PEND) {
      PFETCH_S(c);
      if (c == ']')
        return ONIGERR_INVALID_POSIX_BRACKET_TYPE;
    }
  }

  return 1;  /* 1: is not POSIX bracket, but no error. */
}

static int
fetch_char_property_to_ctype(UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar *prev, *start, *p = *src;

  r = 0;
  start = prev = p;

  while (!PEND) {
    prev = p;
    PFETCH_S(c);
    if (c == '}') {
      r = ONIGENC_PROPERTY_NAME_TO_CTYPE(enc, start, prev);
      if (r < 0) break;

      *src = p;
      return r;
    }
    else if (c == '(' || c == ')' || c == '{' || c == '|') {
      r = ONIGERR_INVALID_CHAR_PROPERTY_NAME;
      break;
    }
  }

  onig_scan_env_set_error_string(env, r, *src, prev);
  return r;
}

static int
parse_char_property(Node** np, OnigToken* tok, UChar** src, UChar* end, ScanEnv* env)
{
  int r, ctype;
  CClassNode* cc;

  ctype = fetch_char_property_to_ctype(src, end, env);
  if (ctype < 0) return ctype;

  *np = node_new_cclass();
  CHECK_NULL_RETURN_MEMERR(*np);
  cc = CCLASS_(*np);
  r = add_ctype_to_cc(cc, ctype, 0, env);
  if (r != 0) return r;
  if (tok->u.prop.not != 0) NCCLASS_SET_NOT(cc);

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
next_state_val(CClassNode* cc, OnigCodePoint *from, OnigCodePoint to,
               int* from_israw, int to_israw,
               enum CCVALTYPE intype, enum CCVALTYPE* type,
               enum CCSTATE* state, ScanEnv* env)
{
  int r;

  switch (*state) {
  case CCS_VALUE:
    if (*type == CCV_SB) {
      if (*from > 0xff)
          return ONIGERR_INVALID_CODE_POINT_VALUE;

      BITSET_SET_BIT(cc->bs, (int )(*from));
    }
    else if (*type == CCV_CODE_POINT) {
      r = add_code_range(&(cc->mbuf), env, *from, *from);
      if (r < 0) return r;
    }
    break;

  case CCS_RANGE:
    if (intype == *type) {
      if (intype == CCV_SB) {
        if (*from > 0xff || to > 0xff)
          return ONIGERR_INVALID_CODE_POINT_VALUE;

        if (*from > to) {
          if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
            goto ccs_range_end;
          else
            return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
        }
        bitset_set_range(cc->bs, (int )*from, (int )to);
      }
      else {
        r = add_code_range(&(cc->mbuf), env, *from, to);
        if (r < 0) return r;
      }
    }
    else {
      if (*from > to) {
        if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
          goto ccs_range_end;
        else
          return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
      }
      bitset_set_range(cc->bs, (int )*from, (int )(to < 0xff ? to : 0xff));
      r = add_code_range(&(cc->mbuf), env, (OnigCodePoint )*from, to);
      if (r < 0) return r;
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

  *from_israw = to_israw;
  *from       = to;
  *type       = intype;
  return 0;
}

static int
code_exist_check(OnigCodePoint c, UChar* from, UChar* end, int ignore_escaped,
                 ScanEnv* env)
{
  int in_esc;
  OnigCodePoint code;
  OnigEncoding enc = env->enc;
  UChar* p = from;

  in_esc = 0;
  while (! PEND) {
    if (ignore_escaped && in_esc) {
      in_esc = 0;
    }
    else {
      PFETCH_S(code);
      if (code == c) return 1;
      if (code == MC_ESC(env->syntax)) in_esc = 1;
    }
  }
  return 0;
}

static int
parse_char_class(Node** np, OnigToken* tok, UChar** src, UChar* end, ScanEnv* env)
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

  *np = NULL_NODE;
  env->parse_depth++;
  if (env->parse_depth > ParseDepthLimit)
    return ONIGERR_PARSE_DEPTH_LIMIT_OVER;
  prev_cc = (CClassNode* )NULL;
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
                           *src, env->pattern_end, 1, env))
      return ONIGERR_EMPTY_CHAR_CLASS;

    CC_ESC_WARN(env, (UChar* )"]");
    r = tok->type = TK_CHAR;  /* allow []...] */
  }

  *np = node = node_new_cclass();
  CHECK_NULL_RETURN_MEMERR(node);
  cc = CCLASS_(node);

  and_start = 0;
  state = CCS_START;
  p = *src;
  while (r != TK_CC_CLOSE) {
    fetched = 0;
    switch (r) {
    case TK_CHAR:
    any_char_in:
      len = ONIGENC_CODE_TO_MBCLEN(env->enc, tok->u.c);
      if (len > 1) {
        in_type = CCV_CODE_POINT;
      }
      else if (len < 0) {
        r = len;
        goto err;
      }
      else {
        /* sb_char: */
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

        len = enclen(env->enc, buf);
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
      r = add_ctype_to_cc(cc, tok->u.prop.ctype, tok->u.prop.not, env);
      if (r != 0) goto err;

    next_class:
      r = next_state_class(cc, &vs, &val_type, &state, env);
      if (r != 0) goto err;
      break;

    case TK_CHAR_PROPERTY:
      {
        int ctype = fetch_char_property_to_ctype(&p, end, env);
        if (ctype < 0) {
          r = ctype;
          goto err;
        }
        r = add_ctype_to_cc(cc, ctype, tok->u.prop.not, env);
        if (r != 0) goto err;
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

        if (val_type == CCV_CLASS) {
          r = ONIGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS;
          goto err;
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
        goto any_char_in;  /* [!--x] is allowed */
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
          goto range_end_val;   /* [0-9-a] is allowed as [0-9\-a] */
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
        if (r != 0) {
          onig_node_free(anode);
          goto cc_open_err;
        }
        acc = CCLASS_(anode);
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
    NCCLASS_SET_NOT(cc);
  else
    NCCLASS_CLEAR_NOT(cc);
  if (IS_NCCLASS_NOT(cc) &&
      IS_SYNTAX_BV(env->syntax, ONIG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC)) {
    int is_empty = (IS_NULL(cc->mbuf) ? 1 : 0);
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
  env->parse_depth--;
  return 0;

 err:
  if (cc != CCLASS_(*np))
    bbuf_free(cc->mbuf);
  return r;
}

static int parse_subexp(Node** top, OnigToken* tok, int term,
                        UChar** src, UChar* end, ScanEnv* env);

#ifdef USE_CALLOUT

/* (?{...}[tag][+-]) (?{{...}}[tag][+-]) */
static int
parse_callout_of_contents(Node** np, int cterm, UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  int i;
  int in;
  int num;
  OnigCodePoint c;
  UChar* code_start;
  UChar* code_end;
  UChar* contents;
  UChar* tag_start;
  UChar* tag_end;
  int brace_nest;
  CalloutListEntry* e;
  RegexExt* ext;
  OnigEncoding enc = env->enc;
  UChar* p = *src;

  if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;

  brace_nest = 0;
  while (PPEEK_IS('{')) {
    brace_nest++;
    PINC_S;
    if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;
  }

  in = ONIG_CALLOUT_IN_PROGRESS;
  code_start = p;
  while (1) {
    if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;

    code_end = p;
    PFETCH_S(c);
    if (c == '}') {
      i = brace_nest;
      while (i > 0) {
        if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;
        PFETCH_S(c);
        if (c == '}') i--;
        else break;
      }
      if (i == 0) break;
    }
  }

  if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

  PFETCH_S(c);
  if (c == '[') {
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    tag_start = p;
    while (! PEND) {
      if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
      tag_end = p;
      PFETCH_S(c);
      if (c == ']') break;
    }
    if (! is_allowed_callout_tag_name(enc, tag_start, tag_end))
      return ONIGERR_INVALID_CALLOUT_TAG_NAME;

    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }
  else {
    tag_start = tag_end = 0;
  }

  if (c == 'X') {
    in |= ONIG_CALLOUT_IN_RETRACTION;
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }
  else if (c == '<') {
    in = ONIG_CALLOUT_IN_RETRACTION;
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }
  else if (c == '>') { /* no needs (default) */
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }

  if (c != cterm)
    return ONIGERR_INVALID_CALLOUT_PATTERN;

  r = reg_callout_list_entry(env, &num);
  if (r != 0) return r;

  ext = onig_get_regex_ext(env->reg);
  if (IS_NULL(ext->pattern)) {
    r = onig_ext_set_pattern(env->reg, env->pattern, env->pattern_end);
    if (r != ONIG_NORMAL) return r;
  }

  if (tag_start != tag_end) {
    r = callout_tag_entry(env->reg, tag_start, tag_end, num);
    if (r != ONIG_NORMAL) return r;
  }

  contents = onigenc_strdup(enc, code_start, code_end);
  CHECK_NULL_RETURN_MEMERR(contents);

  r = node_new_callout(np, ONIG_CALLOUT_OF_CONTENTS, num, ONIG_NON_NAME_ID, env);
  if (r != 0) {
    xfree(contents);
    return r;
  }

  e = onig_reg_callout_list_at(env->reg, num);
  e->of      = ONIG_CALLOUT_OF_CONTENTS;
  e->in      = in;
  e->name_id = ONIG_NON_NAME_ID;
  e->u.content.start = contents;
  e->u.content.end   = contents + (code_end - code_start);

  *src = p;
  return 0;
}

static long
parse_long(OnigEncoding enc, UChar* s, UChar* end, int sign_on, long max, long* rl)
{
  long v;
  long d;
  int flag;
  UChar* p;
  OnigCodePoint c;

  if (s >= end) return ONIGERR_INVALID_CALLOUT_ARG;

  flag = 1;
  v = 0;
  p = s;
  while (p < end) {
    c = ONIGENC_MBC_TO_CODE(enc, p, end);
    p += ONIGENC_MBC_ENC_LEN(enc, p);
    if (c >= '0' && c <= '9') {
      d = (long )(c - '0');
      if (v > (max - d) / 10)
        return ONIGERR_INVALID_CALLOUT_ARG;

      v = v * 10 + d;
    }
    else if (sign_on != 0 && (c == '-' || c == '+')) {
      if (c == '-') flag = -1;
    }
    else
      return ONIGERR_INVALID_CALLOUT_ARG;

    sign_on = 0;
  }

  *rl = flag * v;
  return ONIG_NORMAL;
}

static int
parse_callout_args(int skip_mode, int cterm, UChar** src, UChar* end,
                   unsigned int types[], OnigValue vals[], ScanEnv* env)
{
#define MAX_CALLOUT_ARG_BYTE_LENGTH   128

  int r;
  int n;
  int esc;
  int cn;
  UChar* s;
  UChar* e;
  UChar* eesc;
  OnigCodePoint c;
  UChar* bufend;
  UChar buf[MAX_CALLOUT_ARG_BYTE_LENGTH];
  OnigEncoding enc = env->enc;
  UChar* p = *src;

  if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;

  n = 0;
  while (n < ONIG_CALLOUT_MAX_ARGS_NUM) {
    c   = 0;
    cn  = 0;
    esc = 0;
    eesc = 0;
    bufend = buf;
    s = e = p;
    while (1) {
      if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;

      e = p;
      PFETCH_S(c);
      if (esc != 0) {
        esc = 0;
        if (c == '\\' || c == cterm || c == ',') {
          /* */
        }
        else {
          e = eesc;
          cn++;
        }
        goto add_char;
      }
      else {
        if (c == '\\') {
          esc = 1;
          eesc = e;
        }
        else if (c == cterm || c == ',')
          break;
        else {
          size_t clen;

        add_char:
          if (skip_mode == 0) {
            clen = p - e;
            if (bufend + clen > buf + MAX_CALLOUT_ARG_BYTE_LENGTH)
              return ONIGERR_INVALID_CALLOUT_ARG; /* too long argument */

            xmemcpy(bufend, e, clen);
            bufend += clen;
          }
          cn++;
        }
      }
    }

    if (cn != 0) {
      if (skip_mode == 0) {
        if ((types[n] & ONIG_TYPE_LONG) != 0) {
          int fixed = 0;
          if (cn > 0) {
            long rl;
            r = parse_long(enc, buf, bufend, 1, LONG_MAX, &rl);
            if (r == ONIG_NORMAL) {
              vals[n].l = rl;
              fixed = 1;
              types[n] = ONIG_TYPE_LONG;
            }
          }

          if (fixed == 0) {
            types[n] = (types[n] & ~ONIG_TYPE_LONG);
            if (types[n] == ONIG_TYPE_VOID)
              return ONIGERR_INVALID_CALLOUT_ARG;
          }
        }

        switch (types[n]) {
        case ONIG_TYPE_LONG:
          break;

        case ONIG_TYPE_CHAR:
          if (cn != 1) return ONIGERR_INVALID_CALLOUT_ARG;
          vals[n].c = ONIGENC_MBC_TO_CODE(enc, buf, bufend);
          break;

        case ONIG_TYPE_STRING:
          {
            UChar* rs = onigenc_strdup(enc, buf, bufend);
            CHECK_NULL_RETURN_MEMERR(rs);
            vals[n].s.start = rs;
            vals[n].s.end   = rs + (e - s);
          }
          break;

        case ONIG_TYPE_TAG:
          if (eesc != 0 || ! is_allowed_callout_tag_name(enc, s, e))
            return ONIGERR_INVALID_CALLOUT_TAG_NAME;

          vals[n].s.start = s;
          vals[n].s.end   = e;
          break;

        case ONIG_TYPE_VOID:
        case ONIG_TYPE_POINTER:
          return ONIGERR_PARSER_BUG;
          break;
        }
      }

      n++;
    }

    if (c == cterm) break;
  }

  if (c != cterm) return ONIGERR_INVALID_CALLOUT_PATTERN;

  *src = p;
  return n;
}

/* (*name[TAG]) (*name[TAG]{a,b,..}) */
static int
parse_callout_of_name(Node** np, int cterm, UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  int i;
  int in;
  int num;
  int name_id;
  int arg_num;
  int max_arg_num;
  int opt_arg_num;
  int is_not_single;
  OnigCodePoint c;
  UChar* name_start;
  UChar* name_end;
  UChar* tag_start;
  UChar* tag_end;
  Node*  node;
  CalloutListEntry* e;
  RegexExt* ext;
  unsigned int types[ONIG_CALLOUT_MAX_ARGS_NUM];
  OnigValue    vals[ONIG_CALLOUT_MAX_ARGS_NUM];
  OnigEncoding enc = env->enc;
  UChar* p = *src;

  /* PFETCH_READY; */
  if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;

  node = 0;
  name_start = p;
  while (1) {
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    name_end = p;
    PFETCH_S(c);
    if (c == cterm || c == '[' || c == '{') break;
  }

  if (! is_allowed_callout_name(enc, name_start, name_end))
    return ONIGERR_INVALID_CALLOUT_NAME;

  if (c == '[') {
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    tag_start = p;
    while (! PEND) {
      if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
      tag_end = p;
      PFETCH_S(c);
      if (c == ']') break;
    }
    if (! is_allowed_callout_tag_name(enc, tag_start, tag_end))
      return ONIGERR_INVALID_CALLOUT_TAG_NAME;

    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }
  else {
    tag_start = tag_end = 0;
  }

  if (c == '{') {
    UChar* save;

    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

    /* read for single check only */
    save = p;
    arg_num = parse_callout_args(1, '}', &p, end, 0, 0, env);
    if (arg_num < 0) return arg_num;

    is_not_single = PPEEK_IS(cterm) ?  0 : 1;
    p = save;
    r = get_callout_name_id_by_name(enc, is_not_single, name_start, name_end,
                                    &name_id);
    if (r != ONIG_NORMAL) return r;

    max_arg_num = get_callout_arg_num_by_name_id(name_id);
    for (i = 0; i < max_arg_num; i++) {
      types[i] = get_callout_arg_type_by_name_id(name_id, i);
    }

    arg_num = parse_callout_args(0, '}', &p, end, types, vals, env);
    if (arg_num < 0) return arg_num;

    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
    PFETCH_S(c);
  }
  else {
    arg_num = 0;

    is_not_single = 0;
    r = get_callout_name_id_by_name(enc, is_not_single, name_start, name_end,
                                      &name_id);
    if (r != ONIG_NORMAL) return r;

    max_arg_num = get_callout_arg_num_by_name_id(name_id);
    for (i = 0; i < max_arg_num; i++) {
      types[i] = get_callout_arg_type_by_name_id(name_id, i);
    }
  }

  in = onig_get_callout_in_by_name_id(name_id);
  opt_arg_num = get_callout_opt_arg_num_by_name_id(name_id);
  if (arg_num > max_arg_num || arg_num < (max_arg_num - opt_arg_num))
    return ONIGERR_INVALID_CALLOUT_ARG;

  if (c != cterm)
    return ONIGERR_INVALID_CALLOUT_PATTERN;

  r = reg_callout_list_entry(env, &num);
  if (r != 0) return r;

  ext = onig_get_regex_ext(env->reg);
  if (IS_NULL(ext->pattern)) {
    r = onig_ext_set_pattern(env->reg, env->pattern, env->pattern_end);
    if (r != ONIG_NORMAL) return r;
  }

  if (tag_start != tag_end) {
    r = callout_tag_entry(env->reg, tag_start, tag_end, num);
    if (r != ONIG_NORMAL) return r;
  }

  r = node_new_callout(&node, ONIG_CALLOUT_OF_NAME, num, name_id, env);
  if (r != ONIG_NORMAL) return r;

  e = onig_reg_callout_list_at(env->reg, num);
  e->of         = ONIG_CALLOUT_OF_NAME;
  e->in         = in;
  e->name_id    = name_id;
  e->type       = onig_get_callout_type_by_name_id(name_id);
  e->start_func = onig_get_callout_start_func_by_name_id(name_id);
  e->end_func   = onig_get_callout_end_func_by_name_id(name_id);
  e->u.arg.num        = max_arg_num;
  e->u.arg.passed_num = arg_num;
  for (i = 0; i < max_arg_num; i++) {
    e->u.arg.types[i] = types[i];
    if (i < arg_num)
      e->u.arg.vals[i] = vals[i];
    else
      e->u.arg.vals[i] = get_callout_opt_default_by_name_id(name_id, i);
  }

  *np = node;
  *src = p;
  return 0;
}
#endif

static int
parse_enclosure(Node** np, OnigToken* tok, int term, UChar** src, UChar* end,
                ScanEnv* env)
{
  int r, num;
  Node *target;
  OnigOptionType option;
  OnigCodePoint c;
  int list_capture;
  OnigEncoding enc = env->enc;

  UChar* p = *src;
  PFETCH_READY;

  *np = NULL;
  if (PEND) return ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS;

  option = env->options;
  c = PPEEK;
  if (c == '?' && IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_GROUP_EFFECT)) {
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
      *np = onig_node_new_anchor(ANCHOR_PREC_READ, 0);
      break;
    case '!':  /*         preceding read */
      *np = onig_node_new_anchor(ANCHOR_PREC_READ_NOT, 0);
      break;
    case '>':            /* (?>...) stop backtrack */
      *np = node_new_enclosure(ENCLOSURE_STOP_BACKTRACK);
      break;

    case '\'':
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP)) {
        goto named_group1;
      }
      else
        return ONIGERR_UNDEFINED_GROUP_OPTION;
      break;

    case '<':   /* look behind (?<=...), (?<!...) */
      if (PEND) return ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS;
      PFETCH(c);
      if (c == '=')
        *np = onig_node_new_anchor(ANCHOR_LOOK_BEHIND, 0);
      else if (c == '!')
        *np = onig_node_new_anchor(ANCHOR_LOOK_BEHIND_NOT, 0);
      else {
        if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP)) {
          UChar *name;
          UChar *name_end;
          enum REF_NUM num_type;

          PUNFETCH;
          c = '<';

        named_group1:
          list_capture = 0;

        named_group2:
          name = p;
          r = fetch_name((OnigCodePoint )c, &p, end, &name_end, env, &num,
                         &num_type, 0);
          if (r < 0) return r;

          num = scan_env_add_mem_entry(env);
          if (num < 0) return num;
          if (list_capture != 0 && num >= (int )MEM_STATUS_BITS_NUM)
            return ONIGERR_GROUP_NUMBER_OVER_FOR_CAPTURE_HISTORY;

          r = name_add(env->reg, name, name_end, num, env);
          if (r != 0) return r;
          *np = node_new_memory(1);
          CHECK_NULL_RETURN_MEMERR(*np);
          ENCLOSURE_(*np)->m.regnum = num;
          if (list_capture != 0)
            MEM_STATUS_ON_SIMPLE(env->capture_history, num);
          env->num_named++;
        }
        else {
          return ONIGERR_UNDEFINED_GROUP_OPTION;
        }
      }
      break;

    case '~':
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_TILDE_ABSENT_GROUP)) {
        Node* absent;
        Node* expr;
        int head_bar;
        int is_range_cutter;

        if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

        if (PPEEK_IS('|')) { /* (?~|generator|absent) */
          PINC;
          if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

          head_bar = 1;
          if (PPEEK_IS(')')) { /* (?~|)  : range clear */
            PINC;
            r = make_range_clear(np, env);
            if (r != 0) return r;
            goto end;
          }
        }
        else
          head_bar = 0;

        r = fetch_token(tok, &p, end, env);
        if (r < 0) return r;
        r = parse_subexp(&absent, tok, term, &p, end, env);
        if (r < 0) {
          onig_node_free(absent);
          return r;
        }

        expr = NULL_NODE;
        is_range_cutter = 0;
        if (head_bar != 0) {
          Node* top = absent;
          if (NODE_TYPE(top) != NODE_ALT || IS_NULL(NODE_CDR(top))) {
            expr = NULL_NODE;
            is_range_cutter = 1;
            /* return ONIGERR_INVALID_ABSENT_GROUP_GENERATOR_PATTERN; */
          }
          else {
            absent = NODE_CAR(top);
            expr   = NODE_CDR(top);
            NODE_CAR(top) = NULL_NODE;
            NODE_CDR(top) = NULL_NODE;
            onig_node_free(top);
            if (IS_NULL(NODE_CDR(expr))) {
              top = expr;
              expr = NODE_CAR(top);
              NODE_CAR(top) = NULL_NODE;
              onig_node_free(top);
            }
          }
        }

        r = make_absent_tree(np, absent, expr, is_range_cutter, env);
        if (r != 0) {
          return r;
        }
        goto end;
      }
      else {
        return ONIGERR_UNDEFINED_GROUP_OPTION;
      }
      break;

#ifdef USE_CALLOUT
    case '{':
      if (! IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_BRACE_CALLOUT_CONTENTS))
        return ONIGERR_UNDEFINED_GROUP_OPTION;

      r = parse_callout_of_contents(np, ')', &p, end, env);
      if (r != 0) return r;

      goto end;
      break;
#endif

    case '(':
      /* (?()...) */
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_LPAREN_IF_ELSE)) {
        UChar *prev;
        Node* condition;
        int condition_is_checker;

        if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
        PFETCH(c);
        if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

        if (IS_CODE_DIGIT_ASCII(enc, c)
            || c == '-' || c == '+' || c == '<' || c == '\'') {
          UChar* name_end;
          int back_num;
          int exist_level;
          int level;
          enum REF_NUM num_type;
          int is_enclosed;

          is_enclosed = (c == '<' || c == '\'') ? 1 : 0;
          if (! is_enclosed)
            PUNFETCH;
          prev = p;
          exist_level = 0;
#ifdef USE_BACKREF_WITH_LEVEL
          name_end = NULL_UCHARP; /* no need. escape gcc warning. */
          r = fetch_name_with_level(
                    (OnigCodePoint )(is_enclosed != 0 ? c : '('),
                    &p, end, &name_end,
                    env, &back_num, &level, &num_type);
          if (r == 1) exist_level = 1;
#else
          r = fetch_name((OnigCodePoint )(is_enclosed != 0 ? c : '('),
                         &p, end, &name_end, env, &back_num, &num_type, 1);
#endif
          if (r < 0) {
            if (is_enclosed == 0) {
              goto any_condition;
            }
            else
              return r;
          }

          condition_is_checker = 1;
          if (num_type != IS_NOT_NUM) {
            if (num_type == IS_REL_NUM) {
              back_num = backref_rel_to_abs(back_num, env);
            }
            if (back_num <= 0)
              return ONIGERR_INVALID_BACKREF;

            if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_STRICT_CHECK_BACKREF)) {
              if (back_num > env->num_mem ||
                  IS_NULL(SCANENV_MEMENV(env)[back_num].node))
                return ONIGERR_INVALID_BACKREF;
            }

            condition = node_new_backref_checker(1, &back_num, 0,
#ifdef USE_BACKREF_WITH_LEVEL
                                                 exist_level, level,
#endif
                                                 env);
          }
          else {
            int num;
            int* backs;

            num = onig_name_to_group_numbers(env->reg, prev, name_end, &backs);
            if (num <= 0) {
              onig_scan_env_set_error_string(env,
                        ONIGERR_UNDEFINED_NAME_REFERENCE, prev, name_end);
              return ONIGERR_UNDEFINED_NAME_REFERENCE;
            }
            if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_STRICT_CHECK_BACKREF)) {
              int i;
              for (i = 0; i < num; i++) {
                if (backs[i] > env->num_mem ||
                    IS_NULL(SCANENV_MEMENV(env)[backs[i]].node))
                  return ONIGERR_INVALID_BACKREF;
              }
            }

            condition = node_new_backref_checker(num, backs, 1,
#ifdef USE_BACKREF_WITH_LEVEL
                                                 exist_level, level,
#endif
                                                 env);
          }

          if (is_enclosed != 0) {
            if (PEND) goto err_if_else;
            PFETCH(c);
            if (c != ')') goto err_if_else;
          }
        }
#ifdef USE_CALLOUT
        else if (c == '?') {
          if (IS_SYNTAX_OP2(env->syntax,
                            ONIG_SYN_OP2_QMARK_BRACE_CALLOUT_CONTENTS)) {
            if (! PEND && PPEEK_IS('{')) {
              /* condition part is callouts of contents: (?(?{...})THEN|ELSE) */
              condition_is_checker = 0;
              PFETCH(c);
              r = parse_callout_of_contents(&condition, ')', &p, end, env);
              if (r != 0) return r;
              goto end_condition;
            }
          }
          goto any_condition;
        }
        else if (c == '*' &&
                 IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ASTERISK_CALLOUT_NAME)) {
          condition_is_checker = 0;
          r = parse_callout_of_name(&condition, ')', &p, end, env);
          if (r != 0) return r;
          goto end_condition;
        }
#endif
        else {
        any_condition:
          PUNFETCH;
          condition_is_checker = 0;
          r = fetch_token(tok, &p, end, env);
          if (r < 0) return r;
          r = parse_subexp(&condition, tok, term, &p, end, env);
          if (r < 0) {
            onig_node_free(condition);
            return r;
          }
        }

      end_condition:
        CHECK_NULL_RETURN_MEMERR(condition);

        if (PEND) {
        err_if_else:
          onig_node_free(condition);
          return ONIGERR_END_PATTERN_IN_GROUP;
        }

        if (PPEEK_IS(')')) { /* case: empty body: make backref checker */
          if (condition_is_checker == 0) {
            onig_node_free(condition);
            return ONIGERR_INVALID_IF_ELSE_SYNTAX;
          }
          PFETCH(c);
          *np = condition;
        }
        else { /* if-else */
          int then_is_empty;
          Node *Then, *Else;

          if (PPEEK_IS('|')) {
            PFETCH(c);
            Then = 0;
            then_is_empty = 1;
          }
          else
            then_is_empty = 0;

          r = fetch_token(tok, &p, end, env);
          if (r < 0) {
            onig_node_free(condition);
            return r;
          }
          r = parse_subexp(&target, tok, term, &p, end, env);
          if (r < 0) {
            onig_node_free(condition);
            onig_node_free(target);
            return r;
          }

          if (then_is_empty != 0) {
            Else = target;
          }
          else {
            if (NODE_TYPE(target) == NODE_ALT) {
              Then = NODE_CAR(target);
              if (NODE_CDR(NODE_CDR(target)) == NULL_NODE) {
                Else = NODE_CAR(NODE_CDR(target));
                cons_node_free_alone(NODE_CDR(target));
              }
              else {
                Else = NODE_CDR(target);
              }
              cons_node_free_alone(target);
            }
            else {
              Then = target;
              Else = 0;
            }
          }

          *np = node_new_enclosure_if_else(condition, Then, Else);
          if (IS_NULL(*np)) {
            onig_node_free(condition);
            onig_node_free(Then);
            onig_node_free(Else);
            return ONIGERR_MEMORY;
          }
        }
        goto end;
      }
      else {
        return ONIGERR_UNDEFINED_GROUP_OPTION;
      }
      break;

    case '@':
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ATMARK_CAPTURE_HISTORY)) {
        if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP)) {
          PFETCH(c);
          if (c == '<' || c == '\'') {
            list_capture = 1;
            goto named_group2; /* (?@<name>...) */
          }
          PUNFETCH;
        }

        *np = node_new_memory(0);
        CHECK_NULL_RETURN_MEMERR(*np);
        num = scan_env_add_mem_entry(env);
        if (num < 0) {
          return num;
        }
        else if (num >= (int )MEM_STATUS_BITS_NUM) {
          return ONIGERR_GROUP_NUMBER_OVER_FOR_CAPTURE_HISTORY;
        }
        ENCLOSURE_(*np)->m.regnum = num;
        MEM_STATUS_ON_SIMPLE(env->capture_history, num);
      }
      else {
        return ONIGERR_UNDEFINED_GROUP_OPTION;
      }
      break;

#ifdef USE_POSIXLINE_OPTION
    case 'p':
#endif
    case '-': case 'i': case 'm': case 's': case 'x':
    case 'W': case 'D': case 'S': case 'P':
      {
        int neg = 0;

        while (1) {
          switch (c) {
          case ':':
          case ')':
            break;

          case '-':  neg = 1; break;
          case 'x':  OPTION_NEGATE(option, ONIG_OPTION_EXTEND,     neg); break;
          case 'i':  OPTION_NEGATE(option, ONIG_OPTION_IGNORECASE, neg); break;
          case 's':
            if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_OPTION_PERL)) {
              OPTION_NEGATE(option, ONIG_OPTION_MULTILINE,  neg);
            }
            else
              return ONIGERR_UNDEFINED_GROUP_OPTION;
            break;

          case 'm':
            if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_OPTION_PERL)) {
              OPTION_NEGATE(option, ONIG_OPTION_SINGLELINE, (neg == 0 ? 1 : 0));
            }
            else if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_OPTION_RUBY)) {
              OPTION_NEGATE(option, ONIG_OPTION_MULTILINE,  neg);
            }
            else
              return ONIGERR_UNDEFINED_GROUP_OPTION;
            break;
#ifdef USE_POSIXLINE_OPTION
          case 'p':
            OPTION_NEGATE(option, ONIG_OPTION_MULTILINE|ONIG_OPTION_SINGLELINE, neg);
            break;
#endif
          case 'W': OPTION_NEGATE(option, ONIG_OPTION_WORD_IS_ASCII, neg); break;
          case 'D': OPTION_NEGATE(option, ONIG_OPTION_DIGIT_IS_ASCII, neg); break;
          case 'S': OPTION_NEGATE(option, ONIG_OPTION_SPACE_IS_ASCII, neg); break;
          case 'P': OPTION_NEGATE(option, ONIG_OPTION_POSIX_IS_ASCII, neg); break;

          default:
            return ONIGERR_UNDEFINED_GROUP_OPTION;
          }

          if (c == ')') {
            *np = node_new_option(option);
            CHECK_NULL_RETURN_MEMERR(*np);
            *src = p;
            return 2; /* option only */
          }
          else if (c == ':') {
            OnigOptionType prev = env->options;

            env->options = option;
            r = fetch_token(tok, &p, end, env);
            if (r < 0) return r;
            r = parse_subexp(&target, tok, term, &p, end, env);
            env->options = prev;
            if (r < 0) {
              onig_node_free(target);
              return r;
            }
            *np = node_new_option(option);
            CHECK_NULL_RETURN_MEMERR(*np);
            NODE_BODY(*np) = target;
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
#ifdef USE_CALLOUT
  else if (c == '*' &&
           IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ASTERISK_CALLOUT_NAME)) {
    PINC;
    r = parse_callout_of_name(np, ')', &p, end, env);
    if (r != 0) return r;

    goto end;
  }
#endif
  else {
    if (ONIG_IS_OPTION_ON(env->options, ONIG_OPTION_DONT_CAPTURE_GROUP))
      goto group;

    *np = node_new_memory(0);
    CHECK_NULL_RETURN_MEMERR(*np);
    num = scan_env_add_mem_entry(env);
    if (num < 0) return num;
    ENCLOSURE_(*np)->m.regnum = num;
  }

  CHECK_NULL_RETURN_MEMERR(*np);
  r = fetch_token(tok, &p, end, env);
  if (r < 0) return r;
  r = parse_subexp(&target, tok, term, &p, end, env);
  if (r < 0) {
    onig_node_free(target);
    return r;
  }

  NODE_BODY(*np) = target;

  if (NODE_TYPE(*np) == NODE_ENCLOSURE) {
    if (ENCLOSURE_(*np)->type == ENCLOSURE_MEMORY) {
      /* Don't move this to previous of parse_subexp() */
      r = scan_env_set_mem_node(env, ENCLOSURE_(*np)->m.regnum, *np);
      if (r != 0) return r;
    }
  }

 end:
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
  QuantNode* qn;

  qn = QUANT_(qnode);
  if (qn->lower == 1 && qn->upper == 1)
    return 1;

  switch (NODE_TYPE(target)) {
  case NODE_STRING:
    if (! group) {
      if (str_node_can_be_split(target, env->enc)) {
        Node* n = str_node_split_last_char(target, env->enc);
        if (IS_NOT_NULL(n)) {
          NODE_BODY(qnode) = n;
          return 2;
        }
      }
    }
    break;

  case NODE_QUANT:
    { /* check redundant double repeat. */
      /* verbose warn (?:.?)? etc... but not warn (.?)? etc... */
      QuantNode* qnt   = QUANT_(target);
      int nestq_num   = quantifier_type_num(qn);
      int targetq_num = quantifier_type_num(qnt);

#ifdef USE_WARNING_REDUNDANT_NESTED_REPEAT_OPERATOR
      if (targetq_num >= 0 && nestq_num >= 0 &&
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
      if (targetq_num >= 0 && nestq_num < 0) {
        if (targetq_num == 1 || targetq_num == 2) { /* * or + */
          /* (?:a*){n,m}, (?:a+){n,m} => (?:a*){n,n}, (?:a+){n,n} */
          if (! IS_REPEAT_INFINITE(qn->upper) && qn->upper > 1 && qn->greedy) {
            qn->upper = (qn->lower == 0 ? 1 : qn->lower);
          }
        }
      }
      else {
        NODE_BODY(qnode) = target;
        onig_reduce_nested_quantifier(qnode, target);
        goto q_exit;
      }
    }
    break;

  default:
    break;
  }

  NODE_BODY(qnode) = target;
 q_exit:
  return 0;
}


#ifndef CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS
static int
clear_not_flag_cclass(CClassNode* cc, OnigEncoding enc)
{
  BBuf *tbuf;
  int r;

  if (IS_NCCLASS_NOT(cc)) {
    bitset_invert(cc->bs);

    if (! ONIGENC_IS_SINGLEBYTE(enc)) {
      r = not_code_range_buf(enc, cc->mbuf, &tbuf);
      if (r != 0) return r;

      bbuf_free(cc->mbuf);
      cc->mbuf = tbuf;
    }

    NCCLASS_CLEAR_NOT(cc);
  }

  return 0;
}
#endif /* CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS */

typedef struct {
  ScanEnv*    env;
  CClassNode* cc;
  Node*       alt_root;
  Node**      ptail;
} IApplyCaseFoldArg;

static int
i_apply_case_fold(OnigCodePoint from, OnigCodePoint to[], int to_len, void* arg)
{
  IApplyCaseFoldArg* iarg;
  ScanEnv* env;
  CClassNode* cc;
  BitSetRef bs;

  iarg = (IApplyCaseFoldArg* )arg;
  env = iarg->env;
  cc  = iarg->cc;
  bs = cc->bs;

  if (to_len == 1) {
    int is_in = onig_is_code_in_cc(env->enc, from, cc);
#ifdef CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS
    if ((is_in != 0 && !IS_NCCLASS_NOT(cc)) ||
        (is_in == 0 &&  IS_NCCLASS_NOT(cc))) {
      if (ONIGENC_MBC_MINLEN(env->enc) > 1 || *to >= SINGLE_BYTE_SIZE) {
        add_code_range(&(cc->mbuf), env, *to, *to);
      }
      else {
        BITSET_SET_BIT(bs, *to);
      }
    }
#else
    if (is_in != 0) {
      if (ONIGENC_MBC_MINLEN(env->enc) > 1 || *to >= SINGLE_BYTE_SIZE) {
        if (IS_NCCLASS_NOT(cc)) clear_not_flag_cclass(cc, env->enc);
        add_code_range(&(cc->mbuf), env, *to, *to);
      }
      else {
        if (IS_NCCLASS_NOT(cc)) {
          BITSET_CLEAR_BIT(bs, *to);
        }
        else
          BITSET_SET_BIT(bs, *to);
      }
    }
#endif /* CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS */
  }
  else {
    int r, i, len;
    UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
    Node *snode = NULL_NODE;

    if (onig_is_code_in_cc(env->enc, from, cc)
#ifdef CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS
        && !IS_NCCLASS_NOT(cc)
#endif
        ) {
      for (i = 0; i < to_len; i++) {
        len = ONIGENC_CODE_TO_MBC(env->enc, to[i], buf);
        if (i == 0) {
          snode = onig_node_new_str(buf, buf + len);
          CHECK_NULL_RETURN_MEMERR(snode);

          /* char-class expanded multi-char only
             compare with string folded at match time. */
          NODE_STRING_SET_AMBIG(snode);
        }
        else {
          r = onig_node_str_cat(snode, buf, buf + len);
          if (r < 0) {
            onig_node_free(snode);
            return r;
          }
        }
      }

      *(iarg->ptail) = onig_node_new_alt(snode, NULL_NODE);
      CHECK_NULL_RETURN_MEMERR(*(iarg->ptail));
      iarg->ptail = &(NODE_CDR((*(iarg->ptail))));
    }
  }

  return 0;
}

static int
parse_exp(Node** np, OnigToken* tok, int term, UChar** src, UChar* end,
          ScanEnv* env)
{
  int r, len, group = 0;
  Node* qn;
  Node** targetp;

  *np = NULL;
  if (tok->type == (enum TokenSyms )term)
    goto end_of_token;

  switch (tok->type) {
  case TK_ALT:
  case TK_EOT:
  end_of_token:
  *np = node_new_empty();
  return tok->type;
  break;

  case TK_SUBEXP_OPEN:
    r = parse_enclosure(np, tok, TK_SUBEXP_CLOSE, src, end, env);
    if (r < 0) return r;
    if (r == 1) group = 1;
    else if (r == 2) { /* option only */
      Node* target;
      OnigOptionType prev = env->options;

      env->options = ENCLOSURE_(*np)->o.options;
      r = fetch_token(tok, src, end, env);
      if (r < 0) return r;
      r = parse_subexp(&target, tok, term, src, end, env);
      env->options = prev;
      if (r < 0) {
        onig_node_free(target);
        return r;
      }
      NODE_BODY(*np) = target;
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
      CHECK_NULL_RETURN_MEMERR(*np);

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
      *np = node_new_str_raw_char((UChar )tok->u.c);
      CHECK_NULL_RETURN_MEMERR(*np);
      len = 1;
      while (1) {
        if (len >= ONIGENC_MBC_MINLEN(env->enc)) {
          if (len == enclen(env->enc, STR_(*np)->s)) {/* should not enclen_end() */
            r = fetch_token(tok, src, end, env);
            NODE_STRING_CLEAR_RAW(*np);
            goto string_end;
          }
        }

        r = fetch_token(tok, src, end, env);
        if (r < 0) return r;
        if (r != TK_RAW_BYTE) {
          /* Don't use this, it is wrong for little endian encodings. */
#ifdef USE_PAD_TO_SHORT_BYTE_CHAR
          int rem;
          if (len < ONIGENC_MBC_MINLEN(env->enc)) {
            rem = ONIGENC_MBC_MINLEN(env->enc) - len;
            (void )node_str_head_pad(STR_(*np), rem, (UChar )0);
            if (len + rem == enclen(env->enc, STR_(*np)->s)) {
              NODE_STRING_CLEAR_RAW(*np);
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
      CHECK_NULL_RETURN_MEMERR(*np);
    }
    break;

  case TK_QUOTE_OPEN:
    {
      OnigCodePoint end_op[2];
      UChar *qstart, *qend, *nextp;

      end_op[0] = (OnigCodePoint )MC_ESC(env->syntax);
      end_op[1] = (OnigCodePoint )'E';
      qstart = *src;
      qend = find_str_position(end_op, 2, qstart, end, &nextp, env->enc);
      if (IS_NULL(qend)) {
        nextp = qend = end;
      }
      *np = node_new_str(qstart, qend);
      CHECK_NULL_RETURN_MEMERR(*np);
      *src = nextp;
    }
    break;

  case TK_CHAR_TYPE:
    {
      switch (tok->u.prop.ctype) {
      case ONIGENC_CTYPE_WORD:
        *np = node_new_ctype(tok->u.prop.ctype, tok->u.prop.not, env->options);
        CHECK_NULL_RETURN_MEMERR(*np);
        break;

      case ONIGENC_CTYPE_SPACE:
      case ONIGENC_CTYPE_DIGIT:
      case ONIGENC_CTYPE_XDIGIT:
        {
          CClassNode* cc;

          *np = node_new_cclass();
          CHECK_NULL_RETURN_MEMERR(*np);
          cc = CCLASS_(*np);
          add_ctype_to_cc(cc, tok->u.prop.ctype, 0, env);
          if (tok->u.prop.not != 0) NCCLASS_SET_NOT(cc);
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

      cc = CCLASS_(*np);
      if (IS_IGNORECASE(env->options)) {
        IApplyCaseFoldArg iarg;

        iarg.env      = env;
        iarg.cc       = cc;
        iarg.alt_root = NULL_NODE;
        iarg.ptail    = &(iarg.alt_root);

        r = ONIGENC_APPLY_ALL_CASE_FOLD(env->enc, env->case_fold_flag,
                                        i_apply_case_fold, &iarg);
        if (r != 0) {
          onig_node_free(iarg.alt_root);
          return r;
        }
        if (IS_NOT_NULL(iarg.alt_root)) {
          Node* work = onig_node_new_alt(*np, iarg.alt_root);
          if (IS_NULL(work)) {
            onig_node_free(iarg.alt_root);
            return ONIGERR_MEMORY;
          }
          *np = work;
        }
      }
    }
    break;

  case TK_ANYCHAR:
    *np = node_new_anychar();
    CHECK_NULL_RETURN_MEMERR(*np);
    break;

  case TK_ANYCHAR_ANYTIME:
    *np = node_new_anychar();
    CHECK_NULL_RETURN_MEMERR(*np);
    qn = node_new_quantifier(0, REPEAT_INFINITE, 0);
    CHECK_NULL_RETURN_MEMERR(qn);
    NODE_BODY(qn) = *np;
    *np = qn;
    break;

  case TK_BACKREF:
    len = tok->u.backref.num;
    *np = node_new_backref(len,
                  (len > 1 ? tok->u.backref.refs : &(tok->u.backref.ref1)),
                  tok->u.backref.by_name,
#ifdef USE_BACKREF_WITH_LEVEL
                           tok->u.backref.exist_level,
                           tok->u.backref.level,
#endif
                           env);
    CHECK_NULL_RETURN_MEMERR(*np);
    break;

#ifdef USE_CALL
  case TK_CALL:
    {
      int gnum = tok->u.call.gnum;

      *np = node_new_call(tok->u.call.name, tok->u.call.name_end,
                          gnum, tok->u.call.by_number);
      CHECK_NULL_RETURN_MEMERR(*np);
      env->num_call++;
      if (tok->u.call.by_number != 0 && gnum == 0) {
        env->has_call_zero = 1;
      }
    }
    break;
#endif

  case TK_ANCHOR:
    {
      int ascii_mode =
        IS_WORD_ASCII(env->options) && IS_WORD_ANCHOR_TYPE(tok->u.anchor) ? 1 : 0;
      *np = onig_node_new_anchor(tok->u.anchor, ascii_mode);
    }
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

  case TK_KEEP:
    r = node_new_keep(np, env);
    if (r < 0) return r;
    break;

  case TK_GENERAL_NEWLINE:
    r = node_new_general_newline(np, env);
    if (r < 0) return r;
    break;

  case TK_NO_NEWLINE:
    r = node_new_no_newline(np, env);
    if (r < 0) return r;
    break;

  case TK_TRUE_ANYCHAR:
    r = node_new_true_anychar(np, env);
    if (r < 0) return r;
    break;

  case TK_EXTENDED_GRAPHEME_CLUSTER:
    r = make_extended_grapheme_cluster(np, env);
    if (r < 0) return r;
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
      CHECK_NULL_RETURN_MEMERR(qn);
      QUANT_(qn)->greedy = tok->u.repeat.greedy;
      r = set_quantifier(qn, *targetp, group, env);
      if (r < 0) {
        onig_node_free(qn);
        return r;
      }

      if (tok->u.repeat.possessive != 0) {
        Node* en;
        en = node_new_enclosure(ENCLOSURE_STOP_BACKTRACK);
        if (IS_NULL(en)) {
          onig_node_free(qn);
          return ONIGERR_MEMORY;
        }
        NODE_BODY(en) = qn;
        qn = en;
      }

      if (r == 0) {
        *targetp = qn;
      }
      else if (r == 1) {
        onig_node_free(qn);
      }
      else if (r == 2) { /* split case: /abc+/ */
        Node *tmp;

        *targetp = node_new_list(*targetp, NULL);
        if (IS_NULL(*targetp)) {
          onig_node_free(qn);
          return ONIGERR_MEMORY;
        }
        tmp = NODE_CDR(*targetp) = node_new_list(qn, NULL);
        if (IS_NULL(tmp)) {
          onig_node_free(qn);
          return ONIGERR_MEMORY;
        }
        targetp = &(NODE_CAR(tmp));
      }
      goto re_entry;
    }
  }

  return r;
}

static int
parse_branch(Node** top, OnigToken* tok, int term, UChar** src, UChar* end,
             ScanEnv* env)
{
  int r;
  Node *node, **headp;

  *top = NULL;
  r = parse_exp(&node, tok, term, src, end, env);
  if (r < 0) {
    onig_node_free(node);
    return r;
  }

  if (r == TK_EOT || r == term || r == TK_ALT) {
    *top = node;
  }
  else {
    *top  = node_new_list(node, NULL);
    headp = &(NODE_CDR(*top));
    while (r != TK_EOT && r != term && r != TK_ALT) {
      r = parse_exp(&node, tok, term, src, end, env);
      if (r < 0) {
        onig_node_free(node);
        return r;
      }

      if (NODE_TYPE(node) == NODE_LIST) {
        *headp = node;
        while (IS_NOT_NULL(NODE_CDR(node))) node = NODE_CDR(node);
        headp = &(NODE_CDR(node));
      }
      else {
        *headp = node_new_list(node, NULL);
        headp = &(NODE_CDR(*headp));
      }
    }
  }

  return r;
}

/* term_tok: TK_EOT or TK_SUBEXP_CLOSE */
static int
parse_subexp(Node** top, OnigToken* tok, int term, UChar** src, UChar* end,
             ScanEnv* env)
{
  int r;
  Node *node, **headp;

  *top = NULL;
  env->parse_depth++;
  if (env->parse_depth > ParseDepthLimit)
    return ONIGERR_PARSE_DEPTH_LIMIT_OVER;
  r = parse_branch(&node, tok, term, src, end, env);
  if (r < 0) {
    onig_node_free(node);
    return r;
  }

  if (r == term) {
    *top = node;
  }
  else if (r == TK_ALT) {
    *top  = onig_node_new_alt(node, NULL);
    headp = &(NODE_CDR(*top));
    while (r == TK_ALT) {
      r = fetch_token(tok, src, end, env);
      if (r < 0) return r;
      r = parse_branch(&node, tok, term, src, end, env);
      if (r < 0) {
        onig_node_free(node);
        return r;
      }
      *headp = onig_node_new_alt(node, NULL);
      headp = &(NODE_CDR(*headp));
    }

    if (tok->type != (enum TokenSyms )term)
      goto err;
  }
  else {
    onig_node_free(node);
  err:
    if (term == TK_SUBEXP_CLOSE)
      return ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS;
    else
      return ONIGERR_PARSER_BUG;
  }

  env->parse_depth--;
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

#ifdef USE_CALL
static int
make_call_zero_body(Node* node, ScanEnv* env, Node** rnode)
{
  int r;

  Node* x = node_new_memory(0 /* 0: is not named */);
  CHECK_NULL_RETURN_MEMERR(x);

  NODE_BODY(x) = node;
  ENCLOSURE_(x)->m.regnum = 0;
  r = scan_env_set_mem_node(env, 0, x);
  if (r != 0) {
    onig_node_free(x);
    return r;
  }

  *rnode = x;
  return 0;
}
#endif

extern int
onig_parse_tree(Node** root, const UChar* pattern, const UChar* end,
                regex_t* reg, ScanEnv* env)
{
  int r;
  UChar* p;
#ifdef USE_CALLOUT
  RegexExt* ext;
#endif

  names_clear(reg);

  scan_env_clear(env);
  env->options        = reg->options;
  env->case_fold_flag = reg->case_fold_flag;
  env->enc            = reg->enc;
  env->syntax         = reg->syntax;
  env->pattern        = (UChar* )pattern;
  env->pattern_end    = (UChar* )end;
  env->reg            = reg;

  *root = NULL;

  if (! ONIGENC_IS_VALID_MBC_STRING(env->enc, pattern, end))
    return ONIGERR_INVALID_WIDE_CHAR_VALUE;

  p = (UChar* )pattern;
  r = parse_regexp(root, &p, (UChar* )end, env);

#ifdef USE_CALL
  if (r != 0) return r;

  if (env->has_call_zero != 0) {
    Node* zero_node;
    r = make_call_zero_body(*root, env, &zero_node);
    if (r != 0) return r;

    *root = zero_node;
  }
#endif

  reg->num_mem = env->num_mem;

#ifdef USE_CALLOUT
  ext = REG_EXTP(reg);
  if (IS_NOT_NULL(ext) && ext->callout_num > 0) {
    r = setup_ext_callout_list_values(reg);
  }
#endif

  return r;
}

extern void
onig_scan_env_set_error_string(ScanEnv* env, int ecode ARG_UNUSED,
                               UChar* arg, UChar* arg_end)
{
  env->error     = arg;
  env->error_end = arg_end;
}
