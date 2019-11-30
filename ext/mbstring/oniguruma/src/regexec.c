/**********************************************************************
  regexec.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2019  K.Kosako
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
#include "regint.h"

#define IS_MBC_WORD_ASCII_MODE(enc,s,end,mode) \
  ((mode) == 0 ? ONIGENC_IS_MBC_WORD(enc,s,end) : ONIGENC_IS_MBC_WORD_ASCII(enc,s,end))

#ifdef USE_CRNL_AS_LINE_TERMINATOR
#define ONIGENC_IS_MBC_CRNL(enc,p,end) \
  (ONIGENC_MBC_TO_CODE(enc,p,end) == 13 && \
   ONIGENC_IS_MBC_NEWLINE(enc,(p+enclen(enc,p)),end))
#endif

#define CHECK_INTERRUPT_IN_MATCH

#define STACK_MEM_START(reg, i) \
  (MEM_STATUS_AT((reg)->push_mem_start, (i)) != 0 ? \
   STACK_AT(mem_start_stk[i])->u.mem.pstr : (UChar* )((void* )(mem_start_stk[i])))

#define STACK_MEM_END(reg, i) \
  (MEM_STATUS_AT((reg)->push_mem_end, (i)) != 0 ? \
   STACK_AT(mem_end_stk[i])->u.mem.pstr : (UChar* )((void* )(mem_end_stk[i])))

static int forward_search(regex_t* reg, const UChar* str, const UChar* end, UChar* start, UChar* range, UChar** low, UChar** high, UChar** low_prev);

static int
search_in_range(regex_t* reg, const UChar* str, const UChar* end, const UChar* start, const UChar* range, /* match range */ const UChar* data_range, /* subject string range */ OnigRegion* region, OnigOptionType option, OnigMatchParam* mp);


#ifdef USE_CALLOUT
typedef struct {
  int last_match_at_call_counter;
  struct {
    OnigType  type;
    OnigValue val;
  } slot[ONIG_CALLOUT_DATA_SLOT_NUM];
} CalloutData;
#endif

struct OnigMatchParamStruct {
  unsigned int    match_stack_limit;
  unsigned long   retry_limit_in_match;
#ifdef USE_CALLOUT
  OnigCalloutFunc progress_callout_of_contents;
  OnigCalloutFunc retraction_callout_of_contents;
  int             match_at_call_counter;
  void*           callout_user_data;
  CalloutData*    callout_data;
  int             callout_data_alloc_num;
#endif
};

extern int
onig_set_match_stack_limit_size_of_match_param(OnigMatchParam* param,
                                               unsigned int limit)
{
  param->match_stack_limit = limit;
  return ONIG_NORMAL;
}

extern int
onig_set_retry_limit_in_match_of_match_param(OnigMatchParam* param,
                                             unsigned long limit)
{
  param->retry_limit_in_match = limit;
  return ONIG_NORMAL;
}

extern int
onig_set_progress_callout_of_match_param(OnigMatchParam* param, OnigCalloutFunc f)
{
#ifdef USE_CALLOUT
  param->progress_callout_of_contents = f;
  return ONIG_NORMAL;
#else
  return ONIG_NO_SUPPORT_CONFIG;
#endif
}

extern int
onig_set_retraction_callout_of_match_param(OnigMatchParam* param, OnigCalloutFunc f)
{
#ifdef USE_CALLOUT
  param->retraction_callout_of_contents = f;
  return ONIG_NORMAL;
#else
  return ONIG_NO_SUPPORT_CONFIG;
#endif
}

extern int
onig_set_callout_user_data_of_match_param(OnigMatchParam* param, void* user_data)
{
#ifdef USE_CALLOUT
  param->callout_user_data = user_data;
  return ONIG_NORMAL;
#else
  return ONIG_NO_SUPPORT_CONFIG;
#endif
}


typedef struct {
  void* stack_p;
  int   stack_n;
  OnigOptionType options;
  OnigRegion*    region;
  int            ptr_num;
  const UChar*   start;   /* search start position (for \G: BEGIN_POSITION) */
  unsigned int   match_stack_limit;
  unsigned long  retry_limit_in_match;
  OnigMatchParam* mp;
#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
  int    best_len;      /* for ONIG_OPTION_FIND_LONGEST */
  UChar* best_s;
#endif
} MatchArg;


#if defined(ONIG_DEBUG_COMPILE) || defined(ONIG_DEBUG_MATCH)

/* arguments type */
typedef enum {
  ARG_SPECIAL = -1,
  ARG_NON     =  0,
  ARG_RELADDR =  1,
  ARG_ABSADDR =  2,
  ARG_LENGTH  =  3,
  ARG_MEMNUM  =  4,
  ARG_OPTION  =  5,
  ARG_MODE    =  6
} OpArgType;

typedef struct {
  short int opcode;
  char*     name;
} OpInfoType;

static OpInfoType OpInfo[] = {
  { OP_FINISH,         "finish"},
  { OP_END,            "end"},
  { OP_STR_1,          "str_1"},
  { OP_STR_2,          "str_2"},
  { OP_STR_3,          "str_3"},
  { OP_STR_4,          "str_4"},
  { OP_STR_5,          "str_5"},
  { OP_STR_N,          "str_n"},
  { OP_STR_MB2N1,      "str_mb2-n1"},
  { OP_STR_MB2N2,      "str_mb2-n2"},
  { OP_STR_MB2N3,      "str_mb2-n3"},
  { OP_STR_MB2N,       "str_mb2-n"},
  { OP_STR_MB3N,       "str_mb3n"},
  { OP_STR_MBN,        "str_mbn"},
  { OP_STR_1_IC,       "str_1-ic"},
  { OP_STR_N_IC,       "str_n-ic"},
  { OP_CCLASS,         "cclass"},
  { OP_CCLASS_MB,      "cclass-mb"},
  { OP_CCLASS_MIX,     "cclass-mix"},
  { OP_CCLASS_NOT,     "cclass-not"},
  { OP_CCLASS_MB_NOT,  "cclass-mb-not"},
  { OP_CCLASS_MIX_NOT, "cclass-mix-not"},
  { OP_ANYCHAR,               "anychar"},
  { OP_ANYCHAR_ML,            "anychar-ml"},
  { OP_ANYCHAR_STAR,          "anychar*"},
  { OP_ANYCHAR_ML_STAR,       "anychar-ml*"},
  { OP_ANYCHAR_STAR_PEEK_NEXT,    "anychar*-peek-next"},
  { OP_ANYCHAR_ML_STAR_PEEK_NEXT, "anychar-ml*-peek-next"},
  { OP_WORD,                  "word"},
  { OP_WORD_ASCII,            "word-ascii"},
  { OP_NO_WORD,               "not-word"},
  { OP_NO_WORD_ASCII,         "not-word-ascii"},
  { OP_WORD_BOUNDARY,         "word-boundary"},
  { OP_NO_WORD_BOUNDARY,      "not-word-boundary"},
  { OP_WORD_BEGIN,            "word-begin"},
  { OP_WORD_END,              "word-end"},
  { OP_TEXT_SEGMENT_BOUNDARY, "text-segment-boundary"},
  { OP_BEGIN_BUF,             "begin-buf"},
  { OP_END_BUF,               "end-buf"},
  { OP_BEGIN_LINE,            "begin-line"},
  { OP_END_LINE,              "end-line"},
  { OP_SEMI_END_BUF,          "semi-end-buf"},
  { OP_BEGIN_POSITION,        "begin-position"},
  { OP_BACKREF1,              "backref1"},
  { OP_BACKREF2,              "backref2"},
  { OP_BACKREF_N,             "backref-n"},
  { OP_BACKREF_N_IC,          "backref-n-ic"},
  { OP_BACKREF_MULTI,         "backref_multi"},
  { OP_BACKREF_MULTI_IC,      "backref_multi-ic"},
  { OP_BACKREF_WITH_LEVEL,    "backref_with_level"},
  { OP_BACKREF_WITH_LEVEL_IC, "backref_with_level-c"},
  { OP_BACKREF_CHECK,         "backref_check"},
  { OP_BACKREF_CHECK_WITH_LEVEL, "backref_check_with_level"},
  { OP_MEM_START_PUSH,        "mem-start-push"},
  { OP_MEM_START,             "mem-start"},
  { OP_MEM_END_PUSH,          "mem-end-push"},
#ifdef USE_CALL
  { OP_MEM_END_PUSH_REC,      "mem-end-push-rec"},
#endif
  { OP_MEM_END,               "mem-end"},
#ifdef USE_CALL
  { OP_MEM_END_REC,           "mem-end-rec"},
#endif
  { OP_FAIL,                  "fail"},
  { OP_JUMP,                  "jump"},
  { OP_PUSH,                  "push"},
  { OP_PUSH_SUPER,            "push-super"},
  { OP_POP_OUT,               "pop-out"},
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
  { OP_PUSH_OR_JUMP_EXACT1,   "push-or-jump-e1"},
#endif
  { OP_PUSH_IF_PEEK_NEXT,     "push-if-peek-next"},
  { OP_REPEAT,                "repeat"},
  { OP_REPEAT_NG,             "repeat-ng"},
  { OP_REPEAT_INC,            "repeat-inc"},
  { OP_REPEAT_INC_NG,         "repeat-inc-ng"},
  { OP_EMPTY_CHECK_START,     "empty-check-start"},
  { OP_EMPTY_CHECK_END,       "empty-check-end"},
  { OP_EMPTY_CHECK_END_MEMST, "empty-check-end-memst"},
#ifdef USE_CALL
  { OP_EMPTY_CHECK_END_MEMST_PUSH,"empty-check-end-memst-push"},
#endif
  { OP_PREC_READ_START,       "push-pos"},
  { OP_PREC_READ_END,         "pop-pos"},
  { OP_PREC_READ_NOT_START,   "prec-read-not-start"},
  { OP_PREC_READ_NOT_END,     "prec-read-not-end"},
  { OP_ATOMIC_START,          "atomic-start"},
  { OP_ATOMIC_END,            "atomic-end"},
  { OP_LOOK_BEHIND,           "look-behind"},
  { OP_LOOK_BEHIND_NOT_START, "look-behind-not-start"},
  { OP_LOOK_BEHIND_NOT_END,   "look-behind-not-end"},
  { OP_PUSH_SAVE_VAL,         "push-save-val"},
  { OP_UPDATE_VAR,            "update-var"},
#ifdef USE_CALL
  { OP_CALL,                  "call"},
  { OP_RETURN,                "return"},
#endif
#ifdef USE_CALLOUT
  { OP_CALLOUT_CONTENTS,      "callout-contents"},
  { OP_CALLOUT_NAME,          "callout-name"},
#endif
  { -1, ""}
};

static char*
op2name(int opcode)
{
  int i;

  for (i = 0; OpInfo[i].opcode >= 0; i++) {
    if (opcode == OpInfo[i].opcode) return OpInfo[i].name;
  }

  return "";
}

static void
p_string(FILE* f, int len, UChar* s)
{
  fputs(":", f);
  while (len-- > 0) { fputc(*s++, f); }
}

static void
p_len_string(FILE* f, LengthType len, int mb_len, UChar* s)
{
  int x = len * mb_len;

  fprintf(f, ":%d:", len);
  while (x-- > 0) { fputc(*s++, f); }
}

static void
p_rel_addr(FILE* f, RelAddrType rel_addr, Operation* p, Operation* start)
{
  RelAddrType curr = (RelAddrType )(p - start);

  fprintf(f, "{%d/%d}", rel_addr, curr + rel_addr);
}

static int
bitset_on_num(BitSetRef bs)
{
  int i, n;

  n = 0;
  for (i = 0; i < SINGLE_BYTE_SIZE; i++) {
    if (BITSET_AT(bs, i)) n++;
  }

  return n;
}

static void
print_compiled_byte_code(FILE* f, regex_t* reg, int index,
                         Operation* start, OnigEncoding enc)
{
  int i, n;
  RelAddrType addr;
  LengthType  len;
  MemNumType  mem;
  OnigCodePoint code;
  ModeType mode;
  UChar *q;
  Operation* p;
  enum OpCode opcode;

  p = reg->ops + index;

#ifdef USE_DIRECT_THREADED_CODE
  opcode = reg->ocs[index];
#else
  opcode = p->opcode;
#endif

  fprintf(f, "%s", op2name(opcode));
  switch (opcode) {
  case OP_STR_1:
    p_string(f, 1, p->exact.s); break;
  case OP_STR_2:
    p_string(f, 2, p->exact.s); break;
  case OP_STR_3:
    p_string(f, 3, p->exact.s); break;
  case OP_STR_4:
    p_string(f, 4, p->exact.s); break;
  case OP_STR_5:
    p_string(f, 5, p->exact.s); break;
  case OP_STR_N:
    len = p->exact_n.n;
    p_string(f, len, p->exact_n.s); break;
  case OP_STR_MB2N1:
    p_string(f, 2, p->exact.s); break;
  case OP_STR_MB2N2:
    p_string(f, 4, p->exact.s); break;
  case OP_STR_MB2N3:
    p_string(f, 3, p->exact.s); break;
  case OP_STR_MB2N:
    len = p->exact_n.n;
    p_len_string(f, len, 2, p->exact_n.s); break;
  case OP_STR_MB3N:
    len = p->exact_n.n;
    p_len_string(f, len, 3, p->exact_n.s); break;
  case OP_STR_MBN:
    {
      int mb_len;

      mb_len = p->exact_len_n.len;
      len    = p->exact_len_n.n;
      q      = p->exact_len_n.s;
      fprintf(f, ":%d:%d:", mb_len, len);
      n = len * mb_len;
      while (n-- > 0) { fputc(*q++, f); }
    }
    break;
  case OP_STR_1_IC:
    len = enclen(enc, p->exact.s);
    p_string(f, len, p->exact.s);
    break;
  case OP_STR_N_IC:
    len = p->exact_n.n;
    p_len_string(f, len, 1, p->exact_n.s);
    break;

  case OP_CCLASS:
  case OP_CCLASS_NOT:
    n = bitset_on_num(p->cclass.bsp);
    fprintf(f, ":%d", n);
    break;
  case OP_CCLASS_MB:
  case OP_CCLASS_MB_NOT:
    {
      OnigCodePoint ncode;
      OnigCodePoint* codes;

      codes = (OnigCodePoint* )p->cclass_mb.mb;
      GET_CODE_POINT(ncode, codes);
      codes++;
      GET_CODE_POINT(code, codes);
      fprintf(f, ":%d:0x%x", ncode, code);
    }
    break;
  case OP_CCLASS_MIX:
  case OP_CCLASS_MIX_NOT:
    {
      OnigCodePoint ncode;
      OnigCodePoint* codes;

      codes = (OnigCodePoint* )p->cclass_mix.mb;
      n = bitset_on_num(p->cclass_mix.bsp);

      GET_CODE_POINT(ncode, codes);
      codes++;
      GET_CODE_POINT(code, codes);
      fprintf(f, ":%d:%u:%u", n, code, ncode);
    }
    break;

  case OP_ANYCHAR_STAR_PEEK_NEXT:
  case OP_ANYCHAR_ML_STAR_PEEK_NEXT:
    p_string(f, 1, &(p->anychar_star_peek_next.c));
    break;

  case OP_WORD_BOUNDARY:
  case OP_NO_WORD_BOUNDARY:
  case OP_WORD_BEGIN:
  case OP_WORD_END:
    mode = p->word_boundary.mode;
    fprintf(f, ":%d", mode);
    break;

  case OP_BACKREF_N:
  case OP_BACKREF_N_IC:
    mem = p->backref_n.n1;
    fprintf(f, ":%d", mem);
    break;
  case OP_BACKREF_MULTI_IC:
  case OP_BACKREF_MULTI:
  case OP_BACKREF_CHECK:
    fputs(" ", f);
    n = p->backref_general.num;
    for (i = 0; i < n; i++) {
      mem = (n == 1) ? p->backref_general.n1 : p->backref_general.ns[i];
      if (i > 0) fputs(", ", f);
      fprintf(f, "%d", mem);
    }
    break;
  case OP_BACKREF_WITH_LEVEL:
  case OP_BACKREF_WITH_LEVEL_IC:
  case OP_BACKREF_CHECK_WITH_LEVEL:
    {
      LengthType level;

      level = p->backref_general.nest_level;
      fprintf(f, ":%d", level);
      fputs(" ", f);
      n = p->backref_general.num;
      for (i = 0; i < n; i++) {
        mem = (n == 1) ? p->backref_general.n1 : p->backref_general.ns[i];
        if (i > 0) fputs(", ", f);
        fprintf(f, "%d", mem);
      }
    }
    break;

  case OP_MEM_START:
  case OP_MEM_START_PUSH:
    mem = p->memory_start.num;
    fprintf(f, ":%d", mem);
    break;

  case OP_MEM_END:
  case OP_MEM_END_PUSH:
#ifdef USE_CALL
  case OP_MEM_END_REC:
  case OP_MEM_END_PUSH_REC:
#endif
    mem = p->memory_end.num;
    fprintf(f, ":%d", mem);
    break;

  case OP_JUMP:
    addr = p->jump.addr;
    fputc(':', f);
    p_rel_addr(f, addr, p, start);
    break;

  case OP_PUSH:
  case OP_PUSH_SUPER:
    addr = p->push.addr;
    fputc(':', f);
    p_rel_addr(f, addr, p, start);
    break;

#ifdef USE_OP_PUSH_OR_JUMP_EXACT
  case OP_PUSH_OR_JUMP_EXACT1:
    addr = p->push_or_jump_exact1.addr;
    fputc(':', f);
    p_rel_addr(f, addr, p, start);
    p_string(f, 1, &(p->push_or_jump_exact1.c));
    break;
#endif

  case OP_PUSH_IF_PEEK_NEXT:
    addr = p->push_if_peek_next.addr;
    fputc(':', f);
    p_rel_addr(f, addr, p, start);
    p_string(f, 1, &(p->push_if_peek_next.c));
    break;

  case OP_REPEAT:
  case OP_REPEAT_NG:
    mem = p->repeat.id;
    addr = p->repeat.addr;
    fprintf(f, ":%d:", mem);
    p_rel_addr(f, addr, p, start);
    break;

  case OP_REPEAT_INC:
  case OP_REPEAT_INC_NG:
    mem = p->repeat.id;
    fprintf(f, ":%d", mem);
    break;

  case OP_EMPTY_CHECK_START:
    mem = p->empty_check_start.mem;
    fprintf(f, ":%d", mem);
    break;
  case OP_EMPTY_CHECK_END:
  case OP_EMPTY_CHECK_END_MEMST:
#ifdef USE_CALL
  case OP_EMPTY_CHECK_END_MEMST_PUSH:
#endif
    mem = p->empty_check_end.mem;
    fprintf(f, ":%d", mem);
    break;

  case OP_PREC_READ_NOT_START:
    addr = p->prec_read_not_start.addr;
    fputc(':', f);
    p_rel_addr(f, addr, p, start);
    break;

  case OP_LOOK_BEHIND:
    len = p->look_behind.len;
    fprintf(f, ":%d", len);
    break;

  case OP_LOOK_BEHIND_NOT_START:
    addr = p->look_behind_not_start.addr;
    len  = p->look_behind_not_start.len;
    fprintf(f, ":%d:", len);
    p_rel_addr(f, addr, p, start);
    break;

#ifdef USE_CALL
  case OP_CALL:
    addr = p->call.addr;
    fprintf(f, ":{/%d}", addr);
    break;
#endif

  case OP_PUSH_SAVE_VAL:
    {
      SaveType type;

      type = p->push_save_val.type;
      mem  = p->push_save_val.id;
      fprintf(f, ":%d:%d", type, mem);
    }
    break;

  case OP_UPDATE_VAR:
    {
      UpdateVarType type;

      type = p->update_var.type;
      mem  = p->update_var.id;
      fprintf(f, ":%d:%d", type, mem);
    }
    break;

#ifdef USE_CALLOUT
  case OP_CALLOUT_CONTENTS:
    mem = p->callout_contents.num;
    fprintf(f, ":%d", mem);
    break;

  case OP_CALLOUT_NAME:
    {
      int id;

      id  = p->callout_name.id;
      mem = p->callout_name.num;
      fprintf(f, ":%d:%d", id, mem);
    }
    break;
#endif

  case OP_TEXT_SEGMENT_BOUNDARY:
    if (p->text_segment_boundary.not != 0)
      fprintf(f, ":not");
    break;

  case OP_FINISH:
  case OP_END:
  case OP_ANYCHAR:
  case OP_ANYCHAR_ML:
  case OP_ANYCHAR_STAR:
  case OP_ANYCHAR_ML_STAR:
  case OP_WORD:
  case OP_WORD_ASCII:
  case OP_NO_WORD:
  case OP_NO_WORD_ASCII:
  case OP_BEGIN_BUF:
  case OP_END_BUF:
  case OP_BEGIN_LINE:
  case OP_END_LINE:
  case OP_SEMI_END_BUF:
  case OP_BEGIN_POSITION:
  case OP_BACKREF1:
  case OP_BACKREF2:
  case OP_FAIL:
  case OP_POP_OUT:
  case OP_PREC_READ_START:
  case OP_PREC_READ_END:
  case OP_PREC_READ_NOT_END:
  case OP_ATOMIC_START:
  case OP_ATOMIC_END:
  case OP_LOOK_BEHIND_NOT_END:
#ifdef USE_CALL
  case OP_RETURN:
#endif
    break;

  default:
    fprintf(stderr, "print_compiled_byte_code: undefined code %d\n", opcode);
    break;
  }
}
#endif /* defined(ONIG_DEBUG_COMPILE) || defined(ONIG_DEBUG_MATCH) */

#ifdef ONIG_DEBUG_COMPILE
extern void
onig_print_compiled_byte_code_list(FILE* f, regex_t* reg)
{
  Operation* bp;
  Operation* start = reg->ops;
  Operation* end   = reg->ops + reg->ops_used;

  fprintf(f, "push_mem_start: 0x%x, push_mem_end: 0x%x\n",
          reg->push_mem_start, reg->push_mem_end);
  fprintf(f, "code-length: %d\n", reg->ops_used);

  bp = start;
  while (bp < end) {
    int pos = bp - start;

    fprintf(f, "%4d: ", pos);
    print_compiled_byte_code(f, reg, pos, start, reg->enc);
    fprintf(f, "\n");
    bp++;
  }
  fprintf(f, "\n");
}
#endif


#ifdef USE_CAPTURE_HISTORY
static void history_tree_free(OnigCaptureTreeNode* node);

static void
history_tree_clear(OnigCaptureTreeNode* node)
{
  int i;

  if (IS_NULL(node)) return ;

  for (i = 0; i < node->num_childs; i++) {
    if (IS_NOT_NULL(node->childs[i])) {
      history_tree_free(node->childs[i]);
    }
  }
  for (i = 0; i < node->allocated; i++) {
    node->childs[i] = (OnigCaptureTreeNode* )0;
  }
  node->num_childs = 0;
  node->beg = ONIG_REGION_NOTPOS;
  node->end = ONIG_REGION_NOTPOS;
  node->group = -1;
}

static void
history_tree_free(OnigCaptureTreeNode* node)
{
  history_tree_clear(node);
  if (IS_NOT_NULL(node->childs)) xfree(node->childs);

  xfree(node);
}

static void
history_root_free(OnigRegion* r)
{
  if (IS_NULL(r->history_root)) return ;

  history_tree_free(r->history_root);
  r->history_root = (OnigCaptureTreeNode* )0;
}

static OnigCaptureTreeNode*
history_node_new(void)
{
  OnigCaptureTreeNode* node;

  node = (OnigCaptureTreeNode* )xmalloc(sizeof(OnigCaptureTreeNode));
  CHECK_NULL_RETURN(node);

  node->childs     = (OnigCaptureTreeNode** )0;
  node->allocated  =  0;
  node->num_childs =  0;
  node->group      = -1;
  node->beg        = ONIG_REGION_NOTPOS;
  node->end        = ONIG_REGION_NOTPOS;

  return node;
}

static int
history_tree_add_child(OnigCaptureTreeNode* parent, OnigCaptureTreeNode* child)
{
#define HISTORY_TREE_INIT_ALLOC_SIZE  8

  if (parent->num_childs >= parent->allocated) {
    int n, i;

    if (IS_NULL(parent->childs)) {
      n = HISTORY_TREE_INIT_ALLOC_SIZE;
      parent->childs =
        (OnigCaptureTreeNode** )xmalloc(sizeof(parent->childs[0]) * n);
    }
    else {
      n = parent->allocated * 2;
      parent->childs =
        (OnigCaptureTreeNode** )xrealloc(parent->childs,
                                         sizeof(parent->childs[0]) * n);
    }
    CHECK_NULL_RETURN_MEMERR(parent->childs);
    for (i = parent->allocated; i < n; i++) {
      parent->childs[i] = (OnigCaptureTreeNode* )0;
    }
    parent->allocated = n;
  }

  parent->childs[parent->num_childs] = child;
  parent->num_childs++;
  return 0;
}

static OnigCaptureTreeNode*
history_tree_clone(OnigCaptureTreeNode* node)
{
  int i;
  OnigCaptureTreeNode *clone, *child;

  clone = history_node_new();
  CHECK_NULL_RETURN(clone);

  clone->beg = node->beg;
  clone->end = node->end;
  for (i = 0; i < node->num_childs; i++) {
    child = history_tree_clone(node->childs[i]);
    if (IS_NULL(child)) {
      history_tree_free(clone);
      return (OnigCaptureTreeNode* )0;
    }
    history_tree_add_child(clone, child);
  }

  return clone;
}

extern  OnigCaptureTreeNode*
onig_get_capture_tree(OnigRegion* region)
{
  return region->history_root;
}
#endif /* USE_CAPTURE_HISTORY */

extern void
onig_region_clear(OnigRegion* region)
{
  int i;

  for (i = 0; i < region->num_regs; i++) {
    region->beg[i] = region->end[i] = ONIG_REGION_NOTPOS;
  }
#ifdef USE_CAPTURE_HISTORY
  history_root_free(region);
#endif
}

extern int
onig_region_resize(OnigRegion* region, int n)
{
  region->num_regs = n;

  if (n < ONIG_NREGION)
    n = ONIG_NREGION;

  if (region->allocated == 0) {
    region->beg = (int* )xmalloc(n * sizeof(int));
    region->end = (int* )xmalloc(n * sizeof(int));

    if (region->beg == 0 || region->end == 0)
      return ONIGERR_MEMORY;

    region->allocated = n;
  }
  else if (region->allocated < n) {
    region->beg = (int* )xrealloc(region->beg, n * sizeof(int));
    region->end = (int* )xrealloc(region->end, n * sizeof(int));

    if (region->beg == 0 || region->end == 0)
      return ONIGERR_MEMORY;

    region->allocated = n;
  }

  return 0;
}

static int
onig_region_resize_clear(OnigRegion* region, int n)
{
  int r;

  r = onig_region_resize(region, n);
  if (r != 0) return r;
  onig_region_clear(region);
  return 0;
}

extern int
onig_region_set(OnigRegion* region, int at, int beg, int end)
{
  if (at < 0) return ONIGERR_INVALID_ARGUMENT;

  if (at >= region->allocated) {
    int r = onig_region_resize(region, at + 1);
    if (r < 0) return r;
  }

  region->beg[at] = beg;
  region->end[at] = end;
  return 0;
}

extern void
onig_region_init(OnigRegion* region)
{
  region->num_regs     = 0;
  region->allocated    = 0;
  region->beg          = (int* )0;
  region->end          = (int* )0;
  region->history_root = (OnigCaptureTreeNode* )0;
}

extern OnigRegion*
onig_region_new(void)
{
  OnigRegion* r;

  r = (OnigRegion* )xmalloc(sizeof(OnigRegion));
  CHECK_NULL_RETURN(r);
  onig_region_init(r);
  return r;
}

extern void
onig_region_free(OnigRegion* r, int free_self)
{
  if (r != 0) {
    if (r->allocated > 0) {
      if (r->beg) xfree(r->beg);
      if (r->end) xfree(r->end);
      r->allocated = 0;
    }
#ifdef USE_CAPTURE_HISTORY
    history_root_free(r);
#endif
    if (free_self) xfree(r);
  }
}

extern void
onig_region_copy(OnigRegion* to, OnigRegion* from)
{
#define RREGC_SIZE   (sizeof(int) * from->num_regs)
  int i;

  if (to == from) return;

  if (to->allocated == 0) {
    if (from->num_regs > 0) {
      to->beg = (int* )xmalloc(RREGC_SIZE);
      if (IS_NULL(to->beg)) return;
      to->end = (int* )xmalloc(RREGC_SIZE);
      if (IS_NULL(to->end)) return;
      to->allocated = from->num_regs;
    }
  }
  else if (to->allocated < from->num_regs) {
    to->beg = (int* )xrealloc(to->beg, RREGC_SIZE);
    if (IS_NULL(to->beg)) return;
    to->end = (int* )xrealloc(to->end, RREGC_SIZE);
    if (IS_NULL(to->end)) return;
    to->allocated = from->num_regs;
  }

  for (i = 0; i < from->num_regs; i++) {
    to->beg[i] = from->beg[i];
    to->end[i] = from->end[i];
  }
  to->num_regs = from->num_regs;

#ifdef USE_CAPTURE_HISTORY
  history_root_free(to);

  if (IS_NOT_NULL(from->history_root)) {
    to->history_root = history_tree_clone(from->history_root);
  }
#endif
}

#ifdef USE_CALLOUT
#define CALLOUT_BODY(func, ain, aname_id, anum, user, args, result) do { \
  args.in            = (ain);\
  args.name_id       = (aname_id);\
  args.num           = anum;\
  args.regex         = reg;\
  args.string        = str;\
  args.string_end    = end;\
  args.start         = sstart;\
  args.right_range   = right_range;\
  args.current       = s;\
  args.retry_in_match_counter = retry_in_match_counter;\
  args.msa           = msa;\
  args.stk_base      = stk_base;\
  args.stk           = stk;\
  args.mem_start_stk = mem_start_stk;\
  args.mem_end_stk   = mem_end_stk;\
  result = (func)(&args, user);\
} while (0)

#define RETRACTION_CALLOUT(func, aname_id, anum, user) do {\
  int result;\
  OnigCalloutArgs args;\
  CALLOUT_BODY(func, ONIG_CALLOUT_IN_RETRACTION, aname_id, anum, user, args, result);\
  switch (result) {\
  case ONIG_CALLOUT_FAIL:\
  case ONIG_CALLOUT_SUCCESS:\
    break;\
  default:\
    if (result > 0) {\
      result = ONIGERR_INVALID_ARGUMENT;\
    }\
    best_len = result;\
    goto match_at_end;\
    break;\
  }\
} while(0)
#endif


/** stack **/
#define INVALID_STACK_INDEX   -1

#define STK_ALT_FLAG               0x0001

/* stack type */
/* used by normal-POP */
#define STK_SUPER_ALT             STK_ALT_FLAG
#define STK_ALT                   (0x0002 | STK_ALT_FLAG)
#define STK_ALT_PREC_READ_NOT     (0x0004 | STK_ALT_FLAG)
#define STK_ALT_LOOK_BEHIND_NOT   (0x0006 | STK_ALT_FLAG)

/* handled by normal-POP */
#define STK_MEM_START              0x0010
#define STK_MEM_END                0x8030
#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR
#define STK_REPEAT_INC             (0x0040 | STK_MASK_POP_HANDLED)
#else
#define STK_REPEAT_INC             0x0040
#endif
#ifdef USE_CALLOUT
#define STK_CALLOUT                0x0070
#endif

/* avoided by normal-POP */
#define STK_VOID                   0x0000  /* for fill a blank */
#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR
#define STK_EMPTY_CHECK_START      (0x3000 | STK_MASK_POP_HANDLED)
#else
#define STK_EMPTY_CHECK_START      0x3000
#endif
#define STK_EMPTY_CHECK_END        0x5000  /* for recursive call */
#define STK_MEM_END_MARK           0x8100
#define STK_TO_VOID_START          0x1200  /* mark for "(?>...)" */
/* #define STK_REPEAT                 0x0300 */
#define STK_CALL_FRAME             0x0400
#define STK_RETURN                 0x0500
#define STK_SAVE_VAL               0x0600
#define STK_PREC_READ_START        0x0700
#define STK_PREC_READ_END          0x0800

/* stack type check mask */
#define STK_MASK_POP_USED          STK_ALT_FLAG
#define STK_MASK_POP_HANDLED       0x0010
#define STK_MASK_POP_HANDLED_TIL   (STK_MASK_POP_HANDLED | 0x0004)
#define STK_MASK_TO_VOID_TARGET    0x100e
#define STK_MASK_MEM_END_OR_MARK   0x8000  /* MEM_END or MEM_END_MARK */

typedef intptr_t StackIndex;

typedef struct _StackType {
  unsigned int type;
  int zid;
  union {
    struct {
      Operation* pcode;     /* byte code position */
      UChar*     pstr;      /* string position */
      UChar*     pstr_prev; /* previous char position of pstr */
    } state;
    struct {
      int        count;
#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR
      StackIndex prev_index;  /* index of stack */
#endif
    } repeat_inc;
    struct {
      UChar *pstr;       /* start/end position */
      /* Following information is set, if this stack type is MEM-START */
      StackIndex prev_start;  /* prev. info (for backtrack  "(...)*" ) */
      StackIndex prev_end;    /* prev. info (for backtrack  "(...)*" ) */
    } mem;
    struct {
      UChar *pstr;            /* start position */
#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR
      StackIndex prev_index;  /* index of stack */
#endif
    } empty_check;
#ifdef USE_CALL
    struct {
      Operation *ret_addr; /* byte code position */
      UChar *pstr;         /* string position */
    } call_frame;
#endif
    struct {
      enum SaveType type;
      UChar* v;
      UChar* v2;
    } val;
#ifdef USE_CALLOUT
    struct {
      int num;
      OnigCalloutFunc func;
    } callout;
#endif
  } u;
} StackType;

#ifdef USE_CALLOUT

struct OnigCalloutArgsStruct {
  OnigCalloutIn    in;
  int              name_id;   /* name id or ONIG_NON_NAME_ID */
  int              num;
  OnigRegex        regex;
  const OnigUChar* string;
  const OnigUChar* string_end;
  const OnigUChar* start;
  const OnigUChar* right_range;
  const OnigUChar* current;  /* current matching position */
  unsigned long    retry_in_match_counter;

  /* invisible to users */
  MatchArg*   msa;
  StackType*  stk_base;
  StackType*  stk;
  StackIndex* mem_start_stk;
  StackIndex* mem_end_stk;
};

#endif

#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR

#define PTR_NUM_SIZE(reg)  ((reg)->num_repeat + (reg)->num_empty_check + ((reg)->num_mem + 1) * 2)
#define UPDATE_FOR_STACK_REALLOC do{\
  repeat_stk      = (StackIndex* )alloc_base;\
  empty_check_stk = (StackIndex* )(repeat_stk + reg->num_repeat);\
  mem_start_stk   = (StackIndex* )(empty_check_stk + reg->num_empty_check);\
  mem_end_stk     = mem_start_stk + num_mem + 1;\
} while(0)

#define SAVE_REPEAT_STK_VAR(sid) stk->u.repeat_inc.prev_index = repeat_stk[sid]
#define LOAD_TO_REPEAT_STK_VAR(sid)  repeat_stk[sid] = GET_STACK_INDEX(stk)
#define POP_REPEAT_INC  else if (stk->type == STK_REPEAT_INC) {repeat_stk[stk->zid] = stk->u.repeat_inc.prev_index;}

#define SAVE_EMPTY_CHECK_STK_VAR(sid) stk->u.empty_check.prev_index = empty_check_stk[sid]
#define LOAD_TO_EMPTY_CHECK_STK_VAR(sid)  empty_check_stk[sid] = GET_STACK_INDEX(stk)
#define POP_EMPTY_CHECK_START  else if (stk->type == STK_EMPTY_CHECK_START) {empty_check_stk[stk->zid] = stk->u.empty_check.prev_index;}

#else

#define PTR_NUM_SIZE(reg)  (((reg)->num_mem + 1) * 2)
#define UPDATE_FOR_STACK_REALLOC do{\
  mem_start_stk = (StackIndex* )alloc_base;\
  mem_end_stk   = mem_start_stk + num_mem + 1;\
} while(0)

#define SAVE_REPEAT_STK_VAR(sid)
#define LOAD_TO_REPEAT_STK_VAR(sid)
#define POP_REPEAT_INC

#define SAVE_EMPTY_CHECK_STK_VAR(sid)
#define LOAD_TO_EMPTY_CHECK_STK_VAR(sid)
#define POP_EMPTY_CHECK_START

#endif /* USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR */

#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
#define MATCH_ARG_INIT(msa, reg, arg_option, arg_region, arg_start, mpv) do { \
  (msa).stack_p  = (void* )0;\
  (msa).options  = (arg_option);\
  (msa).region   = (arg_region);\
  (msa).start    = (arg_start);\
  (msa).match_stack_limit  = (mpv)->match_stack_limit;\
  (msa).retry_limit_in_match = (mpv)->retry_limit_in_match;\
  (msa).mp = mpv;\
  (msa).best_len = ONIG_MISMATCH;\
  (msa).ptr_num  = PTR_NUM_SIZE(reg);\
} while(0)
#else
#define MATCH_ARG_INIT(msa, reg, arg_option, arg_region, arg_start, mpv) do { \
  (msa).stack_p  = (void* )0;\
  (msa).options  = (arg_option);\
  (msa).region   = (arg_region);\
  (msa).start    = (arg_start);\
  (msa).match_stack_limit  = (mpv)->match_stack_limit;\
  (msa).retry_limit_in_match = (mpv)->retry_limit_in_match;\
  (msa).mp = mpv;\
  (msa).ptr_num  = PTR_NUM_SIZE(reg);\
} while(0)
#endif

#define MATCH_ARG_FREE(msa)  if ((msa).stack_p) xfree((msa).stack_p)


#define ALLOCA_PTR_NUM_LIMIT   50

#define STACK_INIT(stack_num)  do {\
  if (msa->stack_p) {\
    is_alloca  = 0;\
    alloc_base = msa->stack_p;\
    stk_base   = (StackType* )(alloc_base\
                 + (sizeof(StackIndex) * msa->ptr_num));\
    stk        = stk_base;\
    stk_end    = stk_base + msa->stack_n;\
  }\
  else if (msa->ptr_num > ALLOCA_PTR_NUM_LIMIT) {\
    is_alloca  = 0;\
    alloc_base = (char* )xmalloc(sizeof(StackIndex) * msa->ptr_num\
                  + sizeof(StackType) * (stack_num));\
    CHECK_NULL_RETURN_MEMERR(alloc_base);\
    stk_base   = (StackType* )(alloc_base\
                 + (sizeof(StackIndex) * msa->ptr_num));\
    stk        = stk_base;\
    stk_end    = stk_base + (stack_num);\
  }\
  else {\
    is_alloca  = 1;\
    alloc_base = (char* )xalloca(sizeof(StackIndex) * msa->ptr_num\
                 + sizeof(StackType) * (stack_num));\
    CHECK_NULL_RETURN_MEMERR(alloc_base);\
    stk_base   = (StackType* )(alloc_base\
                 + (sizeof(StackIndex) * msa->ptr_num));\
    stk        = stk_base;\
    stk_end    = stk_base + (stack_num);\
  }\
} while(0);


#define STACK_SAVE do{\
    msa->stack_n = (int )(stk_end - stk_base);\
  if (is_alloca != 0) {\
    size_t size = sizeof(StackIndex) * msa->ptr_num \
                + sizeof(StackType) * msa->stack_n;\
    msa->stack_p = xmalloc(size);\
    CHECK_NULL_RETURN_MEMERR(msa->stack_p);\
    xmemcpy(msa->stack_p, alloc_base, size);\
  }\
  else {\
    msa->stack_p = alloc_base;\
  };\
} while(0)

static unsigned int MatchStackLimit = DEFAULT_MATCH_STACK_LIMIT_SIZE;

extern unsigned int
onig_get_match_stack_limit_size(void)
{
  return MatchStackLimit;
}

extern int
onig_set_match_stack_limit_size(unsigned int size)
{
  MatchStackLimit = size;
  return 0;
}

#ifdef USE_RETRY_LIMIT_IN_MATCH

static unsigned long RetryLimitInMatch = DEFAULT_RETRY_LIMIT_IN_MATCH;

#define CHECK_RETRY_LIMIT_IN_MATCH  do {\
  if (retry_in_match_counter++ > retry_limit_in_match) {\
    MATCH_AT_ERROR_RETURN(ONIGERR_RETRY_LIMIT_IN_MATCH_OVER);\
  }\
} while (0)

#else

#define CHECK_RETRY_LIMIT_IN_MATCH

#endif /* USE_RETRY_LIMIT_IN_MATCH */

extern unsigned long
onig_get_retry_limit_in_match(void)
{
#ifdef USE_RETRY_LIMIT_IN_MATCH
  return RetryLimitInMatch;
#else
  /* return ONIG_NO_SUPPORT_CONFIG; */
  return 0;
#endif
}

extern int
onig_set_retry_limit_in_match(unsigned long size)
{
#ifdef USE_RETRY_LIMIT_IN_MATCH
  RetryLimitInMatch = size;
  return 0;
#else
  return ONIG_NO_SUPPORT_CONFIG;
#endif
}

#ifdef USE_CALLOUT
static OnigCalloutFunc DefaultProgressCallout;
static OnigCalloutFunc DefaultRetractionCallout;
#endif

extern OnigMatchParam*
onig_new_match_param(void)
{
  OnigMatchParam* p;

  p = (OnigMatchParam* )xmalloc(sizeof(*p));
  if (IS_NOT_NULL(p)) {
    onig_initialize_match_param(p);
  }

  return p;
}

extern void
onig_free_match_param_content(OnigMatchParam* p)
{
#ifdef USE_CALLOUT
  if (IS_NOT_NULL(p->callout_data)) {
    xfree(p->callout_data);
    p->callout_data = 0;
  }
#endif
}

extern void
onig_free_match_param(OnigMatchParam* p)
{
  if (IS_NOT_NULL(p)) {
    onig_free_match_param_content(p);
    xfree(p);
  }
}

extern int
onig_initialize_match_param(OnigMatchParam* mp)
{
  mp->match_stack_limit  = MatchStackLimit;
#ifdef USE_RETRY_LIMIT_IN_MATCH
  mp->retry_limit_in_match = RetryLimitInMatch;
#endif

#ifdef USE_CALLOUT
  mp->progress_callout_of_contents   = DefaultProgressCallout;
  mp->retraction_callout_of_contents = DefaultRetractionCallout;
  mp->match_at_call_counter  = 0;
  mp->callout_user_data      = 0;
  mp->callout_data           = 0;
  mp->callout_data_alloc_num = 0;
#endif

  return ONIG_NORMAL;
}

#ifdef USE_CALLOUT

static int
adjust_match_param(regex_t* reg, OnigMatchParam* mp)
{
  RegexExt* ext = reg->extp;

  mp->match_at_call_counter = 0;

  if (IS_NULL(ext) || ext->callout_num == 0) return ONIG_NORMAL;

  if (ext->callout_num > mp->callout_data_alloc_num) {
    CalloutData* d;
    size_t n = ext->callout_num * sizeof(*d);
    if (IS_NOT_NULL(mp->callout_data))
      d = (CalloutData* )xrealloc(mp->callout_data, n);
    else
      d = (CalloutData* )xmalloc(n);
    CHECK_NULL_RETURN_MEMERR(d);

    mp->callout_data = d;
    mp->callout_data_alloc_num = ext->callout_num;
  }

  xmemset(mp->callout_data, 0, mp->callout_data_alloc_num * sizeof(CalloutData));
  return ONIG_NORMAL;
}

#define ADJUST_MATCH_PARAM(reg, mp) \
  r = adjust_match_param(reg, mp);\
  if (r != ONIG_NORMAL) return r;

#define CALLOUT_DATA_AT_NUM(mp, num)  ((mp)->callout_data + ((num) - 1))

extern int
onig_check_callout_data_and_clear_old_values(OnigCalloutArgs* args)
{
  OnigMatchParam* mp;
  int num;
  CalloutData* d;

  mp  = args->msa->mp;
  num = args->num;

  d = CALLOUT_DATA_AT_NUM(mp, num);
  if (d->last_match_at_call_counter != mp->match_at_call_counter) {
    xmemset(d, 0, sizeof(*d));
    d->last_match_at_call_counter = mp->match_at_call_counter;
    return d->last_match_at_call_counter;
  }

  return 0;
}

extern int
onig_get_callout_data_dont_clear_old(regex_t* reg, OnigMatchParam* mp,
                                     int callout_num, int slot,
                                     OnigType* type, OnigValue* val)
{
  OnigType t;
  CalloutData* d;

  if (callout_num <= 0) return ONIGERR_INVALID_ARGUMENT;

  d = CALLOUT_DATA_AT_NUM(mp, callout_num);
  t = d->slot[slot].type;
  if (IS_NOT_NULL(type)) *type = t;
  if (IS_NOT_NULL(val))  *val  = d->slot[slot].val;
  return (t == ONIG_TYPE_VOID ? 1 : ONIG_NORMAL);
}

extern int
onig_get_callout_data_by_callout_args_self_dont_clear_old(OnigCalloutArgs* args,
                                                          int slot, OnigType* type,
                                                          OnigValue* val)
{
  return onig_get_callout_data_dont_clear_old(args->regex, args->msa->mp,
                                              args->num, slot, type, val);
}

extern int
onig_get_callout_data(regex_t* reg, OnigMatchParam* mp,
                      int callout_num, int slot,
                      OnigType* type, OnigValue* val)
{
  OnigType t;
  CalloutData* d;

  if (callout_num <= 0) return ONIGERR_INVALID_ARGUMENT;

  d = CALLOUT_DATA_AT_NUM(mp, callout_num);
  if (d->last_match_at_call_counter != mp->match_at_call_counter) {
    xmemset(d, 0, sizeof(*d));
    d->last_match_at_call_counter = mp->match_at_call_counter;
  }

  t = d->slot[slot].type;
  if (IS_NOT_NULL(type)) *type = t;
  if (IS_NOT_NULL(val))  *val  = d->slot[slot].val;
  return (t == ONIG_TYPE_VOID ? 1 : ONIG_NORMAL);
}

extern int
onig_get_callout_data_by_tag(regex_t* reg, OnigMatchParam* mp,
                             const UChar* tag, const UChar* tag_end, int slot,
                             OnigType* type, OnigValue* val)
{
  int num;

  num = onig_get_callout_num_by_tag(reg, tag, tag_end);
  if (num < 0)  return num;
  if (num == 0) return ONIGERR_INVALID_CALLOUT_TAG_NAME;

  return onig_get_callout_data(reg, mp, num, slot, type, val);
}

extern int
onig_get_callout_data_by_callout_args(OnigCalloutArgs* args,
                                      int callout_num, int slot,
                                      OnigType* type, OnigValue* val)
{
  return onig_get_callout_data(args->regex, args->msa->mp, callout_num, slot,
                               type, val);
}

extern int
onig_get_callout_data_by_callout_args_self(OnigCalloutArgs* args,
                                           int slot, OnigType* type, OnigValue* val)
{
  return onig_get_callout_data(args->regex, args->msa->mp, args->num, slot,
                               type, val);
}

extern int
onig_set_callout_data(regex_t* reg, OnigMatchParam* mp,
                      int callout_num, int slot,
                      OnigType type, OnigValue* val)
{
  CalloutData* d;

  if (callout_num <= 0) return ONIGERR_INVALID_ARGUMENT;

  d = CALLOUT_DATA_AT_NUM(mp, callout_num);
  d->slot[slot].type = type;
  d->slot[slot].val  = *val;
  d->last_match_at_call_counter = mp->match_at_call_counter;

  return ONIG_NORMAL;
}

extern int
onig_set_callout_data_by_tag(regex_t* reg, OnigMatchParam* mp,
                             const UChar* tag, const UChar* tag_end, int slot,
                             OnigType type, OnigValue* val)
{
  int num;

  num = onig_get_callout_num_by_tag(reg, tag, tag_end);
  if (num < 0)  return num;
  if (num == 0) return ONIGERR_INVALID_CALLOUT_TAG_NAME;

  return onig_set_callout_data(reg, mp, num, slot, type, val);
}

extern int
onig_set_callout_data_by_callout_args(OnigCalloutArgs* args,
                                      int callout_num, int slot,
                                      OnigType type, OnigValue* val)
{
  return onig_set_callout_data(args->regex, args->msa->mp, callout_num, slot,
                               type, val);
}

extern int
onig_set_callout_data_by_callout_args_self(OnigCalloutArgs* args,
                                           int slot, OnigType type, OnigValue* val)
{
  return onig_set_callout_data(args->regex, args->msa->mp, args->num, slot,
                               type, val);
}

#else
#define ADJUST_MATCH_PARAM(reg, mp)
#endif /* USE_CALLOUT */


static int
stack_double(int is_alloca, char** arg_alloc_base,
             StackType** arg_stk_base, StackType** arg_stk_end, StackType** arg_stk,
             MatchArg* msa)
{
  unsigned int n;
  int used;
  size_t size;
  size_t new_size;
  char* alloc_base;
  char* new_alloc_base;
  StackType *stk_base, *stk_end, *stk;

  alloc_base = *arg_alloc_base;
  stk_base = *arg_stk_base;
  stk_end  = *arg_stk_end;
  stk      = *arg_stk;

  n = (unsigned int )(stk_end - stk_base);
  size = sizeof(StackIndex) * msa->ptr_num + sizeof(StackType) * n;
  n *= 2;
  new_size = sizeof(StackIndex) * msa->ptr_num + sizeof(StackType) * n;
  if (is_alloca != 0) {
    new_alloc_base = (char* )xmalloc(new_size);
    if (IS_NULL(new_alloc_base)) {
      STACK_SAVE;
      return ONIGERR_MEMORY;
    }
    xmemcpy(new_alloc_base, alloc_base, size);
  }
  else {
    if (msa->match_stack_limit != 0 && n > msa->match_stack_limit) {
      if ((unsigned int )(stk_end - stk_base) == msa->match_stack_limit)
        return ONIGERR_MATCH_STACK_LIMIT_OVER;
      else
        n = msa->match_stack_limit;
    }
    new_alloc_base = (char* )xrealloc(alloc_base, new_size);
    if (IS_NULL(new_alloc_base)) {
      STACK_SAVE;
      return ONIGERR_MEMORY;
    }
  }

  alloc_base = new_alloc_base;
  used = (int )(stk - stk_base);
  *arg_alloc_base = alloc_base;
  *arg_stk_base   = (StackType* )(alloc_base
                                  + (sizeof(StackIndex) * msa->ptr_num));
  *arg_stk      = *arg_stk_base + used;
  *arg_stk_end  = *arg_stk_base + n;
  return 0;
}

#define STACK_ENSURE(n) do {\
    if ((int )(stk_end - stk) < (n)) {\
    int r = stack_double(is_alloca, &alloc_base, &stk_base, &stk_end, &stk, msa);\
    if (r != 0) { STACK_SAVE; return r; } \
    is_alloca = 0;\
    UPDATE_FOR_STACK_REALLOC;\
  }\
} while(0)

#define STACK_AT(index)        (stk_base + (index))
#define GET_STACK_INDEX(stk)   ((stk) - stk_base)

#define STACK_PUSH_TYPE(stack_type) do {\
  STACK_ENSURE(1);\
  stk->type = (stack_type);\
  STACK_INC;\
} while(0)

#define IS_TO_VOID_TARGET(stk) (((stk)->type & STK_MASK_TO_VOID_TARGET) != 0)

#define STACK_PUSH(stack_type,pat,s,sprev) do {\
  STACK_ENSURE(1);\
  stk->type = (stack_type);\
  stk->u.state.pcode     = (pat);\
  stk->u.state.pstr      = (s);\
  stk->u.state.pstr_prev = (sprev);\
  STACK_INC;\
} while(0)

#define STACK_PUSH_ENSURED(stack_type,pat) do {\
  stk->type = (stack_type);\
  stk->u.state.pcode = (pat);\
  STACK_INC;\
} while(0)

#ifdef ONIG_DEBUG_MATCH
#define STACK_PUSH_BOTTOM(stack_type,pat) do {\
  stk->type = (stack_type);\
  stk->u.state.pcode = (pat);\
  stk->u.state.pstr      = s;\
  stk->u.state.pstr_prev = sprev;\
  STACK_INC;\
} while (0)
#else
#define STACK_PUSH_BOTTOM(stack_type,pat) do {\
  stk->type = (stack_type);\
  stk->u.state.pcode = (pat);\
  STACK_INC;\
} while (0)
#endif

#define STACK_PUSH_ALT(pat,s,sprev)       STACK_PUSH(STK_ALT,pat,s,sprev)
#define STACK_PUSH_SUPER_ALT(pat,s,sprev) STACK_PUSH(STK_SUPER_ALT,pat,s,sprev)
#define STACK_PUSH_PREC_READ_START(s,sprev) \
  STACK_PUSH(STK_PREC_READ_START,(Operation* )0,s,sprev)
#define STACK_PUSH_ALT_PREC_READ_NOT(pat,s,sprev) \
  STACK_PUSH(STK_ALT_PREC_READ_NOT,pat,s,sprev)
#define STACK_PUSH_TO_VOID_START        STACK_PUSH_TYPE(STK_TO_VOID_START)
#define STACK_PUSH_ALT_LOOK_BEHIND_NOT(pat,s,sprev) \
  STACK_PUSH(STK_ALT_LOOK_BEHIND_NOT,pat,s,sprev)

#if 0
#define STACK_PUSH_REPEAT(sid, pat) do {\
  STACK_ENSURE(1);\
  stk->type = STK_REPEAT;\
  stk->zid  = (sid);\
  stk->u.repeat.pcode = (pat);\
  STACK_INC;\
} while(0)
#endif

#define STACK_PUSH_REPEAT_INC(sid, ct) do {\
  STACK_ENSURE(1);\
  stk->type = STK_REPEAT_INC;\
  stk->zid  = (sid);\
  stk->u.repeat_inc.count = (ct);\
  SAVE_REPEAT_STK_VAR(sid);\
  LOAD_TO_REPEAT_STK_VAR(sid);\
  STACK_INC;\
} while(0)

#define STACK_PUSH_MEM_START(mnum, s) do {\
  STACK_ENSURE(1);\
  stk->type = STK_MEM_START;\
  stk->zid  = (mnum);\
  stk->u.mem.pstr       = (s);\
  stk->u.mem.prev_start = mem_start_stk[mnum];\
  stk->u.mem.prev_end   = mem_end_stk[mnum];\
  mem_start_stk[mnum]   = GET_STACK_INDEX(stk);\
  mem_end_stk[mnum]     = INVALID_STACK_INDEX;\
  STACK_INC;\
} while(0)

#define STACK_PUSH_MEM_END(mnum, s) do {\
  STACK_ENSURE(1);\
  stk->type = STK_MEM_END;\
  stk->zid  = (mnum);\
  stk->u.mem.pstr       = (s);\
  stk->u.mem.prev_start = mem_start_stk[mnum];\
  stk->u.mem.prev_end   = mem_end_stk[mnum];\
  mem_end_stk[mnum] = GET_STACK_INDEX(stk);\
  STACK_INC;\
} while(0)

#define STACK_PUSH_MEM_END_MARK(mnum) do {\
  STACK_ENSURE(1);\
  stk->type = STK_MEM_END_MARK;\
  stk->zid  = (mnum);\
  STACK_INC;\
} while(0)

#define STACK_GET_MEM_START(mnum, k) do {\
  int level = 0;\
  k = stk;\
  while (k > stk_base) {\
    k--;\
    if ((k->type & STK_MASK_MEM_END_OR_MARK) != 0 \
      && k->zid == (mnum)) {\
      level++;\
    }\
    else if (k->type == STK_MEM_START && k->zid == (mnum)) {\
      if (level == 0) break;\
      level--;\
    }\
  }\
} while(0)

#define STACK_GET_MEM_RANGE(k, mnum, start, end) do {\
  int level = 0;\
  while (k < stk) {\
    if (k->type == STK_MEM_START && k->u.mem.num == (mnum)) {\
      if (level == 0) (start) = k->u.mem.pstr;\
      level++;\
    }\
    else if (k->type == STK_MEM_END && k->u.mem.num == (mnum)) {\
      level--;\
      if (level == 0) {\
        (end) = k->u.mem.pstr;\
        break;\
      }\
    }\
    k++;\
  }\
} while(0)

#define STACK_PUSH_EMPTY_CHECK_START(cnum, s) do {\
  STACK_ENSURE(1);\
  stk->type = STK_EMPTY_CHECK_START;\
  stk->zid  = (cnum);\
  stk->u.empty_check.pstr = (s);\
  SAVE_EMPTY_CHECK_STK_VAR(cnum);\
  LOAD_TO_EMPTY_CHECK_STK_VAR(cnum);\
  STACK_INC;\
} while(0)

#define STACK_PUSH_EMPTY_CHECK_END(cnum) do {\
  STACK_ENSURE(1);\
  stk->type = STK_EMPTY_CHECK_END;\
  stk->zid  = (cnum);\
  STACK_INC;\
} while(0)

#define STACK_PUSH_CALL_FRAME(pat) do {\
  STACK_ENSURE(1);\
  stk->type = STK_CALL_FRAME;\
  stk->u.call_frame.ret_addr = (pat);\
  STACK_INC;\
} while(0)

#define STACK_PUSH_RETURN do {\
  STACK_ENSURE(1);\
  stk->type = STK_RETURN;\
  STACK_INC;\
} while(0)

#define STACK_PUSH_SAVE_VAL(sid, stype, sval) do {\
  STACK_ENSURE(1);\
  stk->type = STK_SAVE_VAL;\
  stk->zid  = (sid);\
  stk->u.val.type = (stype);\
  stk->u.val.v    = (UChar* )(sval);\
  STACK_INC;\
} while(0)

#define STACK_PUSH_SAVE_VAL_WITH_SPREV(sid, stype, sval) do {\
  STACK_ENSURE(1);\
  stk->type = STK_SAVE_VAL;\
  stk->zid  = (sid);\
  stk->u.val.type = (stype);\
  stk->u.val.v    = (UChar* )(sval);\
  stk->u.val.v2   = sprev;\
  STACK_INC;\
} while(0)

#define STACK_GET_SAVE_VAL_TYPE_LAST(stype, sval) do {\
  StackType *k = stk;\
  while (k > stk_base) {\
    k--;\
    STACK_BASE_CHECK(k, "STACK_GET_SAVE_VAL_TYPE_LAST"); \
    if (k->type == STK_SAVE_VAL && k->u.val.type == (stype)) {\
      (sval) = k->u.val.v;\
      break;\
    }\
  }\
} while (0)

#define STACK_GET_SAVE_VAL_TYPE_LAST_ID(stype, sid, sval) do { \
  int level = 0;\
  StackType *k = stk;\
  while (k > stk_base) {\
    k--;\
    STACK_BASE_CHECK(k, "STACK_GET_SAVE_VAL_TYPE_LAST_ID"); \
    if (k->type == STK_SAVE_VAL && k->u.val.type == (stype)\
        && k->zid == (sid)) {\
      if (level == 0) {\
        (sval) = k->u.val.v;\
        break;\
      }\
    }\
    else if (k->type == STK_CALL_FRAME)\
      level--;\
    else if (k->type == STK_RETURN)\
      level++;\
  }\
} while (0)

#define STACK_GET_SAVE_VAL_TYPE_LAST_ID_WITH_SPREV(stype, sid, sval) do { \
  int level = 0;\
  StackType *k = stk;\
  while (k > stk_base) {\
    k--;\
    STACK_BASE_CHECK(k, "STACK_GET_SAVE_VAL_TYPE_LAST_ID"); \
    if (k->type == STK_SAVE_VAL && k->u.val.type == (stype)\
        && k->zid == (sid)) {\
      if (level == 0) {\
        (sval) = k->u.val.v;\
        sprev  = k->u.val.v2;\
        break;\
      }\
    }\
    else if (k->type == STK_CALL_FRAME)\
      level--;\
    else if (k->type == STK_RETURN)\
      level++;\
  }\
} while (0)

#define STACK_GET_SAVE_VAL_TYPE_LAST_ID_FROM(stype, sid, sval, stk_from) do { \
  int level = 0;\
  StackType *k = (stk_from);\
  while (k > stk_base) {\
    STACK_BASE_CHECK(k, "STACK_GET_SAVE_VAL_TYPE_LAST_ID_FROM"); \
    if (k->type == STK_SAVE_VAL && k->u.val.type == (stype)\
        && k->u.val.id == (sid)) {\
      if (level == 0) {\
        (sval) = k->u.val.v;\
        break;\
      }\
    }\
    else if (k->type == STK_CALL_FRAME)\
      level--;\
    else if (k->type == STK_RETURN)\
      level++;\
    k--;\
  }\
} while (0)

#define STACK_PUSH_CALLOUT_CONTENTS(anum, func) do {\
  STACK_ENSURE(1);\
  stk->type = STK_CALLOUT;\
  stk->zid  = ONIG_NON_NAME_ID;\
  stk->u.callout.num = (anum);\
  stk->u.callout.func = (func);\
  STACK_INC;\
} while(0)

#define STACK_PUSH_CALLOUT_NAME(aid, anum, func) do {\
  STACK_ENSURE(1);\
  stk->type = STK_CALLOUT;\
  stk->zid  = (aid);\
  stk->u.callout.num = (anum);\
  stk->u.callout.func = (func);\
  STACK_INC;\
} while(0)

#ifdef ONIG_DEBUG
#define STACK_BASE_CHECK(p, at) \
  if ((p) < stk_base) {\
    fprintf(stderr, "at %s\n", at);\
    MATCH_AT_ERROR_RETURN(ONIGERR_STACK_BUG);\
  }
#else
#define STACK_BASE_CHECK(p, at)
#endif

#define STACK_POP_ONE do {\
  stk--;\
  STACK_BASE_CHECK(stk, "STACK_POP_ONE"); \
} while(0)


#ifdef USE_CALLOUT
#define POP_CALLOUT_CASE \
  else if (stk->type == STK_CALLOUT) {\
    RETRACTION_CALLOUT(stk->u.callout.func, stk->zid, stk->u.callout.num, msa->mp->callout_user_data);\
  }
#else
#define POP_CALLOUT_CASE
#endif

#define STACK_POP  do {\
  switch (pop_level) {\
  case STACK_POP_LEVEL_FREE:\
    while (1) {\
      stk--;\
      STACK_BASE_CHECK(stk, "STACK_POP"); \
      if ((stk->type & STK_MASK_POP_USED) != 0)  break;\
    }\
    break;\
  case STACK_POP_LEVEL_MEM_START:\
    while (1) {\
      stk--;\
      STACK_BASE_CHECK(stk, "STACK_POP 2"); \
      if ((stk->type & STK_MASK_POP_USED) != 0)  break;\
      else if (stk->type == STK_MEM_START) {\
        mem_start_stk[stk->zid] = stk->u.mem.prev_start;\
        mem_end_stk[stk->zid]   = stk->u.mem.prev_end;\
      }\
    }\
    break;\
  default:\
    while (1) {\
      stk--;\
      STACK_BASE_CHECK(stk, "STACK_POP 3"); \
      if ((stk->type & STK_MASK_POP_USED) != 0)  break;\
      else if ((stk->type & STK_MASK_POP_HANDLED) != 0) {\
        if (stk->type == STK_MEM_START) {\
          mem_start_stk[stk->zid] = stk->u.mem.prev_start;\
          mem_end_stk[stk->zid]   = stk->u.mem.prev_end;\
        }\
        else if (stk->type == STK_MEM_END) {\
          mem_start_stk[stk->zid] = stk->u.mem.prev_start;\
          mem_end_stk[stk->zid]   = stk->u.mem.prev_end;\
        }\
        POP_REPEAT_INC \
        POP_EMPTY_CHECK_START \
        POP_CALLOUT_CASE\
      }\
    }\
    break;\
  }\
} while(0)

#define POP_TIL_BODY(aname, til_type) do {\
  while (1) {\
    stk--;\
    STACK_BASE_CHECK(stk, (aname));\
    if ((stk->type & STK_MASK_POP_HANDLED_TIL) != 0) {\
      if (stk->type == (til_type)) break;\
      else {\
        if (stk->type == STK_MEM_START) {\
          mem_start_stk[stk->zid] = stk->u.mem.prev_start;\
          mem_end_stk[stk->zid]   = stk->u.mem.prev_end;\
        }\
        else if (stk->type == STK_MEM_END) {\
          mem_start_stk[stk->zid] = stk->u.mem.prev_start;\
          mem_end_stk[stk->zid]   = stk->u.mem.prev_end;\
        }\
        POP_REPEAT_INC \
        POP_EMPTY_CHECK_START \
        /* Don't call callout here because negation of total success by (?!..) (?<!..) */\
      }\
    }\
  }\
} while(0)

#define STACK_POP_TIL_ALT_PREC_READ_NOT  do {\
  POP_TIL_BODY("STACK_POP_TIL_ALT_PREC_READ_NOT", STK_ALT_PREC_READ_NOT);\
} while(0)

#define STACK_POP_TIL_ALT_LOOK_BEHIND_NOT  do {\
  POP_TIL_BODY("STACK_POP_TIL_ALT_LOOK_BEHIND_NOT", STK_ALT_LOOK_BEHIND_NOT);\
} while(0)


#define STACK_EXEC_TO_VOID(k) do {\
  k = stk;\
  while (1) {\
    k--;\
    STACK_BASE_CHECK(k, "STACK_EXEC_TO_VOID"); \
    if (IS_TO_VOID_TARGET(k)) {\
      if (k->type == STK_TO_VOID_START) {\
        k->type = STK_VOID;\
        break;\
      }\
      k->type = STK_VOID;\
    }\
  }\
} while(0)

#define STACK_GET_PREC_READ_START(k) do {\
  int level = 0;\
  k = stk;\
  while (1) {\
    k--;\
    STACK_BASE_CHECK(k, "STACK_GET_PREC_READ_START");\
    if (IS_TO_VOID_TARGET(k)) {\
      k->type = STK_VOID;\
    }\
    else if (k->type == STK_PREC_READ_START) {\
      if (level == 0) {\
        break;\
      }\
      level--;\
    }\
    else if (k->type == STK_PREC_READ_END) {\
      level++;\
    }\
  }\
} while(0)


#define EMPTY_CHECK_START_SEARCH(sid, k) do {\
  k = stk;\
  while (1) {\
    k--;\
    STACK_BASE_CHECK(k, "EMPTY_CHECK_START_SEARCH"); \
    if (k->type == STK_EMPTY_CHECK_START) {\
      if (k->zid == (sid)) break;\
    }\
  }\
} while(0)

#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR

#define GET_EMPTY_CHECK_START(sid, k) do {\
  if (reg->num_call == 0) {\
    k = STACK_AT(empty_check_stk[sid]);\
  }\
  else {\
    EMPTY_CHECK_START_SEARCH(sid, k);\
  }\
} while(0)
#else

#define GET_EMPTY_CHECK_START(sid, k)  EMPTY_CHECK_START_SEARCH(sid, k)

#endif


#define STACK_EMPTY_CHECK(isnull, sid, s) do {\
  StackType* k;\
  GET_EMPTY_CHECK_START(sid, k);\
  (isnull) = (k->u.empty_check.pstr == (s));\
} while(0)

#define STACK_MEM_START_GET_PREV_END_ADDR(k /* STK_MEM_START*/, reg, addr) do {\
  if (k->u.mem.prev_end == INVALID_STACK_INDEX) {\
    (addr) = 0;\
  }\
  else {\
    if (MEM_STATUS_AT((reg)->push_mem_end, k->zid))\
      (addr) = STACK_AT(k->u.mem.prev_end)->u.mem.pstr;\
    else\
      (addr) = (UChar* )k->u.mem.prev_end;\
  }\
} while (0)

#ifdef USE_STUBBORN_CHECK_CAPTURES_IN_EMPTY_REPEAT
#define STACK_EMPTY_CHECK_MEM(isnull, sid, s, reg) do {\
  StackType* k;\
  GET_EMPTY_CHECK_START(sid, k);\
  if (k->u.empty_check.pstr != (s)) {\
    (isnull) = 0;\
  }\
  else {\
    UChar* endp;\
    (isnull) = 1;\
    while (k < stk) {\
      if (k->type == STK_MEM_START &&\
        MEM_STATUS_LIMIT_AT((reg)->empty_status_mem, k->zid)) {\
        STACK_MEM_START_GET_PREV_END_ADDR(k, reg, endp);\
        if (endp == 0) {\
          (isnull) = 0; break;\
        }\
        else if (STACK_AT(k->u.mem.prev_start)->u.mem.pstr != endp) {\
          (isnull) = 0; break;\
        }\
        else if (endp != s) {\
          (isnull) = -1; /* empty, but position changed */ \
        }\
      }\
      k++;\
    }\
  }\
} while(0)

#define STACK_EMPTY_CHECK_MEM_REC(isnull,sid,s,reg) do {\
  int level = 0;\
  StackType* k = stk;\
  while (1) {\
    k--;\
    STACK_BASE_CHECK(k, "STACK_EMPTY_CHECK_MEM_REC");\
    if (k->type == STK_EMPTY_CHECK_START) {\
      if (k->zid == (sid)) {\
        if (level == 0) {\
          if (k->u.empty_check.pstr != (s)) {\
            (isnull) = 0;\
            break;\
          }\
          else {\
            UChar* endp;\
            (isnull) = 1;\
            while (k < stk) {\
              if (k->type == STK_MEM_START) {\
                if (level == 0 && \
                  MEM_STATUS_LIMIT_AT((reg)->empty_status_mem, k->zid) !=0) {\
                  STACK_MEM_START_GET_PREV_END_ADDR(k, reg, endp);\
                  if (endp == 0) {\
                    (isnull) = 0; break;\
                  }\
                  else if (STACK_AT(k->u.mem.prev_start)->u.mem.pstr != endp) { \
                    (isnull) = 0; break;\
                  }\
                  else if (endp != s) {\
                    (isnull) = -1; /* empty, but position changed */\
                  }\
                }\
              }\
              else if (k->type == STK_EMPTY_CHECK_START) {\
                if (k->zid == (sid)) level++;\
              }\
              else if (k->type == STK_EMPTY_CHECK_END) {\
                if (k->zid == (sid)) level--;\
              }\
              k++;\
            }\
            break;\
          }\
        }\
        else {\
          level--;\
        }\
      }\
    }\
    else if (k->type == STK_EMPTY_CHECK_END) {\
      if (k->zid == (sid)) level++;\
    }\
  }\
} while(0)
#else
#define STACK_EMPTY_CHECK_REC(isnull,id,s) do {\
  int level = 0;\
  StackType* k = stk;\
  while (1) {\
    k--;\
    STACK_BASE_CHECK(k, "STACK_EMPTY_CHECK_REC"); \
    if (k->type == STK_EMPTY_CHECK_START) {\
      if (k->u.empty_check.num == (id)) {\
        if (level == 0) {\
          (isnull) = (k->u.empty_check.pstr == (s));\
          break;\
        }\
      }\
      level--;\
    }\
    else if (k->type == STK_EMPTY_CHECK_END) {\
      level++;\
    }\
  }\
} while(0)
#endif /* USE_STUBBORN_CHECK_CAPTURES_IN_EMPTY_REPEAT */

#define STACK_GET_REPEAT_COUNT_SEARCH(sid, c) do {\
  StackType* k = stk;\
  while (1) {\
    (k)--;\
    STACK_BASE_CHECK(k, "STACK_GET_REPEAT_COUNT_SEARCH");\
    if ((k)->type == STK_REPEAT_INC) {\
      if ((k)->zid == (sid)) {\
        (c) = (k)->u.repeat_inc.count;\
        break;\
      }\
    }\
    else if ((k)->type == STK_RETURN) {\
      int level = -1;\
      while (1) {\
        (k)--;\
        if ((k)->type == STK_CALL_FRAME) {\
          level++;\
          if (level == 0) break;\
        }\
        else if ((k)->type == STK_RETURN) level--;\
      }\
    }\
  }\
} while(0)

#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR

#define STACK_GET_REPEAT_COUNT(sid, c) do {\
  if (reg->num_call == 0) {\
    (c) = (STACK_AT(repeat_stk[sid]))->u.repeat_inc.count;\
  }\
  else {\
    STACK_GET_REPEAT_COUNT_SEARCH(sid, c);\
  }\
} while(0)
#else
#define STACK_GET_REPEAT_COUNT(sid, c) STACK_GET_REPEAT_COUNT_SEARCH(sid, c)
#endif

#define STACK_RETURN(addr)  do {\
  int level = 0;\
  StackType* k = stk;\
  while (1) {\
    k--;\
    STACK_BASE_CHECK(k, "STACK_RETURN"); \
    if (k->type == STK_CALL_FRAME) {\
      if (level == 0) {\
        (addr) = k->u.call_frame.ret_addr;\
        break;\
      }\
      else level--;\
    }\
    else if (k->type == STK_RETURN)\
      level++;\
  }\
} while(0)


#define STRING_CMP(s1,s2,len) do {\
  while (len-- > 0) {\
    if (*s1++ != *s2++) goto fail;\
  }\
} while(0)

#define STRING_CMP_IC(case_fold_flag,s1,ps2,len) do {\
  if (string_cmp_ic(encode, case_fold_flag, s1, ps2, len) == 0) \
    goto fail; \
} while(0)

static int string_cmp_ic(OnigEncoding enc, int case_fold_flag,
                         UChar* s1, UChar** ps2, int mblen)
{
  UChar buf1[ONIGENC_MBC_CASE_FOLD_MAXLEN];
  UChar buf2[ONIGENC_MBC_CASE_FOLD_MAXLEN];
  UChar *p1, *p2, *end1, *s2, *end2;
  int len1, len2;

  s2   = *ps2;
  end1 = s1 + mblen;
  end2 = s2 + mblen;
  while (s1 < end1) {
    len1 = ONIGENC_MBC_CASE_FOLD(enc, case_fold_flag, &s1, end1, buf1);
    len2 = ONIGENC_MBC_CASE_FOLD(enc, case_fold_flag, &s2, end2, buf2);
    if (len1 != len2) return 0;
    p1 = buf1;
    p2 = buf2;
    while (len1-- > 0) {
      if (*p1 != *p2) return 0;
      p1++;
      p2++;
    }
  }

  *ps2 = s2;
  return 1;
}

#define STRING_CMP_VALUE(s1,s2,len,is_fail) do {\
  is_fail = 0;\
  while (len-- > 0) {\
    if (*s1++ != *s2++) {\
      is_fail = 1; break;\
    }\
  }\
} while(0)

#define STRING_CMP_VALUE_IC(case_fold_flag,s1,ps2,len,is_fail) do {\
  if (string_cmp_ic(encode, case_fold_flag, s1, ps2, len) == 0) \
    is_fail = 1; \
  else \
    is_fail = 0; \
} while(0)


#define IS_EMPTY_STR           (str == end)
#define ON_STR_BEGIN(s)        ((s) == str)
#define ON_STR_END(s)          ((s) == end)
#define DATA_ENSURE_CHECK1     (s < right_range)
#define DATA_ENSURE_CHECK(n)   (s + (n) <= right_range)
#define DATA_ENSURE(n)         if (s + (n) > right_range) goto fail

#define INIT_RIGHT_RANGE    right_range = (UChar* )in_right_range

#ifdef USE_CAPTURE_HISTORY
static int
make_capture_history_tree(OnigCaptureTreeNode* node, StackType** kp,
                          StackType* stk_top, UChar* str, regex_t* reg)
{
  int n, r;
  OnigCaptureTreeNode* child;
  StackType* k = *kp;

  while (k < stk_top) {
    if (k->type == STK_MEM_START) {
      n = k->zid;
      if (n <= ONIG_MAX_CAPTURE_HISTORY_GROUP &&
          MEM_STATUS_AT(reg->capture_history, n) != 0) {
        child = history_node_new();
        CHECK_NULL_RETURN_MEMERR(child);
        child->group = n;
        child->beg = (int )(k->u.mem.pstr - str);
        r = history_tree_add_child(node, child);
        if (r != 0) return r;
        *kp = (k + 1);
        r = make_capture_history_tree(child, kp, stk_top, str, reg);
        if (r != 0) return r;

        k = *kp;
        child->end = (int )(k->u.mem.pstr - str);
      }
    }
    else if (k->type == STK_MEM_END) {
      if (k->zid == node->group) {
        node->end = (int )(k->u.mem.pstr - str);
        *kp = k;
        return 0;
      }
    }
    k++;
  }

  return 1; /* 1: root node ending. */
}
#endif

#ifdef USE_BACKREF_WITH_LEVEL
static int mem_is_in_memp(int mem, int num, MemNumType* memp)
{
  int i;

  for (i = 0; i < num; i++) {
    if (mem == (int )memp[i]) return 1;
  }
  return 0;
}

static int
backref_match_at_nested_level(regex_t* reg,
                              StackType* top, StackType* stk_base,
                              int ignore_case, int case_fold_flag,
                              int nest, int mem_num, MemNumType* memp,
                              UChar** s, const UChar* send)
{
  UChar *ss, *p, *pstart, *pend = NULL_UCHARP;
  int level;
  StackType* k;

  level = 0;
  k = top;
  k--;
  while (k >= stk_base) {
    if (k->type == STK_CALL_FRAME) {
      level--;
    }
    else if (k->type == STK_RETURN) {
      level++;
    }
    else if (level == nest) {
      if (k->type == STK_MEM_START) {
        if (mem_is_in_memp(k->zid, mem_num, memp)) {
          pstart = k->u.mem.pstr;
          if (IS_NOT_NULL(pend)) {
            if (pend - pstart > send - *s) return 0; /* or goto next_mem; */
            p  = pstart;
            ss = *s;

            if (ignore_case != 0) {
              if (string_cmp_ic(reg->enc, case_fold_flag,
                                pstart, &ss, (int )(pend - pstart)) == 0)
                return 0; /* or goto next_mem; */
            }
            else {
              while (p < pend) {
                if (*p++ != *ss++) return 0; /* or goto next_mem; */
              }
            }

            *s = ss;
            return 1;
          }
        }
      }
      else if (k->type == STK_MEM_END) {
        if (mem_is_in_memp(k->zid, mem_num, memp)) {
          pend = k->u.mem.pstr;
        }
      }
    }
    k--;
  }

  return 0;
}

static int
backref_check_at_nested_level(regex_t* reg,
                              StackType* top, StackType* stk_base,
                              int nest, int mem_num, MemNumType* memp)
{
  int level;
  StackType* k;

  level = 0;
  k = top;
  k--;
  while (k >= stk_base) {
    if (k->type == STK_CALL_FRAME) {
      level--;
    }
    else if (k->type == STK_RETURN) {
      level++;
    }
    else if (level == nest) {
      if (k->type == STK_MEM_END) {
        if (mem_is_in_memp(k->zid, mem_num, memp)) {
          return 1;
        }
      }
    }
    k--;
  }

  return 0;
}
#endif /* USE_BACKREF_WITH_LEVEL */


#ifdef ONIG_DEBUG_STATISTICS

#define USE_TIMEOFDAY

#ifdef USE_TIMEOFDAY
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
static struct timeval ts, te;
#define GETTIME(t)        gettimeofday(&(t), (struct timezone* )0)
#define TIMEDIFF(te,ts)   (((te).tv_usec - (ts).tv_usec) + \
                           (((te).tv_sec - (ts).tv_sec)*1000000))
#else
#ifdef HAVE_SYS_TIMES_H
#include <sys/times.h>
#endif
static struct tms ts, te;
#define GETTIME(t)         times(&(t))
#define TIMEDIFF(te,ts)   ((te).tms_utime - (ts).tms_utime)
#endif

static int OpCounter[256];
static int OpPrevCounter[256];
static unsigned long OpTime[256];
static int OpCurr = OP_FINISH;
static int OpPrevTarget = OP_FAIL;
static int MaxStackDepth = 0;

#define SOP_IN(opcode) do {\
  if (opcode == OpPrevTarget) OpPrevCounter[OpCurr]++;\
  OpCurr = opcode;\
  OpCounter[opcode]++;\
  GETTIME(ts);\
} while(0)

#define SOP_OUT do {\
  GETTIME(te);\
  OpTime[OpCurr] += TIMEDIFF(te, ts);\
} while(0)

extern void
onig_statistics_init(void)
{
  int i;
  for (i = 0; i < 256; i++) {
    OpCounter[i] = OpPrevCounter[i] = 0; OpTime[i] = 0;
  }
  MaxStackDepth = 0;
}

extern int
onig_print_statistics(FILE* f)
{
  int r;
  int i;

  r = fprintf(f, "   count      prev        time\n");
  if (r < 0) return -1;

  for (i = 0; OpInfo[i].opcode >= 0; i++) {
    r = fprintf(f, "%8d: %8d: %10ld: %s\n",
                OpCounter[i], OpPrevCounter[i], OpTime[i], OpInfo[i].name);
    if (r < 0) return -1;
  }
  r = fprintf(f, "\nmax stack depth: %d\n", MaxStackDepth);
  if (r < 0) return -1;

  return 0;
}

#define STACK_INC do {\
  stk++;\
  if (stk - stk_base > MaxStackDepth) \
    MaxStackDepth = stk - stk_base;\
} while(0)

#else
#define STACK_INC     stk++

#define SOP_IN(opcode)
#define SOP_OUT
#endif


/* matching region of POSIX API */
typedef int regoff_t;

typedef struct {
  regoff_t  rm_so;
  regoff_t  rm_eo;
} posix_regmatch_t;



#ifdef USE_THREADED_CODE

#define BYTECODE_INTERPRETER_START      GOTO_OP;
#define BYTECODE_INTERPRETER_END
#define CASE_OP(x)   L_##x: SOP_IN(OP_##x); sbegin = s; MATCH_DEBUG_OUT(0)
#define DEFAULT_OP   /* L_DEFAULT: */
#define NEXT_OP      sprev = sbegin; JUMP_OP
#define JUMP_OP      GOTO_OP
#ifdef USE_DIRECT_THREADED_CODE
#define GOTO_OP      goto *(p->opaddr)
#else
#define GOTO_OP      goto *opcode_to_label[p->opcode]
#endif
#define BREAK_OP     /* Nothing */

#else

#define BYTECODE_INTERPRETER_START \
  while (1) {\
  MATCH_DEBUG_OUT(0)\
  sbegin = s;\
  switch (p->opcode) {
#define BYTECODE_INTERPRETER_END  } sprev = sbegin; }
#define CASE_OP(x)   case OP_##x: SOP_IN(OP_##x);
#define DEFAULT_OP   default:
#define NEXT_OP      break
#define JUMP_OP      GOTO_OP
#define GOTO_OP      continue; break
#define BREAK_OP     break

#endif /* USE_THREADED_CODE */

#define INC_OP       p++
#define NEXT_OUT     SOP_OUT; NEXT_OP
#define JUMP_OUT     SOP_OUT; JUMP_OP
#define BREAK_OUT    SOP_OUT; BREAK_OP
#define CHECK_INTERRUPT_JUMP_OUT  SOP_OUT; CHECK_INTERRUPT_IN_MATCH; JUMP_OP


#ifdef ONIG_DEBUG_MATCH
#define MATCH_DEBUG_OUT(offset) do {\
      Operation *xp;\
      UChar *q, *bp, buf[50];\
      int len, spos;\
      spos = IS_NOT_NULL(s) ? (int )(s - str) : -1;\
      xp = p - (offset);\
      fprintf(stderr, "%7u: %7ld: %4d> \"",\
              counter, GET_STACK_INDEX(stk), spos);\
      counter++;\
      bp = buf;\
      if (IS_NOT_NULL(s)) {\
        for (i = 0, q = s; i < 7 && q < end; i++) {\
          len = enclen(encode, q);\
          while (len-- > 0) *bp++ = *q++;\
        }\
        if (q < end) { xmemcpy(bp, "...\"", 4); bp += 4; }\
        else         { xmemcpy(bp, "\"",    1); bp += 1; }\
      }\
      else {\
        xmemcpy(bp, "\"", 1); bp += 1;\
      }\
      *bp = 0;\
      fputs((char* )buf, stderr);\
      for (i = 0; i < 20 - (bp - buf); i++) fputc(' ', stderr);\
      if (xp == FinishCode)\
        fprintf(stderr, "----: finish");\
      else {\
        fprintf(stderr, "%4d: ", (int )(xp - reg->ops));\
        print_compiled_byte_code(stderr, reg, (int )(xp - reg->ops), reg->ops, encode);\
      }\
      fprintf(stderr, "\n");\
  } while(0);
#else
#define MATCH_DEBUG_OUT(offset)
#endif

#define MATCH_AT_ERROR_RETURN(err_code)  best_len = err_code; goto match_at_end


/* match data(str - end) from position (sstart). */
/* if sstart == str then set sprev to NULL. */
static int
match_at(regex_t* reg, const UChar* str, const UChar* end,
         const UChar* in_right_range, const UChar* sstart, UChar* sprev,
         MatchArg* msa)
{

#if defined(USE_DIRECT_THREADED_CODE)
  static Operation FinishCode[] = { { .opaddr=&&L_FINISH } };
#else
  static Operation FinishCode[] = { { OP_FINISH } };
#endif

#ifdef USE_THREADED_CODE
  static const void *opcode_to_label[] = {
  &&L_FINISH,
  &&L_END,
  &&L_STR_1,
  &&L_STR_2,
  &&L_STR_3,
  &&L_STR_4,
  &&L_STR_5,
  &&L_STR_N,
  &&L_STR_MB2N1,
  &&L_STR_MB2N2,
  &&L_STR_MB2N3,
  &&L_STR_MB2N,
  &&L_STR_MB3N,
  &&L_STR_MBN,
  &&L_STR_1_IC,
  &&L_STR_N_IC,
  &&L_CCLASS,
  &&L_CCLASS_MB,
  &&L_CCLASS_MIX,
  &&L_CCLASS_NOT,
  &&L_CCLASS_MB_NOT,
  &&L_CCLASS_MIX_NOT,
  &&L_ANYCHAR,
  &&L_ANYCHAR_ML,
  &&L_ANYCHAR_STAR,
  &&L_ANYCHAR_ML_STAR,
  &&L_ANYCHAR_STAR_PEEK_NEXT,
  &&L_ANYCHAR_ML_STAR_PEEK_NEXT,
  &&L_WORD,
  &&L_WORD_ASCII,
  &&L_NO_WORD,
  &&L_NO_WORD_ASCII,
  &&L_WORD_BOUNDARY,
  &&L_NO_WORD_BOUNDARY,
  &&L_WORD_BEGIN,
  &&L_WORD_END,
  &&L_TEXT_SEGMENT_BOUNDARY,
  &&L_BEGIN_BUF,
  &&L_END_BUF,
  &&L_BEGIN_LINE,
  &&L_END_LINE,
  &&L_SEMI_END_BUF,
  &&L_BEGIN_POSITION,
  &&L_BACKREF1,
  &&L_BACKREF2,
  &&L_BACKREF_N,
  &&L_BACKREF_N_IC,
  &&L_BACKREF_MULTI,
  &&L_BACKREF_MULTI_IC,
  &&L_BACKREF_WITH_LEVEL,
  &&L_BACKREF_WITH_LEVEL_IC,
  &&L_BACKREF_CHECK,
  &&L_BACKREF_CHECK_WITH_LEVEL,
  &&L_MEM_START,
  &&L_MEM_START_PUSH,
  &&L_MEM_END_PUSH,
#ifdef USE_CALL
  &&L_MEM_END_PUSH_REC,
#endif
  &&L_MEM_END,
#ifdef USE_CALL
  &&L_MEM_END_REC,
#endif
  &&L_FAIL,
  &&L_JUMP,
  &&L_PUSH,
  &&L_PUSH_SUPER,
  &&L_POP_OUT,
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
  &&L_PUSH_OR_JUMP_EXACT1,
#endif
  &&L_PUSH_IF_PEEK_NEXT,
  &&L_REPEAT,
  &&L_REPEAT_NG,
  &&L_REPEAT_INC,
  &&L_REPEAT_INC_NG,
  &&L_EMPTY_CHECK_START,
  &&L_EMPTY_CHECK_END,
  &&L_EMPTY_CHECK_END_MEMST,
#ifdef USE_CALL
  &&L_EMPTY_CHECK_END_MEMST_PUSH,
#endif
  &&L_PREC_READ_START,
  &&L_PREC_READ_END,
  &&L_PREC_READ_NOT_START,
  &&L_PREC_READ_NOT_END,
  &&L_ATOMIC_START,
  &&L_ATOMIC_END,
  &&L_LOOK_BEHIND,
  &&L_LOOK_BEHIND_NOT_START,
  &&L_LOOK_BEHIND_NOT_END,
  &&L_PUSH_SAVE_VAL,
  &&L_UPDATE_VAR,
#ifdef USE_CALL
  &&L_CALL,
  &&L_RETURN,
#endif
#ifdef USE_CALLOUT
  &&L_CALLOUT_CONTENTS,
  &&L_CALLOUT_NAME,
#endif
  };
#endif

  int i, n, num_mem, best_len, pop_level;
  LengthType tlen, tlen2;
  MemNumType mem;
  RelAddrType addr;
  UChar *s, *q, *ps, *sbegin;
  UChar *right_range;
  int is_alloca;
  char *alloc_base;
  StackType *stk_base, *stk, *stk_end;
  StackType *stkp; /* used as any purpose. */
  StackIndex *mem_start_stk, *mem_end_stk;
  UChar* keep;

#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR
  StackIndex *repeat_stk;
  StackIndex *empty_check_stk;
#endif
#ifdef USE_RETRY_LIMIT_IN_MATCH
  unsigned long retry_limit_in_match;
  unsigned long retry_in_match_counter;
#endif
#ifdef USE_CALLOUT
  int of;
#endif

  Operation* p = reg->ops;
  OnigOptionType option = reg->options;
  OnigEncoding encode = reg->enc;
  OnigCaseFoldType case_fold_flag = reg->case_fold_flag;

#ifdef ONIG_DEBUG_MATCH
  static unsigned int counter = 1;
#endif

#ifdef USE_DIRECT_THREADED_CODE
  if (IS_NULL(msa)) {
    for (i = 0; i < reg->ops_used; i++) {
       const void* addr;
       addr = opcode_to_label[reg->ocs[i]];
       p->opaddr = addr;
       p++;
    }
    return ONIG_NORMAL;
  }
#endif

#ifdef USE_CALLOUT
  msa->mp->match_at_call_counter++;
#endif

#ifdef USE_RETRY_LIMIT_IN_MATCH
  retry_limit_in_match = msa->retry_limit_in_match;
#endif

  pop_level = reg->stack_pop_level;
  num_mem = reg->num_mem;
  STACK_INIT(INIT_MATCH_STACK_SIZE);
  UPDATE_FOR_STACK_REALLOC;
  for (i = 1; i <= num_mem; i++) {
    mem_start_stk[i] = mem_end_stk[i] = INVALID_STACK_INDEX;
  }

#ifdef ONIG_DEBUG_MATCH
  fprintf(stderr, "match_at: str: %p, end: %p, start: %p, sprev: %p\n",
          str, end, sstart, sprev);
  fprintf(stderr, "size: %d, start offset: %d\n",
          (int )(end - str), (int )(sstart - str));
#endif

  best_len = ONIG_MISMATCH;
  keep = s = (UChar* )sstart;
  STACK_PUSH_BOTTOM(STK_ALT, FinishCode);  /* bottom stack */
  INIT_RIGHT_RANGE;

#ifdef USE_RETRY_LIMIT_IN_MATCH
  retry_in_match_counter = 0;
#endif

  BYTECODE_INTERPRETER_START {
    CASE_OP(END)
      n = (int )(s - sstart);
      if (n > best_len) {
        OnigRegion* region;
#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
        if (IS_FIND_LONGEST(option)) {
          if (n > msa->best_len) {
            msa->best_len = n;
            msa->best_s   = (UChar* )sstart;
          }
          else
            goto end_best_len;
        }
#endif
        best_len = n;
        region = msa->region;
        if (region) {
          if (keep > s) keep = s;

#ifdef USE_POSIX_API_REGION_OPTION
          if (IS_POSIX_REGION(msa->options)) {
            posix_regmatch_t* rmt = (posix_regmatch_t* )region;

            rmt[0].rm_so = (regoff_t )(keep - str);
            rmt[0].rm_eo = (regoff_t )(s    - str);
            for (i = 1; i <= num_mem; i++) {
              if (mem_end_stk[i] != INVALID_STACK_INDEX) {
                rmt[i].rm_so = (regoff_t )(STACK_MEM_START(reg, i) - str);
                rmt[i].rm_eo = (regoff_t )(STACK_MEM_END(reg, i)   - str);
              }
              else {
                rmt[i].rm_so = rmt[i].rm_eo = ONIG_REGION_NOTPOS;
              }
            }
          }
          else {
#endif /* USE_POSIX_API_REGION_OPTION */
            region->beg[0] = (int )(keep - str);
            region->end[0] = (int )(s    - str);
            for (i = 1; i <= num_mem; i++) {
              if (mem_end_stk[i] != INVALID_STACK_INDEX) {
                region->beg[i] = (int )(STACK_MEM_START(reg, i) - str);
                region->end[i] = (int )(STACK_MEM_END(reg, i)   - str);
              }
              else {
                region->beg[i] = region->end[i] = ONIG_REGION_NOTPOS;
              }
            }

#ifdef USE_CAPTURE_HISTORY
            if (reg->capture_history != 0) {
              int r;
              OnigCaptureTreeNode* node;

              if (IS_NULL(region->history_root)) {
                region->history_root = node = history_node_new();
                CHECK_NULL_RETURN_MEMERR(node);
              }
              else {
                node = region->history_root;
                history_tree_clear(node);
              }

              node->group = 0;
              node->beg   = (int )(keep - str);
              node->end   = (int )(s    - str);

              stkp = stk_base;
              r = make_capture_history_tree(region->history_root, &stkp,
                                            stk, (UChar* )str, reg);
              if (r < 0) MATCH_AT_ERROR_RETURN(r);
            }
#endif /* USE_CAPTURE_HISTORY */
#ifdef USE_POSIX_API_REGION_OPTION
          } /* else IS_POSIX_REGION() */
#endif
        } /* if (region) */
      } /* n > best_len */

#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
    end_best_len:
#endif
      SOP_OUT;

      if (IS_FIND_CONDITION(option)) {
        if (IS_FIND_NOT_EMPTY(option) && s == sstart) {
          best_len = ONIG_MISMATCH;
          goto fail; /* for retry */
        }
        if (IS_FIND_LONGEST(option) && DATA_ENSURE_CHECK1) {
          goto fail; /* for retry */
        }
      }

      /* default behavior: return first-matching result. */
      goto match_at_end;

    CASE_OP(STR_1)
      DATA_ENSURE(1);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      s++;
      INC_OP;
      NEXT_OUT;

    CASE_OP(STR_1_IC)
      {
        int len;
        UChar *q, lowbuf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

        DATA_ENSURE(1);
        len = ONIGENC_MBC_CASE_FOLD(encode,
                 /* DISABLE_CASE_FOLD_MULTI_CHAR(case_fold_flag), */
                                    case_fold_flag,
                                    &s, end, lowbuf);
        DATA_ENSURE(0);
        q = lowbuf;
        ps = p->exact.s;
        while (len-- > 0) {
          if (*ps != *q) goto fail;
          ps++; q++;
        }
      }
      INC_OP;
      NEXT_OUT;

    CASE_OP(STR_2)
      DATA_ENSURE(2);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      sprev = s;
      s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(STR_3)
      DATA_ENSURE(3);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      sprev = s;
      s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(STR_4)
      DATA_ENSURE(4);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      sprev = s;
      s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(STR_5)
      DATA_ENSURE(5);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      sprev = s;
      s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(STR_N)
      tlen = p->exact_n.n;
      DATA_ENSURE(tlen);
      ps = p->exact_n.s;
      while (tlen-- > 0) {
        if (*ps++ != *s++) goto fail;
      }
      sprev = s - 1;
      INC_OP;
      JUMP_OUT;

    CASE_OP(STR_N_IC)
      {
        int len;
        UChar *q, *endp, lowbuf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

        tlen = p->exact_n.n;
        ps   = p->exact_n.s;
        endp = ps + tlen;
        while (ps < endp) {
          sprev = s;
          DATA_ENSURE(1);
          len = ONIGENC_MBC_CASE_FOLD(encode,
                        /* DISABLE_CASE_FOLD_MULTI_CHAR(case_fold_flag), */
                                      case_fold_flag,
                                      &s, end, lowbuf);
          DATA_ENSURE(0);
          q = lowbuf;
          while (len-- > 0) {
            if (ps >= endp) goto fail;
            if (*ps != *q) goto fail;
            ps++; q++;
          }
        }
      }

      INC_OP;
      JUMP_OUT;

    CASE_OP(STR_MB2N1)
      DATA_ENSURE(2);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      s++;
      INC_OP;
      NEXT_OUT;

    CASE_OP(STR_MB2N2)
      DATA_ENSURE(4);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      sprev = s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(STR_MB2N3)
      DATA_ENSURE(6);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      sprev = s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(STR_MB2N)
      tlen = p->exact_n.n;
      DATA_ENSURE(tlen * 2);
      ps = p->exact_n.s;
      while (tlen-- > 0) {
        if (*ps != *s) goto fail;
        ps++; s++;
        if (*ps != *s) goto fail;
        ps++; s++;
      }
      sprev = s - 2;
      INC_OP;
      JUMP_OUT;

    CASE_OP(STR_MB3N)
      tlen = p->exact_n.n;
      DATA_ENSURE(tlen * 3);
      ps = p->exact_n.s;
      while (tlen-- > 0) {
        if (*ps != *s) goto fail;
        ps++; s++;
        if (*ps != *s) goto fail;
        ps++; s++;
        if (*ps != *s) goto fail;
        ps++; s++;
      }
      sprev = s - 3;
      INC_OP;
      JUMP_OUT;

    CASE_OP(STR_MBN)
      tlen  = p->exact_len_n.len; /* mb byte len */
      tlen2 = p->exact_len_n.n;   /* number of chars */
      tlen2 *= tlen;
      DATA_ENSURE(tlen2);
      ps = p->exact_len_n.s;
      while (tlen2-- > 0) {
        if (*ps != *s) goto fail;
        ps++; s++;
      }
      sprev = s - tlen;
      INC_OP;
      JUMP_OUT;

    CASE_OP(CCLASS)
      DATA_ENSURE(1);
      if (BITSET_AT(p->cclass.bsp, *s) == 0) goto fail;
      s++;
      INC_OP;
      NEXT_OUT;

    CASE_OP(CCLASS_MB)
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_HEAD(encode, s)) goto fail;

    cclass_mb:
      {
        OnigCodePoint code;
        UChar *ss;
        int mb_len;

        mb_len = enclen(encode, s);
        DATA_ENSURE(mb_len);
        ss = s;
        s += mb_len;
        code = ONIGENC_MBC_TO_CODE(encode, ss, s);
        if (! onig_is_in_code_range(p->cclass_mb.mb, code)) goto fail;
      }
      INC_OP;
      NEXT_OUT;

    CASE_OP(CCLASS_MIX)
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_HEAD(encode, s)) {
        goto cclass_mb;
      }
      else {
        if (BITSET_AT(p->cclass_mix.bsp, *s) == 0)
          goto fail;

        s++;
      }
      INC_OP;
      NEXT_OUT;

    CASE_OP(CCLASS_NOT)
      DATA_ENSURE(1);
      if (BITSET_AT(p->cclass.bsp, *s) != 0) goto fail;
      s += enclen(encode, s);
      INC_OP;
      NEXT_OUT;

    CASE_OP(CCLASS_MB_NOT)
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_HEAD(encode, s)) {
        s++;
        goto cc_mb_not_success;
      }

    cclass_mb_not:
      {
        OnigCodePoint code;
        UChar *ss;
        int mb_len = enclen(encode, s);

        if (! DATA_ENSURE_CHECK(mb_len)) {
          DATA_ENSURE(1);
          s = (UChar* )end;
          goto cc_mb_not_success;
        }

        ss = s;
        s += mb_len;
        code = ONIGENC_MBC_TO_CODE(encode, ss, s);
        if (onig_is_in_code_range(p->cclass_mb.mb, code)) goto fail;
      }

    cc_mb_not_success:
      INC_OP;
      NEXT_OUT;

    CASE_OP(CCLASS_MIX_NOT)
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_HEAD(encode, s)) {
        goto cclass_mb_not;
      }
      else {
        if (BITSET_AT(p->cclass_mix.bsp, *s) != 0)
          goto fail;

        s++;
      }
      INC_OP;
      NEXT_OUT;

    CASE_OP(ANYCHAR)
      DATA_ENSURE(1);
      n = enclen(encode, s);
      DATA_ENSURE(n);
      if (ONIGENC_IS_MBC_NEWLINE(encode, s, end)) goto fail;
      s += n;
      INC_OP;
      NEXT_OUT;

    CASE_OP(ANYCHAR_ML)
      DATA_ENSURE(1);
      n = enclen(encode, s);
      DATA_ENSURE(n);
      s += n;
      INC_OP;
      NEXT_OUT;

    CASE_OP(ANYCHAR_STAR)
      INC_OP;
      while (DATA_ENSURE_CHECK1) {
        STACK_PUSH_ALT(p, s, sprev);
        n = enclen(encode, s);
        DATA_ENSURE(n);
        if (ONIGENC_IS_MBC_NEWLINE(encode, s, end))  goto fail;
        sprev = s;
        s += n;
      }
      JUMP_OUT;

    CASE_OP(ANYCHAR_ML_STAR)
      INC_OP;
      while (DATA_ENSURE_CHECK1) {
        STACK_PUSH_ALT(p, s, sprev);
        n = enclen(encode, s);
        if (n > 1) {
          DATA_ENSURE(n);
          sprev = s;
          s += n;
        }
        else {
          sprev = s;
          s++;
        }
      }
      JUMP_OUT;

    CASE_OP(ANYCHAR_STAR_PEEK_NEXT)
      {
        UChar c;

        c = p->anychar_star_peek_next.c;
        INC_OP;
        while (DATA_ENSURE_CHECK1) {
          if (c == *s) {
            STACK_PUSH_ALT(p, s, sprev);
          }
          n = enclen(encode, s);
          DATA_ENSURE(n);
          if (ONIGENC_IS_MBC_NEWLINE(encode, s, end))  goto fail;
          sprev = s;
          s += n;
        }
      }
      NEXT_OUT;

    CASE_OP(ANYCHAR_ML_STAR_PEEK_NEXT)
      {
        UChar c;

        c = p->anychar_star_peek_next.c;
        INC_OP;
        while (DATA_ENSURE_CHECK1) {
          if (c == *s) {
            STACK_PUSH_ALT(p, s, sprev);
          }
          n = enclen(encode, s);
          if (n > 1) {
            DATA_ENSURE(n);
            sprev = s;
            s += n;
          }
          else {
            sprev = s;
            s++;
          }
        }
      }
      NEXT_OUT;

    CASE_OP(WORD)
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_WORD(encode, s, end))
        goto fail;

      s += enclen(encode, s);
      INC_OP;
      NEXT_OUT;

    CASE_OP(WORD_ASCII)
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_WORD_ASCII(encode, s, end))
        goto fail;

      s += enclen(encode, s);
      INC_OP;
      NEXT_OUT;

    CASE_OP(NO_WORD)
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_WORD(encode, s, end))
        goto fail;

      s += enclen(encode, s);
      INC_OP;
      NEXT_OUT;

    CASE_OP(NO_WORD_ASCII)
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_WORD_ASCII(encode, s, end))
        goto fail;

      s += enclen(encode, s);
      INC_OP;
      NEXT_OUT;

    CASE_OP(WORD_BOUNDARY)
      {
        ModeType mode;

        mode = p->word_boundary.mode;
        if (ON_STR_BEGIN(s)) {
          DATA_ENSURE(1);
          if (! IS_MBC_WORD_ASCII_MODE(encode, s, end, mode))
            goto fail;
        }
        else if (ON_STR_END(s)) {
          if (! IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode))
            goto fail;
        }
        else {
          if (IS_MBC_WORD_ASCII_MODE(encode, s, end, mode)
              == IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode))
            goto fail;
        }
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(NO_WORD_BOUNDARY)
      {
        ModeType mode;

        mode = p->word_boundary.mode;
        if (ON_STR_BEGIN(s)) {
          if (DATA_ENSURE_CHECK1 && IS_MBC_WORD_ASCII_MODE(encode, s, end, mode))
            goto fail;
        }
        else if (ON_STR_END(s)) {
          if (IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode))
            goto fail;
        }
        else {
          if (IS_MBC_WORD_ASCII_MODE(encode, s, end, mode)
              != IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode))
            goto fail;
        }
      }
      INC_OP;
      JUMP_OUT;

#ifdef USE_WORD_BEGIN_END
    CASE_OP(WORD_BEGIN)
      {
        ModeType mode;

        mode = p->word_boundary.mode;
        if (DATA_ENSURE_CHECK1 && IS_MBC_WORD_ASCII_MODE(encode, s, end, mode)) {
          if (ON_STR_BEGIN(s) || !IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode)) {
            INC_OP;
            JUMP_OUT;
          }
        }
      }
      goto fail;

    CASE_OP(WORD_END)
      {
        ModeType mode;

        mode = p->word_boundary.mode;
        if (!ON_STR_BEGIN(s) && IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode)) {
          if (ON_STR_END(s) || ! IS_MBC_WORD_ASCII_MODE(encode, s, end, mode)) {
            INC_OP;
            JUMP_OUT;
          }
        }
      }
      goto fail;
#endif

    CASE_OP(TEXT_SEGMENT_BOUNDARY)
      {
        int is_break;

        switch (p->text_segment_boundary.type) {
        case EXTENDED_GRAPHEME_CLUSTER_BOUNDARY:
          is_break = onigenc_egcb_is_break_position(encode, s, sprev, str, end);
          break;
#ifdef USE_UNICODE_WORD_BREAK
        case WORD_BOUNDARY:
          is_break = onigenc_wb_is_break_position(encode, s, sprev, str, end);
          break;
#endif
        default:
          MATCH_AT_ERROR_RETURN(ONIGERR_UNDEFINED_BYTECODE);
          break;
        }

        if (p->text_segment_boundary.not != 0)
          is_break = ! is_break;

        if (is_break != 0) {
          INC_OP;
          JUMP_OUT;
        }
        else {
          goto fail;
        }
      }

    CASE_OP(BEGIN_BUF)
      if (! ON_STR_BEGIN(s)) goto fail;

      INC_OP;
      JUMP_OUT;

    CASE_OP(END_BUF)
      if (! ON_STR_END(s)) goto fail;

      INC_OP;
      JUMP_OUT;

    CASE_OP(BEGIN_LINE)
      if (ON_STR_BEGIN(s)) {
        if (IS_NOTBOL(msa->options)) goto fail;
        INC_OP;
        JUMP_OUT;
      }
      else if (ONIGENC_IS_MBC_NEWLINE(encode, sprev, end) && !ON_STR_END(s)) {
        INC_OP;
        JUMP_OUT;
      }
      goto fail;

    CASE_OP(END_LINE)
      if (ON_STR_END(s)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        if (IS_EMPTY_STR || !ONIGENC_IS_MBC_NEWLINE(encode, sprev, end)) {
#endif
          if (IS_NOTEOL(msa->options)) goto fail;
          INC_OP;
          JUMP_OUT;
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        }
#endif
      }
      else if (ONIGENC_IS_MBC_NEWLINE(encode, s, end)) {
        INC_OP;
        JUMP_OUT;
      }
#ifdef USE_CRNL_AS_LINE_TERMINATOR
      else if (ONIGENC_IS_MBC_CRNL(encode, s, end)) {
        INC_OP;
        JUMP_OUT;
      }
#endif
      goto fail;

    CASE_OP(SEMI_END_BUF)
      if (ON_STR_END(s)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        if (IS_EMPTY_STR || !ONIGENC_IS_MBC_NEWLINE(encode, sprev, end)) {
#endif
          if (IS_NOTEOL(msa->options)) goto fail;
          INC_OP;
          JUMP_OUT;
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        }
#endif
      }
      else if (ONIGENC_IS_MBC_NEWLINE(encode, s, end) &&
               ON_STR_END(s + enclen(encode, s))) {
        INC_OP;
        JUMP_OUT;
      }
#ifdef USE_CRNL_AS_LINE_TERMINATOR
      else if (ONIGENC_IS_MBC_CRNL(encode, s, end)) {
        UChar* ss = s + enclen(encode, s);
        ss += enclen(encode, ss);
        if (ON_STR_END(ss)) {
          INC_OP;
          JUMP_OUT;
        }
      }
#endif
      goto fail;

    CASE_OP(BEGIN_POSITION)
      if (s != msa->start)
        goto fail;

      INC_OP;
      JUMP_OUT;

    CASE_OP(MEM_START_PUSH)
      mem = p->memory_start.num;
      STACK_PUSH_MEM_START(mem, s);
      INC_OP;
      JUMP_OUT;

    CASE_OP(MEM_START)
      mem = p->memory_start.num;
      mem_start_stk[mem] = (StackIndex )((void* )s);
      INC_OP;
      JUMP_OUT;

    CASE_OP(MEM_END_PUSH)
      mem = p->memory_end.num;
      STACK_PUSH_MEM_END(mem, s);
      INC_OP;
      JUMP_OUT;

    CASE_OP(MEM_END)
      mem = p->memory_end.num;
      mem_end_stk[mem] = (StackIndex )((void* )s);
      INC_OP;
      JUMP_OUT;

#ifdef USE_CALL
    CASE_OP(MEM_END_PUSH_REC)
      {
        StackIndex si;

        mem = p->memory_end.num;
        STACK_GET_MEM_START(mem, stkp); /* should be before push mem-end. */
        si = GET_STACK_INDEX(stkp);
        STACK_PUSH_MEM_END(mem, s);
        mem_start_stk[mem] = si;
        INC_OP;
        JUMP_OUT;
      }

    CASE_OP(MEM_END_REC)
      mem = p->memory_end.num;
      mem_end_stk[mem] = (StackIndex )((void* )s);
      STACK_GET_MEM_START(mem, stkp);

      if (MEM_STATUS_AT(reg->push_mem_start, mem))
        mem_start_stk[mem] = GET_STACK_INDEX(stkp);
      else
        mem_start_stk[mem] = (StackIndex )((void* )stkp->u.mem.pstr);

      STACK_PUSH_MEM_END_MARK(mem);
      INC_OP;
      JUMP_OUT;
#endif

    CASE_OP(BACKREF1)
      mem = 1;
      goto backref;

    CASE_OP(BACKREF2)
      mem = 2;
      goto backref;

    CASE_OP(BACKREF_N)
      mem = p->backref_n.n1;
    backref:
      {
        int len;
        UChar *pstart, *pend;

        if (mem_end_stk[mem]   == INVALID_STACK_INDEX) goto fail;
        if (mem_start_stk[mem] == INVALID_STACK_INDEX) goto fail;

        pstart = STACK_MEM_START(reg, mem);
        pend   = STACK_MEM_END(reg, mem);
        n = (int )(pend - pstart);
        if (n != 0) {
          DATA_ENSURE(n);
          sprev = s;
          STRING_CMP(s, pstart, n);
          while (sprev + (len = enclen(encode, sprev)) < s)
            sprev += len;
        }
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(BACKREF_N_IC)
      mem = p->backref_n.n1;
      {
        int len;
        UChar *pstart, *pend;

        if (mem_end_stk[mem]   == INVALID_STACK_INDEX) goto fail;
        if (mem_start_stk[mem] == INVALID_STACK_INDEX) goto fail;

        pstart = STACK_MEM_START(reg, mem);
        pend   = STACK_MEM_END(reg, mem);
        n = (int )(pend - pstart);
        if (n != 0) {
          DATA_ENSURE(n);
          sprev = s;
          STRING_CMP_IC(case_fold_flag, pstart, &s, n);
          while (sprev + (len = enclen(encode, sprev)) < s)
            sprev += len;
        }
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(BACKREF_MULTI)
      {
        int len, is_fail;
        UChar *pstart, *pend, *swork;

        tlen = p->backref_general.num;
        for (i = 0; i < tlen; i++) {
          mem = tlen == 1 ? p->backref_general.n1 : p->backref_general.ns[i];

          if (mem_end_stk[mem]   == INVALID_STACK_INDEX) continue;
          if (mem_start_stk[mem] == INVALID_STACK_INDEX) continue;

          pstart = STACK_MEM_START(reg, mem);
          pend   = STACK_MEM_END(reg, mem);
          n = (int )(pend - pstart);
          if (n != 0) {
            DATA_ENSURE(n);
            sprev = s;
            swork = s;
            STRING_CMP_VALUE(swork, pstart, n, is_fail);
            if (is_fail) continue;
            s = swork;
            while (sprev + (len = enclen(encode, sprev)) < s)
              sprev += len;
          }
          break; /* success */
        }
        if (i == tlen) goto fail;
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(BACKREF_MULTI_IC)
      {
        int len, is_fail;
        UChar *pstart, *pend, *swork;

        tlen = p->backref_general.num;
        for (i = 0; i < tlen; i++) {
          mem = tlen == 1 ? p->backref_general.n1 : p->backref_general.ns[i];

          if (mem_end_stk[mem]   == INVALID_STACK_INDEX) continue;
          if (mem_start_stk[mem] == INVALID_STACK_INDEX) continue;

          pstart = STACK_MEM_START(reg, mem);
          pend   = STACK_MEM_END(reg, mem);
          n = (int )(pend - pstart);
          if (n != 0) {
            DATA_ENSURE(n);
            sprev = s;
            swork = s;
            STRING_CMP_VALUE_IC(case_fold_flag, pstart, &swork, n, is_fail);
            if (is_fail) continue;
            s = swork;
            while (sprev + (len = enclen(encode, sprev)) < s)
              sprev += len;
          }
          break; /* success */
        }
        if (i == tlen) goto fail;
      }
      INC_OP;
      JUMP_OUT;

#ifdef USE_BACKREF_WITH_LEVEL
    CASE_OP(BACKREF_WITH_LEVEL_IC)
      n = 1; /* ignore case */
      goto backref_with_level;
    CASE_OP(BACKREF_WITH_LEVEL)
      {
        int len;
        int level;
        MemNumType* mems;
        UChar* ssave;

        n = 0;
      backref_with_level:
        level = p->backref_general.nest_level;
        tlen  = p->backref_general.num;
        mems = tlen == 1 ? &(p->backref_general.n1) : p->backref_general.ns;

        ssave = s;
        if (backref_match_at_nested_level(reg, stk, stk_base, n,
                    case_fold_flag, level, (int )tlen, mems, &s, end)) {
          if (ssave != s) {
            sprev = ssave;
            while (sprev + (len = enclen(encode, sprev)) < s)
              sprev += len;
          }
        }
        else
          goto fail;
      }
      INC_OP;
      JUMP_OUT;
#endif

    CASE_OP(BACKREF_CHECK)
      {
        MemNumType* mems;

        tlen  = p->backref_general.num;
        mems = tlen == 1 ? &(p->backref_general.n1) : p->backref_general.ns;

        for (i = 0; i < tlen; i++) {
          mem = mems[i];
          if (mem_end_stk[mem]   == INVALID_STACK_INDEX) continue;
          if (mem_start_stk[mem] == INVALID_STACK_INDEX) continue;
          break; /* success */
        }
        if (i == tlen) goto fail;
      }
      INC_OP;
      JUMP_OUT;

#ifdef USE_BACKREF_WITH_LEVEL
    CASE_OP(BACKREF_CHECK_WITH_LEVEL)
      {
        LengthType level;
        MemNumType* mems;

        level = p->backref_general.nest_level;
        tlen  = p->backref_general.num;
        mems = tlen == 1 ? &(p->backref_general.n1) : p->backref_general.ns;

        if (backref_check_at_nested_level(reg, stk, stk_base,
                                          (int )level, (int )tlen, mems) == 0)
          goto fail;
      }
      INC_OP;
      JUMP_OUT;
#endif

    CASE_OP(EMPTY_CHECK_START)
      mem = p->empty_check_start.mem;   /* mem: null check id */
      STACK_PUSH_EMPTY_CHECK_START(mem, s);
      INC_OP;
      JUMP_OUT;

    CASE_OP(EMPTY_CHECK_END)
      {
        int is_empty;

        mem = p->empty_check_end.mem;  /* mem: null check id */
        STACK_EMPTY_CHECK(is_empty, mem, s);
        INC_OP;
        if (is_empty) {
#ifdef ONIG_DEBUG_MATCH
          fprintf(stderr, "EMPTY_CHECK_END: skip  id:%d, s:%p\n", (int )mem, s);
#endif
        empty_check_found:
          /* empty loop founded, skip next instruction */
#if defined(ONIG_DEBUG) && !defined(USE_DIRECT_THREADED_CODE)
          switch (p->opcode) {
          case OP_JUMP:
          case OP_PUSH:
          case OP_REPEAT_INC:
          case OP_REPEAT_INC_NG:
            INC_OP;
            break;
          default:
            MATCH_AT_ERROR_RETURN(ONIGERR_UNEXPECTED_BYTECODE);
            break;
          }
#else
          INC_OP;
#endif
        }
      }
      JUMP_OUT;

#ifdef USE_STUBBORN_CHECK_CAPTURES_IN_EMPTY_REPEAT
    CASE_OP(EMPTY_CHECK_END_MEMST)
      {
        int is_empty;

        mem = p->empty_check_end.mem;  /* mem: null check id */
        STACK_EMPTY_CHECK_MEM(is_empty, mem, s, reg);
        INC_OP;
        if (is_empty) {
#ifdef ONIG_DEBUG_MATCH
          fprintf(stderr, "EMPTY_CHECK_END_MEM: skip  id:%d, s:%p\n", (int)mem, s);
#endif
          if (is_empty == -1) goto fail;
          goto empty_check_found;
        }
      }
      JUMP_OUT;
#endif

#ifdef USE_CALL
    CASE_OP(EMPTY_CHECK_END_MEMST_PUSH)
      {
        int is_empty;

        mem = p->empty_check_end.mem;  /* mem: null check id */
#ifdef USE_STUBBORN_CHECK_CAPTURES_IN_EMPTY_REPEAT
        STACK_EMPTY_CHECK_MEM_REC(is_empty, mem, s, reg);
#else
        STACK_EMPTY_CHECK_REC(is_empty, mem, s);
#endif
        INC_OP;
        if (is_empty) {
#ifdef ONIG_DEBUG_MATCH
          fprintf(stderr, "EMPTY_CHECK_END_MEM_PUSH: skip  id:%d, s:%p\n",
                  (int )mem, s);
#endif
          if (is_empty == -1) goto fail;
          goto empty_check_found;
        }
        else {
          STACK_PUSH_EMPTY_CHECK_END(mem);
        }
      }
      JUMP_OUT;
#endif

    CASE_OP(JUMP)
      addr = p->jump.addr;
      p += addr;
      CHECK_INTERRUPT_JUMP_OUT;

    CASE_OP(PUSH)
      addr = p->push.addr;
      STACK_PUSH_ALT(p + addr, s, sprev);
      INC_OP;
      JUMP_OUT;

    CASE_OP(PUSH_SUPER)
      addr = p->push.addr;
      STACK_PUSH_SUPER_ALT(p + addr, s, sprev);
      INC_OP;
      JUMP_OUT;

    CASE_OP(POP_OUT)
      STACK_POP_ONE;
      /* for stop backtrack */
      /* CHECK_RETRY_LIMIT_IN_MATCH; */
      INC_OP;
      JUMP_OUT;

 #ifdef USE_OP_PUSH_OR_JUMP_EXACT
    CASE_OP(PUSH_OR_JUMP_EXACT1)
      {
        UChar c;

        addr = p->push_or_jump_exact1.addr;
        c    = p->push_or_jump_exact1.c;
        if (DATA_ENSURE_CHECK1 && c == *s) {
          STACK_PUSH_ALT(p + addr, s, sprev);
          INC_OP;
          JUMP_OUT;
        }
      }
      p += addr;
      JUMP_OUT;
#endif

    CASE_OP(PUSH_IF_PEEK_NEXT)
      {
        UChar c;

        addr = p->push_if_peek_next.addr;
        c    = p->push_if_peek_next.c;
        if (DATA_ENSURE_CHECK1 && c == *s) {
          STACK_PUSH_ALT(p + addr, s, sprev);
          INC_OP;
          JUMP_OUT;
        }
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(REPEAT)
      mem  = p->repeat.id;  /* mem: OP_REPEAT ID */
      addr = p->repeat.addr;

      STACK_PUSH_REPEAT_INC(mem, 0);
      if (reg->repeat_range[mem].lower == 0) {
        STACK_PUSH_ALT(p + addr, s, sprev);
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(REPEAT_NG)
      mem  = p->repeat.id;  /* mem: OP_REPEAT ID */
      addr = p->repeat.addr;

      STACK_PUSH_REPEAT_INC(mem, 0);
      if (reg->repeat_range[mem].lower == 0) {
        STACK_PUSH_ALT(p + 1, s, sprev);
        p += addr;
      }
      else
        INC_OP;
      JUMP_OUT;

    CASE_OP(REPEAT_INC)
      mem  = p->repeat_inc.id;  /* mem: OP_REPEAT ID */
      STACK_GET_REPEAT_COUNT(mem, n);
      n++;
      if (n >= reg->repeat_range[mem].upper) {
        /* end of repeat. Nothing to do. */
        INC_OP;
      }
      else if (n >= reg->repeat_range[mem].lower) {
        INC_OP;
        STACK_PUSH_ALT(p, s, sprev);
        p = reg->repeat_range[mem].u.pcode;
      }
      else {
        p = reg->repeat_range[mem].u.pcode;
      }
      STACK_PUSH_REPEAT_INC(mem, n);
      CHECK_INTERRUPT_JUMP_OUT;

    CASE_OP(REPEAT_INC_NG)
      mem = p->repeat_inc.id;  /* mem: OP_REPEAT ID */
      STACK_GET_REPEAT_COUNT(mem, n);
      n++;
      STACK_PUSH_REPEAT_INC(mem, n);
      if (n == reg->repeat_range[mem].upper) {
        INC_OP;
      }
      else {
        if (n >= reg->repeat_range[mem].lower) {
          STACK_PUSH_ALT(reg->repeat_range[mem].u.pcode, s, sprev);
          INC_OP;
        }
        else {
          p = reg->repeat_range[mem].u.pcode;
        }
      }
      CHECK_INTERRUPT_JUMP_OUT;

    CASE_OP(PREC_READ_START)
      STACK_PUSH_PREC_READ_START(s, sprev);
      INC_OP;
      JUMP_OUT;

    CASE_OP(PREC_READ_END)
      STACK_GET_PREC_READ_START(stkp);
      s     = stkp->u.state.pstr;
      sprev = stkp->u.state.pstr_prev;
      STACK_PUSH(STK_PREC_READ_END,0,0,0);
      INC_OP;
      JUMP_OUT;

    CASE_OP(PREC_READ_NOT_START)
      addr = p->prec_read_not_start.addr;
      STACK_PUSH_ALT_PREC_READ_NOT(p + addr, s, sprev);
      INC_OP;
      JUMP_OUT;

    CASE_OP(PREC_READ_NOT_END)
      STACK_POP_TIL_ALT_PREC_READ_NOT;
      goto fail;

    CASE_OP(ATOMIC_START)
      STACK_PUSH_TO_VOID_START;
      INC_OP;
      JUMP_OUT;

    CASE_OP(ATOMIC_END)
      STACK_EXEC_TO_VOID(stkp);
      INC_OP;
      JUMP_OUT;

    CASE_OP(LOOK_BEHIND)
      tlen = p->look_behind.len;
      s = (UChar* )ONIGENC_STEP_BACK(encode, str, s, (int )tlen);
      if (IS_NULL(s)) goto fail;
      sprev = (UChar* )onigenc_get_prev_char_head(encode, str, s);
      INC_OP;
      JUMP_OUT;

    CASE_OP(LOOK_BEHIND_NOT_START)
      addr = p->look_behind_not_start.addr;
      tlen = p->look_behind_not_start.len;
      q = (UChar* )ONIGENC_STEP_BACK(encode, str, s, (int )tlen);
      if (IS_NULL(q)) {
        /* too short case -> success. ex. /(?<!XXX)a/.match("a")
           If you want to change to fail, replace following line. */
        p += addr;
        /* goto fail; */
      }
      else {
        STACK_PUSH_ALT_LOOK_BEHIND_NOT(p + addr, s, sprev);
        s = q;
        sprev = (UChar* )onigenc_get_prev_char_head(encode, str, s);
        INC_OP;
      }
      JUMP_OUT;

    CASE_OP(LOOK_BEHIND_NOT_END)
      STACK_POP_TIL_ALT_LOOK_BEHIND_NOT;
      INC_OP;
      goto fail;

#ifdef USE_CALL
    CASE_OP(CALL)
      addr = p->call.addr;
      INC_OP; STACK_PUSH_CALL_FRAME(p);
      p = reg->ops + addr;
      JUMP_OUT;

    CASE_OP(RETURN)
      STACK_RETURN(p);
      STACK_PUSH_RETURN;
      JUMP_OUT;
#endif

    CASE_OP(PUSH_SAVE_VAL)
      {
        SaveType type;

        type = p->push_save_val.type;
        mem  = p->push_save_val.id; /* mem: save id */
        switch ((enum SaveType )type) {
        case SAVE_KEEP:
          STACK_PUSH_SAVE_VAL(mem, type, s);
          break;

        case SAVE_S:
          STACK_PUSH_SAVE_VAL_WITH_SPREV(mem, type, s);
          break;

        case SAVE_RIGHT_RANGE:
          STACK_PUSH_SAVE_VAL(mem, SAVE_RIGHT_RANGE, right_range);
          break;
        }
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(UPDATE_VAR)
      {
        UpdateVarType type;
        enum SaveType save_type;

        type = p->update_var.type;
        mem  = p->update_var.id; /* mem: save id */

        switch ((enum UpdateVarType )type) {
        case UPDATE_VAR_KEEP_FROM_STACK_LAST:
          STACK_GET_SAVE_VAL_TYPE_LAST(SAVE_KEEP, keep);
          break;
        case UPDATE_VAR_S_FROM_STACK:
          STACK_GET_SAVE_VAL_TYPE_LAST_ID_WITH_SPREV(SAVE_S, mem, s);
          break;
        case UPDATE_VAR_RIGHT_RANGE_FROM_S_STACK:
          save_type = SAVE_S;
          goto get_save_val_type_last_id;
          break;
        case UPDATE_VAR_RIGHT_RANGE_FROM_STACK:
          save_type = SAVE_RIGHT_RANGE;
        get_save_val_type_last_id:
          STACK_GET_SAVE_VAL_TYPE_LAST_ID(save_type, mem, right_range);
          break;
        case UPDATE_VAR_RIGHT_RANGE_INIT:
          INIT_RIGHT_RANGE;
          break;
        }
      }
      INC_OP;
      JUMP_OUT;

#ifdef USE_CALLOUT
    CASE_OP(CALLOUT_CONTENTS)
      of = ONIG_CALLOUT_OF_CONTENTS;
      mem = p->callout_contents.num;
      goto callout_common_entry;
      BREAK_OUT;

    CASE_OP(CALLOUT_NAME)
      {
        int call_result;
        int name_id;
        int in;
        CalloutListEntry* e;
        OnigCalloutFunc func;
        OnigCalloutArgs args;

        of  = ONIG_CALLOUT_OF_NAME;
        mem = p->callout_name.num;

      callout_common_entry:
        e = onig_reg_callout_list_at(reg, mem);
        in = e->in;
        if (of == ONIG_CALLOUT_OF_NAME) {
          name_id = p->callout_name.id;
          func = onig_get_callout_start_func(reg, mem);
        }
        else {
          name_id = ONIG_NON_NAME_ID;
          func = msa->mp->progress_callout_of_contents;
        }

        if (IS_NOT_NULL(func) && (in & ONIG_CALLOUT_IN_PROGRESS) != 0) {
          CALLOUT_BODY(func, ONIG_CALLOUT_IN_PROGRESS, name_id,
                       (int )mem, msa->mp->callout_user_data, args, call_result);
          switch (call_result) {
          case ONIG_CALLOUT_FAIL:
            goto fail;
            break;
          case ONIG_CALLOUT_SUCCESS:
            goto retraction_callout2;
            break;
          default: /* error code */
            if (call_result > 0) {
              call_result = ONIGERR_INVALID_ARGUMENT;
            }
            best_len = call_result;
            goto match_at_end;
            break;
          }
        }
        else {
        retraction_callout2:
          if ((in & ONIG_CALLOUT_IN_RETRACTION) != 0) {
            if (of == ONIG_CALLOUT_OF_NAME) {
              if (IS_NOT_NULL(func)) {
                STACK_PUSH_CALLOUT_NAME(name_id, mem, func);
              }
            }
            else {
              func = msa->mp->retraction_callout_of_contents;
              if (IS_NOT_NULL(func)) {
                STACK_PUSH_CALLOUT_CONTENTS(mem, func);
              }
            }
          }
        }
      }
      INC_OP;
      JUMP_OUT;
#endif

    CASE_OP(FINISH)
      goto match_at_end;

#ifdef ONIG_DEBUG_STATISTICS
    fail:
      SOP_OUT;
      goto fail2;
#endif
    CASE_OP(FAIL)
#ifdef ONIG_DEBUG_STATISTICS
    fail2:
#else
    fail:
#endif
      STACK_POP;
      p     = stk->u.state.pcode;
      s     = stk->u.state.pstr;
      sprev = stk->u.state.pstr_prev;
      CHECK_RETRY_LIMIT_IN_MATCH;
      JUMP_OUT;

    DEFAULT_OP
      MATCH_AT_ERROR_RETURN(ONIGERR_UNDEFINED_BYTECODE);

  } BYTECODE_INTERPRETER_END;

 match_at_end:
  STACK_SAVE;
  return best_len;
}

typedef struct {
  regex_t*    reg;
  OnigRegion* region;
} RR;

struct OnigRegSetStruct {
  RR*          rs;
  int          n;
  int          alloc;
  OnigEncoding enc;
  int          anchor;      /* BEGIN_BUF, BEGIN_POS, (SEMI_)END_BUF */
  OnigLen      anc_dmin;    /* (SEMI_)END_BUF anchor distance */
  OnigLen      anc_dmax;    /* (SEMI_)END_BUF anchor distance */
  int          all_low_high;
  int          anychar_inf;
};

enum SearchRangeStatus {
  SRS_DEAD      = 0,
  SRS_LOW_HIGH  = 1,
  SRS_ALL_RANGE = 2
};

typedef struct {
  int    state;  /* value of enum SearchRangeStatus */
  UChar* low;
  UChar* high;
  UChar* low_prev;
  UChar* sch_range;
} SearchRange;

#define REGSET_MATCH_AND_RETURN_CHECK(upper_range) \
  r = match_at(reg, str, end, (upper_range), s, prev, msas + i); \
  if (r != ONIG_MISMATCH) {\
    if (r >= 0) {\
      goto match;\
    }\
    else goto finish; /* error */ \
  }

static inline int
regset_search_body_position_lead(OnigRegSet* set,
           const UChar* str, const UChar* end,
           const UChar* start, const UChar* range, /* match start range */
           const UChar* orig_range, /* data range */
           OnigOptionType option, MatchArg* msas, int* rmatch_pos)
{
  int r, n, i;
  UChar *s, *prev;
  UChar *low, *high, *low_prev;
  UChar* sch_range;
  regex_t* reg;
  OnigEncoding enc;
  SearchRange* sr;

  n   = set->n;
  enc = set->enc;

  s = (UChar* )start;
  if (s > str)
    prev = onigenc_get_prev_char_head(enc, str, s);
  else
    prev = (UChar* )NULL;

  sr = (SearchRange* )xmalloc(sizeof(*sr) * n);
  CHECK_NULL_RETURN_MEMERR(sr);

  for (i = 0; i < n; i++) {
    reg = set->rs[i].reg;

    sr[i].state = SRS_DEAD;
    if (reg->optimize != OPTIMIZE_NONE) {
      if (reg->dist_max != INFINITE_LEN) {
        if (end - range > reg->dist_max)
          sch_range = (UChar* )range + reg->dist_max;
        else
          sch_range = (UChar* )end;

        if (forward_search(reg, str, end, s, sch_range, &low, &high, &low_prev)) {
          sr[i].state = SRS_LOW_HIGH;
          sr[i].low  = low;
          sr[i].high = high;
          sr[i].low_prev = low_prev;
          sr[i].sch_range = sch_range;
        }
      }
      else {
        sch_range = (UChar* )end;
        if (forward_search(reg, str, end, s, sch_range,
                           &low, &high, (UChar** )NULL)) {
          goto total_active;
        }
      }
    }
    else {
    total_active:
      sr[i].state    = SRS_ALL_RANGE;
      sr[i].low      = s;
      sr[i].high     = (UChar* )range;
      sr[i].low_prev = prev;
    }
  }

#define ACTIVATE_ALL_LOW_HIGH_SEARCH_THRESHOLD_LEN   500

  if (set->all_low_high != 0
      && range - start > ACTIVATE_ALL_LOW_HIGH_SEARCH_THRESHOLD_LEN) {
    do {
      int try_count = 0;
      for (i = 0; i < n; i++) {
        if (sr[i].state == SRS_DEAD) continue;

        if (s <  sr[i].low) continue;
        if (s >= sr[i].high) {
          if (forward_search(set->rs[i].reg, str, end, s, sr[i].sch_range,
                             &low, &high, &low_prev) != 0) {
            sr[i].low      = low;
            sr[i].high     = high;
            sr[i].low_prev = low_prev;
            if (s < low) continue;
          }
          else {
            sr[i].state = SRS_DEAD;
            continue;
          }
        }

        reg = set->rs[i].reg;
        REGSET_MATCH_AND_RETURN_CHECK(orig_range);
        try_count++;
      } /* for (i) */

      if (s >= range) break;

      if (try_count == 0) {
        low = (UChar* )range;
        for (i = 0; i < n; i++) {
          if (sr[i].state == SRS_LOW_HIGH && low > sr[i].low) {
            low = sr[i].low;
            low_prev = sr[i].low_prev;
          }
        }
        if (low == range) break;

        s = low;
        prev = low_prev;
      }
      else {
        prev = s;
        s += enclen(enc, s);
      }
    } while (1);
  }
  else {
    int prev_is_newline = 1;
    do {
      for (i = 0; i < n; i++) {
        if (sr[i].state == SRS_DEAD) continue;
        if (sr[i].state == SRS_LOW_HIGH) {
          if (s <  sr[i].low) continue;
          if (s >= sr[i].high) {
            if (forward_search(set->rs[i].reg, str, end, s, sr[i].sch_range,
                               &low, &high, &low_prev) != 0) {
              sr[i].low      = low;
              sr[i].high     = high;
              /* sr[i].low_prev = low_prev; */
              if (s < low) continue;
            }
            else {
              sr[i].state = SRS_DEAD;
              continue;
            }
          }
        }

        reg = set->rs[i].reg;
        if ((reg->anchor & ANCR_ANYCHAR_INF) == 0 || prev_is_newline != 0) {
          REGSET_MATCH_AND_RETURN_CHECK(orig_range);
        }
      }

      if (s >= range) break;

      if (set->anychar_inf != 0)
        prev_is_newline = ONIGENC_IS_MBC_NEWLINE(set->enc, s, end);

      prev = s;
      s += enclen(enc, s);
    } while (1);
  }

  xfree(sr);
  return ONIG_MISMATCH;

 finish:
  xfree(sr);
  return r;

 match:
  xfree(sr);
  *rmatch_pos = (int )(s - str);
  return i;
}

static inline int
regset_search_body_regex_lead(OnigRegSet* set,
              const UChar* str, const UChar* end,
              const UChar* start, const UChar* orig_range, OnigRegSetLead lead,
              OnigOptionType option, OnigMatchParam* mps[], int* rmatch_pos)
{
  int r;
  int i;
  int n;
  int match_index;
  const UChar* ep;
  regex_t* reg;
  OnigRegion* region;

  n = set->n;

  match_index = ONIG_MISMATCH;
  ep = orig_range;
  for (i = 0; i < n; i++) {
    reg    = set->rs[i].reg;
    region = set->rs[i].region;
    r = search_in_range(reg, str, end, start, ep, orig_range, region, option, mps[i]);
    if (r > 0) {
      if (str + r < ep) {
        match_index = i;
        *rmatch_pos = r;
        if (lead == ONIG_REGSET_PRIORITY_TO_REGEX_ORDER)
          break;

        ep = str + r;
      }
    }
    else if (r == 0) {
      match_index = i;
      *rmatch_pos = r;
      break;
    }
  }

  return match_index;
}

extern int
onig_regset_search_with_param(OnigRegSet* set,
           const UChar* str, const UChar* end,
           const UChar* start, const UChar* range,
           OnigRegSetLead lead, OnigOptionType option, OnigMatchParam* mps[],
           int* rmatch_pos)
{
  int r;
  int i;
  UChar *s, *prev;
  regex_t* reg;
  OnigEncoding enc;
  OnigRegion* region;
  MatchArg* msas;
  const UChar *orig_start = start;
  const UChar *orig_range = range;

  if (set->n == 0)
    return ONIG_MISMATCH;

  if (IS_POSIX_REGION(option))
    return ONIGERR_INVALID_ARGUMENT;

  r = 0;
  enc = set->enc;
  msas = (MatchArg* )NULL;

  for (i = 0; i < set->n; i++) {
    reg    = set->rs[i].reg;
    region = set->rs[i].region;
    ADJUST_MATCH_PARAM(reg, mps[i]);
    if (IS_NOT_NULL(region)) {
      r = onig_region_resize_clear(region, reg->num_mem + 1);
      if (r != 0) goto finish_no_msa;
    }
  }

  if (start > end || start < str) goto mismatch_no_msa;
  if (str < end) {
    /* forward search only */
    if (range <= start)
      return ONIGERR_INVALID_ARGUMENT;
  }

  if (ONIG_IS_OPTION_ON(option, ONIG_OPTION_CHECK_VALIDITY_OF_STRING)) {
    if (! ONIGENC_IS_VALID_MBC_STRING(enc, str, end)) {
      r = ONIGERR_INVALID_WIDE_CHAR_VALUE;
      goto finish_no_msa;
    }
  }

  if (set->anchor != OPTIMIZE_NONE && str < end) {
    UChar *min_semi_end, *max_semi_end;

    if ((set->anchor & ANCR_BEGIN_POSITION) != 0) {
      /* search start-position only */
    begin_position:
      range = start + 1;
    }
    else if ((set->anchor & ANCR_BEGIN_BUF) != 0) {
      /* search str-position only */
      if (start != str) goto mismatch_no_msa;
      range = str + 1;
    }
    else if ((set->anchor & ANCR_END_BUF) != 0) {
      min_semi_end = max_semi_end = (UChar* )end;

    end_buf:
      if ((OnigLen )(max_semi_end - str) < set->anc_dmin)
        goto mismatch_no_msa;

      if ((OnigLen )(min_semi_end - start) > set->anc_dmax) {
        start = min_semi_end - set->anc_dmax;
        if (start < end)
          start = onigenc_get_right_adjust_char_head(enc, str, start);
      }
      if ((OnigLen )(max_semi_end - (range - 1)) < set->anc_dmin) {
        range = max_semi_end - set->anc_dmin + 1;
      }
      if (start > range) goto mismatch_no_msa;
    }
    else if ((set->anchor & ANCR_SEMI_END_BUF) != 0) {
      UChar* pre_end = ONIGENC_STEP_BACK(enc, str, end, 1);

      max_semi_end = (UChar* )end;
      if (ONIGENC_IS_MBC_NEWLINE(enc, pre_end, end)) {
        min_semi_end = pre_end;

#ifdef USE_CRNL_AS_LINE_TERMINATOR
        pre_end = ONIGENC_STEP_BACK(enc, str, pre_end, 1);
        if (IS_NOT_NULL(pre_end) &&
            ONIGENC_IS_MBC_CRNL(enc, pre_end, end)) {
          min_semi_end = pre_end;
        }
#endif
        if (min_semi_end > str && start <= min_semi_end) {
          goto end_buf;
        }
      }
      else {
        min_semi_end = (UChar* )end;
        goto end_buf;
      }
    }
    else if ((set->anchor & ANCR_ANYCHAR_INF_ML) != 0) {
      goto begin_position;
    }
  }
  else if (str == end) { /* empty string */
    start = end = str;
    s = (UChar* )start;
    prev = (UChar* )NULL;

    msas = (MatchArg* )xmalloc(sizeof(*msas) * set->n);
    CHECK_NULL_RETURN_MEMERR(msas);
    for (i = 0; i < set->n; i++) {
      reg = set->rs[i].reg;
      MATCH_ARG_INIT(msas[i], reg, option, set->rs[i].region, start, mps[i]);
    }
    for (i = 0; i < set->n; i++) {
      reg = set->rs[i].reg;
      if (reg->threshold_len == 0) {
        REGSET_MATCH_AND_RETURN_CHECK(end);
      }
    }

    goto mismatch;
  }

  if (lead == ONIG_REGSET_POSITION_LEAD) {
    msas = (MatchArg* )xmalloc(sizeof(*msas) * set->n);
    CHECK_NULL_RETURN_MEMERR(msas);

    for (i = 0; i < set->n; i++) {
      MATCH_ARG_INIT(msas[i], set->rs[i].reg, option, set->rs[i].region,
                     orig_start, mps[i]);
    }

    r = regset_search_body_position_lead(set, str, end, start, range,
                                         orig_range, option, msas, rmatch_pos);
  }
  else {
    r = regset_search_body_regex_lead(set, str, end, start, orig_range,
                                      lead, option, mps, rmatch_pos);
  }
  if (r < 0) goto finish;
  else       goto match2;

 mismatch:
  r = ONIG_MISMATCH;
 finish:
  for (i = 0; i < set->n; i++) {
    if (IS_NOT_NULL(msas))
      MATCH_ARG_FREE(msas[i]);
    if (IS_FIND_NOT_EMPTY(set->rs[i].reg->options) &&
        IS_NOT_NULL(set->rs[i].region)) {
      onig_region_clear(set->rs[i].region);
    }
  }
  if (IS_NOT_NULL(msas)) xfree(msas);
  return r;

 mismatch_no_msa:
  r = ONIG_MISMATCH;
 finish_no_msa:
  return r;

 match:
  *rmatch_pos = (int )(s - str);
 match2:
  for (i = 0; i < set->n; i++) {
    if (IS_NOT_NULL(msas))
      MATCH_ARG_FREE(msas[i]);
    if (IS_FIND_NOT_EMPTY(set->rs[i].reg->options) &&
        IS_NOT_NULL(set->rs[i].region)) {
      onig_region_clear(set->rs[i].region);
    }
  }
  if (IS_NOT_NULL(msas)) xfree(msas);
  return r; /* regex index */
}

extern int
onig_regset_search(OnigRegSet* set, const UChar* str, const UChar* end,
                   const UChar* start, const UChar* range,
                   OnigRegSetLead lead, OnigOptionType option, int* rmatch_pos)
{
  int r;
  int i;
  OnigMatchParam* mp;
  OnigMatchParam** mps;

  mps = (OnigMatchParam** )xmalloc((sizeof(OnigMatchParam*) + sizeof(OnigMatchParam)) * set->n);
  CHECK_NULL_RETURN_MEMERR(mps);

  mp = (OnigMatchParam* )(mps + set->n);

  for (i = 0; i < set->n; i++) {
    onig_initialize_match_param(mp + i);
    mps[i] = mp + i;
  }

  r = onig_regset_search_with_param(set, str, end, start, range, lead, option, mps,
                                    rmatch_pos);
  for (i = 0; i < set->n; i++)
    onig_free_match_param_content(mp + i);

  xfree(mps);

  return r;
}

static UChar*
slow_search(OnigEncoding enc, UChar* target, UChar* target_end,
            const UChar* text, const UChar* text_end, UChar* text_range)
{
  UChar *t, *p, *s, *end;

  end = (UChar* )text_end;
  end -= target_end - target - 1;
  if (end > text_range)
    end = text_range;

  s = (UChar* )text;

  while (s < end) {
    if (*s == *target) {
      p = s + 1;
      t = target + 1;
      while (t < target_end) {
        if (*t != *p++)
          break;
        t++;
      }
      if (t == target_end)
        return s;
    }
    s += enclen(enc, s);
  }

  return (UChar* )NULL;
}

static int
str_lower_case_match(OnigEncoding enc, int case_fold_flag,
                     const UChar* t, const UChar* tend,
                     const UChar* p, const UChar* end)
{
  int lowlen;
  UChar *q, lowbuf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

  while (t < tend) {
    if (p >= end) return 0;
    lowlen = ONIGENC_MBC_CASE_FOLD(enc, case_fold_flag, &p, end, lowbuf);
    q = lowbuf;
    while (lowlen > 0) {
      if (t >= tend)    return 0;
      if (*t++ != *q++) return 0;
      lowlen--;
    }
  }

  return 1;
}

static UChar*
slow_search_ic(OnigEncoding enc, int case_fold_flag,
               UChar* target, UChar* target_end,
               const UChar* text, const UChar* text_end, UChar* text_range)
{
  UChar *s;

  s = (UChar* )text;

  while (s < text_range) {
    if (str_lower_case_match(enc, case_fold_flag, target, target_end,
                             s, text_end))
      return s;

    s += enclen(enc, s);
  }

  return (UChar* )NULL;
}

static UChar*
slow_search_backward(OnigEncoding enc, UChar* target, UChar* target_end,
                     const UChar* text, const UChar* adjust_text,
                     const UChar* text_end, const UChar* text_start)
{
  UChar *t, *p, *s;

  s = (UChar* )text_end;
  s -= (target_end - target);
  if (s > text_start)
    s = (UChar* )text_start;
  else
    s = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, adjust_text, s);

  while (s >= text) {
    if (*s == *target) {
      p = s + 1;
      t = target + 1;
      while (t < target_end) {
        if (*t != *p++)
          break;
        t++;
      }
      if (t == target_end)
        return s;
    }
    s = (UChar* )onigenc_get_prev_char_head(enc, adjust_text, s);
  }

  return (UChar* )NULL;
}

static UChar*
slow_search_backward_ic(OnigEncoding enc, int case_fold_flag,
                        UChar* target, UChar* target_end,
                        const UChar* text, const UChar* adjust_text,
                        const UChar* text_end, const UChar* text_start)
{
  UChar *s;

  s = (UChar* )text_end;
  s -= (target_end - target);
  if (s > text_start)
    s = (UChar* )text_start;
  else
    s = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, adjust_text, s);

  while (s >= text) {
    if (str_lower_case_match(enc, case_fold_flag,
                             target, target_end, s, text_end))
      return s;

    s = (UChar* )onigenc_get_prev_char_head(enc, adjust_text, s);
  }

  return (UChar* )NULL;
}


static UChar*
sunday_quick_search_step_forward(regex_t* reg,
                                 const UChar* target, const UChar* target_end,
                                 const UChar* text, const UChar* text_end,
                                 const UChar* text_range)
{
  const UChar *s, *se, *t, *p, *end;
  const UChar *tail;
  int skip, tlen1;
  int map_offset;
  OnigEncoding enc;

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr,
          "sunday_quick_search_step_forward: text: %p, text_end: %p, text_range: %p\n", text, text_end, text_range);
#endif

  enc = reg->enc;

  tail = target_end - 1;
  tlen1 = (int )(tail - target);
  end = text_range;
  if (end + tlen1 > text_end)
    end = text_end - tlen1;

  map_offset = reg->map_offset;
  s = text;

  while (s < end) {
    p = se = s + tlen1;
    t = tail;
    while (*p == *t) {
      if (t == target) return (UChar* )s;
      p--; t--;
    }
    if (se + map_offset >= text_end) break;
    skip = reg->map[*(se + map_offset)];
#if 0
    t = s;
    do {
      s += enclen(enc, s);
    } while ((s - t) < skip && s < end);
#else
    s += skip;
    if (s < end)
      s = onigenc_get_right_adjust_char_head(enc, text, s);
#endif
  }

  return (UChar* )NULL;
}

static UChar*
sunday_quick_search(regex_t* reg, const UChar* target, const UChar* target_end,
                    const UChar* text, const UChar* text_end,
                    const UChar* text_range)
{
  const UChar *s, *t, *p, *end;
  const UChar *tail;
  int map_offset;

  end = text_range + (target_end - target);
  if (end > text_end)
    end = text_end;

  map_offset = reg->map_offset;
  tail = target_end - 1;
  s = text + (tail - target);

  while (s < end) {
    p = s;
    t = tail;
    while (*p == *t) {
      if (t == target) return (UChar* )p;
      p--; t--;
    }
    if (s + map_offset >= text_end) break;
    s += reg->map[*(s + map_offset)];
  }

  return (UChar* )NULL;
}

static UChar*
map_search(OnigEncoding enc, UChar map[],
           const UChar* text, const UChar* text_range)
{
  const UChar *s = text;

  while (s < text_range) {
    if (map[*s]) return (UChar* )s;

    s += enclen(enc, s);
  }
  return (UChar* )NULL;
}

static UChar*
map_search_backward(OnigEncoding enc, UChar map[],
                    const UChar* text, const UChar* adjust_text,
                    const UChar* text_start)
{
  const UChar *s = text_start;

  while (s >= text) {
    if (map[*s]) return (UChar* )s;

    s = onigenc_get_prev_char_head(enc, adjust_text, s);
  }
  return (UChar* )NULL;
}
extern int
onig_match(regex_t* reg, const UChar* str, const UChar* end, const UChar* at,
           OnigRegion* region, OnigOptionType option)
{
  int r;
  OnigMatchParam mp;

  onig_initialize_match_param(&mp);
  r = onig_match_with_param(reg, str, end, at, region, option, &mp);
  onig_free_match_param_content(&mp);
  return r;
}

extern int
onig_match_with_param(regex_t* reg, const UChar* str, const UChar* end,
                      const UChar* at, OnigRegion* region, OnigOptionType option,
                      OnigMatchParam* mp)
{
  int r;
  UChar *prev;
  MatchArg msa;

  ADJUST_MATCH_PARAM(reg, mp);
  MATCH_ARG_INIT(msa, reg, option, region, at, mp);
  if (region
#ifdef USE_POSIX_API_REGION_OPTION
      && !IS_POSIX_REGION(option)
#endif
      ) {
    r = onig_region_resize_clear(region, reg->num_mem + 1);
  }
  else
    r = 0;

  if (r == 0) {
    if (ONIG_IS_OPTION_ON(option, ONIG_OPTION_CHECK_VALIDITY_OF_STRING)) {
      if (! ONIGENC_IS_VALID_MBC_STRING(reg->enc, str, end)) {
        r = ONIGERR_INVALID_WIDE_CHAR_VALUE;
        goto end;
      }
    }

    prev = (UChar* )onigenc_get_prev_char_head(reg->enc, str, at);
    r = match_at(reg, str, end, end, at, prev, &msa);
  }

 end:
  MATCH_ARG_FREE(msa);
  return r;
}

static int
forward_search(regex_t* reg, const UChar* str, const UChar* end, UChar* start,
               UChar* range, UChar** low, UChar** high, UChar** low_prev)
{
  UChar *p, *pprev = (UChar* )NULL;

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "forward_search: str: %p, end: %p, start: %p, range: %p\n",
          str, end, start, range);
#endif

  p = start;
  if (reg->dist_min != 0) {
    if (end - p <= reg->dist_min)
      return 0; /* fail */

    if (ONIGENC_IS_SINGLEBYTE(reg->enc)) {
      p += reg->dist_min;
    }
    else {
      UChar *q = p + reg->dist_min;
      while (p < q) p += enclen(reg->enc, p);
    }
  }

 retry:
  switch (reg->optimize) {
  case OPTIMIZE_STR:
    p = slow_search(reg->enc, reg->exact, reg->exact_end, p, end, range);
    break;
  case OPTIMIZE_STR_CASE_FOLD:
    p = slow_search_ic(reg->enc, reg->case_fold_flag,
                       reg->exact, reg->exact_end, p, end, range);
    break;

  case OPTIMIZE_STR_FAST:
    p = sunday_quick_search(reg, reg->exact, reg->exact_end, p, end, range);
    break;

  case OPTIMIZE_STR_FAST_STEP_FORWARD:
    p = sunday_quick_search_step_forward(reg, reg->exact, reg->exact_end,
                                         p, end, range);
    break;

  case OPTIMIZE_MAP:
    p = map_search(reg->enc, reg->map, p, range);
    break;
  }

  if (p && p < range) {
    if (p - start < reg->dist_min) {
    retry_gate:
      pprev = p;
      p += enclen(reg->enc, p);
      goto retry;
    }

    if (reg->sub_anchor) {
      UChar* prev;

      switch (reg->sub_anchor) {
      case ANCR_BEGIN_LINE:
        if (!ON_STR_BEGIN(p)) {
          prev = onigenc_get_prev_char_head(reg->enc, (pprev ? pprev : str), p);
          if (!ONIGENC_IS_MBC_NEWLINE(reg->enc, prev, end))
            goto retry_gate;
        }
        break;

      case ANCR_END_LINE:
        if (ON_STR_END(p)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
          prev = (UChar* )onigenc_get_prev_char_head(reg->enc,
                                                     (pprev ? pprev : str), p);
          if (prev && ONIGENC_IS_MBC_NEWLINE(reg->enc, prev, end))
            goto retry_gate;
#endif
        }
        else if (! ONIGENC_IS_MBC_NEWLINE(reg->enc, p, end)
#ifdef USE_CRNL_AS_LINE_TERMINATOR
                 && ! ONIGENC_IS_MBC_CRNL(reg->enc, p, end)
#endif
                 )
          goto retry_gate;

        break;
      }
    }

    if (reg->dist_max == 0) {
      *low = p;
      if (low_prev) {
        if (*low > start)
          *low_prev = onigenc_get_prev_char_head(reg->enc, start, p);
        else
          *low_prev = onigenc_get_prev_char_head(reg->enc,
                                                 (pprev ? pprev : str), p);
      }
      *high = p;
    }
    else {
      if (reg->dist_max != INFINITE_LEN) {
        if (p - str < reg->dist_max) {
          *low = (UChar* )str;
          if (low_prev)
            *low_prev = onigenc_get_prev_char_head(reg->enc, str, *low);
        }
        else {
          *low = p - reg->dist_max;
          if (*low > start) {
            *low = onigenc_get_right_adjust_char_head_with_prev(reg->enc, start,
                                                 *low, (const UChar** )low_prev);
          }
          else {
            if (low_prev)
              *low_prev = onigenc_get_prev_char_head(reg->enc,
                                                     (pprev ? pprev : str), *low);
          }
        }
      }
      /* no needs to adjust *high, *high is used as range check only */
      if (p - str < reg->dist_min)
        *high = (UChar* )str;
      else
        *high = p - reg->dist_min;
    }

#ifdef ONIG_DEBUG_SEARCH
    fprintf(stderr,
            "forward_search success: low: %d, high: %d, dmin: %u, dmax: %u\n",
            (int )(*low - str), (int )(*high - str),
            reg->dist_min, reg->dist_max);
#endif
    return 1; /* success */
  }

  return 0; /* fail */
}


static int
backward_search(regex_t* reg, const UChar* str, const UChar* end, UChar* s,
                const UChar* range, UChar* adjrange, UChar** low, UChar** high)
{
  UChar *p;

  p = s;

 retry:
  switch (reg->optimize) {
  case OPTIMIZE_STR:
  exact_method:
    p = slow_search_backward(reg->enc, reg->exact, reg->exact_end,
                             range, adjrange, end, p);
    break;

  case OPTIMIZE_STR_CASE_FOLD:
    p = slow_search_backward_ic(reg->enc, reg->case_fold_flag,
                                reg->exact, reg->exact_end,
                                range, adjrange, end, p);
    break;

  case OPTIMIZE_STR_FAST:
  case OPTIMIZE_STR_FAST_STEP_FORWARD:
    goto exact_method;
    break;

  case OPTIMIZE_MAP:
    p = map_search_backward(reg->enc, reg->map, range, adjrange, p);
    break;
  }

  if (p) {
    if (reg->sub_anchor) {
      UChar* prev;

      switch (reg->sub_anchor) {
      case ANCR_BEGIN_LINE:
        if (!ON_STR_BEGIN(p)) {
          prev = onigenc_get_prev_char_head(reg->enc, str, p);
          if (IS_NOT_NULL(prev) && !ONIGENC_IS_MBC_NEWLINE(reg->enc, prev, end)) {
            p = prev;
            goto retry;
          }
        }
        break;

      case ANCR_END_LINE:
        if (ON_STR_END(p)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
          prev = onigenc_get_prev_char_head(reg->enc, adjrange, p);
          if (IS_NULL(prev)) goto fail;
          if (ONIGENC_IS_MBC_NEWLINE(reg->enc, prev, end)) {
            p = prev;
            goto retry;
          }
#endif
        }
        else if (! ONIGENC_IS_MBC_NEWLINE(reg->enc, p, end)
#ifdef USE_CRNL_AS_LINE_TERMINATOR
                 && ! ONIGENC_IS_MBC_CRNL(reg->enc, p, end)
#endif
                 ) {
          p = onigenc_get_prev_char_head(reg->enc, adjrange, p);
          if (IS_NULL(p)) goto fail;
          goto retry;
        }
        break;
      }
    }

    if (reg->dist_max != INFINITE_LEN) {
      if (p - str < reg->dist_max)
        *low = (UChar* )str;
      else
        *low = p - reg->dist_max;

      if (reg->dist_min != 0) {
        if (p - str < reg->dist_min)
          *high = (UChar* )str;
        else
          *high = p - reg->dist_min;
      }
      else {
        *high = p;
      }

      *high = onigenc_get_right_adjust_char_head(reg->enc, adjrange, *high);
    }

#ifdef ONIG_DEBUG_SEARCH
    fprintf(stderr, "backward_search: low: %d, high: %d\n",
            (int )(*low - str), (int )(*high - str));
#endif
    return 1; /* success */
  }

 fail:
#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "backward_search: fail.\n");
#endif
  return 0; /* fail */
}


extern int
onig_search(regex_t* reg, const UChar* str, const UChar* end,
            const UChar* start, const UChar* range, OnigRegion* region,
            OnigOptionType option)
{
  int r;
  OnigMatchParam mp;
  const UChar* data_range;

  onig_initialize_match_param(&mp);

  /* The following is an expanded code of onig_search_with_param()  */
  if (range > start)
    data_range = range;
  else
    data_range = end;

  r = search_in_range(reg, str, end, start, range, data_range, region,
                      option, &mp);

  onig_free_match_param_content(&mp);
  return r;

}

static int
search_in_range(regex_t* reg, const UChar* str, const UChar* end,
                const UChar* start, const UChar* range, /* match start range */
                const UChar* data_range, /* subject string range */
                OnigRegion* region,
                OnigOptionType option, OnigMatchParam* mp)
{
  int r;
  UChar *s, *prev;
  MatchArg msa;
  const UChar *orig_start = start;

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr,
     "onig_search (entry point): str: %p, end: %d, start: %d, range: %d\n",
     str, (int )(end - str), (int )(start - str), (int )(range - str));
#endif

  ADJUST_MATCH_PARAM(reg, mp);

  if (region
#ifdef USE_POSIX_API_REGION_OPTION
      && !IS_POSIX_REGION(option)
#endif
      ) {
    r = onig_region_resize_clear(region, reg->num_mem + 1);
    if (r != 0) goto finish_no_msa;
  }

  if (start > end || start < str) goto mismatch_no_msa;

  if (ONIG_IS_OPTION_ON(option, ONIG_OPTION_CHECK_VALIDITY_OF_STRING)) {
    if (! ONIGENC_IS_VALID_MBC_STRING(reg->enc, str, end)) {
      r = ONIGERR_INVALID_WIDE_CHAR_VALUE;
      goto finish_no_msa;
    }
  }


#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
#define MATCH_AND_RETURN_CHECK(upper_range) \
  r = match_at(reg, str, end, (upper_range), s, prev, &msa); \
  if (r != ONIG_MISMATCH) {\
    if (r >= 0) {\
      if (! IS_FIND_LONGEST(reg->options)) {\
        goto match;\
      }\
    }\
    else goto finish; /* error */ \
  }
#else
#define MATCH_AND_RETURN_CHECK(upper_range) \
  r = match_at(reg, str, end, (upper_range), s, prev, &msa); \
  if (r != ONIG_MISMATCH) {\
    if (r >= 0) {\
      goto match;\
    }\
    else goto finish; /* error */ \
  }
#endif /* USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE */


  /* anchor optimize: resume search range */
  if (reg->anchor != 0 && str < end) {
    UChar *min_semi_end, *max_semi_end;

    if (reg->anchor & ANCR_BEGIN_POSITION) {
      /* search start-position only */
    begin_position:
      if (range > start)
        range = start + 1;
      else
        range = start;
    }
    else if (reg->anchor & ANCR_BEGIN_BUF) {
      /* search str-position only */
      if (range > start) {
        if (start != str) goto mismatch_no_msa;
        range = str + 1;
      }
      else {
        if (range <= str) {
          start = str;
          range = str;
        }
        else
          goto mismatch_no_msa;
      }
    }
    else if (reg->anchor & ANCR_END_BUF) {
      min_semi_end = max_semi_end = (UChar* )end;

    end_buf:
      if ((OnigLen )(max_semi_end - str) < reg->anc_dist_min)
        goto mismatch_no_msa;

      if (range > start) {
        if (reg->anc_dist_max != INFINITE_LEN &&
            min_semi_end - start > reg->anc_dist_max) {
          start = min_semi_end - reg->anc_dist_max;
          if (start < end)
            start = onigenc_get_right_adjust_char_head(reg->enc, str, start);
        }
        if (max_semi_end - (range - 1) < reg->anc_dist_min) {
          if (max_semi_end - str + 1 < reg->anc_dist_min)
            goto mismatch_no_msa;
          else
            range = max_semi_end - reg->anc_dist_min + 1;
        }

        if (start > range) goto mismatch_no_msa;
        /* If start == range, match with empty at end.
           Backward search is used. */
      }
      else {
        if (reg->anc_dist_max != INFINITE_LEN &&
            min_semi_end - range > reg->anc_dist_max) {
          range = min_semi_end - reg->anc_dist_max;
        }
        if (max_semi_end - start < reg->anc_dist_min) {
          if (max_semi_end - str < reg->anc_dist_min)
            goto mismatch_no_msa;
          else {
            start = max_semi_end - reg->anc_dist_min;
            start = ONIGENC_LEFT_ADJUST_CHAR_HEAD(reg->enc, str, start);
          }
        }
        if (range > start) goto mismatch_no_msa;
      }
    }
    else if (reg->anchor & ANCR_SEMI_END_BUF) {
      UChar* pre_end = ONIGENC_STEP_BACK(reg->enc, str, end, 1);

      max_semi_end = (UChar* )end;
      if (ONIGENC_IS_MBC_NEWLINE(reg->enc, pre_end, end)) {
        min_semi_end = pre_end;

#ifdef USE_CRNL_AS_LINE_TERMINATOR
        pre_end = ONIGENC_STEP_BACK(reg->enc, str, pre_end, 1);
        if (IS_NOT_NULL(pre_end) &&
            ONIGENC_IS_MBC_CRNL(reg->enc, pre_end, end)) {
          min_semi_end = pre_end;
        }
#endif
        if (min_semi_end > str && start <= min_semi_end) {
          goto end_buf;
        }
      }
      else {
        min_semi_end = (UChar* )end;
        goto end_buf;
      }
    }
    else if ((reg->anchor & ANCR_ANYCHAR_INF_ML)) {
      goto begin_position;
    }
  }
  else if (str == end) { /* empty string */
    static const UChar* address_for_empty_string = (UChar* )"";

#ifdef ONIG_DEBUG_SEARCH
    fprintf(stderr, "onig_search: empty string.\n");
#endif

    if (reg->threshold_len == 0) {
      start = end = str = address_for_empty_string;
      s = (UChar* )start;
      prev = (UChar* )NULL;

      MATCH_ARG_INIT(msa, reg, option, region, start, mp);
      MATCH_AND_RETURN_CHECK(end);
      goto mismatch;
    }
    goto mismatch_no_msa;
  }

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "onig_search(apply anchor): end: %d, start: %d, range: %d\n",
          (int )(end - str), (int )(start - str), (int )(range - str));
#endif

  MATCH_ARG_INIT(msa, reg, option, region, orig_start, mp);

  s = (UChar* )start;
  if (range > start) {   /* forward search */
    if (s > str)
      prev = onigenc_get_prev_char_head(reg->enc, str, s);
    else
      prev = (UChar* )NULL;

    if (reg->optimize != OPTIMIZE_NONE) {
      UChar *sch_range, *low, *high, *low_prev;

      if (reg->dist_max != 0) {
        if (reg->dist_max == INFINITE_LEN)
          sch_range = (UChar* )end;
        else {
          if ((end - range) < reg->dist_max)
            sch_range = (UChar* )end;
          else {
            sch_range = (UChar* )range + reg->dist_max;
          }
        }
      }
      else
        sch_range = (UChar* )range;

      if ((end - start) < reg->threshold_len)
        goto mismatch;

      if (reg->dist_max != INFINITE_LEN) {
        do {
          if (! forward_search(reg, str, end, s, sch_range, &low, &high,
                               &low_prev)) goto mismatch;
          if (s < low) {
            s    = low;
            prev = low_prev;
          }
          while (s <= high) {
            MATCH_AND_RETURN_CHECK(data_range);
            prev = s;
            s += enclen(reg->enc, s);
          }
        } while (s < range);
        goto mismatch;
      }
      else { /* check only. */
        if (! forward_search(reg, str, end, s, sch_range, &low, &high,
                             (UChar** )NULL)) goto mismatch;

        if ((reg->anchor & ANCR_ANYCHAR_INF) != 0) {
          do {
            MATCH_AND_RETURN_CHECK(data_range);
            prev = s;
            s += enclen(reg->enc, s);

            if ((reg->anchor & (ANCR_LOOK_BEHIND | ANCR_PREC_READ_NOT)) == 0) {
              while (!ONIGENC_IS_MBC_NEWLINE(reg->enc, prev, end) && s < range) {
                prev = s;
                s += enclen(reg->enc, s);
              }
            }
          } while (s < range);
          goto mismatch;
        }
      }
    }

    do {
      MATCH_AND_RETURN_CHECK(data_range);
      prev = s;
      s += enclen(reg->enc, s);
    } while (s < range);

    if (s == range) { /* because empty match with /$/. */
      MATCH_AND_RETURN_CHECK(data_range);
    }
  }
  else {  /* backward search */
    if (range < str) goto mismatch;

    if (orig_start < end)
      orig_start += enclen(reg->enc, orig_start); /* is upper range */

    if (reg->optimize != OPTIMIZE_NONE) {
      UChar *low, *high, *adjrange, *sch_start;
      const UChar *min_range;

      if ((end - range) < reg->threshold_len) goto mismatch;

      if (range < end)
        adjrange = ONIGENC_LEFT_ADJUST_CHAR_HEAD(reg->enc, str, range);
      else
        adjrange = (UChar* )end;

      if (end - range > reg->dist_min)
        min_range = range + reg->dist_min;
      else
        min_range = end;

      if (reg->dist_max != INFINITE_LEN) {
        do {
          if (end - s > reg->dist_max)
            sch_start = s + reg->dist_max;
          else {
            sch_start = onigenc_get_prev_char_head(reg->enc, str, end);
          }

          if (backward_search(reg, str, end, sch_start, min_range, adjrange,
                              &low, &high) <= 0)
            goto mismatch;

          if (s > high)
            s = high;

          while (s >= low) {
            prev = onigenc_get_prev_char_head(reg->enc, str, s);
            MATCH_AND_RETURN_CHECK(orig_start);
            s = prev;
          }
        } while (s >= range);
        goto mismatch;
      }
      else { /* check only. */
        sch_start = onigenc_get_prev_char_head(reg->enc, str, end);

        if (backward_search(reg, str, end, sch_start, min_range, adjrange,
                            &low, &high) <= 0) goto mismatch;
      }
    }

    do {
      prev = onigenc_get_prev_char_head(reg->enc, str, s);
      MATCH_AND_RETURN_CHECK(orig_start);
      s = prev;
    } while (s >= range);
  }

 mismatch:
#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
  if (IS_FIND_LONGEST(reg->options)) {
    if (msa.best_len >= 0) {
      s = msa.best_s;
      goto match;
    }
  }
#endif
  r = ONIG_MISMATCH;

 finish:
  MATCH_ARG_FREE(msa);

  /* If result is mismatch and no FIND_NOT_EMPTY option,
     then the region is not set in match_at(). */
  if (IS_FIND_NOT_EMPTY(reg->options) && region
#ifdef USE_POSIX_API_REGION_OPTION
      && !IS_POSIX_REGION(option)
#endif
      ) {
    onig_region_clear(region);
  }

#ifdef ONIG_DEBUG
  if (r != ONIG_MISMATCH)
    fprintf(stderr, "onig_search: error %d\n", r);
#endif
  return r;

 mismatch_no_msa:
  r = ONIG_MISMATCH;
 finish_no_msa:
#ifdef ONIG_DEBUG
  if (r != ONIG_MISMATCH)
    fprintf(stderr, "onig_search: error %d\n", r);
#endif
  return r;

 match:
  MATCH_ARG_FREE(msa);
  return (int )(s - str);
}

extern int
onig_search_with_param(regex_t* reg, const UChar* str, const UChar* end,
                       const UChar* start, const UChar* range, OnigRegion* region,
                       OnigOptionType option, OnigMatchParam* mp)
{
  const UChar* data_range;

  if (range > start)
    data_range = range;
  else
    data_range = end;

  return search_in_range(reg, str, end, start, range, data_range, region,
                         option, mp);
}

extern int
onig_scan(regex_t* reg, const UChar* str, const UChar* end,
          OnigRegion* region, OnigOptionType option,
          int (*scan_callback)(int, int, OnigRegion*, void*),
          void* callback_arg)
{
  int r;
  int n;
  int rs;
  const UChar* start;

  if (ONIG_IS_OPTION_ON(option, ONIG_OPTION_CHECK_VALIDITY_OF_STRING)) {
    if (! ONIGENC_IS_VALID_MBC_STRING(reg->enc, str, end))
      return ONIGERR_INVALID_WIDE_CHAR_VALUE;

    ONIG_OPTION_OFF(option, ONIG_OPTION_CHECK_VALIDITY_OF_STRING);
  }

  n = 0;
  start = str;
  while (1) {
    r = onig_search(reg, str, end, start, end, region, option);
    if (r >= 0) {
      rs = scan_callback(n, r, region, callback_arg);
      n++;
      if (rs != 0)
        return rs;

      if (region->end[0] == start - str) {
        if (start >= end) break;
        start += enclen(reg->enc, start);
      }
      else
        start = str + region->end[0];

      if (start > end)
        break;
    }
    else if (r == ONIG_MISMATCH) {
      break;
    }
    else { /* error */
      return r;
    }
  }

  return n;
}

extern OnigEncoding
onig_get_encoding(regex_t* reg)
{
  return reg->enc;
}

extern OnigOptionType
onig_get_options(regex_t* reg)
{
  return reg->options;
}

extern  OnigCaseFoldType
onig_get_case_fold_flag(regex_t* reg)
{
  return reg->case_fold_flag;
}

extern OnigSyntaxType*
onig_get_syntax(regex_t* reg)
{
  return reg->syntax;
}

extern int
onig_number_of_captures(regex_t* reg)
{
  return reg->num_mem;
}

extern int
onig_number_of_capture_histories(regex_t* reg)
{
#ifdef USE_CAPTURE_HISTORY
  int i, n;

  n = 0;
  for (i = 0; i <= ONIG_MAX_CAPTURE_HISTORY_GROUP; i++) {
    if (MEM_STATUS_AT(reg->capture_history, i) != 0)
      n++;
  }
  return n;
#else
  return 0;
#endif
}

extern void
onig_copy_encoding(OnigEncoding to, OnigEncoding from)
{
  *to = *from;
}

extern int
onig_regset_new(OnigRegSet** rset, int n, regex_t* regs[])
{
#define REGSET_INITIAL_ALLOC_SIZE   10

  int i;
  int r;
  int alloc;
  OnigRegSet* set;
  RR* rs;

  *rset = 0;

  set = (OnigRegSet* )xmalloc(sizeof(*set));
  CHECK_NULL_RETURN_MEMERR(set);

  alloc = n > REGSET_INITIAL_ALLOC_SIZE ? n : REGSET_INITIAL_ALLOC_SIZE;
  rs = (RR* )xmalloc(sizeof(set->rs[0]) * alloc);
  if (IS_NULL(rs)) {
    xfree(set);
    return ONIGERR_MEMORY;
  }

  set->rs    = rs;
  set->n     = 0;
  set->alloc = alloc;

  for (i = 0; i < n; i++) {
    regex_t* reg = regs[i];

    r = onig_regset_add(set, reg);
    if (r != 0) {
      for (i = 0; i < set->n; i++) {
        OnigRegion* region = set->rs[i].region;
        if (IS_NOT_NULL(region))
          onig_region_free(region, 1);
      }
      xfree(set->rs);
      xfree(set);
      return r;
    }
  }

  *rset = set;
  return 0;
}

static void
update_regset_by_reg(OnigRegSet* set, regex_t* reg)
{
  if (set->n == 1) {
    set->enc          = reg->enc;
    set->anchor       = reg->anchor;
    set->anc_dmin     = reg->anc_dist_min;
    set->anc_dmax     = reg->anc_dist_max;
    set->all_low_high =
      (reg->optimize == OPTIMIZE_NONE || reg->dist_max == INFINITE_LEN) ? 0 : 1;
    set->anychar_inf  = (reg->anchor & ANCR_ANYCHAR_INF) != 0 ? 1 : 0;
  }
  else {
    int anchor;

    anchor = set->anchor & reg->anchor;
    if (anchor != 0) {
      OnigLen anc_dmin;
      OnigLen anc_dmax;

      anc_dmin = set->anc_dmin;
      anc_dmax = set->anc_dmax;
      if (anc_dmin > reg->anc_dist_min) anc_dmin = reg->anc_dist_min;
      if (anc_dmax < reg->anc_dist_max) anc_dmax = reg->anc_dist_max;
      set->anc_dmin = anc_dmin;
      set->anc_dmax = anc_dmax;
    }

    set->anchor = anchor;

    if (reg->optimize == OPTIMIZE_NONE || reg->dist_max == INFINITE_LEN)
      set->all_low_high = 0;

    if ((reg->anchor & ANCR_ANYCHAR_INF) != 0)
      set->anychar_inf = 1;
  }
}

extern int
onig_regset_add(OnigRegSet* set, regex_t* reg)
{
  OnigRegion* region;

  if (IS_FIND_LONGEST(reg->options))
    return ONIGERR_INVALID_ARGUMENT;

  if (set->n != 0 && reg->enc != set->enc)
    return ONIGERR_INVALID_ARGUMENT;

  if (set->n >= set->alloc) {
    RR* nrs;
    int new_alloc;

    new_alloc = set->alloc * 2;
    nrs = (RR* )xrealloc(set->rs, sizeof(set->rs[0]) * new_alloc);
    CHECK_NULL_RETURN_MEMERR(nrs);

    set->rs    = nrs;
    set->alloc = new_alloc;
  }

  region = onig_region_new();
  CHECK_NULL_RETURN_MEMERR(region);

  set->rs[set->n].reg    = reg;
  set->rs[set->n].region = region;
  set->n++;

  update_regset_by_reg(set, reg);
  return 0;
}

extern int
onig_regset_replace(OnigRegSet* set, int at, regex_t* reg)
{
  int i;

  if (at < 0 || at >= set->n)
    return ONIGERR_INVALID_ARGUMENT;

  if (IS_NULL(reg)) {
    onig_region_free(set->rs[at].region, 1);
    for (i = at; i < set->n - 1; i++) {
      set->rs[i].reg    = set->rs[i+1].reg;
      set->rs[i].region = set->rs[i+1].region;
    }
    set->n--;
  }
  else {
    if (IS_FIND_LONGEST(reg->options))
      return ONIGERR_INVALID_ARGUMENT;

    if (set->n > 1 && reg->enc != set->enc)
      return ONIGERR_INVALID_ARGUMENT;

    set->rs[at].reg = reg;
  }

  for (i = 0; i < set->n; i++)
    update_regset_by_reg(set, set->rs[i].reg);

  return 0;
}

extern void
onig_regset_free(OnigRegSet* set)
{
  int i;

  for (i = 0; i < set->n; i++) {
    regex_t* reg;
    OnigRegion* region;

    reg    = set->rs[i].reg;
    region = set->rs[i].region;
    onig_free(reg);
    if (IS_NOT_NULL(region))
      onig_region_free(region, 1);
  }

  xfree(set->rs);
  xfree(set);
}

extern int
onig_regset_number_of_regex(OnigRegSet* set)
{
  return set->n;
}

extern regex_t*
onig_regset_get_regex(OnigRegSet* set, int at)
{
  if (at < 0 || at >= set->n)
    return (regex_t* )0;

  return set->rs[at].reg;
}

extern OnigRegion*
onig_regset_get_region(OnigRegSet* set, int at)
{
  if (at < 0 || at >= set->n)
    return (OnigRegion* )0;

  return set->rs[at].region;
}


#ifdef USE_DIRECT_THREADED_CODE
extern int
onig_init_for_match_at(regex_t* reg)
{
  return match_at(reg, (const UChar* )NULL, (const UChar* )NULL,
                  (const UChar* )NULL, (const UChar* )NULL, (UChar* )NULL,
                  (MatchArg* )NULL);
}
#endif


/* for callout functions */

#ifdef USE_CALLOUT

extern OnigCalloutFunc
onig_get_progress_callout(void)
{
  return DefaultProgressCallout;
}

extern int
onig_set_progress_callout(OnigCalloutFunc f)
{
  DefaultProgressCallout = f;
  return ONIG_NORMAL;
}

extern OnigCalloutFunc
onig_get_retraction_callout(void)
{
  return DefaultRetractionCallout;
}

extern int
onig_set_retraction_callout(OnigCalloutFunc f)
{
  DefaultRetractionCallout = f;
  return ONIG_NORMAL;
}

extern int
onig_get_callout_num_by_callout_args(OnigCalloutArgs* args)
{
  return args->num;
}

extern OnigCalloutIn
onig_get_callout_in_by_callout_args(OnigCalloutArgs* args)
{
  return args->in;
}

extern int
onig_get_name_id_by_callout_args(OnigCalloutArgs* args)
{
  return args->name_id;
}

extern const UChar*
onig_get_contents_by_callout_args(OnigCalloutArgs* args)
{
  int num;
  CalloutListEntry* e;

  num = args->num;
  e = onig_reg_callout_list_at(args->regex, num);
  if (IS_NULL(e)) return 0;
  if (e->of == ONIG_CALLOUT_OF_CONTENTS) {
    return e->u.content.start;
  }

  return 0;
}

extern const UChar*
onig_get_contents_end_by_callout_args(OnigCalloutArgs* args)
{
  int num;
  CalloutListEntry* e;

  num = args->num;
  e = onig_reg_callout_list_at(args->regex, num);
  if (IS_NULL(e)) return 0;
  if (e->of == ONIG_CALLOUT_OF_CONTENTS) {
    return e->u.content.end;
  }

  return 0;
}

extern int
onig_get_args_num_by_callout_args(OnigCalloutArgs* args)
{
  int num;
  CalloutListEntry* e;

  num = args->num;
  e = onig_reg_callout_list_at(args->regex, num);
  if (IS_NULL(e)) return ONIGERR_INVALID_ARGUMENT;
  if (e->of == ONIG_CALLOUT_OF_NAME) {
    return e->u.arg.num;
  }

  return ONIGERR_INVALID_ARGUMENT;
}

extern int
onig_get_passed_args_num_by_callout_args(OnigCalloutArgs* args)
{
  int num;
  CalloutListEntry* e;

  num = args->num;
  e = onig_reg_callout_list_at(args->regex, num);
  if (IS_NULL(e)) return ONIGERR_INVALID_ARGUMENT;
  if (e->of == ONIG_CALLOUT_OF_NAME) {
    return e->u.arg.passed_num;
  }

  return ONIGERR_INVALID_ARGUMENT;
}

extern int
onig_get_arg_by_callout_args(OnigCalloutArgs* args, int index,
                             OnigType* type, OnigValue* val)
{
  int num;
  CalloutListEntry* e;

  num = args->num;
  e = onig_reg_callout_list_at(args->regex, num);
  if (IS_NULL(e)) return ONIGERR_INVALID_ARGUMENT;
  if (e->of == ONIG_CALLOUT_OF_NAME) {
    if (IS_NOT_NULL(type)) *type = e->u.arg.types[index];
    if (IS_NOT_NULL(val))  *val  = e->u.arg.vals[index];
    return ONIG_NORMAL;
  }

  return ONIGERR_INVALID_ARGUMENT;
}

extern const UChar*
onig_get_string_by_callout_args(OnigCalloutArgs* args)
{
  return args->string;
}

extern const UChar*
onig_get_string_end_by_callout_args(OnigCalloutArgs* args)
{
  return args->string_end;
}

extern const UChar*
onig_get_start_by_callout_args(OnigCalloutArgs* args)
{
  return args->start;
}

extern const UChar*
onig_get_right_range_by_callout_args(OnigCalloutArgs* args)
{
  return args->right_range;
}

extern const UChar*
onig_get_current_by_callout_args(OnigCalloutArgs* args)
{
  return args->current;
}

extern OnigRegex
onig_get_regex_by_callout_args(OnigCalloutArgs* args)
{
  return args->regex;
}

extern unsigned long
onig_get_retry_counter_by_callout_args(OnigCalloutArgs* args)
{
  return args->retry_in_match_counter;
}


extern int
onig_get_capture_range_in_callout(OnigCalloutArgs* a, int mem_num, int* begin, int* end)
{
  OnigRegex    reg;
  const UChar* str;
  StackType*   stk_base;
  int i;
  StackIndex* mem_start_stk;
  StackIndex* mem_end_stk;

  i = mem_num;
  reg = a->regex;
  str = a->string;
  stk_base = a->stk_base;
  mem_start_stk = a->mem_start_stk;
  mem_end_stk   = a->mem_end_stk;

  if (i > 0) {
    if (a->mem_end_stk[i] != INVALID_STACK_INDEX) {
      *begin = (int )(STACK_MEM_START(reg, i) - str);
      *end   = (int )(STACK_MEM_END(reg, i)   - str);
    }
    else {
      *begin = *end = ONIG_REGION_NOTPOS;
    }
  }
  else
    return ONIGERR_INVALID_ARGUMENT;

  return ONIG_NORMAL;
}

extern int
onig_get_used_stack_size_in_callout(OnigCalloutArgs* a, int* used_num, int* used_bytes)
{
  int n;

  n = (int )(a->stk - a->stk_base);

  if (used_num != 0)
    *used_num = n;

  if (used_bytes != 0)
    *used_bytes = n * sizeof(StackType);

  return ONIG_NORMAL;
}


/* builtin callout functions */

extern int
onig_builtin_fail(OnigCalloutArgs* args ARG_UNUSED, void* user_data ARG_UNUSED)
{
  return ONIG_CALLOUT_FAIL;
}

extern int
onig_builtin_mismatch(OnigCalloutArgs* args ARG_UNUSED, void* user_data ARG_UNUSED)
{
  return ONIG_MISMATCH;
}

extern int
onig_builtin_error(OnigCalloutArgs* args, void* user_data ARG_UNUSED)
{
  int r;
  int n;
  OnigValue val;

  r = onig_get_arg_by_callout_args(args, 0, 0, &val);
  if (r != ONIG_NORMAL) return r;

  n = (int )val.l;
  if (n >= 0) {
    n = ONIGERR_INVALID_CALLOUT_BODY;
  }
  else if (onig_is_error_code_needs_param(n)) {
    n = ONIGERR_INVALID_CALLOUT_BODY;
  }

  return n;
}

extern int
onig_builtin_count(OnigCalloutArgs* args, void* user_data)
{
  (void )onig_check_callout_data_and_clear_old_values(args);

  return onig_builtin_total_count(args, user_data);
}

extern int
onig_builtin_total_count(OnigCalloutArgs* args, void* user_data ARG_UNUSED)
{
  int r;
  int slot;
  OnigType  type;
  OnigValue val;
  OnigValue aval;
  OnigCodePoint count_type;

  r = onig_get_arg_by_callout_args(args, 0, &type, &aval);
  if (r != ONIG_NORMAL) return r;

  count_type = aval.c;
  if (count_type != '>' && count_type != 'X' && count_type != '<')
    return ONIGERR_INVALID_CALLOUT_ARG;

  r = onig_get_callout_data_by_callout_args_self_dont_clear_old(args, 0,
                                                                &type, &val);
  if (r < ONIG_NORMAL)
    return r;
  else if (r > ONIG_NORMAL) {
    /* type == void: initial state */
    val.l = 0;
  }

  if (args->in == ONIG_CALLOUT_IN_RETRACTION) {
    slot = 2;
    if (count_type == '<')
      val.l++;
    else if (count_type == 'X')
      val.l--;
  }
  else {
    slot = 1;
    if (count_type != '<')
      val.l++;
  }

  r = onig_set_callout_data_by_callout_args_self(args, 0, ONIG_TYPE_LONG, &val);
  if (r != ONIG_NORMAL) return r;

  /* slot 1: in progress counter, slot 2: in retraction counter */
  r = onig_get_callout_data_by_callout_args_self_dont_clear_old(args, slot,
                                                                &type, &val);
  if (r < ONIG_NORMAL)
    return r;
  else if (r > ONIG_NORMAL) {
    val.l = 0;
  }

  val.l++;
  r = onig_set_callout_data_by_callout_args_self(args, slot, ONIG_TYPE_LONG, &val);
  if (r != ONIG_NORMAL) return r;

  return ONIG_CALLOUT_SUCCESS;
}

extern int
onig_builtin_max(OnigCalloutArgs* args, void* user_data ARG_UNUSED)
{
  int r;
  int slot;
  long max_val;
  OnigCodePoint count_type;
  OnigType  type;
  OnigValue val;
  OnigValue aval;

  (void )onig_check_callout_data_and_clear_old_values(args);

  slot = 0;
  r = onig_get_callout_data_by_callout_args_self(args, slot, &type, &val);
  if (r < ONIG_NORMAL)
    return r;
  else if (r > ONIG_NORMAL) {
    /* type == void: initial state */
    type  = ONIG_TYPE_LONG;
    val.l = 0;
  }

  r = onig_get_arg_by_callout_args(args, 0, &type, &aval);
  if (r != ONIG_NORMAL) return r;
  if (type == ONIG_TYPE_TAG) {
    r = onig_get_callout_data_by_callout_args(args, aval.tag, 0, &type, &aval);
    if (r < ONIG_NORMAL) return r;
    else if (r > ONIG_NORMAL)
      max_val = 0L;
    else
      max_val = aval.l;
  }
  else { /* LONG */
    max_val = aval.l;
  }

  r = onig_get_arg_by_callout_args(args, 1, &type, &aval);
  if (r != ONIG_NORMAL) return r;

  count_type = aval.c;
  if (count_type != '>' && count_type != 'X' && count_type != '<')
    return ONIGERR_INVALID_CALLOUT_ARG;

  if (args->in == ONIG_CALLOUT_IN_RETRACTION) {
    if (count_type == '<') {
      if (val.l >= max_val) return ONIG_CALLOUT_FAIL;
      val.l++;
    }
    else if (count_type == 'X')
      val.l--;
  }
  else {
    if (count_type != '<') {
      if (val.l >= max_val) return ONIG_CALLOUT_FAIL;
      val.l++;
    }
  }

  r = onig_set_callout_data_by_callout_args_self(args, slot, ONIG_TYPE_LONG, &val);
  if (r != ONIG_NORMAL) return r;

  return ONIG_CALLOUT_SUCCESS;
}

enum OP_CMP {
  OP_EQ,
  OP_NE,
  OP_LT,
  OP_GT,
  OP_LE,
  OP_GE
};

extern int
onig_builtin_cmp(OnigCalloutArgs* args, void* user_data ARG_UNUSED)
{
  int r;
  int slot;
  long lv;
  long rv;
  OnigType  type;
  OnigValue val;
  regex_t* reg;
  enum OP_CMP op;

  reg = args->regex;

  r = onig_get_arg_by_callout_args(args, 0, &type, &val);
  if (r != ONIG_NORMAL) return r;

  if (type == ONIG_TYPE_TAG) {
    r = onig_get_callout_data_by_callout_args(args, val.tag, 0, &type, &val);
    if (r < ONIG_NORMAL) return r;
    else if (r > ONIG_NORMAL)
      lv = 0L;
    else
      lv = val.l;
  }
  else { /* ONIG_TYPE_LONG */
    lv = val.l;
  }

  r = onig_get_arg_by_callout_args(args, 2, &type, &val);
  if (r != ONIG_NORMAL) return r;

  if (type == ONIG_TYPE_TAG) {
    r = onig_get_callout_data_by_callout_args(args, val.tag, 0, &type, &val);
    if (r < ONIG_NORMAL) return r;
    else if (r > ONIG_NORMAL)
      rv = 0L;
    else
      rv = val.l;
  }
  else { /* ONIG_TYPE_LONG */
    rv = val.l;
  }

  slot = 0;
  r = onig_get_callout_data_by_callout_args_self(args, slot, &type, &val);
  if (r < ONIG_NORMAL)
    return r;
  else if (r > ONIG_NORMAL) {
    /* type == void: initial state */
    OnigCodePoint c1, c2;
    UChar* p;

    r = onig_get_arg_by_callout_args(args, 1, &type, &val);
    if (r != ONIG_NORMAL) return r;

    p = val.s.start;
    c1 = ONIGENC_MBC_TO_CODE(reg->enc, p, val.s.end);
    p += ONIGENC_MBC_ENC_LEN(reg->enc, p);
    if (p < val.s.end) {
      c2 = ONIGENC_MBC_TO_CODE(reg->enc, p, val.s.end);
      p += ONIGENC_MBC_ENC_LEN(reg->enc, p);
      if (p != val.s.end)  return ONIGERR_INVALID_CALLOUT_ARG;
    }
    else
      c2 = 0;

    switch (c1) {
    case '=':
      if (c2 != '=') return ONIGERR_INVALID_CALLOUT_ARG;
      op = OP_EQ;
      break;
    case '!':
      if (c2 != '=') return ONIGERR_INVALID_CALLOUT_ARG;
      op = OP_NE;
      break;
    case '<':
      if (c2 == '=') op = OP_LE;
      else if (c2 == 0) op = OP_LT;
      else  return ONIGERR_INVALID_CALLOUT_ARG;
      break;
    case '>':
      if (c2 == '=') op = OP_GE;
      else if (c2 == 0) op = OP_GT;
      else  return ONIGERR_INVALID_CALLOUT_ARG;
      break;
    default:
      return ONIGERR_INVALID_CALLOUT_ARG;
      break;
    }
    val.l = (long )op;
    r = onig_set_callout_data_by_callout_args_self(args, slot, ONIG_TYPE_LONG, &val);
    if (r != ONIG_NORMAL) return r;
  }
  else {
    op = (enum OP_CMP )val.l;
  }

  switch (op) {
  case OP_EQ: r = (lv == rv); break;
  case OP_NE: r = (lv != rv); break;
  case OP_LT: r = (lv <  rv); break;
  case OP_GT: r = (lv >  rv); break;
  case OP_LE: r = (lv <= rv); break;
  case OP_GE: r = (lv >= rv); break;
  }

  return r == 0 ? ONIG_CALLOUT_FAIL : ONIG_CALLOUT_SUCCESS;
}


#include <stdio.h>

static FILE* OutFp;

/* name start with "onig_" for macros. */
static int
onig_builtin_monitor(OnigCalloutArgs* args, void* user_data)
{
  int r;
  int num;
  size_t tag_len;
  const UChar* start;
  const UChar* right;
  const UChar* current;
  const UChar* string;
  const UChar* strend;
  const UChar* tag_start;
  const UChar* tag_end;
  regex_t* reg;
  OnigCalloutIn in;
  OnigType type;
  OnigValue val;
  char buf[20];
  FILE* fp;

  fp = OutFp;

  r = onig_get_arg_by_callout_args(args, 0, &type, &val);
  if (r != ONIG_NORMAL) return r;

  in = onig_get_callout_in_by_callout_args(args);
  if (in == ONIG_CALLOUT_IN_PROGRESS) {
    if (val.c == '<')
      return ONIG_CALLOUT_SUCCESS;
  }
  else {
    if (val.c != 'X' && val.c != '<')
      return ONIG_CALLOUT_SUCCESS;
  }

  num       = onig_get_callout_num_by_callout_args(args);
  start     = onig_get_start_by_callout_args(args);
  right     = onig_get_right_range_by_callout_args(args);
  current   = onig_get_current_by_callout_args(args);
  string    = onig_get_string_by_callout_args(args);
  strend    = onig_get_string_end_by_callout_args(args);
  reg       = onig_get_regex_by_callout_args(args);
  tag_start = onig_get_callout_tag_start(reg, num);
  tag_end   = onig_get_callout_tag_end(reg, num);

  if (tag_start == 0)
    xsnprintf(buf, sizeof(buf), "#%d", num);
  else {
    /* CAUTION: tag string is not terminated with NULL. */
    int i;

    tag_len = tag_end - tag_start;
    if (tag_len >= sizeof(buf)) tag_len = sizeof(buf) - 1;
    for (i = 0; i < tag_len; i++) buf[i] = tag_start[i];
    buf[tag_len] = '\0';
  }

  fprintf(fp, "ONIG-MONITOR: %-4s %s at: %d [%d - %d] len: %d\n",
          buf,
          in == ONIG_CALLOUT_IN_PROGRESS ? "=>" : "<=",
          (int )(current - string),
          (int )(start   - string),
          (int )(right   - string),
          (int )(strend  - string));
  fflush(fp);

  return ONIG_CALLOUT_SUCCESS;
}

extern int
onig_setup_builtin_monitors_by_ascii_encoded_name(void* fp /* FILE* */)
{
  int id;
  char* name;
  OnigEncoding enc;
  unsigned int ts[4];
  OnigValue opts[4];

  if (IS_NOT_NULL(fp))
    OutFp = (FILE* )fp;
  else
    OutFp = stdout;

  enc = ONIG_ENCODING_ASCII;

  name = "MON";
  ts[0] = ONIG_TYPE_CHAR;
  opts[0].c = '>';
  BC_B_O(name, monitor, 1, ts, 1, opts);

  return ONIG_NORMAL;
}

#endif /* USE_CALLOUT */
