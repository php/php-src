#include "nls_uni.h"

static const char *mbfl_language_uni_aliases[] = {"universal", NULL};

const mbfl_language mbfl_language_uni = {
	mbfl_no_language_uni,
	"uni",
	"uni",
	(const char *(*)[])&mbfl_language_uni_aliases,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_base64,
	mbfl_no_encoding_base64
};
