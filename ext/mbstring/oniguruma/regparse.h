/**********************************************************************

  regparse.h -  Oniguruma (regular expression library)

  Copyright (C) 2003  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#ifndef REGPARSE_H
#define REGPARSE_H

#include "regint.h"

/* node type */
#define N_STRING       (1<< 0)
#define N_CCLASS       (1<< 1)
#define N_CTYPE        (1<< 2)
#define N_ANYCHAR      (1<< 3)
#define N_BACKREF      (1<< 4)
#define N_QUALIFIER    (1<< 5)
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
#define NQUALIFIER(node)   ((node)->u.qualifier)
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


#define ANCHOR_ANYCHAR_STAR_MASK (ANCHOR_ANYCHAR_STAR | ANCHOR_ANYCHAR_STAR_PL)
#define ANCHOR_END_BUF_MASK      (ANCHOR_END_BUF | ANCHOR_SEMI_END_BUF)

#define EFFECT_MEMORY           (1<<0)
#define EFFECT_OPTION           (1<<1)
#define EFFECT_STOP_BACKTRACK   (1<<2)

#define REPEAT_INFINITE         -1
#define IS_REPEAT_INFINITE(n)   ((n) == REPEAT_INFINITE)

#define NODE_STR_MARGIN         16
#define NODE_STR_BUF_SIZE       24  /* sizeof(CClassNode) - sizeof(int)*4 */
#define NODE_BACKREFS_SIZE       7

#define NSTR_RAW                (1<<0) /* by backslashed number */
#define NSTR_CASE_AMBIG         (1<<1)

#define NSTRING_LEN(node)            ((node)->u.str.end - (node)->u.str.s)
#define NSTRING_SET_RAW(node)        (node)->u.str.flag |= NSTR_RAW
#define NSTRING_SET_CASE_AMBIG(node) (node)->u.str.flag |= NSTR_CASE_AMBIG
#define NSTRING_IS_RAW(node)         (((node)->u.str.flag & NSTR_RAW) != 0)
#define NSTRING_IS_CASE_AMBIG(node)  \
       (((node)->u.str.flag & NSTR_CASE_AMBIG) != 0)

#define BACKREFS_P(br) \
  (IS_NOT_NULL((br)->back_dynamic) ? (br)->back_dynamic : (br)->back_static);

typedef struct {
  UChar* s;
  UChar* end;
  unsigned int flag;
  int    capa;    /* (allocated size - 1) or 0: use buf[] */
  UChar  buf[NODE_STR_BUF_SIZE];
} StrNode;

typedef struct {
  int    not;
  BitSet bs;
  BBuf*  mbuf;     /* multi-byte info or NULL */
} CClassNode;

typedef struct {
  struct _Node* target;
  int lower;
  int upper;
  int greedy;
  int by_number;         /* {n,m} */
  int target_may_empty;  /* target can match with empty data */
  struct _Node* head_exact;
  struct _Node* next_head_exact;
  int is_refered;     /* include called node. don't eliminate even if {0} */
} QualifierNode;

/* status bits */
#define NST_RECURSION       (1<<0)
#define NST_CALLED          (1<<1)
#define NST_ADDR_FIXED      (1<<2)
#define NST_MIN_FIXED       (1<<3)
#define NST_MAX_FIXED       (1<<4)
#define NST_CLEN_FIXED      (1<<5)
#define NST_MARK1           (1<<6)
#define NST_MARK2           (1<<7)
#define NST_MEM_BACKREFED   (1<<8)
#define NST_SIMPLE_REPEAT   (1<<9)  /* for stop backtrack optimization */

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
#define IS_EFFECT_SIMPLE_REPEAT(en)    (((en)->state & NST_SIMPLE_REPEAT) != 0)

#define SET_CALL_RECURSION(node)       (node)->u.call.state |= NST_RECURSION
#define IS_CALL_RECURSION(cn)          (((cn)->state & NST_RECURSION)  != 0)

typedef struct {
  int state;
  int type;
  int regnum;
  RegOptionType option;
  struct _Node* target;
  AbsAddrType call_addr;
  /* for multiple call reference */
  RegDistance min_len; /* min length (byte) */
  RegDistance max_len; /* max length (byte) */ 
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
} BackrefNode;

typedef struct {
  int type;
  struct _Node* target;
  int char_len;
} AnchorNode;

typedef struct _Node {
  int type;
  union {
    StrNode       str;
    CClassNode    cclass;
    QualifierNode qualifier;
    EffectNode    effect;
#ifdef USE_SUBEXP_CALL
    CallNode      call;
#endif
    BackrefNode   backref;
    AnchorNode    anchor;
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
  RegOptionType   option;
  RegCharEncoding enc;
  RegSyntaxType*  syntax;
  BitStatusType   backtrack_mem;
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
  int             mem_alloc;
  Node*           mem_nodes_static[SCANENV_MEMNODES_SIZE];
  Node**          mem_nodes_dynamic;
} ScanEnv;


#define IS_SYNTAX_OP(syn, opm)    (((syn)->op  & (opm)) != 0)
#define IS_SYNTAX_OP2(syn, opm)   (((syn)->op2 & (opm)) != 0)
#define IS_SYNTAX_BV(syn, bvm)    (((syn)->behavior & (bvm)) != 0)


extern void   regex_scan_env_set_error_string P_((ScanEnv* env, int ecode, UChar* arg, UChar* arg_end));
extern int    regex_scan_unsigned_number P_((UChar** src, UChar* end, RegCharEncoding enc));
extern void   regex_node_conv_to_str_node P_((Node* node, int raw));
extern int    regex_node_str_cat P_((Node* node, UChar* s, UChar* end));
extern void   regex_node_free P_((Node* node));
extern Node*  regex_node_new_effect P_((int type));
extern Node*  regex_node_new_anchor P_((int type));
extern int    regex_free_node_list();
extern int    regex_names_free P_((regex_t* reg));
extern int    regex_parse_make_tree P_((Node** root, UChar* pattern, UChar* end, regex_t* reg, ScanEnv* env));

#ifdef REG_DEBUG
#ifdef USE_NAMED_SUBEXP
extern int regex_print_names(FILE*, regex_t*);
#endif
#endif

#endif /* REGPARSE_H */
