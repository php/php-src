/**********************************************************************

  regparse.c -  Oniguruma (regular expression library)

  Copyright (C) 2003  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#include "regparse.h"

#define WARN_BUFSIZE    256

#define SYN_POSIX_COMMON_OP \
 ( REG_SYN_OP_ANYCHAR | REG_SYN_OP_POSIX_BRACKET | REG_SYN_OP_BACK_REF | \
   REG_SYN_OP_CC | REG_SYN_OP_0INF | REG_SYN_OP_LINE_ANCHOR | \
   REG_SYN_OP_ESC_CONTROL_CHAR )

#define SYN_GNU_REGEX_OP \
  ( REG_SYN_OP_ANYCHAR | REG_SYN_OP_CC | \
    REG_SYN_OP_POSIX_BRACKET | REG_SYN_OP_BACK_REF | \
    REG_SYN_OP_INTERVAL | REG_SYN_OP_SUBEXP | REG_SYN_OP_ALT | \
    REG_SYN_OP_0INF | REG_SYN_OP_1INF | REG_SYN_OP_01 | \
    REG_SYN_OP_ESC_BUF_ANCHOR | REG_SYN_OP_ESC_WORD | \
    REG_SYN_OP_ESC_WORD_BOUND | REG_SYN_OP_ESC_WORD_BEGIN_END | \
    REG_SYN_OP_ESC_WHITE_SPACE | REG_SYN_OP_ESC_DIGIT | \
    REG_SYN_OP_LINE_ANCHOR )

#define SYN_GNU_REGEX_BV \
  ( REG_SYN_CONTEXT_INDEP_ANCHORS | REG_SYN_CONTEXT_INDEP_OPS | \
    REG_SYN_CONTEXT_INVALID_OPS | REG_SYN_ALLOW_INVALID_INTERVAL | \
    REG_SYN_ESCAPE_IN_CC | REG_SYN_ALLOW_RANGE_OP_IN_CC )

#ifdef USE_VARIABLE_SYNTAX
RegSyntaxType RegSyntaxPosixBasic = {
  ( SYN_POSIX_COMMON_OP | REG_SYN_OP_ESC_SUBEXP | REG_SYN_OP_ESC_INTERVAL )
  , 0
  , 0
  , ( REG_OPTION_SINGLELINE | REG_OPTION_MULTILINE )
};

RegSyntaxType RegSyntaxPosixExtended = {
  ( SYN_POSIX_COMMON_OP | REG_SYN_OP_SUBEXP | REG_SYN_OP_INTERVAL |
    REG_SYN_OP_1INF | REG_SYN_OP_01 | REG_SYN_OP_ALT )
  , 0
  , ( REG_SYN_CONTEXT_INDEP_ANCHORS | 
      REG_SYN_CONTEXT_INDEP_OPS | REG_SYN_CONTEXT_INVALID_OPS | 
      REG_SYN_ALLOW_UNMATCHED_CLOSE_SUBEXP | REG_SYN_ALLOW_RANGE_OP_IN_CC )
  , ( REG_OPTION_SINGLELINE | REG_OPTION_MULTILINE )
};

RegSyntaxType RegSyntaxEmacs = {
  ( REG_SYN_OP_ANYCHAR | REG_SYN_OP_CC | REG_SYN_OP_ESC_INTERVAL |
    REG_SYN_OP_ESC_SUBEXP | REG_SYN_OP_ESC_ALT |
    REG_SYN_OP_0INF | REG_SYN_OP_1INF | REG_SYN_OP_01 |
    REG_SYN_OP_BACK_REF | REG_SYN_OP_LINE_ANCHOR |
    REG_SYN_OP_ESC_GNU_BUF_ANCHOR | REG_SYN_OP_ESC_CONTROL_CHAR )
  , 0
  , REG_SYN_ALLOW_EMPTY_RANGE_IN_CC
  , REG_OPTION_NONE
};

RegSyntaxType RegSyntaxGrep = {
  ( REG_SYN_OP_ANYCHAR | REG_SYN_OP_CC | REG_SYN_OP_POSIX_BRACKET |
    REG_SYN_OP_INTERVAL | REG_SYN_OP_ESC_SUBEXP | REG_SYN_OP_ESC_ALT |
    REG_SYN_OP_0INF | REG_SYN_OP_ESC_1INF | REG_SYN_OP_ESC_01 |
    REG_SYN_OP_LINE_ANCHOR )
  , 0
  , ( REG_SYN_ALLOW_EMPTY_RANGE_IN_CC | REG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC )
  , REG_OPTION_NONE
};

RegSyntaxType RegSyntaxGnuRegex = {
  SYN_GNU_REGEX_OP
  , 0
  , SYN_GNU_REGEX_BV
  , REG_OPTION_NONE
};

RegSyntaxType RegSyntaxJava = {
  (( SYN_GNU_REGEX_OP | REG_SYN_OP_NON_GREEDY | REG_SYN_OP_SUBEXP_EFFECT |
     REG_SYN_OP_ESC_CONTROL_CHAR | REG_SYN_OP_ESC_C_CONTROL |
     REG_SYN_OP_QUOTE | REG_SYN_OP_ESC_OCTAL3 | REG_SYN_OP_ESC_X_HEX2 )
   & ~REG_SYN_OP_ESC_WORD_BEGIN_END )
  , ( REG_SYN_OP2_OPTION_PERL |
      REG_SYN_OP2_POSSESSIVE_REPEAT | REG_SYN_OP2_POSSESSIVE_INTERVAL |
      REG_SYN_OP2_CCLASS_SET | REG_SYN_OP2_ESC_V_VTAB |
      REG_SYN_OP2_ESC_U_HEX4 )
  , ( SYN_GNU_REGEX_BV | REG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND )
  , REG_OPTION_SINGLELINE
};

RegSyntaxType RegSyntaxPerl = {
  (( SYN_GNU_REGEX_OP | REG_SYN_OP_NON_GREEDY | REG_SYN_OP_SUBEXP_EFFECT |
     REG_SYN_OP_ESC_OCTAL3 | REG_SYN_OP_ESC_X_HEX2 |
     REG_SYN_OP_ESC_X_BRACE_HEX8 | REG_SYN_OP_ESC_CONTROL_CHAR |
     REG_SYN_OP_ESC_C_CONTROL | REG_SYN_OP_QUOTE )
   & ~REG_SYN_OP_ESC_WORD_BEGIN_END )
  , REG_SYN_OP2_OPTION_PERL
  , SYN_GNU_REGEX_BV
  , REG_OPTION_SINGLELINE
};
#endif /* USE_VARIABLE_SYNTAX */

RegSyntaxType RegSyntaxRuby = {
  (( SYN_GNU_REGEX_OP | REG_SYN_OP_NON_GREEDY | REG_SYN_OP_SUBEXP_EFFECT |
     REG_SYN_OP_ESC_OCTAL3 | REG_SYN_OP_ESC_X_HEX2 |
     REG_SYN_OP_ESC_X_BRACE_HEX8 | REG_SYN_OP_ESC_CONTROL_CHAR |
     REG_SYN_OP_ESC_C_CONTROL )
   & ~REG_SYN_OP_ESC_WORD_BEGIN_END )
  , ( REG_SYN_OP2_OPTION_RUBY |
      REG_SYN_OP2_NAMED_SUBEXP | REG_SYN_OP2_SUBEXP_CALL |
      REG_SYN_OP2_POSSESSIVE_REPEAT | REG_SYN_OP2_CCLASS_SET |
      REG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL |
      REG_SYN_OP2_ESC_M_BAR_META | REG_SYN_OP2_ESC_V_VTAB )
  , ( SYN_GNU_REGEX_BV | REG_SYN_WARN_FOR_CC_OP_NOT_ESCAPED |
      REG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND )
  , REG_OPTION_NONE
};

RegSyntaxType*  RegDefaultSyntax = REG_SYNTAX_RUBY;

#ifdef USE_VARIABLE_SYNTAX
extern int
regex_set_default_syntax(RegSyntaxType* syntax)
{
  if (IS_NULL(syntax))
    syntax = REG_SYNTAX_RUBY;

  RegDefaultSyntax = syntax;
  return 0;
}
#endif

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
  CHECK_NULL_RETURN_VAL(to, REGERR_MEMORY);
  r = BBUF_INIT(to, from->alloc);
  if (r != 0) return r;
  to->used = from->used;
  xmemcpy(to->p, from->p, from->used);
  return 0;
}

#define WC2MB_MAX_BUFLEN    7
#define ONOFF(v,f,negative)    (negative) ? ((v) &= ~(f)) : ((v) |= (f))

#define SET_ALL_MULTI_BYTE_RANGE(pbuf) \
  add_wc_range_to_buf(pbuf, (WCINT )0x80, ~((WCINT )0),\
                     (UChar )0x80, (UChar )0xff);

#define ADD_ALL_MULTI_BYTE_RANGE(code, mbuf) do {\
  if (! IS_SINGLEBYTE_CODE(code)) {\
    r = SET_ALL_MULTI_BYTE_RANGE(&(mbuf));\
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

static void
bitset_set_all(BitSetRef bs)
{
  int i;
  for (i = 0; i < BITSET_SIZE; i++) {
    bs[i] = ~((Bits )0);
  }
}

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

static int
k_strncmp(UChar* s1, UChar* s2, int n)
{
  int x;

  while (n-- > 0) {
    x = *s2++ - *s1++;
    if (x) return x;
  }
  return 0;
}

static void
k_strcpy(UChar* dest, UChar* src, UChar* end)
{
  int len = end - src;
  if (len > 0) {
    xmemcpy(dest, src, len);
    dest[len] = (UChar )0;
  }
}

extern UChar*
regex_strdup(UChar* s, UChar* end)
{
  int len = end - s;

  if (len > 0) {
    UChar* r = (UChar* )xmalloc(len + 1);
    CHECK_NULL_RETURN(r);
    xmemcpy(r, s, len);
    r[len] = (UChar )0;
    return r;
  }
  else return NULL;
}

/* scan pattern methods */
#define PEND_VALUE  -1

#define PFETCH(c)   do { (c) = *p++; } while (0)
#define PUNFETCH    p--
#define PINC        p++
#define PPEEK       (p < end ? *p : PEND_VALUE)
#define PEND        (p < end ?  0 : 1)

#ifdef REG_RUBY_M17N

static int
wc2mb_buf(WCINT wc, UChar **bufs, UChar **bufe, RegCharEncoding enc)
{
  int c, len;

  c = m17n_firstbyte(enc, wc);
  len = mblen(enc, c);
  if (len > (*bufe - *bufs)) {
    *bufs = xmalloc(len);
    CHECK_NULL_RETURN_VAL(*bufs, REGERR_MEMORY);    
  }
  m17n_mbcput(enc, wc, *bufs);
  *bufe = *bufs + len;
  return 0;
}

#else  /* REG_RUBY_M17N */

extern int
regex_wc2mb_first(RegCharEncoding code, WCINT wc)
{
  if (code == REGCODE_ASCII) {
    return (wc & 0xff);
  }
  else if (code == REGCODE_UTF8) {
    if ((wc & 0xffffff80) == 0)
      return wc;
    else {
      if ((wc & 0xfffff800) == 0)
	return ((wc>>6)& 0x1f) | 0xc0;
      else if ((wc & 0xffff0000) == 0)
	return ((wc>>12) & 0x0f) | 0xe0;
      else if ((wc & 0xffe00000) == 0)
	return ((wc>>18) & 0x07) | 0xf0;
      else if ((wc & 0xfc000000) == 0)
	return ((wc>>24) & 0x03) | 0xf8;
      else if ((wc & 0x80000000) == 0)
	return ((wc>>30) & 0x01) | 0xfc;
      else {
	return REGERR_TOO_BIG_WIDE_CHAR_VALUE;
      }
    }
  }
  else {
    int first;

    if ((wc & 0xff0000) != 0) {
      first = (wc >> 16) & 0xff;
      if (mblen(code, first) != 3)
	return REGERR_INVALID_WIDE_CHAR_VALUE;
    }
    else if ((wc & 0xff00) != 0) {
      first = (wc >> 8) & 0xff;
      if (mblen(code, first) != 2)
	return REGERR_INVALID_WIDE_CHAR_VALUE;
    }
    else {
      if (mblen(code, wc) != 1)
	return REGERR_INVALID_WIDE_CHAR_VALUE;
      return wc;
    }
    return first;
  }
}

static int
wc2mb(WCINT wc, UChar buf[], RegCharEncoding code)
{
#define UTF8_TRAILS(wc, shift)   ((((wc) >> (shift)) & 0x3f) | 0x80)
#define UTF8_TRAIL0(wc)          (((wc) & 0x3f) | 0x80)

  UChar *p = buf;

  if (code == REGCODE_UTF8) {
    if ((wc & 0xffffff80) == 0)
      *p++ = wc;
    else {
      if ((wc & 0xfffff800) == 0) {
	*p++ = ((wc>>6)& 0x1f) | 0xc0;
      }
      else if ((wc & 0xffff0000) == 0) {
	*p++ = ((wc>>12) & 0x0f) | 0xe0;
	*p++ = UTF8_TRAILS(wc, 6);
      }
      else if ((wc & 0xffe00000) == 0) {
	*p++ = ((wc>>18) & 0x07) | 0xf0;
	*p++ = UTF8_TRAILS(wc, 12);
	*p++ = UTF8_TRAILS(wc,  6);
      }
      else if ((wc & 0xfc000000) == 0) {
	*p++ = ((wc>>24) & 0x03) | 0xf8;
	*p++ = UTF8_TRAILS(wc, 18);
	*p++ = UTF8_TRAILS(wc, 12);
	*p++ = UTF8_TRAILS(wc,  6);
      }
      else if ((wc & 0x80000000) == 0) {
	*p++ = ((wc>>30) & 0x01) | 0xfc;
	*p++ = UTF8_TRAILS(wc, 24);
	*p++ = UTF8_TRAILS(wc, 18);
	*p++ = UTF8_TRAILS(wc, 12);
	*p++ = UTF8_TRAILS(wc,  6);
      }
      else {
	return REGERR_TOO_BIG_WIDE_CHAR_VALUE;
      }
      *p++ = UTF8_TRAIL0(wc);
    }
  }
  else {
    if ((wc & 0xff0000) != 0) *p++ = ((wc >> 16) & 0xff);
    if ((wc &   0xff00) != 0) *p++ = ((wc >>  8) & 0xff);
    *p++ = (wc & 0xff);

    if (mblen(code, buf[0]) != (p - buf))
      return REGERR_INVALID_WIDE_CHAR_VALUE;
  }

  return p - buf;
}

static int
wc2mb_buf(WCINT wc, UChar **bufs, UChar **bufe, RegCharEncoding code)
{
  int r;
  r = wc2mb(wc, *bufs, code);
  if (r < 0) return r;

  *bufe = (*bufs) + r;
  return 0;
}
#endif /* not REG_RUBY_M17N */

/* used as function pointer value */
static int
is_code_ascii(RegCharEncoding code, UChar c)
{
  return (c < 128 ? 1 : 0);
}

static int
is_code_graph(RegCharEncoding code, UChar c) { return IS_CODE_GRAPH(code, c); }
static int
is_code_print(RegCharEncoding code, UChar c) { return IS_CODE_PRINT(code, c); }
static int
is_code_alnum(RegCharEncoding code, UChar c) { return IS_CODE_ALNUM(code, c); }
static int
is_code_alpha(RegCharEncoding code, UChar c) { return IS_CODE_ALPHA(code, c); }
static int
is_code_lower(RegCharEncoding code, UChar c) { return IS_CODE_LOWER(code, c); }
static int
is_code_upper(RegCharEncoding code, UChar c) { return IS_CODE_UPPER(code, c); }
static int
is_code_cntrl(RegCharEncoding code, UChar c) { return IS_CODE_CNTRL(code, c); }
static int
is_code_punct(RegCharEncoding code, UChar c) { return IS_CODE_PUNCT(code, c); }
static int
is_code_space(RegCharEncoding code, UChar c) { return IS_CODE_SPACE(code, c); }
static int
is_code_blank(RegCharEncoding code, UChar c) { return IS_CODE_BLANK(code, c); }
static int
is_code_digit(RegCharEncoding code, UChar c) { return IS_CODE_DIGIT(code, c); }
static int
is_code_xdigit(RegCharEncoding code, UChar c) { return IS_CODE_XDIGIT(code, c); }

static UChar*
k_strcat_capa(UChar* dest, UChar* dest_end, UChar* src, UChar* src_end, int capa)
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
			UChar* src, UChar* src_end, int capa)
{
  UChar* r;

  r = (UChar* )xmalloc(capa + 1);
  CHECK_NULL_RETURN(r);
  k_strcpy(r, dest, dest_end);
  k_strcpy(r + (dest_end - dest), src, src_end);
  return r;
}

#ifdef USE_NAMED_SUBEXP

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

#include <st.h>

typedef st_table  NameTable;
typedef st_data_t HashDataType;   /* 1.6 st.h doesn't define st_data_t type */

#define NAMEBUF_SIZE    24
#define NAMEBUF_SIZE_1  25

#ifdef REG_DEBUG
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
regex_print_names(FILE* fp, regex_t* reg)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    fprintf(fp, "name table\n");
    st_foreach(t, i_print_name_entry, (HashDataType )fp);
    fputs("\n", fp);
  }
  return 0;
}
#endif

static int
i_free_name_entry(UChar* key, NameEntry* e, void* arg)
{
  xfree(e->name);  /* == key */
  if (IS_NOT_NULL(e->back_refs)) xfree(e->back_refs);
  return ST_DELETE;
}

static int
names_clear(regex_t* reg)
{
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    st_foreach(t, i_free_name_entry, 0);
  }
  return 0;
}

extern int
regex_names_free(regex_t* reg)
{
  int r;
  NameTable* t;

  r = names_clear(reg);
  if (r) return r;

  t = (NameTable* )reg->name_table;
  if (IS_NOT_NULL(t)) st_free_table(t);
  reg->name_table = (void* )NULL;
  return 0;
}

static NameEntry*
name_find(regex_t* reg, UChar* name, UChar* name_end)
{
  int len;
  UChar namebuf[NAMEBUF_SIZE_1];
  UChar *key;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  e = (NameEntry* )NULL;
  if (IS_NOT_NULL(t)) {
    if (*name_end == '\0') {
      key = name;
    }
    else {
      /* dirty, but st.c API claims NULL terminated key. */
      len = name_end - name;
      if (len <= NAMEBUF_SIZE) {
	xmemcpy(namebuf, name, len);
	namebuf[len] = '\0';
	key = namebuf;
      }
      else {
	key = regex_strdup(name, name_end);
	if (IS_NULL(key)) return (NameEntry* )NULL;
      }
    }

    st_lookup(t, (HashDataType )key, (HashDataType * )&e);
    if (key != name && key != namebuf) xfree(key);
  }
  return e;
}

typedef struct {
  int (*func)(UChar*,int,int*,void*);
  void* arg;
  int ret;
} INamesArg;

static int
i_names(UChar* key, NameEntry* e, INamesArg* arg)
{
  int r = (*(arg->func))(e->name, e->back_num,
	     (e->back_num > 1 ? e->back_refs : &(e->back_ref1)), arg->arg);
  if (r != 0) {
    arg->ret = r;
    return ST_STOP;
  }
  return ST_CONTINUE;
}

extern int
regex_foreach_name(regex_t* reg, int (*func)(UChar*,int,int*,void*), void* arg)
{
  INamesArg narg;
  NameTable* t = (NameTable* )reg->name_table;

  narg.ret = 0;
  if (IS_NOT_NULL(t)) {
    narg.func = func;
    narg.arg  = arg;
    st_foreach(t, i_names, (HashDataType )&narg);
  }
  return narg.ret;
}

#else  /* USE_ST_HASH_TABLE */

#define INIT_NAMES_ALLOC_NUM    8

typedef struct {
  NameEntry* e;
  int        num;
  int        alloc;
} NameTable;


#ifdef REG_DEBUG
extern int
regex_print_names(FILE* fp, regex_t* reg)
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
    t->num = 0;
  }
  return 0;
}

extern int
regex_names_free(regex_t* reg)
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
      if (len == e->name_len && k_strncmp(name, e->name, len) == 0)
	return e;
    }
  }
  return (NameEntry* )NULL;
}

extern int
regex_foreach_name(regex_t* reg, int (*func)(UChar*,int,int*,void*), void* arg)
{
  int i, r;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (IS_NOT_NULL(t)) {
    for (i = 0; i < t->num; i++) {
      e = &(t->e[i]);
      r = (*func)(e->name, e->back_num,
		  (e->back_num > 1 ? e->back_refs : &(e->back_ref1)), arg);
      if (r != 0) return r;
    }
  }
  return 0;
}

#endif /* else USE_ST_HASH_TABLE */

static int
name_add(regex_t* reg, UChar* name, UChar* name_end, int backref)
{
  int alloc;
  NameEntry* e;
  NameTable* t = (NameTable* )reg->name_table;

  if (name_end - name <= 0)
    return REGERR_INVALID_SUBEXP_NAME;

  e = name_find(reg, name, name_end);
  if (IS_NULL(e)) {
#ifdef USE_ST_HASH_TABLE
    if (IS_NULL(t)) {
      reg->name_table = t = st_init_strtable();
    }
    e = (NameEntry* )xmalloc(sizeof(NameEntry));
    CHECK_NULL_RETURN_VAL(e, REGERR_MEMORY);

    e->name     = regex_strdup(name, name_end);
    if (IS_NULL(e->name)) return REGERR_MEMORY;
    st_insert(t, (HashDataType )e->name, (HashDataType )e);

    e->name_len   = name_end - name;
    e->back_num   = 0;
    e->back_alloc = 0;
    e->back_refs  = (int* )NULL;

#else

    if (IS_NULL(t)) {
      alloc = INIT_NAMES_ALLOC_NUM;
      t = (NameTable* )xmalloc(sizeof(NameTable));
      CHECK_NULL_RETURN_VAL(t, REGERR_MEMORY);
      t->e     = NULL;
      t->alloc = 0;
      t->num   = 0;

      t->e = (NameEntry* )xmalloc(sizeof(NameEntry) * alloc);
      if (IS_NULL(t->e)) {
	xfree(t);
	return REGERR_MEMORY;
      }
      t->alloc = alloc;
      reg->name_table = t;
      goto clear;
    }
    else if (t->num == t->alloc) {
      int i;

      alloc = t->alloc * 2;
      t->e = (NameEntry* )xrealloc(t->e, sizeof(NameEntry) * alloc);
      CHECK_NULL_RETURN_VAL(t->e, REGERR_MEMORY);
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
    e->name = regex_strdup(name, name_end);
    e->name_len = name_end - name;
#endif
  }

  e->back_num++;
  if (e->back_num == 1) {
    e->back_ref1 = backref;
  }
  else if (e->back_num == 2) {
    alloc = INIT_NAME_BACKREFS_ALLOC_NUM;
    e->back_refs = (int* )xmalloc(sizeof(int) * alloc);
    CHECK_NULL_RETURN_VAL(e->back_refs, REGERR_MEMORY);
    e->back_alloc = alloc;
    e->back_refs[0] = e->back_ref1;
    e->back_refs[1] = backref;
  }
  else {
    if (e->back_num > e->back_alloc) {
      alloc = e->back_alloc * 2;
      e->back_refs = (int* )xrealloc(e->back_refs, sizeof(int) * alloc);
      CHECK_NULL_RETURN_VAL(e->back_refs, REGERR_MEMORY);
      e->back_alloc = alloc;
    }
    e->back_refs[e->back_num - 1] = backref;
  }

  return 0;
}

extern int
regex_name_to_group_numbers(regex_t* reg, UChar* name, UChar* name_end,
			    int** nums)
{
  NameEntry* e;

  e = name_find(reg, name, name_end);
  if (IS_NULL(e)) return REGERR_UNDEFINED_NAME_REFERENCE;

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

#else

extern int
regex_name_to_group_numbers(regex_t* reg, UChar* name, UChar* name_end,
			    int** nums)
{
  return REG_NO_SUPPORT_CONFIG;
}

extern int
regex_foreach_name(regex_t* reg, int (*func)(UChar*,int,int*,void*), void* arg)
{
  return REG_NO_SUPPORT_CONFIG;
}
#endif


#define INIT_SCANENV_MEMNODES_ALLOC_SIZE   16

static void
scan_env_clear(ScanEnv* env)
{
  int i;

  BIT_STATUS_CLEAR(env->backtrack_mem);
  BIT_STATUS_CLEAR(env->backrefed_mem);
  env->error             = (UChar* )NULL;
  env->error_end         = (UChar* )NULL;
  env->num_call          = 0;
  env->num_mem           = 0;
  env->mem_alloc         = 0;
  env->mem_nodes_dynamic = (Node** )NULL;

  for (i = 0; i < SCANENV_MEMNODES_SIZE; i++)
    env->mem_nodes_static[i] = NULL_NODE;
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
      CHECK_NULL_RETURN_VAL(p, REGERR_MEMORY);

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
    return REGERR_INVALID_BACKREF;
  return 0;
}


#ifdef USE_RECYCLE_NODE
typedef struct _FreeNode {
  struct _FreeNode* next;
} FreeNode;

static FreeNode* FreeNodeList = (FreeNode* )NULL;
#endif

extern void
regex_node_free(Node* node)
{
  if (IS_NULL(node)) return ;

  switch (NTYPE(node)) {
  case N_STRING:
    if (IS_NOT_NULL(NSTRING(node).s) && NSTRING(node).s != NSTRING(node).buf) {
      xfree(NSTRING(node).s);
    }
    break;

  case N_LIST:
  case N_ALT:
    regex_node_free(NCONS(node).left);
    regex_node_free(NCONS(node).right);
    break;

  case N_CCLASS:
    if (NCCLASS(node).mbuf)
      bbuf_free(NCCLASS(node).mbuf);
    break;

  case N_QUALIFIER:
    if (NQUALIFIER(node).target)
      regex_node_free(NQUALIFIER(node).target);
    break;

  case N_EFFECT:
    if (NEFFECT(node).target)
      regex_node_free(NEFFECT(node).target);
    break;

  case N_BACKREF:
    if (IS_NOT_NULL(NBACKREF(node).back_dynamic))
      xfree(NBACKREF(node).back_dynamic);
    break;

  case N_ANCHOR:
    if (NANCHOR(node).target)
      regex_node_free(NANCHOR(node).target);
    break;
  }

#ifdef USE_RECYCLE_NODE
  {
    FreeNode* n;

    n = (FreeNode* )node;
    n->next = FreeNodeList;
    FreeNodeList = n;
  }
#else
  xfree(node);
#endif
}

#ifdef USE_RECYCLE_NODE
extern int
regex_free_node_list()
{
  FreeNode* n;

  THREAD_ATOMIC_START;
  while (FreeNodeList) {
    n = FreeNodeList;
    FreeNodeList = FreeNodeList->next;
    xfree(n);
  }
  THREAD_ATOMIC_END;
  return 0;
}
#endif

static Node*
node_new()
{
  Node* node;

#ifdef USE_RECYCLE_NODE
  if (IS_NOT_NULL(FreeNodeList)) {
    node = (Node* )FreeNodeList;
    FreeNodeList = FreeNodeList->next;
    return node;
  }
#endif

  node = (Node* )xmalloc(sizeof(Node));
  return node;
}


static void
initialize_cclass(CClassNode* cc)
{
  BITSET_CLEAR(cc->bs);
  cc->not  = 0;
  cc->mbuf = NULL;
}

static Node*
node_new_cclass()
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_CCLASS;

  initialize_cclass(&(NCCLASS(node)));
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
node_new_anychar()
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
regex_node_new_anchor(int type)
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
node_new_backref(int back_num, int* backrefs, ScanEnv* env)
{
  int i;
  Node* node = node_new();

  CHECK_NULL_RETURN(node);
  node->type = N_BACKREF;
  NBACKREF(node).state    = 0;
  NBACKREF(node).back_num = back_num;
  NBACKREF(node).back_dynamic = (int* )NULL;

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
      regex_node_free(node);
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
node_new_qualifier(int lower, int upper, int by_number)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);
  node->type = N_QUALIFIER;
  NQUALIFIER(node).target = NULL;
  NQUALIFIER(node).lower  = lower;
  NQUALIFIER(node).upper  = upper;
  NQUALIFIER(node).greedy = 1;
  NQUALIFIER(node).by_number        = by_number;
  NQUALIFIER(node).target_may_empty = 0;
  NQUALIFIER(node).head_exact       = NULL_NODE;
  NQUALIFIER(node).next_head_exact  = NULL_NODE;
  NQUALIFIER(node).is_refered       = 0;
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
regex_node_new_effect(int type)
{
  return node_new_effect(type);
}

static Node*
node_new_option(RegOptionType option)
{
  Node* node = node_new_effect(EFFECT_OPTION);
  CHECK_NULL_RETURN(node);
  NEFFECT(node).option = option;
  return node;
}

extern int
regex_node_str_cat(Node* node, UChar* s, UChar* end)
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

	CHECK_NULL_RETURN_VAL(p, REGERR_MEMORY);
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
  return regex_node_str_cat(node, s, s + 1);
}

extern void
regex_node_conv_to_str_node(Node* node, int flag)
{
  node->type = N_STRING;

  NSTRING(node).flag = flag;
  NSTRING(node).capa = 0;
  NSTRING(node).s    = NSTRING(node).buf;
  NSTRING(node).end  = NSTRING(node).buf;
}

static Node*
node_new_str(UChar* s, UChar* end)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  node->type = N_STRING;
  NSTRING(node).capa = 0;
  NSTRING(node).flag = 0;
  NSTRING(node).s    = NSTRING(node).buf;
  NSTRING(node).end  = NSTRING(node).buf;
  if (regex_node_str_cat(node, s, end)) {
    regex_node_free(node);
    return NULL;
  }
  return node;
}

static Node*
node_new_str_raw(UChar* s, UChar* end)
{
  Node* node = node_new_str(s, end);
  NSTRING_SET_RAW(node);
  return node;
}

static Node*
node_new_empty()
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
node_new_str_raw_char(UChar c)
{
  UChar p[1];

  p[0] = c;
  return node_new_str_raw(p, p + 1);
}

static Node*
str_node_split_last_char(StrNode* sn, RegCharEncoding enc)
{
  UChar *p;
  Node* n = NULL_NODE;

  if (sn->end > sn->s) {
    p = regex_get_prev_char_head(enc, sn->s, sn->end);
    if (p && p > sn->s) { /* can be splitted. */
      n = node_new_str(p, sn->end);
      if ((sn->flag & NSTR_RAW) != 0)
	NSTRING_SET_RAW(n);
      sn->end = p;
    }
  }
  return n;
}

static int
str_node_can_be_split(StrNode* sn, RegCharEncoding enc)
{
  if (sn->end > sn->s) {
    return ((mblen(enc, *(sn->s)) < sn->end - sn->s)  ?  1 : 0);
  }
  return 0;
}

extern int
regex_scan_unsigned_number(UChar** src, UChar* end, RegCharEncoding enc)
{
  unsigned int num, val;
  int c;
  UChar* p = *src;

  num = 0;
  while (!PEND) {
    PFETCH(c);
    if (IS_CODE_DIGIT(enc, c)) {
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
				 RegCharEncoding enc)
{
  int c;
  unsigned int num, val;
  UChar* p = *src;

  num = 0;
  while (!PEND && maxlen-- != 0) {
    PFETCH(c);
    if (IS_CODE_XDIGIT(enc, c)) {
      val = (unsigned int )XDIGITVAL(c);
      if ((INT_MAX_LIMIT - val) / 16UL < num)
	return -1;  /* overflow */

      num = (num << 4) + XDIGITVAL(c);
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
			   RegCharEncoding enc)
{
  int c;
  unsigned int num, val;
  UChar* p = *src;

  num = 0;
  while (!PEND && maxlen-- != 0) {
    PFETCH(c);
    if (IS_CODE_ODIGIT(enc, c)) {
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


#define BBUF_WRITE_WCINT(bbuf,pos,wc) \
    BBUF_WRITE(bbuf, pos, &(wc), SIZE_WCINT)

/* data format:
   [multi-byte-head-BitSet][n][from-1][to-1][from-2][to-2] ... [from-n][to-n]
   (all data size is WCINT)
 */
static int
new_wc_range(BBuf** pbuf)
{
#define INIT_MULTI_BYTE_RANGE_SIZE  (SIZE_WCINT * 5)
  int r;
  WCINT n;
  BBuf* bbuf;

  bbuf = *pbuf = (BBuf* )xmalloc(sizeof(BBuf));
  CHECK_NULL_RETURN_VAL(*pbuf, REGERR_MEMORY);
  r = BBUF_INIT(*pbuf, SIZE_BITSET + INIT_MULTI_BYTE_RANGE_SIZE);
  if (r) return r;

  n = 0;
  BBUF_WRITE_WCINT(bbuf, SIZE_BITSET, n);
  BITSET_CLEAR((BitSetRef )bbuf->p);
  return 0;
}

static int
add_wc_range_to_buf(BBuf** pbuf, WCINT from, WCINT to, UChar cfrom, UChar cto)
{
  int r, inc_n, pos;
  int low, high, bound, x;
  WCINT n, *data;
  BBuf* bbuf;

  if (from > to) {
    n = from; from = to; to = n;
  }

  if (IS_NULL(*pbuf)) {
    r = new_wc_range(pbuf);
    if (r) return r;
    bbuf = *pbuf;
    n = 0;
  }
  else {
    bbuf = *pbuf;
    GET_WCINT(n, bbuf->p + SIZE_BITSET);
  }
  data = (WCINT* )(bbuf->p + SIZE_BITSET);
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
  if (n + inc_n > REG_MAX_MULTI_BYTE_RANGES_NUM)
    return REGERR_TOO_MANY_MULTI_BYTE_RANGES;

  if (inc_n != 1) {
    if (from > data[low*2])
      from = data[low*2];
    if (to < data[(high - 1)*2 + 1])
      to = data[(high - 1)*2 + 1];
  }

  if (inc_n != 0 && high < n) {
    int from_pos = SIZE_BITSET + SIZE_WCINT * (1 + high * 2);
    int to_pos   = SIZE_BITSET + SIZE_WCINT * (1 + (low + 1) * 2);
    int size = (n - high) * 2 * SIZE_WCINT;

    if (inc_n > 0) {
      BBUF_MOVE_RIGHT(bbuf, from_pos, to_pos, size);
    }
    else {
      BBUF_MOVE_LEFT_REDUCE(bbuf, from_pos, to_pos);
    }
  }

  pos = SIZE_BITSET + SIZE_WCINT * (1 + low * 2);
  BBUF_ENSURE_SIZE(bbuf, pos + SIZE_WCINT * 2);
  BBUF_WRITE_WCINT(bbuf, pos, from);
  BBUF_WRITE_WCINT(bbuf, pos + SIZE_WCINT, to);
  n += inc_n;
  BBUF_WRITE_WCINT(bbuf, SIZE_BITSET, n);

  if (inc_n > 0) {
    int i;
    UChar tmp;

    if (cfrom > cto) {
      tmp = cfrom; cfrom = cto; cto = tmp;
    }

    for (i = cfrom; i <= cto; i++) {
      BITSET_SET_BIT((BitSetRef)bbuf->p, i);
    }
  }
  return 0;
}

static int
add_wc_range(BBuf** pbuf, ScanEnv* env, WCINT from, WCINT to)
{
  int cfrom, cto;

  if (from > to) {
    if (IS_SYNTAX_BV(env->syntax, REG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
      return 0;
    else
      return REGERR_EMPTY_RANGE_IN_CHAR_CLASS;
  }

  cfrom = WC2MB_FIRST(env->enc, from);
  if (cfrom < 0) return cfrom;
  cto = WC2MB_FIRST(env->enc, to);
  if (cto < 0) return cto;
  return add_wc_range_to_buf(pbuf, from, to, (UChar )cfrom, (UChar )cto);
}

static int
not_wc_range_buf(BBuf* bbuf, BBuf** pbuf)
{
  int r, i, n;
  WCINT pre, from, to, *data;

  *pbuf = (BBuf* )NULL;
  if (IS_NULL(bbuf)) {
  set_all:
    return SET_ALL_MULTI_BYTE_RANGE(pbuf);
  }

  data = (WCINT* )(bbuf->p + SIZE_BITSET);
  GET_WCINT(n, data);
  data++;
  if (n <= 0) goto set_all;

  r = 0;
  pre = 0x80;
  for (i = 0; i < n; i++) {
    from = data[i*2];
    to   = data[i*2+1];
    if (pre <= from - 1) {
      r = add_wc_range_to_buf(pbuf, pre, from - 1, (UChar )0, (UChar )0);
      if (r != 0) return r;
    }
    if (to == ~((WCINT )0)) break;
    pre = to + 1;
  }
  if (to < ~((WCINT )0)) {
    r = add_wc_range_to_buf(pbuf, to + 1, ~((WCINT )0), (UChar )0, (UChar )0);
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
or_wc_range_buf(BBuf* bbuf1, int not1, BBuf* bbuf2, int not2, BBuf** pbuf)
{
  int i, r;
  WCINT n1, *data1;
  WCINT from, to;

  *pbuf = (BBuf* )NULL;
  if (IS_NULL(bbuf1) && IS_NULL(bbuf2)) {
    if (not1 != 0 || not2 != 0)
      return SET_ALL_MULTI_BYTE_RANGE(pbuf);
    return 0;
  }

  r = 0;
  if (IS_NULL(bbuf2))
    SWAP_BBUF_NOT(bbuf1, not1, bbuf2, not2);

  if (IS_NULL(bbuf1)) {
    if (not1 != 0) {
      return SET_ALL_MULTI_BYTE_RANGE(pbuf);
    }
    else {
      if (not2 == 0) {
	return bbuf_clone(pbuf, bbuf2);
      }
      else {
	return not_wc_range_buf(bbuf2, pbuf);
      }
    }
  }

  if (not1 != 0)
    SWAP_BBUF_NOT(bbuf1, not1, bbuf2, not2);

  data1 = (WCINT* )(bbuf1->p + SIZE_BITSET);
  GET_WCINT(n1, data1);
  data1++;

  if (not2 == 0 && not1 == 0) { /* 1 OR 2 */
    r = bbuf_clone(pbuf, bbuf2);
  }
  else if (not1 == 0) { /* 1 OR (not 2) */
    r = not_wc_range_buf(bbuf2, pbuf);
  }
  if (r != 0) return r;

  for (i = 0; i < n1; i++) {
    from = data1[i*2];
    to   = data1[i*2+1];
    r = add_wc_range_to_buf(pbuf, from, to, (UChar )0, (UChar )0);
    if (r != 0) return r;
  }
  return 0;
}

static int
and_wc_range1(BBuf** pbuf, WCINT from1, WCINT to1, WCINT* data, int n)
{
  int i, r;
  WCINT from2, to2;

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
	  r = add_wc_range_to_buf(pbuf, from1, from2-1, (UChar )0, (UChar )0);
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
    r = add_wc_range_to_buf(pbuf, from1, to1, (UChar )0, (UChar )0);
    if (r != 0) return r;
  }
  return 0;
}

static int
and_wc_range_buf(BBuf* bbuf1, int not1, BBuf* bbuf2, int not2, BBuf** pbuf)
{
  int i, j, r;
  WCINT n1, n2, *data1, *data2;
  WCINT from, to, from1, to1, from2, to2;

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

  data1 = (WCINT* )(bbuf1->p + SIZE_BITSET);
  data2 = (WCINT* )(bbuf2->p + SIZE_BITSET);
  GET_WCINT(n1, data1);
  GET_WCINT(n2, data2);
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
	r = add_wc_range_to_buf(pbuf, from, to, (UChar )0, (UChar )0);
	if (r != 0) return r;
      }
    }
  }
  else if (not1 == 0) { /* 1 AND (not 2) */
    for (i = 0; i < n1; i++) {
      from1 = data1[i*2];
      to1   = data1[i*2+1];
      r = and_wc_range1(pbuf, from1, to1, data2, n2);
      if (r != 0) return r;
    }
  }

  return 0;
}

static int
and_cclass(CClassNode* dest, CClassNode* cc)
{
  int r, not1, not2;
  BBuf *buf1, *buf2, *pbuf;
  BitSetRef bsr1, bsr2;
  BitSet bs1, bs2;

  not1 = dest->not;
  bsr1 = dest->bs;
  buf1 = dest->mbuf;
  not2 = cc->not;
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

  if (not1 != 0 && not2 != 0) {
    r = or_wc_range_buf(buf1, 0, buf2, 0, &pbuf);
  }
  else {
    r = and_wc_range_buf(buf1, not1, buf2, not2, &pbuf);
    if (r == 0 && not1 != 0) {
      BBuf *tbuf;
      r = not_wc_range_buf(pbuf, &tbuf);
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
  if (IS_NOT_NULL(pbuf)) {
    bitset_set_all((BitSetRef )pbuf->p); /* Sorry, but I'm tired. */
  }
  return r;
}

static int
or_cclass(CClassNode* dest, CClassNode* cc)
{
  int r, not1, not2;
  BBuf *buf1, *buf2, *pbuf;
  BitSetRef bsr1, bsr2;
  BitSet bs1, bs2;

  not1 = dest->not;
  bsr1 = dest->bs;
  buf1 = dest->mbuf;
  not2 = cc->not;
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

  if (not1 != 0 && not2 != 0) {
    r = and_wc_range_buf(buf1, 0, buf2, 0, &pbuf);
  }
  else {
    r = or_wc_range_buf(buf1, not1, buf2, not2, &pbuf);
    if (r == 0 && not1 != 0) {
      BBuf *tbuf;
      r = not_wc_range_buf(pbuf, &tbuf);
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
  if (IS_NOT_NULL(pbuf)) {
    bitset_set_all((BitSetRef )pbuf->p); /* Sorry, but I'm tired. */
  }
  return r;
}

static int
conv_backslash_value(int c, ScanEnv* env)
{
  if (IS_SYNTAX_OP(env->syntax, REG_SYN_OP_ESC_CONTROL_CHAR)) {
    switch (c) {
    case 'n':  return '\n';
    case 't':  return '\t';
    case 'r':  return '\r';
    case 'f':  return '\f';
    case 'a':  return '\007';
    case 'b':  return '\010';
    case 'e':  return '\033';
    case 'v':
      if (IS_SYNTAX_OP2(env->syntax, REG_SYN_OP2_ESC_V_VTAB))
	return '\v';
      break;

    default:
      break;
    }
  }
  return c;
}

static int
is_invalid_qualifier_target(Node* node)
{
  switch (NTYPE(node)) {
  case N_ANCHOR:
    return 1;
    break;

  case N_EFFECT:
    if (NEFFECT(node).type == EFFECT_OPTION)
      return is_invalid_qualifier_target(NEFFECT(node).target);
    break;

  case N_LIST: /* ex. (?:\G\A)* */
    do {
      if (! is_invalid_qualifier_target(NCONS(node).left)) return 0;
    } while (IS_NOT_NULL(node = NCONS(node).right));
    return 0;
    break;

  case N_ALT:  /* ex. (?:abc|\A)* */
    do {
      if (is_invalid_qualifier_target(NCONS(node).left)) return 1;
    } while (IS_NOT_NULL(node = NCONS(node).right));
    break;

  default:
    break;
  }
  return 0;
}

/* ?:0, *:1, +:2, ??:3, *?:4, +?:5 */
static int
popular_qualifier_num(QualifierNode* qf)
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

static void
reduce_nested_qualifier(Node* pnode, Node* cnode)
{
#define NQ_ASIS    0   /* as is     */
#define NQ_DEL     1   /* delete parent */
#define NQ_A       2   /* to '*'    */
#define NQ_AQ      3   /* to '*?'   */
#define NQ_QQ      4   /* to '??'   */
#define NQ_P_QQ    5   /* to '+)??' */
#define NQ_PQ_Q    6   /* to '+?)?' */

  static char reduces[][6] = {
    {NQ_DEL,  NQ_A,    NQ_A,   NQ_QQ,   NQ_AQ,   NQ_ASIS}, /* '?'  */
    {NQ_DEL,  NQ_DEL,  NQ_DEL, NQ_P_QQ, NQ_P_QQ, NQ_DEL},  /* '*'  */
    {NQ_A,    NQ_A,    NQ_DEL, NQ_ASIS, NQ_P_QQ, NQ_DEL},  /* '+'  */
    {NQ_DEL,  NQ_AQ,   NQ_AQ,  NQ_DEL,  NQ_AQ,   NQ_AQ},   /* '??' */
    {NQ_DEL,  NQ_DEL,  NQ_DEL, NQ_DEL,  NQ_DEL,  NQ_DEL},  /* '*?' */
    {NQ_ASIS, NQ_PQ_Q, NQ_DEL, NQ_AQ,   NQ_AQ,   NQ_DEL}   /* '+?' */
  };

  int pnum, cnum;
  QualifierNode *p, *c;

  p = &(NQUALIFIER(pnode));
  c = &(NQUALIFIER(cnode));
  pnum = popular_qualifier_num(p);
  cnum = popular_qualifier_num(c);

  switch(reduces[cnum][pnum]) {
  case NQ_DEL:
    *p = *c;
    break;
  case NQ_A:
    p->target = c->target;
    p->lower  = 0;  p->upper = REPEAT_INFINITE;  p->greedy = 1;
    break;
  case NQ_AQ:
    p->target = c->target;
    p->lower  = 0;  p->upper = REPEAT_INFINITE;  p->greedy = 0;
    break;
  case NQ_QQ:
    p->target = c->target;
    p->lower  = 0;  p->upper = 1;  p->greedy = 0;
    break;
  case NQ_P_QQ:
    p->target = cnode;
    p->lower  = 0;  p->upper = 1;  p->greedy = 0;
    c->lower  = 1;  c->upper = REPEAT_INFINITE;  c->greedy = 1;
    return ;
    break;
  case NQ_PQ_Q:
    p->target = cnode;
    p->lower  = 0;  p->upper = 1;  p->greedy = 1;
    c->lower  = 1;  c->upper = REPEAT_INFINITE;  c->greedy = 0;
    return ;
    break;
  case NQ_ASIS:
    p->target = cnode;
    return ;
    break;
  }

  c->target = NULL_NODE;
  regex_node_free(cnode);
}


enum TokenSyms {
  TK_EOT      = 0,   /* end of token */
  TK_BYTE     = 1,
  TK_RAW_BYTE = 2,
  TK_WC,
  TK_ANYCHAR,
  TK_CHAR_TYPE,
  TK_BACKREF,
  TK_CALL,
  TK_ANCHOR,
  TK_OP_REPEAT,
  TK_INTERVAL,
  TK_ALT,
  TK_SUBEXP_OPEN,
  TK_SUBEXP_CLOSE,
  TK_CC_OPEN,
  TK_QUOTE_OPEN,
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
    int   c;
    WCINT wc;
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
    } backref;
    struct {
      UChar* name;
      UChar* name_end;
    } call;
  } u;
} RegToken;


static int
fetch_range_qualifier(UChar** src, UChar* end, RegToken* tok, ScanEnv* env)
{
  int low, up, syn_allow;
  int c;
  UChar* p = *src;

  syn_allow = IS_SYNTAX_BV(env->syntax, REG_SYN_ALLOW_INVALID_INTERVAL);

  if (PEND) {
    if (syn_allow)
      return 1;  /* "....{" : OK! */
    else
      return REGERR_END_PATTERN_AT_LEFT_BRACE;  /* "....{" syntax error */
  }

  if (! syn_allow) {
    c = PPEEK;
    if (c == ')' || c == '(' || c == '|') {
      return REGERR_END_PATTERN_AT_LEFT_BRACE;
    }
  }

  low = regex_scan_unsigned_number(&p, end, env->enc);
  if (low < 0) return REGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;
  if (low > REG_MAX_REPEAT_NUM)
    return REGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;

  if (p == *src) goto invalid;  /* can't read low */

  if (PEND) goto invalid;
  PFETCH(c);
  if (c == ',') {
    UChar* prev = p;
    up = regex_scan_unsigned_number(&p, end, env->enc);
    if (up < 0) return REGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;
    if (up > REG_MAX_REPEAT_NUM)
      return REGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;

    if (p == prev) up = REPEAT_INFINITE;  /* {n,} : {n,infinite} */
  }
  else {
    PUNFETCH;
    up = low;  /* {n} : exact n times */
  }

  if (PEND) goto invalid;
  PFETCH(c);
  if (IS_SYNTAX_OP(env->syntax, REG_SYN_OP_ESC_INTERVAL)) {
    if (c != '\\') goto invalid;
    PFETCH(c);
  }
  if (c != '}') goto invalid;

  if (!IS_REPEAT_INFINITE(up) && low > up) {
    return REGERR_UPPER_SMALLER_THAN_LOWER_IN_REPEAT_RANGE;
  }

  tok->type = TK_INTERVAL;
  tok->u.repeat.lower = low;
  tok->u.repeat.upper = up;
  *src = p;
  return 0;

 invalid:
  if (syn_allow)
    return 1;  /* OK */
  else
    return REGERR_INVALID_REPEAT_RANGE_PATTERN;
}

/* \M-, \C-, \c, or \... */
static int
fetch_escaped_value(UChar** src, UChar* end, ScanEnv* env)
{
  int c;
  UChar* p = *src;

  if (PEND) return REGERR_END_PATTERN_AT_BACKSLASH;

  PFETCH(c);
  switch (c) {
  case 'M':
    if (IS_SYNTAX_OP2(env->syntax, REG_SYN_OP2_ESC_M_BAR_META)) {
      if (PEND) return REGERR_END_PATTERN_AT_META;
      PFETCH(c);
      if (c != '-') return REGERR_META_CODE_SYNTAX;
      if (PEND) return REGERR_END_PATTERN_AT_META;
      PFETCH(c);
      if (c == '\\') {
	c = fetch_escaped_value(&p, end, env);
	if (c < 0) return c;
      }
      c = ((c & 0xff) | 0x80);
    }
    else
      goto backslash;
    break;

  case 'C':
    if (IS_SYNTAX_OP2(env->syntax, REG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL)) {
      if (PEND) return REGERR_END_PATTERN_AT_CONTROL;
      PFETCH(c);
      if (c != '-') return REGERR_CONTROL_CODE_SYNTAX;
      goto control;
    }
    else
      goto backslash;

  case 'c':
    if (IS_SYNTAX_OP(env->syntax, REG_SYN_OP_ESC_C_CONTROL)) {
    control:
      if (PEND) return REGERR_END_PATTERN_AT_CONTROL;
      PFETCH(c);
      if (c == '\\') {
	c = fetch_escaped_value(&p, end, env);
	if (c < 0) return c;
      }
      else if (c == '?')
	c = 0177;
      else
	c &= 0x9f;
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

static int fetch_token(RegToken* tok, UChar** src, UChar* end, ScanEnv* env);

#ifdef USE_NAMED_SUBEXP
static int
fetch_name(UChar** src, UChar* end, UChar** name_end, ScanEnv* env)
{
  int len;
  int c = 0;
  UChar *p = *src;

  while (!PEND) {
    *name_end = p;
    PFETCH(c);
    if (c == '>') break;
    else if (c == ')' || c == '\\' || c == '\0')
      return REGERR_INVALID_SUBEXP_NAME;

    len = mblen(env->enc, c);
    while (!PEND && len-- > 1) {
      PFETCH(c);
    }
  }
  if (c != '>') return REGERR_INVALID_SUBEXP_NAME;
  *src = p;
  return 0;
}
#endif

static void
CC_ESC_WARN(ScanEnv* env, UChar *c)
{
#ifdef WARNING
  if (IS_SYNTAX_BV(env->syntax, REG_SYN_WARN_FOR_CC_OP_NOT_ESCAPED) &&
      IS_SYNTAX_BV(env->syntax, REG_SYN_ESCAPE_IN_CC)) {
    char buf[WARN_BUFSIZE];
    regex_snprintf_with_pattern(buf, WARN_BUFSIZE, env->enc,
		env->pattern, env->pattern_end,
		"character class has '%s' without escape", c);
    WARNING(buf);
  }
#endif
}

static void
CCEND_ESC_WARN(ScanEnv* env, UChar* c)
{
#ifdef WARNING
  if (IS_SYNTAX_BV((env)->syntax, REG_SYN_WARN_FOR_CC_OP_NOT_ESCAPED)) {
    char buf[WARN_BUFSIZE];
    regex_snprintf_with_pattern(buf, WARN_BUFSIZE, (env)->enc,
		(env)->pattern, (env)->pattern_end,
		"regular expression has '%s' without escape", c);
    WARNING(buf);
  }
#endif
}

static UChar*
find_str_position(WCINT s[], int n, UChar* from, UChar* to, UChar **next,
		  RegCharEncoding enc)
{
  int i;
  WCINT x;
  UChar *q;
  UChar *p = from;
  
  while (p < to) {
    x = mb2wc(p, to, enc);
    q = p + mblen(enc, *p);
    if (x == s[0]) {
      for (i = 1; i < n && q < to; i++) {
	x = mb2wc(q, to, enc);
	if (x != s[i]) break;
	q += mblen(enc, *q);
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
str_exist_check_with_esc(WCINT s[], int n, UChar* from, UChar* to,
			 WCINT bad, RegCharEncoding enc)
{
  int i, in_esc;
  WCINT x;
  UChar *q;
  UChar *p = from;

  in_esc = 0;
  while (p < to) {
    if (in_esc) {
      in_esc = 0;
      p += mblen(enc, *p);
    }
    else {
      x = mb2wc(p, to, enc);
      q = p + mblen(enc, *p);
      if (x == s[0]) {
	for (i = 1; i < n && q < to; i++) {
	  x = mb2wc(q, to, enc);
	  if (x != s[i]) break;
	  q += mblen(enc, *q);
	}
	if (i >= n) return 1;
	p += mblen(enc, *p);
      }
      else {
	x = mb2wc(p, to, enc);
	if (x == bad) return 0;
	else if (x == '\\') in_esc = 1;
	p = q;
      }
    }
  }
  return 0;
}

static int
fetch_token_in_cc(RegToken* tok, UChar** src, UChar* end, ScanEnv* env)
{
  int c, num;
  RegSyntaxType* syn = env->syntax;
  UChar* prev;
  UChar* p = *src;

  if (PEND) {
    tok->type = TK_EOT;
    return tok->type;
  }

  PFETCH(c);
  tok->type = TK_BYTE;
  tok->base = 0;
  tok->u.c  = c;
  if (c == ']') {
    tok->type = TK_CC_CLOSE;
  }
  else if (c == '-') {
    tok->type = TK_CC_RANGE;
  }
  else if (c == '\\') {
    if (! IS_SYNTAX_BV(syn, REG_SYN_ESCAPE_IN_CC))
      goto end;

    if (PEND) return REGERR_END_PATTERN_AT_BACKSLASH;

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

    case 'x':
      if (PEND) break;

      prev = p;
      if (PPEEK == '{' && IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_X_BRACE_HEX8)) {
	PINC;
	num = scan_unsigned_hexadecimal_number(&p, end, 8, env->enc);
	if (num < 0) return REGERR_TOO_BIG_WIDE_CHAR_VALUE;
	if (!PEND && IS_XDIGIT(*p) && p - prev >= 9)
	  return REGERR_TOO_LONG_WIDE_CHAR_VALUE;

	if (p > prev + 1 && !PEND && PPEEK == '}') {
	  PINC;
	  tok->type = TK_WC;
	  tok->base = 16;
	  tok->u.wc = (WCINT )num;
	}
	else {
	  /* can't read nothing or invalid format */
	  p = prev;
	}
      }
      else if (IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_X_HEX2)) {
	num = scan_unsigned_hexadecimal_number(&p, end, 2, env->enc);
	if (num < 0) return REGERR_TOO_BIG_NUMBER;
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
      if (IS_SYNTAX_OP2(syn, REG_SYN_OP2_ESC_U_HEX4)) {
	num = scan_unsigned_hexadecimal_number(&p, end, 4, env->enc);
	if (num < 0) return REGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type = TK_RAW_BYTE;
	tok->base = 16;
	tok->u.c  = num;
      }
      break;

    case '0':
    case '1': case '2': case '3': case '4': case '5': case '6': case '7':
      if (IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_OCTAL3)) {
	PUNFETCH;
	prev = p;
	num = scan_unsigned_octal_number(&p, end, 3, env->enc);
	if (num < 0) return REGERR_TOO_BIG_NUMBER;
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
	tok->u.c = num;
	tok->type = TK_RAW_BYTE;
      }
      break;
    }
  }
  else if (c == '[') {
    if (IS_SYNTAX_OP(syn, REG_SYN_OP_POSIX_BRACKET) && PPEEK == ':') {
      WCINT send[] = { (WCINT )':', (WCINT )']' };
      tok->backp = p; /* point at '[' is readed */
      PINC;
      if (str_exist_check_with_esc(send, 2, p, end, (WCINT )']', env->enc)) {
	tok->type = TK_POSIX_BRACKET_OPEN;
      }
      else {
	PUNFETCH;
	goto cc_in_cc;
      }
    }
    else {
    cc_in_cc:
      if (IS_SYNTAX_OP2(syn, REG_SYN_OP2_CCLASS_SET)) {
	tok->type = TK_CC_CC_OPEN;
      }
      else {
	CC_ESC_WARN(env, "[");
      }
    }
  }
  else if (c == '&') {
    if (IS_SYNTAX_OP2(syn, REG_SYN_OP2_CCLASS_SET) && !PEND && PPEEK == '&') {
      PINC;
      tok->type = TK_CC_AND;
    }
  }

 end:
  *src = p;
  return tok->type;
}

static int
fetch_token(RegToken* tok, UChar** src, UChar* end, ScanEnv* env)
{
  int r, c, num;
  RegSyntaxType* syn = env->syntax;
  UChar* prev;
  UChar* p = *src;

 start:
  if (PEND) {
    tok->type = TK_EOT;
    return tok->type;
  }

  tok->type = TK_BYTE;
  tok->base = 0;
  PFETCH(c);
  if (c == '\\') {
    if (PEND) return REGERR_END_PATTERN_AT_BACKSLASH;

    PFETCH(c);
    tok->u.c = c;
    tok->escaped = 1;
    switch (c) {
    case '*':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_0INF)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '+':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_1INF)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 1;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '?':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_01)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = 1;
    greedy_check:
      if (!PEND && PPEEK == '?' && IS_SYNTAX_OP(syn, REG_SYN_OP_NON_GREEDY)) {
	PFETCH(c);
	tok->u.repeat.greedy     = 0;
	tok->u.repeat.possessive = 0;
      }
      else if (!PEND && PPEEK == '+' &&
	       ((IS_SYNTAX_OP2(syn, REG_SYN_OP2_POSSESSIVE_REPEAT) &&
		 tok->type != TK_INTERVAL)  ||
		(IS_SYNTAX_OP2(syn, REG_SYN_OP2_POSSESSIVE_INTERVAL) &&
		 tok->type == TK_INTERVAL))) {
	PFETCH(c);
	tok->u.repeat.greedy     = 1;
	tok->u.repeat.possessive = 1;
      }
      else {
	tok->u.repeat.greedy     = 1;
	tok->u.repeat.possessive = 0;
      }
      break;

    case '{':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_INTERVAL)) break;
      tok->backp = p;
      r = fetch_range_qualifier(&p, end, tok, env);
      if (r < 0) return r;  /* error */
      if (r > 0) {
	/* normal char */
      }
      else
	goto greedy_check;
      break;

    case '|':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_ALT)) break;
      tok->type = TK_ALT;
      break;

    case '(':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_SUBEXP)) break;
      tok->type = TK_SUBEXP_OPEN;
      break;

    case ')':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_SUBEXP)) break;
      tok->type = TK_SUBEXP_CLOSE;
      break;

    case 'w':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_WORD)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_WORD;
      break;

    case 'W':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_WORD)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_WORD;
      break;

    case 'b':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_WORD_BOUND)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_WORD_BOUND;
      break;

    case 'B':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_WORD_BOUND)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_NOT_WORD_BOUND;
      break;

#ifdef USE_WORD_BEGIN_END
    case '<':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_WORD_BEGIN_END)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_WORD_BEGIN;
      break;

    case '>':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_WORD_BEGIN_END)) break;
      tok->type = TK_ANCHOR;
      tok->u.anchor = ANCHOR_WORD_END;
      break;
#endif

    case 's':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_WHITE_SPACE)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_WHITE_SPACE;
      break;

    case 'S':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_WHITE_SPACE)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_WHITE_SPACE;
      break;

    case 'd':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_DIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_DIGIT;
      break;

    case 'D':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_DIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.subtype = CTYPE_NOT_DIGIT;
      break;

    case 'A':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_BUF_ANCHOR)) break;
    begin_buf:
      tok->type = TK_ANCHOR;
      tok->u.subtype = ANCHOR_BEGIN_BUF;
      break;

    case 'Z':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_BUF_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.subtype = ANCHOR_SEMI_END_BUF;
      break;

    case 'z':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_BUF_ANCHOR)) break;
    end_buf:
      tok->type = TK_ANCHOR;
      tok->u.subtype = ANCHOR_END_BUF;
      break;

    case 'G':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_BUF_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.subtype = ANCHOR_BEGIN_POSITION;
      break;

    case '`':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_GNU_BUF_ANCHOR)) break;
      goto begin_buf;
      break;

    case '\'':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_GNU_BUF_ANCHOR)) break;
      goto end_buf;
      break;

    case 'x':
      if (PEND) break;

      prev = p;
      if (PPEEK == '{' && IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_X_BRACE_HEX8)) {
	PINC;
	num = scan_unsigned_hexadecimal_number(&p, end, 8, env->enc);
	if (num < 0) return REGERR_TOO_BIG_WIDE_CHAR_VALUE;
	if (!PEND && IS_XDIGIT(*p) && p - prev >= 9)
	  return REGERR_TOO_LONG_WIDE_CHAR_VALUE;

	if (p > prev + 1 && !PEND && PPEEK == '}') {
	  PINC;
	  tok->type = TK_WC;
	  tok->u.wc = (WCINT )num;
	}
	else {
	  /* can't read nothing or invalid format */
	  p = prev;
	}
      }
      else if (IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_X_HEX2)) {
	num = scan_unsigned_hexadecimal_number(&p, end, 2, env->enc);
	if (num < 0) return REGERR_TOO_BIG_NUMBER;
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
      if (IS_SYNTAX_OP2(syn, REG_SYN_OP2_ESC_U_HEX4)) {
	num = scan_unsigned_hexadecimal_number(&p, end, 4, env->enc);
	if (num < 0) return REGERR_TOO_BIG_NUMBER;
	if (p == prev) {  /* can't read nothing. */
	  num = 0; /* but, it's not error */
	}
	tok->type = TK_RAW_BYTE;
	tok->base = 16;
	tok->u.c  = num;
      }
      break;

    case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      PUNFETCH;
      prev = p;
      num = regex_scan_unsigned_number(&p, end, env->enc);
      if (num < 0)  return REGERR_TOO_BIG_NUMBER;
      if (num > REG_MAX_BACKREF_NUM) return REGERR_TOO_BIG_BACKREF_NUMBER;

      if (IS_SYNTAX_OP(syn, REG_SYN_OP_BACK_REF) && 
	  (num <= env->num_mem || num <= 9)) { /* This spec. from GNU regex */
	if (IS_SYNTAX_BV(syn, REG_SYN_STRICT_CHECK_BACKREF)) {
	  if (num > env->num_mem || IS_NULL(SCANENV_MEM_NODES(env)[num]))
	    return REGERR_INVALID_BACKREF;
	}

	tok->type = TK_BACKREF;
	tok->u.backref.num  = 1;
	tok->u.backref.ref1 = num;
	break;
      }
      else if (c == '8' || c == '9') {
	/* normal char */
	p = prev; PINC;
	break;
      }

      p = prev;
      /* fall through */
    case '0':
      if (IS_SYNTAX_OP(syn, REG_SYN_OP_ESC_OCTAL3)) {
	prev = p;
	num = scan_unsigned_octal_number(&p, end, (c == '0' ? 2:3), env->enc);
	if (num < 0) return REGERR_TOO_BIG_NUMBER;
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

#ifdef USE_NAMED_SUBEXP
    case 'k':
      if (IS_SYNTAX_OP2(syn, REG_SYN_OP2_NAMED_SUBEXP)) {
	PFETCH(c);
	if (c == '<') {
	  UChar* name_end;
	  int* backs;

	  prev = p;
	  r = fetch_name(&p, end, &name_end, env);
	  if (r < 0) return r;
	  num = regex_name_to_group_numbers(env->reg, prev, name_end, &backs);
	  if (num <= 0) {
	    regex_scan_env_set_error_string(env,
			    REGERR_UNDEFINED_NAME_REFERENCE, prev, name_end);
	    return REGERR_UNDEFINED_NAME_REFERENCE;
	  }
	  if (IS_SYNTAX_BV(syn, REG_SYN_STRICT_CHECK_BACKREF)) {
	    int i;
	    for (i = 0; i < num; i++) {
	      if (backs[i] > env->num_mem ||
		  IS_NULL(SCANENV_MEM_NODES(env)[backs[i]]))
		return REGERR_INVALID_BACKREF;
	    }
	  }

	  tok->type = TK_BACKREF;
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
      if (IS_SYNTAX_OP2(syn, REG_SYN_OP2_SUBEXP_CALL)) {
	PFETCH(c);
	if (c == '<') {
	  UChar* name_end;

	  prev = p;
	  r = fetch_name(&p, end, &name_end, env);
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
      if (IS_SYNTAX_OP(syn, REG_SYN_OP_QUOTE)) {
	tok->type = TK_QUOTE_OPEN;
      }
      break;

    default:
      PUNFETCH;
      num = fetch_escaped_value(&p, end, env);
      if (num < 0) return num;
      /* set_raw: */
      if (tok->u.c != num) {
	tok->type = TK_RAW_BYTE;
	tok->u.c = num;
      }
      break;
    }
  }
  else {
    tok->u.c = c;
    tok->escaped = 0;

    switch (c) {
    case '.':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ANYCHAR)) break;
      tok->type = TK_ANYCHAR;
      break;

    case '*':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_0INF)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '+':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_1INF)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 1;
      tok->u.repeat.upper = REPEAT_INFINITE;
      goto greedy_check;
      break;

    case '?':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_01)) break;
      tok->type = TK_OP_REPEAT;
      tok->u.repeat.lower = 0;
      tok->u.repeat.upper = 1;
      goto greedy_check;
      break;

    case '{':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_INTERVAL)) break;
      tok->backp = p;
      r = fetch_range_qualifier(&p, end, tok, env);
      if (r < 0) return r;  /* error */
      if (r > 0) {
	/* normal char */
      }
      else
	goto greedy_check;
      break;

    case '|':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_ALT)) break;
      tok->type = TK_ALT;
      break;

    case '(':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_SUBEXP)) break;
      tok->type = TK_SUBEXP_OPEN;
      break;

    case ')':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_SUBEXP)) break;
      tok->type = TK_SUBEXP_CLOSE;
      break;

    case '^':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_LINE_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.subtype = (IS_SINGLELINE(env->option)
			? ANCHOR_BEGIN_BUF : ANCHOR_BEGIN_LINE);
      break;

    case '$':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_LINE_ANCHOR)) break;
      tok->type = TK_ANCHOR;
      tok->u.subtype = (IS_SINGLELINE(env->option)
			? ANCHOR_END_BUF : ANCHOR_END_LINE);
      break;

    case '[':
      if (! IS_SYNTAX_OP(syn, REG_SYN_OP_CC)) break;
      tok->type = TK_CC_OPEN;
      break;

    case ']':
      if (*src > env->pattern)   /* /].../ is allowed. */
	CCEND_ESC_WARN(env, "]");
      break;

    case '#':
      if (IS_EXTEND(env->option)) {
	while (!PEND) {
	  PFETCH(c);
	  if (IS_NEWLINE(c))
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
      break;
    }
  }

  *src = p;
  return tok->type;
}

static void
bitset_by_pred_func(BitSetRef bs, int (*pf)(RegCharEncoding, UChar),
		    RegCharEncoding code, int not)
{
  int c;

  if (not) {
    for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
      if (! pf(code, (UChar )c)) BITSET_SET_BIT(bs, c);
    }
  }
  else {
    for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
      if (pf(code, (UChar )c)) BITSET_SET_BIT(bs, c);
    }
  }
}

typedef struct {
  UChar *name;
  int (*pf)(RegCharEncoding, UChar);
  short int len;
} PosixBracketEntryType;

static int
parse_posix_bracket(CClassNode* cc, UChar** src, UChar* end, ScanEnv* env)
{
#define POSIX_BRACKET_CHECK_LIMIT_LENGTH  20
#define POSIX_BRACKET_NAME_MAX_LEN         6

  static PosixBracketEntryType PBS[] = {
    { "alnum",  is_code_alnum,  5 },
    { "alpha",  is_code_alpha,  5 },
    { "blank",  is_code_blank,  5 },
    { "cntrl",  is_code_cntrl,  5 },
    { "digit",  is_code_digit,  5 },
    { "graph",  is_code_graph,  5 },
    { "lower",  is_code_lower,  5 },
    { "print",  is_code_print,  5 },
    { "punct",  is_code_punct,  5 },
    { "space",  is_code_space,  5 },
    { "upper",  is_code_upper,  5 },
    { "xdigit", is_code_xdigit, 6 },
    { "ascii",  is_code_ascii,  5 }, /* I don't know origin. Perl? */
    { (UChar* )NULL, is_code_alnum, 0 }
  };

  PosixBracketEntryType *pb;
  int not, i, c;
  UChar *p = *src;

  if (PPEEK == '^') {
    PINC;
    not = 1;
  }
  else
    not = 0;

  if (end - p < POSIX_BRACKET_NAME_MAX_LEN + 1)
    goto not_posix_bracket;

  for (pb = PBS; IS_NOT_NULL(pb->name); pb++) {
    if (k_strncmp(p, pb->name, pb->len) == 0) {
      p += pb->len;
      if (end - p < 2 || *p != ':' || *(p+1) != ']')
	return REGERR_INVALID_POSIX_BRACKET_TYPE;

      bitset_by_pred_func(cc->bs, pb->pf, env->enc, not);
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
  if (c == ':' && !PEND) {
    PINC;
    if (!PEND) {
      PFETCH(c);
      if (c == ']')
	return REGERR_INVALID_POSIX_BRACKET_TYPE;
    }
  }

  return 1;   /* 1: is not POSIX bracket, but no error. */
}


enum CCSTATE {
  CCS_VALUE,
  CCS_RANGE,
  CCS_COMPLETE,
  CCS_START
};

enum CCVALTYPE {
  CCV_SB,
  CCV_WC,
  CCV_CLASS
};

static int
next_state_class(CClassNode* cc, RegToken* tok, WCINT* vs,
		 enum CCVALTYPE* type, enum CCSTATE* state, ScanEnv* env)
{
  int r, c;

  if (*state == CCS_RANGE)
    return REGERR_CHAR_CLASS_VALUE_AT_END_OF_RANGE;

  if (*state == CCS_VALUE && *type != CCV_CLASS) {
    if (*type == CCV_SB)
      BITSET_SET_BIT(cc->bs, (int )(*vs));
    else if (*type == CCV_WC) {
      r = add_wc_range(&(cc->mbuf), env, *vs, *vs);
      if (r < 0) return r;
    }
  }

  if (tok->type == TK_CHAR_TYPE) {
    switch (tok->u.subtype) {
    case CTYPE_WORD:
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (IS_CODE_WORD(env->enc, c)) BITSET_SET_BIT(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(env->enc, cc->mbuf);
      break;
    case CTYPE_NOT_WORD:
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! IS_CODE_WORD(env->enc, c)) BITSET_SET_BIT(cc->bs, c);
      }
      break;
    case CTYPE_WHITE_SPACE:
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (IS_CODE_SPACE(env->enc, c)) BITSET_SET_BIT(cc->bs, c);
      }
      break;
    case CTYPE_NOT_WHITE_SPACE:
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! IS_CODE_SPACE(env->enc, c)) BITSET_SET_BIT(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(env->enc, cc->mbuf);
      break;
    case CTYPE_DIGIT:
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (IS_CODE_DIGIT(env->enc, c)) BITSET_SET_BIT(cc->bs, c);
      }
      break;
    case CTYPE_NOT_DIGIT:
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! IS_CODE_DIGIT(env->enc, c)) BITSET_SET_BIT(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(env->enc, cc->mbuf);
      break;
    default:
      return REGERR_PARSER_BUG;
      break;
    }
  }
  else { /* TK_POSIX_BRACKET_OPEN */
    /* nothing */
  }

  *state = CCS_VALUE;
  *type  = CCV_CLASS;
  return 0;
}

static int
next_state_val(CClassNode* cc, WCINT *vs, WCINT v, int* vs_israw, int v_israw,
	       enum CCVALTYPE intype, enum CCVALTYPE* type,
	       enum CCSTATE* state, ScanEnv* env)
{
  int r;

  switch (*state) {
  case CCS_VALUE:
    if (*type == CCV_SB)
      BITSET_SET_BIT(cc->bs, (int )(*vs));
    else if (*type == CCV_WC) {
      r = add_wc_range(&(cc->mbuf), env, *vs, *vs);
      if (r < 0) return r;
    }
    break;

  case CCS_RANGE:
    if (intype == *type) {
      if (intype == CCV_SB) {
	if (IS_IGNORECASE(env->option) && (*vs_israw == 0 && v_israw == 0)) {
	  int low, high;

	  low  = TOLOWER(env->enc, *vs);
	  high = TOLOWER(env->enc, v);
	  if (low > high) {
	    if (IS_SYNTAX_BV(env->syntax, REG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
	      goto ccs_range_end;
	    else
	      return REGERR_EMPTY_RANGE_IN_CHAR_CLASS;
	  }

	  if (low < 'A' && high >= 'a' && high <= 'z') {
	    bitset_set_range(cc->bs, low, (int )'A' - 1);
	    bitset_set_range(cc->bs, (int )'a', high);
	  }
	  else if (high > 'z' && low >= 'a' && low <= 'z') {
	    bitset_set_range(cc->bs, low, (int )'z');
	    bitset_set_range(cc->bs, (int )'z' + 1, high);
	  }
	  else {
	    bitset_set_range(cc->bs, low, high);
	  }
	}
	else {
	  if (*vs > v) {
	    if (IS_SYNTAX_BV(env->syntax, REG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
	      goto ccs_range_end;
	    else
	      return REGERR_EMPTY_RANGE_IN_CHAR_CLASS;
	  }
	  bitset_set_range(cc->bs, (int )*vs, (int )v);
	}
      }
      else {
	r = add_wc_range(&(cc->mbuf), env, *vs, v);
	if (r < 0) return r;
      }
    }
    else {
#ifndef REG_RUBY_M17N
      if (env->enc == REGCODE_UTF8 && intype == CCV_WC && *type == CCV_SB) {
	bitset_set_range(cc->bs, (int )*vs, 0x7f);
	r = add_wc_range(&(cc->mbuf), env, (WCINT )0x80, v);
	if (r < 0) return r;
      }
      else
#endif
	return REGERR_MISMATCH_CODE_LENGTH_IN_CLASS_RANGE;
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
char_exist_check(UChar c, UChar* from, UChar* to, int ignore_escaped,
		 RegCharEncoding enc)
{
  int in_esc;
  UChar* p = from;

  in_esc = 0;
  while (p < to) {
    if (ignore_escaped && in_esc) {
      in_esc = 0;
    }
    else {
      if (*p == c) return 1;
      if (*p == '\\') in_esc = 1;
    }
    p += mblen(enc, *p);
  }
  return 0;
}

static int
parse_char_class(Node** np, RegToken* tok, UChar** src, UChar* end,
		 ScanEnv* env)
{
  int r, neg, len, fetched, and_start;
  WCINT v, vs;
  UChar *p;
  Node* node;
  CClassNode *cc, *prev_cc;
  CClassNode work_cc;

  enum CCSTATE state;
  enum CCVALTYPE val_type, in_type;
  int val_israw, in_israw;

  *np = NULL_NODE;
  r = fetch_token_in_cc(tok, src, end, env);
  if (r == TK_BYTE && tok->u.c == '^') {
    neg = 1;
    r = fetch_token_in_cc(tok, src, end, env);
  }
  else {
    neg = 0;
  }

  if (r < 0) return r;
  if (r == TK_CC_CLOSE) {
    if (! char_exist_check(']', *src, env->pattern_end, 1, env->enc))
      return REGERR_EMPTY_CHAR_CLASS;

    CC_ESC_WARN(env, "]");
    r = tok->type = TK_BYTE;  /* allow []...] */
  }

  *np = node = node_new_cclass();
  CHECK_NULL_RETURN_VAL(node, REGERR_MEMORY);
  cc = &(NCCLASS(node));
  prev_cc = (CClassNode* )NULL;

  and_start = 0;
  state = CCS_START;
  p = *src;
  while (r != TK_CC_CLOSE) {
    fetched = 0;
    switch (r) {
    case TK_BYTE:
      len = mblen(env->enc, tok->u.c);
      if (len > 1) {
	PUNFETCH;
	v = MB2WC(p, end, env->enc);
	p += len;
      }
      else {
      sb_char:
	v = (WCINT )tok->u.c;
      }
      in_israw = 0;
      goto val_entry;
      break;

    case TK_RAW_BYTE:
      len = mblen(env->enc, tok->u.c);
      if (len > 1 && tok->base != 0) { /* tok->base != 0 : octal or hexadec. */
	UChar buf[WC2MB_MAX_BUFLEN];
	UChar* bufp = buf;
	UChar* bufe = buf + WC2MB_MAX_BUFLEN;
	int i, base = tok->base;

	if (len > WC2MB_MAX_BUFLEN) {
	  bufp = (UChar* )xmalloc(len);
	  if (IS_NULL(bufp)) {
	    r = REGERR_MEMORY;
	    goto err;
	  }
	  bufe = bufp + len;
	}
	bufp[0] = tok->u.c;
	for (i = 1; i < len; i++) {
	  r = fetch_token_in_cc(tok, &p, end, env);
	  if (r < 0) goto raw_byte_err;
	  if (r != TK_RAW_BYTE || tok->base != base) break;
	  bufp[i] = tok->u.c;
	}
	if (i < len) {
	  r = REGERR_TOO_SHORT_MULTI_BYTE_STRING;
	raw_byte_err:
	  if (bufp != buf) xfree(bufp);
	  goto err;
	}
	v = MB2WC(bufp, bufe, env->enc);
	fetched = 1;
	if (bufp != buf) xfree(bufp);
      }
      else {
	v = (WCINT )tok->u.c;
      }
      in_israw = 1;
      goto val_entry;
      break;

    case TK_WC:
      v = tok->u.wc;
      in_israw = 1;
    val_entry:
      in_type = (v < SINGLE_BYTE_SIZE ? CCV_SB : CCV_WC);
      r = next_state_val(cc, &vs, v, &val_israw, in_israw, in_type, &val_type,
			 &state, env);
      if (r != 0) goto err;
      break;

    case TK_POSIX_BRACKET_OPEN:
      r = parse_posix_bracket(cc, &p, end, env);
      if (r < 0) goto err;
      if (r == 1) {  /* is not POSIX bracket */
	CC_ESC_WARN(env, "[");
	p = tok->backp;
	v = (WCINT )tok->u.c;
	in_israw = 0;
	goto val_entry;
      }
      /* POSIX bracket fall */
    case TK_CHAR_TYPE:
      r = next_state_class(cc, tok, &vs, &val_type, &state, env);
      if (r != 0) goto err;
      break;

    case TK_CC_RANGE:
      if (state == CCS_VALUE) {
	r = fetch_token_in_cc(tok, &p, end, env);
	if (r < 0) goto err;
	fetched = 1;
	if (r == TK_CC_CLOSE) { /* allow [x-] */
	range_end_val:
	  v = (WCINT )'-';
	  in_israw = 0;
	  goto val_entry;
	}
	else if (r == TK_CC_AND) {
	  CC_ESC_WARN(env, "-");
	  goto range_end_val;
	}
	state = CCS_RANGE;
      }
      else if (state == CCS_START) {
	/* [-xa] is allowed */
	v = (WCINT )tok->u.c;
	in_israw = 0;

	r = fetch_token_in_cc(tok, &p, end, env);
	if (r < 0) goto err;
	fetched = 1;
	/* [--x] or [a&&-x] is warned. */
	if (r == TK_CC_RANGE || and_start != 0)
	  CC_ESC_WARN(env, "-");

	goto val_entry;
      }
      else if (state == CCS_RANGE) {
	CC_ESC_WARN(env, "-");
	goto sb_char;  /* [!--x] is allowed */
      }
      else { /* CCS_COMPLETE */
	r = fetch_token_in_cc(tok, &p, end, env);
	if (r < 0) goto err;
	fetched = 1;
	if (r == TK_CC_CLOSE) goto range_end_val; /* allow [a-b-] */
	else if (r == TK_CC_AND) {
	  CC_ESC_WARN(env, "-");
	  goto range_end_val;
	}
	
	if (IS_SYNTAX_BV(env->syntax, REG_SYN_ALLOW_RANGE_OP_IN_CC)) {
	  CC_ESC_WARN(env, "-");
	  goto sb_char;   /* [0-9-a] is allowed as [0-9\-a] */
	}
	r = REGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS;
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
	r = or_cclass(cc, acc);

      cc_open_err:
	regex_node_free(anode);
	if (r != 0) goto err;
      }
      break;

    case TK_CC_AND: /* && */
      {
	if (state == CCS_VALUE) {
	  r = next_state_val(cc, &vs, 0, &val_israw, 0, CCV_SB,
			     &val_type, &state, env);
	  if (r != 0) goto err;
	}
	/* initialize local variables */
	and_start = 1;
	state = CCS_START;

	if (IS_NOT_NULL(prev_cc)) {
	  r = and_cclass(prev_cc, cc);
	  if (r != 0) goto err;
	}
	else {
	  prev_cc = cc;
	  cc = &work_cc;
	}
	initialize_cclass(cc);
      }
      break;

    case TK_EOT:
      r = REGERR_PREMATURE_END_OF_CHAR_CLASS;
      goto err;
      break;
    default:
      r = REGERR_PARSER_BUG;
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
    r = next_state_val(cc, &vs, 0, &val_israw, 0, CCV_SB,
		       &val_type, &state, env);
    if (r != 0) goto err;
  }

  if (IS_NOT_NULL(prev_cc)) {
    r = and_cclass(prev_cc, cc);
    if (r != 0) goto err;
    cc = prev_cc;
  }

  cc->not = neg;
  if (cc->not != 0 &&
      IS_SYNTAX_BV(env->syntax, REG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC)) {
    int is_empty;

    is_empty = (IS_NULL(cc->mbuf) ? 1 : 0);
    if (is_empty != 0)
      BITSET_IS_EMPTY(cc->bs, is_empty);
    if (is_empty == 0)
      BITSET_SET_BIT(cc->bs, NEWLINE);
  }
  *src = p;
  return 0;

 err:
  regex_node_free(*np);
  return r;
}

static int parse_subexp(Node** top, RegToken* tok, int term,
			UChar** src, UChar* end, ScanEnv* env);

static int
parse_effect(Node** np, RegToken* tok, int term, UChar** src, UChar* end,
	     ScanEnv* env)
{
  Node *target;
  RegOptionType option;
  int r, c, num;
  UChar* p = *src;

  *np = NULL;
  if (PEND) return REGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS;

  option = env->option;
  if (PPEEK == '?' && IS_SYNTAX_OP(env->syntax, REG_SYN_OP_SUBEXP_EFFECT)) {
    PINC;
    if (PEND) return REGERR_END_PATTERN_IN_GROUP;

    PFETCH(c);
    switch (c) {
    case '#':   /* (?#...) comment */
      while (1) {
	if (PEND) return REGERR_END_PATTERN_IN_GROUP;
	PFETCH(c);
	if (c == ')') break;
      }
      *src = p;
      return 3; /* 3: comment */
      break;

    case ':':            /* (?:...) grouping only */
      goto group;
      break;

    case '=':
      *np = regex_node_new_anchor(ANCHOR_PREC_READ);
      break;
    case '!':  /*         preceding read */
      *np = regex_node_new_anchor(ANCHOR_PREC_READ_NOT);
      break;
    case '>':            /* (?>...) stop backtrack */
      *np = node_new_effect(EFFECT_STOP_BACKTRACK);
      break;

    case '<':   /* look behind (?<=...), (?<!...) */
      PFETCH(c);
      if (c == '=')
	*np = regex_node_new_anchor(ANCHOR_LOOK_BEHIND);
      else if (c == '!')
	*np = regex_node_new_anchor(ANCHOR_LOOK_BEHIND_NOT);
#ifdef USE_NAMED_SUBEXP
      else if (IS_SYNTAX_OP2(env->syntax, REG_SYN_OP2_NAMED_SUBEXP)) {
	UChar *name;
	UChar *name_end;
	PUNFETCH;
	name = p;
	r = fetch_name(&p, end, &name_end, env);
	if (r < 0) return r;

	*np = node_new_effect(EFFECT_MEMORY);
	CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
	num = scan_env_add_mem_entry(env);
	if (num < 0) return num;
	NEFFECT(*np).regnum = num;
	r = name_add(env->reg, name, name_end, num);
	if (r != 0) return r;
      }
#endif
      else
	return REGERR_UNDEFINED_GROUP_OPTION;
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
	  case 'x':  ONOFF(option, REG_OPTION_EXTEND,     neg); break;
	  case 'i':  ONOFF(option, REG_OPTION_IGNORECASE, neg); break;
	  case 's':
	    if (IS_SYNTAX_OP2(env->syntax, REG_SYN_OP2_OPTION_PERL)) {
	      ONOFF(option, REG_OPTION_MULTILINE,  neg);
	    }
	    else
	      return REGERR_UNDEFINED_GROUP_OPTION;
	    break;

	  case 'm':
	    if (IS_SYNTAX_OP2(env->syntax, REG_SYN_OP2_OPTION_PERL)) {
	      ONOFF(option, REG_OPTION_SINGLELINE, (neg == 0 ? 1 : 0));
	    }
	    else if (IS_SYNTAX_OP2(env->syntax, REG_SYN_OP2_OPTION_RUBY)) {
	      ONOFF(option, REG_OPTION_MULTILINE,  neg);
	    }
	    else
	      return REGERR_UNDEFINED_GROUP_OPTION;
	    break;
#ifdef USE_POSIXLINE_OPTION
	  case 'p':
	    ONOFF(option, REG_OPTION_MULTILINE|REG_OPTION_SINGLELINE, neg);
	    break;
#endif
	  default:
	    return REGERR_UNDEFINED_GROUP_OPTION;
	  }

	  if (c == ')') { /* option only */
	    if (option == env->option) {
	      *np = node_new_empty();
	      CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
	      *src = p;
	      return 0;
	    }
	    else {
	      *np = node_new_option(option);
	      CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
	      *src = p;
	      return 2; /* option only */
	    }
	  }
	  else if (c == ':') {
	    if (env->option == option) {
	    group:
	      r = fetch_token(tok, &p, end, env);
	      if (r < 0) return r;
	      r = parse_subexp(np, tok, term, &p, end, env);
	      if (r < 0) return r;
	      *src = p;
	      return 1; /* group */
	    }
	    else {
	      RegOptionType prev = env->option;

	      env->option = option;
	      r = fetch_token(tok, &p, end, env);
	      if (r < 0) return r;
	      r = parse_subexp(&target, tok, term, &p, end, env);
	      env->option = prev;
	      if (r < 0) return r;
	      *np = node_new_option(option);
	      CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
	      NEFFECT(*np).target = target;
	      *src = p;
	      return 0;
	    }
	  }

	  if (PEND) return REGERR_END_PATTERN_IN_GROUP;
	  PFETCH(c);
	}
      }
      break;

    default:
      return REGERR_UNDEFINED_GROUP_OPTION;
    }
  }
  else {
#ifdef USE_NAMED_SUBEXP
    if (IS_REG_OPTION_ON(env->option, REG_OPTION_CAPTURE_ONLY_NAMED_GROUP)) {
      goto group;
    }
#endif
    *np = node_new_effect(EFFECT_MEMORY);
    CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
    num = scan_env_add_mem_entry(env);
    if (num < 0) return num;
    NEFFECT(*np).regnum = num;
  }

  CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
  r = fetch_token(tok, &p, end, env);
  if (r < 0) return r;
  r = parse_subexp(&target, tok, term, &p, end, env);
  if (r < 0) return r;

  if (NTYPE(*np) == N_ANCHOR)
    NANCHOR(*np).target = target;
  else
    NEFFECT(*np).target = target;

  *src = p;
  return 0;
}

static int
set_qualifier(Node* qnode, Node* target, int group, ScanEnv* env)
{
  QualifierNode* qn;

  qn = &(NQUALIFIER(qnode));
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

  case N_QUALIFIER:
    { /* check redundant double repeat. */
      /* verbose warn (?:.?)? etc... but not warn (.?)? etc... */
      QualifierNode* qnt = &(NQUALIFIER(target));

#ifdef USE_WARNING_REDUNDANT_NESTED_REPEAT_OPERATOR
      if (qn->by_number == 0 && qnt->by_number == 0) {
	if (IS_REPEAT_INFINITE(qn->upper)) {
	  if (qn->lower == 0) { /* '*' */
	  redundant:
	    {
	      char buf[WARN_BUFSIZE];
	      regex_snprintf_with_pattern(buf, WARN_BUFSIZE, env->enc,
			  env->pattern, env->pattern_end,
			  "redundant nested repeat operator");
	      VERB_WARNING(buf);
	      goto warn_exit;
	    }
	  }
	  else if (qn->lower == 1) { /* '+' */
	    /* (?:a?)+? only allowed. */
	    if (qn->greedy || !(qnt->upper == 1 && qnt->greedy))
	      goto redundant;
	  }
	}
	else if (qn->upper == 1 && qn->lower == 0) {
	  if (qn->greedy) { /* '?' */
	    if (!(qnt->lower == 1 && qnt->greedy == 0)) /* not '+?' */
	      goto redundant;
	  }
	  else { /* '??' */
	    /* '(?:a+)?? only allowd. (?:a*)?? can be replaced to (?:a+)?? */
	    if (!(qnt->greedy && qnt->lower == 1 &&
		  IS_REPEAT_INFINITE(qnt->upper)))
	      goto redundant;
	  }
	}
      }
#endif

#ifdef USE_WARNING_REDUNDANT_NESTED_REPEAT_OPERATOR
    warn_exit:
#endif
      if (popular_qualifier_num(qnt) >= 0 && popular_qualifier_num(qn) >= 0) {
	reduce_nested_qualifier(qnode, target);
	goto q_exit;
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

static int
parse_exp(Node** np, RegToken* tok, int term,
	  UChar** src, UChar* end, ScanEnv* env)
{
  int r, len, c, group = 0;
  Node* qn;

 start:
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
      r = fetch_token(tok, src, end, env);
      if (r < 0) return r;
      r = parse_subexp(&target, tok, term, src, end, env);
      if (r < 0) return r;
      NEFFECT(*np).target = target;	
      return tok->type;
    }
    else if (r == 3) { /* comment */
      r = fetch_token(tok, src, end, env);
      if (r < 0) return r;
      goto start;
    }
    else {
      if (NTYPE(*np) == N_EFFECT && NEFFECT(*np).type == EFFECT_MEMORY) {
	r = scan_env_set_mem_node(env, NEFFECT(*np).regnum, *np);
	if (r != 0) return r;
      }
    }
    break;

  case TK_SUBEXP_CLOSE:
    if (! IS_SYNTAX_BV(env->syntax, REG_SYN_ALLOW_UNMATCHED_CLOSE_SUBEXP))
      return REGERR_UNMATCHED_CLOSE_PARENTHESIS;

    if (tok->escaped) goto tk_raw_byte;
    else goto tk_byte;
    break;

  case TK_BYTE:
  tk_byte:
    *np = node_new_str_char((UChar )tok->u.c);
    CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
    len = mblen(env->enc, tok->u.c);
    if (len > 1) {
      regex_node_str_cat(*np, *src, *src + len - 1);
      *src += (len - 1);
    }
    while (1) {
      r = fetch_token(tok, src, end, env);
      if (r < 0) return r;
      if (r != TK_BYTE) goto repeat;

      r = node_str_cat_char(*np, (UChar )tok->u.c);
      if (r < 0) return r;
      len = mblen(env->enc, tok->u.c);
      if (len > 1) {
	regex_node_str_cat(*np, *src, *src + len - 1);
	*src += (len - 1);
      }
    }
    break;

  case TK_RAW_BYTE:
  tk_raw_byte:
    *np = node_new_str_raw_char((UChar )tok->u.c);
    CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
    while (1) {
      r = fetch_token(tok, src, end, env);
      if (r < 0) return r;
      if (r != TK_RAW_BYTE) goto repeat;

      r = node_str_cat_char(*np, (UChar )tok->u.c);
      if (r < 0) return r;
    }
    break;

  case TK_WC:
    {
      UChar buf[WC2MB_MAX_BUFLEN];
      UChar* bufs = buf;
      UChar* bufe = bufs + WC2MB_MAX_BUFLEN;
      int num = wc2mb_buf(tok->u.wc, &bufs, &bufe, env->enc);
      if (num < 0) return num;
      *np = node_new_str_raw(bufs, bufe);
      if (bufs != buf) xfree(bufs);
      CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
    }
    break;

  case TK_QUOTE_OPEN:
    {
      WCINT end_op[] = { (WCINT )'\\', (WCINT )'E' };
      UChar *qstart, *qend, *nextp;

      qstart = *src;
      qend = find_str_position(end_op, 2, qstart, end, &nextp, env->enc);
      if (IS_NULL(qend)) {
	nextp = qend = end;
      }
      *np = node_new_str(qstart, qend);
      CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
      *src = nextp;
    }
    break;

  case TK_CHAR_TYPE:
    switch (tok->u.subtype) {
    case CTYPE_WORD:
    case CTYPE_NOT_WORD:
      *np = node_new_ctype(tok->u.subtype);
      CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
      break;

    case CTYPE_WHITE_SPACE:
      *np = node_new_cclass();
      CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (IS_CODE_SPACE(env->enc, c)) BITSET_SET_BIT(NCCLASS(*np).bs, c);
      }
      break;

    case CTYPE_NOT_WHITE_SPACE:
      *np = node_new_cclass();
      CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! IS_CODE_SPACE(env->enc, c)) BITSET_SET_BIT(NCCLASS(*np).bs, c);
      }
      break;

    case CTYPE_DIGIT:
      *np = node_new_cclass();
      CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (IS_CODE_DIGIT(env->enc, c)) BITSET_SET_BIT(NCCLASS(*np).bs, c);
      }
      break;

    case CTYPE_NOT_DIGIT:
      *np = node_new_cclass();
      CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! IS_CODE_DIGIT(env->enc, c)) BITSET_SET_BIT(NCCLASS(*np).bs, c);
      }
      break;

    default:
      return REGERR_PARSER_BUG;
      break;
    }
    break;

  case TK_CC_OPEN:
    r = parse_char_class(np, tok, src, end, env);
    if (r != 0) return r;
    break;

  case TK_ANYCHAR:
    *np = node_new_anychar();
    break;

  case TK_BACKREF:
    len = tok->u.backref.num;
    *np = node_new_backref(len,
	       (len > 1 ? tok->u.backref.refs : &(tok->u.backref.ref1)), env);
    CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
    break;

#ifdef USE_SUBEXP_CALL
  case TK_CALL:
    *np = node_new_call(tok->u.call.name, tok->u.call.name_end);
    CHECK_NULL_RETURN_VAL(*np, REGERR_MEMORY);
    env->num_call++;
    break;
#endif

  case TK_ANCHOR:
    *np = regex_node_new_anchor(tok->u.anchor);
    break;

  case TK_OP_REPEAT:
  case TK_INTERVAL:
    if (IS_SYNTAX_BV(env->syntax, REG_SYN_CONTEXT_INDEP_OPS)) {
      if (IS_SYNTAX_BV(env->syntax, REG_SYN_CONTEXT_INVALID_OPS))
	return REGERR_TARGET_OF_REPEAT_OPERATOR_NOT_SPECIFIED;
      else
	*np = node_new_empty();
    }
    else {
      *src = tok->backp;
      goto tk_byte;
    }
    break;

  default:
    return REGERR_PARSER_BUG;
    break;
  }

 re_entry:
  r = fetch_token(tok, src, end, env);
  if (r < 0) return r;

 repeat:
  if (r == TK_OP_REPEAT || r == TK_INTERVAL) {
    if (is_invalid_qualifier_target(*np))
      return REGERR_TARGET_OF_REPEAT_OPERATOR_INVALID;

    qn = node_new_qualifier(tok->u.repeat.lower, tok->u.repeat.upper,
			    (r == TK_INTERVAL ? 1 : 0));
    CHECK_NULL_RETURN_VAL(qn, REGERR_MEMORY);
    NQUALIFIER(qn).greedy = tok->u.repeat.greedy;
    r = set_qualifier(qn, *np, group, env);
    if (r < 0) return r;

    if (tok->u.repeat.possessive != 0) {
      Node* en;
      en = node_new_effect(EFFECT_STOP_BACKTRACK);
      CHECK_NULL_RETURN_VAL(en, REGERR_MEMORY);
      NEFFECT(en).target = qn;
      qn = en;
    }

    if (r == 0) {
      *np = qn;
    }
    else if (r == 2) { /* split case: /abc+/ */
      Node* target = *np;
      *np = node_new_list(target, NULL);
      NCONS(*np).right = node_new_list(qn, NULL);
    }
    goto re_entry;
  }

  return r;
}

static int
parse_branch(Node** top, RegToken* tok, int term,
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
parse_subexp(Node** top, RegToken* tok, int term,
	     UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  Node *node, **headp;

  *top = NULL;
  r = parse_branch(&node, tok, term, src, end, env);
  if (r < 0) return r;

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
      return REGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS;
    else
      return REGERR_PARSER_BUG;
  }

  return r;
}

static int
parse_regexp(Node** top, UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  RegToken tok;

  r = fetch_token(&tok, src, end, env);
  if (r < 0) return r;
  r = parse_subexp(top, &tok, TK_EOT, src, end, env);
  if (r < 0) return r;
  return 0;
}

extern int
regex_parse_make_tree(Node** root, UChar* pattern, UChar* end, regex_t* reg,
		      ScanEnv* env)
{
  int r;
  UChar* p;

#ifdef USE_NAMED_SUBEXP
  names_clear(reg);
#endif

  scan_env_clear(env);
  env->option      = reg->options;
  env->enc         = reg->enc;
  env->syntax      = reg->syntax;
  env->pattern     = pattern;
  env->pattern_end = end;
  env->reg         = reg;

  *root = NULL;
  p = pattern;
  r = parse_regexp(root, &p, end, env);
  reg->num_mem = env->num_mem;
  return r;
}

extern void
regex_scan_env_set_error_string(ScanEnv* env, int ecode,
				UChar* arg, UChar* arg_end)
{
  env->error     = arg;
  env->error_end = arg_end;
}
