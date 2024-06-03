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

/**
 * @file UriNormalize.c
 * Holds the RFC 3986 %URI normalization implementation.
 * NOTE: This source file includes itself twice.
 */

/* What encodings are enabled? */
#include <uriparser/UriDefsConfig.h>
#if (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* Include SELF twice */
# ifdef URI_ENABLE_ANSI
#  define URI_PASS_ANSI 1
#  include "UriNormalize.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriNormalize.c"
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
# include "UriNormalizeBase.h"
# include "UriCommon.h"
# include "UriMemory.h"
#endif



#include <assert.h>



static int URI_FUNC(NormalizeSyntaxEngine)(URI_TYPE(Uri) * uri, unsigned int inMask,
		unsigned int * outMask, UriMemoryManager * memory);

static UriBool URI_FUNC(MakeRangeOwner)(unsigned int * doneMask,
		unsigned int maskTest, URI_TYPE(TextRange) * range,
		UriMemoryManager * memory);
static UriBool URI_FUNC(MakeOwnerEngine)(URI_TYPE(Uri) * uri,
		unsigned int * doneMask, UriMemoryManager * memory);

static void URI_FUNC(FixPercentEncodingInplace)(const URI_CHAR * first,
		const URI_CHAR ** afterLast);
static UriBool URI_FUNC(FixPercentEncodingMalloc)(const URI_CHAR ** first,
		const URI_CHAR ** afterLast, UriMemoryManager * memory);
static void URI_FUNC(FixPercentEncodingEngine)(
		const URI_CHAR * inFirst, const URI_CHAR * inAfterLast,
		const URI_CHAR * outFirst, const URI_CHAR ** outAfterLast);

static UriBool URI_FUNC(ContainsUppercaseLetters)(const URI_CHAR * first,
		const URI_CHAR * afterLast);
static UriBool URI_FUNC(ContainsUglyPercentEncoding)(const URI_CHAR * first,
		const URI_CHAR * afterLast);

static void URI_FUNC(LowercaseInplace)(const URI_CHAR * first,
		const URI_CHAR * afterLast);
static UriBool URI_FUNC(LowercaseMalloc)(const URI_CHAR ** first,
		const URI_CHAR ** afterLast, UriMemoryManager * memory);

static void URI_FUNC(PreventLeakage)(URI_TYPE(Uri) * uri,
		unsigned int revertMask, UriMemoryManager * memory);



static URI_INLINE void URI_FUNC(PreventLeakage)(URI_TYPE(Uri) * uri,
		unsigned int revertMask, UriMemoryManager * memory) {
	if (revertMask & URI_NORMALIZE_SCHEME) {
		/* NOTE: A scheme cannot be the empty string
		 *       so no need to compare .first with .afterLast, here. */
		memory->free(memory, (URI_CHAR *)uri->scheme.first);
		uri->scheme.first = NULL;
		uri->scheme.afterLast = NULL;
	}

	if (revertMask & URI_NORMALIZE_USER_INFO) {
		if (uri->userInfo.first != uri->userInfo.afterLast) {
			memory->free(memory, (URI_CHAR *)uri->userInfo.first);
		}
		uri->userInfo.first = NULL;
		uri->userInfo.afterLast = NULL;
	}

	if (revertMask & URI_NORMALIZE_HOST) {
		if (uri->hostData.ipFuture.first != NULL) {
			/* IPvFuture */
			/* NOTE: An IPvFuture address cannot be the empty string
			 *       so no need to compare .first with .afterLast, here. */
			memory->free(memory, (URI_CHAR *)uri->hostData.ipFuture.first);
			uri->hostData.ipFuture.first = NULL;
			uri->hostData.ipFuture.afterLast = NULL;
			uri->hostText.first = NULL;
			uri->hostText.afterLast = NULL;
		} else if (uri->hostText.first != NULL) {
			/* Regname */
			if (uri->hostText.first != uri->hostText.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->hostText.first);
			}
			uri->hostText.first = NULL;
			uri->hostText.afterLast = NULL;
		}
	}

	/* NOTE: Port cannot happen! */

	if (revertMask & URI_NORMALIZE_PATH) {
		URI_TYPE(PathSegment) * walker = uri->pathHead;
		while (walker != NULL) {
			URI_TYPE(PathSegment) * const next = walker->next;
			if (walker->text.afterLast > walker->text.first) {
				memory->free(memory, (URI_CHAR *)walker->text.first);
			}
			memory->free(memory, walker);
			walker = next;
		}
		uri->pathHead = NULL;
		uri->pathTail = NULL;
	}

	if (revertMask & URI_NORMALIZE_QUERY) {
		if (uri->query.first != uri->query.afterLast) {
			memory->free(memory, (URI_CHAR *)uri->query.first);
		}
		uri->query.first = NULL;
		uri->query.afterLast = NULL;
	}

	if (revertMask & URI_NORMALIZE_FRAGMENT) {
		if (uri->fragment.first != uri->fragment.afterLast) {
			memory->free(memory, (URI_CHAR *)uri->fragment.first);
		}
		uri->fragment.first = NULL;
		uri->fragment.afterLast = NULL;
	}
}



static URI_INLINE UriBool URI_FUNC(ContainsUppercaseLetters)(const URI_CHAR * first,
		const URI_CHAR * afterLast) {
	if ((first != NULL) && (afterLast != NULL) && (afterLast > first)) {
		const URI_CHAR * i = first;
		for (; i < afterLast; i++) {
			/* 6.2.2.1 Case Normalization: uppercase letters in scheme or host */
			if ((*i >= _UT('A')) && (*i <= _UT('Z'))) {
				return URI_TRUE;
			}
		}
	}
	return URI_FALSE;
}



static URI_INLINE UriBool URI_FUNC(ContainsUglyPercentEncoding)(const URI_CHAR * first,
		const URI_CHAR * afterLast) {
	if ((first != NULL) && (afterLast != NULL) && (afterLast > first)) {
		const URI_CHAR * i = first;
		for (; i + 2 < afterLast; i++) {
			if (i[0] == _UT('%')) {
				/* 6.2.2.1 Case Normalization: *
				 * lowercase percent-encodings */
				if (((i[1] >= _UT('a')) && (i[1] <= _UT('f')))
						|| ((i[2] >= _UT('a')) && (i[2] <= _UT('f')))) {
					return URI_TRUE;
				} else {
					/* 6.2.2.2 Percent-Encoding Normalization: *
					 * percent-encoded unreserved characters   */
					const unsigned char left = URI_FUNC(HexdigToInt)(i[1]);
					const unsigned char right = URI_FUNC(HexdigToInt)(i[2]);
					const int code = 16 * left + right;
					if (uriIsUnreserved(code)) {
						return URI_TRUE;
					}
				}
			}
		}
	}
	return URI_FALSE;
}



static URI_INLINE void URI_FUNC(LowercaseInplace)(const URI_CHAR * first,
		const URI_CHAR * afterLast) {
	if ((first != NULL) && (afterLast != NULL) && (afterLast > first)) {
		URI_CHAR * i = (URI_CHAR *)first;
		const int lowerUpperDiff = (_UT('a') - _UT('A'));
		for (; i < afterLast; i++) {
			if ((*i >= _UT('A')) && (*i <=_UT('Z'))) {
				*i = (URI_CHAR)(*i + lowerUpperDiff);
			}
		}
	}
}



static URI_INLINE UriBool URI_FUNC(LowercaseMalloc)(const URI_CHAR ** first,
		const URI_CHAR ** afterLast, UriMemoryManager * memory) {
	int lenInChars;
	const int lowerUpperDiff = (_UT('a') - _UT('A'));
	URI_CHAR * buffer;
	int i = 0;

	if ((first == NULL) || (afterLast == NULL) || (*first == NULL)
			|| (*afterLast == NULL)) {
		return URI_FALSE;
	}

	lenInChars = (int)(*afterLast - *first);
	if (lenInChars == 0) {
		return URI_TRUE;
	} else if (lenInChars < 0) {
		return URI_FALSE;
	}

	buffer = memory->malloc(memory, lenInChars * sizeof(URI_CHAR));
	if (buffer == NULL) {
		return URI_FALSE;
	}

	for (; i < lenInChars; i++) {
		if (((*first)[i] >= _UT('A')) && ((*first)[i] <=_UT('Z'))) {
			buffer[i] = (URI_CHAR)((*first)[i] + lowerUpperDiff);
		} else {
			buffer[i] = (*first)[i];
		}
	}

	*first = buffer;
	*afterLast = buffer + lenInChars;
	return URI_TRUE;
}



/* NOTE: Implementation must stay inplace-compatible */
static URI_INLINE void URI_FUNC(FixPercentEncodingEngine)(
		const URI_CHAR * inFirst, const URI_CHAR * inAfterLast,
		const URI_CHAR * outFirst, const URI_CHAR ** outAfterLast) {
	URI_CHAR * write = (URI_CHAR *)outFirst;
	const int lenInChars = (int)(inAfterLast - inFirst);
	int i = 0;

	/* All but last two */
	for (; i + 2 < lenInChars; i++) {
		if (inFirst[i] != _UT('%')) {
			write[0] = inFirst[i];
			write++;
		} else {
			/* 6.2.2.2 Percent-Encoding Normalization: *
			 * percent-encoded unreserved characters   */
			const URI_CHAR one = inFirst[i + 1];
			const URI_CHAR two = inFirst[i + 2];
			const unsigned char left = URI_FUNC(HexdigToInt)(one);
			const unsigned char right = URI_FUNC(HexdigToInt)(two);
			const int code = 16 * left + right;
			if (uriIsUnreserved(code)) {
				write[0] = (URI_CHAR)(code);
				write++;
			} else {
				/* 6.2.2.1 Case Normalization: *
				 * lowercase percent-encodings */
				write[0] = _UT('%');
				write[1] = URI_FUNC(HexToLetter)(left);
				write[2] = URI_FUNC(HexToLetter)(right);
				write += 3;
			}

			i += 2; /* For the two chars of the percent group we just ate */
		}
	}

	/* Last two */
	for (; i < lenInChars; i++) {
		write[0] = inFirst[i];
		write++;
	}

	*outAfterLast = write;
}



static URI_INLINE void URI_FUNC(FixPercentEncodingInplace)(const URI_CHAR * first,
		const URI_CHAR ** afterLast) {
	/* Death checks */
	if ((first == NULL) || (afterLast == NULL) || (*afterLast == NULL)) {
		return;
	}

	/* Fix inplace */
	URI_FUNC(FixPercentEncodingEngine)(first, *afterLast, first, afterLast);
}



static URI_INLINE UriBool URI_FUNC(FixPercentEncodingMalloc)(const URI_CHAR ** first,
		const URI_CHAR ** afterLast, UriMemoryManager * memory) {
	int lenInChars;
	URI_CHAR * buffer;

	/* Death checks */
	if ((first == NULL) || (afterLast == NULL)
			|| (*first == NULL) || (*afterLast == NULL)) {
		return URI_FALSE;
	}

	/* Old text length */
	lenInChars = (int)(*afterLast - *first);
	if (lenInChars == 0) {
		return URI_TRUE;
	} else if (lenInChars < 0) {
		return URI_FALSE;
	}

	/* New buffer */
	buffer = memory->malloc(memory, lenInChars * sizeof(URI_CHAR));
	if (buffer == NULL) {
		return URI_FALSE;
	}

	/* Fix on copy */
	URI_FUNC(FixPercentEncodingEngine)(*first, *afterLast, buffer, afterLast);
	*first = buffer;
	return URI_TRUE;
}



static URI_INLINE UriBool URI_FUNC(MakeRangeOwner)(unsigned int * doneMask,
		unsigned int maskTest, URI_TYPE(TextRange) * range,
		UriMemoryManager * memory) {
	if (((*doneMask & maskTest) == 0)
			&& (range->first != NULL)
			&& (range->afterLast != NULL)
			&& (range->afterLast > range->first)) {
		const int lenInChars = (int)(range->afterLast - range->first);
		const int lenInBytes = lenInChars * sizeof(URI_CHAR);
		URI_CHAR * dup = memory->malloc(memory, lenInBytes);
		if (dup == NULL) {
			return URI_FALSE; /* Raises malloc error */
		}
		memcpy(dup, range->first, lenInBytes);
		range->first = dup;
		range->afterLast = dup + lenInChars;
		*doneMask |= maskTest;
	}
	return URI_TRUE;
}



static URI_INLINE UriBool URI_FUNC(MakeOwnerEngine)(URI_TYPE(Uri) * uri,
		unsigned int * doneMask, UriMemoryManager * memory) {
	URI_TYPE(PathSegment) * walker = uri->pathHead;
	if (!URI_FUNC(MakeRangeOwner)(doneMask, URI_NORMALIZE_SCHEME,
				&(uri->scheme), memory)
			|| !URI_FUNC(MakeRangeOwner)(doneMask, URI_NORMALIZE_USER_INFO,
				&(uri->userInfo), memory)
			|| !URI_FUNC(MakeRangeOwner)(doneMask, URI_NORMALIZE_QUERY,
				&(uri->query), memory)
			|| !URI_FUNC(MakeRangeOwner)(doneMask, URI_NORMALIZE_FRAGMENT,
				&(uri->fragment), memory)) {
		return URI_FALSE; /* Raises malloc error */
	}

	/* Host */
	if ((*doneMask & URI_NORMALIZE_HOST) == 0) {
		if (uri->hostData.ipFuture.first != NULL) {
			/* IPvFuture */
			if (!URI_FUNC(MakeRangeOwner)(doneMask, URI_NORMALIZE_HOST,
					&(uri->hostData.ipFuture), memory)) {
				return URI_FALSE; /* Raises malloc error */
			}
			uri->hostText.first = uri->hostData.ipFuture.first;
			uri->hostText.afterLast = uri->hostData.ipFuture.afterLast;
		} else if (uri->hostText.first != NULL) {
			/* Regname */
			if (!URI_FUNC(MakeRangeOwner)(doneMask, URI_NORMALIZE_HOST,
					&(uri->hostText), memory)) {
				return URI_FALSE; /* Raises malloc error */
			}
		}
	}

	/* Path */
	if ((*doneMask & URI_NORMALIZE_PATH) == 0) {
		while (walker != NULL) {
			if (!URI_FUNC(MakeRangeOwner)(doneMask, 0, &(walker->text), memory)) {
				/* Free allocations done so far and kill path */

				/* Kill path to one before walker (if any) */
				URI_TYPE(PathSegment) * ranger = uri->pathHead;
				while (ranger != walker) {
					URI_TYPE(PathSegment) * const next = ranger->next;
					if ((ranger->text.first != NULL)
							&& (ranger->text.afterLast != NULL)
							&& (ranger->text.afterLast > ranger->text.first)) {
						memory->free(memory, (URI_CHAR *)ranger->text.first);
					}
					memory->free(memory, ranger);
					ranger = next;
				}

				/* Kill path from walker */
				while (walker != NULL) {
					URI_TYPE(PathSegment) * const next = walker->next;
					memory->free(memory, walker);
					walker = next;
				}

				uri->pathHead = NULL;
				uri->pathTail = NULL;
				return URI_FALSE; /* Raises malloc error */
			}
			walker = walker->next;
		}
		*doneMask |= URI_NORMALIZE_PATH;
	}

	/* Port text, must come last so we don't have to undo that one if it fails. *
	 * Otherwise we would need and extra enum flag for it although the port      *
	 * cannot go unnormalized...                                                */
	if (!URI_FUNC(MakeRangeOwner)(doneMask, 0, &(uri->portText), memory)) {
		return URI_FALSE; /* Raises malloc error */
	}

	return URI_TRUE;
}



unsigned int URI_FUNC(NormalizeSyntaxMaskRequired)(const URI_TYPE(Uri) * uri) {
	unsigned int outMask = URI_NORMALIZED;  /* for NULL uri */
	URI_FUNC(NormalizeSyntaxMaskRequiredEx)(uri, &outMask);
	return outMask;
}



int URI_FUNC(NormalizeSyntaxMaskRequiredEx)(const URI_TYPE(Uri) * uri,
		unsigned int * outMask) {
	UriMemoryManager * const memory = NULL;  /* no use of memory manager */

#if defined(__GNUC__) && ((__GNUC__ > 4) \
		|| ((__GNUC__ == 4) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 2)))
	/* Slower code that fixes a warning, not sure if this is a smart idea */
	URI_TYPE(Uri) writeableClone;
#endif

	if ((uri == NULL) || (outMask == NULL)) {
		return URI_ERROR_NULL;
	}

#if defined(__GNUC__) && ((__GNUC__ > 4) \
		|| ((__GNUC__ == 4) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 2)))
	/* Slower code that fixes a warning, not sure if this is a smart idea */
	memcpy(&writeableClone, uri, 1 * sizeof(URI_TYPE(Uri)));
	URI_FUNC(NormalizeSyntaxEngine)(&writeableClone, 0, outMask, memory);
#else
	URI_FUNC(NormalizeSyntaxEngine)((URI_TYPE(Uri) *)uri, 0, outMask, memory);
#endif
	return URI_SUCCESS;
}



int URI_FUNC(NormalizeSyntaxEx)(URI_TYPE(Uri) * uri, unsigned int mask) {
	return URI_FUNC(NormalizeSyntaxExMm)(uri, mask, NULL);
}



int URI_FUNC(NormalizeSyntaxExMm)(URI_TYPE(Uri) * uri, unsigned int mask,
		UriMemoryManager * memory) {
	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */
	return URI_FUNC(NormalizeSyntaxEngine)(uri, mask, NULL, memory);
}



int URI_FUNC(NormalizeSyntax)(URI_TYPE(Uri) * uri) {
	return URI_FUNC(NormalizeSyntaxEx)(uri, (unsigned int)-1);
}



static URI_INLINE int URI_FUNC(NormalizeSyntaxEngine)(URI_TYPE(Uri) * uri,
		unsigned int inMask, unsigned int * outMask,
		UriMemoryManager * memory) {
	unsigned int doneMask = URI_NORMALIZED;

	/* Not just doing inspection? -> memory manager required! */
	if (outMask == NULL) {
		assert(memory != NULL);
	}

	if (uri == NULL) {
		if (outMask != NULL) {
			*outMask = URI_NORMALIZED;
			return URI_SUCCESS;
		} else {
			return URI_ERROR_NULL;
		}
	}

	if (outMask != NULL) {
		/* Reset mask */
		*outMask = URI_NORMALIZED;
	} else if (inMask == URI_NORMALIZED) {
		/* Nothing to do */
		return URI_SUCCESS;
	}

	/* Scheme, host */
	if (outMask != NULL) {
		const UriBool normalizeScheme = URI_FUNC(ContainsUppercaseLetters)(
				uri->scheme.first, uri->scheme.afterLast);
		const UriBool normalizeHostCase = URI_FUNC(ContainsUppercaseLetters)(
			uri->hostText.first, uri->hostText.afterLast);
		if (normalizeScheme) {
			*outMask |= URI_NORMALIZE_SCHEME;
		}

		if (normalizeHostCase) {
			*outMask |= URI_NORMALIZE_HOST;
		} else {
			const UriBool normalizeHostPrecent = URI_FUNC(ContainsUglyPercentEncoding)(
					uri->hostText.first, uri->hostText.afterLast);
			if (normalizeHostPrecent) {
				*outMask |= URI_NORMALIZE_HOST;
			}
		}
	} else {
		/* Scheme */
		if ((inMask & URI_NORMALIZE_SCHEME) && (uri->scheme.first != NULL)) {
			if (uri->owner) {
				URI_FUNC(LowercaseInplace)(uri->scheme.first, uri->scheme.afterLast);
			} else {
				if (!URI_FUNC(LowercaseMalloc)(&(uri->scheme.first), &(uri->scheme.afterLast), memory)) {
					URI_FUNC(PreventLeakage)(uri, doneMask, memory);
					return URI_ERROR_MALLOC;
				}
				doneMask |= URI_NORMALIZE_SCHEME;
			}
		}

		/* Host */
		if (inMask & URI_NORMALIZE_HOST) {
			if (uri->hostData.ipFuture.first != NULL) {
				/* IPvFuture */
				if (uri->owner) {
					URI_FUNC(LowercaseInplace)(uri->hostData.ipFuture.first,
							uri->hostData.ipFuture.afterLast);
				} else {
					if (!URI_FUNC(LowercaseMalloc)(&(uri->hostData.ipFuture.first),
							&(uri->hostData.ipFuture.afterLast), memory)) {
						URI_FUNC(PreventLeakage)(uri, doneMask, memory);
						return URI_ERROR_MALLOC;
					}
					doneMask |= URI_NORMALIZE_HOST;
				}
				uri->hostText.first = uri->hostData.ipFuture.first;
				uri->hostText.afterLast = uri->hostData.ipFuture.afterLast;
			} else if ((uri->hostText.first != NULL)
					&& (uri->hostData.ip4 == NULL)
					&& (uri->hostData.ip6 == NULL)) {
				/* Regname */
				if (uri->owner) {
					URI_FUNC(FixPercentEncodingInplace)(uri->hostText.first,
							&(uri->hostText.afterLast));
				} else {
					if (!URI_FUNC(FixPercentEncodingMalloc)(
							&(uri->hostText.first),
							&(uri->hostText.afterLast),
							memory)) {
						URI_FUNC(PreventLeakage)(uri, doneMask, memory);
						return URI_ERROR_MALLOC;
					}
					doneMask |= URI_NORMALIZE_HOST;
				}

				URI_FUNC(LowercaseInplace)(uri->hostText.first,
						uri->hostText.afterLast);
			}
		}
	}

	/* User info */
	if (outMask != NULL) {
		const UriBool normalizeUserInfo = URI_FUNC(ContainsUglyPercentEncoding)(
			uri->userInfo.first, uri->userInfo.afterLast);
		if (normalizeUserInfo) {
			*outMask |= URI_NORMALIZE_USER_INFO;
		}
	} else {
		if ((inMask & URI_NORMALIZE_USER_INFO) && (uri->userInfo.first != NULL)) {
			if (uri->owner) {
				URI_FUNC(FixPercentEncodingInplace)(uri->userInfo.first, &(uri->userInfo.afterLast));
			} else {
				if (!URI_FUNC(FixPercentEncodingMalloc)(&(uri->userInfo.first),
						&(uri->userInfo.afterLast), memory)) {
					URI_FUNC(PreventLeakage)(uri, doneMask, memory);
					return URI_ERROR_MALLOC;
				}
				doneMask |= URI_NORMALIZE_USER_INFO;
			}
		}
	}

	/* Path */
	if (outMask != NULL) {
		const URI_TYPE(PathSegment) * walker = uri->pathHead;
		while (walker != NULL) {
			const URI_CHAR * const first = walker->text.first;
			const URI_CHAR * const afterLast = walker->text.afterLast;
			if ((first != NULL)
					&& (afterLast != NULL)
					&& (afterLast > first)
					&& (
						(((afterLast - first) == 1)
							&& (first[0] == _UT('.')))
						||
						(((afterLast - first) == 2)
							&& (first[0] == _UT('.'))
							&& (first[1] == _UT('.')))
						||
						URI_FUNC(ContainsUglyPercentEncoding)(first, afterLast)
					)) {
				*outMask |= URI_NORMALIZE_PATH;
				break;
			}
			walker = walker->next;
		}
	} else if (inMask & URI_NORMALIZE_PATH) {
		URI_TYPE(PathSegment) * walker;
		const UriBool relative = ((uri->scheme.first == NULL)
				&& !uri->absolutePath) ? URI_TRUE : URI_FALSE;

		/* Fix percent-encoding for each segment */
		walker = uri->pathHead;
		if (uri->owner) {
			while (walker != NULL) {
				URI_FUNC(FixPercentEncodingInplace)(walker->text.first, &(walker->text.afterLast));
				walker = walker->next;
			}
		} else {
			while (walker != NULL) {
				if (!URI_FUNC(FixPercentEncodingMalloc)(&(walker->text.first),
						&(walker->text.afterLast), memory)) {
					URI_FUNC(PreventLeakage)(uri, doneMask, memory);
					return URI_ERROR_MALLOC;
				}
				walker = walker->next;
			}
			doneMask |= URI_NORMALIZE_PATH;
		}

		/* 6.2.2.3 Path Segment Normalization */
		if (!URI_FUNC(RemoveDotSegmentsEx)(uri, relative,
				(uri->owner == URI_TRUE)
				|| ((doneMask & URI_NORMALIZE_PATH) != 0),
				memory)) {
			URI_FUNC(PreventLeakage)(uri, doneMask, memory);
			return URI_ERROR_MALLOC;
		}
		URI_FUNC(FixEmptyTrailSegment)(uri, memory);
	}

	/* Query, fragment */
	if (outMask != NULL) {
		const UriBool normalizeQuery = URI_FUNC(ContainsUglyPercentEncoding)(
				uri->query.first, uri->query.afterLast);
		const UriBool normalizeFragment = URI_FUNC(ContainsUglyPercentEncoding)(
				uri->fragment.first, uri->fragment.afterLast);
		if (normalizeQuery) {
			*outMask |= URI_NORMALIZE_QUERY;
		}

		if (normalizeFragment) {
			*outMask |= URI_NORMALIZE_FRAGMENT;
		}
	} else {
		/* Query */
		if ((inMask & URI_NORMALIZE_QUERY) && (uri->query.first != NULL)) {
			if (uri->owner) {
				URI_FUNC(FixPercentEncodingInplace)(uri->query.first, &(uri->query.afterLast));
			} else {
				if (!URI_FUNC(FixPercentEncodingMalloc)(&(uri->query.first),
						&(uri->query.afterLast), memory)) {
					URI_FUNC(PreventLeakage)(uri, doneMask, memory);
					return URI_ERROR_MALLOC;
				}
				doneMask |= URI_NORMALIZE_QUERY;
			}
		}

		/* Fragment */
		if ((inMask & URI_NORMALIZE_FRAGMENT) && (uri->fragment.first != NULL)) {
			if (uri->owner) {
				URI_FUNC(FixPercentEncodingInplace)(uri->fragment.first, &(uri->fragment.afterLast));
			} else {
				if (!URI_FUNC(FixPercentEncodingMalloc)(&(uri->fragment.first),
						&(uri->fragment.afterLast), memory)) {
					URI_FUNC(PreventLeakage)(uri, doneMask, memory);
					return URI_ERROR_MALLOC;
				}
				doneMask |= URI_NORMALIZE_FRAGMENT;
			}
		}
	}

	/* Dup all not duped yet */
	if ((outMask == NULL) && !uri->owner) {
		if (!URI_FUNC(MakeOwnerEngine)(uri, &doneMask, memory)) {
			URI_FUNC(PreventLeakage)(uri, doneMask, memory);
			return URI_ERROR_MALLOC;
		}
		uri->owner = URI_TRUE;
	}

	return URI_SUCCESS;
}



int URI_FUNC(MakeOwnerMm)(URI_TYPE(Uri) * uri, UriMemoryManager * memory) {
	unsigned int doneMask = URI_NORMALIZED;

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	if (uri == NULL) {
		return URI_ERROR_NULL;
	}

	if (uri->owner == URI_TRUE) {
		return URI_SUCCESS;
	}

	if (! URI_FUNC(MakeOwnerEngine)(uri, &doneMask, memory)) {
		URI_FUNC(PreventLeakage)(uri, doneMask, memory);
		return URI_ERROR_MALLOC;
	}

	uri->owner = URI_TRUE;

	return URI_SUCCESS;
}



int URI_FUNC(MakeOwner)(URI_TYPE(Uri) * uri) {
	return URI_FUNC(MakeOwnerMm)(uri, NULL);
}



#endif
