/*
 * uriparser - RFC 3986 URI parsing library
 *
 * Copyright (C) 2007, Weijia Song <songweijia@gmail.com>
 * Copyright (C) 2007, Sebastian Pipping <sebastian@pipping.org>
 * All rights reserved.
 *
 * Redistribution and use in source  and binary forms, with or without
 * modification, are permitted provided  that the following conditions
 * are met:
 *
 *     1. Redistributions  of  source  code   must  retain  the  above
 *        copyright notice, this list  of conditions and the following
 *        disclaimer.
 *
 *     2. Redistributions  in binary  form  must  reproduce the  above
 *        copyright notice, this list  of conditions and the following
 *        disclaimer  in  the  documentation  and/or  other  materials
 *        provided with the distribution.
 *
 *     3. Neither the  name of the  copyright holder nor the  names of
 *        its contributors may be used  to endorse or promote products
 *        derived from  this software  without specific  prior written
 *        permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING, BUT NOT
 * LIMITED TO,  THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS
 * FOR  A  PARTICULAR  PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT  SHALL
 * THE  COPYRIGHT HOLDER  OR CONTRIBUTORS  BE LIABLE  FOR ANY  DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA,  OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* What encodings are enabled? */
#include <uriparser/UriDefsConfig.h>
#if (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* Include SELF twice */
# ifdef URI_ENABLE_ANSI
#  define URI_PASS_ANSI 1
#  include "UriQuery.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriQuery.c"
#  undef URI_PASS_UNICODE
# endif
#else
# ifdef URI_PASS_ANSI
#  include <uriparser/UriDefsAnsi.h>
# else
#  include <uriparser/UriDefsUnicode.h>
#  include <wchar.h>
# endif



#ifndef URI_DOXYGEN
# include <uriparser/Uri.h>
# include "UriCommon.h"
# include "UriMemory.h"
#endif



#include <limits.h>
#include <stddef.h> /* size_t */



static int URI_FUNC(ComposeQueryEngine)(URI_CHAR * dest,
		const URI_TYPE(QueryList) * queryList,
		int maxChars, int * charsWritten, int * charsRequired,
		UriBool spaceToPlus, UriBool normalizeBreaks);

static UriBool URI_FUNC(AppendQueryItem)(URI_TYPE(QueryList) ** prevNext,
		int * itemCount, const URI_CHAR * keyFirst, const URI_CHAR * keyAfter,
		const URI_CHAR * valueFirst, const URI_CHAR * valueAfter,
		UriBool plusToSpace, UriBreakConversion breakConversion,
		UriMemoryManager * memory);



int URI_FUNC(ComposeQueryCharsRequired)(const URI_TYPE(QueryList) * queryList,
		int * charsRequired) {
	const UriBool spaceToPlus = URI_TRUE;
	const UriBool normalizeBreaks = URI_TRUE;

	return URI_FUNC(ComposeQueryCharsRequiredEx)(queryList, charsRequired,
			spaceToPlus, normalizeBreaks);
}



int URI_FUNC(ComposeQueryCharsRequiredEx)(const URI_TYPE(QueryList) * queryList,
		int * charsRequired, UriBool spaceToPlus, UriBool normalizeBreaks) {
	if ((queryList == NULL) || (charsRequired == NULL)) {
		return URI_ERROR_NULL;
	}

	return URI_FUNC(ComposeQueryEngine)(NULL, queryList, 0, NULL,
			charsRequired, spaceToPlus, normalizeBreaks);
}



int URI_FUNC(ComposeQuery)(URI_CHAR * dest,
		const URI_TYPE(QueryList) * queryList, int maxChars, int * charsWritten) {
	const UriBool spaceToPlus = URI_TRUE;
	const UriBool normalizeBreaks = URI_TRUE;

	return URI_FUNC(ComposeQueryEx)(dest, queryList, maxChars, charsWritten,
			spaceToPlus, normalizeBreaks);
}



int URI_FUNC(ComposeQueryEx)(URI_CHAR * dest,
		const URI_TYPE(QueryList) * queryList, int maxChars, int * charsWritten,
		UriBool spaceToPlus, UriBool normalizeBreaks) {
	if ((dest == NULL) || (queryList == NULL)) {
		return URI_ERROR_NULL;
	}

	if (maxChars < 1) {
		return URI_ERROR_OUTPUT_TOO_LARGE;
	}

	return URI_FUNC(ComposeQueryEngine)(dest, queryList, maxChars,
			charsWritten, NULL, spaceToPlus, normalizeBreaks);
}



int URI_FUNC(ComposeQueryMalloc)(URI_CHAR ** dest,
		const URI_TYPE(QueryList) * queryList) {
	const UriBool spaceToPlus = URI_TRUE;
	const UriBool normalizeBreaks = URI_TRUE;

	return URI_FUNC(ComposeQueryMallocEx)(dest, queryList,
			spaceToPlus, normalizeBreaks);
}



int URI_FUNC(ComposeQueryMallocEx)(URI_CHAR ** dest,
		const URI_TYPE(QueryList) * queryList,
		UriBool spaceToPlus, UriBool normalizeBreaks) {
	return URI_FUNC(ComposeQueryMallocExMm)(dest, queryList, spaceToPlus,
			normalizeBreaks, NULL);
}



int URI_FUNC(ComposeQueryMallocExMm)(URI_CHAR ** dest,
		const URI_TYPE(QueryList) * queryList,
		UriBool spaceToPlus, UriBool normalizeBreaks,
		UriMemoryManager * memory) {
	int charsRequired;
	int res;
	URI_CHAR * queryString;

	if (dest == NULL) {
		return URI_ERROR_NULL;
	}

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	/* Calculate space */
	res = URI_FUNC(ComposeQueryCharsRequiredEx)(queryList, &charsRequired,
			spaceToPlus, normalizeBreaks);
	if (res != URI_SUCCESS) {
		return res;
	}
	if (charsRequired == INT_MAX) {
	    return URI_ERROR_MALLOC;
	}
	charsRequired++;

	/* Allocate space */
	queryString = memory->calloc(memory, charsRequired, sizeof(URI_CHAR));
	if (queryString == NULL) {
		return URI_ERROR_MALLOC;
	}

	/* Put query in */
	res = URI_FUNC(ComposeQueryEx)(queryString, queryList, charsRequired,
			NULL, spaceToPlus, normalizeBreaks);
	if (res != URI_SUCCESS) {
		memory->free(memory, queryString);
		return res;
	}

	*dest = queryString;
	return URI_SUCCESS;
}



int URI_FUNC(ComposeQueryEngine)(URI_CHAR * dest,
		const URI_TYPE(QueryList) * queryList,
		int maxChars, int * charsWritten, int * charsRequired,
		UriBool spaceToPlus, UriBool normalizeBreaks) {
	UriBool firstItem = URI_TRUE;
	int ampersandLen = 0;  /* increased to 1 from second item on */
	URI_CHAR * write = dest;

	/* Subtract terminator */
	if (dest == NULL) {
		*charsRequired = 0;
	} else {
		maxChars--;
	}

	while (queryList != NULL) {
		const URI_CHAR * const key = queryList->key;
		const URI_CHAR * const value = queryList->value;
		const int worstCase = (normalizeBreaks == URI_TRUE ? 6 : 3);
		const size_t keyLen = (key == NULL) ? 0 : URI_STRLEN(key);
		int keyRequiredChars;
		const size_t valueLen = (value == NULL) ? 0 : URI_STRLEN(value);
		int valueRequiredChars;

		if ((keyLen >= (size_t)INT_MAX / worstCase) || (valueLen >= (size_t)INT_MAX / worstCase)) {
			return URI_ERROR_OUTPUT_TOO_LARGE;
		}
		keyRequiredChars = worstCase * (int)keyLen;
		valueRequiredChars = worstCase * (int)valueLen;

		if (dest == NULL) {
			(*charsRequired) += ampersandLen + keyRequiredChars + ((value == NULL)
						? 0
						: 1 + valueRequiredChars);

			if (firstItem == URI_TRUE) {
				ampersandLen = 1;
				firstItem = URI_FALSE;
			}
		} else {
			if ((write - dest) + ampersandLen + keyRequiredChars > maxChars) {
				return URI_ERROR_OUTPUT_TOO_LARGE;
			}

			/* Copy key */
			if (firstItem == URI_TRUE) {
				ampersandLen = 1;
				firstItem = URI_FALSE;
			} else {
				write[0] = _UT('&');
				write++;
			}
			write = URI_FUNC(EscapeEx)(key, key + keyLen,
					write, spaceToPlus, normalizeBreaks);

			if (value != NULL) {
				if ((write - dest) + 1 + valueRequiredChars > maxChars) {
					return URI_ERROR_OUTPUT_TOO_LARGE;
				}

				/* Copy value */
				write[0] = _UT('=');
				write++;
				write = URI_FUNC(EscapeEx)(value, value + valueLen,
						write, spaceToPlus, normalizeBreaks);
			}
		}

		queryList = queryList->next;
	}

	if (dest != NULL) {
		write[0] = _UT('\0');
		if (charsWritten != NULL) {
			*charsWritten = (int)(write - dest) + 1; /* .. for terminator */
		}
	}

	return URI_SUCCESS;
}



UriBool URI_FUNC(AppendQueryItem)(URI_TYPE(QueryList) ** prevNext,
		int * itemCount, const URI_CHAR * keyFirst, const URI_CHAR * keyAfter,
		const URI_CHAR * valueFirst, const URI_CHAR * valueAfter,
		UriBool plusToSpace, UriBreakConversion breakConversion,
		UriMemoryManager * memory) {
	const int keyLen = (int)(keyAfter - keyFirst);
	const int valueLen = (int)(valueAfter - valueFirst);
	URI_CHAR * key;
	URI_CHAR * value;

	if ((prevNext == NULL) || (itemCount == NULL)
			|| (keyFirst == NULL) || (keyAfter == NULL)
			|| (keyFirst > keyAfter) || (valueFirst > valueAfter)
			|| ((keyFirst == keyAfter)
				&& (valueFirst == NULL) && (valueAfter == NULL))) {
		return URI_TRUE;
	}

	/* Append new empty item */
	*prevNext = memory->malloc(memory, 1 * sizeof(URI_TYPE(QueryList)));
	if (*prevNext == NULL) {
		return URI_FALSE; /* Raises malloc error */
	}
	(*prevNext)->next = NULL;


	/* Fill key */
	key = memory->malloc(memory, (keyLen + 1) * sizeof(URI_CHAR));
	if (key == NULL) {
		memory->free(memory, *prevNext);
		*prevNext = NULL;
		return URI_FALSE; /* Raises malloc error */
	}

	key[keyLen] = _UT('\0');
	if (keyLen > 0) {
		/* Copy 1:1 */
		memcpy(key, keyFirst, keyLen * sizeof(URI_CHAR));

		/* Unescape */
		URI_FUNC(UnescapeInPlaceEx)(key, plusToSpace, breakConversion);
	}
	(*prevNext)->key = key;


	/* Fill value */
	if (valueFirst != NULL) {
		value = memory->malloc(memory, (valueLen + 1) * sizeof(URI_CHAR));
		if (value == NULL) {
			memory->free(memory, key);
			memory->free(memory, *prevNext);
			*prevNext = NULL;
			return URI_FALSE; /* Raises malloc error */
		}

		value[valueLen] = _UT('\0');
		if (valueLen > 0) {
			/* Copy 1:1 */
			memcpy(value, valueFirst, valueLen * sizeof(URI_CHAR));

			/* Unescape */
			URI_FUNC(UnescapeInPlaceEx)(value, plusToSpace, breakConversion);
		}
		(*prevNext)->value = value;
	} else {
		value = NULL;
	}
	(*prevNext)->value = value;

	(*itemCount)++;
	return URI_TRUE;
}



void URI_FUNC(FreeQueryList)(URI_TYPE(QueryList) * queryList) {
	URI_FUNC(FreeQueryListMm)(queryList, NULL);
}



int URI_FUNC(FreeQueryListMm)(URI_TYPE(QueryList) * queryList,
		UriMemoryManager * memory) {
	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */
	while (queryList != NULL) {
		URI_TYPE(QueryList) * nextBackup = queryList->next;
		memory->free(memory, (URI_CHAR *)queryList->key); /* const cast */
		memory->free(memory, (URI_CHAR *)queryList->value); /* const cast */
		memory->free(memory, queryList);
		queryList = nextBackup;
	}
	return URI_SUCCESS;
}



int URI_FUNC(DissectQueryMalloc)(URI_TYPE(QueryList) ** dest, int * itemCount,
		const URI_CHAR * first, const URI_CHAR * afterLast) {
	const UriBool plusToSpace = URI_TRUE;
	const UriBreakConversion breakConversion = URI_BR_DONT_TOUCH;

	return URI_FUNC(DissectQueryMallocEx)(dest, itemCount, first, afterLast,
			plusToSpace, breakConversion);
}



int URI_FUNC(DissectQueryMallocEx)(URI_TYPE(QueryList) ** dest, int * itemCount,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriBool plusToSpace, UriBreakConversion breakConversion) {
	return URI_FUNC(DissectQueryMallocExMm)(dest, itemCount, first, afterLast,
			plusToSpace, breakConversion, NULL);
}



int URI_FUNC(DissectQueryMallocExMm)(URI_TYPE(QueryList) ** dest, int * itemCount,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriBool plusToSpace, UriBreakConversion breakConversion,
		UriMemoryManager * memory) {
	const URI_CHAR * walk = first;
	const URI_CHAR * keyFirst = first;
	const URI_CHAR * keyAfter = NULL;
	const URI_CHAR * valueFirst = NULL;
	const URI_CHAR * valueAfter = NULL;
	URI_TYPE(QueryList) ** prevNext = dest;
	int nullCounter;
	int * itemsAppended = (itemCount == NULL) ? &nullCounter : itemCount;

	if ((dest == NULL) || (first == NULL) || (afterLast == NULL)) {
		return URI_ERROR_NULL;
	}

	if (first > afterLast) {
		return URI_ERROR_RANGE_INVALID;
	}

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	*dest = NULL;
	*itemsAppended = 0;

	/* Parse query string */
	for (; walk < afterLast; walk++) {
		switch (*walk) {
		case _UT('&'):
			if (valueFirst != NULL) {
				valueAfter = walk;
			} else {
				keyAfter = walk;
			}

			if (URI_FUNC(AppendQueryItem)(prevNext, itemsAppended,
					keyFirst, keyAfter, valueFirst, valueAfter,
					plusToSpace, breakConversion, memory)
					== URI_FALSE) {
				/* Free list we built */
				*itemsAppended = 0;
				URI_FUNC(FreeQueryListMm)(*dest, memory);
				return URI_ERROR_MALLOC;
			}

			/* Make future items children of the current */
			if ((prevNext != NULL) && (*prevNext != NULL)) {
				prevNext = &((*prevNext)->next);
			}

			if (walk + 1 < afterLast) {
				keyFirst = walk + 1;
			} else {
				keyFirst = NULL;
			}
			keyAfter = NULL;
			valueFirst = NULL;
			valueAfter = NULL;
			break;

		case _UT('='):
			/* NOTE: WE treat the first '=' as a separator, */
			/*       all following go into the value part   */
			if (keyAfter == NULL) {
				keyAfter = walk;
				if (walk + 1 <= afterLast) {
					valueFirst = walk + 1;
					valueAfter = walk + 1;
				}
			}
			break;

		default:
			break;
		}
	}

	if (valueFirst != NULL) {
		/* Must be key/value pair */
		valueAfter = walk;
	} else {
		/* Must be key only */
		keyAfter = walk;
	}

	if (URI_FUNC(AppendQueryItem)(prevNext, itemsAppended, keyFirst, keyAfter,
			valueFirst, valueAfter, plusToSpace, breakConversion, memory)
			== URI_FALSE) {
		/* Free list we built */
		*itemsAppended = 0;
		URI_FUNC(FreeQueryListMm)(*dest, memory);
		return URI_ERROR_MALLOC;
	}

	return URI_SUCCESS;
}



#endif
