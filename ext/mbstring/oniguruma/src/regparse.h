#ifndef REGPARSE_H
#define REGPARSE_H
/**********************************************************************
  regparse.h -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2022  K.Kosako
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

#define ND_STRING_MARGIN     16
#define ND_STRING_BUF_SIZE   24  /* sizeof(CClassNode) - sizeof(int)*4 */
#define ND_BACKREFS_SIZE      6

/* node type */
typedef enum {
  ND_STRING  =  0,
  ND_CCLASS  =  1,
  ND_CTYPE   =  2,
  ND_BACKREF =  3,
  ND_QUANT   =  4,
  ND_BAG     =  5,
  ND_ANCHOR  =  6,
  ND_LIST    =  7,
  ND_ALT     =  8,
  ND_CALL    =  9,
  ND_GIMMICK = 10
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
  BODY_IS_NOT_EMPTY     = 0,
  BODY_MAY_BE_EMPTY     = 1,
  BODY_MAY_BE_EMPTY_MEM = 2,
  BODY_MAY_BE_EMPTY_REC = 3
};

/* bytes buffer */
typedef struct _BBuf {
  UChar* p;
  unsigned int used;
  unsigned int alloc;
} BBuf;


struct _Node;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;

  UChar* s;
  UChar* end;
  unsigned int flag;
  UChar  buf[ND_STRING_BUF_SIZE];
  int    capacity;  /* (allocated size - 1) or 0: use buf[] */
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
  int include_referred;  /* include called node. don't eliminate even if {0} */
  MemStatusType empty_status_mem;
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
  OnigLen min_char_len;
  OnigLen max_char_len;
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
  int     called_gnum;
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
  int  back_static[ND_BACKREFS_SIZE];
  int* back_dynamic;
  int  nest_level;
} BackRefNode;

typedef struct {
  NodeType node_type;
  int status;
  struct _Node* parent;
  struct _Node* body;

  int type;
  OnigLen char_min_len;
  OnigLen char_max_len;
  int ascii_mode;
  struct _Node* lead_node;
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

typedef struct {
  int new_val;
} GroupNumMap;


#define NULL_NODE  ((Node* )0)


/* node type bit */
#define ND_TYPE2BIT(type)      (1<<(type))

#define ND_BIT_STRING     ND_TYPE2BIT(ND_STRING)
#define ND_BIT_CCLASS     ND_TYPE2BIT(ND_CCLASS)
#define ND_BIT_CTYPE      ND_TYPE2BIT(ND_CTYPE)
#define ND_BIT_BACKREF    ND_TYPE2BIT(ND_BACKREF)
#define ND_BIT_QUANT      ND_TYPE2BIT(ND_QUANT)
#define ND_BIT_BAG        ND_TYPE2BIT(ND_BAG)
#define ND_BIT_ANCHOR     ND_TYPE2BIT(ND_ANCHOR)
#define ND_BIT_LIST       ND_TYPE2BIT(ND_LIST)
#define ND_BIT_ALT        ND_TYPE2BIT(ND_ALT)
#define ND_BIT_CALL       ND_TYPE2BIT(ND_CALL)
#define ND_BIT_GIMMICK    ND_TYPE2BIT(ND_GIMMICK)

#define ND_TYPE(node)             ((node)->u.base.node_type)
#define ND_SET_TYPE(node, ntype)   (node)->u.base.node_type = (ntype)

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

#define ND_CAR(node)     (CONS_(node)->car)
#define ND_CDR(node)     (CONS_(node)->cdr)

#define CTYPE_ANYCHAR      -1
#define ND_IS_ANYCHAR(node) \
  (ND_TYPE(node) == ND_CTYPE && CTYPE_(node)->ctype == CTYPE_ANYCHAR)


#define ANCR_ANYCHAR_INF_MASK  (ANCR_ANYCHAR_INF | ANCR_ANYCHAR_INF_ML)
#define ANCR_END_BUF_MASK      (ANCR_END_BUF | ANCR_SEMI_END_BUF)

#define ND_STRING_CRUDE           (1<<0)
#define ND_STRING_CASE_EXPANDED   (1<<1)

#define ND_STRING_LEN(node)            (int )((node)->u.str.end - (node)->u.str.s)
#define ND_STRING_SET_CRUDE(node)         (node)->u.str.flag |= ND_STRING_CRUDE
#define ND_STRING_CLEAR_CRUDE(node)       (node)->u.str.flag &= ~ND_STRING_CRUDE
#define ND_STRING_SET_CASE_EXPANDED(node) (node)->u.str.flag |= ND_STRING_CASE_EXPANDED
#define ND_STRING_IS_CRUDE(node) \
  (((node)->u.str.flag & ND_STRING_CRUDE) != 0)
#define ND_STRING_IS_CASE_EXPANDED(node) \
  (((node)->u.str.flag & ND_STRING_CASE_EXPANDED) != 0)

#define BACKREFS_P(br) \
  (IS_NOT_NULL((br)->back_dynamic) ? (br)->back_dynamic : (br)->back_static)

/* node status bits */
#define ND_ST_FIXED_MIN           (1<<0)
#define ND_ST_FIXED_MAX           (1<<1)
#define ND_ST_FIXED_CLEN          (1<<2)
#define ND_ST_MARK1               (1<<3)
#define ND_ST_MARK2               (1<<4)
#define ND_ST_STRICT_REAL_REPEAT  (1<<5)
#define ND_ST_RECURSION           (1<<6)
#define ND_ST_CALLED              (1<<7)
#define ND_ST_FIXED_ADDR          (1<<8)
#define ND_ST_NAMED_GROUP         (1<<9)
#define ND_ST_IN_REAL_REPEAT      (1<<10) /* STK_REPEAT is nested in stack. */
#define ND_ST_IN_ZERO_REPEAT      (1<<11) /* (....){0} */
#define ND_ST_IN_MULTI_ENTRY      (1<<12)
#define ND_ST_NEST_LEVEL          (1<<13)
#define ND_ST_BY_NUMBER           (1<<14) /* {n,m} */
#define ND_ST_BY_NAME             (1<<15) /* backref by name */
#define ND_ST_BACKREF             (1<<16)
#define ND_ST_CHECKER             (1<<17)
#define ND_ST_PROHIBIT_RECURSION  (1<<18)
#define ND_ST_SUPER               (1<<19)
#define ND_ST_EMPTY_STATUS_CHECK  (1<<20)
#define ND_ST_IGNORECASE          (1<<21)
#define ND_ST_MULTILINE           (1<<22)
#define ND_ST_TEXT_SEGMENT_WORD   (1<<23)
#define ND_ST_ABSENT_WITH_SIDE_EFFECTS (1<<24)  /* stopper or clear */
#define ND_ST_FIXED_CLEN_MIN_SURE (1<<25)
#define ND_ST_REFERENCED          (1<<26)
#define ND_ST_INPEEK              (1<<27)
#define ND_ST_WHOLE_OPTIONS       (1<<28)


#define ND_STATUS(node)           (((Node* )node)->u.base.status)
#define ND_STATUS_ADD(node,f)     (ND_STATUS(node) |= (ND_ST_ ## f))
#define ND_STATUS_REMOVE(node,f)  (ND_STATUS(node) &= ~(ND_ST_ ## f))

#define ND_IS_BY_NUMBER(node)       ((ND_STATUS(node) & ND_ST_BY_NUMBER)      != 0)
#define ND_IS_IN_REAL_REPEAT(node)  ((ND_STATUS(node) & ND_ST_IN_REAL_REPEAT) != 0)
#define ND_IS_CALLED(node)          ((ND_STATUS(node) & ND_ST_CALLED)         != 0)
#define ND_IS_IN_MULTI_ENTRY(node)  ((ND_STATUS(node) & ND_ST_IN_MULTI_ENTRY) != 0)
#define ND_IS_RECURSION(node)       ((ND_STATUS(node) & ND_ST_RECURSION)      != 0)
#define ND_IS_IN_ZERO_REPEAT(node)  ((ND_STATUS(node) & ND_ST_IN_ZERO_REPEAT) != 0)
#define ND_IS_NAMED_GROUP(node)     ((ND_STATUS(node) & ND_ST_NAMED_GROUP)  != 0)
#define ND_IS_FIXED_ADDR(node)      ((ND_STATUS(node) & ND_ST_FIXED_ADDR)   != 0)
#define ND_IS_FIXED_CLEN(node)      ((ND_STATUS(node) & ND_ST_FIXED_CLEN)   != 0)
#define ND_IS_FIXED_MIN(node)       ((ND_STATUS(node) & ND_ST_FIXED_MIN)    != 0)
#define ND_IS_FIXED_MAX(node)       ((ND_STATUS(node) & ND_ST_FIXED_MAX)    != 0)
#define ND_IS_MARK1(node)           ((ND_STATUS(node) & ND_ST_MARK1)        != 0)
#define ND_IS_MARK2(node)           ((ND_STATUS(node) & ND_ST_MARK2)        != 0)
#define ND_IS_NEST_LEVEL(node)      ((ND_STATUS(node) & ND_ST_NEST_LEVEL)   != 0)
#define ND_IS_BY_NAME(node)         ((ND_STATUS(node) & ND_ST_BY_NAME)      != 0)
#define ND_IS_BACKREF(node)         ((ND_STATUS(node) & ND_ST_BACKREF)      != 0)
#define ND_IS_CHECKER(node)         ((ND_STATUS(node) & ND_ST_CHECKER)      != 0)
#define ND_IS_SUPER(node)           ((ND_STATUS(node) & ND_ST_SUPER)        != 0)
#define ND_IS_PROHIBIT_RECURSION(node) \
    ((ND_STATUS(node) & ND_ST_PROHIBIT_RECURSION) != 0)
#define ND_IS_STRICT_REAL_REPEAT(node) \
    ((ND_STATUS(node) & ND_ST_STRICT_REAL_REPEAT) != 0)
#define ND_IS_EMPTY_STATUS_CHECK(node) \
    ((ND_STATUS(node) & ND_ST_EMPTY_STATUS_CHECK) != 0)
#define ND_IS_IGNORECASE(node)      ((ND_STATUS(node) & ND_ST_IGNORECASE) != 0)
#define ND_IS_MULTILINE(node)       ((ND_STATUS(node) & ND_ST_MULTILINE) != 0)
#define ND_IS_TEXT_SEGMENT_WORD(node)  ((ND_STATUS(node) & ND_ST_TEXT_SEGMENT_WORD) != 0)
#define ND_IS_ABSENT_WITH_SIDE_EFFECTS(node)  ((ND_STATUS(node) & ND_ST_ABSENT_WITH_SIDE_EFFECTS) != 0)
#define ND_IS_FIXED_CLEN_MIN_SURE(node)  ((ND_STATUS(node) & ND_ST_FIXED_CLEN_MIN_SURE) != 0)
#define ND_IS_REFERENCED(node)      ((ND_STATUS(node) & ND_ST_REFERENCED) != 0)
#define ND_IS_INPEEK(node)          ((ND_STATUS(node) & ND_ST_INPEEK) != 0)
#define ND_IS_WHOLE_OPTIONS(node)   ((ND_STATUS(node) & ND_ST_WHOLE_OPTIONS) != 0)

#define ND_PARENT(node)         ((node)->u.base.parent)
#define ND_BODY(node)           ((node)->u.base.body)
#define ND_QUANT_BODY(node)     ((node)->body)
#define ND_BAG_BODY(node)       ((node)->body)
#define ND_CALL_BODY(node)      ((node)->body)
#define ND_ANCHOR_BODY(node)    ((node)->body)

#define PARSEENV_MEMENV_SIZE  8
#define PARSEENV_MEMENV(senv) \
 (IS_NOT_NULL((senv)->mem_env_dynamic) ? \
    (senv)->mem_env_dynamic : (senv)->mem_env_static)

#define IS_SYNTAX_OP(syn, opm)    (((syn)->op  & (opm)) != 0)
#define IS_SYNTAX_OP2(syn, opm)   (((syn)->op2 & (opm)) != 0)
#define IS_SYNTAX_BV(syn, bvm)    (((syn)->behavior & (bvm)) != 0)

#define ID_ENTRY(env, id) do {\
  id = (env)->id_num++;\
} while(0)


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
  int              num_mem;
  int              num_named;
  int              mem_alloc;
  MemEnv           mem_env_static[PARSEENV_MEMENV_SIZE];
  MemEnv*          mem_env_dynamic;
  int              backref_num;
  int              keep_num;
  int              id_num;
  int              save_alloc_num;
  SaveItem*        saves;
#ifdef USE_CALL
  UnsetAddrList*   unset_addr_list;
#endif
  unsigned int     parse_depth;
#ifdef ONIG_DEBUG_PARSE
  unsigned int     max_parse_depth;
#endif
  unsigned int     flags;
} ParseEnv;

#define PE_FLAG_HAS_CALL_ZERO      (1<<0)
#define PE_FLAG_HAS_WHOLE_OPTIONS  (1<<1)
#define PE_FLAG_HAS_ABSENT_STOPPER (1<<2)


extern int    onig_renumber_name_table P_((regex_t* reg, GroupNumMap* map));

extern int    onig_strncmp P_((const UChar* s1, const UChar* s2, int n));
extern void   onig_strcpy P_((UChar* dest, const UChar* src, const UChar* end));
extern void   onig_scan_env_set_error_string P_((ParseEnv* env, int ecode, UChar* arg, UChar* arg_end));
extern int    onig_reduce_nested_quantifier P_((Node* pnode));
extern int    onig_node_copy(Node** rcopy, Node* from);
extern int    onig_node_str_cat P_((Node* node, const UChar* s, const UChar* end));
extern int    onig_node_str_set P_((Node* node, const UChar* s, const UChar* end, int need_free));
extern void   onig_node_str_clear P_((Node* node, int need_free));
extern void   onig_node_free P_((Node* node));
extern int    onig_node_reset_empty P_((Node* node));
extern int    onig_node_reset_fail P_((Node* node));
extern Node*  onig_node_new_bag P_((enum BagType type));
extern Node*  onig_node_new_str P_((const UChar* s, const UChar* end));
extern Node*  onig_node_new_list P_((Node* left, Node* right));
extern Node*  onig_node_new_alt P_((Node* left, Node* right));
extern int    onig_names_free P_((regex_t* reg));
extern int    onig_parse_tree P_((Node** root, const UChar* pattern, const UChar* end, regex_t* reg, ParseEnv* env));
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
