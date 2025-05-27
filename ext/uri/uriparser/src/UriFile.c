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
#  include "UriFile.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriFile.c"
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
#endif



#include <stdlib.h>  /* for size_t, avoiding stddef.h for older MSVCs */



static URI_INLINE int URI_FUNC(FilenameToUriString)(const URI_CHAR * filename,
		URI_CHAR * uriString, UriBool fromUnix) {
	const URI_CHAR * input = filename;
	const URI_CHAR * lastSep = input - 1;
	UriBool firstSegment = URI_TRUE;
	URI_CHAR * output = uriString;
	UriBool absolute;
	UriBool is_windows_network;

	if ((filename == NULL) || (uriString == NULL)) {
		return URI_ERROR_NULL;
	}

	is_windows_network = (filename[0] == _UT('\\')) && (filename[1] == _UT('\\'));
	absolute = fromUnix
			? (filename[0] == _UT('/'))
			: (((filename[0] != _UT('\0')) && (filename[1] == _UT(':')))
				|| is_windows_network);

	if (absolute) {
		const URI_CHAR * const prefix = fromUnix
				? _UT("file://")
				: is_windows_network
					? _UT("file:")
					: _UT("file:///");
		const size_t prefixLen = URI_STRLEN(prefix);

		/* Copy prefix */
		memcpy(uriString, prefix, prefixLen * sizeof(URI_CHAR));
		output += prefixLen;
	}

	/* Copy and escape on the fly */
	for (;;) {
		if ((input[0] == _UT('\0'))
				|| (fromUnix && input[0] == _UT('/'))
				|| (!fromUnix && input[0] == _UT('\\'))) {
			/* Copy text after last separator */
			if (lastSep + 1 < input) {
				if (!fromUnix && absolute && (firstSegment == URI_TRUE)) {
					/* Quick hack to not convert "C:" to "C%3A" */
					const int charsToCopy = (int)(input - (lastSep + 1));
					memcpy(output, lastSep + 1, charsToCopy * sizeof(URI_CHAR));
					output += charsToCopy;
				} else {
					output = URI_FUNC(EscapeEx)(lastSep + 1, input, output,
							URI_FALSE, URI_FALSE);
				}
			}
			firstSegment = URI_FALSE;
		}

		if (input[0] == _UT('\0')) {
			output[0] = _UT('\0');
			break;
		} else if (fromUnix && (input[0] == _UT('/'))) {
			/* Copy separators unmodified */
			output[0] = _UT('/');
			output++;
			lastSep = input;
		} else if (!fromUnix && (input[0] == _UT('\\'))) {
			/* Convert backslashes to forward slashes */
			output[0] = _UT('/');
			output++;
			lastSep = input;
		}
		input++;
	}

	return URI_SUCCESS;
}



static URI_INLINE int URI_FUNC(UriStringToFilename)(const URI_CHAR * uriString,
		URI_CHAR * filename, UriBool toUnix) {
	if ((uriString == NULL) || (filename == NULL)) {
		return URI_ERROR_NULL;
	}

	{
		const UriBool file_unknown_slashes =
				URI_STRNCMP(uriString, _UT("file:"), URI_STRLEN(_UT("file:"))) == 0;
		const UriBool file_one_or_more_slashes = file_unknown_slashes
				&& (URI_STRNCMP(uriString, _UT("file:/"), URI_STRLEN(_UT("file:/"))) == 0);
		const UriBool file_two_or_more_slashes = file_one_or_more_slashes
				&& (URI_STRNCMP(uriString, _UT("file://"), URI_STRLEN(_UT("file://"))) == 0);
		const UriBool file_three_or_more_slashes = file_two_or_more_slashes
				&& (URI_STRNCMP(uriString, _UT("file:///"), URI_STRLEN(_UT("file:///"))) == 0);

		const size_t charsToSkip = file_two_or_more_slashes
				? file_three_or_more_slashes
					? toUnix
						/* file:///bin/bash */
						? URI_STRLEN(_UT("file://"))
						/* file:///E:/Documents%20and%20Settings */
						: URI_STRLEN(_UT("file:///"))
					/* file://Server01/Letter.txt */
					: URI_STRLEN(_UT("file://"))
				: ((file_one_or_more_slashes && toUnix)
					/* file:/bin/bash */
					/* https://tools.ietf.org/html/rfc8089#appendix-B */
					? URI_STRLEN(_UT("file:"))
					: ((! toUnix && file_unknown_slashes && ! file_one_or_more_slashes)
						/* file:c:/path/to/file */
						/* https://tools.ietf.org/html/rfc8089#appendix-E.2 */
						? URI_STRLEN(_UT("file:"))
						: 0));
		const size_t charsToCopy = URI_STRLEN(uriString + charsToSkip) + 1;

		const UriBool is_windows_network_with_authority =
				(toUnix == URI_FALSE)
				&& file_two_or_more_slashes
				&& ! file_three_or_more_slashes;

		URI_CHAR * const unescape_target = is_windows_network_with_authority
				? (filename + 2)
				: filename;

		if (is_windows_network_with_authority) {
			filename[0] = '\\';
			filename[1] = '\\';
		}

		memcpy(unescape_target, uriString + charsToSkip, charsToCopy * sizeof(URI_CHAR));
		URI_FUNC(UnescapeInPlaceEx)(filename, URI_FALSE, URI_BR_DONT_TOUCH);
	}

	/* Convert forward slashes to backslashes */
	if (!toUnix) {
		URI_CHAR * walker = filename;
		while (walker[0] != _UT('\0')) {
			if (walker[0] == _UT('/')) {
				walker[0] = _UT('\\');
			}
			walker++;
		}
	}

	return URI_SUCCESS;
}



int URI_FUNC(UnixFilenameToUriString)(const URI_CHAR * filename, URI_CHAR * uriString) {
	return URI_FUNC(FilenameToUriString)(filename, uriString, URI_TRUE);
}



int URI_FUNC(WindowsFilenameToUriString)(const URI_CHAR * filename, URI_CHAR * uriString) {
	return URI_FUNC(FilenameToUriString)(filename, uriString, URI_FALSE);
}



int URI_FUNC(UriStringToUnixFilename)(const URI_CHAR * uriString, URI_CHAR * filename) {
	return URI_FUNC(UriStringToFilename)(uriString, filename, URI_TRUE);
}



int URI_FUNC(UriStringToWindowsFilename)(const URI_CHAR * uriString, URI_CHAR * filename) {
	return URI_FUNC(UriStringToFilename)(uriString, filename, URI_FALSE);
}



#endif
