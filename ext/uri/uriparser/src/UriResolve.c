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
#  include "UriResolve.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriResolve.c"
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



/* Appends a relative URI to an absolute. The last path segment of
 * the absolute URI is replaced. */
static URI_INLINE UriBool URI_FUNC(MergePath)(URI_TYPE(Uri) * absWork,
		const URI_TYPE(Uri) * relAppend, UriMemoryManager * memory) {
	URI_TYPE(PathSegment) * sourceWalker;
	URI_TYPE(PathSegment) * destPrev;
	if (relAppend->pathHead == NULL) {
		return URI_TRUE;
	}

	/* Replace last segment ("" if trailing slash) with first of append chain */
	if (absWork->pathHead == NULL) {
		URI_TYPE(PathSegment) * const dup = memory->malloc(memory, sizeof(URI_TYPE(PathSegment)));
		if (dup == NULL) {
			return URI_FALSE; /* Raises malloc error */
		}
		dup->next = NULL;
		absWork->pathHead = dup;
		absWork->pathTail = dup;
	}
	absWork->pathTail->text.first = relAppend->pathHead->text.first;
	absWork->pathTail->text.afterLast = relAppend->pathHead->text.afterLast;

	/* Append all the others */
	sourceWalker = relAppend->pathHead->next;
	if (sourceWalker == NULL) {
		return URI_TRUE;
	}
	destPrev = absWork->pathTail;

	for (;;) {
		URI_TYPE(PathSegment) * const dup = memory->malloc(memory, sizeof(URI_TYPE(PathSegment)));
		if (dup == NULL) {
			destPrev->next = NULL;
			absWork->pathTail = destPrev;
			return URI_FALSE; /* Raises malloc error */
		}
		dup->text = sourceWalker->text;
		destPrev->next = dup;

		if (sourceWalker->next == NULL) {
			absWork->pathTail = dup;
			absWork->pathTail->next = NULL;
			break;
		}
		destPrev = dup;
		sourceWalker = sourceWalker->next;
	}

	return URI_TRUE;
}


static int URI_FUNC(ResolveAbsolutePathFlag)(URI_TYPE(Uri) * absWork,
		UriMemoryManager * memory) {
	if (absWork == NULL) {
		return URI_ERROR_NULL;
	}

	if (URI_FUNC(IsHostSet)(absWork) && absWork->absolutePath) {
		/* Empty segment needed, instead? */
		if (absWork->pathHead == NULL) {
			URI_TYPE(PathSegment) * const segment = memory->malloc(memory, sizeof(URI_TYPE(PathSegment)));
			if (segment == NULL) {
				return URI_ERROR_MALLOC;
			}
			segment->text.first = URI_FUNC(SafeToPointTo);
			segment->text.afterLast = URI_FUNC(SafeToPointTo);
			segment->next = NULL;

			absWork->pathHead = segment;
			absWork->pathTail = segment;
		}

		absWork->absolutePath = URI_FALSE;
	}

	return URI_SUCCESS;
}


static int URI_FUNC(AddBaseUriImpl)(URI_TYPE(Uri) * absDest,
		const URI_TYPE(Uri) * relSource,
		const URI_TYPE(Uri) * absBase,
		UriResolutionOptions options, UriMemoryManager * memory) {
	UriBool relSourceHasScheme;

	if (absDest == NULL) {
		return URI_ERROR_NULL;
	}
	URI_FUNC(ResetUri)(absDest);

	if ((relSource == NULL) || (absBase == NULL)) {
		return URI_ERROR_NULL;
	}

	/* absBase absolute? */
	if (absBase->scheme.first == NULL) {
		return URI_ERROR_ADDBASE_REL_BASE;
	}

	/* [00/32] 	-- A non-strict parser may ignore a scheme in the reference */
	/* [00/32] 	-- if it is identical to the base URI's scheme. */
	/* [00/32] 	if ((not strict) and (R.scheme == Base.scheme)) then */
	relSourceHasScheme = (relSource->scheme.first != NULL) ? URI_TRUE : URI_FALSE;
	if ((options & URI_RESOLVE_IDENTICAL_SCHEME_COMPAT)
			&& (absBase->scheme.first != NULL)
			&& (relSource->scheme.first != NULL)
			&& (0 == URI_FUNC(CompareRange)(&(absBase->scheme), &(relSource->scheme)))) {
	/* [00/32] 		undefine(R.scheme); */
		relSourceHasScheme = URI_FALSE;
	/* [00/32] 	endif; */
	}

	/* [01/32]	if defined(R.scheme) then */
				if (relSourceHasScheme) {
	/* [02/32]		T.scheme = R.scheme; */
					absDest->scheme = relSource->scheme;
	/* [03/32]		T.authority = R.authority; */
					if (!URI_FUNC(CopyAuthority)(absDest, relSource, memory)) {
						return URI_ERROR_MALLOC;
					}
	/* [04/32]		T.path = remove_dot_segments(R.path); */
					if (!URI_FUNC(CopyPath)(absDest, relSource, memory)) {
						return URI_ERROR_MALLOC;
					}
					if (!URI_FUNC(RemoveDotSegmentsAbsolute)(absDest, memory)) {
						return URI_ERROR_MALLOC;
					}
	/* [05/32]		T.query = R.query; */
					absDest->query = relSource->query;
	/* [06/32]	else */
				} else {
	/* [07/32]		if defined(R.authority) then */
					if (URI_FUNC(IsHostSet)(relSource)) {
	/* [08/32]			T.authority = R.authority; */
						if (!URI_FUNC(CopyAuthority)(absDest, relSource, memory)) {
							return URI_ERROR_MALLOC;
						}
	/* [09/32]			T.path = remove_dot_segments(R.path); */
						if (!URI_FUNC(CopyPath)(absDest, relSource, memory)) {
							return URI_ERROR_MALLOC;
						}
						if (!URI_FUNC(RemoveDotSegmentsAbsolute)(absDest, memory)) {
							return URI_ERROR_MALLOC;
						}
	/* [10/32]			T.query = R.query; */
						absDest->query = relSource->query;
	/* [11/32]		else */
					} else {
	/* [28/32]			T.authority = Base.authority; */
						if (!URI_FUNC(CopyAuthority)(absDest, absBase, memory)) {
							return URI_ERROR_MALLOC;
						}
	/* [12/32]			if (R.path == "") then */
						if (relSource->pathHead == NULL && !relSource->absolutePath) {
	/* [13/32]				T.path = Base.path; */
							if (!URI_FUNC(CopyPath)(absDest, absBase, memory)) {
								return URI_ERROR_MALLOC;
							}
	/* [14/32]				if defined(R.query) then */
							if (relSource->query.first != NULL) {
	/* [15/32]					T.query = R.query; */
								absDest->query = relSource->query;
	/* [16/32]				else */
							} else {
	/* [17/32]					T.query = Base.query; */
								absDest->query = absBase->query;
	/* [18/32]				endif; */
							}
	/* [19/32]			else */
						} else {
	/* [20/32]				if (R.path starts-with "/") then */
							if (relSource->absolutePath) {
								int res;
	/* [21/32]					T.path = remove_dot_segments(R.path); */
								if (!URI_FUNC(CopyPath)(absDest, relSource, memory)) {
									return URI_ERROR_MALLOC;
								}
								res = URI_FUNC(ResolveAbsolutePathFlag)(absDest, memory);
								if (res != URI_SUCCESS) {
									return res;
								}
								if (!URI_FUNC(RemoveDotSegmentsAbsolute)(absDest, memory)) {
									return URI_ERROR_MALLOC;
								}
	/* [22/32]				else */
							} else {
	/* [23/32]					T.path = merge(Base.path, R.path); */
								if (!URI_FUNC(CopyPath)(absDest, absBase, memory)) {
									return URI_ERROR_MALLOC;
								}
								if (!URI_FUNC(MergePath)(absDest, relSource, memory)) {
									return URI_ERROR_MALLOC;
								}
	/* [24/32]					T.path = remove_dot_segments(T.path); */
								if (!URI_FUNC(RemoveDotSegmentsAbsolute)(absDest, memory)) {
									return URI_ERROR_MALLOC;
								}

								if (!URI_FUNC(FixAmbiguity)(absDest, memory)) {
									return URI_ERROR_MALLOC;
								}
	/* [25/32]				endif; */
							}
	/* [26/32]				T.query = R.query; */
							absDest->query = relSource->query;
	/* [27/32]			endif; */
						}
						URI_FUNC(FixEmptyTrailSegment)(absDest, memory);
	/* [29/32]		endif; */
					}
	/* [30/32]		T.scheme = Base.scheme; */
					absDest->scheme = absBase->scheme;
	/* [31/32]	endif; */
				}
	/* [32/32]	T.fragment = R.fragment; */
				absDest->fragment = relSource->fragment;

	return URI_SUCCESS;

}



int URI_FUNC(AddBaseUri)(URI_TYPE(Uri) * absDest,
		const URI_TYPE(Uri) * relSource, const URI_TYPE(Uri) * absBase) {
	const UriResolutionOptions options = URI_RESOLVE_STRICTLY;
	return URI_FUNC(AddBaseUriEx)(absDest, relSource, absBase, options);
}



int URI_FUNC(AddBaseUriEx)(URI_TYPE(Uri) * absDest,
		const URI_TYPE(Uri) * relSource, const URI_TYPE(Uri) * absBase,
		UriResolutionOptions options) {
	return URI_FUNC(AddBaseUriExMm)(absDest, relSource, absBase, options, NULL);
}



int URI_FUNC(AddBaseUriExMm)(URI_TYPE(Uri) * absDest,
		const URI_TYPE(Uri) * relSource, const URI_TYPE(Uri) * absBase,
		UriResolutionOptions options, UriMemoryManager * memory) {
	int res;

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	res = URI_FUNC(AddBaseUriImpl)(absDest, relSource, absBase, options, memory);
	if ((res != URI_SUCCESS) && (absDest != NULL)) {
		URI_FUNC(FreeUriMembersMm)(absDest, memory);
	}
	return res;
}



#endif
