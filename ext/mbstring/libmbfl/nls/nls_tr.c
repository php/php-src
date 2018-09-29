#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "mbfilter.h"
#include "nls_tr.h"

const mbfl_language mbfl_language_turkish = {
	mbfl_no_language_turkish,
	"Turkish",
	"tr",
	NULL,
	mbfl_no_encoding_8859_9,
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit
};
