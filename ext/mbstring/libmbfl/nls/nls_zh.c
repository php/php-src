#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "mbfilter.h"
#include "nls_zh.h"

const mbfl_language mbfl_language_simplified_chinese = {
	mbfl_no_language_simplified_chinese,
	"Simplified Chinese",
	"zh-cn",
	NULL,
	mbfl_no_encoding_hz,
	mbfl_no_encoding_base64,
	mbfl_no_encoding_7bit
};

const mbfl_language mbfl_language_traditional_chinese = {
	mbfl_no_language_traditional_chinese,
	"Traditional Chinese",
	"zh-tw",
	NULL,
	mbfl_no_encoding_big5,
	mbfl_no_encoding_base64,
	mbfl_no_encoding_8bit
};
