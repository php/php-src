/*-----------------------------------------------------------------------------
 * MurmurHash3 was written by Austin Appleby, and is placed in the public
 * domain.
 *
 * This implementation was written by Shane Day, and is also public domain.
 *
 * This implementation was modified to match PMurHash128.cpp.
 */

/* ------------------------------------------------------------------------- */

// Microsoft Visual Studio

#if defined(_MSC_VER) && (_MSC_VER < 1600)

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

// Other compilers

#else // defined(_MSC_VER)

#include <stdint.h>

#endif // !defined(_MSC_VER)

/* ------------------------------------------------------------------------- */
/* Prototypes */

void PMurHash32_Process(uint32_t *ph1, uint32_t *pcarry, const void *key, int len);
uint32_t PMurHash32_Result(uint32_t h1, uint32_t carry, uint32_t total_length);
