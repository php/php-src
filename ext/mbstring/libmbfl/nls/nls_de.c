#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>

#include "mbfilter.h"
#include "nls_de.h"

static const char *mbfl_language_german_aliases[] = {"Deutsch", NULL};

const mbfl_language mbfl_language_german = {
	mbfl_no_language_german,
	"German",
	"de",
	(const char *(*)[])&mbfl_language_german_aliases,
	mbfl_no_encoding_8859_15,
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit
};
