#ifndef MBFL_UTF7_HELPER_H
#define MBFL_UTF7_HELPER_H

#include "mbfilter.h"

/* Ways which a Base64-encoded section can end: */
#define DASH 0xFC
#define DIRECT 0xFD
#define ASCII 0xFE
#define ILLEGAL 0xFF

static inline bool is_base64_end_valid(unsigned char n, bool gap, bool is_surrogate)
{
	return !(gap || is_surrogate || n == ASCII || n == ILLEGAL);
}

static inline bool has_surrogate(uint16_t cp, bool is_surrogate)
{
	return !is_surrogate && cp >= 0xD800 && cp <= 0xDBFF;
}

#endif /* MBFL_UTF7_HELPER_H */
