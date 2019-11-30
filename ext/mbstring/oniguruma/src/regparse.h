#ifndef REGPARSE_H
#define REGPARSE_H
/**********************************************************************
  regparse.h -  Oniguruma (regular expression library)
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

#define NODE_STRING_MARGIN         16
#define NODE_STRING_BUF_SIZE       20  /* sizeof(CClassNode) - sizeof(int)*4 */
#define NODE_BACKREFS_SIZE          6

/* node type */
typedef enum {
  NODE_STRING    =  0,
  NODE_CCLASS    =  1,
  NODE_CTYPE     =  2,
  NODE_BACKREF   =  3,
  NODE_QUANT     =  4,
  NODE_BAG       =  5,
  NODE_ANCHOR    =  6,
  NODE_LIST      =  7,
  NODE_ALT       =  8,
  NODE_CALL      =  9,
  NODE_GIMMICK   = 10
} NodeType;

enum BagType {
  BAG_MEMORY         = 0,
  BAG_OPTION         = 1,
  BAG_STOP_BACKTRACK = 2,
  BAG_IF_ELSE        = 3,
};

enum GimmickType {
  GIMMICK_FAIL       = 0,
  GIMMICK_SAVE       = 1,
  GIMMICK_UPDATE_VAR = 2,
#ifdef USE_CALLOUT
  GIMMICK_CALLOUT    = 3,
#endif
};

enum BodyEmptyType {
  BODY_IS_NOT_EMPTY             = 0,
  BODY_IS_EMPTY_POSSIBILITY     = 1,
  BODY_IS_EMPTY_POSSIBILITY_MEM = 2,
  BODY_IS_EMPTY_POSSIBILITY_REC = 3
};

struct _Node;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;

  UChar* s;
  UChar* end;
  unsigned int flag;
  UChar  buf[NODE_STRING_BUF_SIZE];
  int    capacity;    /* (allocated size - 1) or 0: use buf[] */
  int    case_min_len;
} StrNode;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;

  unsigned int flags;
  BitSet bs;
  BBuf*  mbuf;   /* multi-byte info or NULL */
} CClassNode;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;
  struct _Node* body;

  int lower;
  int upper;
  int greedy;
  enum BodyEmptyType emptiness;
  struct _Node* head_exact;
  struct _Node* next_head_exact;
  int include_referred;   /* include called node. don't eliminate even if {0} */
} QuantNode;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;
  struct _Node* body;

  enum BagType type;
  union {
    struct {
      int regnum;
      AbsAddrType called_addr;
      int entry_count;
      int called_state;
    } m;
    struct {
      OnigOptionType options;
    } o;
    struct {
      /* body is condition */
      struct _Node* Then;
      struct _Node* Else;
    } te;
  };
  /* for multiple call reference */
  OnigLen min_len;   /* min length (byte) */
  OnigLen max_len;   /* max length (byte) */
  int char_len;      /* character length  */
  int opt_count;     /* referenced count in optimize_nodes() */
} BagNode;

#ifdef USE_CALL

typedef struct {
  int           offset;
  struct _Node* target;
} UnsetAddr;

typedef struct {
  int        num;
  int        alloc;
  UnsetAddr* us;
} UnsetAddrList;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;
  struct _Node* body; /* to BagNode : BAG_MEMORY */

  int     by_number;
  int     group_num;
  UChar*  name;
  UChar*  name_end;
  int     entry_count;
} CallNode;

#endif

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;

  int  back_num;
  int  back_static[NODE_BACKREFS_SIZE];
  int* back_dynamic;
  int  nest_level;
} BackRefNode;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;
  struct _Node* body;

  int type;
  int char_len;
  int ascii_mode;
} AnchorNode;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;

  struct _Node* car;
  struct _Node* cdr;
} ConsAltNode;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;

  int ctype;
  int not;
  OnigOptionType options;
  int ascii_mode;
} CtypeNode;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;

  enum GimmickType type;
  int  detail_type;
  int  num;
  int  id;
} GimmickNode;

typedef struct _Node {
  union {
    struct {
      NodeType node_type;
      int status;
      struct _Node* parent;
      struct _Node* body;
    } base;

    StrNode       str;
    CClassNode    cclass;
    QuantNode     quant;
    BagNode       bag;
    BackRefNode   backref;
    AnchorNode    anchor;
    ConsAltNode   cons;
    CtypeNode     ctype;
#ifdef USE_CALL
    CallNode      call;
#endif
    GimmickNode   gimmick;
  } u;
} Node;

#define NULL_NODE  ((Node* )0)


/* node type bit */
#define NODE_TYPE2BIT(type)      (1<<(type))

#define NODE_BIT_STRING     NODE_TYPE2BIT(NODE_STRING)
#define NODE_BIT_CCLASS     NODE_TYPE2BIT(NODE_CCLASS)
#define NODE_BIT_CTYPE      NODE_TYPE2BIT(NODE_CTYPE)
#define NODE_BIT_BACKREF    NODE_TYPE2BIT(NODE_BACKREF)
#define NODE_BIT_QUANT      NODE_TYPE2BIT(NODE_QUANT)
#define NODE_BIT_BAG        NODE_TYPE2BIT(NODE_BAG)
#define NODE_BIT_ANCHOR     NODE_TYPE2BIT(NODE_ANCHOR)
#define NODE_BIT_LIST       NODE_TYPE2BIT(NODE_LIST)
#define NODE_BIT_ALT        NODE_TYPE2BIT(NODE_ALT)
#define NODE_BIT_CALL       NODE_TYPE2BIT(NODE_CALL)
#define NODE_BIT_GIMMICK    NODE_TYPE2BIT(NODE_GIMMICK)

#define NODE_TYPE(node)             ((node)->u.base.node_type)
#define NODE_SET_TYPE(node, ntype)   (node)->u.base.node_type = (ntype)

#define STR_(node)         (&((node)->u.str))
#define CCLASS_(node)      (&((node)->u.cclass))
#define CTYPE_(node)       (&((node)->u.ctype))
#define BACKREF_(node)     (&((node)->u.backref))
#define QUANT_(node)       (&((node)->u.quant))
#define BAG_(node)         (&((node)->u.bag))
#define ANCHOR_(node)      (&((node)->u.anchor))
#define CONS_(node)        (&((node)->u.cons))
#define CALL_(node)        (&((node)->u.call))
#define GIMMICK_(node)     (&((node)->u.gimmick))

#define NODE_CAR(node)         (CONS_(node)->car)
#define NODE_CDR(node)         (CONS_(node)->cdr)

#define CTYPE_ANYCHAR      -1
#define NODE_IS_ANYCHAR(node) \
  (NODE_TYPE(node) == NODE_CTYPE && CTYPE_(node)->ctype == CTYPE_ANYCHAR)

#define CTYPE_OPTION(node, reg) \
  (NODE_IS_FIXED_OPTION(node) ? CTYPE_(node)->options : reg->options)


#define ANCR_ANYCHAR_INF_MASK  (ANCR_ANYCHAR_INF | ANCR_ANYCHAR_INF_ML)
#define ANCR_END_BUF_MASK      (ANCR_END_BUF | ANCR_SEMI_END_BUF)

#define NODE_STRING_CRUDE              (1<<0)
#define NODE_STRING_CASE_EXPANDED      (1<<1)
#define NODE_STRING_CASE_FOLD_MATCH    (1<<2)

#define NODE_STRING_LEN(node)            (int )((node)->u.str.end - (node)->u.str.s)
#define NODE_STRING_SET_CRUDE(node)         (node)->u.str.flag |= NODE_STRING_CRUDE
#define NODE_STRING_CLEAR_CRUDE(node)       (node)->u.str.flag &= ~NODE_STRING_CRUDE
#define NODE_STRING_SET_CASE_EXPANDED(node) (node)->u.str.flag |= NODE_STRING_CASE_EXPANDED
#define NODE_STRING_SET_CASE_FOLD_MATCH(node) (node)->u.str.flag |= NODE_STRING_CASE_FOLD_MATCH
#define NODE_STRING_IS_CRUDE(node) \
  (((node)->u.str.flag & NODE_STRING_CRUDE) != 0)
#define NODE_STRING_IS_CASE_EXPANDED(node) \
  (((node)->u.str.flag & NODE_STRING_CASE_EXPANDED) != 0)
#define NODE_STRING_IS_CASE_FOLD_MATCH(node) \
  (((node)->u.str.flag & NODE_STRING_CASE_FOLD_MATCH) != 0)

#define BACKREFS_P(br) \
  (IS_NOT_NULL((br)->back_dynamic) ? (br)->back_dynamic : (br)->back_static)

/* node status bits */
#define NODE_ST_MIN_FIXED             (1<<0)
#define NODE_ST_MAX_FIXED             (1<<1)
#define NODE_ST_CLEN_FIXED            (1<<2)
#define NODE_ST_MARK1                 (1<<3)
#define NODE_ST_MARK2                 (1<<4)
#define NODE_ST_STRICT_REAL_REPEAT    (1<<5)
#define NODE_ST_RECURSION             (1<<6)
#define NODE_ST_CALLED                (1<<7)
#define NODE_ST_ADDR_FIXED            (1<<8)
#define NODE_ST_NAMED_GROUP           (1<<9)
#define NODE_ST_IN_REAL_REPEAT        (1<<10) /* STK_REPEAT is nested in stack. */
#define NODE_ST_IN_ZERO_REPEAT        (1<<11) /* (....){0} */
#define NODE_ST_IN_MULTI_ENTRY        (1<<12)
#define NODE_ST_NEST_LEVEL            (1<<13)
#define NODE_ST_BY_NUMBER             (1<<14) /* {n,m} */
#define NODE_ST_BY_NAME               (1<<15) /* backref by name */
#define NODE_ST_BACKREF               (1<<16)
#define NODE_ST_CHECKER               (1<<17)
#define NODE_ST_FIXED_OPTION          (1<<18)
#define NODE_ST_PROHIBIT_RECURSION    (1<<19)
#define NODE_ST_SUPER                 (1<<20)
#define NODE_ST_EMPTY_STATUS_CHECK    (1<<21)


#define NODE_STATUS(node)           (((Node* )node)->u.base.status)
#define NODE_STATUS_ADD(node,f)     (NODE_STATUS(node) |= (NODE_ST_ ## f))
#define NODE_STATUS_REMOVE(node,f)  (NODE_STATUS(node) &= ~(NODE_ST_ ## f))

#define NODE_IS_BY_NUMBER(node)       ((NODE_STATUS(node) & NODE_ST_BY_NUMBER)      != 0)
#define NODE_IS_IN_REAL_REPEAT(node)  ((NODE_STATUS(node) & NODE_ST_IN_REAL_REPEAT) != 0)
#define NODE_IS_CALLED(node)          ((NODE_STATUS(node) & NODE_ST_CALLED)         != 0)
#define NODE_IS_IN_MULTI_ENTRY(node)  ((NODE_STATUS(node) & NODE_ST_IN_MULTI_ENTRY) != 0)
#define NODE_IS_RECURSION(node)       ((NODE_STATUS(node) & NODE_ST_RECURSION)      != 0)
#define NODE_IS_IN_ZERO_REPEAT(node)  ((NODE_STATUS(node) & NODE_ST_IN_ZERO_REPEAT) != 0)
#define NODE_IS_NAMED_GROUP(node)     ((NODE_STATUS(node) & NODE_ST_NAMED_GROUP)  != 0)
#define NODE_IS_ADDR_FIXED(node)      ((NODE_STATUS(node) & NODE_ST_ADDR_FIXED)   != 0)
#define NODE_IS_CLEN_FIXED(node)      ((NODE_STATUS(node) & NODE_ST_CLEN_FIXED)   != 0)
#define NODE_IS_MIN_FIXED(node)       ((NODE_STATUS(node) & NODE_ST_MIN_FIXED)    != 0)
#define NODE_IS_MAX_FIXED(node)       ((NODE_STATUS(node) & NODE_ST_MAX_FIXED)    != 0)
#define NODE_IS_MARK1(node)           ((NODE_STATUS(node) & NODE_ST_MARK1)        != 0)
#define NODE_IS_MARK2(node)           ((NODE_STATUS(node) & NODE_ST_MARK2)        != 0)
#define NODE_IS_NEST_LEVEL(node)      ((NODE_STATUS(node) & NODE_ST_NEST_LEVEL)   != 0)
#define NODE_IS_BY_NAME(node)         ((NODE_STATUS(node) & NODE_ST_BY_NAME)      != 0)
#define NODE_IS_BACKREF(node)         ((NODE_STATUS(node) & NODE_ST_BACKREF)      != 0)
#define NODE_IS_CHECKER(node)         ((NODE_STATUS(node) & NODE_ST_CHECKER)      != 0)
#define NODE_IS_FIXED_OPTION(node)    ((NODE_STATUS(node) & NODE_ST_FIXED_OPTION) != 0)
#define NODE_IS_SUPER(node)           ((NODE_STATUS(node) & NODE_ST_SUPER)        != 0)
#define NODE_IS_PROHIBIT_RECURSION(node) \
    ((NODE_STATUS(node) & NODE_ST_PROHIBIT_RECURSION) != 0)
#define NODE_IS_STRICT_REAL_REPEAT(node) \
    ((NODE_STATUS(node) & NODE_ST_STRICT_REAL_REPEAT) != 0)
#define NODE_IS_EMPTY_STATUS_CHECK(node) \
    ((NODE_STATUS(node) & NODE_ST_EMPTY_STATUS_CHECK) != 0)

#define NODE_PARENT(node)         ((node)->u.base.parent)
#define NODE_BODY(node)           ((node)->u.base.body)
#define NODE_QUANT_BODY(node)     ((node)->body)
#define NODE_BAG_BODY(node)       ((node)->body)
#define NODE_CALL_BODY(node)      ((node)->body)
#define NODE_ANCHOR_BODY(node)    ((node)->body)

#define SCANENV_MEMENV_SIZE               8
#define SCANENV_MEMENV(senv) \
 (IS_NOT_NULL((senv)->mem_env_dynamic) ? \
    (senv)->mem_env_dynamic : (senv)->mem_env_static)

typedef struct {
  Node* mem_node;
  Node* empty_repeat_node;
} MemEnv;

typedef struct {
  enum SaveType type;
} SaveItem;

typedef struct {
  OnigOptionType   options;
  OnigCaseFoldType case_fold_flag;
  OnigEncoding     enc;
  OnigSyntaxType*  syntax;
  MemStatusType    cap_history;
  MemStatusType    backtrack_mem; /* backtrack/recursion */
  MemStatusType    backrefed_mem;
  UChar*           pattern;
  UChar*           pattern_end;
  UChar*           error;
  UChar*           error_end;
  regex_t*         reg;       /* for reg->names only */
  int              num_call;
#ifdef USE_CALL
  UnsetAddrList*   unset_addr_list;
  int              has_call_zero;
#endif
  int              num_mem;
  int              num_named;
  int              mem_alloc;
  MemEnv           mem_env_static[SCANENV_MEMENV_SIZE];
  MemEnv*          mem_env_dynamic;
  unsigned int     parse_depth;
#ifdef ONIG_DEBUG_PARSE
  unsigned int     max_parse_depth;
#endif
  int backref_num;
  int keep_num;
  int save_num;
  int save_alloc_num;
  SaveItem* saves;
} ScanEnv;


#define IS_SYNTAX_OP(syn, opm)    (((syn)->op  & (opm)) != 0)
#define IS_SYNTAX_OP2(syn, opm)   (((syn)->op2 & (opm)) != 0)
#define IS_SYNTAX_BV(syn, bvm)    (((syn)->behavior & (bvm)) != 0)

typedef struct {
  int new_val;
} GroupNumRemap;

extern int    onig_renumber_name_table P_((regex_t* reg, GroupNumRemap* map));

extern int    onig_strncmp P_((const UChar* s1, const UChar* s2, int n));
extern void   onig_strcpy P_((UChar* dest, const UChar* src, const UChar* end));
extern void   onig_scan_env_set_error_string P_((ScanEnv* env, int ecode, UChar* arg, UChar* arg_end));
extern int    onig_reduce_nested_quantifier P_((Node* pnode));
extern int    onig_node_str_cat P_((Node* node, const UChar* s, const UChar* end));
extern int    onig_node_str_set P_((Node* node, const UChar* s, const UChar* end));
extern void   onig_node_free P_((Node* node));
extern Node*  onig_node_new_bag P_((enum BagType type));
extern Node*  onig_node_new_anchor P_((int type, int ascii_mode));
extern Node*  onig_node_new_str P_((const UChar* s, const UChar* end));
extern Node*  onig_node_new_list P_((Node* left, Node* right));
extern Node*  onig_node_new_alt P_((Node* left, Node* right));
extern void   onig_node_str_clear P_((Node* node));
extern int    onig_names_free P_((regex_t* reg));
extern int    onig_parse_tree P_((Node** root, const UChar* pattern, const UChar* end, regex_t* reg, ScanEnv* env));
extern int    onig_free_shared_cclass_table P_((void));
extern int    onig_is_code_in_cc P_((OnigEncoding enc, OnigCodePoint code, CClassNode* cc));
extern int    onig_new_cclass_with_code_list(Node** rnode, OnigEncoding enc, int n, OnigCodePoint codes[]);
extern OnigLen onig_get_tiny_min_len(Node* node, unsigned int inhibit_node_types, int* invalid_node);

#ifdef USE_CALLOUT
extern int onig_global_callout_names_free(void);
#endif

#ifdef ONIG_DEBUG
extern int onig_print_names(FILE*, regex_t*);
#endif

#endif /* REGPARSE_H */
