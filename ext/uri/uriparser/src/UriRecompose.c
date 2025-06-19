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
#  include "UriRecompose.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriRecompose.c"
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



static int URI_FUNC(ToStringEngine)(URI_CHAR * dest, const URI_TYPE(Uri) * uri,
		int maxChars, int * charsWritten, int * charsRequired);



int URI_FUNC(ToStringCharsRequired)(const URI_TYPE(Uri) * uri,
		int * charsRequired) {
	const int MAX_CHARS = ((unsigned int)-1) >> 1;
	return URI_FUNC(ToStringEngine)(NULL, uri, MAX_CHARS, NULL, charsRequired);
}



int URI_FUNC(ToString)(URI_CHAR * dest, const URI_TYPE(Uri) * uri,
		int maxChars, int * charsWritten) {
	return URI_FUNC(ToStringEngine)(dest, uri, maxChars, charsWritten, NULL);
}



static URI_INLINE int URI_FUNC(ToStringEngine)(URI_CHAR * dest,
		const URI_TYPE(Uri) * uri, int maxChars, int * charsWritten,
		int * charsRequired) {
	int written = 0;
	if ((uri == NULL) || ((dest == NULL) && (charsRequired == NULL))) {
		if (charsWritten != NULL) {
			*charsWritten = 0;
		}
		return URI_ERROR_NULL;
	}

	if (maxChars < 1) {
		if (charsWritten != NULL) {
			*charsWritten = 0;
		}
		return URI_ERROR_TOSTRING_TOO_LONG;
	}
	maxChars--; /* So we don't have to subtract 1 for '\0' all the time */

	/* [01/19]	result = "" */
				if (dest != NULL) {
					dest[0] = _UT('\0');
				} else {
					(*charsRequired) = 0;
				}
	/* [02/19]	if defined(scheme) then */
				if (uri->scheme.first != NULL) {
	/* [03/19]		append scheme to result; */
					const int charsToWrite
							= (int)(uri->scheme.afterLast - uri->scheme.first);
					if (dest != NULL) {
						if (written + charsToWrite <= maxChars) {
							memcpy(dest + written, uri->scheme.first,
									charsToWrite * sizeof(URI_CHAR));
							written += charsToWrite;
						} else {
							dest[0] = _UT('\0');
							if (charsWritten != NULL) {
								*charsWritten = 0;
							}
							return URI_ERROR_TOSTRING_TOO_LONG;
						}
					} else {
						(*charsRequired) += charsToWrite;
					}
	/* [04/19]		append ":" to result; */
					if (dest != NULL) {
						if (written + 1 <= maxChars) {
							memcpy(dest + written, _UT(":"),
									1 * sizeof(URI_CHAR));
							written += 1;
						} else {
							dest[0] = _UT('\0');
							if (charsWritten != NULL) {
								*charsWritten = 0;
							}
							return URI_ERROR_TOSTRING_TOO_LONG;
						}
					} else {
						(*charsRequired) += 1;
					}
	/* [05/19]	endif; */
				}
	/* [06/19]	if defined(authority) then */
				if (URI_FUNC(IsHostSet)(uri)) {
	/* [07/19]		append "//" to result; */
					if (dest != NULL) {
						if (written + 2 <= maxChars) {
							memcpy(dest + written, _UT("//"),
									2 * sizeof(URI_CHAR));
							written += 2;
						} else {
							dest[0] = _UT('\0');
							if (charsWritten != NULL) {
								*charsWritten = 0;
							}
							return URI_ERROR_TOSTRING_TOO_LONG;
						}
					} else {
						(*charsRequired) += 2;
					}
	/* [08/19]		append authority to result; */
					/* UserInfo */
					if (uri->userInfo.first != NULL) {
						const int charsToWrite = (int)(uri->userInfo.afterLast - uri->userInfo.first);
						if (dest != NULL) {
							if (written + charsToWrite <= maxChars) {
								memcpy(dest + written, uri->userInfo.first,
										charsToWrite * sizeof(URI_CHAR));
								written += charsToWrite;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}

							if (written + 1 <= maxChars) {
								memcpy(dest + written, _UT("@"),
										1 * sizeof(URI_CHAR));
								written += 1;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}
						} else {
							(*charsRequired) += charsToWrite + 1;
						}
					}

					/* Host */
					if (uri->hostData.ip4 != NULL) {
						/* IPv4 */
						int i = 0;
						for (; i < 4; i++) {
							const unsigned char value = uri->hostData.ip4->data[i];
							const int charsToWrite = (value > 99) ? 3 : ((value > 9) ? 2 : 1);
							if (dest != NULL) {
								if (written + charsToWrite <= maxChars) {
									URI_CHAR text[4];
									if (value > 99) {
										text[0] = _UT('0') + (value / 100);
										text[1] = _UT('0') + ((value % 100) / 10);
										text[2] = _UT('0') + (value % 10);
									} else if (value > 9)  {
										text[0] = _UT('0') + (value / 10);
										text[1] = _UT('0') + (value % 10);
									} else {
										text[0] = _UT('0') + value;
									}
									text[charsToWrite] = _UT('\0');
									memcpy(dest + written, text, charsToWrite * sizeof(URI_CHAR));
									written += charsToWrite;
								} else {
									dest[0] = _UT('\0');
									if (charsWritten != NULL) {
										*charsWritten = 0;
									}
									return URI_ERROR_TOSTRING_TOO_LONG;
								}
								if (i < 3) {
									if (written + 1 <= maxChars) {
										memcpy(dest + written, _UT("."),
												1 * sizeof(URI_CHAR));
										written += 1;
									} else {
										dest[0] = _UT('\0');
										if (charsWritten != NULL) {
											*charsWritten = 0;
										}
										return URI_ERROR_TOSTRING_TOO_LONG;
									}
								}
							} else {
								(*charsRequired) += charsToWrite + ((i == 3) ? 0 : 1);
							}
						}
					} else if (uri->hostData.ip6 != NULL) {
						/* IPv6 */
						int i = 0;
						if (dest != NULL) {
							if (written + 1 <= maxChars) {
								memcpy(dest + written, _UT("["),
										1 * sizeof(URI_CHAR));
								written += 1;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}
						} else {
							(*charsRequired) += 1;
						}

						for (; i < 16; i++) {
							const unsigned char value = uri->hostData.ip6->data[i];
							if (dest != NULL) {
								if (written + 2 <= maxChars) {
									URI_CHAR text[3];
									text[0] = URI_FUNC(HexToLetterEx)(value / 16, URI_FALSE);
									text[1] = URI_FUNC(HexToLetterEx)(value % 16, URI_FALSE);
									text[2] = _UT('\0');
									memcpy(dest + written, text, 2 * sizeof(URI_CHAR));
									written += 2;
								} else {
									dest[0] = _UT('\0');
									if (charsWritten != NULL) {
										*charsWritten = 0;
									}
									return URI_ERROR_TOSTRING_TOO_LONG;
								}
							} else {
								(*charsRequired) += 2;
							}
							if (((i & 1) == 1) && (i < 15)) {
								if (dest != NULL) {
									if (written + 1 <= maxChars) {
										memcpy(dest + written, _UT(":"),
												1 * sizeof(URI_CHAR));
										written += 1;
									} else {
										dest[0] = _UT('\0');
										if (charsWritten != NULL) {
											*charsWritten = 0;
										}
										return URI_ERROR_TOSTRING_TOO_LONG;
									}
								} else {
									(*charsRequired) += 1;
								}
							}
						}

						if (dest != NULL) {
							if (written + 1 <= maxChars) {
								memcpy(dest + written, _UT("]"),
										1 * sizeof(URI_CHAR));
								written += 1;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}
						} else {
							(*charsRequired) += 1;
						}
					} else if (uri->hostData.ipFuture.first != NULL) {
						/* IPvFuture */
						const int charsToWrite = (int)(uri->hostData.ipFuture.afterLast
								- uri->hostData.ipFuture.first);
						if (dest != NULL) {
							if (written + 1 <= maxChars) {
								memcpy(dest + written, _UT("["),
										1 * sizeof(URI_CHAR));
								written += 1;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}

							if (written + charsToWrite <= maxChars) {
								memcpy(dest + written, uri->hostData.ipFuture.first, charsToWrite * sizeof(URI_CHAR));
								written += charsToWrite;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}

							if (written + 1 <= maxChars) {
								memcpy(dest + written, _UT("]"),
										1 * sizeof(URI_CHAR));
								written += 1;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}
						} else {
							(*charsRequired) += 1 + charsToWrite + 1;
						}
					} else if (uri->hostText.first != NULL) {
						/* Regname */
						const int charsToWrite = (int)(uri->hostText.afterLast - uri->hostText.first);
						if (dest != NULL) {
							if (written + charsToWrite <= maxChars) {
								memcpy(dest + written, uri->hostText.first,
										charsToWrite * sizeof(URI_CHAR));
								written += charsToWrite;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}
						} else {
							(*charsRequired) += charsToWrite;
						}
					}

					/* Port */
					if (uri->portText.first != NULL) {
						const int charsToWrite = (int)(uri->portText.afterLast - uri->portText.first);
						if (dest != NULL) {
							/* Leading ':' */
							if (written + 1 <= maxChars) {
									memcpy(dest + written, _UT(":"),
											1 * sizeof(URI_CHAR));
									written += 1;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}

							/* Port number */
							if (written + charsToWrite <= maxChars) {
								memcpy(dest + written, uri->portText.first,
										charsToWrite * sizeof(URI_CHAR));
								written += charsToWrite;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}
						} else {
							(*charsRequired) += 1 + charsToWrite;
						}
					}
	/* [09/19]	endif; */
				}
	/* [10/19]	append path to result; */
				/* Slash needed here? */
				if (uri->absolutePath || ((uri->pathHead != NULL)
						&& URI_FUNC(IsHostSet)(uri))) {
					if (dest != NULL) {
						if (written + 1 <= maxChars) {
							memcpy(dest + written, _UT("/"),
									1 * sizeof(URI_CHAR));
							written += 1;
						} else {
							dest[0] = _UT('\0');
							if (charsWritten != NULL) {
								*charsWritten = 0;
							}
							return URI_ERROR_TOSTRING_TOO_LONG;
						}
					} else {
						(*charsRequired) += 1;
					}
				}

				if (uri->pathHead != NULL) {
					URI_TYPE(PathSegment) * walker = uri->pathHead;
					do {
						const int charsToWrite = (int)(walker->text.afterLast - walker->text.first);
						if (dest != NULL) {
							if (written + charsToWrite <= maxChars) {
								memcpy(dest + written, walker->text.first,
										charsToWrite * sizeof(URI_CHAR));
								written += charsToWrite;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}
						} else {
							(*charsRequired) += charsToWrite;
						}

						/* Not last segment -> append slash */
						if (walker->next != NULL) {
							if (dest != NULL) {
								if (written + 1 <= maxChars) {
									memcpy(dest + written, _UT("/"),
											1 * sizeof(URI_CHAR));
									written += 1;
								} else {
									dest[0] = _UT('\0');
									if (charsWritten != NULL) {
										*charsWritten = 0;
									}
									return URI_ERROR_TOSTRING_TOO_LONG;
								}
							} else {
								(*charsRequired) += 1;
							}
						}

						walker = walker->next;
					} while (walker != NULL);
				}
	/* [11/19]	if defined(query) then */
				if (uri->query.first != NULL) {
	/* [12/19]		append "?" to result; */
					if (dest != NULL) {
						if (written + 1 <= maxChars) {
							memcpy(dest + written, _UT("?"),
									1 * sizeof(URI_CHAR));
							written += 1;
						} else {
							dest[0] = _UT('\0');
							if (charsWritten != NULL) {
								*charsWritten = 0;
							}
							return URI_ERROR_TOSTRING_TOO_LONG;
						}
					} else {
						(*charsRequired) += 1;
					}
	/* [13/19]		append query to result; */
					{
						const int charsToWrite
								= (int)(uri->query.afterLast - uri->query.first);
						if (dest != NULL) {
							if (written + charsToWrite <= maxChars) {
								memcpy(dest + written, uri->query.first,
										charsToWrite * sizeof(URI_CHAR));
								written += charsToWrite;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}
						} else {
							(*charsRequired) += charsToWrite;
						}
					}
	/* [14/19]	endif; */
				}
	/* [15/19]	if defined(fragment) then */
				if (uri->fragment.first != NULL) {
	/* [16/19]		append "#" to result; */
					if (dest != NULL) {
						if (written + 1 <= maxChars) {
							memcpy(dest + written, _UT("#"),
									1 * sizeof(URI_CHAR));
							written += 1;
						} else {
							dest[0] = _UT('\0');
							if (charsWritten != NULL) {
								*charsWritten = 0;
							}
							return URI_ERROR_TOSTRING_TOO_LONG;
						}
					} else {
						(*charsRequired) += 1;
					}
	/* [17/19]		append fragment to result; */
					{
						const int charsToWrite
								= (int)(uri->fragment.afterLast - uri->fragment.first);
						if (dest != NULL) {
							if (written + charsToWrite <= maxChars) {
								memcpy(dest + written, uri->fragment.first,
										charsToWrite * sizeof(URI_CHAR));
								written += charsToWrite;
							} else {
								dest[0] = _UT('\0');
								if (charsWritten != NULL) {
									*charsWritten = 0;
								}
								return URI_ERROR_TOSTRING_TOO_LONG;
							}
						} else {
							(*charsRequired) += charsToWrite;
						}
					}
	/* [18/19]	endif; */
				}
	/* [19/19]	return result; */
				if (dest != NULL) {
					dest[written++] = _UT('\0');
					if (charsWritten != NULL) {
						*charsWritten = written;
					}
				}
				return URI_SUCCESS;
}



#endif
