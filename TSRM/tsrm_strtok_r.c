#include <stdio.h>

#include "tsrm_config_common.h"
#include "tsrm_strtok_r.h"

static inline int in_character_class(char ch, const char *delim)
{
	while (*delim) {
		if (*delim == ch) {
			return 1;
		}
		delim++;
	}
	return 0;
}

TSRM_API char *tsrm_strtok_r(char *s, const char *delim, char **last)
{
	char *token;

	if (s == NULL) {
		s = *last;
	}

	while (*s && in_character_class(*s, delim)) {
		s++;
	}
	if (!*s) {
		return NULL;
	}

	token = s;

	while (*s && !in_character_class(*s, delim)) {
		s++;
	}
	if (!*s) {
		*last = s;
	} else {
		*s = '\0';
		*last = s + 1;
	}
	return token;
}

#if 0

main()
{
	char foo[] = "/foo/bar//\\barbara";
	char *last;
	char *token;

	token = tsrm_strtok_r(foo, "/\\", &last);
	while (token) {
		printf ("Token = '%s'\n", token);
		token = tsrm_strtok_r(NULL, "/\\", &last);
	}

	return 0;
}

#endif
