/*
 * Copyright (C) 2024 Alexander Borisov
 *
 * Author: Niels Dossche <nielsdos@php.net>
 */

#ifndef LEXBOR_SWAR_H
#define LEXBOR_SWAR_H

#ifdef __cplusplus
extern "C" {
#endif


#include "lexbor/core/base.h"


/* 
 * Based on techniques from https://graphics.stanford.edu/~seander/bithacks.html
 */
#define LEXBOR_SWAR_ONES (~((size_t) 0) / 0xFF)
#define LEXBOR_SWAR_REPEAT(x) (LEXBOR_SWAR_ONES * (x))
#define LEXBOR_SWAR_HAS_ZERO(v) (((v) - LEXBOR_SWAR_ONES) & ~(v) & LEXBOR_SWAR_REPEAT(0x80))
#define LEXBOR_SWAR_IS_LITTLE_ENDIAN (*(unsigned char *) &(uint16_t){1})


/*
 * When handling hot loops that search for a set of characters,
 * this function can be used to quickly move the data pointer much
 * closer to the first occurrence of such a character.
 */
lxb_inline const lxb_char_t *
lexbor_swar_seek4(const lxb_char_t *data, const lxb_char_t *end,
                  lxb_char_t c1, lxb_char_t c2, lxb_char_t c3, lxb_char_t c4)
{
    size_t bytes, matches, t1, t2, t3, t4;

    if (LEXBOR_SWAR_IS_LITTLE_ENDIAN) {
        while (data + sizeof(size_t) <= end) {
            memcpy(&bytes, data, sizeof(size_t));

            t1 = bytes ^ LEXBOR_SWAR_REPEAT(c1);
            t2 = bytes ^ LEXBOR_SWAR_REPEAT(c2);
            t3 = bytes ^ LEXBOR_SWAR_REPEAT(c3);
            t4 = bytes ^ LEXBOR_SWAR_REPEAT(c4);
            matches =   LEXBOR_SWAR_HAS_ZERO(t1) | LEXBOR_SWAR_HAS_ZERO(t2)
                      | LEXBOR_SWAR_HAS_ZERO(t3) | LEXBOR_SWAR_HAS_ZERO(t4);

            if (matches) {
                data += ((((matches - 1) & LEXBOR_SWAR_ONES) * LEXBOR_SWAR_ONES)
                        >> (sizeof(size_t) * 8 - 8)) - 1;
                break;
            } else {
                data += sizeof(size_t);
            }
        }
    }

    return data;
}

lxb_inline const lxb_char_t *
lexbor_swar_seek3(const lxb_char_t *data, const lxb_char_t *end,
                  lxb_char_t c1, lxb_char_t c2, lxb_char_t c3)
{
    size_t bytes, matches, t1, t2, t3;

    if (LEXBOR_SWAR_IS_LITTLE_ENDIAN) {
        while (data + sizeof(size_t) <= end) {
            memcpy(&bytes, data, sizeof(size_t));

            t1 = bytes ^ LEXBOR_SWAR_REPEAT(c1);
            t2 = bytes ^ LEXBOR_SWAR_REPEAT(c2);
            t3 = bytes ^ LEXBOR_SWAR_REPEAT(c3);
            matches =   LEXBOR_SWAR_HAS_ZERO(t1) | LEXBOR_SWAR_HAS_ZERO(t2)
                       | LEXBOR_SWAR_HAS_ZERO(t3);

            if (matches) {
                data += ((((matches - 1) & LEXBOR_SWAR_ONES) * LEXBOR_SWAR_ONES)
                         >> (sizeof(size_t) * 8 - 8)) - 1;
                break;
            } else {
                data += sizeof(size_t);
            }
        }
    }

    return data;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_SWAR_H */

