#include "nls_de.h"

static const char *mbfl_language_german_aliases[] = {"Deutsch", NULL};

const mbfl_language mbfl_language_german = {
	mbfl_no_language_german,
	"German",
	"de",
	mbfl_language_german_aliases,
	mbfl_no_encoding_8859_15,
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit
};
