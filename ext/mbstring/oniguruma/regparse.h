#ifndef REGPARSE_H
#define REGPARSE_H
/**********************************************************************
  regparse.h -  Oniguruma (regular expression library)
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

#include "regint.h"

/* node type */
#define N_STRING       (1<< 0)
#define N_CCLASS       (1<< 1)
#define N_CTYPE        (1<< 2)
#define N_ANYCHAR      (1<< 3)
#define N_BACKREF      (1<< 4)
#define N_QUANTIFIER   (1<< 5)
#define N_EFFECT       (1<< 6)
#define N_ANCHOR       (1<< 7)
#define N_LIST         (1<< 8)
#define N_ALT          (1<< 9)
#define N_CALL         (1<<10)

#define IS_NODE_TYPE_SIMPLE(type) \
  (((type) & (N_STRING | N_CCLASS | N_CTYPE | N_ANYCHAR | N_BACKREF)) != 0)

#define NTYPE(node)        ((node)->type)
#define NCONS(node)        ((node)->u.cons)
#define NSTRING(node)      ((node)->u.str)
#define NCCLASS(node)      ((node)->u.cclass)
#define NCTYPE(node)       ((node)->u.ctype)
#define NQUANTIFIER(node)  ((node)->u.quantifier)
#define NANCHOR(node)      ((node)->u.anchor)
#define NBACKREF(node)     ((node)->u.backref)
#define NEFFECT(node)      ((node)->u.effect)
#define NCALL(node)        ((node)->u.call)

#define CTYPE_WORD              (1<<0)
#define CTYPE_NOT_WORD          (1<<1)
#define CTYPE_WHITE_SPACE       (1<<2)
#define CTYPE_NOT_WHITE_SPACE   (1<<3)
#define CTYPE_DIGIT             (1<<4)
#define CTYPE_NOT_DIGIT         (1<<5)
#define CTYPE_XDIGIT            (1<<6)
#define CTYPE_NOT_XDIGIT        (1<<7)

#define ANCHOR_ANYCHAR_STAR_MASK (ANCHOR_ANYCHAR_STAR | ANCHOR_ANYCHAR_STAR_ML)
#define ANCHOR_END_BUF_MASK      (ANCHOR_END_BUF | ANCHOR_SEMI_END_BUF)

#define EFFECT_MEMORY           (1<<0)
#define EFFECT_OPTION           (1<<1)
#define EFFECT_STOP_BACKTRACK   (1<<2)

#define NODE_STR_MARGIN         16
#define NODE_STR_BUF_SIZE       24  /* sizeof(CClassNode) - sizeof(int)*4 */
#define NODE_BACKREFS_SIZE       6

#define NSTR_RAW                (1<<0) /* by backslashed number */
#define NSTR_AMBIG              (1<<1)
#define NSTR_AMBIG_REDUCE       (1<<2)

#define NSTRING_LEN(node)             ((node)->u.str.end - (node)->u.str.s)
#define NSTRING_SET_RAW(node)          (node)->u.str.flag |= NSTR_RAW
#define NSTRING_CLEAR_RAW(node)        (node)->u.str.flag &= ~NSTR_RAW
#define NSTRING_SET_AMBIG(node)        (node)->u.str.flag |= NSTR_AMBIG
#define NSTRING_SET_AMBIG_REDUCE(node) (node)->u.str.flag |= NSTR_AMBIG_REDUCE
#define NSTRING_IS_RAW(node)          (((node)->u.str.flag & NSTR_RAW)   != 0)
#define NSTRING_IS_AMBIG(node)        (((node)->u.str.flag & NSTR_AMBIG) != 0)
#define NSTRING_IS_AMBIG_REDUCE(node) \
  (((node)->u.str.flag & NSTR_AMBIG_REDUCE) != 0)

#define BACKREFS_P(br) \
  (IS_NOT_NULL((br)->back_dynamic) ? (br)->back_dynamic : (br)->back_static);

#define NQ_TARGET_ISNOT_EMPTY     0
#define NQ_TARGET_IS_EMPTY        1
#define NQ_TARGET_IS_EMPTY_MEM    2
#define NQ_TARGET_IS_EMPTY_REC    3


typedef struct {
  UChar* s;
  UChar* end;
  unsigned int flag;
  int    capa;    /* (allocated size - 1) or 0: use buf[] */
  UChar  buf[NODE_STR_BUF_SIZE];
} StrNode;

/* move to regint.h */
#if 0
typedef struct {
  int    flags;
  BitSet bs;
  BBuf*  mbuf;     /* multi-byte info or NULL */
} CClassNode;
#endif

typedef struct {
  int state;
  struct _Node* target;
  int lower;
  int upper;
  int greedy;
  int target_empty_info;
  struct _Node* head_exact;
  struct _Node* next_head_exact;
  int is_refered;     /* include called node. don't eliminate even if {0} */
#ifdef USE_COMBINATION_EXPLOSION_CHECK
  int comb_exp_check_num;  /* 1,2,3...: check,  0: no check  */
#endif
} QuantifierNode;

/* status bits */
#define NST_MIN_FIXED             (1<<0)
#define NST_MAX_FIXED             (1<<1)
#define NST_CLEN_FIXED            (1<<2)
#define NST_MARK1                 (1<<3)
#define NST_MARK2                 (1<<4)
#define NST_MEM_BACKREFED         (1<<5)
#define NST_STOP_BT_SIMPLE_REPEAT (1<<6)
#define NST_RECURSION             (1<<7)
#define NST_CALLED                (1<<8)
#define NST_ADDR_FIXED            (1<<9)
#define NST_NAMED_GROUP           (1<<10)
#define NST_NAME_REF              (1<<11)
#define NST_IN_REPEAT             (1<<12) /* STK_REPEAT is nested in stack. */
#define NST_NEST_LEVEL            (1<<13)
#define NST_BY_NUMBER             (1<<14) /* {n,m} */

#define SET_EFFECT_STATUS(node,f)      (node)->u.effect.state |=  (f)
#define CLEAR_EFFECT_STATUS(node,f)    (node)->u.effect.state &= ~(f)

#define IS_EFFECT_CALLED(en)           (((en)->state & NST_CALLED)        != 0)
#define IS_EFFECT_ADDR_FIXED(en)       (((en)->state & NST_ADDR_FIXED)    != 0)
#define IS_EFFECT_RECURSION(en)        (((en)->state & NST_RECURSION)     != 0)
#define IS_EFFECT_MARK1(en)            (((en)->state & NST_MARK1)         != 0)
#define IS_EFFECT_MARK2(en)            (((en)->state & NST_MARK2)         != 0)
#define IS_EFFECT_MIN_FIXED(en)        (((en)->state & NST_MIN_FIXED)     != 0)
#define IS_EFFECT_MAX_FIXED(en)        (((en)->state & NST_MAX_FIXED)     != 0)
#define IS_EFFECT_CLEN_FIXED(en)       (((en)->state & NST_CLEN_FIXED)    != 0)
#define IS_EFFECT_STOP_BT_SIMPLE_REPEAT(en) \
    (((en)->state & NST_STOP_BT_SIMPLE_REPEAT) != 0)
#define IS_EFFECT_NAMED_GROUP(en)      (((en)->state & NST_NAMED_GROUP)   != 0)

#define SET_CALL_RECURSION(node)       (node)->u.call.state |= NST_RECURSION
#define IS_CALL_RECURSION(cn)          (((cn)->state & NST_RECURSION)  != 0)
#define IS_CALL_NAME_REF(cn)           (((cn)->state & NST_NAME_REF)   != 0)
#define IS_BACKREF_NAME_REF(bn)        (((bn)->state & NST_NAME_REF)   != 0)
#define IS_BACKREF_NEST_LEVEL(bn)      (((bn)->state & NST_NEST_LEVEL) != 0)
#define IS_QUANTIFIER_IN_REPEAT(qn)     (((qn)->state & NST_IN_REPEAT)  != 0)
#define IS_QUANTIFIER_BY_NUMBER(qn)     (((qn)->state & NST_BY_NUMBER)  != 0)

typedef struct {
  int state;
  int type;
  int regnum;
  OnigOptionType option;
  struct _Node* target;
  AbsAddrType call_addr;
  /* for multiple call reference */
  OnigDistance min_len; /* min length (byte) */
  OnigDistance max_len; /* max length (byte) */ 
  int char_len;        /* character length  */
  int opt_count;       /* referenced count in optimize_node_left() */
} EffectNode;

#define CALLNODE_REFNUM_UNDEF  -1

#ifdef USE_SUBEXP_CALL

typedef struct {
  int offset;
  struct _Node* target;
} UnsetAddr;

typedef struct {
  int num;
  int alloc;
  UnsetAddr* us;
} UnsetAddrList;

typedef struct {
  int     state;
  int     ref_num;
  UChar*  name;
  UChar*  name_end;
  struct _Node* target;  /* EffectNode : EFFECT_MEMORY */
  UnsetAddrList* unset_addr_list;
} CallNode;

#endif

typedef struct {
  int     state;
  int     back_num;
  int     back_static[NODE_BACKREFS_SIZE];
  int*    back_dynamic;
  int     nest_level;
} BackrefNode;

typedef struct {
  int type;
  struct _Node* target;
  int char_len;
} AnchorNode;

typedef struct _Node {
  int type;
  union {
    StrNode        str;
    CClassNode     cclass;
    QuantifierNode quantifier;
    EffectNode     effect;
#ifdef USE_SUBEXP_CALL
    CallNode       call;
#endif
    BackrefNode    backref;
    AnchorNode     anchor;
    struct {
      struct _Node* left;
      struct _Node* right;
    } cons;
    struct {
      int type;
    } ctype;
  } u;
} Node;

#define NULL_NODE  ((Node* )0)

#define SCANENV_MEMNODES_SIZE               8
#define SCANENV_MEM_NODES(senv)   \
 (IS_NOT_NULL((senv)->mem_nodes_dynamic) ? \
    (senv)->mem_nodes_dynamic : (senv)->mem_nodes_static)

typedef struct {
  OnigOptionType  option;
  OnigAmbigType   ambig_flag;
  OnigEncoding    enc;
  OnigSyntaxType* syntax;
  BitStatusType   capture_history;
  BitStatusType   bt_mem_start;
  BitStatusType   bt_mem_end;
  BitStatusType   backrefed_mem;
  UChar*          pattern;
  UChar*          pattern_end;
  UChar*          error;
  UChar*          error_end;
  regex_t*        reg;       /* for reg->names only */
  int             num_call;
#ifdef USE_SUBEXP_CALL
  UnsetAddrList*  unset_addr_list;
#endif
  int             num_mem;
#ifdef USE_NAMED_GROUP
  int             num_named;
#endif
  int             mem_alloc;
  Node*           mem_nodes_static[SCANENV_MEMNODES_SIZE];
  Node**          mem_nodes_dynamic;
#ifdef USE_COMBINATION_EXPLOSION_CHECK
  int num_comb_exp_check;
  int comb_exp_max_regnum;
  int curr_max_regnum;
  int has_recursion;
#endif
} ScanEnv;


#define IS_SYNTAX_OP(syn, opm)    (((syn)->op  & (opm)) != 0)
#define IS_SYNTAX_OP2(syn, opm)   (((syn)->op2 & (opm)) != 0)
#define IS_SYNTAX_BV(syn, bvm)    (((syn)->behavior & (bvm)) != 0)


#ifdef USE_NAMED_GROUP
typedef struct {
  int new_val;
} GroupNumRemap;

extern int    onig_renumber_name_table P_((regex_t* reg, GroupNumRemap* map));
#endif

extern int    onig_strncmp P_((const UChar* s1, const UChar* s2, int n));
extern void   onig_scan_env_set_error_string P_((ScanEnv* env, int ecode, UChar* arg, UChar* arg_end));
extern int    onig_scan_unsigned_number P_((UChar** src, const UChar* end, OnigEncoding enc));
extern void   onig_reduce_nested_quantifier P_((Node* pnode, Node* cnode));
extern void   onig_node_conv_to_str_node P_((Node* node, int raw));
extern int    onig_node_str_cat P_((Node* node, const UChar* s, const UChar* end));
extern void   onig_node_free P_((Node* node));
extern Node*  onig_node_new_effect P_((int type));
extern Node*  onig_node_new_anchor P_((int type));
extern Node*  onig_node_new_str P_((const UChar* s, const UChar* end));
extern Node*  onig_node_new_list P_((Node* left, Node* right));
extern void   onig_node_str_clear P_((Node* node));
extern int    onig_free_node_list P_((void));
extern int    onig_names_free P_((regex_t* reg));
extern int    onig_parse_make_tree P_((Node** root, const UChar* pattern, const UChar* end, regex_t* reg, ScanEnv* env));

#ifdef ONIG_DEBUG
#ifdef USE_NAMED_GROUP
extern int onig_print_names(FILE*, regex_t*);
#endif
#endif

#endif /* REGPARSE_H */
