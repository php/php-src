/*
 * Copyright (C) 2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HASH_H
#define LEXBOR_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/dobject.h"
#include "lexbor/core/mraw.h"


#define LEXBOR_HASH_SHORT_SIZE     16
#define LEXBOR_HASH_TABLE_MIN_SIZE 32


typedef struct lexbor_hash_search lexbor_hash_search_t;
typedef struct lexbor_hash_insert lexbor_hash_insert_t;

#ifndef LEXBOR_HASH_EXTERN
LXB_EXTERN const lexbor_hash_insert_t *lexbor_hash_insert_raw;
LXB_EXTERN const lexbor_hash_insert_t *lexbor_hash_insert_lower;
LXB_EXTERN const lexbor_hash_insert_t *lexbor_hash_insert_upper;

LXB_EXTERN const lexbor_hash_search_t *lexbor_hash_search_raw;
LXB_EXTERN const lexbor_hash_search_t *lexbor_hash_search_lower;
LXB_EXTERN const lexbor_hash_search_t *lexbor_hash_search_upper;
#endif

/*
 * FIXME:
 * It is necessary to add the rebuild of a hash table
 * and optimize collisions.
 */

typedef struct lexbor_hash lexbor_hash_t;
typedef struct lexbor_hash_entry lexbor_hash_entry_t;

typedef uint32_t
(*lexbor_hash_id_f)(const lxb_char_t *key, size_t size);

typedef lxb_status_t
(*lexbor_hash_copy_f)(lexbor_hash_t *hash, lexbor_hash_entry_t *entry,
                      const lxb_char_t *key, size_t size);

typedef bool
(*lexbor_hash_cmp_f)(const lxb_char_t *first,
                     const lxb_char_t *second, size_t size);

struct lexbor_hash_entry {
    union {
        lxb_char_t *long_str;
        lxb_char_t short_str[LEXBOR_HASH_SHORT_SIZE + 1];
    } u;

    size_t              length;

    lexbor_hash_entry_t *next;
};

struct lexbor_hash {
    lexbor_dobject_t    *entries;
    lexbor_mraw_t       *mraw;

    lexbor_hash_entry_t **table;
    size_t              table_size;

    size_t              struct_size;
};

struct lexbor_hash_insert {
    lexbor_hash_id_f   hash; /* For generate a hash id. */
    lexbor_hash_cmp_f  cmp;  /* For compare key. */
    lexbor_hash_copy_f copy; /* For copy key. */
};

struct lexbor_hash_search {
    lexbor_hash_id_f   hash; /* For generate a hash id. */
    lexbor_hash_cmp_f  cmp;  /* For compare key. */
};


LXB_API lexbor_hash_t *
lexbor_hash_create(void);

LXB_API lxb_status_t
lexbor_hash_init(lexbor_hash_t *hash, size_t table_size, size_t struct_size);

LXB_API void
lexbor_hash_clean(lexbor_hash_t *hash);

LXB_API lexbor_hash_t *
lexbor_hash_destroy(lexbor_hash_t *hash, bool destroy_obj);


LXB_API void *
lexbor_hash_insert(lexbor_hash_t *hash, const lexbor_hash_insert_t *insert,
                   const lxb_char_t *key, size_t length);

LXB_API void *
lexbor_hash_insert_by_entry(lexbor_hash_t *hash, lexbor_hash_entry_t *entry,
                            const lexbor_hash_search_t *search,
                            const lxb_char_t *key, size_t length);

LXB_API void
lexbor_hash_remove(lexbor_hash_t *hash, const lexbor_hash_search_t *search,
                   const lxb_char_t *key, size_t length);

LXB_API void *
lexbor_hash_search(lexbor_hash_t *hash, const lexbor_hash_search_t *search,
                   const lxb_char_t *key, size_t length);

LXB_API void
lexbor_hash_remove_by_hash_id(lexbor_hash_t *hash, uint32_t hash_id,
                              const lxb_char_t *key, size_t length,
                              const lexbor_hash_cmp_f cmp_func);

LXB_API void *
lexbor_hash_search_by_hash_id(lexbor_hash_t *hash, uint32_t hash_id,
                              const lxb_char_t *key, size_t length,
                              const lexbor_hash_cmp_f cmp_func);


LXB_API uint32_t
lexbor_hash_make_id(const lxb_char_t *key, size_t length);

LXB_API uint32_t
lexbor_hash_make_id_lower(const lxb_char_t *key, size_t length);

LXB_API uint32_t
lexbor_hash_make_id_upper(const lxb_char_t *key, size_t length);

LXB_API lxb_status_t
lexbor_hash_copy(lexbor_hash_t *hash, lexbor_hash_entry_t *entry,
                 const lxb_char_t *key, size_t length);

LXB_API lxb_status_t
lexbor_hash_copy_lower(lexbor_hash_t *hash, lexbor_hash_entry_t *entry,
                       const lxb_char_t *key, size_t length);

LXB_API lxb_status_t
lexbor_hash_copy_upper(lexbor_hash_t *hash, lexbor_hash_entry_t *entry,
                       const lxb_char_t *key, size_t length);


/*
 * Inline functions
 */
lxb_inline lexbor_mraw_t *
lexbor_hash_mraw(const lexbor_hash_t *hash)
{
    return hash->mraw;
}

lxb_inline lxb_char_t *
lexbor_hash_entry_str(const lexbor_hash_entry_t *entry)
{
    if (entry->length <= LEXBOR_HASH_SHORT_SIZE) {
        return (lxb_char_t *) entry->u.short_str;
    }

    return entry->u.long_str;
}

lxb_inline lxb_char_t *
lexbor_hash_entry_str_set(lexbor_hash_entry_t *entry,
                          lxb_char_t *data, size_t length)
{
    entry->length = length;

    if (length <= LEXBOR_HASH_SHORT_SIZE) {
        memcpy(entry->u.short_str, data, length);
        return (lxb_char_t *) entry->u.short_str;
    }

    entry->u.long_str = data;
    return entry->u.long_str;
}

lxb_inline void
lexbor_hash_entry_str_free(lexbor_hash_t *hash, lexbor_hash_entry_t *entry)
{
    if (entry->length > LEXBOR_HASH_SHORT_SIZE) {
        lexbor_mraw_free(hash->mraw, entry->u.long_str);
    }

    entry->length = 0;
}

lxb_inline lexbor_hash_entry_t *
lexbor_hash_entry_create(lexbor_hash_t *hash)
{
    return (lexbor_hash_entry_t *) lexbor_dobject_calloc(hash->entries);
}

lxb_inline lexbor_hash_entry_t *
lexbor_hash_entry_destroy(lexbor_hash_t *hash, lexbor_hash_entry_t *entry)
{
    return (lexbor_hash_entry_t *) lexbor_dobject_free(hash->entries, entry);
}

lxb_inline size_t
lexbor_hash_entries_count(lexbor_hash_t *hash)
{
    return lexbor_dobject_allocated(hash->entries);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HASH_H */
