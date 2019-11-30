/**********************************************************************
  regcomp.c -  Oniguruma (regular expression library)
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

#include "regparse.h"

#define OPS_INIT_SIZE  8

OnigCaseFoldType OnigDefaultCaseFoldFlag = ONIGENC_CASE_FOLD_MIN;

#if 0
typedef struct {
  int  n;
  int  alloc;
  int* v;
} int_stack;

static int
make_int_stack(int_stack** rs, int init_size)
{
  int_stack* s;
  int* v;

  *rs = 0;

  s = xmalloc(sizeof(*s));
  if (IS_NULL(s)) return ONIGERR_MEMORY;

  v = (int* )xmalloc(sizeof(int) * init_size);
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
free_int_stack(int_stack* s)
{
  if (IS_NOT_NULL(s)) {
    if (IS_NOT_NULL(s->v))
      xfree(s->v);
    xfree(s);
  }
}

static int
int_stack_push(int_stack* s, int v)
{
  if (s->n >= s->alloc) {
    int new_size = s->alloc * 2;
    int* nv = (int* )xrealloc(s->v, sizeof(int) * new_size);
    if (IS_NULL(nv)) return ONIGERR_MEMORY;

    s->alloc = new_size;
    s->v = nv;
  }

  s->v[s->n] = v;
  s->n++;
  return ONIG_NORMAL;
}

static int
int_stack_pop(int_stack* s)
{
  int v;

#ifdef ONIG_DEBUG
  if (s->n <= 0) {
    fprintf(stderr, "int_stack_pop: fail empty. %p\n", s);
    return 0;
  }
#endif

  v = s->v[s->n];
  s->n--;
  return v;
}
#endif

static int
ops_init(regex_t* reg, int init_alloc_size)
{
  Operation* p;
  size_t size;

  if (init_alloc_size > 0) {
    size = sizeof(Operation) * init_alloc_size;
    p = (Operation* )xrealloc(reg->ops, size);
    CHECK_NULL_RETURN_MEMERR(p);
#ifdef USE_DIRECT_THREADED_CODE
    {
      enum OpCode* cp;
      size = sizeof(enum OpCode) * init_alloc_size;
      cp = (enum OpCode* )xrealloc(reg->ocs, size);
      CHECK_NULL_RETURN_MEMERR(cp);
      reg->ocs = cp;
    }
#endif
  }
  else {
    p  = (Operation* )0;
#ifdef USE_DIRECT_THREADED_CODE
    reg->ocs = (enum OpCode* )0;
#endif
  }

  reg->ops = p;
  reg->ops_curr  = 0; /* !!! not yet done ops_new() */
  reg->ops_alloc = init_alloc_size;
  reg->ops_used  = 0;

  return ONIG_NORMAL;
}

static int
ops_expand(regex_t* reg, int n)
{
#define MIN_OPS_EXPAND_SIZE   4

#ifdef USE_DIRECT_THREADED_CODE
  enum OpCode* cp;
#endif
  Operation* p;
  size_t size;

  if (n <= 0) n = MIN_OPS_EXPAND_SIZE;

  n += reg->ops_alloc;

  size = sizeof(Operation) * n;
  p = (Operation* )xrealloc(reg->ops, size);
  CHECK_NULL_RETURN_MEMERR(p);

#ifdef USE_DIRECT_THREADED_CODE
  size = sizeof(enum OpCode) * n;
  cp = (enum OpCode* )xrealloc(reg->ocs, size);
  CHECK_NULL_RETURN_MEMERR(cp);
  reg->ocs = cp;
#endif

  reg->ops = p;
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
  int r;

  if (reg->ops_used >= reg->ops_alloc) {
    r = ops_expand(reg, reg->ops_alloc);
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
    case OP_STR_N: case OP_STR_MB2N: case OP_STR_MB3N: case OP_STR_N_IC:
      if (! is_in_string_pool(reg, op->exact_n.s))
        xfree(op->exact_n.s);
      break;
    case OP_STR_1: case OP_STR_2: case OP_STR_3: case OP_STR_4:
    case OP_STR_5: case OP_STR_MB2N1: case OP_STR_MB2N2:
    case OP_STR_MB2N3: case OP_STR_1_IC:
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
    case OP_BACKREF_WITH_LEVEL:
    case OP_BACKREF_WITH_LEVEL_IC:
    case OP_BACKREF_CHECK:
    case OP_BACKREF_CHECK_WITH_LEVEL:
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
    case OP_STR_N_IC:
      total += op->exact_n.n;
      break;
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
    case OP_STR_N_IC:
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
int_multiply_cmp(int x, int y, int v)
{
  if (x == 0 || y == 0) return -1;

  if (x < INT_MAX / y) {
    int xy = x * y;
    if (xy > v) return 1;
    else {
      if (xy == v) return 0;
      else return -1;
    }
  }
  else
    return 1;
}

extern int
onig_positive_int_multiply(int x, int y)
{
  if (x == 0 || y == 0) return 0;

  if (x < INT_MAX / y)
    return x * y;
  else
    return -1;
}


static void
node_swap(Node* a, Node* b)
{
  Node c;

  c = *a; *a = *b; *b = c;

  if (NODE_TYPE(a) == NODE_STRING) {
    StrNode* sn = STR_(a);
    if (sn->capacity == 0) {
      int len = (int )(sn->end - sn->s);
      sn->s   = sn->buf;
      sn->end = sn->s + len;
    }
  }

  if (NODE_TYPE(b) == NODE_STRING) {
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
  while (IS_NOT_NULL(NODE_CDR(list))) {
    list = NODE_CDR(list);
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
    while (IS_NOT_NULL(NODE_CDR(list)))
      list = NODE_CDR(list);

    NODE_CDR(list) = n;
  }

  return n;
}

static int
node_str_node_cat(Node* node, Node* add)
{
  int r;

  if (STR_(node)->flag != STR_(add)->flag)
    return ONIGERR_TYPE_BUG;

  r = onig_node_str_cat(node, STR_(add)->s, STR_(add)->end);
  if (r != 0) return r;

  if (NODE_STRING_IS_CASE_FOLD_MATCH(node))
    STR_(node)->case_min_len += STR_(add)->case_min_len;

  return 0;
}

static int
node_str_cat_case_fold(Node* node, const UChar* s, const UChar* end, int case_min_len)
{
  int r;

  if (! NODE_STRING_IS_CASE_FOLD_MATCH(node))
    return ONIGERR_TYPE_BUG;

  r = onig_node_str_cat(node, s, end);
  if (r != 0) return r;

  STR_(node)->case_min_len += case_min_len;
  return 0;
}

static void
node_conv_to_str_node(Node* node, int flag)
{
  NODE_SET_TYPE(node, NODE_STRING);
  STR_(node)->flag     = flag;
  STR_(node)->s        = STR_(node)->buf;
  STR_(node)->end      = STR_(node)->buf;
  STR_(node)->capacity = 0;
  STR_(node)->case_min_len = 0;
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

  for (i = 0; i < (int )BITSET_SIZE; i++) {
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

static int compile_length_tree(Node* node, regex_t* reg);
static int compile_tree(Node* node, regex_t* reg, ScanEnv* env);


#define IS_NEED_STR_LEN_OP(op) \
   ((op) == OP_STR_N    || (op) == OP_STR_MB2N ||\
    (op) == OP_STR_MB3N || (op) == OP_STR_MBN  || (op) == OP_STR_N_IC)

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
  switch (NODE_TYPE(node)) {
  case NODE_STRING:
    {
      StrNode* sn = STR_(node);
      return (sn->end != sn->s);
    }
    break;

  case NODE_CCLASS:
  case NODE_CTYPE:
    return 1;
    break;

  default:
    return 0;
    break;
  }
}

static int
compile_quant_body_with_empty_check(QuantNode* qn, regex_t* reg, ScanEnv* env)
{
  int r;
  int saved_num_empty_check;
  int emptiness;
  Node* body;

  body = NODE_BODY((Node* )qn);
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
    if (emptiness == BODY_IS_EMPTY_POSSIBILITY)
      r = add_op(reg, OP_EMPTY_CHECK_END);
    else if (emptiness == BODY_IS_EMPTY_POSSIBILITY_MEM) {
      if (NODE_IS_EMPTY_STATUS_CHECK(qn) != 0)
        r = add_op(reg, OP_EMPTY_CHECK_END_MEMST);
      else
        r = add_op(reg, OP_EMPTY_CHECK_END);
    }
#ifdef USE_CALL
    else if (emptiness == BODY_IS_EMPTY_POSSIBILITY_REC)
      r = add_op(reg, OP_EMPTY_CHECK_END_MEMST_PUSH);
#endif

    if (r != 0) return r;
    COP(reg)->empty_check_end.mem = saved_num_empty_check; /* NULL CHECK ID */
  }
  return r;
}

#ifdef USE_CALL
static int
compile_call(CallNode* node, regex_t* reg, ScanEnv* env)
{
  int r;
  int offset;

  r = add_op(reg, OP_CALL);
  if (r != 0) return r;

  COP(reg)->call.addr = 0; /* dummy addr. */

  offset = COP_CURR_OFFSET_BYTES(reg, call.addr);
  r = unset_addr_list_add(env->unset_addr_list, offset, NODE_CALL_BODY(node));
  return r;
}
#endif

static int
compile_tree_n_times(Node* node, int n, regex_t* reg, ScanEnv* env)
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

    if (op == OP_STR_N_IC)
      COP(reg)->exact_n.n = byte_len;
    else
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

  if (NODE_STRING_IS_CASE_FOLD_MATCH(node) != 0) return 1;

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
compile_ambig_string_node(Node* node, regex_t* reg)
{
  int r;
  int len;
  int byte_len;
  UChar* p;
  StrNode* sn;
  OnigEncoding enc = reg->enc;

  sn = STR_(node);
  len = enclen(enc, sn->s);
  byte_len = (int )(sn->end - sn->s);
  if (len == byte_len) {
    r = add_op(reg, OP_STR_1_IC);
    if (r != 0) return r;

    xmemset(COP(reg)->exact.s, 0, sizeof(COP(reg)->exact.s));
    xmemcpy(COP(reg)->exact.s, sn->s, (size_t )byte_len);
  }
  else {
    r = add_op(reg, OP_STR_N_IC);
    if (r != 0) return r;

    p = onigenc_strdup(enc, sn->s, sn->end);
    CHECK_NULL_RETURN_MEMERR(p);

    COP(reg)->exact_n.s = p;
    COP(reg)->exact_n.n = byte_len;
  }

  return 0;
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
  if (NODE_STRING_IS_CASE_FOLD_MATCH(node) != 0) {
    return compile_ambig_string_node(node, reg);
  }

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
                          regex_t* reg, ScanEnv* env)
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
      NODE_IS_ANYCHAR(NODE_QUANT_BODY(qn)))
    return 1;
  else
    return 0;
}

#define QUANTIFIER_EXPAND_LIMIT_SIZE   10
#define CKN_ON   (ckn > 0)

static int
compile_length_quantifier_node(QuantNode* qn, regex_t* reg)
{
  int len, mod_tlen;
  int infinite = IS_INFINITE_REPEAT(qn->upper);
  enum BodyEmptyType emptiness = qn->emptiness;
  int tlen = compile_length_tree(NODE_QUANT_BODY(qn), reg);

  if (tlen < 0) return tlen;
  if (tlen == 0) return 0;

  /* anychar repeat */
  if (is_anychar_infinite_greedy(qn)) {
    if (qn->lower <= 1 ||
        int_multiply_cmp(tlen, qn->lower, QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0) {
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
       int_multiply_cmp(tlen, qn->lower, QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
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
            int_multiply_cmp(tlen + OPSIZE_PUSH, qn->upper,
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
compile_quantifier_node(QuantNode* qn, regex_t* reg, ScanEnv* env)
{
  int i, r, mod_tlen;
  int infinite = IS_INFINITE_REPEAT(qn->upper);
  enum BodyEmptyType emptiness = qn->emptiness;
  int tlen = compile_length_tree(NODE_QUANT_BODY(qn), reg);

  if (tlen < 0) return tlen;
  if (tlen == 0) return 0;

  if (is_anychar_infinite_greedy(qn) &&
      (qn->lower <= 1 ||
       int_multiply_cmp(tlen, qn->lower, QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
    if (r != 0) return r;
    if (IS_NOT_NULL(qn->next_head_exact)) {
      r = add_op(reg,
                 IS_MULTILINE(CTYPE_OPTION(NODE_QUANT_BODY(qn), reg)) ?
                 OP_ANYCHAR_ML_STAR_PEEK_NEXT : OP_ANYCHAR_STAR_PEEK_NEXT);
      if (r != 0) return r;

      COP(reg)->anychar_star_peek_next.c = STR_(qn->next_head_exact)->s[0];
      return 0;
    }
    else {
      r = add_op(reg,
                 IS_MULTILINE(CTYPE_OPTION(NODE_QUANT_BODY(qn), reg)) ?
                 OP_ANYCHAR_ML_STAR : OP_ANYCHAR_STAR);
      return r;
    }
  }

  mod_tlen = tlen;
  if (emptiness != BODY_IS_NOT_EMPTY)
    mod_tlen += OPSIZE_EMPTY_CHECK_START + OPSIZE_EMPTY_CHECK_END;

  if (infinite &&
      (qn->lower <= 1 ||
       int_multiply_cmp(tlen, qn->lower, QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
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
      r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
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

      r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
    }
    else {
      /* Nothing output */
      r = 0;
    }
  }
  else if (! infinite && qn->greedy &&
           (qn->upper == 1 ||
            int_multiply_cmp(tlen + OPSIZE_PUSH, qn->upper,
                             QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    int n = qn->upper - qn->lower;

    r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
    if (r != 0) return r;

    for (i = 0; i < n; i++) {
      int v = onig_positive_int_multiply(n - i, tlen + OPSIZE_PUSH);
      if (v < 0) return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = v;

      r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
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

    r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
  }
  else {
    r = compile_range_repeat_node(qn, mod_tlen, emptiness, reg, env);
  }
  return r;
}

static int
compile_length_option_node(BagNode* node, regex_t* reg)
{
  int tlen;
  OnigOptionType prev = reg->options;

  reg->options = node->o.options;
  tlen = compile_length_tree(NODE_BAG_BODY(node), reg);
  reg->options = prev;

  return tlen;
}

static int
compile_option_node(BagNode* node, regex_t* reg, ScanEnv* env)
{
  int r;
  OnigOptionType prev = reg->options;

  reg->options = node->o.options;
  r = compile_tree(NODE_BAG_BODY(node), reg, env);
  reg->options = prev;

  return r;
}

static int
compile_length_bag_node(BagNode* node, regex_t* reg)
{
  int len;
  int tlen;

  if (node->type == BAG_OPTION)
    return compile_length_option_node(node, reg);

  if (NODE_BAG_BODY(node)) {
    tlen = compile_length_tree(NODE_BAG_BODY(node), reg);
    if (tlen < 0) return tlen;
  }
  else
    tlen = 0;

  switch (node->type) {
  case BAG_MEMORY:
#ifdef USE_CALL

    if (node->m.regnum == 0 && NODE_IS_CALLED(node)) {
      len = tlen + OPSIZE_CALL + OPSIZE_JUMP + OPSIZE_RETURN;
      return len;
    }

    if (NODE_IS_CALLED(node)) {
      len = OPSIZE_MEM_START_PUSH + tlen
        + OPSIZE_CALL + OPSIZE_JUMP + OPSIZE_RETURN;
      if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
        len += (NODE_IS_RECURSION(node)
                ? OPSIZE_MEM_END_PUSH_REC : OPSIZE_MEM_END_PUSH);
      else
        len += (NODE_IS_RECURSION(node)
                ? OPSIZE_MEM_END_REC : OPSIZE_MEM_END);
    }
    else if (NODE_IS_RECURSION(node)) {
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
    if (NODE_IS_STRICT_REAL_REPEAT(node)) {
      int v;
      QuantNode* qn;

      qn = QUANT_(NODE_BAG_BODY(node));
      tlen = compile_length_tree(NODE_QUANT_BODY(qn), reg);
      if (tlen < 0) return tlen;

      v = onig_positive_int_multiply(qn->lower, tlen);
      if (v < 0) return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;
      len = v + OPSIZE_PUSH + tlen + OPSIZE_POP_OUT + OPSIZE_JUMP;
    }
    else {
      len = OPSIZE_ATOMIC_START + tlen + OPSIZE_ATOMIC_END;
    }
    break;

  case BAG_IF_ELSE:
    {
      Node* cond = NODE_BAG_BODY(node);
      Node* Then = node->te.Then;
      Node* Else = node->te.Else;

      len = compile_length_tree(cond, reg);
      if (len < 0) return len;
      len += OPSIZE_PUSH;
      len += OPSIZE_ATOMIC_START + OPSIZE_ATOMIC_END;

      if (IS_NOT_NULL(Then)) {
        tlen = compile_length_tree(Then, reg);
        if (tlen < 0) return tlen;
        len += tlen;
      }

      len += OPSIZE_JUMP + OPSIZE_ATOMIC_END;

      if (IS_NOT_NULL(Else)) {
        tlen = compile_length_tree(Else, reg);
        if (tlen < 0) return tlen;
        len += tlen;
      }
    }
    break;

  case BAG_OPTION:
    /* never come here, but set for escape warning */
    len = 0;
    break;
  }

  return len;
}

static int get_char_len_node(Node* node, regex_t* reg, int* len);

static int
compile_bag_memory_node(BagNode* node, regex_t* reg, ScanEnv* env)
{
  int r;

#ifdef USE_CALL
  if (NODE_IS_CALLED(node)) {
    int len;

    r = add_op(reg, OP_CALL);
    if (r != 0) return r;

    node->m.called_addr = COP_CURR_OFFSET(reg) + 1 + OPSIZE_JUMP;
    NODE_STATUS_ADD(node, ADDR_FIXED);
    COP(reg)->call.addr = (int )node->m.called_addr;

    if (node->m.regnum == 0) {
      len = compile_length_tree(NODE_BAG_BODY(node), reg);
      len += OPSIZE_RETURN;

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = len + SIZE_INC;

      r = compile_tree(NODE_BAG_BODY(node), reg, env);
      if (r != 0) return r;

      r = add_op(reg, OP_RETURN);
      return r;
    }
    else {
      len = compile_length_tree(NODE_BAG_BODY(node), reg);
      len += (OPSIZE_MEM_START_PUSH + OPSIZE_RETURN);
      if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
        len += (NODE_IS_RECURSION(node)
                ? OPSIZE_MEM_END_PUSH_REC : OPSIZE_MEM_END_PUSH);
      else
        len += (NODE_IS_RECURSION(node) ? OPSIZE_MEM_END_REC : OPSIZE_MEM_END);

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

  r = compile_tree(NODE_BAG_BODY(node), reg, env);
  if (r != 0) return r;

#ifdef USE_CALL
  if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
    r = add_op(reg, (NODE_IS_RECURSION(node)
                     ? OP_MEM_END_PUSH_REC : OP_MEM_END_PUSH));
  else
    r = add_op(reg, (NODE_IS_RECURSION(node) ? OP_MEM_END_REC : OP_MEM_END));
  if (r != 0) return r;
  COP(reg)->memory_end.num = node->m.regnum;

  if (NODE_IS_CALLED(node)) {
    if (r != 0) return r;
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
compile_bag_node(BagNode* node, regex_t* reg, ScanEnv* env)
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
    if (NODE_IS_STRICT_REAL_REPEAT(node)) {
      QuantNode* qn = QUANT_(NODE_BAG_BODY(node));
      r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
      if (r != 0) return r;

      len = compile_length_tree(NODE_QUANT_BODY(qn), reg);
      if (len < 0) return len;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = SIZE_INC + len + OPSIZE_POP_OUT + OPSIZE_JUMP;

      r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
      if (r != 0) return r;
      r = add_op(reg, OP_POP_OUT);
      if (r != 0) return r;

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = -((int )OPSIZE_PUSH + len + (int )OPSIZE_POP_OUT);
    }
    else {
      r = add_op(reg, OP_ATOMIC_START);
      if (r != 0) return r;
      r = compile_tree(NODE_BAG_BODY(node), reg, env);
      if (r != 0) return r;
      r = add_op(reg, OP_ATOMIC_END);
    }
    break;

  case BAG_IF_ELSE:
    {
      int cond_len, then_len, else_len, jump_len;
      Node* cond = NODE_BAG_BODY(node);
      Node* Then = node->te.Then;
      Node* Else = node->te.Else;

      r = add_op(reg, OP_ATOMIC_START);
      if (r != 0) return r;

      cond_len = compile_length_tree(cond, reg);
      if (cond_len < 0) return cond_len;
      if (IS_NOT_NULL(Then)) {
        then_len = compile_length_tree(Then, reg);
        if (then_len < 0) return then_len;
      }
      else
        then_len = 0;

      jump_len = cond_len + then_len + OPSIZE_ATOMIC_END + OPSIZE_JUMP;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = SIZE_INC + jump_len;

      r = compile_tree(cond, reg, env);
      if (r != 0) return r;
      r = add_op(reg, OP_ATOMIC_END);
      if (r != 0) return r;

      if (IS_NOT_NULL(Then)) {
        r = compile_tree(Then, reg, env);
        if (r != 0) return r;
      }

      if (IS_NOT_NULL(Else)) {
        else_len = compile_length_tree(Else, reg);
        if (else_len < 0) return else_len;
      }
      else
        else_len = 0;

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = OPSIZE_ATOMIC_END + else_len + SIZE_INC;

      r = add_op(reg, OP_ATOMIC_END);
      if (r != 0) return r;

      if (IS_NOT_NULL(Else)) {
        r = compile_tree(Else, reg, env);
      }
    }
    break;
  }

  return r;
}

static int
compile_length_anchor_node(AnchorNode* node, regex_t* reg)
{
  int len;
  int tlen = 0;

  if (IS_NOT_NULL(NODE_ANCHOR_BODY(node))) {
    tlen = compile_length_tree(NODE_ANCHOR_BODY(node), reg);
    if (tlen < 0) return tlen;
  }

  switch (node->type) {
  case ANCR_PREC_READ:
    len = OPSIZE_PREC_READ_START + tlen + OPSIZE_PREC_READ_END;
    break;
  case ANCR_PREC_READ_NOT:
    len = OPSIZE_PREC_READ_NOT_START + tlen + OPSIZE_PREC_READ_NOT_END;
    break;
  case ANCR_LOOK_BEHIND:
    len = OPSIZE_LOOK_BEHIND + tlen;
    break;
  case ANCR_LOOK_BEHIND_NOT:
    len = OPSIZE_LOOK_BEHIND_NOT_START + tlen + OPSIZE_LOOK_BEHIND_NOT_END;
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
compile_anchor_node(AnchorNode* node, regex_t* reg, ScanEnv* env)
{
  int r, len;
  enum OpCode op;

  switch (node->type) {
  case ANCR_BEGIN_BUF:      r = add_op(reg, OP_BEGIN_BUF);      break;
  case ANCR_END_BUF:        r = add_op(reg, OP_END_BUF);        break;
  case ANCR_BEGIN_LINE:     r = add_op(reg, OP_BEGIN_LINE);     break;
  case ANCR_END_LINE:       r = add_op(reg, OP_END_LINE);       break;
  case ANCR_SEMI_END_BUF:   r = add_op(reg, OP_SEMI_END_BUF);   break;
  case ANCR_BEGIN_POSITION: r = add_op(reg, OP_BEGIN_POSITION); break;

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
      if (ONIG_IS_OPTION_ON(reg->options, ONIG_OPTION_TEXT_SEGMENT_WORD))
        type = WORD_BOUNDARY;
#endif

      COP(reg)->text_segment_boundary.type = type;
      COP(reg)->text_segment_boundary.not =
        (node->type == ANCR_NO_TEXT_SEGMENT_BOUNDARY ? 1 : 0);
    }
    break;

  case ANCR_PREC_READ:
    r = add_op(reg, OP_PREC_READ_START);
    if (r != 0) return r;
    r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
    if (r != 0) return r;
    r = add_op(reg, OP_PREC_READ_END);
    break;

  case ANCR_PREC_READ_NOT:
    len = compile_length_tree(NODE_ANCHOR_BODY(node), reg);
    if (len < 0) return len;

    r = add_op(reg, OP_PREC_READ_NOT_START);
    if (r != 0) return r;
    COP(reg)->prec_read_not_start.addr = SIZE_INC + len + OPSIZE_PREC_READ_NOT_END;
    r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
    if (r != 0) return r;
    r = add_op(reg, OP_PREC_READ_NOT_END);
    break;

  case ANCR_LOOK_BEHIND:
    {
      int n;
      r = add_op(reg, OP_LOOK_BEHIND);
      if (r != 0) return r;
      if (node->char_len < 0) {
        r = get_char_len_node(NODE_ANCHOR_BODY(node), reg, &n);
        if (r != 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
      }
      else
        n = node->char_len;

      COP(reg)->look_behind.len = n;
      r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
    }
    break;

  case ANCR_LOOK_BEHIND_NOT:
    {
      int n;

      len = compile_length_tree(NODE_ANCHOR_BODY(node), reg);
      r = add_op(reg, OP_LOOK_BEHIND_NOT_START);
      if (r != 0) return r;
      COP(reg)->look_behind_not_start.addr = SIZE_INC + len + OPSIZE_LOOK_BEHIND_NOT_END;

      if (node->char_len < 0) {
        r = get_char_len_node(NODE_ANCHOR_BODY(node), reg, &n);
        if (r != 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
      }
      else
        n = node->char_len;

      COP(reg)->look_behind_not_start.len = n;

      r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
      if (r != 0) return r;
      r = add_op(reg, OP_LOOK_BEHIND_NOT_END);
    }
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
  int r;

  switch (node->type) {
  case GIMMICK_FAIL:
    r = add_op(reg, OP_FAIL);
    break;

  case GIMMICK_SAVE:
    r = add_op(reg, OP_PUSH_SAVE_VAL);
    if (r != 0) return r;
    COP(reg)->push_save_val.type = node->detail_type;
    COP(reg)->push_save_val.id   = node->id;
    break;

  case GIMMICK_UPDATE_VAR:
    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = node->detail_type;
    COP(reg)->update_var.id   = node->id;
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
    len = OPSIZE_PUSH_SAVE_VAL;
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
  }

  return len;
}

static int
compile_length_tree(Node* node, regex_t* reg)
{
  int len, r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    len = 0;
    do {
      r = compile_length_tree(NODE_CAR(node), reg);
      if (r < 0) return r;
      len += r;
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    r = len;
    break;

  case NODE_ALT:
    {
      int n;

      n = r = 0;
      do {
        r += compile_length_tree(NODE_CAR(node), reg);
        n++;
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
      r += (OPSIZE_PUSH + OPSIZE_JUMP) * (n - 1);
    }
    break;

  case NODE_STRING:
    if (NODE_STRING_IS_CRUDE(node))
      r = compile_length_string_crude_node(STR_(node), reg);
    else
      r = compile_length_string_node(node, reg);
    break;

  case NODE_CCLASS:
    r = compile_length_cclass_node(CCLASS_(node), reg);
    break;

  case NODE_CTYPE:
    r = SIZE_OPCODE;
    break;

  case NODE_BACKREF:
    r = OPSIZE_BACKREF;
    break;

#ifdef USE_CALL
  case NODE_CALL:
    r = OPSIZE_CALL;
    break;
#endif

  case NODE_QUANT:
    r = compile_length_quantifier_node(QUANT_(node), reg);
    break;

  case NODE_BAG:
    r = compile_length_bag_node(BAG_(node), reg);
    break;

  case NODE_ANCHOR:
    r = compile_length_anchor_node(ANCHOR_(node), reg);
    break;

  case NODE_GIMMICK:
    r = compile_length_gimmick_node(GIMMICK_(node), reg);
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return r;
}

static int
compile_tree(Node* node, regex_t* reg, ScanEnv* env)
{
  int n, len, pos, r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    do {
      r = compile_tree(NODE_CAR(node), reg, env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ALT:
    {
      Node* x = node;
      len = 0;
      do {
        len += compile_length_tree(NODE_CAR(x), reg);
        if (IS_NOT_NULL(NODE_CDR(x))) {
          len += OPSIZE_PUSH + OPSIZE_JUMP;
        }
      } while (IS_NOT_NULL(x = NODE_CDR(x)));
      pos = COP_CURR_OFFSET(reg) + 1 + len;  /* goal position */

      do {
        len = compile_length_tree(NODE_CAR(node), reg);
        if (IS_NOT_NULL(NODE_CDR(node))) {
          enum OpCode push = NODE_IS_SUPER(node) ? OP_PUSH_SUPER : OP_PUSH;
          r = add_op(reg, push);
          if (r != 0) break;
          COP(reg)->push.addr = SIZE_INC + len + OPSIZE_JUMP;
        }
        r = compile_tree(NODE_CAR(node), reg, env);
        if (r != 0) break;
        if (IS_NOT_NULL(NODE_CDR(node))) {
          len = pos - (COP_CURR_OFFSET(reg) + 1);
          r = add_op(reg, OP_JUMP);
          if (r != 0) break;
          COP(reg)->jump.addr = len;
        }
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

  case NODE_STRING:
    if (NODE_STRING_IS_CRUDE(node))
      r = compile_string_crude_node(STR_(node), reg);
    else
      r = compile_string_node(node, reg);
    break;

  case NODE_CCLASS:
    r = compile_cclass_node(CCLASS_(node), reg);
    break;

  case NODE_CTYPE:
    {
      int op;

      switch (CTYPE_(node)->ctype) {
      case CTYPE_ANYCHAR:
        r = add_op(reg, IS_MULTILINE(CTYPE_OPTION(node, reg)) ?
                   OP_ANYCHAR_ML : OP_ANYCHAR);
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

  case NODE_BACKREF:
    {
      BackRefNode* br = BACKREF_(node);

      if (NODE_IS_CHECKER(node)) {
#ifdef USE_BACKREF_WITH_LEVEL
        if (NODE_IS_NEST_LEVEL(node)) {
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
        if (NODE_IS_NEST_LEVEL(node)) {
          if ((reg->options & ONIG_OPTION_IGNORECASE) != 0)
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
          if (IS_IGNORECASE(reg->options)) {
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

          r = add_op(reg, IS_IGNORECASE(reg->options) ?
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
  case NODE_CALL:
    r = compile_call(CALL_(node), reg, env);
    break;
#endif

  case NODE_QUANT:
    r = compile_quantifier_node(QUANT_(node), reg, env);
    break;

  case NODE_BAG:
    r = compile_bag_node(BAG_(node), reg, env);
    break;

  case NODE_ANCHOR:
    r = compile_anchor_node(ANCHOR_(node), reg, env);
    break;

  case NODE_GIMMICK:
    r = compile_gimmick_node(GIMMICK_(node), reg);
    break;

  default:
#ifdef ONIG_DEBUG
    fprintf(stderr, "compile_tree: undefined node type %d\n", NODE_TYPE(node));
#endif
    break;
  }

  return r;
}

static int
noname_disable_map(Node** plink, GroupNumRemap* map, int* counter)
{
  int r = 0;
  Node* node = *plink;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = noname_disable_map(&(NODE_CAR(node)), map, counter);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    {
      Node** ptarget = &(NODE_BODY(node));
      Node*  old = *ptarget;
      r = noname_disable_map(ptarget, map, counter);
      if (r != 0) return r;
      if (*ptarget != old && NODE_TYPE(*ptarget) == NODE_QUANT) {
        r = onig_reduce_nested_quantifier(node);
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      if (en->type == BAG_MEMORY) {
        if (NODE_IS_NAMED_GROUP(node)) {
          (*counter)++;
          map[en->m.regnum].new_val = *counter;
          en->m.regnum = *counter;
          r = noname_disable_map(&(NODE_BODY(node)), map, counter);
        }
        else {
          *plink = NODE_BODY(node);
          NODE_BODY(node) = NULL_NODE;
          onig_node_free(node);
          r = noname_disable_map(plink, map, counter);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        r = noname_disable_map(&(NODE_BAG_BODY(en)), map, counter);
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = noname_disable_map(&(en->te.Then), map, counter);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = noname_disable_map(&(en->te.Else), map, counter);
          if (r != 0) return r;
        }
      }
      else
        r = noname_disable_map(&(NODE_BODY(node)), map, counter);
    }
    break;

  case NODE_ANCHOR:
    if (IS_NOT_NULL(NODE_BODY(node)))
      r = noname_disable_map(&(NODE_BODY(node)), map, counter);
    break;

  default:
    break;
  }

  return r;
}

static int
renumber_node_backref(Node* node, GroupNumRemap* map)
{
  int i, pos, n, old_num;
  int *backs;
  BackRefNode* bn = BACKREF_(node);

  if (! NODE_IS_BY_NAME(node))
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
renumber_by_map(Node* node, GroupNumRemap* map)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = renumber_by_map(NODE_CAR(node), map);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    r = renumber_by_map(NODE_BODY(node), map);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      r = renumber_by_map(NODE_BODY(node), map);
      if (r != 0) return r;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = renumber_by_map(en->te.Then, map);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = renumber_by_map(en->te.Else, map);
          if (r != 0) return r;
        }
      }
    }
    break;

  case NODE_BACKREF:
    r = renumber_node_backref(node, map);
    break;

  case NODE_ANCHOR:
    if (IS_NOT_NULL(NODE_BODY(node)))
      r = renumber_by_map(NODE_BODY(node), map);
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

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = numbered_ref_check(NODE_CAR(node));
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (IS_NULL(NODE_BODY(node)))
      break;
    /* fall */
  case NODE_QUANT:
    r = numbered_ref_check(NODE_BODY(node));
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      r = numbered_ref_check(NODE_BODY(node));
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

  case NODE_BACKREF:
    if (! NODE_IS_BY_NAME(node))
      return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;
    break;

  default:
    break;
  }

  return r;
}

static int
disable_noname_group_capture(Node** root, regex_t* reg, ScanEnv* env)
{
  int r, i, pos, counter;
  MemStatusType loc;
  GroupNumRemap* map;

  map = (GroupNumRemap* )xalloca(sizeof(GroupNumRemap) * (env->num_mem + 1));
  CHECK_NULL_RETURN_MEMERR(map);
  for (i = 1; i <= env->num_mem; i++) {
    map[i].new_val = 0;
  }
  counter = 0;
  r = noname_disable_map(root, map, &counter);
  if (r != 0) return r;

  r = renumber_by_map(*root, map);
  if (r != 0) return r;

  for (i = 1, pos = 1; i <= env->num_mem; i++) {
    if (map[i].new_val > 0) {
      SCANENV_MEMENV(env)[pos] = SCANENV_MEMENV(env)[i];
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
    if (! NODE_IS_ADDR_FIXED(uslist->us[i].target))
      return ONIGERR_PARSER_BUG;

    en = BAG_(uslist->us[i].target);
    addr   = en->m.called_addr;
    offset = uslist->us[i].offset;

    paddr = (AbsAddrType* )((char* )reg->ops + offset);
    *paddr = addr;
  }
  return 0;
}
#endif


#define GET_CHAR_LEN_VARLEN           -1
#define GET_CHAR_LEN_TOP_ALT_VARLEN   -2

/* fixed size pattern node only */
static int
get_char_len_node1(Node* node, regex_t* reg, int* len, int level)
{
  int tlen;
  int r = 0;

  level++;
  *len = 0;
  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    do {
      r = get_char_len_node1(NODE_CAR(node), reg, &tlen, level);
      if (r == 0)
        *len = distance_add(*len, tlen);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ALT:
    {
      int tlen2;
      int varlen = 0;

      r = get_char_len_node1(NODE_CAR(node), reg, &tlen, level);
      while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node))) {
        r = get_char_len_node1(NODE_CAR(node), reg, &tlen2, level);
        if (r == 0) {
          if (tlen != tlen2)
            varlen = 1;
        }
      }
      if (r == 0) {
        if (varlen != 0) {
          if (level == 1)
            r = GET_CHAR_LEN_TOP_ALT_VARLEN;
          else
            r = GET_CHAR_LEN_VARLEN;
        }
        else
          *len = tlen;
      }
    }
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);
      UChar *s = sn->s;

      while (s < sn->end) {
        s += enclen(reg->enc, s);
        (*len)++;
      }
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->lower == qn->upper) {
        if (qn->upper == 0) {
          *len = 0;
        }
        else {
          r = get_char_len_node1(NODE_BODY(node), reg, &tlen, level);
          if (r == 0)
            *len = distance_multiply(tlen, qn->lower);
        }
      }
      else
        r = GET_CHAR_LEN_VARLEN;
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    if (! NODE_IS_RECURSION(node))
      r = get_char_len_node1(NODE_BODY(node), reg, len, level);
    else
      r = GET_CHAR_LEN_VARLEN;
    break;
#endif

  case NODE_CTYPE:
  case NODE_CCLASS:
    *len = 1;
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      switch (en->type) {
      case BAG_MEMORY:
#ifdef USE_CALL
        if (NODE_IS_CLEN_FIXED(node))
          *len = en->char_len;
        else {
          r = get_char_len_node1(NODE_BODY(node), reg, len, level);
          if (r == 0) {
            en->char_len = *len;
            NODE_STATUS_ADD(node, CLEN_FIXED);
          }
        }
        break;
#endif
      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        r = get_char_len_node1(NODE_BODY(node), reg, len, level);
        break;
      case BAG_IF_ELSE:
        {
          int clen, elen;

          r = get_char_len_node1(NODE_BODY(node), reg, &clen, level);
          if (r == 0) {
            if (IS_NOT_NULL(en->te.Then)) {
              r = get_char_len_node1(en->te.Then, reg, &tlen, level);
              if (r != 0) break;
            }
            else tlen = 0;
            if (IS_NOT_NULL(en->te.Else)) {
              r = get_char_len_node1(en->te.Else, reg, &elen, level);
              if (r != 0) break;
            }
            else elen = 0;

            if (clen + tlen != elen) {
              r = GET_CHAR_LEN_VARLEN;
            }
            else {
              *len = elen;
            }
          }
        }
        break;
      }
    }
    break;

  case NODE_ANCHOR:
  case NODE_GIMMICK:
    break;

  case NODE_BACKREF:
    if (NODE_IS_CHECKER(node))
      break;
    /* fall */
  default:
    r = GET_CHAR_LEN_VARLEN;
    break;
  }

  return r;
}

static int
get_char_len_node(Node* node, regex_t* reg, int* len)
{
  return get_char_len_node1(node, reg, len, 0);
}

/* x is not included y ==>  1 : 0 */
static int
is_exclusive(Node* x, Node* y, regex_t* reg)
{
  int i, len;
  OnigCodePoint code;
  UChar *p;
  NodeType ytype;

 retry:
  ytype = NODE_TYPE(y);
  switch (NODE_TYPE(x)) {
  case NODE_CTYPE:
    {
      if (CTYPE_(x)->ctype == CTYPE_ANYCHAR ||
          CTYPE_(y)->ctype == CTYPE_ANYCHAR)
        break;

      switch (ytype) {
      case NODE_CTYPE:
        if (CTYPE_(y)->ctype == CTYPE_(x)->ctype &&
            CTYPE_(y)->not   != CTYPE_(x)->not &&
            CTYPE_(y)->ascii_mode == CTYPE_(x)->ascii_mode)
          return 1;
        else
          return 0;
        break;

      case NODE_CCLASS:
      swap:
        {
          Node* tmp;
          tmp = x; x = y; y = tmp;
          goto retry;
        }
        break;

      case NODE_STRING:
        goto swap;
        break;

      default:
        break;
      }
    }
    break;

  case NODE_CCLASS:
    {
      int range;
      CClassNode* xc = CCLASS_(x);

      switch (ytype) {
      case NODE_CTYPE:
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

      case NODE_CCLASS:
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

      case NODE_STRING:
        goto swap;
        break;

      default:
        break;
      }
    }
    break;

  case NODE_STRING:
    {
      StrNode* xs = STR_(x);

      if (NODE_STRING_LEN(x) == 0)
        break;

      switch (ytype) {
      case NODE_CTYPE:
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

      case NODE_CCLASS:
        {
          CClassNode* cc = CCLASS_(y);

          code = ONIGENC_MBC_TO_CODE(reg->enc, xs->s,
                                     xs->s + ONIGENC_MBC_MAXLEN(reg->enc));
          return onig_is_code_in_cc(reg->enc, code, cc) == 0;
        }
        break;

      case NODE_STRING:
        {
          UChar *q;
          StrNode* ys = STR_(y);

          len = NODE_STRING_LEN(x);
          if (len > NODE_STRING_LEN(y)) len = NODE_STRING_LEN(y);
          if (NODE_STRING_IS_CASE_FOLD_MATCH(x) || NODE_STRING_IS_CASE_FOLD_MATCH(y)) {
            /* tiny version */
            return 0;
          }
          else {
            for (i = 0, p = ys->s, q = xs->s; i < len; i++, p++, q++) {
              if (*p != *q) return 1;
            }
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
get_head_value_node(Node* node, int exact, regex_t* reg)
{
  Node* n = NULL_NODE;

  switch (NODE_TYPE(node)) {
  case NODE_BACKREF:
  case NODE_ALT:
#ifdef USE_CALL
  case NODE_CALL:
#endif
    break;

  case NODE_CTYPE:
    if (CTYPE_(node)->ctype == CTYPE_ANYCHAR)
      break;
    /* fall */
  case NODE_CCLASS:
    if (exact == 0) {
      n = node;
    }
    break;

  case NODE_LIST:
    n = get_head_value_node(NODE_CAR(node), exact, reg);
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);

      if (sn->end <= sn->s)
        break;

      if (exact == 0 ||
          ! IS_IGNORECASE(reg->options) || NODE_STRING_IS_CRUDE(node)) {
        n = node;
      }
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);
      if (qn->lower > 0) {
        if (IS_NOT_NULL(qn->head_exact))
          n = qn->head_exact;
        else
          n = get_head_value_node(NODE_BODY(node), exact, reg);
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_OPTION:
        {
          OnigOptionType options = reg->options;

          reg->options = BAG_(node)->o.options;
          n = get_head_value_node(NODE_BODY(node), exact, reg);
          reg->options = options;
        }
        break;

      case BAG_MEMORY:
      case BAG_STOP_BACKTRACK:
      case BAG_IF_ELSE:
        n = get_head_value_node(NODE_BODY(node), exact, reg);
        break;
      }
    }
    break;

  case NODE_ANCHOR:
    if (ANCHOR_(node)->type == ANCR_PREC_READ)
      n = get_head_value_node(NODE_BODY(node), exact, reg);
    break;

  case NODE_GIMMICK:
  default:
    break;
  }

  return n;
}

static int
check_type_tree(Node* node, int type_mask, int bag_mask, int anchor_mask)
{
  NodeType type;
  int r = 0;

  type = NODE_TYPE(node);
  if ((NODE_TYPE2BIT(type) & type_mask) == 0)
    return 1;

  switch (type) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = check_type_tree(NODE_CAR(node), type_mask, bag_mask, anchor_mask);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    r = check_type_tree(NODE_BODY(node), type_mask, bag_mask, anchor_mask);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      if (((1<<en->type) & bag_mask) == 0)
        return 1;

      r = check_type_tree(NODE_BODY(node), type_mask, bag_mask, anchor_mask);
      if (r == 0 && en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = check_type_tree(en->te.Then, type_mask, bag_mask, anchor_mask);
          if (r != 0) break;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = check_type_tree(en->te.Else, type_mask, bag_mask, anchor_mask);
        }
      }
    }
    break;

  case NODE_ANCHOR:
    type = ANCHOR_(node)->type;
    if ((type & anchor_mask) == 0)
      return 1;

    if (IS_NOT_NULL(NODE_BODY(node)))
      r = check_type_tree(NODE_BODY(node), type_mask, bag_mask, anchor_mask);
    break;

  case NODE_GIMMICK:
  default:
    break;
  }
  return r;
}

static OnigLen
tree_min_len(Node* node, ScanEnv* env)
{
  OnigLen len;
  OnigLen tmin;

  len = 0;
  switch (NODE_TYPE(node)) {
  case NODE_BACKREF:
    if (! NODE_IS_CHECKER(node)) {
      int i;
      int* backs;
      MemEnv* mem_env = SCANENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);
      if (NODE_IS_RECURSION(node)) break;

      backs = BACKREFS_P(br);
      len = tree_min_len(mem_env[backs[0]].mem_node, env);
      for (i = 1; i < br->back_num; i++) {
        tmin = tree_min_len(mem_env[backs[i]].mem_node, env);
        if (len > tmin) len = tmin;
      }
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    {
      Node* t = NODE_BODY(node);
      if (NODE_IS_RECURSION(node)) {
        if (NODE_IS_MIN_FIXED(t))
          len = BAG_(t)->min_len;
      }
      else
        len = tree_min_len(t, env);
    }
    break;
#endif

  case NODE_LIST:
    do {
      tmin = tree_min_len(NODE_CAR(node), env);
      len = distance_add(len, tmin);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ALT:
    {
      Node *x, *y;
      y = node;
      do {
        x = NODE_CAR(y);
        tmin = tree_min_len(x, env);
        if (y == node) len = tmin;
        else if (len > tmin) len = tmin;
      } while (IS_NOT_NULL(y = NODE_CDR(y)));
    }
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);
      len = (int )(sn->end - sn->s);
    }
    break;

  case NODE_CTYPE:
  case NODE_CCLASS:
    len = ONIGENC_MBC_MINLEN(env->enc);
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->lower > 0) {
        len = tree_min_len(NODE_BODY(node), env);
        len = distance_multiply(len, qn->lower);
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_MEMORY:
        if (NODE_IS_MIN_FIXED(node))
          len = en->min_len;
        else {
          if (NODE_IS_MARK1(node))
            len = 0;  /* recursive */
          else {
            NODE_STATUS_ADD(node, MARK1);
            len = tree_min_len(NODE_BODY(node), env);
            NODE_STATUS_REMOVE(node, MARK1);

            en->min_len = len;
            NODE_STATUS_ADD(node, MIN_FIXED);
          }
        }
        break;

      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        len = tree_min_len(NODE_BODY(node), env);
        break;
      case BAG_IF_ELSE:
        {
          OnigLen elen;

          len = tree_min_len(NODE_BODY(node), env);
          if (IS_NOT_NULL(en->te.Then))
            len += tree_min_len(en->te.Then, env);
          if (IS_NOT_NULL(en->te.Else))
            elen = tree_min_len(en->te.Else, env);
          else elen = 0;

          if (elen < len) len = elen;
        }
        break;
      }
    }
    break;

  case NODE_GIMMICK:
    {
      GimmickNode* g = GIMMICK_(node);
      if (g->type == GIMMICK_FAIL) {
        len = INFINITE_LEN;
        break;
      }
    }
    /* fall */
  case NODE_ANCHOR:
  default:
    break;
  }

  return len;
}

static OnigLen
tree_max_len(Node* node, ScanEnv* env)
{
  OnigLen len;
  OnigLen tmax;

  len = 0;
  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    do {
      tmax = tree_max_len(NODE_CAR(node), env);
      len = distance_add(len, tmax);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ALT:
    do {
      tmax = tree_max_len(NODE_CAR(node), env);
      if (len < tmax) len = tmax;
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);
      len = (OnigLen )(sn->end - sn->s);
    }
    break;

  case NODE_CTYPE:
  case NODE_CCLASS:
    len = ONIGENC_MBC_MAXLEN_DIST(env->enc);
    break;

  case NODE_BACKREF:
    if (! NODE_IS_CHECKER(node)) {
      int i;
      int* backs;
      MemEnv* mem_env = SCANENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);
      if (NODE_IS_RECURSION(node)) {
        len = INFINITE_LEN;
        break;
      }
      backs = BACKREFS_P(br);
      for (i = 0; i < br->back_num; i++) {
        tmax = tree_max_len(mem_env[backs[i]].mem_node, env);
        if (len < tmax) len = tmax;
      }
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    if (! NODE_IS_RECURSION(node))
      len = tree_max_len(NODE_BODY(node), env);
    else
      len = INFINITE_LEN;
    break;
#endif

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->upper != 0) {
        len = tree_max_len(NODE_BODY(node), env);
        if (len != 0) {
          if (! IS_INFINITE_REPEAT(qn->upper))
            len = distance_multiply(len, qn->upper);
          else
            len = INFINITE_LEN;
        }
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_MEMORY:
        if (NODE_IS_MAX_FIXED(node))
          len = en->max_len;
        else {
          if (NODE_IS_MARK1(node))
            len = INFINITE_LEN;
          else {
            NODE_STATUS_ADD(node, MARK1);
            len = tree_max_len(NODE_BODY(node), env);
            NODE_STATUS_REMOVE(node, MARK1);

            en->max_len = len;
            NODE_STATUS_ADD(node, MAX_FIXED);
          }
        }
        break;

      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        len = tree_max_len(NODE_BODY(node), env);
        break;
      case BAG_IF_ELSE:
        {
          OnigLen tlen, elen;

          len = tree_max_len(NODE_BODY(node), env);
          if (IS_NOT_NULL(en->te.Then)) {
            tlen = tree_max_len(en->te.Then, env);
            len = distance_add(len, tlen);
          }
          if (IS_NOT_NULL(en->te.Else))
            elen = tree_max_len(en->te.Else, env);
          else elen = 0;

          if (elen > len) len = elen;
        }
        break;
      }
    }
    break;

  case NODE_ANCHOR:
  case NODE_GIMMICK:
  default:
    break;
  }

  return len;
}

static int
check_backrefs(Node* node, ScanEnv* env)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = check_backrefs(NODE_CAR(node), env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) {
      r = 0;
      break;
    }
    /* fall */
  case NODE_QUANT:
    r = check_backrefs(NODE_BODY(node), env);
    break;

  case NODE_BAG:
    r = check_backrefs(NODE_BODY(node), env);
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

  case NODE_BACKREF:
    {
      int i;
      BackRefNode* br = BACKREF_(node);
      int* backs = BACKREFS_P(br);
      MemEnv* mem_env = SCANENV_MEMENV(env);

      for (i = 0; i < br->back_num; i++) {
        if (backs[i] > env->num_mem)
          return ONIGERR_INVALID_BACKREF;

        NODE_STATUS_ADD(mem_env[backs[i]].mem_node, BACKREF);
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
set_empty_repeat_node_trav(Node* node, Node* empty, ScanEnv* env)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = set_empty_repeat_node_trav(NODE_CAR(node), empty, env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
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
      r = set_empty_repeat_node_trav(NODE_BODY(node), empty, env);
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->emptiness != BODY_IS_NOT_EMPTY) empty = node;
      r = set_empty_repeat_node_trav(NODE_BODY(node), empty, env);
    }
    break;

  case NODE_BAG:
    if (IS_NOT_NULL(NODE_BODY(node))) {
      r = set_empty_repeat_node_trav(NODE_BODY(node), empty, env);
      if (r != 0) return r;
    }
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_BACKREF(node)) {
          if (IS_NOT_NULL(empty))
            SCANENV_MEMENV(env)[en->m.regnum].empty_repeat_node = empty;
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

  while ((parent = NODE_PARENT(me)) != NULL_NODE) {
    if (parent == node) return 1;
    me = parent;
  }
  return 0;
}

static void
set_empty_status_check_trav(Node* node, ScanEnv* env)
{
  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      set_empty_status_check_trav(NODE_CAR(node), env);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);

      if (! ANCHOR_HAS_BODY(an)) break;
      set_empty_status_check_trav(NODE_BODY(node), env);
    }
    break;

  case NODE_QUANT:
    set_empty_status_check_trav(NODE_BODY(node), env);
    break;

  case NODE_BAG:
    if (IS_NOT_NULL(NODE_BODY(node)))
      set_empty_status_check_trav(NODE_BODY(node), env);
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

  case NODE_BACKREF:
    {
      int i;
      int* backs;
      MemEnv* mem_env = SCANENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);
      backs = BACKREFS_P(br);
      for (i = 0; i < br->back_num; i++) {
        Node* ernode = mem_env[backs[i]].empty_repeat_node;
        if (IS_NOT_NULL(ernode)) {
          if (! is_ancestor_node(ernode, node)) {
            MEM_STATUS_LIMIT_ON(env->reg->empty_status_mem, backs[i]);
            NODE_STATUS_ADD(ernode, EMPTY_STATUS_CHECK);
            NODE_STATUS_ADD(mem_env[backs[i]].mem_node, EMPTY_STATUS_CHECK);
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
  NODE_PARENT(node) = parent;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      set_parent_node_trav(NODE_CAR(node), node);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) break;
    set_parent_node_trav(NODE_BODY(node), node);
    break;

  case NODE_QUANT:
    set_parent_node_trav(NODE_BODY(node), node);
    break;

  case NODE_BAG:
    if (IS_NOT_NULL(NODE_BODY(node)))
      set_parent_node_trav(NODE_BODY(node), node);
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
infinite_recursive_call_check(Node* node, ScanEnv* env, int head)
{
  int ret;
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    {
      Node *x;
      OnigLen min;

      x = node;
      do {
        ret = infinite_recursive_call_check(NODE_CAR(x), env, head);
        if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;
        r |= ret;
        if (head != 0) {
          min = tree_min_len(NODE_CAR(x), env);
          if (min != 0) head = 0;
        }
      } while (IS_NOT_NULL(x = NODE_CDR(x)));
    }
    break;

  case NODE_ALT:
    {
      int must;

      must = RECURSION_MUST;
      do {
        ret = infinite_recursive_call_check(NODE_CAR(node), env, head);
        if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;

        r    |= (ret & RECURSION_EXIST);
        must &= ret;
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
      r |= must;
    }
    break;

  case NODE_QUANT:
    r = infinite_recursive_call_check(NODE_BODY(node), env, head);
    if (r < 0) return r;
    if ((r & RECURSION_MUST) != 0) {
      if (QUANT_(node)->lower == 0)
        r &= ~RECURSION_MUST;
    }
    break;

  case NODE_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node)))
      break;
    /* fall */
  case NODE_CALL:
    r = infinite_recursive_call_check(NODE_BODY(node), env, head);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_MARK2(node))
          return 0;
        else if (NODE_IS_MARK1(node))
          return (head == 0 ? RECURSION_EXIST | RECURSION_MUST
                  : RECURSION_EXIST | RECURSION_MUST | RECURSION_INFINITE);
        else {
          NODE_STATUS_ADD(node, MARK2);
          r = infinite_recursive_call_check(NODE_BODY(node), env, head);
          NODE_STATUS_REMOVE(node, MARK2);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        int eret;

        ret = infinite_recursive_call_check(NODE_BODY(node), env, head);
        if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;
        r |= ret;
        if (IS_NOT_NULL(en->te.Then)) {
          OnigLen min;
          if (head != 0) {
            min = tree_min_len(NODE_BODY(node), env);
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
        r = infinite_recursive_call_check(NODE_BODY(node), env, head);
      }
    }
    break;

  default:
    break;
  }

  return r;
}

static int
infinite_recursive_call_check_trav(Node* node, ScanEnv* env)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = infinite_recursive_call_check_trav(NODE_CAR(node), env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) {
      r = 0;
      break;
    }
    /* fall */
  case NODE_QUANT:
    r = infinite_recursive_call_check_trav(NODE_BODY(node), env);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_RECURSION(node) && NODE_IS_CALLED(node)) {
          int ret;

          NODE_STATUS_ADD(node, MARK1);

          ret = infinite_recursive_call_check(NODE_BODY(node), env, 1);
          if (ret < 0) return ret;
          else if ((ret & (RECURSION_MUST | RECURSION_INFINITE)) != 0)
            return ONIGERR_NEVER_ENDING_RECURSION;

          NODE_STATUS_REMOVE(node, MARK1);
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

    r = infinite_recursive_call_check_trav(NODE_BODY(node), env);
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

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    r = 0;
    do {
      r |= recursive_call_check(NODE_CAR(node));
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) {
      r = 0;
      break;
    }
    /* fall */
  case NODE_QUANT:
    r = recursive_call_check(NODE_BODY(node));
    break;

  case NODE_CALL:
    r = recursive_call_check(NODE_BODY(node));
    if (r != 0) {
      if (NODE_IS_MARK1(NODE_BODY(node)))
        NODE_STATUS_ADD(node, RECURSION);
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_MARK2(node))
          return 0;
        else if (NODE_IS_MARK1(node))
          return 1; /* recursion */
        else {
          NODE_STATUS_ADD(node, MARK2);
          r = recursive_call_check(NODE_BODY(node));
          NODE_STATUS_REMOVE(node, MARK2);
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
        r |= recursive_call_check(NODE_BODY(node));
      }
      else {
        r = recursive_call_check(NODE_BODY(node));
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
recursive_call_check_trav(Node* node, ScanEnv* env, int state)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    {
      int ret;
      do {
        ret = recursive_call_check_trav(NODE_CAR(node), env, state);
        if (ret == FOUND_CALLED_NODE) r = FOUND_CALLED_NODE;
        else if (ret < 0) return ret;
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

  case NODE_QUANT:
    r = recursive_call_check_trav(NODE_BODY(node), env, state);
    if (QUANT_(node)->upper == 0) {
      if (r == FOUND_CALLED_NODE)
        QUANT_(node)->include_referred = 1;
    }
    break;

  case NODE_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);
      if (ANCHOR_HAS_BODY(an))
        r = recursive_call_check_trav(NODE_ANCHOR_BODY(an), env, state);
    }
    break;

  case NODE_BAG:
    {
      int ret;
      int state1;
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_CALLED(node) || (state & IN_RECURSION) != 0) {
          if (! NODE_IS_RECURSION(node)) {
            NODE_STATUS_ADD(node, MARK1);
            r = recursive_call_check(NODE_BODY(node));
            if (r != 0) {
              NODE_STATUS_ADD(node, RECURSION);
              MEM_STATUS_ON(env->backtrack_mem, en->m.regnum);
            }
            NODE_STATUS_REMOVE(node, MARK1);
          }

          if (NODE_IS_CALLED(node))
            r = FOUND_CALLED_NODE;
        }
      }

      state1 = state;
      if (NODE_IS_RECURSION(node))
        state1 |= IN_RECURSION;

      ret = recursive_call_check_trav(NODE_BODY(node), env, state1);
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
  if (NODE_CDR(prev) != a) return ;

  NODE_CDR(prev) = NODE_CDR(a);
  NODE_CDR(a) = NULL_NODE;
}

static int
reduce_string_list(Node* node)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    {
      Node* prev;
      Node* curr;
      Node* prev_node;
      Node* next_node;

      prev = NULL_NODE;
      do {
        next_node = NODE_CDR(node);
        curr = NODE_CAR(node);
        if (NODE_TYPE(curr) == NODE_STRING) {
          if (IS_NULL(prev) || STR_(curr)->flag != STR_(prev)->flag) {
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
          prev = NULL_NODE;
          prev_node = node;
        }

        node = next_node;
      } while (r == 0 && IS_NOT_NULL(node));
    }
    break;

  case NODE_ALT:
    do {
      r = reduce_string_list(NODE_CAR(node));
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (IS_NULL(NODE_BODY(node)))
      break;
    /* fall */
  case NODE_QUANT:
    r = reduce_string_list(NODE_BODY(node));
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      r = reduce_string_list(NODE_BODY(node));
      if (r != 0) return r;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = reduce_string_list(en->te.Then);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = reduce_string_list(en->te.Else);
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
#define IN_LOOK_BEHIND  (1<<6)


/* divide different length alternatives in look-behind.
  (?<=A|B) ==> (?<=A)|(?<=B)
  (?<!A|B) ==> (?<!A)(?<!B)
*/
static int
divide_look_behind_alternatives(Node* node)
{
  Node *head, *np, *insert_node;
  AnchorNode* an = ANCHOR_(node);
  int anc_type = an->type;

  head = NODE_ANCHOR_BODY(an);
  np = NODE_CAR(head);
  node_swap(node, head);
  NODE_CAR(node) = head;
  NODE_BODY(head) = np;

  np = node;
  while (IS_NOT_NULL(np = NODE_CDR(np))) {
    insert_node = onig_node_new_anchor(anc_type, an->ascii_mode);
    CHECK_NULL_RETURN_MEMERR(insert_node);
    NODE_BODY(insert_node) = NODE_CAR(np);
    NODE_CAR(np) = insert_node;
  }

  if (anc_type == ANCR_LOOK_BEHIND_NOT) {
    np = node;
    do {
      NODE_SET_TYPE(np, NODE_LIST);  /* alt -> list */
    } while (IS_NOT_NULL(np = NODE_CDR(np)));
  }
  return 0;
}

static int
tune_look_behind(Node* node, regex_t* reg, ScanEnv* env)
{
  int r, len;
  AnchorNode* an = ANCHOR_(node);

  r = get_char_len_node(NODE_ANCHOR_BODY(an), reg, &len);
  if (r == 0)
    an->char_len = len;
  else if (r == GET_CHAR_LEN_VARLEN)
    r = ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
  else if (r == GET_CHAR_LEN_TOP_ALT_VARLEN) {
    if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND))
      r = divide_look_behind_alternatives(node);
    else
      r = ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
  }

  return r;
}

static int
tune_next(Node* node, Node* next_node, regex_t* reg)
{
  NodeType type;

 retry:
  type = NODE_TYPE(node);
  if (type == NODE_QUANT) {
    QuantNode* qn = QUANT_(node);
    if (qn->greedy && IS_INFINITE_REPEAT(qn->upper)) {
#ifdef USE_QUANT_PEEK_NEXT
      Node* n = get_head_value_node(next_node, 1, reg);
      /* '\0': for UTF-16BE etc... */
      if (IS_NOT_NULL(n) && STR_(n)->s[0] != '\0') {
        qn->next_head_exact = n;
      }
#endif
      /* automatic posseivation a*b ==> (?>a*)b */
      if (qn->lower <= 1) {
        if (is_strict_real_node(NODE_BODY(node))) {
          Node *x, *y;
          x = get_head_value_node(NODE_BODY(node), 0, reg);
          if (IS_NOT_NULL(x)) {
            y = get_head_value_node(next_node,  0, reg);
            if (IS_NOT_NULL(y) && is_exclusive(x, y, reg)) {
              Node* en = onig_node_new_bag(BAG_STOP_BACKTRACK);
              CHECK_NULL_RETURN_MEMERR(en);
              NODE_STATUS_ADD(en, STRICT_REAL_REPEAT);
              node_swap(node, en);
              NODE_BODY(node) = en;
            }
          }
        }
      }
    }
  }
  else if (type == NODE_BAG) {
    BagNode* en = BAG_(node);
    if (en->type == BAG_MEMORY) {
      node = NODE_BODY(node);
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
get_min_max_byte_len_case_fold_items(int n, OnigCaseFoldCodeItem items[], int* rmin, int* rmax)
{
  int i, len, minlen, maxlen;

  minlen = INT_MAX;
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
conv_string_case_fold(OnigEncoding enc, OnigCaseFoldType case_fold_flag,
           UChar* s, UChar* end, UChar** rs, UChar** rend, int* rcase_min_len)
{
  UChar *p, buf[ONIGENC_MBC_CASE_FOLD_MAXLEN];
  UChar *sbuf, *ebuf, *sp;
  int i, n, len, sbuf_size;

  *rs = NULL;
  sbuf_size = (int )(end - s) * 2;
  sbuf = (UChar* )xmalloc(sbuf_size);
  CHECK_NULL_RETURN_MEMERR(sbuf);
  ebuf = sbuf + sbuf_size;

  n = 0;
  sp = sbuf;
  p = s;
  while (p < end) {
    len = ONIGENC_MBC_CASE_FOLD(enc, case_fold_flag, &p, end, buf);
    for (i = 0; i < len; i++) {
      if (sp >= ebuf) {
        sbuf = (UChar* )xrealloc(sbuf, sbuf_size * 2);
        CHECK_NULL_RETURN_MEMERR(sbuf);
        sp = sbuf + sbuf_size;
        sbuf_size *= 2;
        ebuf = sbuf + sbuf_size;
      }

      *sp++ = buf[i];
    }
    n++;
  }

  *rs = sbuf;
  *rend = sp;
  *rcase_min_len = n;
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
                      unsigned int flag, int case_min_len)
{
  int r;
  Node *sn, *list;

  list = *rlist;
  sn   = *rsn;

  if (IS_NOT_NULL(sn) && STR_(sn)->flag == flag) {
    if (NODE_STRING_IS_CASE_FOLD_MATCH(sn))
      r = node_str_cat_case_fold(sn, s, end, case_min_len);
    else
      r = onig_node_str_cat(sn, s, end);
  }
  else {
    sn = onig_node_new_str(s, end);
    CHECK_NULL_RETURN_MEMERR(sn);

    STR_(sn)->flag = flag;
    STR_(sn)->case_min_len = case_min_len;
    r = unravel_cf_node_add(&list, sn);
  }

  if (r == 0) {
    *rlist = list;
    *rsn = sn;
  }
  return r;
}

static int
unravel_cf_string_fold_add(Node** rlist, Node** rsn, OnigEncoding enc,
                      OnigCaseFoldType case_fold_flag, UChar* s, UChar* end)
{
  int r;
  int case_min_len;
  UChar *rs, *rend;

  r = conv_string_case_fold(enc, case_fold_flag, s, end,
                            &rs, &rend, &case_min_len);
  if (r != 0) return r;

  r = unravel_cf_string_add(rlist, rsn, rs, rend,
                            NODE_STRING_CASE_FOLD_MATCH, case_min_len);
  xfree(rs);

  return r;
}

static int
unravel_cf_string_alt_or_cc_add(Node** rlist, int n,
            OnigCaseFoldCodeItem items[], int byte_len, OnigEncoding enc,
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

      NODE_CDR(curr) = alt;
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
                UChar* s, int one_len)
{
  int r, i, found;

  found = 0;
  for (i = 0; i < n; i++) {
    OnigCaseFoldCodeItem* item = items + i;
    if (item->byte_len == one_len) {
      if (item->code_len == 1) {
        found = 1;
      }
    }
  }

  if (found == 0) {
    r = unravel_cf_string_add(rlist, rsn, s, s + one_len, 0 /* flag */, 0);
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
  int r, n, one_len, min_len, max_len, in_look_behind;
  UChar *start, *end, *p, *q;
  StrNode* snode;
  Node *sn, *list;
  OnigEncoding enc;
  OnigCaseFoldCodeItem items[ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM];

  if (NODE_STRING_IS_CASE_EXPANDED(node)) return 0;

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

    one_len = enclen(enc, p);
    if (n == 0) {
      q = p + one_len;
      r = unravel_cf_string_add(&list, &sn, p, q, 0 /* flag */, 0);
      if (r != 0) goto err;
    }
    else {
      if (in_look_behind != 0) {
        q = p + one_len;
        r = unravel_cf_look_behind_add(&list, &sn, n, items, enc, p, one_len);
        if (r != 0) goto err;
      }
      else {
        get_min_max_byte_len_case_fold_items(n, items, &min_len, &max_len);
        q = p + max_len;
        if (one_len == max_len && min_len == max_len) {
          r = unravel_cf_string_alt_or_cc_add(&list, n, items, max_len, enc,
                                              reg->case_fold_flag, p, q);
          if (r != 0) goto err;
          sn = NULL_NODE;
        }
        else {
          r = unravel_cf_string_fold_add(&list, &sn, enc, reg->case_fold_flag,
                                         p, q);
          if (r != 0) goto err;
        }
      }
    }

    p = q;
  }

  if (IS_NOT_NULL(list)) {
    if (node_list_len(list) == 1) {
      node_swap(node, NODE_CAR(list));
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

#ifdef USE_STUBBORN_CHECK_CAPTURES_IN_EMPTY_REPEAT
static enum BodyEmptyType
quantifiers_memory_node_info(Node* node)
{
  int r = BODY_IS_EMPTY_POSSIBILITY;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    {
      int v;
      do {
        v = quantifiers_memory_node_info(NODE_CAR(node));
        if (v > r) r = v;
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    if (NODE_IS_RECURSION(node)) {
      return BODY_IS_EMPTY_POSSIBILITY_REC; /* tiny version */
    }
    else
      r = quantifiers_memory_node_info(NODE_BODY(node));
    break;
#endif

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);
      if (qn->upper != 0) {
        r = quantifiers_memory_node_info(NODE_BODY(node));
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_MEMORY:
        if (NODE_IS_RECURSION(node)) {
          return BODY_IS_EMPTY_POSSIBILITY_REC;
        }
        return BODY_IS_EMPTY_POSSIBILITY_MEM;
        break;

      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        r = quantifiers_memory_node_info(NODE_BODY(node));
        break;
      case BAG_IF_ELSE:
        {
          int v;
          r = quantifiers_memory_node_info(NODE_BODY(node));
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

  case NODE_BACKREF:
  case NODE_STRING:
  case NODE_CTYPE:
  case NODE_CCLASS:
  case NODE_ANCHOR:
  case NODE_GIMMICK:
  default:
    break;
  }

  return r;
}
#endif /* USE_STUBBORN_CHECK_CAPTURES_IN_EMPTY_REPEAT */


#ifdef USE_CALL

#ifdef __GNUC__
__inline
#endif
static int
tune_call_node_call(CallNode* cn, ScanEnv* env, int state)
{
  MemEnv* mem_env = SCANENV_MEMENV(env);

  if (cn->by_number != 0) {
    int gnum = cn->group_num;

    if (env->num_named > 0 &&
        IS_SYNTAX_BV(env->syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
        ! ONIG_IS_OPTION_ON(env->options, ONIG_OPTION_CAPTURE_GROUP)) {
      return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;
    }

    if (gnum > env->num_mem) {
      onig_scan_env_set_error_string(env, ONIGERR_UNDEFINED_GROUP_REFERENCE,
                                     cn->name, cn->name_end);
      return ONIGERR_UNDEFINED_GROUP_REFERENCE;
    }

  set_call_attr:
    NODE_CALL_BODY(cn) = mem_env[cn->group_num].mem_node;
    if (IS_NULL(NODE_CALL_BODY(cn))) {
      onig_scan_env_set_error_string(env, ONIGERR_UNDEFINED_NAME_REFERENCE,
                                     cn->name, cn->name_end);
      return ONIGERR_UNDEFINED_NAME_REFERENCE;
    }
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
      cn->group_num = refs[0];
      goto set_call_attr;
    }
  }

  return 0;
}

static void
tune_call2_call(Node* node)
{
  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      tune_call2_call(NODE_CAR(node));
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    tune_call2_call(NODE_BODY(node));
    break;

  case NODE_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      tune_call2_call(NODE_BODY(node));
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (! NODE_IS_MARK1(node)) {
          NODE_STATUS_ADD(node, MARK1);
          tune_call2_call(NODE_BODY(node));
          NODE_STATUS_REMOVE(node, MARK1);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        tune_call2_call(NODE_BODY(node));
        if (IS_NOT_NULL(en->te.Then))
          tune_call2_call(en->te.Then);
        if (IS_NOT_NULL(en->te.Else))
          tune_call2_call(en->te.Else);
      }
      else {
        tune_call2_call(NODE_BODY(node));
      }
    }
    break;

  case NODE_CALL:
    if (! NODE_IS_MARK1(node)) {
      NODE_STATUS_ADD(node, MARK1);
      {
        CallNode* cn = CALL_(node);
        Node* called = NODE_CALL_BODY(cn);

        cn->entry_count++;

        NODE_STATUS_ADD(called, CALLED);
        BAG_(called)->m.entry_count++;
        tune_call2_call(called);
      }
      NODE_STATUS_REMOVE(node, MARK1);
    }
    break;

  default:
    break;
  }
}

static int
tune_call(Node* node, ScanEnv* env, int state)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = tune_call(NODE_CAR(node), env, state);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    if (QUANT_(node)->upper == 0)
      state |= IN_ZERO_REPEAT;

    r = tune_call(NODE_BODY(node), env, state);
    break;

  case NODE_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      r = tune_call(NODE_BODY(node), env, state);
    else
      r = 0;
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if ((state & IN_ZERO_REPEAT) != 0) {
          NODE_STATUS_ADD(node, IN_ZERO_REPEAT);
          BAG_(node)->m.entry_count--;
        }
        r = tune_call(NODE_BODY(node), env, state);
      }
      else if (en->type == BAG_IF_ELSE) {
        r = tune_call(NODE_BODY(node), env, state);
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = tune_call(en->te.Then, env, state);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else))
          r = tune_call(en->te.Else, env, state);
      }
      else
        r = tune_call(NODE_BODY(node), env, state);
    }
    break;

  case NODE_CALL:
    if ((state & IN_ZERO_REPEAT) != 0) {
      NODE_STATUS_ADD(node, IN_ZERO_REPEAT);
      CALL_(node)->entry_count--;
    }

    r = tune_call_node_call(CALL_(node), env, state);
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

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = tune_call2(NODE_CAR(node));
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    if (QUANT_(node)->upper != 0)
      r = tune_call2(NODE_BODY(node));
    break;

  case NODE_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      r = tune_call2(NODE_BODY(node));
    break;

  case NODE_BAG:
    if (! NODE_IS_IN_ZERO_REPEAT(node))
      r = tune_call2(NODE_BODY(node));

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

  case NODE_CALL:
    if (! NODE_IS_IN_ZERO_REPEAT(node)) {
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
  switch (NODE_TYPE(node)) {
  case NODE_ALT:
    state |= IN_ALT;
    /* fall */
  case NODE_LIST:
    do {
      tune_called_state_call(NODE_CAR(node), state);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 2)
        state |= IN_REAL_REPEAT;
      if (qn->lower != qn->upper)
        state |= IN_VAR_REPEAT;

      tune_called_state_call(NODE_QUANT_BODY(qn), state);
    }
    break;

  case NODE_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);

      switch (an->type) {
      case ANCR_PREC_READ_NOT:
      case ANCR_LOOK_BEHIND_NOT:
        state |= IN_NOT;
        /* fall */
      case ANCR_PREC_READ:
      case ANCR_LOOK_BEHIND:
        tune_called_state_call(NODE_ANCHOR_BODY(an), state);
        break;
      default:
        break;
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_MARK1(node)) {
          if ((~en->m.called_state & state) != 0) {
            en->m.called_state |= state;
            tune_called_state_call(NODE_BODY(node), state);
          }
        }
        else {
          NODE_STATUS_ADD(node, MARK1);
          en->m.called_state |= state;
          tune_called_state_call(NODE_BODY(node), state);
          NODE_STATUS_REMOVE(node, MARK1);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        state |= IN_ALT;
        tune_called_state_call(NODE_BODY(node), state);
        if (IS_NOT_NULL(en->te.Then)) {
          tune_called_state_call(en->te.Then, state);
        }
        if (IS_NOT_NULL(en->te.Else))
          tune_called_state_call(en->te.Else, state);
      }
      else {
        tune_called_state_call(NODE_BODY(node), state);
      }
    }
    break;

  case NODE_CALL:
    tune_called_state_call(NODE_BODY(node), state);
    break;

  default:
    break;
  }
}

static void
tune_called_state(Node* node, int state)
{
  switch (NODE_TYPE(node)) {
  case NODE_ALT:
    state |= IN_ALT;
    /* fall */
  case NODE_LIST:
    do {
      tune_called_state(NODE_CAR(node), state);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

#ifdef USE_CALL
  case NODE_CALL:
    tune_called_state_call(node, state);
    break;
#endif

  case NODE_BAG:
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
        tune_called_state(NODE_BODY(node), state);
        break;
      case BAG_IF_ELSE:
        state |= IN_ALT;
        tune_called_state(NODE_BODY(node), state);
        if (IS_NOT_NULL(en->te.Then))
          tune_called_state(en->te.Then, state);
        if (IS_NOT_NULL(en->te.Else))
          tune_called_state(en->te.Else, state);
        break;
      }
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 2)
        state |= IN_REAL_REPEAT;
      if (qn->lower != qn->upper)
        state |= IN_VAR_REPEAT;

      tune_called_state(NODE_QUANT_BODY(qn), state);
    }
    break;

  case NODE_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);

      switch (an->type) {
      case ANCR_PREC_READ_NOT:
      case ANCR_LOOK_BEHIND_NOT:
        state |= IN_NOT;
        /* fall */
      case ANCR_PREC_READ:
      case ANCR_LOOK_BEHIND:
        tune_called_state(NODE_ANCHOR_BODY(an), state);
        break;
      default:
        break;
      }
    }
    break;

  case NODE_BACKREF:
  case NODE_STRING:
  case NODE_CTYPE:
  case NODE_CCLASS:
  case NODE_GIMMICK:
  default:
    break;
  }
}

#endif  /* USE_CALL */


static int tune_tree(Node* node, regex_t* reg, int state, ScanEnv* env);

#ifdef __GNUC__
__inline
#endif
static int
tune_anchor(Node* node, regex_t* reg, int state, ScanEnv* env)
{
/* allowed node types in look-behind */
#define ALLOWED_TYPE_IN_LB \
  ( NODE_BIT_LIST | NODE_BIT_ALT | NODE_BIT_STRING | NODE_BIT_CCLASS \
  | NODE_BIT_CTYPE | NODE_BIT_ANCHOR | NODE_BIT_BAG | NODE_BIT_QUANT \
  | NODE_BIT_CALL | NODE_BIT_GIMMICK)

#define ALLOWED_BAG_IN_LB       ( 1<<BAG_MEMORY | 1<<BAG_OPTION | 1<<BAG_IF_ELSE )
#define ALLOWED_BAG_IN_LB_NOT   ( 1<<BAG_OPTION | 1<<BAG_IF_ELSE )

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

  int r;
  AnchorNode* an = ANCHOR_(node);

  switch (an->type) {
  case ANCR_PREC_READ:
    r = tune_tree(NODE_ANCHOR_BODY(an), reg, state, env);
    break;
  case ANCR_PREC_READ_NOT:
    r = tune_tree(NODE_ANCHOR_BODY(an), reg, (state | IN_NOT), env);
    break;

  case ANCR_LOOK_BEHIND:
    {
      r = check_type_tree(NODE_ANCHOR_BODY(an), ALLOWED_TYPE_IN_LB,
                          ALLOWED_BAG_IN_LB, ALLOWED_ANCHOR_IN_LB);
      if (r < 0) return r;
      if (r > 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
      r = tune_tree(NODE_ANCHOR_BODY(an), reg, (state|IN_LOOK_BEHIND), env);
      if (r != 0) return r;
      r = tune_look_behind(node, reg, env);
    }
    break;

  case ANCR_LOOK_BEHIND_NOT:
    {
      r = check_type_tree(NODE_ANCHOR_BODY(an), ALLOWED_TYPE_IN_LB,
                          ALLOWED_BAG_IN_LB_NOT, ALLOWED_ANCHOR_IN_LB_NOT);
      if (r < 0) return r;
      if (r > 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
      r = tune_tree(NODE_ANCHOR_BODY(an), reg, (state|IN_NOT|IN_LOOK_BEHIND),
                    env);
      if (r != 0) return r;
      r = tune_look_behind(node, reg, env);
    }
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
tune_quant(Node* node, regex_t* reg, int state, ScanEnv* env)
{
  int r;
  OnigLen d;
  QuantNode* qn = QUANT_(node);
  Node* body = NODE_BODY(node);

  if ((state & IN_REAL_REPEAT) != 0) {
    NODE_STATUS_ADD(node, IN_REAL_REPEAT);
  }
  if ((state & IN_MULTI_ENTRY) != 0) {
    NODE_STATUS_ADD(node, IN_MULTI_ENTRY);
  }

  if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 1) {
    d = tree_min_len(body, env);
    if (d == 0) {
#ifdef USE_STUBBORN_CHECK_CAPTURES_IN_EMPTY_REPEAT
      qn->emptiness = quantifiers_memory_node_info(body);
#else
      qn->emptiness = BODY_IS_EMPTY_POSSIBILITY;
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
  if (NODE_TYPE(body) == NODE_STRING) {
    if (!IS_INFINITE_REPEAT(qn->lower) && qn->lower == qn->upper &&
        qn->lower > 1 && qn->lower <= EXPAND_STRING_MAX_LENGTH) {
      int len = NODE_STRING_LEN(body);

      if (len * qn->lower <= EXPAND_STRING_MAX_LENGTH) {
        int i, n = qn->lower;
        node_conv_to_str_node(node, STR_(body)->flag);
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
    if (NODE_TYPE(body) == NODE_QUANT) {
      QuantNode* tqn = QUANT_(body);
      if (IS_NOT_NULL(tqn->head_exact)) {
        qn->head_exact  = tqn->head_exact;
        tqn->head_exact = NULL;
      }
    }
    else {
      qn->head_exact = get_head_value_node(NODE_BODY(node), 1, reg);
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
tune_tree(Node* node, regex_t* reg, int state, ScanEnv* env)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    {
      Node* prev = NULL_NODE;
      do {
        r = tune_tree(NODE_CAR(node), reg, state, env);
        if (IS_NOT_NULL(prev) && r == 0) {
          r = tune_next(prev, NODE_CAR(node), reg);
        }
        prev = NODE_CAR(node);
      } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

  case NODE_ALT:
    do {
      r = tune_tree(NODE_CAR(node), reg, (state | IN_ALT), env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_STRING:
    if (IS_IGNORECASE(reg->options) && !NODE_STRING_IS_CRUDE(node)) {
      r = unravel_case_fold_string(node, reg, state);
    }
    break;

  case NODE_BACKREF:
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
        if (NODE_IS_NEST_LEVEL(node)) {
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

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      switch (en->type) {
      case BAG_OPTION:
        {
          OnigOptionType options = reg->options;
          reg->options = BAG_(node)->o.options;
          r = tune_tree(NODE_BODY(node), reg, state, env);
          reg->options = options;
        }
        break;

      case BAG_MEMORY:
#ifdef USE_CALL
        state |= en->m.called_state;
#endif

        if ((state & (IN_ALT | IN_NOT | IN_VAR_REPEAT | IN_MULTI_ENTRY)) != 0
            || NODE_IS_RECURSION(node)) {
          MEM_STATUS_ON(env->backtrack_mem, en->m.regnum);
        }
        r = tune_tree(NODE_BODY(node), reg, state, env);
        break;

      case BAG_STOP_BACKTRACK:
        {
          Node* target = NODE_BODY(node);
          r = tune_tree(target, reg, state, env);
          if (NODE_TYPE(target) == NODE_QUANT) {
            QuantNode* tqn = QUANT_(target);
            if (IS_INFINITE_REPEAT(tqn->upper) && tqn->lower <= 1 &&
                tqn->greedy != 0) {  /* (?>a*), a*+ etc... */
              if (is_strict_real_node(NODE_BODY(target)))
                NODE_STATUS_ADD(node, STRICT_REAL_REPEAT);
            }
          }
        }
        break;

      case BAG_IF_ELSE:
        r = tune_tree(NODE_BODY(node), reg, (state | IN_ALT), env);
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

  case NODE_QUANT:
    r = tune_quant(node, reg, state, env);
    break;

  case NODE_ANCHOR:
    r = tune_anchor(node, reg, state, env);
    break;

#ifdef USE_CALL
  case NODE_CALL:
#endif
  case NODE_CTYPE:
  case NODE_CCLASS:
  case NODE_GIMMICK:
  default:
    break;
  }

  return r;
}

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


#define OPT_EXACT_MAXLEN   24

#if OPT_EXACT_MAXLEN >= UCHAR_MAX
#error Too big OPT_EXACT_MAXLEN
#endif

typedef struct {
  OnigLen min;  /* min byte length */
  OnigLen max;  /* max byte length */
} MinMax;

typedef struct {
  MinMax           mm;
  OnigEncoding     enc;
  OnigOptionType   options;
  OnigCaseFoldType case_fold_flag;
  ScanEnv*         scan_env;
} OptEnv;

typedef struct {
  int left;
  int right;
} OptAnc;

typedef struct {
  MinMax     mm;   /* position */
  OptAnc     anc;
  int        reach_end;
  int        case_fold;
  int        len;
  UChar      s[OPT_EXACT_MAXLEN];
} OptStr;

typedef struct {
  MinMax    mm;     /* position */
  OptAnc    anc;
  int       value;  /* weighted value */
  UChar     map[CHAR_MAP_SIZE];
} OptMap;

typedef struct {
  MinMax  len;
  OptAnc  anc;
  OptStr  sb;     /* boundary */
  OptStr  sm;     /* middle */
  OptStr  spr;    /* prec read (?=...) */
  OptMap  map;    /* boundary */
} OptNode;


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
distance_value(MinMax* mm)
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
comp_distance_value(MinMax* d1, MinMax* d2, int v1, int v2)
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

static int
is_equal_mml(MinMax* a, MinMax* b)
{
  return a->min == b->min && a->max == b->max;
}

static void
set_mml(MinMax* l, OnigLen min, OnigLen max)
{
  l->min = min;
  l->max = max;
}

static void
clear_mml(MinMax* l)
{
  l->min = l->max = 0;
}

static void
copy_mml(MinMax* to, MinMax* from)
{
  to->min = from->min;
  to->max = from->max;
}

static void
add_mml(MinMax* to, MinMax* from)
{
  to->min = distance_add(to->min, from->min);
  to->max = distance_add(to->max, from->max);
}

static void
alt_merge_mml(MinMax* to, MinMax* from)
{
  if (to->min > from->min) to->min = from->min;
  if (to->max < from->max) to->max = from->max;
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
  clear_mml(&e->mm);
  clear_opt_anc_info(&e->anc);
  e->reach_end      = 0;
  e->case_fold      = 0;
  e->len            = 0;
  e->s[0]           = '\0';
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

  if (add->case_fold != 0) {
    if (! to->case_fold) {
      if (to->len > 1 || to->len >= add->len) return 0;  /* avoid */

      to->case_fold = 1;
    }
  }

  r = 0;
  p = add->s;
  end = p + add->len;
  for (i = to->len; p < end; ) {
    len = enclen(enc, p);
    if (i + len > OPT_EXACT_MAXLEN) {
      r = 1; /* 1:full */
      break;
    }
    for (j = 0; j < len && p < end; j++)
      to->s[i++] = *p++;
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
    for (j = 0; j < len && p < end; j++)
      to->s[i++] = *p++;
  }

  to->len = i;

  if (p >= end && to->len == (int )(end - s))
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

  if (! is_equal_mml(&to->mm, &add->mm)) {
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
  if (add->case_fold != 0)
    to->case_fold = 1;

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

  if (now->case_fold == 0) vn *= 2;
  if (alt->case_fold == 0) va *= 2;

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

static int
add_char_amb_opt_map(OptMap* map, UChar* p, UChar* end,
                     OnigEncoding enc, OnigCaseFoldType fold_flag)
{
  OnigCaseFoldCodeItem items[ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM];
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
  int i, n;

  add_char_opt_map(map, p[0], enc);

  fold_flag = DISABLE_CASE_FOLD_MULTI_CHAR(fold_flag);
  n = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, fold_flag, p, end, items);
  if (n < 0) return n;

  for (i = 0; i < n; i++) {
    ONIGENC_CODE_TO_MBC(enc, items[i].code[0], buf);
    add_char_opt_map(map, buf[0], enc);
  }

  return 0;
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

  if (e->case_fold != 0) {
    case_value = 1;
  }
  else
    case_value = 3;

  ae = COMP_EM_BASE * e->len * case_value;
  am = COMP_EM_BASE * 5 * 2 / m->value;
  return comp_distance_value(&e->mm, &m->mm, ae, am);
}

static void
alt_merge_opt_map(OnigEncoding enc, OptMap* to, OptMap* add)
{
  int i, val;

  /* if (! is_equal_mml(&to->mm, &add->mm)) return ; */
  if (to->value == 0) return ;
  if (add->value == 0 || to->mm.max < add->mm.min) {
    clear_opt_map(to);
    return ;
  }

  alt_merge_mml(&to->mm, &add->mm);

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
set_bound_node_opt_info(OptNode* opt, MinMax* plen)
{
  copy_mml(&(opt->sb.mm),  plen);
  copy_mml(&(opt->spr.mm), plen);
  copy_mml(&(opt->map.mm), plen);
}

static void
clear_node_opt_info(OptNode* opt)
{
  clear_mml(&opt->len);
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
  add_mml(&to->len, &add->len);
}

static void
alt_merge_node_opt_info(OptNode* to, OptNode* add, OptEnv* env)
{
  alt_merge_opt_anc_info(&to->anc, &add->anc);
  alt_merge_opt_exact(&to->sb,  &add->sb, env);
  alt_merge_opt_exact(&to->sm,  &add->sm, env);
  alt_merge_opt_exact(&to->spr, &add->spr, env);
  alt_merge_opt_map(env->enc, &to->map, &add->map);

  alt_merge_mml(&to->len, &add->len);
}


#define MAX_NODE_OPT_INFO_REF_COUNT    5

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

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    {
      OptEnv nenv;
      Node* nd = node;

      copy_opt_env(&nenv, env);
      do {
        r = optimize_nodes(NODE_CAR(nd), &xo, &nenv);
        if (r == 0) {
          add_mml(&nenv.mm, &xo.len);
          concat_left_node_opt_info(enc, opt, &xo);
        }
      } while (r == 0 && IS_NOT_NULL(nd = NODE_CDR(nd)));
    }
    break;

  case NODE_ALT:
    {
      Node* nd = node;

      do {
        r = optimize_nodes(NODE_CAR(nd), &xo, env);
        if (r == 0) {
          if (nd == node) copy_node_opt_info(opt, &xo);
          else            alt_merge_node_opt_info(opt, &xo, env);
        }
      } while ((r == 0) && IS_NOT_NULL(nd = NODE_CDR(nd)));
    }
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);
      int slen = (int )(sn->end - sn->s);

      if (! NODE_STRING_IS_CASE_FOLD_MATCH(node)) {
        concat_opt_exact_str(&opt->sb, sn->s, sn->end, enc);
        if (slen > 0) {
          add_char_opt_map(&opt->map, *(sn->s), enc);
        }
        set_mml(&opt->len, slen, slen);
      }
      else {
        int max, min;

        concat_opt_exact_str(&opt->sb, sn->s, sn->end, enc);
        opt->sb.case_fold = 1;

        if (slen > 0) {
          r = add_char_amb_opt_map(&opt->map, sn->s, sn->end,
                                   enc, env->case_fold_flag);
          if (r != 0) break;
        }

        max = slen;
        min = sn->case_min_len * ONIGENC_MBC_MINLEN(enc);
        set_mml(&opt->len, min, max);
      }
    }
    break;

  case NODE_CCLASS:
    {
      int z;
      CClassNode* cc = CCLASS_(node);

      /* no need to check ignore case. (set in tune_tree()) */

      if (IS_NOT_NULL(cc->mbuf) || IS_NCCLASS_NOT(cc)) {
        OnigLen min = ONIGENC_MBC_MINLEN(enc);
        OnigLen max = ONIGENC_MBC_MAXLEN_DIST(enc);

        set_mml(&opt->len, min, max);
      }
      else {
        for (i = 0; i < SINGLE_BYTE_SIZE; i++) {
          z = BITSET_AT(cc->bs, i);
          if ((z && ! IS_NCCLASS_NOT(cc)) || (! z && IS_NCCLASS_NOT(cc))) {
            add_char_opt_map(&opt->map, (UChar )i, enc);
          }
        }
        set_mml(&opt->len, 1, 1);
      }
    }
    break;

  case NODE_CTYPE:
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
      set_mml(&opt->len, min, max);
    }
    break;

  case NODE_ANCHOR:
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
        r = optimize_nodes(NODE_BODY(node), &xo, env);
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

  case NODE_BACKREF:
    if (! NODE_IS_CHECKER(node)) {
      int* backs;
      OnigLen min, max, tmin, tmax;
      MemEnv* mem_env = SCANENV_MEMENV(env->scan_env);
      BackRefNode* br = BACKREF_(node);

      if (NODE_IS_RECURSION(node)) {
        set_mml(&opt->len, 0, INFINITE_LEN);
        break;
      }
      backs = BACKREFS_P(br);
      min = tree_min_len(mem_env[backs[0]].mem_node, env->scan_env);
      max = tree_max_len(mem_env[backs[0]].mem_node, env->scan_env);
      for (i = 1; i < br->back_num; i++) {
        tmin = tree_min_len(mem_env[backs[i]].mem_node, env->scan_env);
        tmax = tree_max_len(mem_env[backs[i]].mem_node, env->scan_env);
        if (min > tmin) min = tmin;
        if (max < tmax) max = tmax;
      }
      set_mml(&opt->len, min, max);
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    if (NODE_IS_RECURSION(node))
      set_mml(&opt->len, 0, INFINITE_LEN);
    else {
      OnigOptionType save = env->options;
      env->options = BAG_(NODE_BODY(node))->o.options;
      r = optimize_nodes(NODE_BODY(node), opt, env);
      env->options = save;
    }
    break;
#endif

  case NODE_QUANT:
    {
      OnigLen min, max;
      QuantNode* qn = QUANT_(node);

      r = optimize_nodes(NODE_BODY(node), &xo, env);
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
            NODE_IS_ANYCHAR(NODE_BODY(node)) && qn->greedy != 0) {
          if (IS_MULTILINE(CTYPE_OPTION(NODE_QUANT_BODY(qn), env)))
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
      set_mml(&opt->len, min, max);
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      switch (en->type) {
      case BAG_OPTION:
        {
          OnigOptionType save = env->options;

          env->options = en->o.options;
          r = optimize_nodes(NODE_BODY(node), opt, env);
          env->options = save;
        }
        break;

      case BAG_MEMORY:
#ifdef USE_CALL
        en->opt_count++;
        if (en->opt_count > MAX_NODE_OPT_INFO_REF_COUNT) {
          OnigLen min, max;

          min = 0;
          max = INFINITE_LEN;
          if (NODE_IS_MIN_FIXED(node)) min = en->min_len;
          if (NODE_IS_MAX_FIXED(node)) max = en->max_len;
          set_mml(&opt->len, min, max);
        }
        else
#endif
          {
            r = optimize_nodes(NODE_BODY(node), opt, env);
            if (is_set_opt_anc_info(&opt->anc, ANCR_ANYCHAR_INF_MASK)) {
              if (MEM_STATUS_AT0(env->scan_env->backrefed_mem, en->m.regnum))
                remove_opt_anc_info(&opt->anc, ANCR_ANYCHAR_INF_MASK);
            }
          }
        break;

      case BAG_STOP_BACKTRACK:
        r = optimize_nodes(NODE_BODY(node), opt, env);
        break;

      case BAG_IF_ELSE:
        {
          OptEnv nenv;

          copy_opt_env(&nenv, env);
          r = optimize_nodes(NODE_BAG_BODY(en), &xo, &nenv);
          if (r == 0) {
            add_mml(&nenv.mm, &xo.len);
            concat_left_node_opt_info(enc, opt, &xo);
            if (IS_NOT_NULL(en->te.Then)) {
              r = optimize_nodes(en->te.Then, &xo, &nenv);
              if (r == 0) {
                concat_left_node_opt_info(enc, opt, &xo);
              }
            }

            if (IS_NOT_NULL(en->te.Else)) {
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

  case NODE_GIMMICK:
    break;

  default:
#ifdef ONIG_DEBUG
    fprintf(stderr, "optimize_nodes: undefined node type %d\n", NODE_TYPE(node));
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

  if (e->len == 0) return 0;

  reg->exact = (UChar* )xmalloc(e->len);
  CHECK_NULL_RETURN_MEMERR(reg->exact);
  xmemcpy(reg->exact, e->s, e->len);
  reg->exact_end = reg->exact + e->len;

  if (e->case_fold) {
    reg->optimize = OPTIMIZE_STR_CASE_FOLD;
  }
  else {
    int allow_reverse;

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
  }

  reg->dist_min = e->mm.min;
  reg->dist_max = e->mm.max;

  if (reg->dist_min != INFINITE_LEN) {
    int n;
    if (e->case_fold != 0)
      n = 1;
    else
      n = (int )(reg->exact_end - reg->exact);

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
    reg->threshold_len = reg->dist_min + 1;
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
set_optimize_info_from_tree(Node* node, regex_t* reg, ScanEnv* scan_env)
{
  int r;
  OptNode opt;
  OptEnv env;

  env.enc            = reg->enc;
  env.options        = reg->options;
  env.case_fold_flag = reg->case_fold_flag;
  env.scan_env       = scan_env;
  clear_mml(&env.mm);

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
  print_optimize_info(stderr, reg);
#endif
  return r;
}

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

#ifdef ONIG_DEBUG

static void print_enc_string(FILE* fp, OnigEncoding enc,
                             const UChar *s, const UChar *end)
{
  fprintf(fp, "\nPATTERN: /");

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
      fputc((int )*s, fp);
      s++;
    }
  }

  fprintf(fp, "/\n");
}

#endif /* ONIG_DEBUG */

#if defined(ONIG_DEBUG_COMPILE) || defined(ONIG_DEBUG_MATCH)

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
  static const char* on[] = { "NONE", "STR",
                              "STR_FAST", "STR_FAST_STEP_FORWARD",
                              "STR_CASE_FOLD", "MAP" };

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
    fprintf(f, "]: length: %ld\n", (reg->exact_end - reg->exact));
  }
  else if (reg->optimize & OPTIMIZE_MAP) {
    int c, i, n = 0;

    for (i = 0; i < CHAR_MAP_SIZE; i++)
      if (reg->map[i]) n++;

    fprintf(f, "map: n=%d\n", n);
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
            fprintf(f, "%d", i);
        }
      }
      fprintf(f, "]\n");
    }
  }
}
#endif


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

extern int
onig_compile(regex_t* reg, const UChar* pattern, const UChar* pattern_end,
             OnigErrorInfo* einfo)
{
  int r;
  Node*  root;
  ScanEnv  scan_env;
#ifdef USE_CALL
  UnsetAddrList  uslist = {0};
#endif

  root = 0;
  if (IS_NOT_NULL(einfo)) {
    einfo->enc = reg->enc;
    einfo->par = (UChar* )NULL;
  }

#ifdef ONIG_DEBUG
  print_enc_string(stderr, reg->enc, pattern, pattern_end);
#endif

  if (reg->ops_alloc == 0) {
    r = ops_init(reg, OPS_INIT_SIZE);
    if (r != 0) goto end;
  }
  else
    reg->ops_used = 0;

  reg->string_pool        = 0;
  reg->string_pool_end    = 0;
  reg->num_mem            = 0;
  reg->num_repeat         = 0;
  reg->num_empty_check    = 0;
  reg->repeat_range_alloc = 0;
  reg->repeat_range       = (RepeatRange* )NULL;
  reg->empty_status_mem   = 0;

  r = onig_parse_tree(&root, pattern, pattern_end, reg, &scan_env);
  if (r != 0) goto err;

  r = reduce_string_list(root);
  if (r != 0) goto err;

  /* mixed use named group and no-named group */
  if (scan_env.num_named > 0 &&
      IS_SYNTAX_BV(scan_env.syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
      ! ONIG_IS_OPTION_ON(reg->options, ONIG_OPTION_CAPTURE_GROUP)) {
    if (scan_env.num_named != scan_env.num_mem)
      r = disable_noname_group_capture(&root, reg, &scan_env);
    else
      r = numbered_ref_check(root);

    if (r != 0) goto err;
  }

  r = check_backrefs(root, &scan_env);
  if (r != 0) goto err;

#ifdef USE_CALL
  if (scan_env.num_call > 0) {
    r = unset_addr_list_init(&uslist, scan_env.num_call);
    if (r != 0) goto err;
    scan_env.unset_addr_list = &uslist;
    r = tune_call(root, &scan_env, 0);
    if (r != 0) goto err_unset;
    r = tune_call2(root);
    if (r != 0) goto err_unset;
    r = recursive_call_check_trav(root, &scan_env, 0);
    if (r  < 0) goto err_unset;
    r = infinite_recursive_call_check_trav(root, &scan_env);
    if (r != 0) goto err_unset;

    tune_called_state(root, 0);
  }

  reg->num_call = scan_env.num_call;
#endif

#ifdef ONIG_DEBUG_PARSE
  fprintf(stderr, "MAX PARSE DEPTH: %d\n", scan_env.max_parse_depth);
  fprintf(stderr, "TREE (parsed)\n");
  print_tree(stderr, root);
  fprintf(stderr, "\n");
#endif

  r = tune_tree(root, reg, 0, &scan_env);
  if (r != 0) goto err_unset;

  if (scan_env.backref_num != 0) {
    set_parent_node_trav(root, NULL_NODE);
    r = set_empty_repeat_node_trav(root, NULL_NODE, &scan_env);
    if (r != 0) goto err_unset;
    set_empty_status_check_trav(root, &scan_env);
  }

#ifdef ONIG_DEBUG_PARSE
  fprintf(stderr, "TREE (after tune)\n");
  print_tree(stderr, root);
  fprintf(stderr, "\n");
#endif

  reg->capture_history  = scan_env.cap_history;
  reg->push_mem_start   = scan_env.backtrack_mem | scan_env.cap_history;

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
  if (r != 0) goto err_unset;
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

    set_addr_in_repeat_range(reg);

    if ((reg->push_mem_end != 0)
#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR
        || (reg->num_repeat      != 0)
        || (reg->num_empty_check != 0)
#endif
#ifdef USE_CALLOUT
        || (IS_NOT_NULL(reg->extp) && reg->extp->callout_num != 0)
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
  onig_print_names(stderr, reg);
  onig_print_compiled_byte_code_list(stderr, reg);
#endif

#ifdef USE_DIRECT_THREADED_CODE
  /* opcode -> opaddr */
  onig_init_for_match_at(reg);
#endif

 end:
  return r;

 err_unset:
#ifdef USE_CALL
  if (scan_env.num_call > 0) {
    unset_addr_list_end(&uslist);
  }
#endif
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
    return ONIGERR_DEFAULT_ENCODING_IS_NOT_SETTED;

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

  (reg)->enc              = enc;
  (reg)->options          = option;
  (reg)->syntax           = syntax;
  (reg)->optimize         = 0;
  (reg)->exact            = (UChar* )NULL;
  (reg)->extp             = (RegexExt* )NULL;

  (reg)->ops              = (Operation* )NULL;
  (reg)->ops_curr         = (Operation* )NULL;
  (reg)->ops_used         = 0;
  (reg)->ops_alloc        = 0;
  (reg)->name_table       = (void* )NULL;

  (reg)->case_fold_flag   = case_fold_flag;
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
  int i;
  NodeType type;
  UChar* p;
  int add = 3;

  Indent(f, indent);
  if (IS_NULL(node)) {
    fprintf(f, "ERROR: null node!!!\n");
    exit(0);
  }

  type = NODE_TYPE(node);
  switch (type) {
  case NODE_LIST:
  case NODE_ALT:
    if (type == NODE_LIST)
      fprintf(f, "<list:%p>\n", node);
    else
      fprintf(f, "<alt:%p>\n", node);

    print_indent_tree(f, NODE_CAR(node), indent + add);
    while (IS_NOT_NULL(node = NODE_CDR(node))) {
      if (NODE_TYPE(node) != type) {
        fprintf(f, "ERROR: list/alt right is not a cons. %d\n", NODE_TYPE(node));
        exit(0);
      }
      print_indent_tree(f, NODE_CAR(node), indent + add);
    }
    break;

  case NODE_STRING:
    {
      char* str;
      char* mode;

      if (NODE_STRING_IS_CRUDE(node))
        mode = "-crude";
      else if (NODE_STRING_IS_CASE_FOLD_MATCH(node))
        mode = "-case_fold_match";
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

  case NODE_CCLASS:
    fprintf(f, "<cclass:%p>", node);
    if (IS_NCCLASS_NOT(CCLASS_(node))) fputs(" not", f);
    if (CCLASS_(node)->mbuf) {
      BBuf* bbuf = CCLASS_(node)->mbuf;
      for (i = 0; i < bbuf->used; i++) {
        if (i > 0) fprintf(f, ",");
        fprintf(f, "%0x", bbuf->p[i]);
      }
    }
    break;

  case NODE_CTYPE:
    fprintf(f, "<ctype:%p> ", node);
    switch (CTYPE_(node)->ctype) {
    case CTYPE_ANYCHAR:
      fprintf(f, "<anychar:%p>", node);
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

  case NODE_ANCHOR:
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
      print_indent_tree(f, NODE_BODY(node), indent + add);
      break;
    case ANCR_PREC_READ_NOT:
      fprintf(f, "prec read not\n");
      print_indent_tree(f, NODE_BODY(node), indent + add);
      break;
    case ANCR_LOOK_BEHIND:
      fprintf(f, "look behind\n");
      print_indent_tree(f, NODE_BODY(node), indent + add);
      break;
    case ANCR_LOOK_BEHIND_NOT:
      fprintf(f, "look behind not\n");
      print_indent_tree(f, NODE_BODY(node), indent + add);
      break;

    default:
      fprintf(f, "ERROR: undefined anchor type.\n");
      break;
    }
    break;

  case NODE_BACKREF:
    {
      int* p;
      BackRefNode* br = BACKREF_(node);
      p = BACKREFS_P(br);
      fprintf(f, "<backref%s:%p>", NODE_IS_CHECKER(node) ? "-checker" : "", node);
      for (i = 0; i < br->back_num; i++) {
        if (i > 0) fputs(", ", f);
        fprintf(f, "%d", p[i]);
      }
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    {
      CallNode* cn = CALL_(node);
      fprintf(f, "<call:%p>", node);
      p_string(f, cn->name_end - cn->name, cn->name);
    }
    break;
#endif

  case NODE_QUANT:
    fprintf(f, "<quantifier:%p>{%d,%d}%s\n", node,
            QUANT_(node)->lower, QUANT_(node)->upper,
            (QUANT_(node)->greedy ? "" : "?"));
    print_indent_tree(f, NODE_BODY(node), indent + add);
    break;

  case NODE_BAG:
    fprintf(f, "<bag:%p> ", node);
    if (BAG_(node)->type == BAG_IF_ELSE) {
      Node* Then;
      Node* Else;
      BagNode* bn;

      bn = BAG_(node);
      fprintf(f, "if-else\n");
      print_indent_tree(f, NODE_BODY(node), indent + add);

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

      break;
    }

    switch (BAG_(node)->type) {
    case BAG_OPTION:
      fprintf(f, "option:%d", BAG_(node)->o.options);
      break;
    case BAG_MEMORY:
      fprintf(f, "memory:%d", BAG_(node)->m.regnum);
      break;
    case BAG_STOP_BACKTRACK:
      fprintf(f, "stop-bt");
      break;
    default:
      break;
    }
    fprintf(f, "\n");
    print_indent_tree(f, NODE_BODY(node), indent + add);
    break;

  case NODE_GIMMICK:
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
    fprintf(f, "print_indent_tree: undefined node type %d\n", NODE_TYPE(node));
    break;
  }

  if (type != NODE_LIST && type != NODE_ALT && type != NODE_QUANT &&
      type != NODE_BAG)
    fprintf(f, "\n");
  fflush(f);
}

static void
print_tree(FILE* f, Node* node)
{
  print_indent_tree(f, node, 0);
}
#endif
