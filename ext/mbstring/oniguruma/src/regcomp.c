/**********************************************************************
  regcomp.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2024  K.Kosako
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

#define OPS_INIT_SIZE  8

#define ND_IS_REAL_IGNORECASE(node) \
  (ND_IS_IGNORECASE(node) && !ND_STRING_IS_CRUDE(node))

typedef struct {
  OnigLen min;
  OnigLen max;
} MinMaxLen;

typedef struct {
  OnigLen min;
  OnigLen max;
  int min_is_sure;
} MinMaxCharLen;

OnigCaseFoldType OnigDefaultCaseFoldFlag = ONIGENC_CASE_FOLD_MIN;

static OnigLen node_min_byte_len(Node* node, ParseEnv* env);

static int
ops_init(regex_t* reg, int init_alloc_size)
{
  Operation* p;
  size_t size;

  if (init_alloc_size <= 0)
    return ONIGERR_PARSER_BUG;

  size = sizeof(Operation) * init_alloc_size;
  p = (Operation* )xrealloc(reg->ops, size);
  CHECK_NULL_RETURN_MEMERR(p);
  reg->ops = p;
#ifdef USE_DIRECT_THREADED_CODE
  {
    enum OpCode* cp;
    size = sizeof(enum OpCode) * init_alloc_size;
    cp = (enum OpCode* )xrealloc(reg->ocs, size);
    CHECK_NULL_RETURN_MEMERR(cp);
    reg->ocs = cp;
  }
#endif

  reg->ops_curr  = 0; /* !!! not yet done ops_new() */
  reg->ops_alloc = init_alloc_size;
  reg->ops_used  = 0;

  return ONIG_NORMAL;
}

static int
ops_resize(regex_t* reg, int n)
{
#ifdef USE_DIRECT_THREADED_CODE
  enum OpCode* cp;
#endif
  Operation* p;
  size_t size;

  if (n == reg->ops_alloc) return ONIG_NORMAL;
  if (n <= 0) return ONIGERR_PARSER_BUG;

  size = sizeof(Operation) * n;
  p = (Operation* )xrealloc(reg->ops, size);
  CHECK_NULL_RETURN_MEMERR(p);
  reg->ops = p;

#ifdef USE_DIRECT_THREADED_CODE
  size = sizeof(enum OpCode) * n;
  cp = (enum OpCode* )xrealloc(reg->ocs, size);
  CHECK_NULL_RETURN_MEMERR(cp);
  reg->ocs = cp;
#endif

  reg->ops_alloc = n;
  if (reg->ops_used == 0)
    reg->ops_curr = 0;
  else
    reg->ops_curr = reg->ops + (reg->ops_used - 1);

  return ONIG_NORMAL;
}

static int
ops_new(regex_t* reg)
{
  if (reg->ops_used >= reg->ops_alloc) {
    int r = ops_resize(reg, reg->ops_alloc << 1);
    if (r != ONIG_NORMAL) return r;
  }

  reg->ops_curr = reg->ops + reg->ops_used;
  reg->ops_used++;

  xmemset(reg->ops_curr, 0, sizeof(Operation));
  return ONIG_NORMAL;
}

static int
is_in_string_pool(regex_t* reg, UChar* s)
{
  return (s >= reg->string_pool && s < reg->string_pool_end);
}

static void
ops_free(regex_t* reg)
{
  int i;

  if (IS_NULL(reg->ops)) return ;

  for (i = 0; i < (int )reg->ops_used; i++) {
    enum OpCode opcode;
    Operation* op;

    op = reg->ops + i;

#ifdef USE_DIRECT_THREADED_CODE
    opcode = *(reg->ocs + i);
#else
    opcode = op->opcode;
#endif

    switch (opcode) {
    case OP_STR_MBN:
      if (! is_in_string_pool(reg, op->exact_len_n.s))
        xfree(op->exact_len_n.s);
      break;
    case OP_STR_N: case OP_STR_MB2N: case OP_STR_MB3N:
      if (! is_in_string_pool(reg, op->exact_n.s))
        xfree(op->exact_n.s);
      break;
    case OP_STR_1: case OP_STR_2: case OP_STR_3: case OP_STR_4:
    case OP_STR_5: case OP_STR_MB2N1: case OP_STR_MB2N2:
    case OP_STR_MB2N3:
      break;

    case OP_CCLASS_NOT: case OP_CCLASS:
      xfree(op->cclass.bsp);
      break;

    case OP_CCLASS_MB_NOT: case OP_CCLASS_MB:
      xfree(op->cclass_mb.mb);
      break;
    case OP_CCLASS_MIX_NOT: case OP_CCLASS_MIX:
      xfree(op->cclass_mix.mb);
      xfree(op->cclass_mix.bsp);
      break;

    case OP_BACKREF1: case OP_BACKREF2: case OP_BACKREF_N: case OP_BACKREF_N_IC:
      break;
    case OP_BACKREF_MULTI:      case OP_BACKREF_MULTI_IC:
    case OP_BACKREF_CHECK:
#ifdef USE_BACKREF_WITH_LEVEL
    case OP_BACKREF_WITH_LEVEL:
    case OP_BACKREF_WITH_LEVEL_IC:
    case OP_BACKREF_CHECK_WITH_LEVEL:
#endif
      if (op->backref_general.num != 1)
        xfree(op->backref_general.ns);
      break;

    default:
      break;
    }
  }

  xfree(reg->ops);
#ifdef USE_DIRECT_THREADED_CODE
  xfree(reg->ocs);
  reg->ocs = 0;
#endif

  reg->ops = 0;
  reg->ops_curr  = 0;
  reg->ops_alloc = 0;
  reg->ops_used  = 0;
}

static int
ops_calc_size_of_string_pool(regex_t* reg)
{
  int i;
  int total;

  if (IS_NULL(reg->ops)) return 0;

  total = 0;
  for (i = 0; i < (int )reg->ops_used; i++) {
    enum OpCode opcode;
    Operation* op;

    op = reg->ops + i;
#ifdef USE_DIRECT_THREADED_CODE
    opcode = *(reg->ocs + i);
#else
    opcode = op->opcode;
#endif

    switch (opcode) {
    case OP_STR_MBN:
      total += op->exact_len_n.len * op->exact_len_n.n;
      break;
    case OP_STR_N:
    case OP_STR_MB2N:
      total += op->exact_n.n * 2;
      break;
    case OP_STR_MB3N:
      total += op->exact_n.n * 3;
      break;

    default:
      break;
    }
  }

  return total;
}

static int
ops_make_string_pool(regex_t* reg)
{
  int i;
  int len;
  int size;
  UChar* pool;
  UChar* curr;

  size = ops_calc_size_of_string_pool(reg);
  if (size <= 0) {
    return 0;
  }

  curr = pool = (UChar* )xmalloc((size_t )size);
  CHECK_NULL_RETURN_MEMERR(pool);

  for (i = 0; i < (int )reg->ops_used; i++) {
    enum OpCode opcode;
    Operation* op;

    op = reg->ops + i;
#ifdef USE_DIRECT_THREADED_CODE
    opcode = *(reg->ocs + i);
#else
    opcode = op->opcode;
#endif

    switch (opcode) {
    case OP_STR_MBN:
      len = op->exact_len_n.len * op->exact_len_n.n;
      xmemcpy(curr, op->exact_len_n.s, len);
      xfree(op->exact_len_n.s);
      op->exact_len_n.s = curr;
      curr += len;
      break;
    case OP_STR_N:
      len = op->exact_n.n;
    copy:
      xmemcpy(curr, op->exact_n.s, len);
      xfree(op->exact_n.s);
      op->exact_n.s = curr;
      curr += len;
      break;
    case OP_STR_MB2N:
      len = op->exact_n.n * 2;
      goto copy;
      break;
    case OP_STR_MB3N:
      len = op->exact_n.n * 3;
      goto copy;
      break;

    default:
      break;
    }
  }

  reg->string_pool     = pool;
  reg->string_pool_end = pool + size;
  return 0;
}

extern OnigCaseFoldType
onig_get_default_case_fold_flag(void)
{
  return OnigDefaultCaseFoldFlag;
}

extern int
onig_set_default_case_fold_flag(OnigCaseFoldType case_fold_flag)
{
  OnigDefaultCaseFoldFlag = case_fold_flag;
  return 0;
}

static int
len_multiply_cmp(OnigLen x, int y, OnigLen v)
{
  if (x == 0 || y == 0) return -1;

  if (x < INFINITE_LEN / y) {
    OnigLen xy = x * (OnigLen )y;
    if (xy > v) return 1;
    else {
      if (xy == v) return 0;
      else return -1;
    }
  }
  else
    return v == INFINITE_LEN ? 0 : 1;
}

extern int
onig_positive_int_multiply(int x, int y)
{
  if (x == 0 || y == 0) return 0;

  if (x < ONIG_INT_MAX / y)
    return x * y;
  else
    return -1;
}


static void
node_swap(Node* a, Node* b)
{
  Node c;

  c = *a; *a = *b; *b = c;

  if (ND_TYPE(a) == ND_STRING) {
    StrNode* sn = STR_(a);
    if (sn->capacity == 0) {
      int len = (int )(sn->end - sn->s);
      sn->s   = sn->buf;
      sn->end = sn->s + len;
    }
  }

  if (ND_TYPE(b) == ND_STRING) {
    StrNode* sn = STR_(b);
    if (sn->capacity == 0) {
      int len = (int )(sn->end - sn->s);
      sn->s   = sn->buf;
      sn->end = sn->s + len;
    }
  }
}

static int
node_list_len(Node* list)
{
  int len;

  len = 1;
  while (IS_NOT_NULL(ND_CDR(list))) {
    list = ND_CDR(list);
    len++;
  }

  return len;
}

static Node*
node_list_add(Node* list, Node* x)
{
  Node *n;

  n = onig_node_new_list(x, NULL);
  if (IS_NULL(n)) return NULL_NODE;

  if (IS_NOT_NULL(list)) {
    while (IS_NOT_NULL(ND_CDR(list)))
      list = ND_CDR(list);

    ND_CDR(list) = n;
  }

  return n;
}

static int
node_str_node_cat(Node* node, Node* add)
{
  int r;

  if (ND_STATUS(node) != ND_STATUS(add))
    return ONIGERR_TYPE_BUG;

  if (STR_(node)->flag != STR_(add)->flag)
    return ONIGERR_TYPE_BUG;

  r = onig_node_str_cat(node, STR_(add)->s, STR_(add)->end);
  if (r != 0) return r;

  return 0;
}

static void
node_conv_to_str_node(Node* node, Node* ref_node)
{
  xmemset(node, 0, sizeof(*node));
  ND_SET_TYPE(node, ND_STRING);
  ND_STATUS(node) = ND_STATUS(ref_node);

  STR_(node)->flag     = STR_(ref_node)->flag;
  STR_(node)->s        = STR_(node)->buf;
  STR_(node)->end      = STR_(node)->buf;
  STR_(node)->capacity = 0;
}

static OnigLen
distance_add(OnigLen d1, OnigLen d2)
{
  if (d1 == INFINITE_LEN || d2 == INFINITE_LEN)
    return INFINITE_LEN;
  else {
    if (d1 <= INFINITE_LEN - d2) return d1 + d2;
    else return INFINITE_LEN;
  }
}

static OnigLen
distance_multiply(OnigLen d, int m)
{
  if (m == 0) return 0;

  if (d < INFINITE_LEN / m)
    return d * m;
  else
    return INFINITE_LEN;
}

static int
bitset_is_empty(BitSetRef bs)
{
  int i;

  for (i = 0; i < (int )BITSET_REAL_SIZE; i++) {
    if (bs[i] != 0) return 0;
  }
  return 1;
}

#ifdef USE_CALL

static int
unset_addr_list_init(UnsetAddrList* list, int size)
{
  UnsetAddr* p = (UnsetAddr* )xmalloc(sizeof(UnsetAddr)* size);
  CHECK_NULL_RETURN_MEMERR(p);

  list->num   = 0;
  list->alloc = size;
  list->us    = p;
  return 0;
}

static void
unset_addr_list_end(UnsetAddrList* list)
{
  if (IS_NOT_NULL(list->us))
    xfree(list->us);
}

static int
unset_addr_list_add(UnsetAddrList* list, int offset, struct _Node* node)
{
  UnsetAddr* p;
  int size;

  if (list->num >= list->alloc) {
    size = list->alloc * 2;
    p = (UnsetAddr* )xrealloc(list->us, sizeof(UnsetAddr) * size);
    CHECK_NULL_RETURN_MEMERR(p);
    list->alloc = size;
    list->us    = p;
  }

  list->us[list->num].offset = offset;
  list->us[list->num].target = node;
  list->num++;
  return 0;
}
#endif /* USE_CALL */

enum CharLenReturnType {
  CHAR_LEN_NORMAL = 0,       /* fixed or variable */
  CHAR_LEN_TOP_ALT_FIXED = 1
};

static int
mmcl_fixed(MinMaxCharLen* c)
{
  return (c->min == c->max && c->min != INFINITE_LEN);
}

static void
mmcl_set(MinMaxCharLen* l, OnigLen len)
{
  l->min = len;
  l->max = len;
  l->min_is_sure = TRUE;
}

static void
mmcl_set_min_max(MinMaxCharLen* l, OnigLen min, OnigLen max, int min_is_sure)
{
  l->min = min;
  l->max = max;
  l->min_is_sure = min_is_sure;
}

static void
mmcl_add(MinMaxCharLen* to, MinMaxCharLen* add)
{
  to->min = distance_add(to->min, add->min);
  to->max = distance_add(to->max, add->max);

  to->min_is_sure = add->min_is_sure != FALSE && to->min_is_sure != FALSE;
}

static void
mmcl_multiply(MinMaxCharLen* to, int m)
{
  to->min = distance_multiply(to->min, m);
  to->max = distance_multiply(to->max, m);
}

static void
mmcl_repeat_range_multiply(MinMaxCharLen* to, int mlow, int mhigh)
{
  to->min = distance_multiply(to->min, mlow);

  if (IS_INFINITE_REPEAT(mhigh))
    to->max = INFINITE_LEN;
  else
    to->max = distance_multiply(to->max, mhigh);
}

static void
mmcl_alt_merge(MinMaxCharLen* to, MinMaxCharLen* alt)
{
  if (to->min > alt->min) {
    to->min         = alt->min;
    to->min_is_sure = alt->min_is_sure;
  }
  else if (to->min == alt->min) {
    if (alt->min_is_sure != FALSE)
      to->min_is_sure = TRUE;
  }

  if (to->max < alt->max) to->max = alt->max;
}

#ifndef ONIG_DONT_OPTIMIZE

static int
mml_is_equal(MinMaxLen* a, MinMaxLen* b)
{
  return a->min == b->min && a->max == b->max;
}

static void
mml_set_min_max(MinMaxLen* l, OnigLen min, OnigLen max)
{
  l->min = min;
  l->max = max;
}

static void
mml_clear(MinMaxLen* l)
{
  l->min = l->max = 0;
}

static void
mml_copy(MinMaxLen* to, MinMaxLen* from)
{
  to->min = from->min;
  to->max = from->max;
}

static void
mml_add(MinMaxLen* to, MinMaxLen* add)
{
  to->min = distance_add(to->min, add->min);
  to->max = distance_add(to->max, add->max);
}

static void
mml_alt_merge(MinMaxLen* to, MinMaxLen* alt)
{
  if (to->min > alt->min) to->min = alt->min;
  if (to->max < alt->max) to->max = alt->max;
}

#endif

/* fixed size pattern node only */
static int
node_char_len1(Node* node, regex_t* reg, MinMaxCharLen* ci, ParseEnv* env,
               int level)
{
  MinMaxCharLen tci;
  int r = CHAR_LEN_NORMAL;

  level++;

  switch (ND_TYPE(node)) {
  case ND_LIST:
    {
      int first = TRUE;
      do {
        r = node_char_len1(ND_CAR(node), reg, &tci, env, level);
        if (r < 0) break;
        if (first == TRUE) {
          *ci = tci;
          first = FALSE;
        }
        else
          mmcl_add(ci, &tci);
      } while (IS_NOT_NULL(node = ND_CDR(node)));
    }
    break;

  case ND_ALT:
    {
      int fixed;

      r = node_char_len1(ND_CAR(node), reg, ci, env, level);
      if (r < 0) break;

      fixed = TRUE;
      while (IS_NOT_NULL(node = ND_CDR(node))) {
        r = node_char_len1(ND_CAR(node), reg, &tci, env, level);
        if (r < 0) break;
        if (! mmcl_fixed(&tci))
          fixed = FALSE;
        mmcl_alt_merge(ci, &tci);
      }
      if (r < 0) break;

      r = CHAR_LEN_NORMAL;
      if (mmcl_fixed(ci)) break;

      if (fixed == TRUE && level == 1) {
        r = CHAR_LEN_TOP_ALT_FIXED;
      }
    }
    break;

  case ND_STRING:
    {
      OnigLen clen;
      StrNode* sn = STR_(node);
      UChar *s = sn->s;

      if (ND_IS_REAL_IGNORECASE(node) &&
          CASE_FOLD_IS_NOT_ASCII_ONLY(env->case_fold_flag)) {
        /* Such a case is possible.
           ex. /(?i)(?<=\1)(a)/
           Backref node refer to capture group, but it doesn't tune yet.
         */
        r = ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
        break;
      }

      clen = 0;
      while (s < sn->end) {
        s += enclen(reg->enc, s);
        clen = distance_add(clen, 1);
      }
      mmcl_set(ci, clen);
    }
    break;

  case ND_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->lower == qn->upper) {
        if (qn->upper == 0) {
          mmcl_set(ci, 0);
        }
        else {
          r = node_char_len1(ND_BODY(node), reg, ci, env, level);
          if (r < 0) break;
          mmcl_multiply(ci, qn->lower);
        }
      }
      else {
        r = node_char_len1(ND_BODY(node), reg, ci, env, level);
        if (r < 0) break;
        mmcl_repeat_range_multiply(ci, qn->lower, qn->upper);
      }
    }
    break;

#ifdef USE_CALL
  case ND_CALL:
    if (ND_IS_RECURSION(node))
      mmcl_set_min_max(ci, 0, INFINITE_LEN, FALSE);
    else
      r = node_char_len1(ND_BODY(node), reg, ci, env, level);
    break;
#endif

  case ND_CTYPE:
  case ND_CCLASS:
    mmcl_set(ci, 1);
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      switch (en->type) {
      case BAG_MEMORY:
        if (ND_IS_FIXED_CLEN(node)) {
          mmcl_set_min_max(ci, en->min_char_len, en->max_char_len,
                           ND_IS_FIXED_CLEN_MIN_SURE(node));
        }
        else {
          if (ND_IS_MARK1(node)) {
            mmcl_set_min_max(ci, 0, INFINITE_LEN, FALSE);
          }
          else {
            ND_STATUS_ADD(node, MARK1);
            r = node_char_len1(ND_BODY(node), reg, ci, env, level);
            ND_STATUS_REMOVE(node, MARK1);
            if (r < 0) break;

            en->min_char_len = ci->min;
            en->max_char_len = ci->max;
            ND_STATUS_ADD(node, FIXED_CLEN);
            if (ci->min_is_sure != FALSE)
              ND_STATUS_ADD(node, FIXED_CLEN_MIN_SURE);
          }
        }
        /* can't optimize look-behind if capture exists. */
        ci->min_is_sure = FALSE;
        break;
      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        r = node_char_len1(ND_BODY(node), reg, ci, env, level);
        break;
      case BAG_IF_ELSE:
        {
          MinMaxCharLen eci;

          r = node_char_len1(ND_BODY(node), reg, ci, env, level);
          if (r < 0) break;

          if (IS_NOT_NULL(en->te.Then)) {
            r = node_char_len1(en->te.Then, reg, &tci, env, level);
            if (r < 0) break;
            mmcl_add(ci, &tci);
          }

          if (IS_NOT_NULL(en->te.Else)) {
            r = node_char_len1(en->te.Else, reg, &eci, env, level);
            if (r < 0) break;
          }
          else {
            mmcl_set(&eci, 0);
          }

          mmcl_alt_merge(ci, &eci);
        }
        break;
      default: /* never come here */
        r = ONIGERR_PARSER_BUG;
        break;
      }
    }
    break;

  case ND_GIMMICK:
    mmcl_set(ci, 0);
    break;

  case ND_ANCHOR:
  zero:
    mmcl_set(ci, 0);
    /* can't optimize look-behind if anchor exists. */
    ci->min_is_sure = FALSE;
    break;

  case ND_BACKREF:
    if (ND_IS_CHECKER(node))
      goto zero;

    if (ND_IS_RECURSION(node)) {
#ifdef USE_BACKREF_WITH_LEVEL
      if (ND_IS_NEST_LEVEL(node)) {
        mmcl_set_min_max(ci, 0, INFINITE_LEN, FALSE);
        break;
      }
#endif

      mmcl_set_min_max(ci, 0, 0, FALSE);
      break;
    }

    {
      int i;
      int* backs;
      MemEnv* mem_env = PARSEENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);

      backs = BACKREFS_P(br);
      r = node_char_len1(mem_env[backs[0]].mem_node, reg, ci, env, level);
      if (r < 0) break;
      if (! mmcl_fixed(ci)) ci->min_is_sure = FALSE;

      for (i = 1; i < br->back_num; i++) {
        r = node_char_len1(mem_env[backs[i]].mem_node, reg, &tci, env, level);
        if (r < 0) break;
        if (! mmcl_fixed(&tci)) tci.min_is_sure = FALSE;
        mmcl_alt_merge(ci, &tci);
      }
    }
    break;

  default: /* never come here */
    r = ONIGERR_PARSER_BUG;
    break;
  }

  return r;
}

static int
node_char_len(Node* node, regex_t* reg, MinMaxCharLen* ci, ParseEnv* env)
{
  return node_char_len1(node, reg, ci, env, 0);
}


static int
add_op(regex_t* reg, int opcode)
{
  int r;

  r = ops_new(reg);
  if (r != ONIG_NORMAL) return r;

#ifdef USE_DIRECT_THREADED_CODE
  *(reg->ocs + (reg->ops_curr - reg->ops)) = opcode;
#else
  reg->ops_curr->opcode = opcode;
#endif

  return 0;
}

static int compile_length_tree(Node* node, regex_t* reg, ParseEnv* env);
static int compile_tree(Node* node, regex_t* reg, ParseEnv* env);


#define IS_NEED_STR_LEN_OP(op) \
   ((op) == OP_STR_N    || (op) == OP_STR_MB2N ||\
    (op) == OP_STR_MB3N || (op) == OP_STR_MBN)

static int
select_str_opcode(int mb_len, int str_len)
{
  int op;

  switch (mb_len) {
  case 1:
    switch (str_len) {
    case 1:  op = OP_STR_1; break;
    case 2:  op = OP_STR_2; break;
    case 3:  op = OP_STR_3; break;
    case 4:  op = OP_STR_4; break;
    case 5:  op = OP_STR_5; break;
    default: op = OP_STR_N; break;
    }
    break;

  case 2:
    switch (str_len) {
    case 1:  op = OP_STR_MB2N1; break;
    case 2:  op = OP_STR_MB2N2; break;
    case 3:  op = OP_STR_MB2N3; break;
    default: op = OP_STR_MB2N;  break;
    }
    break;

  case 3:
    op = OP_STR_MB3N;
    break;

  default:
    op = OP_STR_MBN;
    break;
  }

  return op;
}

static int
is_strict_real_node(Node* node)
{
  switch (ND_TYPE(node)) {
  case ND_STRING:
    {
      StrNode* sn = STR_(node);
      return (sn->end != sn->s);
    }
    break;

  case ND_CCLASS:
  case ND_CTYPE:
    return 1;
    break;

  default:
    return 0;
    break;
  }
}

static int
compile_quant_body_with_empty_check(QuantNode* qn, regex_t* reg, ParseEnv* env)
{
  int r;
  int saved_num_empty_check;
  int emptiness;
  Node* body;

  body = ND_BODY((Node* )qn);
  emptiness = qn->emptiness;
  saved_num_empty_check = reg->num_empty_check;

  if (emptiness != BODY_IS_NOT_EMPTY) {
    r = add_op(reg, OP_EMPTY_CHECK_START);
    if (r != 0) return r;
    COP(reg)->empty_check_start.mem = reg->num_empty_check; /* NULL CHECK ID */
    reg->num_empty_check++;
  }

  r = compile_tree(body, reg, env);
  if (r != 0) return r;

  if (emptiness != BODY_IS_NOT_EMPTY) {
    if (emptiness == BODY_MAY_BE_EMPTY)
      r = add_op(reg, OP_EMPTY_CHECK_END);
    else if (emptiness == BODY_MAY_BE_EMPTY_MEM) {
      if (ND_IS_EMPTY_STATUS_CHECK(qn) != 0 && qn->empty_status_mem != 0) {
        r = add_op(reg, OP_EMPTY_CHECK_END_MEMST);
        if (r != 0) return r;
        COP(reg)->empty_check_end.empty_status_mem = qn->empty_status_mem;
      }
      else
        r = add_op(reg, OP_EMPTY_CHECK_END);
    }
#ifdef USE_CALL
    else if (emptiness == BODY_MAY_BE_EMPTY_REC) {
      r = add_op(reg, OP_EMPTY_CHECK_END_MEMST_PUSH);
      if (r != 0) return r;
      COP(reg)->empty_check_end.empty_status_mem = qn->empty_status_mem;
    }
#endif

    if (r != 0) return r;
    COP(reg)->empty_check_end.mem = saved_num_empty_check; /* NULL CHECK ID */
  }
  return r;
}

#ifdef USE_CALL
static int
compile_call(CallNode* node, regex_t* reg, ParseEnv* env)
{
  int r;
  int offset;

  r = add_op(reg, OP_CALL);
  if (r != 0) return r;

  COP(reg)->call.addr = 0; /* dummy addr. */
#ifdef ONIG_DEBUG_MATCH_COUNTER
  COP(reg)->call.called_mem = node->called_gnum;
#endif

  offset = COP_CURR_OFFSET_BYTES(reg, call.addr);
  r = unset_addr_list_add(env->unset_addr_list, offset, ND_CALL_BODY(node));
  return r;
}
#endif

static int
compile_tree_n_times(Node* node, int n, regex_t* reg, ParseEnv* env)
{
  int i, r;

  for (i = 0; i < n; i++) {
    r = compile_tree(node, reg, env);
    if (r != 0) return r;
  }
  return 0;
}

static int
add_compile_string_length(UChar* s ARG_UNUSED, int mb_len, int str_len,
                          regex_t* reg ARG_UNUSED)
{
  return 1;
}

static int
add_compile_string(UChar* s, int mb_len, int str_len, regex_t* reg)
{
  int op;
  int r;
  int byte_len;
  UChar* p;
  UChar* end;

  op = select_str_opcode(mb_len, str_len);
  r = add_op(reg, op);
  if (r != 0) return r;

  byte_len = mb_len * str_len;
  end = s + byte_len;

  if (op == OP_STR_MBN) {
    p = onigenc_strdup(reg->enc, s, end);
    CHECK_NULL_RETURN_MEMERR(p);

    COP(reg)->exact_len_n.len = mb_len;
    COP(reg)->exact_len_n.n   = str_len;
    COP(reg)->exact_len_n.s   = p;
  }
  else if (IS_NEED_STR_LEN_OP(op)) {
    p = onigenc_strdup(reg->enc, s, end);
    CHECK_NULL_RETURN_MEMERR(p);
    COP(reg)->exact_n.n = str_len;
    COP(reg)->exact_n.s = p;
  }
  else {
    xmemset(COP(reg)->exact.s, 0, sizeof(COP(reg)->exact.s));
    xmemcpy(COP(reg)->exact.s, s, (size_t )byte_len);
  }

  return 0;
}

static int
compile_length_string_node(Node* node, regex_t* reg)
{
  int rlen, r, len, prev_len, slen;
  UChar *p, *prev;
  StrNode* sn;
  OnigEncoding enc = reg->enc;

  sn = STR_(node);
  if (sn->end <= sn->s)
    return 0;

  p = prev = sn->s;
  prev_len = enclen(enc, p);
  p += prev_len;
  slen = 1;
  rlen = 0;

  for (; p < sn->end; ) {
    len = enclen(enc, p);
    if (len == prev_len) {
      slen++;
    }
    else {
      r = add_compile_string_length(prev, prev_len, slen, reg);
      rlen += r;
      prev = p;
      slen = 1;
      prev_len = len;
    }
    p += len;
  }

  r = add_compile_string_length(prev, prev_len, slen, reg);
  rlen += r;
  return rlen;
}

static int
compile_length_string_crude_node(StrNode* sn, regex_t* reg)
{
  if (sn->end <= sn->s)
    return 0;

  return add_compile_string_length(sn->s, 1 /* sb */, (int )(sn->end - sn->s),
                                   reg);
}

static int
compile_string_node(Node* node, regex_t* reg)
{
  int r, len, prev_len, slen;
  UChar *p, *prev, *end;
  StrNode* sn;
  OnigEncoding enc = reg->enc;

  sn = STR_(node);
  if (sn->end <= sn->s)
    return 0;

  end = sn->end;

  p = prev = sn->s;
  prev_len = enclen(enc, p);
  p += prev_len;
  slen = 1;

  for (; p < end; ) {
    len = enclen(enc, p);
    if (len == prev_len) {
      slen++;
    }
    else {
      r = add_compile_string(prev, prev_len, slen, reg);
      if (r != 0) return r;

      prev  = p;
      slen  = 1;
      prev_len = len;
    }

    p += len;
  }

  return add_compile_string(prev, prev_len, slen, reg);
}

static int
compile_string_crude_node(StrNode* sn, regex_t* reg)
{
  if (sn->end <= sn->s)
    return 0;

  return add_compile_string(sn->s, 1 /* sb */, (int )(sn->end - sn->s), reg);
}

static void*
set_multi_byte_cclass(BBuf* mbuf, regex_t* reg)
{
  size_t len;
  void* p;

  len = (size_t )mbuf->used;
  p = xmalloc(len);
  if (IS_NULL(p)) return NULL;

  xmemcpy(p, mbuf->p, len);
  return p;
}

static int
compile_length_cclass_node(CClassNode* cc, regex_t* reg)
{
  return 1;
}

static int
compile_cclass_node(CClassNode* cc, regex_t* reg)
{
  int r;

  if (IS_NULL(cc->mbuf)) {
    r = add_op(reg, IS_NCCLASS_NOT(cc) ? OP_CCLASS_NOT : OP_CCLASS);
    if (r != 0) return r;

    COP(reg)->cclass.bsp = xmalloc(SIZE_BITSET);
    CHECK_NULL_RETURN_MEMERR(COP(reg)->cclass.bsp);
    xmemcpy(COP(reg)->cclass.bsp, cc->bs, SIZE_BITSET);
  }
  else {
    void* p;

    if (ONIGENC_MBC_MINLEN(reg->enc) > 1 || bitset_is_empty(cc->bs)) {
      r = add_op(reg, IS_NCCLASS_NOT(cc) ? OP_CCLASS_MB_NOT : OP_CCLASS_MB);
      if (r != 0) return r;

      p = set_multi_byte_cclass(cc->mbuf, reg);
      CHECK_NULL_RETURN_MEMERR(p);
      COP(reg)->cclass_mb.mb = p;
    }
    else {
      r = add_op(reg, IS_NCCLASS_NOT(cc) ? OP_CCLASS_MIX_NOT : OP_CCLASS_MIX);
      if (r != 0) return r;

      COP(reg)->cclass_mix.bsp = xmalloc(SIZE_BITSET);
      CHECK_NULL_RETURN_MEMERR(COP(reg)->cclass_mix.bsp);
      xmemcpy(COP(reg)->cclass_mix.bsp, cc->bs, SIZE_BITSET);

      p = set_multi_byte_cclass(cc->mbuf, reg);
      CHECK_NULL_RETURN_MEMERR(p);
      COP(reg)->cclass_mix.mb = p;
    }
  }

  return 0;
}

static void
set_addr_in_repeat_range(regex_t* reg)
{
  int i;

  for (i = 0; i < reg->num_repeat; i++) {
    RepeatRange* p = reg->repeat_range + i;
    int offset = p->u.offset;
    p->u.pcode = reg->ops + offset;
  }
}

static int
entry_repeat_range(regex_t* reg, int id, int lower, int upper, int ops_index)
{
#define REPEAT_RANGE_ALLOC  4

  RepeatRange* p;

  if (reg->repeat_range_alloc == 0) {
    p = (RepeatRange* )xmalloc(sizeof(RepeatRange) * REPEAT_RANGE_ALLOC);
    CHECK_NULL_RETURN_MEMERR(p);
    reg->repeat_range = p;
    reg->repeat_range_alloc = REPEAT_RANGE_ALLOC;
  }
  else if (reg->repeat_range_alloc <= id) {
    int n;
    n = reg->repeat_range_alloc + REPEAT_RANGE_ALLOC;
    p = (RepeatRange* )xrealloc(reg->repeat_range, sizeof(RepeatRange) * n);
    CHECK_NULL_RETURN_MEMERR(p);
    reg->repeat_range = p;
    reg->repeat_range_alloc = n;
  }
  else {
    p = reg->repeat_range;
  }

  p[id].lower    = lower;
  p[id].upper    = (IS_INFINITE_REPEAT(upper) ? 0x7fffffff : upper);
  p[id].u.offset = ops_index;
  return 0;
}

static int
compile_range_repeat_node(QuantNode* qn, int target_len, int emptiness,
                          regex_t* reg, ParseEnv* env)
{
  int r;
  int num_repeat = reg->num_repeat++;

  r = add_op(reg, qn->greedy ? OP_REPEAT : OP_REPEAT_NG);
  if (r != 0) return r;

  COP(reg)->repeat.id   = num_repeat;
  COP(reg)->repeat.addr = SIZE_INC + target_len + OPSIZE_REPEAT_INC;

  r = entry_repeat_range(reg, num_repeat, qn->lower, qn->upper,
                         COP_CURR_OFFSET(reg) + OPSIZE_REPEAT);
  if (r != 0) return r;

  r = compile_quant_body_with_empty_check(qn, reg, env);
  if (r != 0) return r;

  r = add_op(reg, qn->greedy ? OP_REPEAT_INC : OP_REPEAT_INC_NG);
  if (r != 0) return r;

  COP(reg)->repeat_inc.id = num_repeat;
  return r;
}

static int
is_anychar_infinite_greedy(QuantNode* qn)
{
  if (qn->greedy && IS_INFINITE_REPEAT(qn->upper) &&
      ND_IS_ANYCHAR(ND_QUANT_BODY(qn)))
    return 1;
  else
    return 0;
}

#define QUANTIFIER_EXPAND_LIMIT_SIZE   10
#define CKN_ON   (ckn > 0)

static int
compile_length_quantifier_node(QuantNode* qn, regex_t* reg, ParseEnv* env)
{
  int len, mod_tlen;
  int infinite = IS_INFINITE_REPEAT(qn->upper);
  enum BodyEmptyType emptiness = qn->emptiness;
  int tlen = compile_length_tree(ND_QUANT_BODY(qn), reg, env);

  if (tlen < 0) return tlen;
  if (tlen == 0) return 0;

  /* anychar repeat */
  if (is_anychar_infinite_greedy(qn)) {
    if (qn->lower <= 1 ||
        len_multiply_cmp((OnigLen )tlen, qn->lower, QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0) {
      if (IS_NOT_NULL(qn->next_head_exact))
        return OPSIZE_ANYCHAR_STAR_PEEK_NEXT + tlen * qn->lower;
      else
        return OPSIZE_ANYCHAR_STAR + tlen * qn->lower;
    }
  }

  mod_tlen = tlen;
  if (emptiness != BODY_IS_NOT_EMPTY)
    mod_tlen += OPSIZE_EMPTY_CHECK_START + OPSIZE_EMPTY_CHECK_END;

  if (infinite &&
      (qn->lower <= 1 ||
       len_multiply_cmp(tlen, qn->lower, QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    if (qn->lower == 1 && tlen > QUANTIFIER_EXPAND_LIMIT_SIZE) {
      len = OPSIZE_JUMP;
    }
    else {
      len = tlen * qn->lower;
    }

    if (qn->greedy) {
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
      if (IS_NOT_NULL(qn->head_exact))
        len += OPSIZE_PUSH_OR_JUMP_EXACT1 + mod_tlen + OPSIZE_JUMP;
      else
#endif
      if (IS_NOT_NULL(qn->next_head_exact))
        len += OPSIZE_PUSH_IF_PEEK_NEXT + mod_tlen + OPSIZE_JUMP;
      else
        len += OPSIZE_PUSH + mod_tlen + OPSIZE_JUMP;
    }
    else
      len += OPSIZE_JUMP + mod_tlen + OPSIZE_PUSH;
  }
  else if (qn->upper == 0) {
    if (qn->include_referred != 0) { /* /(?<n>..){0}/ */
      len = OPSIZE_JUMP + tlen;
    }
    else
      len = 0;
  }
  else if (!infinite && qn->greedy &&
           (qn->upper == 1 ||
            len_multiply_cmp((OnigLen )tlen + OPSIZE_PUSH, qn->upper,
                             QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    len = tlen * qn->lower;
    len += (OPSIZE_PUSH + tlen) * (qn->upper - qn->lower);
  }
  else if (!qn->greedy && qn->upper == 1 && qn->lower == 0) { /* '??' */
    len = OPSIZE_PUSH + OPSIZE_JUMP + tlen;
  }
  else {
    len = OPSIZE_REPEAT_INC + mod_tlen + OPSIZE_REPEAT;
  }

  return len;
}

static int
compile_quantifier_node(QuantNode* qn, regex_t* reg, ParseEnv* env)
{
  int i, r, mod_tlen;
  int infinite = IS_INFINITE_REPEAT(qn->upper);
  enum BodyEmptyType emptiness = qn->emptiness;
  int tlen = compile_length_tree(ND_QUANT_BODY(qn), reg, env);

  if (tlen < 0) return tlen;
  if (tlen == 0) return 0;

  if (is_anychar_infinite_greedy(qn) &&
      (qn->lower <= 1 ||
       len_multiply_cmp((OnigLen )tlen, qn->lower,
                        QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    r = compile_tree_n_times(ND_QUANT_BODY(qn), qn->lower, reg, env);
    if (r != 0) return r;
    if (IS_NOT_NULL(qn->next_head_exact)) {
      r = add_op(reg, ND_IS_MULTILINE(ND_QUANT_BODY(qn)) ?
                 OP_ANYCHAR_ML_STAR_PEEK_NEXT : OP_ANYCHAR_STAR_PEEK_NEXT);
      if (r != 0) return r;

      COP(reg)->anychar_star_peek_next.c = STR_(qn->next_head_exact)->s[0];
      return 0;
    }
    else {
      r = add_op(reg, ND_IS_MULTILINE(ND_QUANT_BODY(qn)) ?
                 OP_ANYCHAR_ML_STAR : OP_ANYCHAR_STAR);
      return r;
    }
  }

  mod_tlen = tlen;
  if (emptiness != BODY_IS_NOT_EMPTY)
    mod_tlen += OPSIZE_EMPTY_CHECK_START + OPSIZE_EMPTY_CHECK_END;

  if (infinite &&
      (qn->lower <= 1 ||
       len_multiply_cmp((OnigLen )tlen, qn->lower,
                        QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    int addr;

    if (qn->lower == 1 && tlen > QUANTIFIER_EXPAND_LIMIT_SIZE) {
      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      if (qn->greedy) {
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
        if (IS_NOT_NULL(qn->head_exact))
          COP(reg)->jump.addr = OPSIZE_PUSH_OR_JUMP_EXACT1 + SIZE_INC;
        else
#endif
        if (IS_NOT_NULL(qn->next_head_exact))
          COP(reg)->jump.addr = OPSIZE_PUSH_IF_PEEK_NEXT + SIZE_INC;
        else
          COP(reg)->jump.addr = OPSIZE_PUSH + SIZE_INC;
      }
      else {
        COP(reg)->jump.addr = OPSIZE_JUMP + SIZE_INC;
      }
    }
    else {
      r = compile_tree_n_times(ND_QUANT_BODY(qn), qn->lower, reg, env);
      if (r != 0) return r;
    }

    if (qn->greedy) {
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
      if (IS_NOT_NULL(qn->head_exact)) {
        r = add_op(reg, OP_PUSH_OR_JUMP_EXACT1);
        if (r != 0) return r;
        COP(reg)->push_or_jump_exact1.addr = SIZE_INC + mod_tlen + OPSIZE_JUMP;
        COP(reg)->push_or_jump_exact1.c    = STR_(qn->head_exact)->s[0];

        r = compile_quant_body_with_empty_check(qn, reg, env);
        if (r != 0) return r;

        addr = -(mod_tlen + (int )OPSIZE_PUSH_OR_JUMP_EXACT1);
      }
      else
#endif
      if (IS_NOT_NULL(qn->next_head_exact)) {
        r = add_op(reg, OP_PUSH_IF_PEEK_NEXT);
        if (r != 0) return r;
        COP(reg)->push_if_peek_next.addr = SIZE_INC + mod_tlen + OPSIZE_JUMP;
        COP(reg)->push_if_peek_next.c    = STR_(qn->next_head_exact)->s[0];

        r = compile_quant_body_with_empty_check(qn, reg, env);
        if (r != 0) return r;

        addr = -(mod_tlen + (int )OPSIZE_PUSH_IF_PEEK_NEXT);
      }
      else {
        r = add_op(reg, OP_PUSH);
        if (r != 0) return r;
        COP(reg)->push.addr = SIZE_INC + mod_tlen + OPSIZE_JUMP;

        r = compile_quant_body_with_empty_check(qn, reg, env);
        if (r != 0) return r;

        addr = -(mod_tlen + (int )OPSIZE_PUSH);
      }

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = addr;
    }
    else {
      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = mod_tlen + SIZE_INC;

      r = compile_quant_body_with_empty_check(qn, reg, env);
      if (r != 0) return r;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = -mod_tlen;
    }
  }
  else if (qn->upper == 0) {
    if (qn->include_referred != 0) { /* /(?<n>..){0}/ */
      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = tlen + SIZE_INC;

      r = compile_tree(ND_QUANT_BODY(qn), reg, env);
    }
    else {
      /* Nothing output */
      r = 0;
    }
  }
  else if (! infinite && qn->greedy &&
           (qn->upper == 1 ||
            len_multiply_cmp((OnigLen )tlen + OPSIZE_PUSH, qn->upper,
                             QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    int n = qn->upper - qn->lower;

    r = compile_tree_n_times(ND_QUANT_BODY(qn), qn->lower, reg, env);
    if (r != 0) return r;

    for (i = 0; i < n; i++) {
      int v = onig_positive_int_multiply(n - i, tlen + OPSIZE_PUSH);
      if (v < 0) return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = v;

      r = compile_tree(ND_QUANT_BODY(qn), reg, env);
      if (r != 0) return r;
    }
  }
  else if (! qn->greedy && qn->upper == 1 && qn->lower == 0) { /* '??' */
    r = add_op(reg, OP_PUSH);
    if (r != 0) return r;
    COP(reg)->push.addr = SIZE_INC + OPSIZE_JUMP;

    r = add_op(reg, OP_JUMP);
    if (r != 0) return r;
    COP(reg)->jump.addr = tlen + SIZE_INC;

    r = compile_tree(ND_QUANT_BODY(qn), reg, env);
  }
  else {
    r = compile_range_repeat_node(qn, mod_tlen, emptiness, reg, env);
  }
  return r;
}

static int
compile_length_option_node(BagNode* node, regex_t* reg, ParseEnv* env)
{
  int tlen;

  tlen = compile_length_tree(ND_BAG_BODY(node), reg, env);

  return tlen;
}

static int
compile_option_node(BagNode* node, regex_t* reg, ParseEnv* env)
{
  int r;

  r = compile_tree(ND_BAG_BODY(node), reg, env);

  return r;
}

static int
compile_length_bag_node(BagNode* node, regex_t* reg, ParseEnv* env)
{
  int len;
  int tlen;

  if (node->type == BAG_OPTION)
    return compile_length_option_node(node, reg, env);

  if (ND_BAG_BODY(node)) {
    tlen = compile_length_tree(ND_BAG_BODY(node), reg, env);
    if (tlen < 0) return tlen;
  }
  else
    tlen = 0;

  switch (node->type) {
  case BAG_MEMORY:
#ifdef USE_CALL

    if (node->m.regnum == 0 && ND_IS_CALLED(node)) {
      len = tlen + OPSIZE_CALL + OPSIZE_JUMP + OPSIZE_RETURN;
      return len;
    }

    if (ND_IS_CALLED(node)) {
      len = OPSIZE_MEM_START_PUSH + tlen
        + OPSIZE_CALL + OPSIZE_JUMP + OPSIZE_RETURN;
      if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
        len += (ND_IS_RECURSION(node)
                ? OPSIZE_MEM_END_PUSH_REC : OPSIZE_MEM_END_PUSH);
      else
        len += (ND_IS_RECURSION(node)
                ? OPSIZE_MEM_END_REC : OPSIZE_MEM_END);
    }
    else if (ND_IS_RECURSION(node)) {
      len = OPSIZE_MEM_START_PUSH;
      len += tlen + (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum)
                     ? OPSIZE_MEM_END_PUSH_REC : OPSIZE_MEM_END_REC);
    }
    else
#endif
    {
      if (MEM_STATUS_AT0(reg->push_mem_start, node->m.regnum))
        len = OPSIZE_MEM_START_PUSH;
      else
        len = OPSIZE_MEM_START;

      len += tlen + (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum)
                     ? OPSIZE_MEM_END_PUSH : OPSIZE_MEM_END);
    }
    break;

  case BAG_STOP_BACKTRACK:
    if (ND_IS_STRICT_REAL_REPEAT(node)) {
      int v;
      QuantNode* qn;

      qn = QUANT_(ND_BAG_BODY(node));
      tlen = compile_length_tree(ND_QUANT_BODY(qn), reg, env);
      if (tlen < 0) return tlen;

      v = onig_positive_int_multiply(qn->lower, tlen);
      if (v < 0) return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;
      len = v + OPSIZE_PUSH + tlen + OPSIZE_POP + OPSIZE_JUMP;
    }
    else {
      len = OPSIZE_MARK + tlen + OPSIZE_CUT_TO_MARK;
    }
    break;

  case BAG_IF_ELSE:
    {
      Node* cond = ND_BAG_BODY(node);
      Node* Then = node->te.Then;
      Node* Else = node->te.Else;

      len = compile_length_tree(cond, reg, env);
      if (len < 0) return len;
      len += OPSIZE_PUSH + OPSIZE_MARK + OPSIZE_CUT_TO_MARK;

      if (IS_NOT_NULL(Then)) {
        tlen = compile_length_tree(Then, reg, env);
        if (tlen < 0) return tlen;
        len += tlen;
      }

      len += OPSIZE_JUMP + OPSIZE_CUT_TO_MARK;

      if (IS_NOT_NULL(Else)) {
        tlen = compile_length_tree(Else, reg, env);
        if (tlen < 0) return tlen;
        len += tlen;
      }
    }
    break;

  case BAG_OPTION:
    /* never come here, but set for escape warning */
    len = 0;
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return len;
}

static int
compile_bag_memory_node(BagNode* node, regex_t* reg, ParseEnv* env)
{
  int r;

#ifdef USE_CALL
  if (ND_IS_CALLED(node)) {
    int len;

    r = add_op(reg, OP_CALL);
    if (r != 0) return r;

    node->m.called_addr = COP_CURR_OFFSET(reg) + 1 + OPSIZE_JUMP;
    ND_STATUS_ADD(node, FIXED_ADDR);
    COP(reg)->call.addr = (int )node->m.called_addr;

    if (node->m.regnum == 0) {
      len = compile_length_tree(ND_BAG_BODY(node), reg, env);
      len += OPSIZE_RETURN;

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = len + SIZE_INC;

      r = compile_tree(ND_BAG_BODY(node), reg, env);
      if (r != 0) return r;

      r = add_op(reg, OP_RETURN);
      return r;
    }
    else {
      len = compile_length_tree(ND_BAG_BODY(node), reg, env);
      len += (OPSIZE_MEM_START_PUSH + OPSIZE_RETURN);
      if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
        len += (ND_IS_RECURSION(node)
                ? OPSIZE_MEM_END_PUSH_REC : OPSIZE_MEM_END_PUSH);
      else
        len += (ND_IS_RECURSION(node) ? OPSIZE_MEM_END_REC : OPSIZE_MEM_END);

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = len + SIZE_INC;
    }
  }
#endif

  if (MEM_STATUS_AT0(reg->push_mem_start, node->m.regnum))
    r = add_op(reg, OP_MEM_START_PUSH);
  else
    r = add_op(reg, OP_MEM_START);
  if (r != 0) return r;
  COP(reg)->memory_start.num = node->m.regnum;

  r = compile_tree(ND_BAG_BODY(node), reg, env);
  if (r != 0) return r;

#ifdef USE_CALL
  if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
    r = add_op(reg, (ND_IS_RECURSION(node)
                     ? OP_MEM_END_PUSH_REC : OP_MEM_END_PUSH));
  else
    r = add_op(reg, (ND_IS_RECURSION(node) ? OP_MEM_END_REC : OP_MEM_END));
  if (r != 0) return r;
  COP(reg)->memory_end.num = node->m.regnum;

  if (ND_IS_CALLED(node)) {
    r = add_op(reg, OP_RETURN);
  }
#else
  if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
    r = add_op(reg, OP_MEM_END_PUSH);
  else
    r = add_op(reg, OP_MEM_END);
  if (r != 0) return r;
  COP(reg)->memory_end.num = node->m.regnum;
#endif

  return r;
}

static int
compile_bag_node(BagNode* node, regex_t* reg, ParseEnv* env)
{
  int r, len;

  switch (node->type) {
  case BAG_MEMORY:
    r = compile_bag_memory_node(node, reg, env);
    break;

  case BAG_OPTION:
    r = compile_option_node(node, reg, env);
    break;

  case BAG_STOP_BACKTRACK:
    if (ND_IS_STRICT_REAL_REPEAT(node)) {
      QuantNode* qn = QUANT_(ND_BAG_BODY(node));
      r = compile_tree_n_times(ND_QUANT_BODY(qn), qn->lower, reg, env);
      if (r != 0) return r;

      len = compile_length_tree(ND_QUANT_BODY(qn), reg, env);
      if (len < 0) return len;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = SIZE_INC + len + OPSIZE_POP + OPSIZE_JUMP;

      r = compile_tree(ND_QUANT_BODY(qn), reg, env);
      if (r != 0) return r;
      r = add_op(reg, OP_POP);
      if (r != 0) return r;

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = -((int )OPSIZE_PUSH + len + (int )OPSIZE_POP);
    }
    else {
      MemNumType mid;

      ID_ENTRY(env, mid);
      r = add_op(reg, OP_MARK);
      if (r != 0) return r;
      COP(reg)->mark.id = mid;
      COP(reg)->mark.save_pos = 0;

      r = compile_tree(ND_BAG_BODY(node), reg, env);
      if (r != 0) return r;
      r = add_op(reg, OP_CUT_TO_MARK);
      if (r != 0) return r;
      COP(reg)->cut_to_mark.id = mid;
      COP(reg)->cut_to_mark.restore_pos = 0;
    }
    break;

  case BAG_IF_ELSE:
    {
      int cond_len, then_len, else_len, jump_len;
      MemNumType mid;
      Node* cond = ND_BAG_BODY(node);
      Node* Then = node->te.Then;
      Node* Else = node->te.Else;

      ID_ENTRY(env, mid);

      r = add_op(reg, OP_MARK);
      if (r != 0) return r;
      COP(reg)->mark.id = mid;
      COP(reg)->mark.save_pos = 0;

      cond_len = compile_length_tree(cond, reg, env);
      if (cond_len < 0) return cond_len;
      if (IS_NOT_NULL(Then)) {
        then_len = compile_length_tree(Then, reg, env);
        if (then_len < 0) return then_len;
      }
      else
        then_len = 0;

      jump_len = cond_len + then_len + OPSIZE_CUT_TO_MARK + OPSIZE_JUMP;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = SIZE_INC + jump_len;

      r = compile_tree(cond, reg, env);
      if (r != 0) return r;
      r = add_op(reg, OP_CUT_TO_MARK);
      if (r != 0) return r;
      COP(reg)->cut_to_mark.id = mid;
      COP(reg)->cut_to_mark.restore_pos = 0;

      if (IS_NOT_NULL(Then)) {
        r = compile_tree(Then, reg, env);
        if (r != 0) return r;
      }

      if (IS_NOT_NULL(Else)) {
        else_len = compile_length_tree(Else, reg, env);
        if (else_len < 0) return else_len;
      }
      else
        else_len = 0;

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = OPSIZE_CUT_TO_MARK + else_len + SIZE_INC;

      r = add_op(reg, OP_CUT_TO_MARK);
      if (r != 0) return r;
      COP(reg)->cut_to_mark.id = mid;
      COP(reg)->cut_to_mark.restore_pos = 0;

      if (IS_NOT_NULL(Else)) {
        r = compile_tree(Else, reg, env);
      }
    }
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return r;
}

static int
compile_length_anchor_node(AnchorNode* node, regex_t* reg, ParseEnv* env)
{
  int len;
  int tlen = 0;

  if (IS_NOT_NULL(ND_ANCHOR_BODY(node))) {
    tlen = compile_length_tree(ND_ANCHOR_BODY(node), reg, env);
    if (tlen < 0) return tlen;
  }

  switch (node->type) {
  case ANCR_PREC_READ:
    len = OPSIZE_MARK + tlen + OPSIZE_CUT_TO_MARK;
    break;
  case ANCR_PREC_READ_NOT:
    len = OPSIZE_PUSH + OPSIZE_MARK + tlen + OPSIZE_POP_TO_MARK + OPSIZE_POP + OPSIZE_FAIL;
    break;
  case ANCR_LOOK_BEHIND:
    if (node->char_min_len == node->char_max_len)
      len = OPSIZE_MARK + OPSIZE_STEP_BACK_START + tlen + OPSIZE_CUT_TO_MARK;
    else {
      len = OPSIZE_SAVE_VAL + OPSIZE_UPDATE_VAR + OPSIZE_MARK + OPSIZE_PUSH + OPSIZE_UPDATE_VAR + OPSIZE_FAIL + OPSIZE_JUMP + OPSIZE_STEP_BACK_START + OPSIZE_STEP_BACK_NEXT + tlen + OPSIZE_CHECK_POSITION + OPSIZE_CUT_TO_MARK + OPSIZE_UPDATE_VAR;

      if (IS_NOT_NULL(node->lead_node)) {
        int llen = compile_length_tree(node->lead_node, reg, env);
        if (llen < 0) return llen;

        len += OPSIZE_MOVE + llen;
      }

      if ((env->flags & PE_FLAG_HAS_ABSENT_STOPPER) != 0)
        len += OPSIZE_SAVE_VAL + OPSIZE_UPDATE_VAR;
    }
    break;
  case ANCR_LOOK_BEHIND_NOT:
    if (node->char_min_len == node->char_max_len)
      len = OPSIZE_MARK + OPSIZE_PUSH + OPSIZE_STEP_BACK_START + tlen + OPSIZE_POP_TO_MARK + OPSIZE_FAIL + OPSIZE_POP;
    else {
      len = OPSIZE_SAVE_VAL + OPSIZE_UPDATE_VAR + OPSIZE_MARK + OPSIZE_PUSH + OPSIZE_STEP_BACK_START + OPSIZE_STEP_BACK_NEXT + tlen + OPSIZE_CHECK_POSITION + OPSIZE_POP_TO_MARK + OPSIZE_UPDATE_VAR + OPSIZE_POP + OPSIZE_FAIL + OPSIZE_UPDATE_VAR + OPSIZE_POP + OPSIZE_POP;

      if (IS_NOT_NULL(node->lead_node)) {
        int llen = compile_length_tree(node->lead_node, reg, env);
        if (llen < 0) return llen;

        len += OPSIZE_MOVE + llen;
      }

      if ((env->flags & PE_FLAG_HAS_ABSENT_STOPPER) != 0)
        len += OPSIZE_SAVE_VAL + OPSIZE_UPDATE_VAR;
    }
    break;

  case ANCR_WORD_BOUNDARY:
  case ANCR_NO_WORD_BOUNDARY:
#ifdef USE_WORD_BEGIN_END
  case ANCR_WORD_BEGIN:
  case ANCR_WORD_END:
#endif
    len = OPSIZE_WORD_BOUNDARY;
    break;

  case ANCR_TEXT_SEGMENT_BOUNDARY:
  case ANCR_NO_TEXT_SEGMENT_BOUNDARY:
    len = SIZE_OPCODE;
    break;

  default:
    len = SIZE_OPCODE;
    break;
  }

  return len;
}

static int
compile_anchor_look_behind_node(AnchorNode* node, regex_t* reg, ParseEnv* env)
{
  int r;

  if (node->char_min_len == node->char_max_len) {
    MemNumType mid;

    ID_ENTRY(env, mid);
    r = add_op(reg, OP_MARK);
    if (r != 0) return r;
    COP(reg)->mark.id = mid;
    COP(reg)->mark.save_pos = FALSE;

    r = add_op(reg, OP_STEP_BACK_START);
    if (r != 0) return r;
    COP(reg)->step_back_start.initial   = node->char_min_len;
    COP(reg)->step_back_start.remaining = 0;
    COP(reg)->step_back_start.addr      = 1;

    r = compile_tree(ND_ANCHOR_BODY(node), reg, env);
    if (r != 0) return r;

    r = add_op(reg, OP_CUT_TO_MARK);
    if (r != 0) return r;
    COP(reg)->cut_to_mark.id = mid;
    COP(reg)->cut_to_mark.restore_pos = FALSE;
  }
  else {
    OnigLen diff;
    MemNumType mid1, mid2;
    MemNumType mid3 = 0; /* ignore uninitialized warning */

    if (IS_NOT_NULL(node->lead_node)) {
      MinMaxCharLen ci;

      r = node_char_len(node->lead_node, reg, &ci, env);
      if (r < 0) return r;
      r = add_op(reg, OP_MOVE);
      if (r != 0) return r;
      COP(reg)->move.n = -((RelPositionType )ci.min);
      r = compile_tree(node->lead_node, reg, env);
      if (r != 0) return r;
    }

    ID_ENTRY(env, mid1);
    r = add_op(reg, OP_SAVE_VAL);
    if (r != 0) return r;
    COP(reg)->save_val.type = SAVE_RIGHT_RANGE;
    COP(reg)->save_val.id   = mid1;

    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_TO_S;

    ID_ENTRY(env, mid2);
    r = add_op(reg, OP_MARK);
    if (r != 0) return r;
    COP(reg)->mark.id = mid2;
    COP(reg)->mark.save_pos = FALSE;

    r = add_op(reg, OP_PUSH);
    if (r != 0) return r;
    COP(reg)->push.addr = SIZE_INC + OPSIZE_JUMP;

    r = add_op(reg, OP_JUMP);
    if (r != 0) return r;
    COP(reg)->jump.addr = SIZE_INC + OPSIZE_UPDATE_VAR + OPSIZE_FAIL;

    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_FROM_STACK;
    COP(reg)->update_var.id    = mid1;
    COP(reg)->update_var.clear = FALSE;
    r = add_op(reg, OP_FAIL);
    if (r != 0) return r;

    if ((env->flags & PE_FLAG_HAS_ABSENT_STOPPER) != 0) {
      ID_ENTRY(env, mid3);
      r = add_op(reg, OP_SAVE_VAL);
      if (r != 0) return r;
      COP(reg)->save_val.type = SAVE_RIGHT_RANGE;
      COP(reg)->save_val.id   = mid3;
    }

    r = add_op(reg, OP_STEP_BACK_START);
    if (r != 0) return r;

    if (node->char_max_len != INFINITE_LEN)
      diff = node->char_max_len - node->char_min_len;
    else
      diff = INFINITE_LEN;

    COP(reg)->step_back_start.initial   = node->char_min_len;
    COP(reg)->step_back_start.remaining = diff;
    COP(reg)->step_back_start.addr      = 2;

    r = add_op(reg, OP_STEP_BACK_NEXT);
    if (r != 0) return r;

    r = compile_tree(ND_ANCHOR_BODY(node), reg, env);
    if (r != 0) return r;

    if ((env->flags & PE_FLAG_HAS_ABSENT_STOPPER) != 0) {
      r = add_op(reg, OP_UPDATE_VAR);
      if (r != 0) return r;
      COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_FROM_STACK;
      COP(reg)->update_var.id    = mid3;
      COP(reg)->update_var.clear = FALSE;
    }

    r = add_op(reg, OP_CHECK_POSITION);
    if (r != 0) return r;
    COP(reg)->check_position.type = CHECK_POSITION_CURRENT_RIGHT_RANGE;

    r = add_op(reg, OP_CUT_TO_MARK);
    if (r != 0) return r;
    COP(reg)->cut_to_mark.id = mid2;
    COP(reg)->cut_to_mark.restore_pos = FALSE;

    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_FROM_STACK;
    COP(reg)->update_var.id    = mid1;
    COP(reg)->update_var.clear = TRUE;
  }

  return r;
}

static int
compile_anchor_look_behind_not_node(AnchorNode* node, regex_t* reg,
                                    ParseEnv* env)
{
  int r;
  int len;

  len = compile_length_tree(ND_ANCHOR_BODY(node), reg, env);

  if (node->char_min_len == node->char_max_len) {
    MemNumType mid;

    ID_ENTRY(env, mid);
    r = add_op(reg, OP_MARK);
    if (r != 0) return r;
    COP(reg)->mark.id = mid;
    COP(reg)->mark.save_pos = FALSE;

    r = add_op(reg, OP_PUSH);
    if (r != 0) return r;
    COP(reg)->push.addr = SIZE_INC + OPSIZE_STEP_BACK_START + len + OPSIZE_POP_TO_MARK + OPSIZE_FAIL;

    r = add_op(reg, OP_STEP_BACK_START);
    if (r != 0) return r;
    COP(reg)->step_back_start.initial   = node->char_min_len;
    COP(reg)->step_back_start.remaining = 0;
    COP(reg)->step_back_start.addr      = 1;

    r = compile_tree(ND_ANCHOR_BODY(node), reg, env);
    if (r != 0) return r;

    r = add_op(reg, OP_POP_TO_MARK);
    if (r != 0) return r;
    COP(reg)->pop_to_mark.id = mid;
    r = add_op(reg, OP_FAIL);
    if (r != 0) return r;
    r = add_op(reg, OP_POP);
  }
  else {
    OnigLen diff;
    MemNumType mid1, mid2;
    MemNumType mid3 = 0; /* ignore uninitialized warning */

    ID_ENTRY(env, mid1);
    r = add_op(reg, OP_SAVE_VAL);
    if (r != 0) return r;
    COP(reg)->save_val.type = SAVE_RIGHT_RANGE;
    COP(reg)->save_val.id   = mid1;

    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_TO_S;

    ID_ENTRY(env, mid2);
    r = add_op(reg, OP_MARK);
    if (r != 0) return r;
    COP(reg)->mark.id = mid2;
    COP(reg)->mark.save_pos = FALSE;

    r = add_op(reg, OP_PUSH);
    if (r != 0) return r;

    COP(reg)->push.addr = SIZE_INC + OPSIZE_STEP_BACK_START + OPSIZE_STEP_BACK_NEXT + len + OPSIZE_CHECK_POSITION + OPSIZE_POP_TO_MARK + OPSIZE_UPDATE_VAR + OPSIZE_POP + OPSIZE_FAIL;
    if ((env->flags & PE_FLAG_HAS_ABSENT_STOPPER) != 0)
      COP(reg)->push.addr += OPSIZE_SAVE_VAL + OPSIZE_UPDATE_VAR;

    if (IS_NOT_NULL(node->lead_node)) {
      int clen;
      MinMaxCharLen ci;

      clen = compile_length_tree(node->lead_node, reg, env);
      COP(reg)->push.addr += OPSIZE_MOVE + clen;

      r = node_char_len(node->lead_node, reg, &ci, env);
      if (r < 0) return r;
      r = add_op(reg, OP_MOVE);
      if (r != 0) return r;
      COP(reg)->move.n = -((RelPositionType )ci.min);

      r = compile_tree(node->lead_node, reg, env);
      if (r != 0) return r;
    }

    if ((env->flags & PE_FLAG_HAS_ABSENT_STOPPER) != 0) {
      ID_ENTRY(env, mid3);
      r = add_op(reg, OP_SAVE_VAL);
      if (r != 0) return r;
      COP(reg)->save_val.type = SAVE_RIGHT_RANGE;
      COP(reg)->save_val.id   = mid3;
    }

    r = add_op(reg, OP_STEP_BACK_START);
    if (r != 0) return r;

    if (node->char_max_len != INFINITE_LEN)
      diff = node->char_max_len - node->char_min_len;
    else
      diff = INFINITE_LEN;

    COP(reg)->step_back_start.initial   = node->char_min_len;
    COP(reg)->step_back_start.remaining = diff;
    COP(reg)->step_back_start.addr      = 2;

    r = add_op(reg, OP_STEP_BACK_NEXT);
    if (r != 0) return r;

    r = compile_tree(ND_ANCHOR_BODY(node), reg, env);
    if (r != 0) return r;

    if ((env->flags & PE_FLAG_HAS_ABSENT_STOPPER) != 0) {
      r = add_op(reg, OP_UPDATE_VAR);
      if (r != 0) return r;
      COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_FROM_STACK;
      COP(reg)->update_var.id    = mid3;
      COP(reg)->update_var.clear = FALSE;
    }

    r = add_op(reg, OP_CHECK_POSITION);
    if (r != 0) return r;
    COP(reg)->check_position.type = CHECK_POSITION_CURRENT_RIGHT_RANGE;

    r = add_op(reg, OP_POP_TO_MARK);
    if (r != 0) return r;
    COP(reg)->pop_to_mark.id = mid2;

    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_FROM_STACK;
    COP(reg)->update_var.id   = mid1;
    COP(reg)->update_var.clear = FALSE;

    r = add_op(reg, OP_POP); /* pop save val */
    if (r != 0) return r;
    r = add_op(reg, OP_FAIL);
    if (r != 0) return r;

    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_FROM_STACK;
    COP(reg)->update_var.id   = mid1;
    COP(reg)->update_var.clear = FALSE;

    r = add_op(reg, OP_POP); /* pop mark */
    if (r != 0) return r;
    r = add_op(reg, OP_POP); /* pop save val */
  }

  return r;
}

static int
compile_anchor_node(AnchorNode* node, regex_t* reg, ParseEnv* env)
{
  int r, len;
  enum OpCode op;
  MemNumType mid;

  switch (node->type) {
  case ANCR_BEGIN_BUF:      r = add_op(reg, OP_BEGIN_BUF);      break;
  case ANCR_END_BUF:        r = add_op(reg, OP_END_BUF);        break;
  case ANCR_BEGIN_LINE:     r = add_op(reg, OP_BEGIN_LINE);     break;
  case ANCR_END_LINE:       r = add_op(reg, OP_END_LINE);       break;
  case ANCR_SEMI_END_BUF:   r = add_op(reg, OP_SEMI_END_BUF);   break;
  case ANCR_BEGIN_POSITION:
    r = add_op(reg, OP_CHECK_POSITION);
    if (r != 0) return r;
    COP(reg)->check_position.type = CHECK_POSITION_SEARCH_START;
    break;

  case ANCR_WORD_BOUNDARY:
    op = OP_WORD_BOUNDARY;
  word:
    r = add_op(reg, op);
    if (r != 0) return r;
    COP(reg)->word_boundary.mode = (ModeType )node->ascii_mode;
    break;

  case ANCR_NO_WORD_BOUNDARY:
    op = OP_NO_WORD_BOUNDARY; goto word;
    break;
#ifdef USE_WORD_BEGIN_END
  case ANCR_WORD_BEGIN:
    op = OP_WORD_BEGIN; goto word;
    break;
  case ANCR_WORD_END:
    op = OP_WORD_END; goto word;
    break;
#endif

  case ANCR_TEXT_SEGMENT_BOUNDARY:
  case ANCR_NO_TEXT_SEGMENT_BOUNDARY:
    {
      enum TextSegmentBoundaryType type;

      r = add_op(reg, OP_TEXT_SEGMENT_BOUNDARY);
      if (r != 0) return r;

      type = EXTENDED_GRAPHEME_CLUSTER_BOUNDARY;
#ifdef USE_UNICODE_WORD_BREAK
      if (ND_IS_TEXT_SEGMENT_WORD(node))
        type = WORD_BOUNDARY;
#endif

      COP(reg)->text_segment_boundary.type = type;
      COP(reg)->text_segment_boundary.not =
        (node->type == ANCR_NO_TEXT_SEGMENT_BOUNDARY ? 1 : 0);
    }
    break;

  case ANCR_PREC_READ:
    {
      ID_ENTRY(env, mid);
      r = add_op(reg, OP_MARK);
      if (r != 0) return r;
      COP(reg)->mark.id = mid;
      COP(reg)->mark.save_pos = TRUE;

      r = compile_tree(ND_ANCHOR_BODY(node), reg, env);
      if (r != 0) return r;

      r = add_op(reg, OP_CUT_TO_MARK);
      if (r != 0) return r;
      COP(reg)->cut_to_mark.id = mid;
      COP(reg)->cut_to_mark.restore_pos = TRUE;
    }
    break;

  case ANCR_PREC_READ_NOT:
    {
      len = compile_length_tree(ND_ANCHOR_BODY(node), reg, env);
      if (len < 0) return len;

      ID_ENTRY(env, mid);
      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = SIZE_INC + OPSIZE_MARK + len +
                            OPSIZE_POP_TO_MARK + OPSIZE_POP + OPSIZE_FAIL;

      r = add_op(reg, OP_MARK);
      if (r != 0) return r;
      COP(reg)->mark.id = mid;
      COP(reg)->mark.save_pos = FALSE;

      r = compile_tree(ND_ANCHOR_BODY(node), reg, env);
      if (r != 0) return r;

      r = add_op(reg, OP_POP_TO_MARK);
      if (r != 0) return r;
      COP(reg)->pop_to_mark.id = mid;

      r = add_op(reg, OP_POP);
      if (r != 0) return r;
      r = add_op(reg, OP_FAIL);
    }
    break;

  case ANCR_LOOK_BEHIND:
    r = compile_anchor_look_behind_node(node, reg, env);
    break;

  case ANCR_LOOK_BEHIND_NOT:
    r = compile_anchor_look_behind_not_node(node, reg, env);
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return r;
}

static int
compile_gimmick_node(GimmickNode* node, regex_t* reg)
{
  int r = 0;

  switch (node->type) {
  case GIMMICK_FAIL:
    r = add_op(reg, OP_FAIL);
    break;

  case GIMMICK_SAVE:
    r = add_op(reg, OP_SAVE_VAL);
    if (r != 0) return r;
    COP(reg)->save_val.type = node->detail_type;
    COP(reg)->save_val.id   = node->id;
    break;

  case GIMMICK_UPDATE_VAR:
    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = node->detail_type;
    COP(reg)->update_var.id   = node->id;
    COP(reg)->update_var.clear = FALSE;
    break;

#ifdef USE_CALLOUT
  case GIMMICK_CALLOUT:
    switch (node->detail_type) {
    case ONIG_CALLOUT_OF_CONTENTS:
    case ONIG_CALLOUT_OF_NAME:
      {
        if (node->detail_type == ONIG_CALLOUT_OF_NAME) {
          r = add_op(reg, OP_CALLOUT_NAME);
          if (r != 0) return r;
          COP(reg)->callout_name.id  = node->id;
          COP(reg)->callout_name.num = node->num;
        }
        else {
          r = add_op(reg, OP_CALLOUT_CONTENTS);
          if (r != 0) return r;
          COP(reg)->callout_contents.num = node->num;
        }
      }
      break;

    default:
      r = ONIGERR_TYPE_BUG;
      break;
    }
#endif
  }

  return r;
}

static int
compile_length_gimmick_node(GimmickNode* node, regex_t* reg)
{
  int len;

  switch (node->type) {
  case GIMMICK_FAIL:
    len = OPSIZE_FAIL;
    break;

  case GIMMICK_SAVE:
    len = OPSIZE_SAVE_VAL;
    break;

  case GIMMICK_UPDATE_VAR:
    len = OPSIZE_UPDATE_VAR;
    break;

#ifdef USE_CALLOUT
  case GIMMICK_CALLOUT:
    switch (node->detail_type) {
    case ONIG_CALLOUT_OF_CONTENTS:
      len = OPSIZE_CALLOUT_CONTENTS;
      break;
    case ONIG_CALLOUT_OF_NAME:
      len = OPSIZE_CALLOUT_NAME;
      break;

    default:
      len = ONIGERR_TYPE_BUG;
      break;
    }
    break;
#endif

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return len;
}

static int
compile_length_tree(Node* node, regex_t* reg, ParseEnv* env)
{
  int len, r;

  switch (ND_TYPE(node)) {
  case ND_LIST:
    len = 0;
    do {
      r = compile_length_tree(ND_CAR(node), reg, env);
      if (r < 0) return r;
      len += r;
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    r = len;
    break;

  case ND_ALT:
    {
      int n;

      n = r = 0;
      do {
        r += compile_length_tree(ND_CAR(node), reg, env);
        n++;
      } while (IS_NOT_NULL(node = ND_CDR(node)));
      r += (OPSIZE_PUSH + OPSIZE_JUMP) * (n - 1);
    }
    break;

  case ND_STRING:
    if (ND_STRING_IS_CRUDE(node))
      r = compile_length_string_crude_node(STR_(node), reg);
    else
      r = compile_length_string_node(node, reg);
    break;

  case ND_CCLASS:
    r = compile_length_cclass_node(CCLASS_(node), reg);
    break;

  case ND_CTYPE:
    r = SIZE_OPCODE;
    break;

  case ND_BACKREF:
    r = OPSIZE_BACKREF;
    break;

#ifdef USE_CALL
  case ND_CALL:
    r = OPSIZE_CALL;
    break;
#endif

  case ND_QUANT:
    r = compile_length_quantifier_node(QUANT_(node), reg, env);
    break;

  case ND_BAG:
    r = compile_length_bag_node(BAG_(node), reg, env);
    break;

  case ND_ANCHOR:
    r = compile_length_anchor_node(ANCHOR_(node), reg, env);
    break;

  case ND_GIMMICK:
    r = compile_length_gimmick_node(GIMMICK_(node), reg);
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return r;
}

static int
compile_tree(Node* node, regex_t* reg, ParseEnv* env)
{
  int n, len, pos, r = 0;

  switch (ND_TYPE(node)) {
  case ND_LIST:
    do {
      r = compile_tree(ND_CAR(node), reg, env);
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_ALT:
    {
      Node* x = node;
      len = 0;
      do {
        len += compile_length_tree(ND_CAR(x), reg, env);
        if (IS_NOT_NULL(ND_CDR(x))) {
          len += OPSIZE_PUSH + OPSIZE_JUMP;
        }
      } while (IS_NOT_NULL(x = ND_CDR(x)));
      pos = COP_CURR_OFFSET(reg) + 1 + len;  /* goal position */

      do {
        len = compile_length_tree(ND_CAR(node), reg, env);
        if (IS_NOT_NULL(ND_CDR(node))) {
          enum OpCode push = ND_IS_SUPER(node) ? OP_PUSH_SUPER : OP_PUSH;
          r = add_op(reg, push);
          if (r != 0) break;
          COP(reg)->push.addr = SIZE_INC + len + OPSIZE_JUMP;
        }
        r = compile_tree(ND_CAR(node), reg, env);
        if (r != 0) break;
        if (IS_NOT_NULL(ND_CDR(node))) {
          len = pos - (COP_CURR_OFFSET(reg) + 1);
          r = add_op(reg, OP_JUMP);
          if (r != 0) break;
          COP(reg)->jump.addr = len;
        }
      } while (IS_NOT_NULL(node = ND_CDR(node)));
    }
    break;

  case ND_STRING:
    if (ND_STRING_IS_CRUDE(node))
      r = compile_string_crude_node(STR_(node), reg);
    else
      r = compile_string_node(node, reg);
    break;

  case ND_CCLASS:
    r = compile_cclass_node(CCLASS_(node), reg);
    break;

  case ND_CTYPE:
    {
      int op;

      switch (CTYPE_(node)->ctype) {
      case CTYPE_ANYCHAR:
        r = add_op(reg, ND_IS_MULTILINE(node) ? OP_ANYCHAR_ML : OP_ANYCHAR);
        break;

      case ONIGENC_CTYPE_WORD:
        if (CTYPE_(node)->ascii_mode == 0) {
          op = CTYPE_(node)->not != 0 ? OP_NO_WORD : OP_WORD;
        }
        else {
          op = CTYPE_(node)->not != 0 ? OP_NO_WORD_ASCII : OP_WORD_ASCII;
        }
        r = add_op(reg, op);
        break;

      default:
        return ONIGERR_TYPE_BUG;
        break;
      }
    }
    break;

  case ND_BACKREF:
    {
      BackRefNode* br = BACKREF_(node);

      if (ND_IS_CHECKER(node)) {
#ifdef USE_BACKREF_WITH_LEVEL
        if (ND_IS_NEST_LEVEL(node)) {
          r = add_op(reg, OP_BACKREF_CHECK_WITH_LEVEL);
          if (r != 0) return r;
          COP(reg)->backref_general.nest_level = br->nest_level;
        }
        else
#endif
          {
            r = add_op(reg, OP_BACKREF_CHECK);
            if (r != 0) return r;
          }
        goto add_bacref_mems;
      }
      else {
#ifdef USE_BACKREF_WITH_LEVEL
        if (ND_IS_NEST_LEVEL(node)) {
          if (ND_IS_IGNORECASE(node))
            r = add_op(reg, OP_BACKREF_WITH_LEVEL_IC);
          else
            r = add_op(reg, OP_BACKREF_WITH_LEVEL);

          if (r != 0) return r;
          COP(reg)->backref_general.nest_level = br->nest_level;
          goto add_bacref_mems;
        }
        else
#endif
        if (br->back_num == 1) {
          n = br->back_static[0];
          if (ND_IS_IGNORECASE(node)) {
            r = add_op(reg, OP_BACKREF_N_IC);
            if (r != 0) return r;
            COP(reg)->backref_n.n1 = n;
          }
          else {
            switch (n) {
            case 1:  r = add_op(reg, OP_BACKREF1); break;
            case 2:  r = add_op(reg, OP_BACKREF2); break;
            default:
              r = add_op(reg, OP_BACKREF_N);
              if (r != 0) return r;
              COP(reg)->backref_n.n1 = n;
              break;
            }
          }
        }
        else {
          int num;
          int* p;

          r = add_op(reg, ND_IS_IGNORECASE(node) ?
                     OP_BACKREF_MULTI_IC : OP_BACKREF_MULTI);
          if (r != 0) return r;

        add_bacref_mems:
          num = br->back_num;
          COP(reg)->backref_general.num = num;
          if (num == 1) {
            COP(reg)->backref_general.n1 = br->back_static[0];
          }
          else {
            int i, j;
            MemNumType* ns;

            ns = xmalloc(sizeof(MemNumType) * num);
            CHECK_NULL_RETURN_MEMERR(ns);
            COP(reg)->backref_general.ns = ns;
            p = BACKREFS_P(br);
            for (i = num - 1, j = 0; i >= 0; i--, j++) {
              ns[j] = p[i];
            }
          }
        }
      }
    }
    break;

#ifdef USE_CALL
  case ND_CALL:
    r = compile_call(CALL_(node), reg, env);
    break;
#endif

  case ND_QUANT:
    r = compile_quantifier_node(QUANT_(node), reg, env);
    break;

  case ND_BAG:
    r = compile_bag_node(BAG_(node), reg, env);
    break;

  case ND_ANCHOR:
    r = compile_anchor_node(ANCHOR_(node), reg, env);
    break;

  case ND_GIMMICK:
    r = compile_gimmick_node(GIMMICK_(node), reg);
    break;

  default:
#ifdef ONIG_DEBUG
    fprintf(DBGFP, "compile_tree: undefined node type %d\n", ND_TYPE(node));
#endif
    break;
  }

  return r;
}

static int
make_named_capture_number_map(Node** plink, GroupNumMap* map, int* counter)
{
  int r;
  Node* node = *plink;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = make_named_capture_number_map(&(ND_CAR(node)), map, counter);
    } while (r >= 0 && IS_NOT_NULL(node = ND_CDR(node)));
    if (r < 0) return r;
    break;

  case ND_QUANT:
    {
      Node** ptarget = &(ND_BODY(node));
      r = make_named_capture_number_map(ptarget, map, counter);
      if (r < 0) return r;
      if (r == 1 && ND_TYPE(*ptarget) == ND_QUANT) {
        return onig_reduce_nested_quantifier(node);
      }
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);
      if (en->type == BAG_MEMORY) {
        if (ND_IS_NAMED_GROUP(node)) {
          (*counter)++;
          map[en->m.regnum].new_val = *counter;
          en->m.regnum = *counter;
          r = make_named_capture_number_map(&(ND_BODY(node)), map, counter);
          if (r < 0) return r;
        }
        else {
          *plink = ND_BODY(node);
          ND_BODY(node) = NULL_NODE;
          onig_node_free(node);
          r = make_named_capture_number_map(plink, map, counter);
          if (r < 0) return r;
          return 1;
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        r = make_named_capture_number_map(&(ND_BAG_BODY(en)), map, counter);
        if (r < 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = make_named_capture_number_map(&(en->te.Then), map, counter);
          if (r < 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = make_named_capture_number_map(&(en->te.Else), map, counter);
          if (r < 0) return r;
        }
      }
      else {
        r = make_named_capture_number_map(&(ND_BODY(node)), map, counter);
        if (r < 0) return r;
      }
    }
    break;

  case ND_ANCHOR:
    if (IS_NOT_NULL(ND_BODY(node))) {
      r = make_named_capture_number_map(&(ND_BODY(node)), map, counter);
      if (r < 0) return r;
    }
    break;

  default:
    break;
  }

  return 0;
}

static int
renumber_backref_node(Node* node, GroupNumMap* map)
{
  int i, pos, n, old_num;
  int *backs;
  BackRefNode* bn = BACKREF_(node);

  if (! ND_IS_BY_NAME(node))
    return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;

  old_num = bn->back_num;
  if (IS_NULL(bn->back_dynamic))
    backs = bn->back_static;
  else
    backs = bn->back_dynamic;

  for (i = 0, pos = 0; i < old_num; i++) {
    n = map[backs[i]].new_val;
    if (n > 0) {
      backs[pos] = n;
      pos++;
    }
  }

  bn->back_num = pos;
  return 0;
}

static int
renumber_backref_traverse(Node* node, GroupNumMap* map)
{
  int r = 0;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = renumber_backref_traverse(ND_CAR(node), map);
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_QUANT:
    r = renumber_backref_traverse(ND_BODY(node), map);
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      r = renumber_backref_traverse(ND_BODY(node), map);
      if (r != 0) return r;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = renumber_backref_traverse(en->te.Then, map);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = renumber_backref_traverse(en->te.Else, map);
          if (r != 0) return r;
        }
      }
    }
    break;

  case ND_BACKREF:
    r = renumber_backref_node(node, map);
    break;

  case ND_ANCHOR:
    if (IS_NOT_NULL(ND_BODY(node)))
      r = renumber_backref_traverse(ND_BODY(node), map);
    break;

  default:
    break;
  }

  return r;
}

static int
numbered_ref_check(Node* node)
{
  int r = 0;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = numbered_ref_check(ND_CAR(node));
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_ANCHOR:
    if (IS_NULL(ND_BODY(node)))
      break;
    /* fall */
  case ND_QUANT:
    r = numbered_ref_check(ND_BODY(node));
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      r = numbered_ref_check(ND_BODY(node));
      if (r != 0) return r;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = numbered_ref_check(en->te.Then);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = numbered_ref_check(en->te.Else);
          if (r != 0) return r;
        }
      }
    }

    break;

  case ND_BACKREF:
    if (! ND_IS_BY_NAME(node))
      return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;
    break;

  default:
    break;
  }

  return r;
}

static int
disable_noname_group_capture(Node** root, regex_t* reg, ParseEnv* env)
{
  int r, i, pos, counter;
  MemStatusType loc;
  GroupNumMap* map;

  map = (GroupNumMap* )xalloca(sizeof(GroupNumMap) * (env->num_mem + 1));
  CHECK_NULL_RETURN_MEMERR(map);
  for (i = 1; i <= env->num_mem; i++) {
    map[i].new_val = 0;
  }
  counter = 0;
  r = make_named_capture_number_map(root, map, &counter);
  if (r < 0) return r;

  r = renumber_backref_traverse(*root, map);
  if (r != 0) return r;

  for (i = 1, pos = 1; i <= env->num_mem; i++) {
    if (map[i].new_val > 0) {
      PARSEENV_MEMENV(env)[pos] = PARSEENV_MEMENV(env)[i];
      pos++;
    }
  }

  loc = env->cap_history;
  MEM_STATUS_CLEAR(env->cap_history);
  for (i = 1; i <= ONIG_MAX_CAPTURE_HISTORY_GROUP; i++) {
    if (MEM_STATUS_AT(loc, i)) {
      MEM_STATUS_ON_SIMPLE(env->cap_history, map[i].new_val);
    }
  }

  env->num_mem = env->num_named;
  reg->num_mem = env->num_named;

  return onig_renumber_name_table(reg, map);
}

#ifdef USE_CALL
static int
fix_unset_addr_list(UnsetAddrList* uslist, regex_t* reg)
{
  int i, offset;
  BagNode* en;
  AbsAddrType addr;
  AbsAddrType* paddr;

  for (i = 0; i < uslist->num; i++) {
    if (! ND_IS_FIXED_ADDR(uslist->us[i].target)) {
      if (ND_IS_CALLED(uslist->us[i].target))
        return ONIGERR_PARSER_BUG;
      else {
        /* CASE: called node doesn't have called address.
           ex. /((|a\g<1>)(.){0}){0}\g<3>/
           group-1 doesn't called, but compiled into bytecodes,
           because group-3 is referred from outside.
        */
        continue;
      }
    }

    en = BAG_(uslist->us[i].target);
    addr   = en->m.called_addr;
    offset = uslist->us[i].offset;

    paddr = (AbsAddrType* )((char* )reg->ops + offset);
    *paddr = addr;
  }
  return 0;
}
#endif

/* x is not included y ==>  1 : 0 */
static int
is_exclusive(Node* x, Node* y, regex_t* reg)
{
  int i, len;
  OnigCodePoint code;
  UChar *p;
  NodeType ytype;

 retry:
  ytype = ND_TYPE(y);
  switch (ND_TYPE(x)) {
  case ND_CTYPE:
    {
      if (CTYPE_(x)->ctype == CTYPE_ANYCHAR ||
          CTYPE_(y)->ctype == CTYPE_ANYCHAR)
        break;

      switch (ytype) {
      case ND_CTYPE:
        if (CTYPE_(y)->ctype == CTYPE_(x)->ctype &&
            CTYPE_(y)->not   != CTYPE_(x)->not &&
            CTYPE_(y)->ascii_mode == CTYPE_(x)->ascii_mode)
          return 1;
        else
          return 0;
        break;

      case ND_CCLASS:
      swap:
        {
          Node* tmp;
          tmp = x; x = y; y = tmp;
          goto retry;
        }
        break;

      case ND_STRING:
        goto swap;
        break;

      default:
        break;
      }
    }
    break;

  case ND_CCLASS:
    {
      int range;
      CClassNode* xc = CCLASS_(x);

      switch (ytype) {
      case ND_CTYPE:
        switch (CTYPE_(y)->ctype) {
        case CTYPE_ANYCHAR:
          return 0;
          break;

        case ONIGENC_CTYPE_WORD:
          if (CTYPE_(y)->not == 0) {
            if (IS_NULL(xc->mbuf) && !IS_NCCLASS_NOT(xc)) {
              range = CTYPE_(y)->ascii_mode != 0 ? 128 : SINGLE_BYTE_SIZE;
              for (i = 0; i < range; i++) {
                if (BITSET_AT(xc->bs, i)) {
                  if (ONIGENC_IS_CODE_WORD(reg->enc, i)) return 0;
                }
              }
              return 1;
            }
            return 0;
          }
          else {
            if (IS_NOT_NULL(xc->mbuf)) return 0;
            if (IS_NCCLASS_NOT(xc)) return 0;

            range = CTYPE_(y)->ascii_mode != 0 ? 128 : SINGLE_BYTE_SIZE;
            for (i = 0; i < range; i++) {
              if (! ONIGENC_IS_CODE_WORD(reg->enc, i)) {
                if (BITSET_AT(xc->bs, i))
                  return 0;
              }
            }
            for (i = range; i < SINGLE_BYTE_SIZE; i++) {
              if (BITSET_AT(xc->bs, i)) return 0;
            }
            return 1;
          }
          break;

        default:
          break;
        }
        break;

      case ND_CCLASS:
        {
          int v;
          CClassNode* yc = CCLASS_(y);

          for (i = 0; i < SINGLE_BYTE_SIZE; i++) {
            v = BITSET_AT(xc->bs, i);
            if ((v != 0 && !IS_NCCLASS_NOT(xc)) || (v == 0 && IS_NCCLASS_NOT(xc))) {
              v = BITSET_AT(yc->bs, i);
              if ((v != 0 && !IS_NCCLASS_NOT(yc)) ||
                  (v == 0 && IS_NCCLASS_NOT(yc)))
                return 0;
            }
          }
          if ((IS_NULL(xc->mbuf) && !IS_NCCLASS_NOT(xc)) ||
              (IS_NULL(yc->mbuf) && !IS_NCCLASS_NOT(yc)))
            return 1;
          return 0;
        }
        break;

      case ND_STRING:
        goto swap;
        break;

      default:
        break;
      }
    }
    break;

  case ND_STRING:
    {
      StrNode* xs = STR_(x);

      if (ND_STRING_LEN(x) == 0)
        break;

      switch (ytype) {
      case ND_CTYPE:
        switch (CTYPE_(y)->ctype) {
        case CTYPE_ANYCHAR:
          break;

        case ONIGENC_CTYPE_WORD:
          if (CTYPE_(y)->ascii_mode == 0) {
            if (ONIGENC_IS_MBC_WORD(reg->enc, xs->s, xs->end))
              return CTYPE_(y)->not;
            else
              return !(CTYPE_(y)->not);
          }
          else {
            if (ONIGENC_IS_MBC_WORD_ASCII(reg->enc, xs->s, xs->end))
              return CTYPE_(y)->not;
            else
              return !(CTYPE_(y)->not);
          }
          break;
        default:
          break;
        }
        break;

      case ND_CCLASS:
        {
          CClassNode* cc = CCLASS_(y);

          code = ONIGENC_MBC_TO_CODE(reg->enc, xs->s,
                                     xs->s + ONIGENC_MBC_MAXLEN(reg->enc));
          return onig_is_code_in_cc(reg->enc, code, cc) == 0;
        }
        break;

      case ND_STRING:
        {
          UChar *q;
          StrNode* ys = STR_(y);

          len = ND_STRING_LEN(x);
          if (len > ND_STRING_LEN(y)) len = ND_STRING_LEN(y);

          for (i = 0, p = ys->s, q = xs->s; i < len; i++, p++, q++) {
            if (*p != *q) return 1;
          }
        }
        break;

      default:
        break;
      }
    }
    break;

  default:
    break;
  }

  return 0;
}

static Node*
get_tree_head_literal(Node* node, int exact, regex_t* reg)
{
  Node* n = NULL_NODE;

  switch (ND_TYPE(node)) {
  case ND_BACKREF:
  case ND_ALT:
#ifdef USE_CALL
  case ND_CALL:
#endif
    break;

  case ND_CTYPE:
    if (CTYPE_(node)->ctype == CTYPE_ANYCHAR)
      break;
    /* fall */
  case ND_CCLASS:
    if (exact == 0) {
      n = node;
    }
    break;

  case ND_LIST:
    n = get_tree_head_literal(ND_CAR(node), exact, reg);
    break;

  case ND_STRING:
    {
      StrNode* sn = STR_(node);

      if (sn->end <= sn->s)
        break;

      if (exact == 0 || !ND_IS_REAL_IGNORECASE(node)) {
        n = node;
      }
    }
    break;

  case ND_QUANT:
    {
      QuantNode* qn = QUANT_(node);
      if (qn->lower > 0) {
        if (IS_NOT_NULL(qn->head_exact))
          n = qn->head_exact;
        else
          n = get_tree_head_literal(ND_BODY(node), exact, reg);
      }
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_OPTION:
      case BAG_MEMORY:
      case BAG_STOP_BACKTRACK:
        n = get_tree_head_literal(ND_BODY(node), exact, reg);
        break;
      default:
        break;
      }
    }
    break;

  case ND_ANCHOR:
    if (ANCHOR_(node)->type == ANCR_PREC_READ)
      n = get_tree_head_literal(ND_BODY(node), exact, reg);
    break;

  case ND_GIMMICK:
  default:
    break;
  }

  return n;
}

enum GetValue {
  GET_VALUE_NONE   = -1,
  GET_VALUE_IGNORE =  0,
  GET_VALUE_FOUND  =  1
};

#define MAX_NEST_LEVEL_GET_TREE_TAIL_LITERAL  16

static int
get_tree_tail_literal(Node* node, Node** rnode, regex_t* reg, int nest_level)
{
  int r;

  nest_level++;
  if (nest_level >= MAX_NEST_LEVEL_GET_TREE_TAIL_LITERAL) {
    return GET_VALUE_NONE;
  }

  switch (ND_TYPE(node)) {
  case ND_LIST:
    if (IS_NULL(ND_CDR(node))) {
      r = get_tree_tail_literal(ND_CAR(node), rnode, reg, nest_level);
    }
    else {
      r = get_tree_tail_literal(ND_CDR(node), rnode, reg, nest_level);
      if (r == GET_VALUE_IGNORE) {
        r = get_tree_tail_literal(ND_CAR(node), rnode, reg, nest_level);
      }
    }
    break;

#ifdef USE_CALL
  case ND_CALL:
    r = get_tree_tail_literal(ND_BODY(node), rnode, reg, nest_level);
    break;
#endif

  case ND_CTYPE:
    if (CTYPE_(node)->ctype == CTYPE_ANYCHAR) {
      r = GET_VALUE_NONE;
      break;
    }
    /* fall */
  case ND_CCLASS:
    *rnode = node;
    r = GET_VALUE_FOUND;
    break;

  case ND_STRING:
    {
      StrNode* sn = STR_(node);

      if (sn->end <= sn->s) {
        r = GET_VALUE_IGNORE;
        break;
      }

      if (ND_IS_REAL_IGNORECASE(node)) {
        r = GET_VALUE_NONE;
        break;
      }

      *rnode = node;
      r = GET_VALUE_FOUND;
    }
    break;

  case ND_QUANT:
    {
      QuantNode* qn = QUANT_(node);
      if (qn->lower != 0) {
        r = get_tree_tail_literal(ND_BODY(node), rnode, reg, nest_level);
      }
      else
        r = GET_VALUE_NONE;
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (ND_IS_MARK1(node))
          r = GET_VALUE_NONE;
        else {
          ND_STATUS_ADD(node, MARK1);
          r = get_tree_tail_literal(ND_BODY(node), rnode, reg, nest_level);
          ND_STATUS_REMOVE(node, MARK1);
        }
      }
      else {
        r = get_tree_tail_literal(ND_BODY(node), rnode, reg, nest_level);
      }
    }
    break;

  case ND_ANCHOR:
  case ND_GIMMICK:
    r = GET_VALUE_IGNORE;
    break;

  case ND_ALT:
  case ND_BACKREF:
  default:
    r = GET_VALUE_NONE;
    break;
  }

  return r;
}

static int
check_called_node_in_look_behind(Node* node, int not)
{
  int r;

  r = 0;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = check_called_node_in_look_behind(ND_CAR(node), not);
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_QUANT:
    r = check_called_node_in_look_behind(ND_BODY(node), not);
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (ND_IS_MARK1(node))
          return 0;
        else {
          ND_STATUS_ADD(node, MARK1);
          r = check_called_node_in_look_behind(ND_BODY(node), not);
          ND_STATUS_REMOVE(node, MARK1);
        }
      }
      else {
        r = check_called_node_in_look_behind(ND_BODY(node), not);
        if (r == 0 && en->type == BAG_IF_ELSE) {
          if (IS_NOT_NULL(en->te.Then)) {
            r = check_called_node_in_look_behind(en->te.Then, not);
            if (r != 0) break;
          }
          if (IS_NOT_NULL(en->te.Else)) {
            r = check_called_node_in_look_behind(en->te.Else, not);
          }
        }
      }
    }
    break;

  case ND_ANCHOR:
    if (IS_NOT_NULL(ND_BODY(node)))
      r = check_called_node_in_look_behind(ND_BODY(node), not);
    break;

  case ND_GIMMICK:
    if (ND_IS_ABSENT_WITH_SIDE_EFFECTS(node) != 0)
      return 1;
    break;

  default:
    break;
  }

  return r;
}

/* allowed node types in look-behind */
#define ALLOWED_TYPE_IN_LB \
  ( ND_BIT_LIST | ND_BIT_ALT | ND_BIT_STRING | ND_BIT_CCLASS \
  | ND_BIT_CTYPE | ND_BIT_ANCHOR | ND_BIT_BAG | ND_BIT_QUANT \
  | ND_BIT_CALL | ND_BIT_BACKREF | ND_BIT_GIMMICK)

#define ALLOWED_BAG_IN_LB       ( 1<<BAG_MEMORY | 1<<BAG_OPTION | 1<<BAG_STOP_BACKTRACK | 1<<BAG_IF_ELSE )
#define ALLOWED_BAG_IN_LB_NOT   ( 1<<BAG_OPTION | 1<<BAG_STOP_BACKTRACK | 1<<BAG_IF_ELSE )

#define ALLOWED_ANCHOR_IN_LB \
  ( ANCR_LOOK_BEHIND | ANCR_BEGIN_LINE | ANCR_END_LINE | ANCR_BEGIN_BUF \
  | ANCR_BEGIN_POSITION | ANCR_WORD_BOUNDARY | ANCR_NO_WORD_BOUNDARY \
  | ANCR_WORD_BEGIN | ANCR_WORD_END \
  | ANCR_TEXT_SEGMENT_BOUNDARY | ANCR_NO_TEXT_SEGMENT_BOUNDARY )

#define ALLOWED_ANCHOR_IN_LB_NOT \
  ( ANCR_LOOK_BEHIND | ANCR_LOOK_BEHIND_NOT | ANCR_BEGIN_LINE \
  | ANCR_END_LINE | ANCR_BEGIN_BUF | ANCR_BEGIN_POSITION | ANCR_WORD_BOUNDARY \
  | ANCR_NO_WORD_BOUNDARY | ANCR_WORD_BEGIN | ANCR_WORD_END \
  | ANCR_TEXT_SEGMENT_BOUNDARY | ANCR_NO_TEXT_SEGMENT_BOUNDARY )


static int
check_node_in_look_behind(Node* node, int not, int* used)
{
  static unsigned int
    bag_mask[2] = { ALLOWED_BAG_IN_LB, ALLOWED_BAG_IN_LB_NOT };

  static unsigned int
    anchor_mask[2] = { ALLOWED_ANCHOR_IN_LB, ALLOWED_ANCHOR_IN_LB_NOT };

  NodeType type;
  int r = 0;

  type = ND_TYPE(node);
  if ((ND_TYPE2BIT(type) & ALLOWED_TYPE_IN_LB) == 0)
    return 1;

  switch (type) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = check_node_in_look_behind(ND_CAR(node), not, used);
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_QUANT:
    r = check_node_in_look_behind(ND_BODY(node), not, used);
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);
      if (((1<<en->type) & bag_mask[not]) == 0)
        return 1;

      r = check_node_in_look_behind(ND_BODY(node), not, used);
      if (r != 0) break;

      if (en->type == BAG_MEMORY) {
        if (ND_IS_BACKREF(node) || ND_IS_CALLED(node)
         || ND_IS_REFERENCED(node))
          *used = TRUE;
      }
      else if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = check_node_in_look_behind(en->te.Then, not, used);
          if (r != 0) break;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = check_node_in_look_behind(en->te.Else, not, used);
        }
      }
    }
    break;

  case ND_ANCHOR:
    type = ANCHOR_(node)->type;
    if ((type & anchor_mask[not]) == 0)
      return 1;

    if (IS_NOT_NULL(ND_BODY(node)))
      r = check_node_in_look_behind(ND_BODY(node), not, used);
    break;

  case ND_GIMMICK:
    if (ND_IS_ABSENT_WITH_SIDE_EFFECTS(node) != 0)
      return 1;

    {
      GimmickNode* g = GIMMICK_(node);
      if (g->type == GIMMICK_SAVE && g->detail_type == SAVE_KEEP)
        *used = TRUE;
    }
    break;

  case ND_CALL:
    if (ND_IS_RECURSION(node)) {
      /* fix: Issue 38040 in oss-fuzz */
      /* This node should be removed before recursive call check. */
      *used = TRUE;
    }
    else
      r = check_called_node_in_look_behind(ND_BODY(node), not);
    break;

  default:
    break;
  }
  return r;
}

static OnigLen
node_min_byte_len(Node* node, ParseEnv* env)
{
  OnigLen len;
  OnigLen tmin;

  len = 0;
  switch (ND_TYPE(node)) {
  case ND_BACKREF:
    if (! ND_IS_CHECKER(node)) {
      int i;
      int* backs;
      MemEnv* mem_env = PARSEENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);
      if (ND_IS_RECURSION(node)) break;

      backs = BACKREFS_P(br);
      len = node_min_byte_len(mem_env[backs[0]].mem_node, env);
      for (i = 1; i < br->back_num; i++) {
        tmin = node_min_byte_len(mem_env[backs[i]].mem_node, env);
        if (len > tmin) len = tmin;
      }
    }
    break;

#ifdef USE_CALL
  case ND_CALL:
    {
      Node* t = ND_BODY(node);
      if (ND_IS_FIXED_MIN(t))
        len = BAG_(t)->min_len;
      else
        len = node_min_byte_len(t, env);
    }
    break;
#endif

  case ND_LIST:
    do {
      tmin = node_min_byte_len(ND_CAR(node), env);
      len = distance_add(len, tmin);
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_ALT:
    {
      Node *x, *y;
      y = node;
      do {
        x = ND_CAR(y);
        tmin = node_min_byte_len(x, env);
        if (y == node) len = tmin;
        else if (len > tmin) len = tmin;
      } while (IS_NOT_NULL(y = ND_CDR(y)));
    }
    break;

  case ND_STRING:
    {
      StrNode* sn = STR_(node);
      len = (int )(sn->end - sn->s);
    }
    break;

  case ND_CTYPE:
  case ND_CCLASS:
    len = ONIGENC_MBC_MINLEN(env->enc);
    break;

  case ND_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->lower > 0) {
        len = node_min_byte_len(ND_BODY(node), env);
        len = distance_multiply(len, qn->lower);
      }
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_MEMORY:
        if (ND_IS_FIXED_MIN(node))
          len = en->min_len;
        else {
          if (ND_IS_MARK1(node))
            len = 0;  /* recursive */
          else {
            ND_STATUS_ADD(node, MARK1);
            len = node_min_byte_len(ND_BODY(node), env);
            ND_STATUS_REMOVE(node, MARK1);

            en->min_len = len;
            ND_STATUS_ADD(node, FIXED_MIN);
          }
        }
        break;

      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        len = node_min_byte_len(ND_BODY(node), env);
        break;
      case BAG_IF_ELSE:
        {
          OnigLen elen;

          len = node_min_byte_len(ND_BODY(node), env);
          if (IS_NOT_NULL(en->te.Then))
            len += node_min_byte_len(en->te.Then, env);
          if (IS_NOT_NULL(en->te.Else))
            elen = node_min_byte_len(en->te.Else, env);
          else elen = 0;

          if (elen < len) len = elen;
        }
        break;
      }
    }
    break;

  case ND_GIMMICK:
    {
      GimmickNode* g = GIMMICK_(node);
      if (g->type == GIMMICK_FAIL) {
        len = INFINITE_LEN;
        break;
      }
    }
    /* fall */
  case ND_ANCHOR:
  default:
    break;
  }

  return len;
}

static int
check_backrefs(Node* node, ParseEnv* env)
{
  int r;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = check_backrefs(ND_CAR(node), env);
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) {
      r = 0;
      break;
    }
    /* fall */
  case ND_QUANT:
    r = check_backrefs(ND_BODY(node), env);
    break;

  case ND_BAG:
    r = check_backrefs(ND_BODY(node), env);
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_IF_ELSE) {
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = check_backrefs(en->te.Then, env);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = check_backrefs(en->te.Else, env);
        }
      }
    }
    break;

  case ND_BACKREF:
    {
      int i;
      BackRefNode* br = BACKREF_(node);
      int* backs = BACKREFS_P(br);
      MemEnv* mem_env = PARSEENV_MEMENV(env);

      for (i = 0; i < br->back_num; i++) {
        if (backs[i] > env->num_mem)
          return ONIGERR_INVALID_BACKREF;

        ND_STATUS_ADD(mem_env[backs[i]].mem_node, BACKREF);
      }
      r = 0;
    }
    break;

  default:
    r = 0;
    break;
  }

  return r;
}

static int
set_empty_repeat_node_trav(Node* node, Node* empty, ParseEnv* env)
{
  int r;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = set_empty_repeat_node_trav(ND_CAR(node), empty, env);
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);

      if (! ANCHOR_HAS_BODY(an)) {
        r = 0;
        break;
      }

      switch (an->type) {
      case ANCR_PREC_READ:
      case ANCR_LOOK_BEHIND:
        empty = NULL_NODE;
        break;
      default:
        break;
      }
      r = set_empty_repeat_node_trav(ND_BODY(node), empty, env);
    }
    break;

  case ND_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->emptiness != BODY_IS_NOT_EMPTY) empty = node;
      r = set_empty_repeat_node_trav(ND_BODY(node), empty, env);
    }
    break;

  case ND_BAG:
    if (IS_NOT_NULL(ND_BODY(node))) {
      r = set_empty_repeat_node_trav(ND_BODY(node), empty, env);
      if (r != 0) return r;
    }
    {
      BagNode* en = BAG_(node);

      r = 0;
      if (en->type == BAG_MEMORY) {
        if (ND_IS_BACKREF(node)) {
          if (IS_NOT_NULL(empty))
            PARSEENV_MEMENV(env)[en->m.regnum].empty_repeat_node = empty;
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = set_empty_repeat_node_trav(en->te.Then, empty, env);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = set_empty_repeat_node_trav(en->te.Else, empty, env);
        }
      }
    }
    break;

  default:
    r = 0;
    break;
  }

  return r;
}

static int
is_ancestor_node(Node* node, Node* me)
{
  Node* parent;

  while ((parent = ND_PARENT(me)) != NULL_NODE) {
    if (parent == node) return 1;
    me = parent;
  }
  return 0;
}

static void
set_empty_status_check_trav(Node* node, ParseEnv* env)
{
  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      set_empty_status_check_trav(ND_CAR(node), env);
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);

      if (! ANCHOR_HAS_BODY(an)) break;
      set_empty_status_check_trav(ND_BODY(node), env);
    }
    break;

  case ND_QUANT:
    set_empty_status_check_trav(ND_BODY(node), env);
    break;

  case ND_BAG:
    if (IS_NOT_NULL(ND_BODY(node)))
      set_empty_status_check_trav(ND_BODY(node), env);
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          set_empty_status_check_trav(en->te.Then, env);
        }
        if (IS_NOT_NULL(en->te.Else)) {
          set_empty_status_check_trav(en->te.Else, env);
        }
      }
    }
    break;

  case ND_BACKREF:
    {
      int i;
      int* backs;
      MemEnv* mem_env = PARSEENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);
      backs = BACKREFS_P(br);
      for (i = 0; i < br->back_num; i++) {
        Node* ernode = mem_env[backs[i]].empty_repeat_node;
        if (IS_NOT_NULL(ernode)) {
          if (! is_ancestor_node(ernode, node)) {
            MEM_STATUS_LIMIT_ON(QUANT_(ernode)->empty_status_mem, backs[i]);
            ND_STATUS_ADD(ernode, EMPTY_STATUS_CHECK);
            ND_STATUS_ADD(mem_env[backs[i]].mem_node, EMPTY_STATUS_CHECK);
          }
        }
      }
    }
    break;

  default:
    break;
  }
}

static void
set_parent_node_trav(Node* node, Node* parent)
{
  ND_PARENT(node) = parent;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      set_parent_node_trav(ND_CAR(node), node);
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) break;
    set_parent_node_trav(ND_BODY(node), node);
    break;

  case ND_QUANT:
    set_parent_node_trav(ND_BODY(node), node);
    break;

  case ND_BAG:
    if (IS_NOT_NULL(ND_BODY(node)))
      set_parent_node_trav(ND_BODY(node), node);
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then))
          set_parent_node_trav(en->te.Then, node);
        if (IS_NOT_NULL(en->te.Else)) {
          set_parent_node_trav(en->te.Else, node);
        }
      }
    }
    break;

  default:
    break;
  }
}


#ifdef USE_CALL

#define RECURSION_EXIST        (1<<0)
#define RECURSION_MUST         (1<<1)
#define RECURSION_INFINITE     (1<<2)

static int
infinite_recursive_call_check(Node* node, ParseEnv* env, int head)
{
  int ret;
  int r = 0;

  switch (ND_TYPE(node)) {
  case ND_LIST:
    {
      Node *x;
      OnigLen min;

      x = node;
      do {
        ret = infinite_recursive_call_check(ND_CAR(x), env, head);
        if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;
        r |= ret;
        if (head != 0) {
          min = node_min_byte_len(ND_CAR(x), env);
          if (min != 0) head = 0;
        }
      } while (IS_NOT_NULL(x = ND_CDR(x)));
    }
    break;

  case ND_ALT:
    {
      int must;

      must = RECURSION_MUST;
      do {
        ret = infinite_recursive_call_check(ND_CAR(node), env, head);
        if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;

        r    |= (ret & RECURSION_EXIST);
        must &= ret;
      } while (IS_NOT_NULL(node = ND_CDR(node)));
      r |= must;
    }
    break;

  case ND_QUANT:
    if (QUANT_(node)->upper == 0) break;

    r = infinite_recursive_call_check(ND_BODY(node), env, head);
    if (r < 0) return r;
    if ((r & RECURSION_MUST) != 0) {
      if (QUANT_(node)->lower == 0)
        r &= ~RECURSION_MUST;
    }
    break;

  case ND_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node)))
      break;
    /* fall */
  case ND_CALL:
    r = infinite_recursive_call_check(ND_BODY(node), env, head);
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (ND_IS_MARK2(node))
          return 0;
        else if (ND_IS_MARK1(node))
          return (head == 0 ? RECURSION_EXIST | RECURSION_MUST
                  : RECURSION_EXIST | RECURSION_MUST | RECURSION_INFINITE);
        else {
          ND_STATUS_ADD(node, MARK2);
          r = infinite_recursive_call_check(ND_BODY(node), env, head);
          ND_STATUS_REMOVE(node, MARK2);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        int eret;

        ret = infinite_recursive_call_check(ND_BODY(node), env, head);
        if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;
        r |= ret;
        if (IS_NOT_NULL(en->te.Then)) {
          OnigLen min;
          if (head != 0) {
            min = node_min_byte_len(ND_BODY(node), env);
          }
          else min = 0;

          ret = infinite_recursive_call_check(en->te.Then, env, min != 0 ? 0:head);
          if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;
          r |= ret;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          eret = infinite_recursive_call_check(en->te.Else, env, head);
          if (eret < 0 || (eret & RECURSION_INFINITE) != 0) return eret;
          r |= (eret & RECURSION_EXIST);
          if ((eret & RECURSION_MUST) == 0)
            r &= ~RECURSION_MUST;
        }
        else {
          r &= ~RECURSION_MUST;
        }
      }
      else {
        r = infinite_recursive_call_check(ND_BODY(node), env, head);
      }
    }
    break;

  default:
    break;
  }

  return r;
}

static int
infinite_recursive_call_check_trav(Node* node, ParseEnv* env)
{
  int r;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = infinite_recursive_call_check_trav(ND_CAR(node), env);
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) {
      r = 0;
      break;
    }
    /* fall */
  case ND_QUANT:
    r = infinite_recursive_call_check_trav(ND_BODY(node), env);
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (ND_IS_RECURSION(node) && ND_IS_CALLED(node)) {
          int ret;

          ND_STATUS_ADD(node, MARK1);

          ret = infinite_recursive_call_check(ND_BODY(node), env, 1);
          if (ret < 0) return ret;
          else if ((ret & (RECURSION_MUST | RECURSION_INFINITE)) != 0)
            return ONIGERR_NEVER_ENDING_RECURSION;

          ND_STATUS_REMOVE(node, MARK1);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = infinite_recursive_call_check_trav(en->te.Then, env);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = infinite_recursive_call_check_trav(en->te.Else, env);
          if (r != 0) return r;
        }
      }
    }

    r = infinite_recursive_call_check_trav(ND_BODY(node), env);
    break;

  default:
    r = 0;
    break;
  }

  return r;
}

static int
recursive_call_check(Node* node)
{
  int r;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    r = 0;
    do {
      r |= recursive_call_check(ND_CAR(node));
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) {
      r = 0;
      break;
    }
    /* fall */
  case ND_QUANT:
    r = recursive_call_check(ND_BODY(node));
    break;

  case ND_CALL:
    r = recursive_call_check(ND_BODY(node));
    if (r != 0) {
      if (ND_IS_MARK1(ND_BODY(node)))
        ND_STATUS_ADD(node, RECURSION);
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (ND_IS_MARK2(node))
          return 0;
        else if (ND_IS_MARK1(node))
          return 1; /* recursion */
        else {
          ND_STATUS_ADD(node, MARK2);
          r = recursive_call_check(ND_BODY(node));
          ND_STATUS_REMOVE(node, MARK2);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        r = 0;
        if (IS_NOT_NULL(en->te.Then)) {
          r |= recursive_call_check(en->te.Then);
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r |= recursive_call_check(en->te.Else);
        }
        r |= recursive_call_check(ND_BODY(node));
      }
      else {
        r = recursive_call_check(ND_BODY(node));
      }
    }
    break;

  default:
    r = 0;
    break;
  }

  return r;
}

#define IN_RECURSION         (1<<0)
#define FOUND_CALLED_NODE    1

static int
recursive_call_check_trav(Node* node, ParseEnv* env, int state)
{
  int r = 0;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    {
      int ret;
      do {
        ret = recursive_call_check_trav(ND_CAR(node), env, state);
        if (ret == FOUND_CALLED_NODE) r = FOUND_CALLED_NODE;
        else if (ret < 0) return ret;
      } while (IS_NOT_NULL(node = ND_CDR(node)));
    }
    break;

  case ND_QUANT:
    r = recursive_call_check_trav(ND_BODY(node), env, state);
    if (QUANT_(node)->upper == 0) {
      if (r == FOUND_CALLED_NODE)
        QUANT_(node)->include_referred = 1;
    }
    break;

  case ND_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);
      if (ANCHOR_HAS_BODY(an))
        r = recursive_call_check_trav(ND_ANCHOR_BODY(an), env, state);
    }
    break;

  case ND_BAG:
    {
      int ret;
      int state1;
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (ND_IS_CALLED(node)) {
          r = FOUND_CALLED_NODE;
          goto check_recursion;
        }
        else if ((state & IN_RECURSION) != 0) {
        check_recursion:
          if (! ND_IS_RECURSION(node)) {
            ND_STATUS_ADD(node, MARK1);
            ret = recursive_call_check(ND_BODY(node));
            if (ret != 0) {
              ND_STATUS_ADD(node, RECURSION);
              MEM_STATUS_ON(env->backtrack_mem, en->m.regnum);
            }
            ND_STATUS_REMOVE(node, MARK1);
          }
        }
      }

      state1 = state;
      if (ND_IS_RECURSION(node))
        state1 |= IN_RECURSION;

      ret = recursive_call_check_trav(ND_BODY(node), env, state1);
      if (ret == FOUND_CALLED_NODE)
        r = FOUND_CALLED_NODE;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          ret = recursive_call_check_trav(en->te.Then, env, state1);
          if (ret == FOUND_CALLED_NODE)
            r = FOUND_CALLED_NODE;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          ret = recursive_call_check_trav(en->te.Else, env, state1);
          if (ret == FOUND_CALLED_NODE)
            r = FOUND_CALLED_NODE;
        }
      }
    }
    break;

  default:
    break;
  }

  return r;
}

#endif

static void
remove_from_list(Node* prev, Node* a)
{
  if (ND_CDR(prev) != a) return ;

  ND_CDR(prev) = ND_CDR(a);
  ND_CDR(a) = NULL_NODE;
}

static int
reduce_string_list(Node* node, OnigEncoding enc)
{
  int r = 0;

  switch (ND_TYPE(node)) {
  case ND_LIST:
    {
      Node* prev;
      Node* curr;
      Node* prev_node;
      Node* next_node;

      prev = NULL_NODE;
      do {
        next_node = ND_CDR(node);
        curr = ND_CAR(node);
        if (ND_TYPE(curr) == ND_STRING) {
          if (IS_NULL(prev)
              || STR_(curr)->flag  != STR_(prev)->flag
              || ND_STATUS(curr) != ND_STATUS(prev)) {
            prev = curr;
            prev_node = node;
          }
          else {
            r = node_str_node_cat(prev, curr);
            if (r != 0) return r;
            remove_from_list(prev_node, node);
            onig_node_free(node);
          }
        }
        else {
          if (IS_NOT_NULL(prev)) {
#ifdef USE_CHECK_VALIDITY_OF_STRING_IN_TREE
            StrNode* sn = STR_(prev);
            if (! ONIGENC_IS_VALID_MBC_STRING(enc, sn->s, sn->end))
              return ONIGERR_INVALID_WIDE_CHAR_VALUE;
#endif
            prev = NULL_NODE;
          }
          r = reduce_string_list(curr, enc);
          if (r != 0) return r;
          prev_node = node;
        }

        node = next_node;
      } while (r == 0 && IS_NOT_NULL(node));

#ifdef USE_CHECK_VALIDITY_OF_STRING_IN_TREE
      if (IS_NOT_NULL(prev)) {
        StrNode* sn = STR_(prev);
        if (! ONIGENC_IS_VALID_MBC_STRING(enc, sn->s, sn->end))
          return ONIGERR_INVALID_WIDE_CHAR_VALUE;
      }
#endif
    }
    break;

  case ND_ALT:
    do {
      r = reduce_string_list(ND_CAR(node), enc);
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

#ifdef USE_CHECK_VALIDITY_OF_STRING_IN_TREE
  case ND_STRING:
    {
      StrNode* sn = STR_(node);
      if (! ONIGENC_IS_VALID_MBC_STRING(enc, sn->s, sn->end))
        return ONIGERR_INVALID_WIDE_CHAR_VALUE;
    }
    break;
#endif

  case ND_ANCHOR:
    if (IS_NULL(ND_BODY(node)))
      break;
    /* fall */
  case ND_QUANT:
    r = reduce_string_list(ND_BODY(node), enc);
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      r = reduce_string_list(ND_BODY(node), enc);
      if (r != 0) return r;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = reduce_string_list(en->te.Then, enc);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = reduce_string_list(en->te.Else, enc);
          if (r != 0) return r;
        }
      }
    }
    break;

  default:
    break;
  }

  return r;
}


#define IN_ALT          (1<<0)
#define IN_NOT          (1<<1)
#define IN_REAL_REPEAT  (1<<2)
#define IN_VAR_REPEAT   (1<<3)
#define IN_ZERO_REPEAT  (1<<4)
#define IN_MULTI_ENTRY  (1<<5)
#define IN_PREC_READ    (1<<6)
#define IN_LOOK_BEHIND  (1<<7)
#define IN_PEEK         (1<<8)

/* divide different length alternatives in look-behind.
  (?<=A|B) ==> (?<=A)|(?<=B)
  (?<!A|B) ==> (?<!A)(?<!B)
*/
static int
divide_look_behind_alternatives(Node* node)
{
  int r;
  int anc_type;
  Node *head, *np, *insert_node;
  AnchorNode* an;

  an = ANCHOR_(node);
  anc_type = an->type;

  head = ND_ANCHOR_BODY(an);
  np = ND_CAR(head);
  node_swap(node, head);
  ND_CAR(node) = head;
  ND_BODY(head) = np;

  np = node;
  while (IS_NOT_NULL(np = ND_CDR(np))) {
    r = onig_node_copy(&insert_node, head);
    if (r != 0) return r;
    CHECK_NULL_RETURN_MEMERR(insert_node);
    ND_BODY(insert_node) = ND_CAR(np);
    ND_CAR(np) = insert_node;
  }

  if (anc_type == ANCR_LOOK_BEHIND_NOT) {
    np = node;
    do {
      ND_SET_TYPE(np, ND_LIST);  /* alt -> list */
    } while (IS_NOT_NULL(np = ND_CDR(np)));
  }
  return 0;
}

static int
node_reduce_in_look_behind(Node* node)
{
  NodeType type;
  Node* body;

  if (ND_TYPE(node) != ND_QUANT) return 0;

  body = ND_BODY(node);
  type = ND_TYPE(body);
  if (type == ND_STRING || type == ND_CTYPE ||
      type == ND_CCLASS || type == ND_BACKREF) {
    QuantNode* qn = QUANT_(node);
    qn->upper = qn->lower;
    if (qn->upper == 0)
      return 1; /* removed */
  }

  return 0;
}

static int
list_reduce_in_look_behind(Node* node)
{
  int r;

  switch (ND_TYPE(node)) {
  case ND_QUANT:
    r = node_reduce_in_look_behind(node);
    if (r > 0) r = 0;
    break;

  case ND_LIST:
    do {
      r = node_reduce_in_look_behind(ND_CAR(node));
      if (r <= 0) break;
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  default:
    r = 0;
    break;
  }

  return r;
}

static int
alt_reduce_in_look_behind(Node* node, regex_t* reg, ParseEnv* env)
{
  int r;

  switch (ND_TYPE(node)) {
  case ND_ALT:
    do {
      r = list_reduce_in_look_behind(ND_CAR(node));
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  default:
    r = list_reduce_in_look_behind(node);
    break;
  }

  return r;
}

static int tune_tree(Node* node, regex_t* reg, int state, ParseEnv* env);

static int
tune_look_behind(Node* node, regex_t* reg, int state, ParseEnv* env)
{
  int r;
  int state1;
  int used;
  MinMaxCharLen ci;
  Node* body;
  AnchorNode* an = ANCHOR_(node);

  used = FALSE;
  r = check_node_in_look_behind(ND_ANCHOR_BODY(an),
                                an->type == ANCR_LOOK_BEHIND_NOT ? 1 : 0,
                                &used);
  if (r < 0) return r;
  if (r > 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;

  if (an->type == ANCR_LOOK_BEHIND_NOT)
    state1 = state | IN_NOT | IN_LOOK_BEHIND;
  else
    state1 = state | IN_LOOK_BEHIND;

  body = ND_ANCHOR_BODY(an);
  /* Execute tune_tree(body) before call node_char_len().
     Because case-fold expansion must be done before node_char_len().
   */
  r = tune_tree(body, reg, state1, env);
  if (r != 0) return r;

  r = alt_reduce_in_look_behind(body, reg, env);
  if (r != 0) return r;

  r = node_char_len(body, reg, &ci, env);
  if (r >= 0) {
    /* #177: overflow in onigenc_step_back() */
    if ((ci.max != INFINITE_LEN && ci.max > LOOK_BEHIND_MAX_CHAR_LEN)
      || ci.min > LOOK_BEHIND_MAX_CHAR_LEN) {
      return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
    }

    if (ci.min == 0 && ci.min_is_sure != FALSE && used == FALSE) {
      if (an->type == ANCR_LOOK_BEHIND_NOT)
        r = onig_node_reset_fail(node);
      else
        r = onig_node_reset_empty(node);

      return r;
    }

    if (r == CHAR_LEN_TOP_ALT_FIXED) {
      if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND)) {
        r = divide_look_behind_alternatives(node);
        if (r == 0)
          r = tune_tree(node, reg, state, env);
      }
      else if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_VARIABLE_LEN_LOOK_BEHIND))
        goto normal;
      else
        r = ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
    }
    else { /* CHAR_LEN_NORMAL */
    normal:
      if (ci.min == INFINITE_LEN) {
        r = ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
      }
      else {
        if (ci.min != ci.max &&
            ! IS_SYNTAX_BV(env->syntax, ONIG_SYN_VARIABLE_LEN_LOOK_BEHIND)) {
          r = ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
        }
        else {
          Node* tail;

          /* check lead_node is already set by double call after
             divide_look_behind_alternatives() */
          if (IS_NULL(an->lead_node)) {
            an->char_min_len = ci.min;
            an->char_max_len = ci.max;
            r = get_tree_tail_literal(body, &tail, reg, 0);
            if (r == GET_VALUE_FOUND) {
              r = onig_node_copy(&(an->lead_node), tail);
              if (r != 0) return r;
            }
          }
          r = ONIG_NORMAL;
        }
      }
    }
  }

  return r;
}

static int
tune_next(Node* node, Node* next_node, regex_t* reg)
{
  int called;
  NodeType type;

  called = FALSE;

 retry:
  type = ND_TYPE(node);
  if (type == ND_QUANT) {
    QuantNode* qn = QUANT_(node);
    if (qn->greedy && IS_INFINITE_REPEAT(qn->upper)) {
#ifdef USE_QUANT_PEEK_NEXT
      if (called == FALSE) {
        Node* n = get_tree_head_literal(next_node, 1, reg);
        /* '\0': for UTF-16BE etc... */
        if (IS_NOT_NULL(n) && STR_(n)->s[0] != '\0') {
          qn->next_head_exact = n;
        }
      }
#endif
      /* automatic posseivation a*b ==> (?>a*)b */
      if (qn->lower <= 1) {
        if (is_strict_real_node(ND_BODY(node))) {
          Node *x, *y;
          x = get_tree_head_literal(ND_BODY(node), 0, reg);
          if (IS_NOT_NULL(x)) {
            y = get_tree_head_literal(next_node,  0, reg);
            if (IS_NOT_NULL(y) && is_exclusive(x, y, reg)) {
              Node* en = onig_node_new_bag(BAG_STOP_BACKTRACK);
              CHECK_NULL_RETURN_MEMERR(en);
              ND_STATUS_ADD(en, STRICT_REAL_REPEAT);
              node_swap(node, en);
              ND_BODY(node) = en;
            }
          }
        }
      }
    }
  }
  else if (type == ND_BAG) {
    BagNode* en = BAG_(node);
    if (en->type == BAG_MEMORY) {
      if (ND_IS_CALLED(node))
        called = TRUE;
      node = ND_BODY(node);
      goto retry;
    }
  }
  return 0;
}


static int
is_all_code_len_1_items(int n, OnigCaseFoldCodeItem items[])
{
  int i;

  for (i = 0; i < n; i++) {
    OnigCaseFoldCodeItem* item = items + i;
    if (item->code_len != 1) return 0;
  }

  return 1;
}

static int
get_min_max_byte_len_case_fold_items(int n, OnigCaseFoldCodeItem items[],
                                     OnigLen* rmin, OnigLen* rmax)
{
  int i;
  OnigLen len, minlen, maxlen;

  minlen = INFINITE_LEN;
  maxlen = 0;
  for (i = 0; i < n; i++) {
    OnigCaseFoldCodeItem* item = items + i;

    len = item->byte_len;
    if (len < minlen) minlen = len;
    if (len > maxlen) maxlen = len;
  }

  *rmin = minlen;
  *rmax = maxlen;
  return 0;
}

static int
make_code_list_to_string(Node** rnode, OnigEncoding enc,
                         int n, OnigCodePoint codes[])
{
  int r, i, len;
  Node* node;
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];

  *rnode = NULL_NODE;
  node = onig_node_new_str(NULL, NULL);
  CHECK_NULL_RETURN_MEMERR(node);

  for (i = 0; i < n; i++) {
    len = ONIGENC_CODE_TO_MBC(enc, codes[i], buf);
    if (len < 0) {
      r = len;
      goto err;
    }

    r = onig_node_str_cat(node, buf, buf + len);
    if (r != 0) goto err;
  }

  *rnode = node;
  return 0;

 err:
  onig_node_free(node);
  return r;
}

static int
unravel_cf_node_add(Node** rlist, Node* add)
{
  Node *list;

  list = *rlist;
  if (IS_NULL(list)) {
    list = onig_node_new_list(add, NULL);
    CHECK_NULL_RETURN_MEMERR(list);
    *rlist = list;
  }
  else {
    Node* r = node_list_add(list, add);
    CHECK_NULL_RETURN_MEMERR(r);
  }

  return 0;
}

static int
unravel_cf_string_add(Node** rlist, Node** rsn, UChar* s, UChar* end,
                      unsigned int flag)
{
  int r;
  Node *sn, *list;

  list = *rlist;
  sn   = *rsn;

  if (IS_NOT_NULL(sn) && STR_(sn)->flag == flag) {
    r = onig_node_str_cat(sn, s, end);
  }
  else {
    sn = onig_node_new_str(s, end);
    CHECK_NULL_RETURN_MEMERR(sn);

    STR_(sn)->flag = flag;
    r = unravel_cf_node_add(&list, sn);
  }

  if (r == 0) {
    *rlist = list;
    *rsn = sn;
  }
  return r;
}

static int
unravel_cf_string_alt_or_cc_add(Node** rlist, int n,
            OnigCaseFoldCodeItem items[], OnigEncoding enc,
            OnigCaseFoldType case_fold_flag, UChar* s, UChar* end)
{
  int r, i;
  Node* node;

  if (is_all_code_len_1_items(n, items)) {
    OnigCodePoint codes[14];/* least ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM + 1 */

    codes[0] = ONIGENC_MBC_TO_CODE(enc, s, end);
    for (i = 0; i < n; i++) {
      OnigCaseFoldCodeItem* item = items + i;
      codes[i+1] = item->code[0];
    }
    r = onig_new_cclass_with_code_list(&node, enc, n + 1, codes);
    if (r != 0) return r;
  }
  else {
    Node *snode, *alt, *curr;

    snode = onig_node_new_str(s, end);
    CHECK_NULL_RETURN_MEMERR(snode);
    node = curr = onig_node_new_alt(snode, NULL_NODE);
    if (IS_NULL(curr)) {
      onig_node_free(snode);
      return ONIGERR_MEMORY;
    }

    r = 0;
    for (i = 0; i < n; i++) {
      OnigCaseFoldCodeItem* item = items + i;
      r = make_code_list_to_string(&snode, enc, item->code_len, item->code);
      if (r != 0) {
        onig_node_free(node);
        return r;
      }

      alt = onig_node_new_alt(snode, NULL_NODE);
      if (IS_NULL(alt)) {
        onig_node_free(snode);
        onig_node_free(node);
        return ONIGERR_MEMORY;
      }

      ND_CDR(curr) = alt;
      curr = alt;
    }
  }

  r = unravel_cf_node_add(rlist, node);
  if (r != 0) onig_node_free(node);
  return r;
}

static int
unravel_cf_look_behind_add(Node** rlist, Node** rsn,
                int n, OnigCaseFoldCodeItem items[], OnigEncoding enc,
                UChar* s, OnigLen one_len)
{
  int r, i, found;

  found = FALSE;
  for (i = 0; i < n; i++) {
    OnigCaseFoldCodeItem* item = items + i;
    if (item->byte_len == one_len) {
      if (item->code_len == 1) {
        found = TRUE;
        break;
      }
    }
  }

  if (found == FALSE) {
    r = unravel_cf_string_add(rlist, rsn, s, s + one_len, 0 /* flag */);
  }
  else {
    Node* node;
    OnigCodePoint codes[14];/* least ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM + 1 */

    found = 0;
    codes[found++] = ONIGENC_MBC_TO_CODE(enc, s, s + one_len);
    for (i = 0; i < n; i++) {
      OnigCaseFoldCodeItem* item = items + i;
      if (item->byte_len == one_len) {
        if (item->code_len == 1) {
          codes[found++] = item->code[0];
        }
      }
    }
    r = onig_new_cclass_with_code_list(&node, enc, found, codes);
    if (r != 0) return r;

    r = unravel_cf_node_add(rlist, node);
    if (r != 0) onig_node_free(node);

    *rsn = NULL_NODE;
  }

  return r;
}

static int
unravel_case_fold_string(Node* node, regex_t* reg, int state)
{
  int r, n, in_look_behind;
  OnigLen min_len, max_len, one_len;
  UChar *start, *end, *p, *q;
  StrNode* snode;
  Node *sn, *list;
  OnigEncoding enc;
  OnigCaseFoldCodeItem items[ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM];

  if (ND_STRING_IS_CASE_EXPANDED(node)) return 0;

  ND_STATUS_REMOVE(node, IGNORECASE);
  snode = STR_(node);
  start = snode->s;
  end   = snode->end;
  if (start >= end) return 0;

  in_look_behind = (state & IN_LOOK_BEHIND) != 0;
  enc = reg->enc;

  list = sn = NULL_NODE;
  p = start;
  while (p < end) {
    n = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, reg->case_fold_flag, p, end,
                                           items);
    if (n < 0) {
      r = n;
      goto err;
    }

    one_len = (OnigLen )enclen(enc, p);
    if (n == 0) {
      q = p + one_len;
      if (q > end) q = end;
      r = unravel_cf_string_add(&list, &sn, p, q, 0 /* flag */);
      if (r != 0) goto err;
    }
    else {
      if (in_look_behind != 0) {
        q = p + one_len;
        if (items[0].byte_len != one_len) {
          r = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, reg->case_fold_flag, p, q,
                                                 items);
          if (r < 0) goto err;
          n = r;
        }
        r = unravel_cf_look_behind_add(&list, &sn, n, items, enc, p, one_len);
        if (r != 0) goto err;
      }
      else {
        get_min_max_byte_len_case_fold_items(n, items, &min_len, &max_len);
        if (min_len != max_len) {
          r = ONIGERR_PARSER_BUG;
          goto err;
        }

        q = p + max_len;
        r = unravel_cf_string_alt_or_cc_add(&list, n, items, enc,
                                            reg->case_fold_flag, p, q);
        if (r != 0) goto err;
        sn = NULL_NODE;
      }
    }

    p = q;
  }

  if (IS_NOT_NULL(list)) {
    if (node_list_len(list) == 1) {
      node_swap(node, ND_CAR(list));
    }
    else {
      node_swap(node, list);
    }
    onig_node_free(list);
  }
  else {
    node_swap(node, sn);
    onig_node_free(sn);
  }
  return 0;

 err:
  if (IS_NOT_NULL(list))
    onig_node_free(list);
  else if (IS_NOT_NULL(sn))
    onig_node_free(sn);

  return r;
}

#ifdef USE_RIGID_CHECK_CAPTURES_IN_EMPTY_REPEAT
static enum BodyEmptyType
quantifiers_memory_node_info(Node* node)
{
  int r = BODY_MAY_BE_EMPTY;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    {
      int v;
      do {
        v = quantifiers_memory_node_info(ND_CAR(node));
        if (v > r) r = v;
      } while (IS_NOT_NULL(node = ND_CDR(node)));
    }
    break;

#ifdef USE_CALL
  case ND_CALL:
    if (ND_IS_RECURSION(node)) {
      return BODY_MAY_BE_EMPTY_REC; /* tiny version */
    }
    else
      r = quantifiers_memory_node_info(ND_BODY(node));
    break;
#endif

  case ND_QUANT:
    {
      QuantNode* qn = QUANT_(node);
      if (qn->upper != 0) {
        r = quantifiers_memory_node_info(ND_BODY(node));
      }
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_MEMORY:
        if (ND_IS_RECURSION(node)) {
          return BODY_MAY_BE_EMPTY_REC;
        }
        return BODY_MAY_BE_EMPTY_MEM;
        break;

      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        r = quantifiers_memory_node_info(ND_BODY(node));
        break;
      case BAG_IF_ELSE:
        {
          int v;
          r = quantifiers_memory_node_info(ND_BODY(node));
          if (IS_NOT_NULL(en->te.Then)) {
            v = quantifiers_memory_node_info(en->te.Then);
            if (v > r) r = v;
          }
          if (IS_NOT_NULL(en->te.Else)) {
            v = quantifiers_memory_node_info(en->te.Else);
            if (v > r) r = v;
          }
        }
        break;
      }
    }
    break;

  case ND_BACKREF:
  case ND_STRING:
  case ND_CTYPE:
  case ND_CCLASS:
  case ND_ANCHOR:
  case ND_GIMMICK:
  default:
    break;
  }

  return r;
}
#endif /* USE_RIGID_CHECK_CAPTURES_IN_EMPTY_REPEAT */


#ifdef USE_CALL

#ifdef __GNUC__
__inline
#endif
static int
check_call_reference(CallNode* cn, ParseEnv* env, int state)
{
  MemEnv* mem_env = PARSEENV_MEMENV(env);

  if (cn->by_number != 0) {
    int gnum = cn->called_gnum;

    if (env->num_named > 0 &&
        IS_SYNTAX_BV(env->syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
        ! OPTON_CAPTURE_GROUP(env->options)) {
      return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;
    }

    if (gnum > env->num_mem) {
      onig_scan_env_set_error_string(env, ONIGERR_UNDEFINED_GROUP_REFERENCE,
                                     cn->name, cn->name_end);
      return ONIGERR_UNDEFINED_GROUP_REFERENCE;
    }

  set_call_attr:
    ND_CALL_BODY(cn) = mem_env[cn->called_gnum].mem_node;
    if (IS_NULL(ND_CALL_BODY(cn))) {
      onig_scan_env_set_error_string(env, ONIGERR_UNDEFINED_NAME_REFERENCE,
                                     cn->name, cn->name_end);
      return ONIGERR_UNDEFINED_NAME_REFERENCE;
    }

    ND_STATUS_ADD(ND_CALL_BODY(cn), REFERENCED);
  }
  else {
    int *refs;

    int n = onig_name_to_group_numbers(env->reg, cn->name, cn->name_end, &refs);
    if (n <= 0) {
      onig_scan_env_set_error_string(env, ONIGERR_UNDEFINED_NAME_REFERENCE,
                                     cn->name, cn->name_end);
      return ONIGERR_UNDEFINED_NAME_REFERENCE;
    }
    else if (n > 1) {
      onig_scan_env_set_error_string(env, ONIGERR_MULTIPLEX_DEFINITION_NAME_CALL,
                                     cn->name, cn->name_end);
      return ONIGERR_MULTIPLEX_DEFINITION_NAME_CALL;
    }
    else {
      cn->called_gnum = refs[0];
      goto set_call_attr;
    }
  }

  return 0;
}

#ifdef USE_WHOLE_OPTIONS
static int
check_whole_options_position(Node* node /* root */, ParseEnv* env)
{
  int is_list;

  is_list = FALSE;

#ifdef USE_CALL
  if ((env->flags & PE_FLAG_HAS_CALL_ZERO) != 0) {
    node = ND_BODY(node);
  }
#endif

 start:
  switch (ND_TYPE(node)) {
  case ND_LIST:
    if (IS_NOT_NULL(ND_CDR(node)))
      is_list = TRUE;

    node = ND_CAR(node);
    goto start;
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_OPTION) {
        if (ND_IS_WHOLE_OPTIONS(node)) {
          if (is_list == TRUE && IS_NOT_NULL(ND_BODY(node)))
            break;

          return 0;
        }
      }
    }
    break;

  default:
    break;
  }

  return ONIGERR_INVALID_GROUP_OPTION;
}
#endif

static void
tune_call2_call(Node* node)
{
  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      tune_call2_call(ND_CAR(node));
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_QUANT:
    tune_call2_call(ND_BODY(node));
    break;

  case ND_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      tune_call2_call(ND_BODY(node));
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (! ND_IS_MARK1(node)) {
          ND_STATUS_ADD(node, MARK1);
          tune_call2_call(ND_BODY(node));
          ND_STATUS_REMOVE(node, MARK1);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        tune_call2_call(ND_BODY(node));
        if (IS_NOT_NULL(en->te.Then))
          tune_call2_call(en->te.Then);
        if (IS_NOT_NULL(en->te.Else))
          tune_call2_call(en->te.Else);
      }
      else {
        tune_call2_call(ND_BODY(node));
      }
    }
    break;

  case ND_CALL:
    if (! ND_IS_MARK1(node)) {
      ND_STATUS_ADD(node, MARK1);
      {
        CallNode* cn = CALL_(node);
        Node* called = ND_CALL_BODY(cn);

        cn->entry_count++;

        ND_STATUS_ADD(called, CALLED);
        BAG_(called)->m.entry_count++;
        tune_call2_call(called);
      }
      ND_STATUS_REMOVE(node, MARK1);
    }
    break;

  default:
    break;
  }
}

static int
tune_call(Node* node, ParseEnv* env, int state)
{
  int r;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = tune_call(ND_CAR(node), env, state);
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_QUANT:
    if (QUANT_(node)->upper == 0)
      state |= IN_ZERO_REPEAT;

    r = tune_call(ND_BODY(node), env, state);
    break;

  case ND_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      r = tune_call(ND_BODY(node), env, state);
    else
      r = 0;
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if ((state & IN_ZERO_REPEAT) != 0) {
          ND_STATUS_ADD(node, IN_ZERO_REPEAT);
          BAG_(node)->m.entry_count--;
        }
        r = tune_call(ND_BODY(node), env, state);
      }
      else if (en->type == BAG_IF_ELSE) {
        r = tune_call(ND_BODY(node), env, state);
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = tune_call(en->te.Then, env, state);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else))
          r = tune_call(en->te.Else, env, state);
      }
      else
        r = tune_call(ND_BODY(node), env, state);
    }
    break;

  case ND_CALL:
    if ((state & IN_ZERO_REPEAT) != 0) {
      ND_STATUS_ADD(node, IN_ZERO_REPEAT);
      CALL_(node)->entry_count--;
    }

    r = check_call_reference(CALL_(node), env, state);
    break;

  default:
    r = 0;
    break;
  }

  return r;
}

static int
tune_call2(Node* node)
{
  int r = 0;

  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = tune_call2(ND_CAR(node));
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_QUANT:
    if (QUANT_(node)->upper != 0)
      r = tune_call2(ND_BODY(node));
    break;

  case ND_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      r = tune_call2(ND_BODY(node));
    break;

  case ND_BAG:
    if (! ND_IS_IN_ZERO_REPEAT(node))
      r = tune_call2(ND_BODY(node));

    {
      BagNode* en = BAG_(node);

      if (r != 0) return r;
      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = tune_call2(en->te.Then);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else))
          r = tune_call2(en->te.Else);
      }
    }
    break;

  case ND_CALL:
    if (! ND_IS_IN_ZERO_REPEAT(node)) {
      tune_call2_call(node);
    }
    break;

  default:
    break;
  }

  return r;
}


static void
tune_called_state_call(Node* node, int state)
{
  switch (ND_TYPE(node)) {
  case ND_ALT:
    state |= IN_ALT;
    /* fall */
  case ND_LIST:
    do {
      tune_called_state_call(ND_CAR(node), state);
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 2)
        state |= IN_REAL_REPEAT;
      if (qn->lower != qn->upper)
        state |= IN_VAR_REPEAT;
      if ((state & IN_PEEK) != 0)
        ND_STATUS_ADD(node, INPEEK);

      tune_called_state_call(ND_QUANT_BODY(qn), state);
    }
    break;

  case ND_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);

      switch (an->type) {
      case ANCR_PREC_READ_NOT:
      case ANCR_LOOK_BEHIND_NOT:
        state |= (IN_NOT | IN_PEEK);
        tune_called_state_call(ND_ANCHOR_BODY(an), state);
        break;
      case ANCR_PREC_READ:
      case ANCR_LOOK_BEHIND:
        state |= IN_PEEK;
        tune_called_state_call(ND_ANCHOR_BODY(an), state);
        break;
      default:
        break;
      }
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (ND_IS_MARK1(node)) {
          if ((~en->m.called_state & state) != 0) {
            en->m.called_state |= state;
            tune_called_state_call(ND_BODY(node), state);
          }
        }
        else {
          ND_STATUS_ADD(node, MARK1);
          en->m.called_state |= state;
          tune_called_state_call(ND_BODY(node), state);
          ND_STATUS_REMOVE(node, MARK1);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        state |= IN_ALT;
        tune_called_state_call(ND_BODY(node), state);
        if (IS_NOT_NULL(en->te.Then)) {
          tune_called_state_call(en->te.Then, state);
        }
        if (IS_NOT_NULL(en->te.Else))
          tune_called_state_call(en->te.Else, state);
      }
      else {
        tune_called_state_call(ND_BODY(node), state);
      }
    }
    break;

  case ND_CALL:
    if ((state & IN_PEEK) != 0)
      ND_STATUS_ADD(node, INPEEK);
    if ((state & IN_REAL_REPEAT) != 0)
      ND_STATUS_ADD(node, IN_REAL_REPEAT);

    tune_called_state_call(ND_BODY(node), state);
    break;

  default:
    break;
  }
}

static void
tune_called_state(Node* node, int state)
{
  switch (ND_TYPE(node)) {
  case ND_ALT:
    state |= IN_ALT;
    /* fall */
  case ND_LIST:
    do {
      tune_called_state(ND_CAR(node), state);
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

#ifdef USE_CALL
  case ND_CALL:
    if ((state & IN_PEEK) != 0)
      ND_STATUS_ADD(node, INPEEK);
    if ((state & IN_REAL_REPEAT) != 0)
      ND_STATUS_ADD(node, IN_REAL_REPEAT);

    tune_called_state_call(node, state);
    break;
#endif

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      switch (en->type) {
      case BAG_MEMORY:
        if (en->m.entry_count > 1)
          state |= IN_MULTI_ENTRY;

        en->m.called_state |= state;
        /* fall */
      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        tune_called_state(ND_BODY(node), state);
        break;
      case BAG_IF_ELSE:
        state |= IN_ALT;
        tune_called_state(ND_BODY(node), state);
        if (IS_NOT_NULL(en->te.Then))
          tune_called_state(en->te.Then, state);
        if (IS_NOT_NULL(en->te.Else))
          tune_called_state(en->te.Else, state);
        break;
      }
    }
    break;

  case ND_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 2)
        state |= IN_REAL_REPEAT;
      if (qn->lower != qn->upper)
        state |= IN_VAR_REPEAT;
      if ((state & IN_PEEK) != 0)
        ND_STATUS_ADD(node, INPEEK);

      tune_called_state(ND_QUANT_BODY(qn), state);
    }
    break;

  case ND_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);

      switch (an->type) {
      case ANCR_PREC_READ_NOT:
      case ANCR_LOOK_BEHIND_NOT:
        state |= (IN_NOT | IN_PEEK);
        tune_called_state(ND_ANCHOR_BODY(an), state);
        break;
      case ANCR_PREC_READ:
      case ANCR_LOOK_BEHIND:
        state |= IN_PEEK;
        tune_called_state(ND_ANCHOR_BODY(an), state);
        break;
      default:
        break;
      }
    }
    break;

  case ND_BACKREF:
  case ND_STRING:
  case ND_CTYPE:
  case ND_CCLASS:
  case ND_GIMMICK:
  default:
    break;
  }
}

#endif  /* USE_CALL */


#ifdef __GNUC__
__inline
#endif
static int
tune_anchor(Node* node, regex_t* reg, int state, ParseEnv* env)
{
  int r;
  AnchorNode* an = ANCHOR_(node);

  switch (an->type) {
  case ANCR_PREC_READ:
    r = tune_tree(ND_ANCHOR_BODY(an), reg, (state | IN_PREC_READ), env);
    break;
  case ANCR_PREC_READ_NOT:
    r = tune_tree(ND_ANCHOR_BODY(an), reg, (state | IN_PREC_READ | IN_NOT),
                  env);
    break;

  case ANCR_LOOK_BEHIND:
  case ANCR_LOOK_BEHIND_NOT:
    r = tune_look_behind(node, reg, state, env);
    break;

  default:
    r = 0;
    break;
  }

  return r;
}

#ifdef __GNUC__
__inline
#endif
static int
tune_quant(Node* node, regex_t* reg, int state, ParseEnv* env)
{
  int r;
  QuantNode* qn = QUANT_(node);
  Node* body = ND_BODY(node);

  if ((state & IN_REAL_REPEAT) != 0) {
    ND_STATUS_ADD(node, IN_REAL_REPEAT);
  }
  if ((state & IN_MULTI_ENTRY) != 0) {
    ND_STATUS_ADD(node, IN_MULTI_ENTRY);
  }

  if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 1) {
    OnigLen d = node_min_byte_len(body, env);
    if (d == 0) {
#ifdef USE_RIGID_CHECK_CAPTURES_IN_EMPTY_REPEAT
      qn->emptiness = quantifiers_memory_node_info(body);
#else
      qn->emptiness = BODY_MAY_BE_EMPTY;
#endif
    }
  }

  if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 2)
    state |= IN_REAL_REPEAT;
  if (qn->lower != qn->upper)
    state |= IN_VAR_REPEAT;

  r = tune_tree(body, reg, state, env);
  if (r != 0) return r;

  /* expand string */
#define EXPAND_STRING_MAX_LENGTH  100
  if (ND_TYPE(body) == ND_STRING) {
    if (!IS_INFINITE_REPEAT(qn->lower) && qn->lower == qn->upper &&
        qn->lower > 1 && qn->lower <= EXPAND_STRING_MAX_LENGTH) {
      int len = ND_STRING_LEN(body);

      if (len * qn->lower <= EXPAND_STRING_MAX_LENGTH) {
        int i, n = qn->lower;
        node_conv_to_str_node(node, body);
        for (i = 0; i < n; i++) {
          r = node_str_node_cat(node, body);
          if (r != 0) return r;
        }
        onig_node_free(body);
        return r;
      }
    }
  }

  if (qn->greedy && (qn->emptiness == BODY_IS_NOT_EMPTY)) {
    if (ND_TYPE(body) == ND_QUANT) {
      QuantNode* tqn = QUANT_(body);
      if (IS_NOT_NULL(tqn->head_exact)) {
        qn->head_exact  = tqn->head_exact;
        tqn->head_exact = NULL;
      }
    }
    else {
      qn->head_exact = get_tree_head_literal(ND_BODY(node), 1, reg);
    }
  }

  return r;
}

/* tune_tree does the following work.
 1. check empty loop. (set qn->emptiness)
 2. expand ignore-case in char class.
 3. set memory status bit flags. (reg->mem_stats)
 4. set qn->head_exact for [push, exact] -> [push_or_jump_exact1, exact].
 5. find invalid patterns in look-behind.
 6. expand repeated string.
 */
static int
tune_tree(Node* node, regex_t* reg, int state, ParseEnv* env)
{
  int r = 0;

  switch (ND_TYPE(node)) {
  case ND_LIST:
    {
      Node* prev = NULL_NODE;
      do {
        r = tune_tree(ND_CAR(node), reg, state, env);
        if (IS_NOT_NULL(prev) && r == 0) {
          r = tune_next(prev, ND_CAR(node), reg);
        }
        prev = ND_CAR(node);
      } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    }
    break;

  case ND_ALT:
    do {
      r = tune_tree(ND_CAR(node), reg, (state | IN_ALT), env);
    } while (r == 0 && IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_STRING:
    if (ND_IS_REAL_IGNORECASE(node)) {
      r = unravel_case_fold_string(node, reg, state);
    }
    break;

  case ND_BACKREF:
    {
      int i;
      int* p;
      BackRefNode* br = BACKREF_(node);
      p = BACKREFS_P(br);
      for (i = 0; i < br->back_num; i++) {
        if (p[i] > env->num_mem)  return ONIGERR_INVALID_BACKREF;
        MEM_STATUS_ON(env->backrefed_mem, p[i]);
#if 0
#ifdef USE_BACKREF_WITH_LEVEL
        if (ND_IS_NEST_LEVEL(node)) {
          MEM_STATUS_ON(env->backtrack_mem, p[i]);
        }
#endif
#else
        /* More precisely, it should be checked whether alt/repeat exists before
           the subject capture node, and then this backreference position
           exists before (or in) the capture node. */
        MEM_STATUS_ON(env->backtrack_mem, p[i]);
#endif
      }
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      switch (en->type) {
      case BAG_OPTION:
        {
          OnigOptionType options = reg->options;
          reg->options = BAG_(node)->o.options;
          r = tune_tree(ND_BODY(node), reg, state, env);
          reg->options = options;
        }
        break;

      case BAG_MEMORY:
#ifdef USE_CALL
        state |= en->m.called_state;
#endif

        if ((state & (IN_ALT | IN_NOT | IN_VAR_REPEAT | IN_MULTI_ENTRY)) != 0
            || ND_IS_RECURSION(node)) {
          MEM_STATUS_ON(env->backtrack_mem, en->m.regnum);
        }
        r = tune_tree(ND_BODY(node), reg, state, env);
        break;

      case BAG_STOP_BACKTRACK:
        {
          Node* target = ND_BODY(node);
          r = tune_tree(target, reg, state, env);
          if (ND_TYPE(target) == ND_QUANT) {
            QuantNode* tqn = QUANT_(target);
            if (IS_INFINITE_REPEAT(tqn->upper) && tqn->lower <= 1 &&
                tqn->greedy != 0) {  /* (?>a*), a*+ etc... */
              if (is_strict_real_node(ND_BODY(target)))
                ND_STATUS_ADD(node, STRICT_REAL_REPEAT);
            }
          }
        }
        break;

      case BAG_IF_ELSE:
        r = tune_tree(ND_BODY(node), reg, (state | IN_ALT), env);
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = tune_tree(en->te.Then, reg, (state | IN_ALT), env);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else))
          r = tune_tree(en->te.Else, reg, (state | IN_ALT), env);
        break;
      }
    }
    break;

  case ND_QUANT:
    if ((state & (IN_PREC_READ | IN_LOOK_BEHIND)) != 0)
      ND_STATUS_ADD(node, INPEEK);

    r = tune_quant(node, reg, state, env);
    break;

  case ND_ANCHOR:
    r = tune_anchor(node, reg, state, env);
    break;

#ifdef USE_CALL
  case ND_CALL:
#endif
  case ND_CTYPE:
  case ND_CCLASS:
  case ND_GIMMICK:
  default:
    break;
  }

  return r;
}

#ifndef ONIG_DONT_OPTIMIZE
static int
set_sunday_quick_search_or_bmh_skip_table(regex_t* reg, int case_expand,
                                          UChar* s, UChar* end,
                                          UChar skip[], int* roffset)
{
  int i, j, k, len, offset;
  int n, clen;
  UChar* p;
  OnigEncoding enc;
  OnigCaseFoldCodeItem items[ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM];
  UChar buf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

  enc = reg->enc;
  offset = ENC_GET_SKIP_OFFSET(enc);
  if (offset == ENC_SKIP_OFFSET_1_OR_0) {
    UChar* p = s;
    while (1) {
      len = enclen(enc, p);
      if (p + len >= end) {
        if (len == 1) offset = 1;
        else          offset = 0;
        break;
      }
      p += len;
    }
  }

  len = (int )(end - s);
  if (len + offset >= UCHAR_MAX)
    return ONIGERR_PARSER_BUG;

  *roffset = offset;

  for (i = 0; i < CHAR_MAP_SIZE; i++) {
    skip[i] = (UChar )(len + offset);
  }

  for (p = s; p < end; ) {
    int z;

    clen = enclen(enc, p);
    if (p + clen > end) clen = (int )(end - p);

    len = (int )(end - p);
    for (j = 0; j < clen; j++) {
      z = len - j + (offset - 1);
      if (z <= 0) break;
      skip[p[j]] = z;
    }

    if (case_expand != 0) {
      n = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, reg->case_fold_flag,
                                             p, end, items);
      for (k = 0; k < n; k++) {
        ONIGENC_CODE_TO_MBC(enc, items[k].code[0], buf);
        for (j = 0; j < clen; j++) {
          z = len - j + (offset - 1);
          if (z <= 0) break;
          if (skip[buf[j]] > z)
            skip[buf[j]] = z;
        }
      }
    }

    p += clen;
  }

  return 0;
}
#endif


#define OPT_EXACT_MAXLEN   24

#if OPT_EXACT_MAXLEN >= UCHAR_MAX
#error Too big OPT_EXACT_MAXLEN
#endif

typedef struct {
  MinMaxLen        mm;
  OnigEncoding     enc;
  OnigCaseFoldType case_fold_flag;
  ParseEnv*        scan_env;
} OptEnv;

typedef struct {
  int left;
  int right;
} OptAnc;

typedef struct {
  MinMaxLen  mm;   /* position */
  OptAnc     anc;
  int        reach_end;
  int        len;
  UChar      s[OPT_EXACT_MAXLEN];
} OptStr;

typedef struct {
  MinMaxLen mm;     /* position */
  OptAnc    anc;
  int       value;  /* weighted value */
  UChar     map[CHAR_MAP_SIZE];
} OptMap;

typedef struct {
  MinMaxLen len;
  OptAnc  anc;
  OptStr  sb;     /* boundary */
  OptStr  sm;     /* middle */
  OptStr  spr;    /* prec read (?=...) */
  OptMap  map;    /* boundary */
} OptNode;


#ifndef ONIG_DONT_OPTIMIZE

static int
map_position_value(OnigEncoding enc, int i)
{
  static const short int Vals[] = {
     5,  1,  1,  1,  1,  1,  1,  1,  1, 10, 10,  1,  1, 10,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    12,  4,  7,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,
     6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,
     5,  6,  6,  6,  6,  7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
     6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  6,  5,  5,  5,
     5,  6,  6,  6,  6,  7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
     6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  1
  };

  if (i < (int )(sizeof(Vals)/sizeof(Vals[0]))) {
    if (i == 0 && ONIGENC_MBC_MINLEN(enc) > 1)
      return 20;
    else
      return (int )Vals[i];
  }
  else
    return 4;   /* Take it easy. */
}

static int
distance_value(MinMaxLen* mm)
{
  /* 1000 / (min-max-dist + 1) */
  static const short int dist_vals[] = {
    1000,  500,  333,  250,  200,  167,  143,  125,  111,  100,
      91,   83,   77,   71,   67,   63,   59,   56,   53,   50,
      48,   45,   43,   42,   40,   38,   37,   36,   34,   33,
      32,   31,   30,   29,   29,   28,   27,   26,   26,   25,
      24,   24,   23,   23,   22,   22,   21,   21,   20,   20,
      20,   19,   19,   19,   18,   18,   18,   17,   17,   17,
      16,   16,   16,   16,   15,   15,   15,   15,   14,   14,
      14,   14,   14,   14,   13,   13,   13,   13,   13,   13,
      12,   12,   12,   12,   12,   12,   11,   11,   11,   11,
      11,   11,   11,   11,   11,   10,   10,   10,   10,   10
  };

  OnigLen d;

  if (mm->max == INFINITE_LEN) return 0;

  d = mm->max - mm->min;
  if (d < (OnigLen )(sizeof(dist_vals)/sizeof(dist_vals[0])))
    /* return dist_vals[d] * 16 / (mm->min + 12); */
    return (int )dist_vals[d];
  else
    return 1;
}

static int
comp_distance_value(MinMaxLen* d1, MinMaxLen* d2, int v1, int v2)
{
  if (v2 <= 0) return -1;
  if (v1 <= 0) return  1;

  v1 *= distance_value(d1);
  v2 *= distance_value(d2);

  if (v2 > v1) return  1;
  if (v2 < v1) return -1;

  if (d2->min < d1->min) return  1;
  if (d2->min > d1->min) return -1;
  return 0;
}

static void
copy_opt_env(OptEnv* to, OptEnv* from)
{
  *to = *from;
}

static void
clear_opt_anc_info(OptAnc* a)
{
  a->left  = 0;
  a->right = 0;
}

static void
copy_opt_anc_info(OptAnc* to, OptAnc* from)
{
  *to = *from;
}

static void
concat_opt_anc_info(OptAnc* to, OptAnc* left, OptAnc* right,
                    OnigLen left_len, OnigLen right_len)
{
  clear_opt_anc_info(to);

  to->left = left->left;
  if (left_len == 0) {
    to->left |= right->left;
  }

  to->right = right->right;
  if (right_len == 0) {
    to->right |= left->right;
  }
  else {
    to->right |= (left->right & ANCR_PREC_READ_NOT);
  }
}

static int
is_left(int a)
{
  if (a == ANCR_END_BUF  || a == ANCR_SEMI_END_BUF ||
      a == ANCR_END_LINE || a == ANCR_PREC_READ || a == ANCR_PREC_READ_NOT)
    return 0;

  return 1;
}

static int
is_set_opt_anc_info(OptAnc* to, int anc)
{
  if ((to->left & anc) != 0) return 1;

  return ((to->right & anc) != 0 ? 1 : 0);
}

static void
add_opt_anc_info(OptAnc* to, int anc)
{
  if (is_left(anc))
    to->left |= anc;
  else
    to->right |= anc;
}

static void
remove_opt_anc_info(OptAnc* to, int anc)
{
  if (is_left(anc))
    to->left &= ~anc;
  else
    to->right &= ~anc;
}

static void
alt_merge_opt_anc_info(OptAnc* to, OptAnc* add)
{
  to->left  &= add->left;
  to->right &= add->right;
}

static int
is_full_opt_exact(OptStr* e)
{
  return e->len >= OPT_EXACT_MAXLEN;
}

static void
clear_opt_exact(OptStr* e)
{
  mml_clear(&e->mm);
  clear_opt_anc_info(&e->anc);
  e->reach_end = 0;
  e->len       = 0;
  e->s[0]      = '\0';
}

static void
copy_opt_exact(OptStr* to, OptStr* from)
{
  *to = *from;
}

static int
concat_opt_exact(OptStr* to, OptStr* add, OnigEncoding enc)
{
  int i, j, len, r;
  UChar *p, *end;
  OptAnc tanc;

  r = 0;
  p = add->s;
  end = p + add->len;
  for (i = to->len; p < end; ) {
    len = enclen(enc, p);
    if (i + len > OPT_EXACT_MAXLEN) {
      r = 1; /* 1:full */
      break;
    }
    for (j = 0; j < len && p < end; j++) {
      /* coverity[overrun-local] */
      to->s[i++] = *p++;
    }
  }

  to->len = i;
  to->reach_end = (p == end ? add->reach_end : 0);

  concat_opt_anc_info(&tanc, &to->anc, &add->anc, 1, 1);
  if (! to->reach_end) tanc.right = 0;
  copy_opt_anc_info(&to->anc, &tanc);

  return r;
}

static void
concat_opt_exact_str(OptStr* to, UChar* s, UChar* end, OnigEncoding enc)
{
  int i, j, len;
  UChar *p;

  for (i = to->len, p = s; p < end && i < OPT_EXACT_MAXLEN; ) {
    len = enclen(enc, p);
    if (i + len > OPT_EXACT_MAXLEN) break;
    for (j = 0; j < len && p < end; j++) {
      /* coverity[overrun-local] */
      to->s[i++] = *p++;
    }
  }

  to->len = i;

  if (p >= end)
    to->reach_end = 1;
}

static void
alt_merge_opt_exact(OptStr* to, OptStr* add, OptEnv* env)
{
  int i, j, len;

  if (add->len == 0 || to->len == 0) {
    clear_opt_exact(to);
    return ;
  }

  if (! mml_is_equal(&to->mm, &add->mm)) {
    clear_opt_exact(to);
    return ;
  }

  for (i = 0; i < to->len && i < add->len; ) {
    if (to->s[i] != add->s[i]) break;
    len = enclen(env->enc, to->s + i);

    for (j = 1; j < len; j++) {
      if (to->s[i+j] != add->s[i+j]) break;
    }
    if (j < len) break;
    i += len;
  }

  if (! add->reach_end || i < add->len || i < to->len) {
    to->reach_end = 0;
  }
  to->len = i;

  alt_merge_opt_anc_info(&to->anc, &add->anc);
  if (! to->reach_end) to->anc.right = 0;
}

static void
select_opt_exact(OnigEncoding enc, OptStr* now, OptStr* alt)
{
  int vn, va;

  vn = now->len;
  va = alt->len;

  if (va == 0) {
    return ;
  }
  else if (vn == 0) {
    copy_opt_exact(now, alt);
    return ;
  }
  else if (vn <= 2 && va <= 2) {
    /* ByteValTable[x] is big value --> low price */
    va = map_position_value(enc, now->s[0]);
    vn = map_position_value(enc, alt->s[0]);

    if (now->len > 1) vn += 5;
    if (alt->len > 1) va += 5;
  }

  vn *= 2;
  va *= 2;

  if (comp_distance_value(&now->mm, &alt->mm, vn, va) > 0)
    copy_opt_exact(now, alt);
}

static void
clear_opt_map(OptMap* map)
{
  static const OptMap clean_info = {
    {0, 0}, {0, 0}, 0,
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  };

  xmemcpy(map, &clean_info, sizeof(OptMap));
}

static void
copy_opt_map(OptMap* to, OptMap* from)
{
  *to = *from;
}

static void
add_char_opt_map(OptMap* m, UChar c, OnigEncoding enc)
{
  if (m->map[c] == 0) {
    m->map[c] = 1;
    m->value += map_position_value(enc, c);
  }
}

static void
select_opt_map(OptMap* now, OptMap* alt)
{
  static int z = 1<<15; /* 32768: something big value */

  int vn, va;

  if (alt->value == 0) return ;
  if (now->value == 0) {
    copy_opt_map(now, alt);
    return ;
  }

  vn = z / now->value;
  va = z / alt->value;
  if (comp_distance_value(&now->mm, &alt->mm, vn, va) > 0)
    copy_opt_map(now, alt);
}

static int
comp_opt_exact_or_map(OptStr* e, OptMap* m)
{
#define COMP_EM_BASE  20
  int ae, am;
  int case_value;

  if (m->value <= 0) return -1;

  case_value = 3;
  ae = COMP_EM_BASE * e->len * case_value;
  am = COMP_EM_BASE * 5 * 2 / m->value;
  return comp_distance_value(&e->mm, &m->mm, ae, am);
}

static void
alt_merge_opt_map(OnigEncoding enc, OptMap* to, OptMap* add)
{
  int i, val;

  /* if (! mml_is_equal(&to->mm, &add->mm)) return ; */
  if (to->value == 0) return ;
  if (add->value == 0 || to->mm.max < add->mm.min) {
    clear_opt_map(to);
    return ;
  }

  mml_alt_merge(&to->mm, &add->mm);

  val = 0;
  for (i = 0; i < CHAR_MAP_SIZE; i++) {
    if (add->map[i])
      to->map[i] = 1;

    if (to->map[i])
      val += map_position_value(enc, i);
  }
  to->value = val;

  alt_merge_opt_anc_info(&to->anc, &add->anc);
}

static void
set_bound_node_opt_info(OptNode* opt, MinMaxLen* plen)
{
  mml_copy(&(opt->sb.mm),  plen);
  mml_copy(&(opt->spr.mm), plen);
  mml_copy(&(opt->map.mm), plen);
}

static void
clear_node_opt_info(OptNode* opt)
{
  mml_clear(&opt->len);
  clear_opt_anc_info(&opt->anc);
  clear_opt_exact(&opt->sb);
  clear_opt_exact(&opt->sm);
  clear_opt_exact(&opt->spr);
  clear_opt_map(&opt->map);
}

static void
copy_node_opt_info(OptNode* to, OptNode* from)
{
  *to = *from;
}

static void
concat_left_node_opt_info(OnigEncoding enc, OptNode* to, OptNode* add)
{
  int sb_reach, sm_reach;
  OptAnc tanc;

  concat_opt_anc_info(&tanc, &to->anc, &add->anc, to->len.max, add->len.max);
  copy_opt_anc_info(&to->anc, &tanc);

  if (add->sb.len > 0 && to->len.max == 0) {
    concat_opt_anc_info(&tanc, &to->anc, &add->sb.anc, to->len.max, add->len.max);
    copy_opt_anc_info(&add->sb.anc, &tanc);
  }

  if (add->map.value > 0 && to->len.max == 0) {
    if (add->map.mm.max == 0)
      add->map.anc.left |= to->anc.left;
  }

  sb_reach = to->sb.reach_end;
  sm_reach = to->sm.reach_end;

  if (add->len.max != 0)
    to->sb.reach_end = to->sm.reach_end = 0;

  if (add->sb.len > 0) {
    if (sb_reach) {
      concat_opt_exact(&to->sb, &add->sb, enc);
      clear_opt_exact(&add->sb);
    }
    else if (sm_reach) {
      concat_opt_exact(&to->sm, &add->sb, enc);
      clear_opt_exact(&add->sb);
    }
  }
  select_opt_exact(enc, &to->sm, &add->sb);
  select_opt_exact(enc, &to->sm, &add->sm);

  if (to->spr.len > 0) {
    if (add->len.max > 0) {
      if (to->spr.mm.max == 0)
        select_opt_exact(enc, &to->sb, &to->spr);
      else
        select_opt_exact(enc, &to->sm, &to->spr);
    }
  }
  else if (add->spr.len > 0) {
    copy_opt_exact(&to->spr, &add->spr);
  }

  select_opt_map(&to->map, &add->map);
  mml_add(&to->len, &add->len);
}

static void
alt_merge_node_opt_info(OptNode* to, OptNode* add, OptEnv* env)
{
  alt_merge_opt_anc_info(&to->anc, &add->anc);
  alt_merge_opt_exact(&to->sb,  &add->sb, env);
  alt_merge_opt_exact(&to->sm,  &add->sm, env);
  alt_merge_opt_exact(&to->spr, &add->spr, env);
  alt_merge_opt_map(env->enc, &to->map, &add->map);

  mml_alt_merge(&to->len, &add->len);
}

static OnigLen
node_max_byte_len(Node* node, ParseEnv* env)
{
  OnigLen len;
  OnigLen tmax;

  len = 0;
  switch (ND_TYPE(node)) {
  case ND_LIST:
    do {
      tmax = node_max_byte_len(ND_CAR(node), env);
      len = distance_add(len, tmax);
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_ALT:
    do {
      tmax = node_max_byte_len(ND_CAR(node), env);
      if (len < tmax) len = tmax;
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_STRING:
    {
      StrNode* sn = STR_(node);
      len = (OnigLen )(sn->end - sn->s);
    }
    break;

  case ND_CTYPE:
  case ND_CCLASS:
    len = ONIGENC_MBC_MAXLEN_DIST(env->enc);
    break;

  case ND_BACKREF:
    if (! ND_IS_CHECKER(node)) {
      int i;
      int* backs;
      MemEnv* mem_env = PARSEENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);
      if (ND_IS_RECURSION(node)) {
#ifdef USE_BACKREF_WITH_LEVEL
        if (ND_IS_NEST_LEVEL(node)) {
          len = INFINITE_LEN;
        }
#endif
        break;
      }
      backs = BACKREFS_P(br);
      for (i = 0; i < br->back_num; i++) {
        tmax = node_max_byte_len(mem_env[backs[i]].mem_node, env);
        if (len < tmax) len = tmax;
      }
    }
    break;

#ifdef USE_CALL
  case ND_CALL:
    if (! ND_IS_RECURSION(node))
      len = node_max_byte_len(ND_BODY(node), env);
    else
      len = INFINITE_LEN;
    break;
#endif

  case ND_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->upper != 0) {
        len = node_max_byte_len(ND_BODY(node), env);
        if (len != 0) {
          if (! IS_INFINITE_REPEAT(qn->upper))
            len = distance_multiply(len, qn->upper);
          else
            len = INFINITE_LEN;
        }
      }
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_MEMORY:
        if (ND_IS_FIXED_MAX(node))
          len = en->max_len;
        else {
          if (ND_IS_MARK1(node))
            len = INFINITE_LEN;
          else {
            ND_STATUS_ADD(node, MARK1);
            len = node_max_byte_len(ND_BODY(node), env);
            ND_STATUS_REMOVE(node, MARK1);

            en->max_len = len;
            ND_STATUS_ADD(node, FIXED_MAX);
          }
        }
        break;

      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        len = node_max_byte_len(ND_BODY(node), env);
        break;
      case BAG_IF_ELSE:
        {
          OnigLen tlen, elen;

          len = node_max_byte_len(ND_BODY(node), env);
          if (IS_NOT_NULL(en->te.Then)) {
            tlen = node_max_byte_len(en->te.Then, env);
            len = distance_add(len, tlen);
          }
          if (IS_NOT_NULL(en->te.Else))
            elen = node_max_byte_len(en->te.Else, env);
          else elen = 0;

          if (elen > len) len = elen;
        }
        break;
      }
    }
    break;

  case ND_ANCHOR:
  case ND_GIMMICK:
  default:
    break;
  }

  return len;
}

#define MAX_ND_OPT_INFO_REF_COUNT    5

static int
optimize_nodes(Node* node, OptNode* opt, OptEnv* env)
{
  int i;
  int r;
  OptNode xo;
  OnigEncoding enc;

  r = 0;
  enc = env->enc;
  clear_node_opt_info(opt);
  set_bound_node_opt_info(opt, &env->mm);

  switch (ND_TYPE(node)) {
  case ND_LIST:
    {
      OptEnv nenv;
      Node* nd = node;

      copy_opt_env(&nenv, env);
      do {
        r = optimize_nodes(ND_CAR(nd), &xo, &nenv);
        if (r == 0) {
          mml_add(&nenv.mm, &xo.len);
          concat_left_node_opt_info(enc, opt, &xo);
        }
      } while (r == 0 && IS_NOT_NULL(nd = ND_CDR(nd)));
    }
    break;

  case ND_ALT:
    {
      Node* nd = node;

      do {
        r = optimize_nodes(ND_CAR(nd), &xo, env);
        if (r == 0) {
          if (nd == node) copy_node_opt_info(opt, &xo);
          else            alt_merge_node_opt_info(opt, &xo, env);
        }
      } while ((r == 0) && IS_NOT_NULL(nd = ND_CDR(nd)));
    }
    break;

  case ND_STRING:
    {
      StrNode* sn = STR_(node);
      int slen = (int )(sn->end - sn->s);

      concat_opt_exact_str(&opt->sb, sn->s, sn->end, enc);
      if (slen > 0) {
        add_char_opt_map(&opt->map, *(sn->s), enc);
      }
      mml_set_min_max(&opt->len, slen, slen);
    }
    break;

  case ND_CCLASS:
    {
      int z;
      CClassNode* cc = CCLASS_(node);

      /* no need to check ignore case. (set in tune_tree()) */

      if (IS_NOT_NULL(cc->mbuf) || IS_NCCLASS_NOT(cc)) {
        OnigLen min = ONIGENC_MBC_MINLEN(enc);
        OnigLen max = ONIGENC_MBC_MAXLEN_DIST(enc);

        mml_set_min_max(&opt->len, min, max);
      }
      else {
        for (i = 0; i < SINGLE_BYTE_SIZE; i++) {
          z = BITSET_AT(cc->bs, i);
          if ((z && ! IS_NCCLASS_NOT(cc)) || (! z && IS_NCCLASS_NOT(cc))) {
            add_char_opt_map(&opt->map, (UChar )i, enc);
          }
        }
        mml_set_min_max(&opt->len, 1, 1);
      }
    }
    break;

  case ND_CTYPE:
    {
      int min, max;
      int range;

      max = ONIGENC_MBC_MAXLEN_DIST(enc);

      if (max == 1) {
        min = 1;

        switch (CTYPE_(node)->ctype) {
        case CTYPE_ANYCHAR:
          break;

        case ONIGENC_CTYPE_WORD:
          range = CTYPE_(node)->ascii_mode != 0 ? 128 : SINGLE_BYTE_SIZE;
          if (CTYPE_(node)->not != 0) {
            for (i = 0; i < range; i++) {
              if (! ONIGENC_IS_CODE_WORD(enc, i)) {
                add_char_opt_map(&opt->map, (UChar )i, enc);
              }
            }
            for (i = range; i < SINGLE_BYTE_SIZE; i++) {
              add_char_opt_map(&opt->map, (UChar )i, enc);
            }
          }
          else {
            for (i = 0; i < range; i++) {
              if (ONIGENC_IS_CODE_WORD(enc, i)) {
                add_char_opt_map(&opt->map, (UChar )i, enc);
              }
            }
          }
          break;
        }
      }
      else {
        min = ONIGENC_MBC_MINLEN(enc);
      }
      mml_set_min_max(&opt->len, min, max);
    }
    break;

  case ND_ANCHOR:
    switch (ANCHOR_(node)->type) {
    case ANCR_BEGIN_BUF:
    case ANCR_BEGIN_POSITION:
    case ANCR_BEGIN_LINE:
    case ANCR_END_BUF:
    case ANCR_SEMI_END_BUF:
    case ANCR_END_LINE:
    case ANCR_PREC_READ_NOT:
    case ANCR_LOOK_BEHIND:
      add_opt_anc_info(&opt->anc, ANCHOR_(node)->type);
      break;

    case ANCR_PREC_READ:
      {
        r = optimize_nodes(ND_BODY(node), &xo, env);
        if (r == 0) {
          if (xo.sb.len > 0)
            copy_opt_exact(&opt->spr, &xo.sb);
          else if (xo.sm.len > 0)
            copy_opt_exact(&opt->spr, &xo.sm);

          opt->spr.reach_end = 0;

          if (xo.map.value > 0)
            copy_opt_map(&opt->map, &xo.map);
        }
      }
      break;

    case ANCR_LOOK_BEHIND_NOT:
      break;
    }
    break;

  case ND_BACKREF:
    if (! ND_IS_CHECKER(node)) {
      OnigLen min, max;

      min = node_min_byte_len(node, env->scan_env);
      max = node_max_byte_len(node, env->scan_env);
      mml_set_min_max(&opt->len, min, max);
    }
    break;

#ifdef USE_CALL
  case ND_CALL:
    if (ND_IS_RECURSION(node))
      mml_set_min_max(&opt->len, 0, INFINITE_LEN);
    else {
      r = optimize_nodes(ND_BODY(node), opt, env);
    }
    break;
#endif

  case ND_QUANT:
    {
      OnigLen min, max;
      QuantNode* qn = QUANT_(node);

      /* Issue #175
         ex. /\g<1>{0}(?<=|())/

         Empty and unused nodes in look-behind is removed in
         tune_look_behind().
         Called group nodes are assigned to be not called if the caller side is
         inside of zero-repetition.
         As a result, the nodes are considered unused.
       */
      if (qn->upper == 0) {
        mml_set_min_max(&opt->len, 0, 0);
        break;
      }

      r = optimize_nodes(ND_BODY(node), &xo, env);
      if (r != 0) break;

      if (qn->lower > 0) {
        copy_node_opt_info(opt, &xo);
        if (xo.sb.len > 0) {
          if (xo.sb.reach_end) {
            for (i = 2; i <= qn->lower && ! is_full_opt_exact(&opt->sb); i++) {
              int rc = concat_opt_exact(&opt->sb, &xo.sb, enc);
              if (rc > 0) break;
            }
            if (i < qn->lower) opt->sb.reach_end = 0;
          }
        }

        if (qn->lower != qn->upper) {
          opt->sb.reach_end = 0;
          opt->sm.reach_end = 0;
        }
        if (qn->lower > 1)
          opt->sm.reach_end = 0;
      }

      if (IS_INFINITE_REPEAT(qn->upper)) {
        if (env->mm.max == 0 &&
            ND_IS_ANYCHAR(ND_BODY(node)) && qn->greedy != 0) {
          if (ND_IS_MULTILINE(ND_QUANT_BODY(qn)))
            add_opt_anc_info(&opt->anc, ANCR_ANYCHAR_INF_ML);
          else
            add_opt_anc_info(&opt->anc, ANCR_ANYCHAR_INF);
        }

        max = (xo.len.max > 0 ? INFINITE_LEN : 0);
      }
      else {
        max = distance_multiply(xo.len.max, qn->upper);
      }

      min = distance_multiply(xo.len.min, qn->lower);
      mml_set_min_max(&opt->len, min, max);
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      switch (en->type) {
      case BAG_STOP_BACKTRACK:
      case BAG_OPTION:
        r = optimize_nodes(ND_BODY(node), opt, env);
        break;

      case BAG_MEMORY:
#ifdef USE_CALL
        en->opt_count++;
        if (en->opt_count > MAX_ND_OPT_INFO_REF_COUNT) {
          OnigLen min, max;

          min = 0;
          max = INFINITE_LEN;
          if (ND_IS_FIXED_MIN(node)) min = en->min_len;
          if (ND_IS_FIXED_MAX(node)) max = en->max_len;
          mml_set_min_max(&opt->len, min, max);
        }
        else
#endif
          {
            r = optimize_nodes(ND_BODY(node), opt, env);
            if (is_set_opt_anc_info(&opt->anc, ANCR_ANYCHAR_INF_MASK)) {
              if (MEM_STATUS_AT0(env->scan_env->backrefed_mem, en->m.regnum))
                remove_opt_anc_info(&opt->anc, ANCR_ANYCHAR_INF_MASK);
            }
          }
        break;

      case BAG_IF_ELSE:
        {
          OptEnv nenv;

          if (IS_NOT_NULL(en->te.Else)) {
            copy_opt_env(&nenv, env);
            r = optimize_nodes(ND_BAG_BODY(en), &xo, &nenv);
            if (r == 0) {
              mml_add(&nenv.mm, &xo.len);
              concat_left_node_opt_info(enc, opt, &xo);
              if (IS_NOT_NULL(en->te.Then)) {
                r = optimize_nodes(en->te.Then, &xo, &nenv);
                if (r == 0) {
                  concat_left_node_opt_info(enc, opt, &xo);
                }
              }

                r = optimize_nodes(en->te.Else, &xo, env);
                if (r == 0)
                  alt_merge_node_opt_info(opt, &xo, env);
            }
          }
        }
        break;
      }
    }
    break;

  case ND_GIMMICK:
    break;

  default:
#ifdef ONIG_DEBUG
    fprintf(DBGFP, "optimize_nodes: undefined node type %d\n", ND_TYPE(node));
#endif
    r = ONIGERR_TYPE_BUG;
    break;
  }

  return r;
}

static int
set_optimize_exact(regex_t* reg, OptStr* e)
{
  int r;
  int allow_reverse;

  if (e->len == 0) return 0;

  reg->exact = (UChar* )xmalloc(e->len);
  CHECK_NULL_RETURN_MEMERR(reg->exact);
  xmemcpy(reg->exact, e->s, e->len);
  reg->exact_end = reg->exact + e->len;

  allow_reverse =
    ONIGENC_IS_ALLOWED_REVERSE_MATCH(reg->enc, reg->exact, reg->exact_end);

  if (e->len >= 2 || (e->len >= 1 && allow_reverse)) {
    r = set_sunday_quick_search_or_bmh_skip_table(reg, 0,
                                                  reg->exact, reg->exact_end,
                                                  reg->map, &(reg->map_offset));
    if (r != 0) return r;

    reg->optimize = (allow_reverse != 0
                     ? OPTIMIZE_STR_FAST
                     : OPTIMIZE_STR_FAST_STEP_FORWARD);
  }
  else {
    reg->optimize = OPTIMIZE_STR;
  }

  reg->dist_min = e->mm.min;
  reg->dist_max = e->mm.max;

  if (reg->dist_min != INFINITE_LEN) {
    int n = (int )(reg->exact_end - reg->exact);
    reg->threshold_len = reg->dist_min + n;
  }

  return 0;
}

static void
set_optimize_map(regex_t* reg, OptMap* m)
{
  int i;

  for (i = 0; i < CHAR_MAP_SIZE; i++)
    reg->map[i] = m->map[i];

  reg->optimize   = OPTIMIZE_MAP;
  reg->dist_min   = m->mm.min;
  reg->dist_max   = m->mm.max;

  if (reg->dist_min != INFINITE_LEN) {
    reg->threshold_len = reg->dist_min + ONIGENC_MBC_MINLEN(reg->enc);
  }
}

static void
set_sub_anchor(regex_t* reg, OptAnc* anc)
{
  reg->sub_anchor |= anc->left  & ANCR_BEGIN_LINE;
  reg->sub_anchor |= anc->right & ANCR_END_LINE;
}

#if defined(ONIG_DEBUG_COMPILE) || defined(ONIG_DEBUG_MATCH)
static void print_optimize_info(FILE* f, regex_t* reg);
#endif

static int
set_optimize_info_from_tree(Node* node, regex_t* reg, ParseEnv* scan_env)
{
  int r;
  OptNode opt;
  OptEnv env;

  env.enc            = reg->enc;
  env.case_fold_flag = reg->case_fold_flag;
  env.scan_env       = scan_env;
  mml_clear(&env.mm);

  r = optimize_nodes(node, &opt, &env);
  if (r != 0) return r;

  reg->anchor = opt.anc.left & (ANCR_BEGIN_BUF |
        ANCR_BEGIN_POSITION | ANCR_ANYCHAR_INF | ANCR_ANYCHAR_INF_ML |
        ANCR_LOOK_BEHIND);

  if ((opt.anc.left & (ANCR_LOOK_BEHIND | ANCR_PREC_READ_NOT)) != 0)
    reg->anchor &= ~ANCR_ANYCHAR_INF_ML;

  reg->anchor |= opt.anc.right & (ANCR_END_BUF | ANCR_SEMI_END_BUF |
                                  ANCR_PREC_READ_NOT);

  if (reg->anchor & (ANCR_END_BUF | ANCR_SEMI_END_BUF)) {
    reg->anc_dist_min = opt.len.min;
    reg->anc_dist_max = opt.len.max;
  }

  if (opt.sb.len > 0 || opt.sm.len > 0) {
    select_opt_exact(reg->enc, &opt.sb, &opt.sm);
    if (opt.map.value > 0 && comp_opt_exact_or_map(&opt.sb, &opt.map) > 0) {
      goto set_map;
    }
    else {
      r = set_optimize_exact(reg, &opt.sb);
      set_sub_anchor(reg, &opt.sb.anc);
    }
  }
  else if (opt.map.value > 0) {
  set_map:
    set_optimize_map(reg, &opt.map);
    set_sub_anchor(reg, &opt.map.anc);
  }
  else {
    reg->sub_anchor |= opt.anc.left & ANCR_BEGIN_LINE;
    if (opt.len.max == 0)
      reg->sub_anchor |= opt.anc.right & ANCR_END_LINE;
  }

#if defined(ONIG_DEBUG_COMPILE) || defined(ONIG_DEBUG_MATCH)
  print_optimize_info(DBGFP, reg);
#endif
  return r;
}
#endif /* ONIG_DONT_OPTIMIZE */

static void
clear_optimize_info(regex_t* reg)
{
  reg->optimize      = OPTIMIZE_NONE;
  reg->anchor        = 0;
  reg->anc_dist_min  = 0;
  reg->anc_dist_max  = 0;
  reg->sub_anchor    = 0;
  reg->exact_end     = (UChar* )NULL;
  reg->map_offset    = 0;
  reg->threshold_len = 0;
  if (IS_NOT_NULL(reg->exact)) {
    xfree(reg->exact);
    reg->exact = (UChar* )NULL;
  }
}

#if defined(ONIG_DEBUG_PARSE)  || defined(ONIG_DEBUG_MATCH) || \
    defined(ONIG_DEBUG_SEARCH) || defined(ONIG_DEBUG_COMPILE)

static void
print_enc_string(FILE* fp, OnigEncoding enc,
                 const UChar *s, const UChar *end)
{
  if (ONIGENC_MBC_MINLEN(enc) > 1) {
    const UChar *p;
    OnigCodePoint code;

    p = s;
    while (p < end) {
      code = ONIGENC_MBC_TO_CODE(enc, p, end);
      if (code >= 0x80) {
        fprintf(fp, " 0x%04x ", (int )code);
      }
      else {
        fputc((int )code, fp);
      }

      p += enclen(enc, p);
    }
  }
  else {
    while (s < end) {
      if (ONIGENC_MBC_MAXLEN(enc) == 1) {
        if (*s >= 0x80) {
          fprintf(fp, "\\x%02x", (unsigned int )*s);
        }
        else {
          fputc((int )*s, fp);
        }
      }
      else { /* for UTF-8 */
        fputc((int )*s, fp);
      }
      s++;
    }
  }
}

static void
print_options(FILE* fp, OnigOptionType o)
{
  if ((o & ONIG_OPTION_IGNORECASE) != 0)      fprintf(fp, " IGNORECASE");
  if ((o & ONIG_OPTION_EXTEND) != 0)          fprintf(fp, " EXTEND");
  if ((o & ONIG_OPTION_MULTILINE) != 0)       fprintf(fp, " MULTILINE");
  if ((o & ONIG_OPTION_SINGLELINE) != 0)      fprintf(fp, " SINGLELINE");
  if ((o & ONIG_OPTION_FIND_LONGEST) != 0)    fprintf(fp, " FIND_LONGEST");
  if ((o & ONIG_OPTION_FIND_NOT_EMPTY) != 0)  fprintf(fp, " FIND_NOT_EMPTY");
  if ((o & ONIG_OPTION_NEGATE_SINGLELINE) != 0)  fprintf(fp, " NEGATE_SINGLELINE");
  if ((o & ONIG_OPTION_DONT_CAPTURE_GROUP) != 0) fprintf(fp, " DONT_CAPTURE_GROUP");
  if ((o & ONIG_OPTION_CAPTURE_GROUP) != 0)   fprintf(fp, " CAPTURE_GROUP");
  if ((o & ONIG_OPTION_NOTBOL) != 0)          fprintf(fp, " NOTBOL");
  if ((o & ONIG_OPTION_NOTEOL) != 0)          fprintf(fp, " NOTEOL");
  if ((o & ONIG_OPTION_POSIX_REGION) != 0)    fprintf(fp, " POSIX_REGION");
  if ((o & ONIG_OPTION_CHECK_VALIDITY_OF_STRING) != 0) fprintf(fp, " CHECK_VALIDITY_OF_STRING");
  if ((o & ONIG_OPTION_IGNORECASE_IS_ASCII) != 0) fprintf(fp, " IGNORECASE_IS_ASCII");
  if ((o & ONIG_OPTION_WORD_IS_ASCII) != 0)   fprintf(fp, " WORD_IS_ASCII");
  if ((o & ONIG_OPTION_DIGIT_IS_ASCII) != 0)  fprintf(fp, " DIGIT_IS_ASCII");
  if ((o & ONIG_OPTION_SPACE_IS_ASCII) != 0)  fprintf(fp, " SPACE_IS_ASCII");
  if ((o & ONIG_OPTION_POSIX_IS_ASCII) != 0)  fprintf(fp, " POSIX_IS_ASCII");
  if ((o & ONIG_OPTION_TEXT_SEGMENT_EXTENDED_GRAPHEME_CLUSTER) != 0) fprintf(fp, " TEXT_SEGMENT_EXTENDED_GRAPHEME_CLUSTER");
  if ((o & ONIG_OPTION_TEXT_SEGMENT_WORD) != 0) fprintf(fp, " TEXT_SEGMENT_WORD");
  if ((o & ONIG_OPTION_NOT_BEGIN_STRING) != 0) fprintf(fp, " NOT_BIGIN_STRING");
  if ((o & ONIG_OPTION_NOT_END_STRING) != 0)   fprintf(fp, " NOT_END_STRING");
  if ((o & ONIG_OPTION_NOT_BEGIN_POSITION) != 0) fprintf(fp, " NOT_BEGIN_POSITION");
  if ((o & ONIG_OPTION_CALLBACK_EACH_MATCH) != 0) fprintf(fp, " CALLBACK_EACH_MATCH");
}

#endif

#if defined(ONIG_DEBUG_COMPILE) || defined(ONIG_DEBUG_MATCH)

#ifndef ONIG_DONT_OPTIMIZE

static void
print_distance_range(FILE* f, OnigLen a, OnigLen b)
{
  if (a == INFINITE_LEN)
    fputs("inf", f);
  else
    fprintf(f, "(%u)", a);

  fputs("-", f);

  if (b == INFINITE_LEN)
    fputs("inf", f);
  else
    fprintf(f, "(%u)", b);
}

static void
print_anchor(FILE* f, int anchor)
{
  int q = 0;

  fprintf(f, "[");

  if (anchor & ANCR_BEGIN_BUF) {
    fprintf(f, "begin-buf");
    q = 1;
  }
  if (anchor & ANCR_BEGIN_LINE) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "begin-line");
  }
  if (anchor & ANCR_BEGIN_POSITION) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "begin-pos");
  }
  if (anchor & ANCR_END_BUF) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "end-buf");
  }
  if (anchor & ANCR_SEMI_END_BUF) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "semi-end-buf");
  }
  if (anchor & ANCR_END_LINE) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "end-line");
  }
  if (anchor & ANCR_ANYCHAR_INF) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "anychar-inf");
  }
  if (anchor & ANCR_ANYCHAR_INF_ML) {
    if (q) fprintf(f, ", ");
    fprintf(f, "anychar-inf-ml");
  }

  fprintf(f, "]");
}

static void
print_optimize_info(FILE* f, regex_t* reg)
{
  static const char* on[] =
    { "NONE", "STR", "STR_FAST", "STR_FAST_STEP_FORWARD", "MAP" };

  fprintf(f, "optimize: %s\n", on[reg->optimize]);
  fprintf(f, "  anchor: "); print_anchor(f, reg->anchor);
  if ((reg->anchor & ANCR_END_BUF_MASK) != 0)
    print_distance_range(f, reg->anc_dist_min, reg->anc_dist_max);
  fprintf(f, "\n");

  if (reg->optimize) {
    fprintf(f, "  sub anchor: "); print_anchor(f, reg->sub_anchor);
    fprintf(f, "\n");
  }
  fprintf(f, "\n");

  if (reg->exact) {
    UChar *p;
    fprintf(f, "exact: [");
    for (p = reg->exact; p < reg->exact_end; p++) {
      fputc(*p, f);
    }
    fprintf(f, "]: length: %ld, dmin: %u, ",
            (reg->exact_end - reg->exact), reg->dist_min);
    if (reg->dist_max == INFINITE_LEN)
      fprintf(f, "dmax: inf.\n");
    else
      fprintf(f, "dmax: %u\n", reg->dist_max);
  }
  else if (reg->optimize & OPTIMIZE_MAP) {
    int c, i, n = 0;

    for (i = 0; i < CHAR_MAP_SIZE; i++)
      if (reg->map[i]) n++;

    fprintf(f, "map: n=%d, dmin: %u, dmax: %u\n",
            n, reg->dist_min, reg->dist_max);
    if (n > 0) {
      c = 0;
      fputc('[', f);
      for (i = 0; i < CHAR_MAP_SIZE; i++) {
        if (reg->map[i] != 0) {
          if (c > 0)  fputs(", ", f);
          c++;
          if (ONIGENC_MBC_MAXLEN(reg->enc) == 1 &&
              ONIGENC_IS_CODE_PRINT(reg->enc, (OnigCodePoint )i))
            fputc(i, f);
          else
            fprintf(f, "0x%02x", i);
        }
      }
      fprintf(f, "]\n");
    }
  }
}
#endif /* ONIG_DONT_OPTIMIZE */
#endif /* defined(ONIG_DEBUG_COMPILE) || defined(ONIG_DEBUG_MATCH) */


extern RegexExt*
onig_get_regex_ext(regex_t* reg)
{
  if (IS_NULL(reg->extp)) {
    RegexExt* ext = (RegexExt* )xmalloc(sizeof(*ext));
    if (IS_NULL(ext)) return 0;

    ext->pattern      = 0;
    ext->pattern_end  = 0;
#ifdef USE_CALLOUT
    ext->tag_table    = 0;
    ext->callout_num  = 0;
    ext->callout_list_alloc = 0;
    ext->callout_list = 0;
#endif

    reg->extp = ext;
  }

  return reg->extp;
}

static void
free_regex_ext(RegexExt* ext)
{
  if (IS_NOT_NULL(ext)) {
    if (IS_NOT_NULL(ext->pattern))
      xfree((void* )ext->pattern);

#ifdef USE_CALLOUT
    if (IS_NOT_NULL(ext->tag_table))
      onig_callout_tag_table_free(ext->tag_table);

    if (IS_NOT_NULL(ext->callout_list))
      onig_free_reg_callout_list(ext->callout_num, ext->callout_list);
#endif

    xfree(ext);
  }
}

extern int
onig_ext_set_pattern(regex_t* reg, const UChar* pattern, const UChar* pattern_end)
{
  RegexExt* ext;
  UChar* s;

  ext = onig_get_regex_ext(reg);
  CHECK_NULL_RETURN_MEMERR(ext);

  s = onigenc_strdup(reg->enc, pattern, pattern_end);
  CHECK_NULL_RETURN_MEMERR(s);

  ext->pattern     = s;
  ext->pattern_end = s + (pattern_end - pattern);

  return ONIG_NORMAL;
}

extern void
onig_free_body(regex_t* reg)
{
  if (IS_NOT_NULL(reg)) {
    ops_free(reg);
    if (IS_NOT_NULL(reg->string_pool)) {
      xfree(reg->string_pool);
      reg->string_pool_end = reg->string_pool = 0;
    }
    if (IS_NOT_NULL(reg->exact))            xfree(reg->exact);
    if (IS_NOT_NULL(reg->repeat_range))     xfree(reg->repeat_range);
    if (IS_NOT_NULL(reg->extp)) {
      free_regex_ext(reg->extp);
      reg->extp = 0;
    }

    onig_names_free(reg);
  }
}

extern void
onig_free(regex_t* reg)
{
  if (IS_NOT_NULL(reg)) {
    onig_free_body(reg);
    xfree(reg);
  }
}


#ifdef ONIG_DEBUG_PARSE
static void print_tree P_((FILE* f, Node* node));
#endif

extern int onig_init_for_match_at(regex_t* reg);

static int parse_and_tune(regex_t* reg, const UChar* pattern,
  const UChar* pattern_end, ParseEnv *scan_env, Node** rroot,
  OnigErrorInfo* einfo
#ifdef USE_CALL
  , UnsetAddrList* uslist
#endif
)
{
  int r;
  Node* root;

  root = NULL_NODE;
  if (IS_NOT_NULL(einfo)) {
    einfo->enc = reg->enc;
    einfo->par = (UChar* )NULL;
  }

  r = onig_parse_tree(&root, pattern, pattern_end, reg, scan_env);
  if (r != 0) goto err;

#ifdef USE_WHOLE_OPTIONS
  if ((scan_env->flags & PE_FLAG_HAS_WHOLE_OPTIONS) != 0) {
    r = check_whole_options_position(root, scan_env);
    if (r != 0) goto err;
  }
#endif

  r = reduce_string_list(root, reg->enc);
  if (r != 0) goto err;

  /* mixed use named group and no-named group */
  if (scan_env->num_named > 0 &&
      IS_SYNTAX_BV(scan_env->syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
      ! OPTON_CAPTURE_GROUP(reg->options)) {
    if (scan_env->num_named != scan_env->num_mem)
      r = disable_noname_group_capture(&root, reg, scan_env);
    else
      r = numbered_ref_check(root);

    if (r != 0) goto err;
  }

  r = check_backrefs(root, scan_env);
  if (r != 0) goto err;

#ifdef USE_CALL
  if (scan_env->num_call > 0) {
    r = unset_addr_list_init(uslist, scan_env->num_call);
    if (r != 0) goto err;
    scan_env->unset_addr_list = uslist;
    r = tune_call(root, scan_env, 0);
    if (r != 0) goto err_unset;
    r = tune_call2(root);
    if (r != 0) goto err_unset;
    r = recursive_call_check_trav(root, scan_env, 0);
    if (r  < 0) goto err_unset;
    r = infinite_recursive_call_check_trav(root, scan_env);
    if (r != 0) goto err_unset;

    tune_called_state(root, 0);
  }

  reg->num_call = scan_env->num_call;
#endif

#ifdef ONIG_DEBUG_PARSE
  fprintf(DBGFP, "MAX PARSE DEPTH: %d\n", scan_env->max_parse_depth);
#endif

  r = tune_tree(root, reg, 0, scan_env);
  if (r != 0) {
#ifdef ONIG_DEBUG_PARSE
    fprintf(DBGFP, "TREE (error in tune)\n");
    print_tree(DBGFP, root);
    fprintf(DBGFP, "\n");
#endif
    goto err_unset;
  }

  if (scan_env->backref_num != 0) {
    set_parent_node_trav(root, NULL_NODE);
    r = set_empty_repeat_node_trav(root, NULL_NODE, scan_env);
    if (r != 0) goto err_unset;
    set_empty_status_check_trav(root, scan_env);
  }

  *rroot = root;
  return r;

 err_unset:
#ifdef USE_CALL
  if (scan_env->num_call > 0) {
    unset_addr_list_end(uslist);
  }
#endif
 err:
  if (IS_NOT_NULL(scan_env->error)) {
    if (IS_NOT_NULL(einfo)) {
      einfo->par     = scan_env->error;
      einfo->par_end = scan_env->error_end;
    }
  }

  onig_node_free(root);
  if (IS_NOT_NULL(scan_env->mem_env_dynamic))
    xfree(scan_env->mem_env_dynamic);

  *rroot = NULL_NODE;
  return r;
}

extern int
onig_compile(regex_t* reg, const UChar* pattern, const UChar* pattern_end,
             OnigErrorInfo* einfo)
{
  int r;
  Node* root;
  ParseEnv scan_env;
#ifdef USE_CALL
  UnsetAddrList uslist = {0};
#endif

#if defined(ONIG_DEBUG_PARSE)  || defined(ONIG_DEBUG_MATCH) || \
    defined(ONIG_DEBUG_SEARCH) || defined(ONIG_DEBUG_COMPILE)
  fprintf(DBGFP, "\nPATTERN: /");
  print_enc_string(DBGFP, reg->enc, pattern, pattern_end);
  fprintf(DBGFP, "/\n");
  fprintf(DBGFP, "OPTIONS:");
  print_options(DBGFP, reg->options);
  fprintf(DBGFP, "\n");
#endif

  if (reg->ops_alloc == 0) {
    r = ops_init(reg, OPS_INIT_SIZE);
    if (r != 0) {
      if (IS_NOT_NULL(einfo)) {
        einfo->enc = reg->enc;
        einfo->par = (UChar* )NULL;
      }
      return r;
    }
  }
  else
    reg->ops_used = 0;

  r = parse_and_tune(reg, pattern, pattern_end, &scan_env, &root, einfo
#ifdef USE_CALL
                     , &uslist
#endif
                    );
  if (r != 0) return r;

#ifdef ONIG_DEBUG_PARSE
  fprintf(DBGFP, "TREE (after tune)\n");
  print_tree(DBGFP, root);
  fprintf(DBGFP, "\n");
#endif

  reg->capture_history = scan_env.cap_history;
  reg->push_mem_start  = scan_env.backtrack_mem | scan_env.cap_history;

#ifdef USE_CALLOUT
  if (IS_NOT_NULL(reg->extp) && reg->extp->callout_num != 0) {
    reg->push_mem_end = reg->push_mem_start;
  }
  else {
    if (MEM_STATUS_IS_ALL_ON(reg->push_mem_start))
      reg->push_mem_end = scan_env.backrefed_mem | scan_env.cap_history;
    else
      reg->push_mem_end = reg->push_mem_start &
                        (scan_env.backrefed_mem | scan_env.cap_history);
  }
#else
  if (MEM_STATUS_IS_ALL_ON(reg->push_mem_start))
    reg->push_mem_end = scan_env.backrefed_mem | scan_env.cap_history;
  else
    reg->push_mem_end = reg->push_mem_start &
                      (scan_env.backrefed_mem | scan_env.cap_history);
#endif

  clear_optimize_info(reg);
#ifndef ONIG_DONT_OPTIMIZE
  r = set_optimize_info_from_tree(root, reg, &scan_env);
  if (r != 0)  {
#ifdef USE_CALL
    if (scan_env.num_call > 0) {
      unset_addr_list_end(&uslist);
    }
#endif
    goto err;
  }
#endif

  if (IS_NOT_NULL(scan_env.mem_env_dynamic)) {
    xfree(scan_env.mem_env_dynamic);
    scan_env.mem_env_dynamic = (MemEnv* )NULL;
  }

  r = compile_tree(root, reg, &scan_env);
  if (r == 0) {
    if (scan_env.keep_num > 0) {
      r = add_op(reg, OP_UPDATE_VAR);
      if (r != 0) goto err;

      COP(reg)->update_var.type = UPDATE_VAR_KEEP_FROM_STACK_LAST;
      COP(reg)->update_var.id   = 0; /* not used */
      COP(reg)->update_var.clear = FALSE;
    }

    r = add_op(reg, OP_END);
    if (r != 0) goto err;

#ifdef USE_CALL
    if (scan_env.num_call > 0) {
      r = fix_unset_addr_list(&uslist, reg);
      unset_addr_list_end(&uslist);
      if (r != 0) goto err;
    }
#endif

    r = ops_resize(reg, reg->ops_used);
    if (r != ONIG_NORMAL) goto err;

    set_addr_in_repeat_range(reg);

    if ((reg->push_mem_end != 0)
#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR
        || (reg->num_repeat      != 0)
        || (reg->num_empty_check != 0)
#endif
#ifdef USE_CALLOUT
        || (IS_NOT_NULL(reg->extp) && reg->extp->callout_num != 0)
#endif
#ifdef USE_CALL
        || scan_env.num_call > 0
#endif
        )
      reg->stack_pop_level = STACK_POP_LEVEL_ALL;
    else {
      if (reg->push_mem_start != 0)
        reg->stack_pop_level = STACK_POP_LEVEL_MEM_START;
      else
        reg->stack_pop_level = STACK_POP_LEVEL_FREE;
    }

    r = ops_make_string_pool(reg);
    if (r != 0) goto err;
  }
#ifdef USE_CALL
  else if (scan_env.num_call > 0) {
    unset_addr_list_end(&uslist);
  }
#endif
  onig_node_free(root);

#ifdef ONIG_DEBUG_COMPILE
  onig_print_names(DBGFP, reg);
  onig_print_compiled_byte_code_list(DBGFP, reg);
#endif

#ifdef USE_DIRECT_THREADED_CODE
  /* opcode -> opaddr */
  onig_init_for_match_at(reg);
#endif

  return r;

 err:
  if (IS_NOT_NULL(scan_env.error)) {
    if (IS_NOT_NULL(einfo)) {
      einfo->par     = scan_env.error;
      einfo->par_end = scan_env.error_end;
    }
  }

  onig_node_free(root);
  if (IS_NOT_NULL(scan_env.mem_env_dynamic))
      xfree(scan_env.mem_env_dynamic);
  return r;
}


static int onig_inited = 0;

extern int
onig_reg_init(regex_t* reg, OnigOptionType option, OnigCaseFoldType case_fold_flag,
              OnigEncoding enc, OnigSyntaxType* syntax)
{
  int r;

  xmemset(reg, 0, sizeof(*reg));

  if (onig_inited == 0) {
#if 0
    return ONIGERR_LIBRARY_IS_NOT_INITIALIZED;
#else
    r = onig_initialize(&enc, 1);
    if (r != 0)
      return ONIGERR_FAIL_TO_INITIALIZE;

    onig_warning("You didn't call onig_initialize() explicitly");
#endif
  }

  if (IS_NULL(reg))
    return ONIGERR_INVALID_ARGUMENT;

  if (ONIGENC_IS_UNDEF(enc))
    return ONIGERR_DEFAULT_ENCODING_IS_NOT_SET;

  if ((option & (ONIG_OPTION_DONT_CAPTURE_GROUP|ONIG_OPTION_CAPTURE_GROUP))
      == (ONIG_OPTION_DONT_CAPTURE_GROUP|ONIG_OPTION_CAPTURE_GROUP)) {
    return ONIGERR_INVALID_COMBINATION_OF_OPTIONS;
  }

  if ((option & ONIG_OPTION_NEGATE_SINGLELINE) != 0) {
    option |= syntax->options;
    option &= ~ONIG_OPTION_SINGLELINE;
  }
  else
    option |= syntax->options;

  if ((option & ONIG_OPTION_IGNORECASE_IS_ASCII) != 0) {
    case_fold_flag &= ~(INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR |
                        ONIGENC_CASE_FOLD_TURKISH_AZERI);
    case_fold_flag |= ONIGENC_CASE_FOLD_ASCII_ONLY;
  }

  (reg)->enc            = enc;
  (reg)->options        = option;
  (reg)->syntax         = syntax;
  (reg)->optimize       = 0;
  (reg)->exact          = (UChar* )NULL;
  (reg)->extp           = (RegexExt* )NULL;
  (reg)->ops            = (Operation* )NULL;
  (reg)->ops_curr       = (Operation* )NULL;
  (reg)->ops_used       = 0;
  (reg)->ops_alloc      = 0;
  (reg)->name_table     = (void* )NULL;
  (reg)->case_fold_flag = case_fold_flag;
  return 0;
}

extern int
onig_new_without_alloc(regex_t* reg,
                       const UChar* pattern, const UChar* pattern_end,
                       OnigOptionType option, OnigEncoding enc,
                       OnigSyntaxType* syntax, OnigErrorInfo* einfo)
{
  int r;

  r = onig_reg_init(reg, option, ONIGENC_CASE_FOLD_DEFAULT, enc, syntax);
  if (r != 0) return r;

  r = onig_compile(reg, pattern, pattern_end, einfo);
  return r;
}

extern int
onig_new(regex_t** reg, const UChar* pattern, const UChar* pattern_end,
         OnigOptionType option, OnigEncoding enc, OnigSyntaxType* syntax,
         OnigErrorInfo* einfo)
{
  int r;

  *reg = (regex_t* )xmalloc(sizeof(regex_t));
  if (IS_NULL(*reg)) return ONIGERR_MEMORY;

  r = onig_reg_init(*reg, option, ONIGENC_CASE_FOLD_DEFAULT, enc, syntax);
  if (r != 0) {
    xfree(*reg);
    *reg = NULL;
    return r;
  }

  r = onig_compile(*reg, pattern, pattern_end, einfo);
  if (r != 0) {
    onig_free(*reg);
    *reg = NULL;
  }
  return r;
}

extern int
onig_initialize(OnigEncoding encodings[], int n)
{
  int i;
  int r;

  if (onig_inited != 0)
    return 0;

  onigenc_init();

  onig_inited = 1;

  for (i = 0; i < n; i++) {
    OnigEncoding enc = encodings[i];
    r = onig_initialize_encoding(enc);
    if (r != 0)
      return r;
  }

  return ONIG_NORMAL;
}

typedef struct EndCallListItem {
  struct EndCallListItem* next;
  void (*func)(void);
} EndCallListItemType;

static EndCallListItemType* EndCallTop;

extern void onig_add_end_call(void (*func)(void))
{
  EndCallListItemType* item;

  item = (EndCallListItemType* )xmalloc(sizeof(*item));
  if (item == 0) return ;

  item->next = EndCallTop;
  item->func = func;

  EndCallTop = item;
}

static void
exec_end_call_list(void)
{
  EndCallListItemType* prev;
  void (*func)(void);

  while (EndCallTop != 0) {
    func = EndCallTop->func;
    (*func)();

    prev = EndCallTop;
    EndCallTop = EndCallTop->next;
    xfree(prev);
  }
}

extern int
onig_end(void)
{
  exec_end_call_list();

#ifdef USE_CALLOUT
  onig_global_callout_names_free();
#endif

  onigenc_end();

  onig_inited = 0;

  return 0;
}

extern int
onig_is_in_code_range(const UChar* p, OnigCodePoint code)
{
  OnigCodePoint n, *data;
  OnigCodePoint low, high, x;

  GET_CODE_POINT(n, p);
  data = (OnigCodePoint* )p;
  data++;

  for (low = 0, high = n; low < high; ) {
    x = (low + high) >> 1;
    if (code > data[x * 2 + 1])
      low = x + 1;
    else
      high = x;
  }

  return ((low < n && code >= data[low * 2]) ? 1 : 0);
}

extern int
onig_is_code_in_cc_len(int elen, OnigCodePoint code, /* CClassNode* */ void* cc_arg)
{
  int found;
  CClassNode* cc = (CClassNode* )cc_arg;

  if (elen > 1 || (code >= SINGLE_BYTE_SIZE)) {
    if (IS_NULL(cc->mbuf)) {
      found = 0;
    }
    else {
      found = onig_is_in_code_range(cc->mbuf->p, code) != 0;
    }
  }
  else {
    found = BITSET_AT(cc->bs, code) != 0;
  }

  if (IS_NCCLASS_NOT(cc))
    return !found;
  else
    return found;
}

extern int
onig_is_code_in_cc(OnigEncoding enc, OnigCodePoint code, CClassNode* cc)
{
  int len;

  if (ONIGENC_MBC_MINLEN(enc) > 1) {
    len = 2;
  }
  else {
    len = ONIGENC_CODE_TO_MBCLEN(enc, code);
    if (len < 0) return 0;
  }
  return onig_is_code_in_cc_len(len, code, cc);
}


#define MANY_REPEAT_OF_ANYCHAR   20

typedef enum {
  MJ_NO     = 0,
  MJ_YES    = 1,
  MJ_IGNORE = 2,
} MJ_RESULT;

static MJ_RESULT
mostly_just_anychar(Node* node, int in_reluctant)
{
  MJ_RESULT r;

  r = MJ_NO;
  switch (ND_TYPE(node)) {
  case ND_LIST:
    {
      int found = FALSE;
      do {
        r = mostly_just_anychar(ND_CAR(node), in_reluctant);
        if (r == MJ_NO) break;
        if (r == MJ_YES) found = TRUE;
      } while (IS_NOT_NULL(node = ND_CDR(node)));
      if (r == MJ_IGNORE) {
        if (found == TRUE) r = MJ_YES;
      }
    }
    break;

  case ND_ALT:
    r = MJ_IGNORE;
    do {
      r = mostly_just_anychar(ND_CAR(node), in_reluctant);
      if (r == MJ_YES) break;
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->upper == 0)
        r = MJ_IGNORE;
      else {
        if (in_reluctant == FALSE) {
          if (qn->greedy != 0 &&
              (! IS_INFINITE_REPEAT(qn->upper) &&
               qn->upper <= MANY_REPEAT_OF_ANYCHAR)) {
            in_reluctant = TRUE;
          }
        }
        r = mostly_just_anychar(ND_BODY(node), in_reluctant);
      }
    }
    break;

  case ND_ANCHOR:
    switch (ANCHOR_(node)->type) {
    case ANCR_PREC_READ:
    case ANCR_PREC_READ_NOT:
    case ANCR_LOOK_BEHIND:
    case ANCR_LOOK_BEHIND_NOT:
    case ANCR_TEXT_SEGMENT_BOUNDARY: /* \y */
      r = MJ_IGNORE;
      break;
    default:
      break;
    }
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = mostly_just_anychar(en->te.Then, in_reluctant);
          if (r == MJ_YES) break;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = mostly_just_anychar(en->te.Else, in_reluctant);
        }
      }
      else {
        r = mostly_just_anychar(ND_BODY(node), in_reluctant);
      }
    }
    break;

  case ND_CTYPE:
    if (CTYPE_(node)->ctype == CTYPE_ANYCHAR)
      r = MJ_YES;
    else
      r = MJ_NO;
    break;

  case ND_STRING:
    if (ND_STRING_LEN(node) == 0) {
      r = MJ_IGNORE;
      break;
    }
    /* fall */
  case ND_CCLASS:
    r = MJ_NO;
    break;

#ifdef USE_CALL
  case ND_CALL:
    /* ignore call */
#endif
  case ND_BACKREF:
  case ND_GIMMICK:
    r = MJ_IGNORE;
    break;

  default:
    break;
  }

  return r;
}

#define MAX_CALLS_IN_DETECT   10

typedef struct {
  int prec_read;
  int look_behind;
  int backref;
  int backref_with_level;
  int call;
  int is_keep;
  int anychar_reluctant_many;
  int empty_check_nest_level;
  int max_empty_check_nest_level;
  int heavy_element;
} SlowElementCount;

static int
detect_can_be_slow(Node* node, SlowElementCount* ct, int ncall, int calls[])
{
  int r;

  r = 0;
  switch (ND_TYPE(node)) {
  case ND_LIST:
  case ND_ALT:
    do {
      r = detect_can_be_slow(ND_CAR(node), ct, ncall, calls);
      if (r != 0) return r;
    } while (IS_NOT_NULL(node = ND_CDR(node)));
    break;

  case ND_QUANT:
    {
      QuantNode* qn;
      Node* body;
      int prev_heavy_element = 0;

      qn = QUANT_(node);
      body = ND_BODY(node);

      if (qn->emptiness != BODY_IS_NOT_EMPTY) {
        prev_heavy_element = ct->heavy_element;
        ct->empty_check_nest_level++;
        if (ct->empty_check_nest_level > ct->max_empty_check_nest_level)
          ct->max_empty_check_nest_level = ct->empty_check_nest_level;
      }
      else if (IS_INFINITE_REPEAT(qn->upper) || qn->upper > 0) {
        MJ_RESULT mr = mostly_just_anychar(body, (qn->greedy == 0));
        if (mr == MJ_YES)
          ct->anychar_reluctant_many++;
      }

      r = detect_can_be_slow(body, ct, ncall, calls);

      if (qn->emptiness != BODY_IS_NOT_EMPTY) {
        if (ND_IS_INPEEK(node)) {
          if (ct->empty_check_nest_level > 2) {
            if (prev_heavy_element == ct->heavy_element)
              ct->heavy_element++;
          }
        }
        ct->empty_check_nest_level--;
      }
    }
    break;

  case ND_ANCHOR:
    switch (ANCHOR_(node)->type) {
    case ANCR_PREC_READ:
    case ANCR_PREC_READ_NOT:
      ct->prec_read++;
      break;
    case ANCR_LOOK_BEHIND:
    case ANCR_LOOK_BEHIND_NOT:
      ct->look_behind++;
      break;
    default:
      break;
    }

    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      r = detect_can_be_slow(ND_BODY(node), ct, ncall, calls);
    break;

  case ND_BAG:
    {
      BagNode* en = BAG_(node);

      r = detect_can_be_slow(ND_BODY(node), ct, ncall, calls);
      if (r != 0) return r;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = detect_can_be_slow(en->te.Then, ct, ncall, calls);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = detect_can_be_slow(en->te.Else, ct, ncall, calls);
          if (r != 0) return r;
        }
      }
    }
    break;

#ifdef USE_BACKREF_WITH_LEVEL
  case ND_BACKREF:
    if (ND_IS_NEST_LEVEL(node))
      ct->heavy_element++;
    else
      ct->backref++;
    break;
#endif

#ifdef USE_CALL
  case ND_CALL:
    {
      int i;
      int found;
      int gnum;

      gnum = CALL_(node)->called_gnum;
      ct->call++;

      if (ND_IS_RECURSION(node) && ND_IS_INPEEK(node) &&
          ND_IS_IN_REAL_REPEAT(node)) {
         ct->heavy_element += 10;
      }

      found = FALSE;
      for (i = 0; i < ncall; i++) {
        if (gnum == calls[i]) {
          found = TRUE;
          break;
        }
      }

      if (! found) {
        if (ncall + 1 < MAX_CALLS_IN_DETECT) {
          calls[ncall] = gnum;
          r = detect_can_be_slow(ND_BODY(node), ct, ncall + 1, calls);
        }
        else {
          ct->heavy_element++;
        }
      }
    }
    break;
#endif
  case ND_GIMMICK:
    {
      GimmickNode* g = GIMMICK_(node);
      if (g->type == GIMMICK_SAVE && g->detail_type == SAVE_KEEP)
        ct->is_keep = TRUE;
    }
    break;

  default:
    break;
  }

  return r;
}

extern int
onig_detect_can_be_slow_pattern(const UChar* pattern,
  const UChar* pattern_end, OnigOptionType option, OnigEncoding enc,
  OnigSyntaxType* syntax)
{
  int r;
  regex_t* reg;
  Node* root;
  ParseEnv scan_env;
  SlowElementCount count;
  int calls[MAX_CALLS_IN_DETECT];
#ifdef USE_CALL
  UnsetAddrList  uslist = {0};
#endif

  reg = (regex_t* )xmalloc(sizeof(regex_t));
  if (IS_NULL(reg)) return ONIGERR_MEMORY;

  r = onig_reg_init(reg, option, ONIGENC_CASE_FOLD_DEFAULT, enc, syntax);
  if (r != 0) {
    xfree(reg);
    return r;
  }

  r = parse_and_tune(reg, pattern, pattern_end, &scan_env, &root, NULL
#ifdef USE_CALL
                     , &uslist
#endif
                    );
  if (r != 0) goto err;

#ifdef USE_CALL
  if (scan_env.num_call > 0) {
    unset_addr_list_end(&uslist);
  }
#endif

  count.prec_read          = 0;
  count.look_behind        = 0;
  count.backref            = 0;
  count.backref_with_level = 0;
  count.call               = 0;
  count.is_keep            = FALSE;
  count.anychar_reluctant_many     = 0;
  count.empty_check_nest_level     = 0;
  count.max_empty_check_nest_level = 0;
  count.heavy_element = 0;

  r = detect_can_be_slow(root, &count, 0, calls);
  if (r == 0) {
    int n;

    n = count.prec_read + count.look_behind
      + count.backref + count.backref_with_level + count.call
      + count.anychar_reluctant_many;

    if (count.is_keep) count.max_empty_check_nest_level++;

    if (count.max_empty_check_nest_level > 2)
      n += count.max_empty_check_nest_level - 2;
    if (count.heavy_element != 0) {
      if (count.heavy_element < 0x10000)
        n += count.heavy_element << 8;
      else
        n += count.heavy_element;
    }

    r = n;

#ifdef ONIG_DEBUG_PARSE
    fprintf(DBGFP, "-- detect can be slow --\n");
    fprintf(DBGFP, "  prec_read:          %d\n", count.prec_read);
    fprintf(DBGFP, "  look_behind:        %d\n", count.look_behind);
    fprintf(DBGFP, "  backref:            %d\n", count.backref);
    fprintf(DBGFP, "  backref_with_level: %d\n", count.backref_with_level);
    fprintf(DBGFP, "  call:               %d\n", count.call);
    fprintf(DBGFP, "  is_keep:            %d\n", count.is_keep);
    fprintf(DBGFP, "  any_reluctant_many: %d\n", count.anychar_reluctant_many);
    fprintf(DBGFP, "  max_empty_check_nest_level: %d\n", count.max_empty_check_nest_level);
    fprintf(DBGFP, "  heavy_element:      %d\n", count.heavy_element);
    fprintf(DBGFP, "  r:                  %d\n", r);
    fprintf(DBGFP, "\n");
#endif
  }

  if (IS_NOT_NULL(scan_env.mem_env_dynamic))
    xfree(scan_env.mem_env_dynamic);

 err:
  onig_node_free(root);
  onig_free(reg);
  return r;
}


#ifdef ONIG_DEBUG_PARSE

#ifdef USE_CALL
static void
p_string(FILE* f, int len, UChar* s)
{
  fputs(":", f);
  while (len-- > 0) { fputc(*s++, f); }
}
#endif

static void
Indent(FILE* f, int indent)
{
  int i;
  for (i = 0; i < indent; i++) putc(' ', f);
}

static void
print_indent_tree(FILE* f, Node* node, int indent)
{
  static char* emptiness_name[] = { "", " empty", " empty_mem", " empty_rec" };

  int i;
  NodeType type;
  UChar* p;
  int add = 3;

  Indent(f, indent);
  if (IS_NULL(node)) {
    fprintf(f, "ERROR: null node!!!\n");
    exit(0);
  }

  type = ND_TYPE(node);
  switch (type) {
  case ND_LIST:
  case ND_ALT:
    if (type == ND_LIST)
      fprintf(f, "<list:%p>\n", node);
    else
      fprintf(f, "<alt:%p>\n", node);

    print_indent_tree(f, ND_CAR(node), indent + add);
    while (IS_NOT_NULL(node = ND_CDR(node))) {
      if (ND_TYPE(node) != type) {
        fprintf(f, "ERROR: list/alt right is not a cons. %d\n", ND_TYPE(node));
        exit(0);
      }
      print_indent_tree(f, ND_CAR(node), indent + add);
    }
    break;

  case ND_STRING:
    {
      char* str;
      char* mode;

      if (ND_STRING_IS_CRUDE(node))
        mode = "-crude";
      else if (ND_IS_IGNORECASE(node))
        mode = "-ignorecase";
      else
        mode = "";

      if (STR_(node)->s == STR_(node)->end)
        str = "empty-string";
      else
        str = "string";

      fprintf(f, "<%s%s:%p>", str, mode, node);
      for (p = STR_(node)->s; p < STR_(node)->end; p++) {
        if (*p >= 0x20 && *p < 0x7f)
          fputc(*p, f);
        else {
          fprintf(f, " 0x%02x", *p);
        }
      }
    }
    break;

  case ND_CCLASS:
#define CCLASS_MBUF_MAX_OUTPUT_NUM   10

    fprintf(f, "<cclass:%p>", node);
    if (IS_NCCLASS_NOT(CCLASS_(node))) fputs(" not", f);
    if (CCLASS_(node)->mbuf) {
      BBuf* bbuf = CCLASS_(node)->mbuf;
      fprintf(f, " mbuf(%u) ", bbuf->used);
      for (i = 0; i < bbuf->used && i < CCLASS_MBUF_MAX_OUTPUT_NUM; i++) {
        if (i > 0) fprintf(f, ",");
        fprintf(f, "%0x", bbuf->p[i]);
      }
      if (i < bbuf->used) fprintf(f, "...");
    }
    break;

  case ND_CTYPE:
    fprintf(f, "<ctype:%p> ", node);
    switch (CTYPE_(node)->ctype) {
    case CTYPE_ANYCHAR:
      fprintf(f, "anychar");
      break;

    case ONIGENC_CTYPE_WORD:
      if (CTYPE_(node)->not != 0)
        fputs("not word", f);
      else
        fputs("word",     f);

      if (CTYPE_(node)->ascii_mode != 0)
        fputs(" (ascii)", f);

      break;

    default:
      fprintf(f, "ERROR: undefined ctype.\n");
      exit(0);
    }
    break;

  case ND_ANCHOR:
    fprintf(f, "<anchor:%p> ", node);
    switch (ANCHOR_(node)->type) {
    case ANCR_BEGIN_BUF:        fputs("begin buf",      f); break;
    case ANCR_END_BUF:          fputs("end buf",        f); break;
    case ANCR_BEGIN_LINE:       fputs("begin line",     f); break;
    case ANCR_END_LINE:         fputs("end line",       f); break;
    case ANCR_SEMI_END_BUF:     fputs("semi end buf",   f); break;
    case ANCR_BEGIN_POSITION:   fputs("begin position", f); break;

    case ANCR_WORD_BOUNDARY:    fputs("word boundary",     f); break;
    case ANCR_NO_WORD_BOUNDARY: fputs("not word boundary", f); break;
#ifdef USE_WORD_BEGIN_END
    case ANCR_WORD_BEGIN:       fputs("word begin", f);     break;
    case ANCR_WORD_END:         fputs("word end", f);       break;
#endif
    case ANCR_TEXT_SEGMENT_BOUNDARY:
      fputs("text-segment boundary", f); break;
    case ANCR_NO_TEXT_SEGMENT_BOUNDARY:
      fputs("no text-segment boundary", f); break;
    case ANCR_PREC_READ:
      fprintf(f, "prec read\n");
      print_indent_tree(f, ND_BODY(node), indent + add);
      break;
    case ANCR_PREC_READ_NOT:
      fprintf(f, "prec read not\n");
      print_indent_tree(f, ND_BODY(node), indent + add);
      break;
    case ANCR_LOOK_BEHIND:
      fprintf(f, "look behind\n");
      print_indent_tree(f, ND_BODY(node), indent + add);
      break;
    case ANCR_LOOK_BEHIND_NOT:
      fprintf(f, "look behind not\n");
      print_indent_tree(f, ND_BODY(node), indent + add);
      break;

    default:
      fprintf(f, "ERROR: undefined anchor type.\n");
      break;
    }
    break;

  case ND_BACKREF:
    {
      int* p;
      BackRefNode* br = BACKREF_(node);
      p = BACKREFS_P(br);
      fprintf(f, "<backref%s:%p>", ND_IS_CHECKER(node) ? "-checker" : "", node);
      for (i = 0; i < br->back_num; i++) {
        if (i > 0) fputs(", ", f);
        fprintf(f, "%d", p[i]);
      }
#ifdef USE_BACKREF_WITH_LEVEL
      if (ND_IS_NEST_LEVEL(node)) {
        fprintf(f, ", level: %d", br->nest_level);
      }
#endif
    }
    break;

#ifdef USE_CALL
  case ND_CALL:
    {
      CallNode* cn = CALL_(node);
      fprintf(f, "<call:%p>", node);
      fprintf(f, " num: %d, name", cn->called_gnum);
      p_string(f, cn->name_end - cn->name, cn->name);
      if (ND_IS_RECURSION(node)) fprintf(f, ", recursion");
      if (ND_IS_INPEEK(node))    fprintf(f, ", in-peek");
      if (ND_IS_IN_REAL_REPEAT(node)) fprintf(f, ", in-real-repeat");
    }
    break;
#endif

  case ND_QUANT:
    {
      fprintf(f, "<quantifier:%p>{%d,%d}%s%s%s", node,
              QUANT_(node)->lower, QUANT_(node)->upper,
              (QUANT_(node)->greedy ? "" : "?"),
              QUANT_(node)->include_referred == 0 ? "" : " referred",
              emptiness_name[QUANT_(node)->emptiness]);
      if (ND_IS_INPEEK(node)) fprintf(f, ", in-peek");
      fprintf(f, "\n");
      print_indent_tree(f, ND_BODY(node), indent + add);
    }
    break;

  case ND_BAG:
    {
      BagNode* bn = BAG_(node);
      fprintf(f, "<bag:%p> ", node);
      if (bn->type == BAG_IF_ELSE) {
        Node* Then;
        Node* Else;

        fprintf(f, "if-else\n");
        print_indent_tree(f, ND_BODY(node), indent + add);

        Then = bn->te.Then;
        Else = bn->te.Else;
        if (IS_NULL(Then)) {
          Indent(f, indent + add);
          fprintf(f, "THEN empty\n");
        }
        else
          print_indent_tree(f, Then, indent + add);

        if (IS_NULL(Else)) {
          Indent(f, indent + add);
          fprintf(f, "ELSE empty\n");
        }
        else
          print_indent_tree(f, Else, indent + add);
      }
      else {
        switch (bn->type) {
        case BAG_OPTION:
          fprintf(f, "option:%d", bn->o.options);
          break;
        case BAG_MEMORY:
          fprintf(f, "memory:%d", bn->m.regnum);
          if (ND_IS_CALLED(node)) {
            fprintf(f, ", called");
            if (ND_IS_RECURSION(node))
              fprintf(f, ", recursion");
          }
          else if (ND_IS_REFERENCED(node))
            fprintf(f, ", referenced");

          if (ND_IS_FIXED_ADDR(node))
            fprintf(f, ", fixed-addr");
          if ((bn->m.called_state & IN_PEEK) != 0)
            fprintf(f, ", in-peek");
          break;
        case BAG_STOP_BACKTRACK:
          fprintf(f, "stop-bt");
          break;
        default:
          break;
        }
        fprintf(f, "\n");
        print_indent_tree(f, ND_BODY(node), indent + add);
      }
    }
    break;

  case ND_GIMMICK:
    fprintf(f, "<gimmick:%p> ", node);
    switch (GIMMICK_(node)->type) {
    case GIMMICK_FAIL:
      fprintf(f, "fail");
      break;
    case GIMMICK_SAVE:
      fprintf(f, "save:%d:%d", GIMMICK_(node)->detail_type, GIMMICK_(node)->id);
      break;
    case GIMMICK_UPDATE_VAR:
      fprintf(f, "update_var:%d:%d", GIMMICK_(node)->detail_type, GIMMICK_(node)->id);
      break;
#ifdef USE_CALLOUT
    case GIMMICK_CALLOUT:
      switch (GIMMICK_(node)->detail_type) {
      case ONIG_CALLOUT_OF_CONTENTS:
        fprintf(f, "callout:contents:%d", GIMMICK_(node)->num);
        break;
      case ONIG_CALLOUT_OF_NAME:
        fprintf(f, "callout:name:%d:%d", GIMMICK_(node)->id, GIMMICK_(node)->num);
        break;
      }
#endif
    }
    break;

  default:
    fprintf(f, "print_indent_tree: undefined node type %d\n", ND_TYPE(node));
    break;
  }

  if (type != ND_LIST && type != ND_ALT && type != ND_QUANT &&
      type != ND_BAG)
    fprintf(f, "\n");
  fflush(f);
}

static void
print_tree(FILE* f, Node* node)
{
  print_indent_tree(f, node, 0);
}
#endif
