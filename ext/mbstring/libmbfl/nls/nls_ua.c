#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif


#include "mbfilter.h"
#include "nls_ua.h"

const mbfl_language mbfl_language_ukrainian = {
	mbfl_no_language_ukrainian,
	"Ukrainian",
	"ua",
	NULL,
	mbfl_no_encoding_koi8u,
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit
};
