/* private.h: bcmath library header.    	*/
/*
    Copyright (C) 1991, 1992, 1993, 1994, 1997 Free Software Foundation, Inc.
    Copyright (C) 2000 Philip A. Nelson

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.  (LICENSE)

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.

    You may contact the author by:
       e-mail:  philnelson@acm.org
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062

*************************************************************************/

/* "Private" routines to bcmath. */

#include <stdbool.h>
#include <stddef.h>

/* This will be 0x01010101 for 32-bit and 0x0101010101010101 for 64-bit */
#define SWAR_ONES (~((size_t) 0) / 0xFF)
/* This repeats a byte `x` into an entire 32/64-bit word.
 * Example: SWAR_REPEAT(0xAB) will be 0xABABABAB for 32-bit and 0xABABABABABABABAB for 64-bit. */
#define SWAR_REPEAT(x) (SWAR_ONES * (x))

/* Bytes swap */
#ifdef _MSC_VER
#  include <stdlib.h>
#  define BC_BSWAP32(u) _byteswap_ulong(u)
#  define BC_BSWAP64(u) _byteswap_uint64(u)
#else
#  ifdef __GNUC__
#    define BC_BSWAP32(u) __builtin_bswap32(u)
#    define BC_BSWAP64(u) __builtin_bswap64(u)
#  elif defined(__has_builtin)
#    if __has_builtin(__builtin_bswap32)
#      define BC_BSWAP32(u) __builtin_bswap32(u)
#    endif // __has_builtin(__builtin_bswap32)
#    if __has_builtin(__builtin_bswap64)
#      define BC_BSWAP64(u) __builtin_bswap64(u)
#    endif // __has_builtin(__builtin_bswap64)
#  endif // __GNUC__
#endif // _MSC_VER
#ifndef BC_BSWAP32
static inline uint32_t BC_BSWAP32(uint32_t u)
{
  return (((u & 0xff000000) >> 24)
          | ((u & 0x00ff0000) >>  8)
          | ((u & 0x0000ff00) <<  8)
          | ((u & 0x000000ff) << 24));
}
#endif
#ifndef BC_BSWAP64
static inline uint64_t BC_BSWAP64(uint64_t u)
{
   return (((u & 0xff00000000000000ULL) >> 56)
          | ((u & 0x00ff000000000000ULL) >> 40)
          | ((u & 0x0000ff0000000000ULL) >> 24)
          | ((u & 0x000000ff00000000ULL) >>  8)
          | ((u & 0x00000000ff000000ULL) <<  8)
          | ((u & 0x0000000000ff0000ULL) << 24)
          | ((u & 0x000000000000ff00ULL) << 40)
          | ((u & 0x00000000000000ffULL) << 56));
}
#endif

#if SIZEOF_SIZE_T >= 8
#  define BC_BSWAP(u) BC_BSWAP64(u)
#  define BC_VECTOR uint64_t
#else
#  define BC_BSWAP(u) BC_BSWAP32(u)
#  define BC_VECTOR uint32_t
#endif

#ifdef WORDS_BIGENDIAN
#  define BC_LITTLE_ENDIAN 0
#else
#  define BC_LITTLE_ENDIAN 1
#endif


/* routines */
bcmath_compare_result _bc_do_compare (bc_num n1, bc_num n2, bool use_sign);
bc_num _bc_do_add (bc_num n1, bc_num n2);
bc_num _bc_do_sub (bc_num n1, bc_num n2);
void _bc_rm_leading_zeros (bc_num num);
