#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "mbfilter.h"
#include "nls_en.h"

const mbfl_language mbfl_language_english = {
	mbfl_no_language_english,
	"English",
	"en",
	NULL,
	mbfl_no_encoding_8859_1,
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit
};
