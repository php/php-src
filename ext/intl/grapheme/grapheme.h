#ifndef GRAPHEME_MASK_H
#define GRAPHEME_MASK_H

#include <unicode/ubrk.h>
#include <unicode/utypes.h>

UBool grapheme_mask_validate_mask_char(const char *mask_char, size_t mask_char_len, UErrorCode *status);

#endif

