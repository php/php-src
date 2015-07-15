/**
 * "How to detect integer overflow in C and C++ addition and subtraction"
 * http://ptspts.blogspot.com/2014/01/how-to-detect-integer-overflow-in-c-and.html
 */
#ifndef _INT_OVERFLOW_H
#define _INT_OVERFLOW_H 1

#undef MAX_INT_SIZE
#ifdef __cplusplus  /* g++-4.6 doesn't have __builtin_choose_expr. */
static inline unsigned TO_UNSIGNED_LOW(int x) { return x; } 
static inline unsigned TO_UNSIGNED_LOW(unsigned x) { return x; } 
static inline unsigned long long TO_UNSIGNED_LOW(long long x) { return x; } 
static inline unsigned long long TO_UNSIGNED_LOW(unsigned long long x) { return x; } 
#endif
#if __SIZEOF_INT128__ >= 16 || ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)) && __SIZEOF_SIZE_T__ >= 8)
#ifdef __cplusplus  /* g++-4.6 doesn't have __builtin_choose_expr. */
static inline __uint128_t TO_UNSIGNED_LOW(__int128_t x) { return x; } 
static inline __uint128_t TO_UNSIGNED_LOW(__uint128_t x) { return x; } 
#else
#define TO_UNSIGNED_LOW(x) ( \
    __builtin_choose_expr(sizeof(x) <= sizeof(int), (unsigned)(x), \
    __builtin_choose_expr(sizeof(x) <= sizeof(long long), (unsigned long long)(x), \
    __builtin_choose_expr(sizeof(x) <= sizeof(__int128_t), (__uint128_t)(x), \
    (void)0))))  /* Compile-time error when assigned to something. */
#endif
#define MAX_INT_SIZE (sizeof(long long) > sizeof(__int128_t) ? sizeof(long long) : sizeof(__int128_t))
#else
#ifndef __cplusplus
#define TO_UNSIGNED_LOW(x) ( \
    __builtin_choose_expr(sizeof(x) <= sizeof(int), (unsigned)(x), \
    __builtin_choose_expr(sizeof(x) <= sizeof(long long), (unsigned long long)(x), \
    (void)0)))  /* Compile-time error when assigned to something. */
#endif
#define MAX_INT_SIZE (sizeof(long long))
#endif
/* Converts to the the corresponding (or a bit larger) unsigned type.
 */
#define to_unsigned(x) TO_UNSIGNED_LOW(x)

/* Doesn't evaluate x. Returns (int)0 or (int)1 indicating whether the value or
 * type x is unsigned.
 */
#define is_unsigned(a) (((__typeof__(a))-1) > 0)

/* Detect signed addition overflow, without executing a single overflowing
 * operation.
 */
#define _IS_ADD_OVERFLOW_S(x, y, c) ({ \
    __typeof__(x) _x = (x), _y  = (y), _z = \
        (_x ^ ~_y) & (1 << (sizeof(_x) * 8 - 1)); \
    (int)(((_z & (((_x ^ _z) + _y + (c)) ^ ~_y)) >> \
    (sizeof(_x) * 8 - 1)) & 1); })

/* Detect signed subtraction overflow, without executing a single overflowing
 * operation.
 */
#define _IS_SUBTRACT_OVERFLOW_S(x, y, c) ({ \
    __typeof__(x) _x = (x), _y  = (y), _z = \
        (_x ^ ~_y) & (1 << (sizeof(_x) * 8 - 1)); \
    (int)(((_z & (((_x ^ _z) - _y - (c)) ^ _y)) >> \
    (sizeof(_x) * 8 - 1)) & 1); })

/* Returns (int)0 or (int)1 indicating whether the addition x + y + c would
 * overflow or underflow. x and y must be of the same (signed or unsigned)
 * integer type, and c must be 0 or 1, of any integer (or bool) type.
 */
#define is_add_overflow(x, y, c) ( \
    sizeof(x) > MAX_INT_SIZE && !is_unsigned(x) ? \
    _IS_ADD_OVERFLOW_S(x, y, c) : ({ \
    __typeof__(x) _x = (x), _y = (y), _s = \
        (__typeof__(x))(to_unsigned(_x) + _y + (c)); \
    is_unsigned(_x) ? \
    (int)((((_x & _y) | ((_x | _y) & ~_s)) >> (sizeof(_x) * 8 - 1))) : \
    (int)((((_s ^ _x) & (_s ^ _y)) >> (sizeof(_x) * 8 - 1)) & 1); }))

/* Returns (int)0 or (int)1 indicating whether the subtraction x - y - c would
 * overflow or underflow. x and y must be of the same (signed or unsigned)
 * integer type, and c must be 0 or 1, of any integer (or bool) type.
 */
#define is_subtract_overflow(x, y, c) ( \
    sizeof(x) > MAX_INT_SIZE && !is_unsigned(x) ? \
    _IS_SUBTRACT_OVERFLOW_S(x, y, c) : ({ \
    __typeof__(x) _x = (x), _y = (y), _s = \
        (__typeof__(x))(to_unsigned(_x) - _y - (c)); \
    is_unsigned(_x) ? \
    (int)((((~_x & _y) | ((~_x | _y) & _s)) >> (sizeof(_x) * 8 - 1))) : \
    (int)((((_x ^ _y) & (_s ^ _x)) >> (sizeof(_x) * 8 - 1)) & 1); }))

#endif  /* INT_OVERFLOW_H */
