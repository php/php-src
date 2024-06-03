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
#  include "UriShorten.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriShorten.c"
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



static URI_INLINE UriBool URI_FUNC(AppendSegment)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	/* Create segment */
	URI_TYPE(PathSegment) * segment = memory->malloc(memory, 1 * sizeof(URI_TYPE(PathSegment)));
	if (segment == NULL) {
		return URI_FALSE; /* Raises malloc error */
	}
	segment->next = NULL;
	segment->text.first = first;
	segment->text.afterLast = afterLast;

	/* Put into chain */
	if (uri->pathTail == NULL) {
		uri->pathHead = segment;
	} else {
		uri->pathTail->next = segment;
	}
	uri->pathTail = segment;

	return URI_TRUE;
}



static URI_INLINE UriBool URI_FUNC(EqualsAuthority)(const URI_TYPE(Uri) * first,
		const URI_TYPE(Uri) * second) {
	/* IPv4 */
	if (first->hostData.ip4 != NULL) {
		return ((second->hostData.ip4 != NULL)
				&& !memcmp(first->hostData.ip4->data,
					second->hostData.ip4->data, 4)) ? URI_TRUE : URI_FALSE;
	}

	/* IPv6 */
	if (first->hostData.ip6 != NULL) {
		return ((second->hostData.ip6 != NULL)
				&& !memcmp(first->hostData.ip6->data,
					second->hostData.ip6->data, 16)) ? URI_TRUE : URI_FALSE;
	}

	/* IPvFuture */
	if (first->hostData.ipFuture.first != NULL) {
		return ((second->hostData.ipFuture.first != NULL)
				&& !URI_FUNC(CompareRange)(&first->hostData.ipFuture,
					&second->hostData.ipFuture)) ? URI_TRUE : URI_FALSE;
	}

	return !URI_FUNC(CompareRange)(&first->hostText, &second->hostText)
			? URI_TRUE : URI_FALSE;
}



static int URI_FUNC(RemoveBaseUriImpl)(URI_TYPE(Uri) * dest,
		const URI_TYPE(Uri) * absSource,
		const URI_TYPE(Uri) * absBase,
		UriBool domainRootMode, UriMemoryManager * memory) {
	if (dest == NULL) {
		return URI_ERROR_NULL;
	}
	URI_FUNC(ResetUri)(dest);

	if ((absSource == NULL) || (absBase == NULL)) {
		return URI_ERROR_NULL;
	}

	/* absBase absolute? */
	if (absBase->scheme.first == NULL) {
		return URI_ERROR_REMOVEBASE_REL_BASE;
	}

	/* absSource absolute? */
	if (absSource->scheme.first == NULL) {
		return URI_ERROR_REMOVEBASE_REL_SOURCE;
	}

	/* [01/50]	if (A.scheme != Base.scheme) then */
				if (URI_FUNC(CompareRange)(&absSource->scheme, &absBase->scheme)) {
	/* [02/50]	   T.scheme    = A.scheme; */
					dest->scheme = absSource->scheme;
	/* [03/50]	   T.authority = A.authority; */
					if (!URI_FUNC(CopyAuthority)(dest, absSource, memory)) {
						return URI_ERROR_MALLOC;
					}
	/* [04/50]	   T.path      = A.path; */
					if (!URI_FUNC(CopyPath)(dest, absSource, memory)) {
						return URI_ERROR_MALLOC;
					}
	/* [05/50]	else */
				} else {
	/* [06/50]	   undef(T.scheme); */
					/* NOOP */
	/* [07/50]	   if (A.authority != Base.authority) then */
					if (!URI_FUNC(EqualsAuthority)(absSource, absBase)) {
	/* [08/50]	      T.authority = A.authority; */
						if (!URI_FUNC(CopyAuthority)(dest, absSource, memory)) {
							return URI_ERROR_MALLOC;
						}
	/* [09/50]	      T.path      = A.path; */
						if (!URI_FUNC(CopyPath)(dest, absSource, memory)) {
							return URI_ERROR_MALLOC;
						}
	/* [10/50]	   else */
					} else {
	/* [11/50]	      if domainRootMode then */
						if (domainRootMode == URI_TRUE) {
	/* [12/50]	         undef(T.authority); */
							/* NOOP */
	/* [13/50]	         if (first(A.path) == "") then */
							/* GROUPED */
	/* [14/50]	            T.path   = "/." + A.path; */
								/* GROUPED */
	/* [15/50]	         else */
								/* GROUPED */
	/* [16/50]	            T.path   = A.path; */
								/* GROUPED */
	/* [17/50]	         endif; */
							if (!URI_FUNC(CopyPath)(dest, absSource, memory)) {
								return URI_ERROR_MALLOC;
							}
							dest->absolutePath = URI_TRUE;

							if (!URI_FUNC(FixAmbiguity)(dest, memory)) {
								return URI_ERROR_MALLOC;
							}
	/* [18/50]	      else */
						} else {
							const URI_TYPE(PathSegment) * sourceSeg = absSource->pathHead;
							const URI_TYPE(PathSegment) * baseSeg = absBase->pathHead;
	/* [19/50]	         bool pathNaked = true; */
							UriBool pathNaked = URI_TRUE;
	/* [20/50]	         undef(last(Base.path)); */
							/* NOOP */
	/* [21/50]	         T.path = ""; */
							dest->absolutePath = URI_FALSE;
	/* [22/50]	         while (first(A.path) == first(Base.path)) do */
							while ((sourceSeg != NULL) && (baseSeg != NULL)
									&& !URI_FUNC(CompareRange)(&sourceSeg->text, &baseSeg->text)
									&& !((sourceSeg->text.first == sourceSeg->text.afterLast)
										&& ((sourceSeg->next == NULL) != (baseSeg->next == NULL)))) {
	/* [23/50]	            A.path++; */
								sourceSeg = sourceSeg->next;
	/* [24/50]	            Base.path++; */
								baseSeg = baseSeg->next;
	/* [25/50]	         endwhile; */
							}
	/* [26/50]	         while defined(first(Base.path)) do */
							while ((baseSeg != NULL) && (baseSeg->next != NULL)) {
	/* [27/50]	            Base.path++; */
								baseSeg = baseSeg->next;
	/* [28/50]	            T.path += "../"; */
								if (!URI_FUNC(AppendSegment)(dest, URI_FUNC(ConstParent),
										URI_FUNC(ConstParent) + 2, memory)) {
									return URI_ERROR_MALLOC;
								}
	/* [29/50]	            pathNaked = false; */
								pathNaked = URI_FALSE;
	/* [30/50]	         endwhile; */
							}
	/* [31/50]	         while defined(first(A.path)) do */
							while (sourceSeg != NULL) {
	/* [32/50]	            if pathNaked then */
								if (pathNaked == URI_TRUE) {
	/* [33/50]	               if (first(A.path) contains ":") then */
									UriBool containsColon = URI_FALSE;
									const URI_CHAR * ch = sourceSeg->text.first;
									for (; ch < sourceSeg->text.afterLast; ch++) {
										if (*ch == _UT(':')) {
											containsColon = URI_TRUE;
											break;
										}
									}

									if (containsColon) {
	/* [34/50]	                  T.path += "./"; */
										if (!URI_FUNC(AppendSegment)(dest, URI_FUNC(ConstPwd),
												URI_FUNC(ConstPwd) + 1, memory)) {
											return URI_ERROR_MALLOC;
										}
	/* [35/50]	               elseif (first(A.path) == "") then */
									} else if (sourceSeg->text.first == sourceSeg->text.afterLast) {
	/* [36/50]	                  T.path += "/."; */
										if (!URI_FUNC(AppendSegment)(dest, URI_FUNC(ConstPwd),
												URI_FUNC(ConstPwd) + 1, memory)) {
											return URI_ERROR_MALLOC;
										}
	/* [37/50]	               endif; */
									}
	/* [38/50]	            endif; */
								}
	/* [39/50]	            T.path += first(A.path); */
								if (!URI_FUNC(AppendSegment)(dest, sourceSeg->text.first,
										sourceSeg->text.afterLast, memory)) {
									return URI_ERROR_MALLOC;
								}
	/* [40/50]	            pathNaked = false; */
								pathNaked = URI_FALSE;
	/* [41/50]	            A.path++; */
								sourceSeg = sourceSeg->next;
	/* [42/50]	            if defined(first(A.path)) then */
								/* NOOP */
	/* [43/50]	               T.path += + "/"; */
								/* NOOP */
	/* [44/50]	            endif; */
								/* NOOP */
	/* [45/50]	         endwhile; */
							}
	/* [46/50]	      endif; */
						}
	/* [47/50]	   endif; */
					}
	/* [48/50]	endif; */
				}
	/* [49/50]	T.query     = A.query; */
				dest->query = absSource->query;
	/* [50/50]	T.fragment  = A.fragment; */
				dest->fragment = absSource->fragment;

	return URI_SUCCESS;
}



int URI_FUNC(RemoveBaseUri)(URI_TYPE(Uri) * dest,
		const URI_TYPE(Uri) * absSource,
		const URI_TYPE(Uri) * absBase,
		UriBool domainRootMode) {
	return URI_FUNC(RemoveBaseUriMm)(dest, absSource, absBase,
			domainRootMode, NULL);
}



int URI_FUNC(RemoveBaseUriMm)(URI_TYPE(Uri) * dest,
		const URI_TYPE(Uri) * absSource,
		const URI_TYPE(Uri) * absBase,
		UriBool domainRootMode, UriMemoryManager * memory) {
	int res;

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	res = URI_FUNC(RemoveBaseUriImpl)(dest, absSource,
			absBase, domainRootMode, memory);
	if ((res != URI_SUCCESS) && (dest != NULL)) {
		URI_FUNC(FreeUriMembersMm)(dest, memory);
	}
	return res;
}



#endif
