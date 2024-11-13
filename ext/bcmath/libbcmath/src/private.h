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
#include "zend_portability.h"

/* This will be 0x01010101 for 32-bit and 0x0101010101010101 for 64-bit */
#define SWAR_ONES (~((size_t) 0) / 0xFF)
/* This repeats a byte `x` into an entire 32/64-bit word.
 * Example: SWAR_REPEAT(0xAB) will be 0xABABABAB for 32-bit and 0xABABABABABABABAB for 64-bit. */
#define SWAR_REPEAT(x) (SWAR_ONES * (x))

#if SIZEOF_SIZE_T >= 8
#  define BC_BSWAP(u) ZEND_BYTES_SWAP64(u)
   typedef uint64_t BC_VECTOR;
#  define BC_VECTOR_SIZE 8
/* The boundary number is computed from BASE ** BC_VECTOR_SIZE */
#  define BC_VECTOR_BOUNDARY_NUM (BC_VECTOR) 100000000
#else
#  define BC_BSWAP(u) ZEND_BYTES_SWAP32(u)
   typedef uint32_t BC_VECTOR;
#  define BC_VECTOR_SIZE 4
/* The boundary number is computed from BASE ** BC_VECTOR_SIZE */
#  define BC_VECTOR_BOUNDARY_NUM (BC_VECTOR) 10000
#endif

#ifdef WORDS_BIGENDIAN
#  define BC_LITTLE_ENDIAN 0
#else
#  define BC_LITTLE_ENDIAN 1
#endif

/*
 * Adding more than this many times may cause uint32_t/uint64_t to overflow.
 * Typically this is 1844 for 64bit and 42 for 32bit.
 */
#define BC_VECTOR_NO_OVERFLOW_ADD_COUNT (~((BC_VECTOR) 0) / (BC_VECTOR_BOUNDARY_NUM * BC_VECTOR_BOUNDARY_NUM))


/* routines */
bcmath_compare_result _bc_do_compare (bc_num n1, bc_num n2, size_t scale, bool use_sign);
bc_num _bc_do_add (bc_num n1, bc_num n2);
bc_num _bc_do_sub (bc_num n1, bc_num n2);
void _bc_rm_leading_zeros (bc_num num);
