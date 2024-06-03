/*
 * uriparser - RFC 3986 URI parsing library
 *
 * Copyright (C) 2018, Weijia Song <songweijia@gmail.com>
 * Copyright (C) 2018, Sebastian Pipping <sebastian@pipping.org>
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

#ifndef URI_MEMORY_H
#define URI_MEMORY_H 1



#ifndef URI_DOXYGEN
# include <uriparser/Uri.h>
#endif



#define URI_CHECK_MEMORY_MANAGER(memory)  \
	do { \
		if (memory == NULL) { \
			memory = &defaultMemoryManager; \
		} else if (uriMemoryManagerIsComplete(memory) != URI_TRUE) { \
			return URI_ERROR_MEMORY_MANAGER_INCOMPLETE; \
		} \
	} while (0)



#ifdef __cplusplus
# define URIPARSER_EXTERN extern "C"
#else
# define URIPARSER_EXTERN extern
#endif

URIPARSER_EXTERN UriMemoryManager defaultMemoryManager;

#undef URIPARSER_EXTERN



UriBool uriMemoryManagerIsComplete(const UriMemoryManager * memory);



#endif /* URI_MEMORY_H */
