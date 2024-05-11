/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SELECTORS_SELECTOR_H
#define LEXBOR_CSS_SELECTORS_SELECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/str.h"
#include "lexbor/css/selectors/base.h"
#include "lexbor/css/syntax/anb.h"


typedef enum {
    LXB_CSS_SELECTOR_TYPE__UNDEF = 0x00,
    LXB_CSS_SELECTOR_TYPE_ANY,
    LXB_CSS_SELECTOR_TYPE_ELEMENT,                 /* div, tag name <div> */
    LXB_CSS_SELECTOR_TYPE_ID,                      /* #hash */
    LXB_CSS_SELECTOR_TYPE_CLASS,                   /* .class */
    LXB_CSS_SELECTOR_TYPE_ATTRIBUTE,               /* [key=val], <... key="val"> */
    LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS,            /* :pseudo */
    LXB_CSS_SELECTOR_TYPE_PSEUDO_CLASS_FUNCTION,   /* :function(...) */
    LXB_CSS_SELECTOR_TYPE_PSEUDO_ELEMENT,          /* ::pseudo */
    LXB_CSS_SELECTOR_TYPE_PSEUDO_ELEMENT_FUNCTION, /* ::function(...) */
    LXB_CSS_SELECTOR_TYPE__LAST_ENTRY
}
lxb_css_selector_type_t;

typedef enum {
    LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT = 0x00, /* WHITESPACE */
    LXB_CSS_SELECTOR_COMBINATOR_CLOSE,             /* two compound selectors [key=val].foo */
    LXB_CSS_SELECTOR_COMBINATOR_CHILD,             /* '>' */
    LXB_CSS_SELECTOR_COMBINATOR_SIBLING,           /* '+' */
    LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING,         /* '~' */
    LXB_CSS_SELECTOR_COMBINATOR_CELL,              /* '||' */
    LXB_CSS_SELECTOR_COMBINATOR__LAST_ENTRY
}
lxb_css_selector_combinator_t;

typedef enum {
    LXB_CSS_SELECTOR_MATCH_EQUAL = 0x00,  /*  = */
    LXB_CSS_SELECTOR_MATCH_INCLUDE,       /* ~= */
    LXB_CSS_SELECTOR_MATCH_DASH,          /* |= */
    LXB_CSS_SELECTOR_MATCH_PREFIX,        /* ^= */
    LXB_CSS_SELECTOR_MATCH_SUFFIX,        /* $= */
    LXB_CSS_SELECTOR_MATCH_SUBSTRING,     /* *= */
    LXB_CSS_SELECTOR_MATCH__LAST_ENTRY
}
lxb_css_selector_match_t;

typedef enum {
    LXB_CSS_SELECTOR_MODIFIER_UNSET = 0x00,
    LXB_CSS_SELECTOR_MODIFIER_I,
    LXB_CSS_SELECTOR_MODIFIER_S,
    LXB_CSS_SELECTOR_MODIFIER__LAST_ENTRY
}
lxb_css_selector_modifier_t;

typedef struct {
    lxb_css_selector_match_t    match;
    lxb_css_selector_modifier_t modifier;
    lexbor_str_t                value;
}
lxb_css_selector_attribute_t;

typedef struct {
    unsigned type;
    void     *data;
}
lxb_css_selector_pseudo_t;

typedef struct {
    lxb_css_syntax_anb_t          anb;
    lxb_css_selector_list_t       *of;
}
lxb_css_selector_anb_of_t;

struct lxb_css_selector {
    lxb_css_selector_type_t       type;
    lxb_css_selector_combinator_t combinator;

    lexbor_str_t                  name;
    lexbor_str_t                  ns;

    union lxb_css_selector_u {
        lxb_css_selector_attribute_t attribute;
        lxb_css_selector_pseudo_t    pseudo;
    }
    u;

    lxb_css_selector_t            *next;
    lxb_css_selector_t            *prev;

    lxb_css_selector_list_t       *list;
};


/*
 *   I       S       A       B       C
 * 1 bit | 1 bit | 9 bit | 9 bit | 9 bit
 */
typedef uint32_t lxb_css_selector_specificity_t;

#define LXB_CSS_SELECTOR_SPECIFICITY_MASK_N(sp, n)                            \
    ((sp) & ~((((uint32_t) 1 << 9) - 1) << (n)))

#define LXB_CSS_SELECTOR_SPECIFICITY_MASK                                     \
    ((((uint32_t) 1 << 31) - 1) << (9))

#define lxb_css_selector_sp_i(sp)  ((sp) >> 28)

#define lxb_css_selector_sp_s(sp)                                             \
    (((sp) >> 27) & ~((((uint32_t) 1 << 31) - 1) << (1)))

#define lxb_css_selector_sp_a(sp)                                             \
    (((sp) >> 18) & ~LXB_CSS_SELECTOR_SPECIFICITY_MASK)

#define lxb_css_selector_sp_b(sp)                                             \
    (((sp) >> 9) & ~LXB_CSS_SELECTOR_SPECIFICITY_MASK)

#define lxb_css_selector_sp_c(sp)                                             \
    ((sp) & ~LXB_CSS_SELECTOR_SPECIFICITY_MASK)

#define lxb_css_selector_sp_set_i(sp, num)                                    \
    sp = (LXB_CSS_SELECTOR_SPECIFICITY_MASK_N((sp), 28) | ((num) << 28))

#define lxb_css_selector_sp_set_s(sp, num)                                    \
    sp = (LXB_CSS_SELECTOR_SPECIFICITY_MASK_N((sp), 27) | ((num) << 27))

#define lxb_css_selector_sp_set_a(sp, num)                                    \
    sp = (LXB_CSS_SELECTOR_SPECIFICITY_MASK_N((sp), 18) | ((num) << 18))

#define lxb_css_selector_sp_set_b(sp, num)                                    \
    sp = (LXB_CSS_SELECTOR_SPECIFICITY_MASK_N((sp), 9) | ((num) << 9))

#define lxb_css_selector_sp_set_c(sp, num)                                    \
    sp = (LXB_CSS_SELECTOR_SPECIFICITY_MASK_N((sp), 0) | (num))

#define lxb_css_selector_sp_add_s(sp, num)                                   \
    (lxb_css_selector_sp_set_s((sp), lxb_css_selector_sp_s(sp) + num))

#define lxb_css_selector_sp_add_a(sp, num)                                   \
    (lxb_css_selector_sp_set_a((sp), lxb_css_selector_sp_a(sp) + num))

#define lxb_css_selector_sp_add_b(sp, num)                                   \
    (lxb_css_selector_sp_set_b((sp), lxb_css_selector_sp_b(sp) + num))

#define lxb_css_selector_sp_add_c(sp, num)                                   \
    (lxb_css_selector_sp_set_c((sp), lxb_css_selector_sp_c(sp) + num))

#define lxb_css_selector_sp_up_i(num)  (num | (1 << 28))
#define lxb_css_selector_sp_up_s(num)  (num | (1 << 27))

#define LXB_CSS_SELECTOR_SP_S_MAX  ((1 << 28) - 1)
#define LXB_CSS_SELECTOR_SP_A_MAX  ((1 << 27) - 1)
#define LXB_CSS_SELECTOR_SP_B_MAX  ((1 << 18) - 1)
#define LXB_CSS_SELECTOR_SP_C_MAX  ((1 <<  9) - 1)


struct lxb_css_selector_list {
    lxb_css_selector_t             *first;
    lxb_css_selector_t             *last;

    lxb_css_selector_t             *parent;

    lxb_css_selector_list_t        *next;
    lxb_css_selector_list_t        *prev;

    lxb_css_memory_t               *memory;

    lxb_css_selector_specificity_t specificity;
};


LXB_API lxb_css_selector_t *
lxb_css_selector_create(lxb_css_selector_list_t *list);

LXB_API void
lxb_css_selector_destroy(lxb_css_selector_t *selector);

LXB_API void
lxb_css_selector_destroy_chain(lxb_css_selector_t *selector);

LXB_API void
lxb_css_selector_remove(lxb_css_selector_t *selector);

LXB_API lxb_css_selector_list_t *
lxb_css_selector_list_create(lxb_css_memory_t *mem);

LXB_API void
lxb_css_selector_list_remove(lxb_css_selector_list_t *list);

LXB_API void
lxb_css_selector_list_selectors_remove(lxb_css_selectors_t *selectors,
                                       lxb_css_selector_list_t *list);

LXB_API void
lxb_css_selector_list_destroy(lxb_css_selector_list_t *list);

LXB_API void
lxb_css_selector_list_destroy_chain(lxb_css_selector_list_t *list);

LXB_API void
lxb_css_selector_list_destroy_memory(lxb_css_selector_list_t *list);

LXB_API lxb_status_t
lxb_css_selector_serialize(lxb_css_selector_t *selector,
                           lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_selector_serialize_chain(lxb_css_selector_t *selector,
                                 lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_char_t *
lxb_css_selector_serialize_chain_char(lxb_css_selector_t *selector,
                                      size_t *out_length);

LXB_API lxb_status_t
lxb_css_selector_serialize_list(lxb_css_selector_list_t *list,
                                    lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_selector_serialize_list_chain(lxb_css_selector_list_t *list,
                                      lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_char_t *
lxb_css_selector_serialize_list_chain_char(lxb_css_selector_list_t *list,
                                           size_t *out_length);

LXB_API lxb_status_t
lxb_css_selector_serialize_anb_of(lxb_css_selector_anb_of_t *anbof,
                                  lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_char_t *
lxb_css_selector_combinator(lxb_css_selector_t *selector, size_t *out_length);

LXB_API void
lxb_css_selector_list_append(lxb_css_selector_list_t *list,
                             lxb_css_selector_t *selector);

LXB_API void
lxb_css_selector_list_append_next(lxb_css_selector_list_t *dist,
                                  lxb_css_selector_list_t *src);

LXB_API void
lxb_css_selector_append_next(lxb_css_selector_t *dist, lxb_css_selector_t *src);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SELECTORS_SELECTOR_H */
