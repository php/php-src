#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "mbfilter.h"
#include "nls_neutral.h"

const mbfl_language mbfl_language_neutral = {
	mbfl_no_language_neutral,
	"neutral",
	"neutral",
	NULL,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_base64,
	mbfl_no_encoding_base64
};
