#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "mbfilter.h"
#include "nls_ru.h"

const mbfl_language mbfl_language_russian = {
	mbfl_no_language_russian,
	"Russian",
	"ru",
	NULL,
	mbfl_no_encoding_koi8r,
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit
};
