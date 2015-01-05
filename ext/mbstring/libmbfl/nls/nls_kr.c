#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "mbfilter.h"
#include "nls_kr.h"

const mbfl_language mbfl_language_korean = {
	mbfl_no_language_korean,
	"Korean",
	"ko",
	NULL,
	mbfl_no_encoding_2022kr,
	mbfl_no_encoding_base64,
	mbfl_no_encoding_7bit
};

