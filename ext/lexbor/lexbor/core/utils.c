/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/utils.h"


size_t
lexbor_utils_power(size_t t, size_t k)
{
    size_t res = 1;

    while (k) {
        if (k & 1) {
            res *= t;
        }

        t *= t;
        k >>= 1;
    }

    return res;
}

size_t
lexbor_utils_hash_hash(const lxb_char_t *key, size_t key_size)
{
    size_t hash, i;

    for (hash = i = 0; i < key_size; i++) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
