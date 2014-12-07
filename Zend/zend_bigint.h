/*
  +----------------------------------------------------------------------+
  | Zend Big Integer Support                                             |
  +----------------------------------------------------------------------+
  | Copyright (c) 2014 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrea Faulds <ajf@ajf.me>                                  |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_BIGINT_H
#define ZEND_BIGINT_H

#include <gmp.h>

#include "zend.h"
#include "zend_types.h"



/*** INTERNAL FUNCTIONS ***/

/* Called by zend_startup */
void zend_startup_bigint(void);

/*** INITIALISERS ***/

/* HERE BE DRAGONS: Memory allocated internally to hold bigint data is always
 * non-persistent - even if the zend_bigint struct itself is persistent,
 * the data it points is not.
 */

/* Allocates a bigint and returns pointer, does NOT initialise */
ZEND_API zend_bigint* zend_bigint_alloc(void);

/* Initialises a bigint */
ZEND_API void zend_bigint_init(zend_bigint *big);

/* Convenience function: Allocates and initialises a bigint, returns pointer */
ZEND_API zend_bigint* zend_bigint_init_alloc(void);

/* Initialises a bigint from a string with the specified base (in range 2-36)
 * Returns FAILURE on failure (if the string is not entirely numeric), else SUCCESS
 */
ZEND_API int zend_bigint_init_from_string(zend_bigint *big, const char *str, int base);

/* Initialises a bigint from a string with the specified base (in range 2-36)
 * Takes a length - due to an extra memory allocation, this function is slower
 * Returns FAILURE on failure, else SUCCESS */
ZEND_API int zend_bigint_init_from_string_length(zend_bigint *big, const char *str, size_t length, int base);

/* Intialises a bigint from a C-string with the specified base (10 or 16)
 * If endptr is not NULL, it it set to point to first character after number
 * If base is zero, it shall be detected from the prefix: 0x/0X for 16, else 10
 * Leading whitespace is ignored, will take as many valid characters as possible
 * Stops at first non-valid character, else null byte
 * If there are no valid characters, the bigint is initialised to zero
 * This behaviour is supposed to match that of strtol but is not exactly the same
 */
ZEND_API void zend_bigint_init_strtol(zend_bigint *big, const char *str, char** endptr, int base);

/* Initialises a bigint from a long */
ZEND_API void zend_bigint_init_from_long(zend_bigint *big, zend_long value);

/* Initialises a bigint from a double */
ZEND_API void zend_bigint_init_from_double(zend_bigint *big, double value);

/* Initialises a bigint and duplicates a bigint to it (copies value) */
ZEND_API void zend_bigint_init_dup(zend_bigint *big, const zend_bigint *source);

/* Destroys a bigint (does NOT deallocate) */
ZEND_API void zend_bigint_dtor(zend_bigint *big);

/* Decreases the refcount of a bigint and, if <= 0, destroys and frees it */
ZEND_API void zend_bigint_release(zend_bigint *big);

/*** INFORMATION ***/

/* Returns true if bigint can fit into type without truncation
   ulong is zend_ulong, long is zend_long */
ZEND_API zend_bool zend_bigint_can_fit_ulong(const zend_bigint *big);
ZEND_API zend_bool zend_bigint_can_fit_long(const zend_bigint *big);

/* Returns sign of bigint (-1 for negative, 0 for zero or 1 for positive) */
ZEND_API int zend_bigint_sign(const zend_bigint *big);

/* Returns true if two integers divisible (i.e. num mod divisor == 0) */
ZEND_API zend_bool zend_bigint_divisible(const zend_bigint *num, const zend_bigint *divisor);
ZEND_API zend_bool zend_bigint_divisible_long(const zend_bigint *num, zend_long divisor);
ZEND_API zend_bool zend_bigint_long_divisible(zend_long num, const zend_bigint *divisor);

/*** CONVERTORS ***/

/* Converts to long; if it won't fit, wraps around (like zend_dval_to_lval) */
ZEND_API zend_long zend_bigint_to_long(const zend_bigint *big);

/* Converts to long; if it won't fit, saturates (caps at ZEND_LONG_MAX/_MIN) */
ZEND_API zend_long zend_bigint_to_long_saturate(const zend_bigint *big);

/* Converts to long; if it won't fit, may return garbage
 * If it didn't fit, sets overflow to 1, else to 0 */
ZEND_API zend_long zend_bigint_to_long_ex(const zend_bigint *big, zend_bool *overflow);

/* Converts to unsigned long; this will cap at the max value of an unsigned long */
ZEND_API zend_ulong zend_bigint_to_ulong(const zend_bigint *big);

/* Converts to bool */
ZEND_API zend_bool zend_bigint_to_bool(const zend_bigint *big);

/* Converts to double; this will lose precision beyond a certain point */
ZEND_API double zend_bigint_to_double(const zend_bigint *big);

/* Converts to decimal C string
 * HERE BE DRAGONS: String allocated is non-persistent */
ZEND_API char* zend_bigint_to_string(const zend_bigint *big);

/* Convenience function: Converts to zend string */
ZEND_API zend_string* zend_bigint_to_zend_string(const zend_bigint *big, int persistent);

/* Converts to C string of arbitrary base */
ZEND_API char* zend_bigint_to_string_base(const zend_bigint *big, int base);

/*** OPERATIONS **/

/* By the way, in case you're wondering, you can indeed use something as both
 * output and operand. For example, zend_bigint_add_long(foo, foo, 1) is
 * perfectly valid for incrementing foo. This is because gmp supports it, and
 * zend_bigint is (at the time of writing, at least) merely a thin wrapper
 * around gmp. This is not advisable, however, because bigints are reference-
 * counted and should be copy-on-write so far as userland PHP code cares. Do
 * it sparingly, and never to bigints which have been exposed to userland. With
 * great power comes great responsibility.
 */

/* Adds two integers */
ZEND_API void zend_bigint_add(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_add_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);
ZEND_API void zend_bigint_long_add_long(zend_bigint *out, zend_long op1, zend_long op2);

/* Subtracts two integers */
ZEND_API void zend_bigint_subtract(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_subtract_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);
ZEND_API void zend_bigint_long_subtract(zend_bigint *out, zend_long op1, const zend_bigint *op2);
ZEND_API void zend_bigint_long_subtract_long(zend_bigint *out, zend_long op1, zend_long op2);

/* Multiplies two integers */
ZEND_API void zend_bigint_multiply(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_multiply_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);
ZEND_API void zend_bigint_long_multiply_long(zend_bigint *out, zend_long op1, zend_long op2);

/* Raises an integer base to an integer power */
ZEND_API void zend_bigint_pow_ulong(zend_bigint *out, const zend_bigint *base, zend_ulong power);
ZEND_API void zend_bigint_long_pow_ulong(zend_bigint *out, zend_long base, zend_ulong power);

/* Divides an integer by an integer
 * _as_double functions may provide better precision than converting to double
 * and doing a floating-point division
 */
ZEND_API void zend_bigint_divide(zend_bigint *out, const zend_bigint *big, const zend_bigint *divisor);
ZEND_API double zend_bigint_divide_as_double(const zend_bigint *num, const zend_bigint *divisor);
ZEND_API void zend_bigint_divide_long(zend_bigint *out, const zend_bigint *big, zend_long divisor);
ZEND_API double zend_bigint_divide_long_as_double(const zend_bigint *num, zend_long divisor);
ZEND_API void zend_bigint_long_divide(zend_bigint *out, zend_long big, const zend_bigint *divisor);
ZEND_API double zend_bigint_long_divide_as_double(zend_long num, const zend_bigint *divisor);

/* Finds the remainder of the division of two integers */
ZEND_API void zend_bigint_modulus(zend_bigint *out, const zend_bigint *num, const zend_bigint *divisor);
ZEND_API void zend_bigint_modulus_long(zend_bigint *out, const zend_bigint *num, zend_long divisor);
ZEND_API void zend_bigint_long_modulus(zend_bigint *out, zend_long num, const zend_bigint *divisor);

/* Finds the one's complement of an integer */
ZEND_API void zend_bigint_ones_complement(zend_bigint *out, const zend_bigint *op);

/* Finds the bitwise OR of two integers */
ZEND_API void zend_bigint_or(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_or_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);

/* Finds the bitwise AND of two integers */
ZEND_API void zend_bigint_and(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_and_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);

/* Finds the bitwise XOR of two integers */
ZEND_API void zend_bigint_xor(zend_bigint *out, const zend_bigint *op1, const zend_bigint *op2);
ZEND_API void zend_bigint_xor_long(zend_bigint *out, const zend_bigint *op1, zend_long op2);

/* Shifts an integer left by an integer number of bits */
ZEND_API void zend_bigint_shift_left_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift);
ZEND_API void zend_bigint_long_shift_left_ulong(zend_bigint *out, zend_long num, zend_ulong shift);

/* Shifts an integer right by an integer number of bits */
ZEND_API void zend_bigint_shift_right_ulong(zend_bigint *out, const zend_bigint *num, zend_ulong shift);

/* Compares two numbers
 * Result is negative if op1 > op2, zero if op1 == op2, positive if op1 < op2)
 */
ZEND_API int zend_bigint_cmp(const zend_bigint *op1, const zend_bigint *op2);
ZEND_API int zend_bigint_cmp_long(const zend_bigint *op1, zend_long op2);
ZEND_API int zend_bigint_cmp_double(const zend_bigint *op1, double op2);

/* Finds the absolute value of an integer */
ZEND_API void zend_bigint_abs(zend_bigint *out, const zend_bigint *big);

#endif
