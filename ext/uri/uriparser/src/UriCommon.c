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
#  include "UriCommon.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriCommon.c"
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
#endif



/*extern*/ const URI_CHAR * const URI_FUNC(SafeToPointTo) = _UT("X");
/*extern*/ const URI_CHAR * const URI_FUNC(ConstPwd) = _UT(".");
/*extern*/ const URI_CHAR * const URI_FUNC(ConstParent) = _UT("..");



void URI_FUNC(ResetUri)(URI_TYPE(Uri) * uri) {
	if (uri == NULL) {
		return;
	}
	memset(uri, 0, sizeof(URI_TYPE(Uri)));
}



/* Compares two text ranges for equal text content */
int URI_FUNC(CompareRange)(
		const URI_TYPE(TextRange) * a,
		const URI_TYPE(TextRange) * b) {
	int diff;

	/* NOTE: Both NULL means equal! */
	if ((a == NULL) || (b == NULL)) {
		return ((a == NULL) ? 0 : 1) - ((b == NULL) ? 0 : 1);
	}

	/* NOTE: Both NULL means equal! */
	if ((a->first == NULL) || (b->first == NULL)) {
		return ((a->first == NULL) ? 0 : 1) - ((b->first == NULL) ? 0 : 1);
	}

	diff = ((int)(a->afterLast - a->first) - (int)(b->afterLast - b->first));
	if (diff > 0) {
		return 1;
	} else if (diff < 0) {
		return -1;
	}

	diff = URI_STRNCMP(a->first, b->first, (a->afterLast - a->first));

	if (diff > 0) {
		return 1;
	} else if (diff < 0) {
		return -1;
	}

	return diff;
}



UriBool URI_FUNC(RemoveDotSegmentsEx)(URI_TYPE(Uri) * uri,
		UriBool relative, UriBool pathOwned, UriMemoryManager * memory) {
	URI_TYPE(PathSegment) * walker;
	if ((uri == NULL) || (uri->pathHead == NULL)) {
		return URI_TRUE;
	}

	walker = uri->pathHead;
	walker->reserved = NULL; /* Prev pointer */
	do {
		UriBool removeSegment = URI_FALSE;
		int len = (int)(walker->text.afterLast - walker->text.first);
		switch (len) {
		case 1:
			if ((walker->text.first)[0] == _UT('.')) {
				/* "." segment -> remove if not essential */
				URI_TYPE(PathSegment) * const prev = walker->reserved;
				URI_TYPE(PathSegment) * const nextBackup = walker->next;

				/*
				 * Is this dot segment essential,
				 * i.e. is there a chance of changing semantics by dropping this dot segment?
				 *
				 * For example, changing "./http://foo" into "http://foo" would change semantics
				 * and hence the dot segment is essential to that case and cannot be removed.
				 */
				removeSegment = URI_TRUE;
				if (relative && (walker == uri->pathHead) && (walker->next != NULL)) {
					const URI_CHAR * ch = walker->next->text.first;
					for (; ch < walker->next->text.afterLast; ch++) {
						if (*ch == _UT(':')) {
							removeSegment = URI_FALSE;
							break;
						}
					}
				}

				if (removeSegment) {
					/* .. then let's go remove that segment. */
					/* Last segment? */
					if (walker->next != NULL) {
						/* Not last segment, i.e. first or middle segment
						 * OLD: (prev|NULL) <- walker <- next
						 * NEW: (prev|NULL) <----------- next */
						walker->next->reserved = prev;

						if (prev == NULL) {
							/* First but not last segment
							 * OLD: head -> walker -> next
							 * NEW: head -----------> next */
							uri->pathHead = walker->next;
						} else {
							/* Middle segment
							 * OLD: prev -> walker -> next
							 * NEW: prev -----------> next */
							prev->next = walker->next;
						}

						if (pathOwned && (walker->text.first != walker->text.afterLast)) {
							memory->free(memory, (URI_CHAR *)walker->text.first);
						}
						memory->free(memory, walker);
					} else {
						/* Last segment */
						if (pathOwned && (walker->text.first != walker->text.afterLast)) {
							memory->free(memory, (URI_CHAR *)walker->text.first);
						}

						if (prev == NULL) {
							/* Last and first */
							if (URI_FUNC(IsHostSet)(uri)) {
								/* Replace "." with empty segment to represent trailing slash */
								walker->text.first = URI_FUNC(SafeToPointTo);
								walker->text.afterLast = URI_FUNC(SafeToPointTo);
							} else {
								memory->free(memory, walker);

								uri->pathHead = NULL;
								uri->pathTail = NULL;
							}
						} else {
							/* Last but not first, replace "." with empty segment to represent trailing slash */
							walker->text.first = URI_FUNC(SafeToPointTo);
							walker->text.afterLast = URI_FUNC(SafeToPointTo);
						}
					}

					walker = nextBackup;
				}
			}
			break;

		case 2:
			if (((walker->text.first)[0] == _UT('.'))
					&& ((walker->text.first)[1] == _UT('.'))) {
				/* Path ".." -> remove this and the previous segment */
				URI_TYPE(PathSegment) * const prev = walker->reserved;
				URI_TYPE(PathSegment) * prevPrev;
				URI_TYPE(PathSegment) * const nextBackup = walker->next;

				removeSegment = URI_TRUE;
				if (relative) {
					if (prev == NULL) {
						/* We cannot remove traversal beyond because the
						 * URI is relative and may be resolved later.
						 * So we can simplify "a/../b/d" to "b/d" but
						 * we cannot simplify "../b/d" (outside of reference resolution). */
						removeSegment = URI_FALSE;
					} else if ((prev != NULL)
							&& ((prev->text.afterLast - prev->text.first) == 2)
							&& ((prev->text.first)[0] == _UT('.'))
							&& ((prev->text.first)[1] == _UT('.'))) {
						/* We need to protect against mis-simplifying "a/../../b" to "a/b". */
						removeSegment = URI_FALSE;
					}
				}

				if (removeSegment) {
					if (prev != NULL) {
						/* Not first segment */
						prevPrev = prev->reserved;
						if (prevPrev != NULL) {
							/* Not even prev is the first one
							 * OLD: prevPrev -> prev -> walker -> (next|NULL)
							 * NEW: prevPrev -------------------> (next|NULL) */
							prevPrev->next = walker->next;
							if (walker->next != NULL) {
								/* Update parent relationship as well
								 * OLD: prevPrev <- prev <- walker <- next
								 * NEW: prevPrev <------------------- next */
								walker->next->reserved = prevPrev;
							} else {
								/* Last segment -> insert "" segment to represent trailing slash, update tail */
								URI_TYPE(PathSegment) * const segment = memory->calloc(memory, 1, sizeof(URI_TYPE(PathSegment)));
								if (segment == NULL) {
									if (pathOwned && (walker->text.first != walker->text.afterLast)) {
										memory->free(memory, (URI_CHAR *)walker->text.first);
									}
									memory->free(memory, walker);

									if (pathOwned && (prev->text.first != prev->text.afterLast)) {
										memory->free(memory, (URI_CHAR *)prev->text.first);
									}
									memory->free(memory, prev);

									return URI_FALSE; /* Raises malloc error */
								}
								segment->text.first = URI_FUNC(SafeToPointTo);
								segment->text.afterLast = URI_FUNC(SafeToPointTo);
								prevPrev->next = segment;
								uri->pathTail = segment;
							}

							if (pathOwned && (walker->text.first != walker->text.afterLast)) {
								memory->free(memory, (URI_CHAR *)walker->text.first);
							}
							memory->free(memory, walker);

							if (pathOwned && (prev->text.first != prev->text.afterLast)) {
								memory->free(memory, (URI_CHAR *)prev->text.first);
							}
							memory->free(memory, prev);

							walker = nextBackup;
						} else {
							/* Prev is the first segment */
							if (walker->next != NULL) {
								uri->pathHead = walker->next;
								walker->next->reserved = NULL;

								if (pathOwned && (walker->text.first != walker->text.afterLast)) {
									memory->free(memory, (URI_CHAR *)walker->text.first);
								}
								memory->free(memory, walker);
							} else {
								/* Re-use segment for "" path segment to represent trailing slash, update tail */
								URI_TYPE(PathSegment) * const segment = walker;
								if (pathOwned && (segment->text.first != segment->text.afterLast)) {
									memory->free(memory, (URI_CHAR *)segment->text.first);
								}
								segment->text.first = URI_FUNC(SafeToPointTo);
								segment->text.afterLast = URI_FUNC(SafeToPointTo);
								uri->pathHead = segment;
								uri->pathTail = segment;
							}

							if (pathOwned && (prev->text.first != prev->text.afterLast)) {
								memory->free(memory, (URI_CHAR *)prev->text.first);
							}
							memory->free(memory, prev);

							walker = nextBackup;
						}
					} else {
						URI_TYPE(PathSegment) * const anotherNextBackup = walker->next;
						int freeWalker = URI_TRUE;

						/* First segment */
						if (walker->next != NULL) {
							/* First segment of multiple -> update head
							 * OLD: head -> walker -> next
							 * NEW: head -----------> next */
							uri->pathHead = walker->next;

							/* Update parent link as well
							 * OLD: head <- walker <- next
							 * NEW: head <----------- next */
							walker->next->reserved = NULL;
						} else {
							if (uri->absolutePath) {
								/* First and only segment -> update head
								 * OLD: head -> walker -> NULL
								 * NEW: head -----------> NULL */
								uri->pathHead = NULL;

								/* Last segment -> update tail
								 * OLD: tail -> walker
								 * NEW: tail -> NULL */
								uri->pathTail = NULL;
							} else {
								/* Re-use segment for "" path segment to represent trailing slash,
								 * then update head and tail */
								if (pathOwned && (walker->text.first != walker->text.afterLast)) {
									memory->free(memory, (URI_CHAR *)walker->text.first);
								}
								walker->text.first = URI_FUNC(SafeToPointTo);
								walker->text.afterLast = URI_FUNC(SafeToPointTo);
								freeWalker = URI_FALSE;
							}
						}

						if (freeWalker) {
							if (pathOwned && (walker->text.first != walker->text.afterLast)) {
								memory->free(memory, (URI_CHAR *)walker->text.first);
							}
							memory->free(memory, walker);
						}

						walker = anotherNextBackup;
					}
				}
			}
			break;
		} /* end of switch */

		if (!removeSegment) {
			/* .. then let's move to the next element, and start again. */
			if (walker->next != NULL) {
				walker->next->reserved = walker;
			} else {
				/* Last segment -> update tail */
				uri->pathTail = walker;
			}
			walker = walker->next;
		}
	} while (walker != NULL);

	return URI_TRUE;
}



/* Properly removes "." and ".." path segments */
UriBool URI_FUNC(RemoveDotSegmentsAbsolute)(URI_TYPE(Uri) * uri,
		UriMemoryManager * memory) {
	const UriBool ABSOLUTE = URI_FALSE;
	if (uri == NULL) {
		return URI_TRUE;
	}
	return URI_FUNC(RemoveDotSegmentsEx)(uri, ABSOLUTE, uri->owner, memory);
}



unsigned char URI_FUNC(HexdigToInt)(URI_CHAR hexdig) {
	switch (hexdig) {
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
		return (unsigned char)(9 + hexdig - _UT('9'));

	case _UT('a'):
	case _UT('b'):
	case _UT('c'):
	case _UT('d'):
	case _UT('e'):
	case _UT('f'):
		return (unsigned char)(15 + hexdig - _UT('f'));

	case _UT('A'):
	case _UT('B'):
	case _UT('C'):
	case _UT('D'):
	case _UT('E'):
	case _UT('F'):
		return (unsigned char)(15 + hexdig - _UT('F'));

	default:
		return 0;
	}
}



URI_CHAR URI_FUNC(HexToLetter)(unsigned int value) {
	/* Uppercase recommended in section 2.1. of RFC 3986 *
	 * http://tools.ietf.org/html/rfc3986#section-2.1    */
	return URI_FUNC(HexToLetterEx)(value, URI_TRUE);
}



URI_CHAR URI_FUNC(HexToLetterEx)(unsigned int value, UriBool uppercase) {
	switch (value) {
	case  0: return _UT('0');
	case  1: return _UT('1');
	case  2: return _UT('2');
	case  3: return _UT('3');
	case  4: return _UT('4');
	case  5: return _UT('5');
	case  6: return _UT('6');
	case  7: return _UT('7');
	case  8: return _UT('8');
	case  9: return _UT('9');

	case 10: return (uppercase == URI_TRUE) ? _UT('A') : _UT('a');
	case 11: return (uppercase == URI_TRUE) ? _UT('B') : _UT('b');
	case 12: return (uppercase == URI_TRUE) ? _UT('C') : _UT('c');
	case 13: return (uppercase == URI_TRUE) ? _UT('D') : _UT('d');
	case 14: return (uppercase == URI_TRUE) ? _UT('E') : _UT('e');
	default: return (uppercase == URI_TRUE) ? _UT('F') : _UT('f');
	}
}



/* Checks if a URI has the host component set. */
UriBool URI_FUNC(IsHostSet)(const URI_TYPE(Uri) * uri) {
	return (uri != NULL)
			&& ((uri->hostText.first != NULL)
				|| (uri->hostData.ip4 != NULL)
				|| (uri->hostData.ip6 != NULL)
				|| (uri->hostData.ipFuture.first != NULL)
			);
}



/* Copies the path segment list from one URI to another. */
UriBool URI_FUNC(CopyPath)(URI_TYPE(Uri) * dest,
		const URI_TYPE(Uri) * source, UriMemoryManager * memory) {
	if (source->pathHead == NULL) {
		/* No path component */
		dest->pathHead = NULL;
		dest->pathTail = NULL;
	} else {
		/* Copy list but not the text contained */
		URI_TYPE(PathSegment) * sourceWalker = source->pathHead;
		URI_TYPE(PathSegment) * destPrev = NULL;
		do {
			URI_TYPE(PathSegment) * cur = memory->malloc(memory, sizeof(URI_TYPE(PathSegment)));
			if (cur == NULL) {
				/* Fix broken list */
				if (destPrev != NULL) {
					destPrev->next = NULL;
				}
				return URI_FALSE; /* Raises malloc error */
			}

			/* From this functions usage we know that *
			 * the dest URI cannot be uri->owner      */
			cur->text = sourceWalker->text;
			if (destPrev == NULL) {
				/* First segment ever */
				dest->pathHead = cur;
			} else {
				destPrev->next = cur;
			}
			destPrev = cur;
			sourceWalker = sourceWalker->next;
		} while (sourceWalker != NULL);
		dest->pathTail = destPrev;
		dest->pathTail->next = NULL;
	}

	dest->absolutePath = source->absolutePath;
	return URI_TRUE;
}



/* Copies the authority part of an URI over to another. */
UriBool URI_FUNC(CopyAuthority)(URI_TYPE(Uri) * dest,
		const URI_TYPE(Uri) * source, UriMemoryManager * memory) {
	/* From this functions usage we know that *
	 * the dest URI cannot be uri->owner      */
	
	/* Copy userInfo */
	dest->userInfo = source->userInfo;

	/* Copy hostText */
	dest->hostText = source->hostText;

	/* Copy hostData */
	if (source->hostData.ip4 != NULL) {
		dest->hostData.ip4 = memory->malloc(memory, sizeof(UriIp4));
		if (dest->hostData.ip4 == NULL) {
			return URI_FALSE; /* Raises malloc error */
		}
		*(dest->hostData.ip4) = *(source->hostData.ip4);
		dest->hostData.ip6 = NULL;
		dest->hostData.ipFuture.first = NULL;
		dest->hostData.ipFuture.afterLast = NULL;
	} else if (source->hostData.ip6 != NULL) {
		dest->hostData.ip4 = NULL;
		dest->hostData.ip6 = memory->malloc(memory, sizeof(UriIp6));
		if (dest->hostData.ip6 == NULL) {
			return URI_FALSE; /* Raises malloc error */
		}
		*(dest->hostData.ip6) = *(source->hostData.ip6);
		dest->hostData.ipFuture.first = NULL;
		dest->hostData.ipFuture.afterLast = NULL;
	} else {
		dest->hostData.ip4 = NULL;
		dest->hostData.ip6 = NULL;
		dest->hostData.ipFuture = source->hostData.ipFuture;
	}

	/* Copy portText */
	dest->portText = source->portText;

	return URI_TRUE;
}



UriBool URI_FUNC(FixAmbiguity)(URI_TYPE(Uri) * uri,
		UriMemoryManager * memory) {
	URI_TYPE(PathSegment) * segment;

	if (	/* Case 1: absolute path, empty first segment */
			(uri->absolutePath
			&& (uri->pathHead != NULL)
			&& (uri->pathHead->text.afterLast == uri->pathHead->text.first))

			/* Case 2: relative path, empty first and second segment */
			|| (!uri->absolutePath
			&& (uri->pathHead != NULL)
			&& (uri->pathHead->next != NULL)
			&& (uri->pathHead->text.afterLast == uri->pathHead->text.first)
			&& (uri->pathHead->next->text.afterLast == uri->pathHead->next->text.first))) {
		/* NOOP */
	} else {
		return URI_TRUE;
	}

	segment = memory->malloc(memory, 1 * sizeof(URI_TYPE(PathSegment)));
	if (segment == NULL) {
		return URI_FALSE; /* Raises malloc error */
	}

	/* Insert "." segment in front */
	segment->next = uri->pathHead;
	segment->text.first = URI_FUNC(ConstPwd);
	segment->text.afterLast = URI_FUNC(ConstPwd) + 1;
	uri->pathHead = segment;
	return URI_TRUE;
}



void URI_FUNC(FixEmptyTrailSegment)(URI_TYPE(Uri) * uri,
		UriMemoryManager * memory) {
	/* Fix path if only one empty segment */
	if (!uri->absolutePath
			&& !URI_FUNC(IsHostSet)(uri)
			&& (uri->pathHead != NULL)
			&& (uri->pathHead->next == NULL)
			&& (uri->pathHead->text.first == uri->pathHead->text.afterLast)) {
		memory->free(memory, uri->pathHead);
		uri->pathHead = NULL;
		uri->pathTail = NULL;
	}
}



#endif
