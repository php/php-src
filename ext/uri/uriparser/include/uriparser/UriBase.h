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
 * @file UriBase.h
 * Holds definitions independent of the encoding pass.
 */

#ifndef URI_BASE_H
#define URI_BASE_H 1



/* Version helper macro */
#define URI_ANSI_TO_UNICODE_HELPER(x)  L ## x
#define URI_ANSI_TO_UNICODE(x)         URI_ANSI_TO_UNICODE_HELPER(x)



/* Version */
#define URI_VER_MAJOR           0
#define URI_VER_MINOR           9
#define URI_VER_RELEASE         9
#define URI_VER_SUFFIX_ANSI     ""
#define URI_VER_SUFFIX_UNICODE  URI_ANSI_TO_UNICODE(URI_VER_SUFFIX_ANSI)



/* More version helper macros */
#define URI_INT_TO_ANSI_HELPER(x) #x
#define URI_INT_TO_ANSI(x) URI_INT_TO_ANSI_HELPER(x)

#define URI_INT_TO_UNICODE_HELPER(x) URI_ANSI_TO_UNICODE(#x)
#define URI_INT_TO_UNICODE(x) URI_INT_TO_UNICODE_HELPER(x)

#define URI_VER_ANSI_HELPER(ma, mi, r, s) \
	URI_INT_TO_ANSI(ma) "." \
	URI_INT_TO_ANSI(mi) "." \
	URI_INT_TO_ANSI(r) \
	s

#define URI_VER_UNICODE_HELPER(ma, mi, r, s) \
	URI_INT_TO_UNICODE(ma) L"." \
	URI_INT_TO_UNICODE(mi) L"." \
	URI_INT_TO_UNICODE(r) \
	s



/* Full version strings */
#define URI_VER_ANSI     URI_VER_ANSI_HELPER(URI_VER_MAJOR, URI_VER_MINOR, URI_VER_RELEASE, URI_VER_SUFFIX_ANSI)
#define URI_VER_UNICODE  URI_VER_UNICODE_HELPER(URI_VER_MAJOR, URI_VER_MINOR, URI_VER_RELEASE, URI_VER_SUFFIX_UNICODE)



/* Unused parameter macro */
#ifdef __GNUC__
# define URI_UNUSED(x) unused_##x __attribute__((unused))
#else
# define URI_UNUSED(x) x
#endif



/* Import/export decorator */
#if defined(_MSC_VER)
# if defined(URI_STATIC_BUILD)
#  define URI_PUBLIC
# elif defined(URI_LIBRARY_BUILD)
#  define URI_PUBLIC __declspec(dllexport)
# else
#  define URI_PUBLIC __declspec(dllimport)
# endif
#else
# if ! defined(URI_LIBRARY_BUILD) || ! defined(URI_VISIBILITY)
#  define URI_PUBLIC
# else
#  define URI_PUBLIC __attribute__ ((visibility("default")))
# endif
#endif



typedef int UriBool; /**< Boolean type */

#define URI_TRUE     1
#define URI_FALSE    0



/* Shared errors */
#define URI_SUCCESS                        0
#define URI_ERROR_SYNTAX                   1 /* Parsed text violates expected format */
#define URI_ERROR_NULL                     2 /* One of the params passed was NULL
                                                although it mustn't be */
#define URI_ERROR_MALLOC                   3 /* Requested memory could not be allocated */
#define URI_ERROR_OUTPUT_TOO_LARGE         4 /* Some output is to large for the receiving buffer */
#define URI_ERROR_NOT_IMPLEMENTED          8 /* The called function is not implemented yet */
#define URI_ERROR_RANGE_INVALID            9 /* The parameters passed contained invalid ranges */
#define URI_ERROR_MEMORY_MANAGER_INCOMPLETE  10 /* [>=0.9.0] The UriMemoryManager passed does not implement all needed functions */


/* Errors specific to ToString */
#define URI_ERROR_TOSTRING_TOO_LONG        URI_ERROR_OUTPUT_TOO_LARGE /* Deprecated, test for URI_ERROR_OUTPUT_TOO_LARGE instead */

/* Errors specific to AddBaseUri */
#define URI_ERROR_ADDBASE_REL_BASE         5 /* Given base is not absolute */

/* Errors specific to RemoveBaseUri */
#define URI_ERROR_REMOVEBASE_REL_BASE      6 /* Given base is not absolute */
#define URI_ERROR_REMOVEBASE_REL_SOURCE    7 /* Given base is not absolute */

/* Error specific to uriTestMemoryManager */
#define URI_ERROR_MEMORY_MANAGER_FAULTY   11 /* [>=0.9.0] The UriMemoryManager given did not pass the test suite */

/* Error specific to uriSetUserInfo */
#define URI_ERROR_SETUSERINFO_HOST_NOT_SET 12 /* [>=0.9.9] The %URI given does not have the host set */

/* Error specific to uriSetPort */
#define URI_ERROR_SETPORT_HOST_NOT_SET     13 /* [>=0.9.9] The %URI given does not have the host set */

/* Error specific to uriSetHost* */
#define URI_ERROR_SETHOST_USERINFO_SET     14 /* [>=0.9.9] The %URI given does have user info set */
#define URI_ERROR_SETHOST_PORT_SET         15 /* [>=0.9.9] The %URI given does have a port set */



#ifndef URI_DOXYGEN
# include <stdio.h> /* For NULL, snprintf */
# include <ctype.h> /* For wchar_t */
# include <string.h> /* For strlen, memset, memcpy */
# include <stdlib.h> /* For malloc */
#endif /* URI_DOXYGEN */



/**
 * Holds an IPv4 address.
 */
typedef struct UriIp4Struct {
	unsigned char data[4]; /**< Each octet in one byte */
} UriIp4; /**< @copydoc UriIp4Struct */



/**
 * Holds an IPv6 address.
 */
typedef struct UriIp6Struct {
	unsigned char data[16]; /**< Each quad in two bytes */
} UriIp6; /**< @copydoc UriIp6Struct */


struct UriMemoryManagerStruct;  /* foward declaration to break loop */


/**
 * Function signature that custom malloc(3) functions must conform to
 *
 * @since 0.9.0
 */
typedef void * (*UriFuncMalloc)(struct UriMemoryManagerStruct *, size_t);

/**
 * Function signature that custom calloc(3) functions must conform to
 *
 * @since 0.9.0
 */
typedef void * (*UriFuncCalloc)(struct UriMemoryManagerStruct *, size_t, size_t);

/**
 * Function signature that custom realloc(3) functions must conform to
 *
 * @since 0.9.0
 */
typedef void * (*UriFuncRealloc)(struct UriMemoryManagerStruct *, void *, size_t);

/**
 * Function signature that custom reallocarray(3) functions must conform to
 *
 * @since 0.9.0
 */
typedef void * (*UriFuncReallocarray)(struct UriMemoryManagerStruct *, void *, size_t, size_t);

/**
 * Function signature that custom free(3) functions must conform to
 *
 * @since 0.9.0
 */
typedef void (*UriFuncFree)(struct UriMemoryManagerStruct *, void *);


/**
 * Class-like interface of custom memory managers
 *
 * @see uriCompleteMemoryManager
 * @see uriEmulateCalloc
 * @see uriEmulateReallocarray
 * @see uriTestMemoryManager
 * @since 0.9.0
 */
typedef struct UriMemoryManagerStruct {
	UriFuncMalloc malloc; /**< Pointer to custom malloc(3) */
	UriFuncCalloc calloc; /**< Pointer to custom calloc(3); to emulate using malloc and memset see uriEmulateCalloc */
	UriFuncRealloc realloc; /**< Pointer to custom realloc(3) */
	UriFuncReallocarray reallocarray; /**< Pointer to custom reallocarray(3); to emulate using realloc see uriEmulateReallocarray */
	UriFuncFree free; /**< Pointer to custom free(3) */
	void * userData; /**< Pointer to data that the other function members need access to */
} UriMemoryManager; /**< @copydoc UriMemoryManagerStruct */


/**
 * Specifies a line break conversion mode.
 */
typedef enum UriBreakConversionEnum {
	URI_BR_TO_LF, /**< Convert to Unix line breaks ("\\x0a") */
	URI_BR_TO_CRLF, /**< Convert to Windows line breaks ("\\x0d\\x0a") */
	URI_BR_TO_CR, /**< Convert to Macintosh line breaks ("\\x0d") */
	URI_BR_TO_UNIX = URI_BR_TO_LF, /**< @copydoc UriBreakConversionEnum::URI_BR_TO_LF */
	URI_BR_TO_WINDOWS = URI_BR_TO_CRLF, /**< @copydoc UriBreakConversionEnum::URI_BR_TO_CRLF */
	URI_BR_TO_MAC = URI_BR_TO_CR, /**< @copydoc UriBreakConversionEnum::URI_BR_TO_CR */
	URI_BR_DONT_TOUCH /**< Copy line breaks unmodified */
} UriBreakConversion; /**< @copydoc UriBreakConversionEnum */



/**
 * Specifies which component of a %URI has to be normalized.
 */
typedef enum UriNormalizationMaskEnum {
	URI_NORMALIZED = 0, /**< Do not normalize anything */
	URI_NORMALIZE_SCHEME = 1 << 0, /**< Normalize scheme (fix uppercase letters) */
	URI_NORMALIZE_USER_INFO = 1 << 1, /**< Normalize user info (fix uppercase percent-encodings) */
	URI_NORMALIZE_HOST = 1 << 2, /**< Normalize host (fix uppercase letters) */
	URI_NORMALIZE_PATH = 1 << 3, /**< Normalize path (fix uppercase percent-encodings and redundant dot segments) */
	URI_NORMALIZE_QUERY = 1 << 4, /**< Normalize query (fix uppercase percent-encodings) */
	URI_NORMALIZE_FRAGMENT = 1 << 5, /**< Normalize fragment (fix uppercase percent-encodings) */
	URI_NORMALIZE_PORT = 1 << 6 /**< Normalize port (drop leading zeros) @since 0.9.9 */
} UriNormalizationMask; /**< @copydoc UriNormalizationMaskEnum */



/**
 * Specifies how to resolve %URI references.
 */
typedef enum UriResolutionOptionsEnum {
	URI_RESOLVE_STRICTLY = 0, /**< Full RFC conformance */
	URI_RESOLVE_IDENTICAL_SCHEME_COMPAT = 1 << 0 /**< Treat %URI to resolve with identical scheme as having no scheme */
} UriResolutionOptions; /**< @copydoc UriResolutionOptionsEnum */



/**
 * Wraps a memory manager backend that only provides malloc and free
 * to make a complete memory manager ready to be used.
 *
 * The core feature of this wrapper is that you don't need to implement
 * realloc if you don't want to.  The wrapped memory manager uses
 * backend->malloc, memcpy, and backend->free and soieof(size_t) extra
 * bytes per allocation to emulate fallback realloc for you.
 *
 * memory->calloc is uriEmulateCalloc.
 * memory->free uses backend->free and handles the size header.
 * memory->malloc uses backend->malloc and adds a size header.
 * memory->realloc uses memory->malloc, memcpy, and memory->free and reads
 *                 the size header.
 * memory->reallocarray is uriEmulateReallocarray.
 *
 * The internal workings behind memory->free, memory->malloc, and
 * memory->realloc may change so the functions exposed by these function
 * pointer sshould be consided internal and not public API.
 *
 * @param memory   <b>OUT</b>: Where to write the wrapped memory manager to
 * @param backend  <b>IN</b>: Memory manager to use as a backend
 * @return          Error code or 0 on success
 *
 * @see uriEmulateCalloc
 * @see uriEmulateReallocarray
 * @see UriMemoryManager
 * @since 0.9.0
 */
URI_PUBLIC int uriCompleteMemoryManager(UriMemoryManager * memory,
		UriMemoryManager * backend);



/**
 * Offers emulation of calloc(3) based on memory->malloc and memset.
 * See "man 3 calloc" as well.
 *
 * @param memory  <b>IN</b>: Memory manager to use, should not be NULL
 * @param nmemb   <b>IN</b>: Number of elements to allocate
 * @param size    <b>IN</b>: Size in bytes per element
 * @return        Pointer to allocated memory or NULL
 *
 * @see uriCompleteMemoryManager
 * @see uriEmulateReallocarray
 * @see UriMemoryManager
 * @since 0.9.0
 */
URI_PUBLIC void * uriEmulateCalloc(UriMemoryManager * memory,
		size_t nmemb, size_t size);



/**
 * Offers emulation of reallocarray(3) based on memory->realloc.
 * See "man 3 reallocarray" as well.
 *
 * @param memory  <b>IN</b>: Memory manager to use, should not be NULL
 * @param ptr     <b>IN</b>: Pointer allocated using memory->malloc/... or NULL
 * @param nmemb   <b>IN</b>: Number of elements to allocate
 * @param size    <b>IN</b>: Size in bytes per element
 * @return        Pointer to allocated memory or NULL
 *
 * @see uriCompleteMemoryManager
 * @see uriEmulateCalloc
 * @see UriMemoryManager
 * @since 0.9.0
 */
URI_PUBLIC void * uriEmulateReallocarray(UriMemoryManager * memory,
		void * ptr, size_t nmemb, size_t size);



/**
 * Run multiple tests against a given memory manager.
 * For example, one test
 * 1. allocates a small amount of memory,
 * 2. writes some magic bytes to it,
 * 3. reallocates it,
 * 4. checks that previous values are still present,
 * 5. and frees that memory.
 *
 * It is recommended to compile with AddressSanitizer enabled
 * to take full advantage of uriTestMemoryManager.
 *
 * @param memory  <b>IN</b>: Memory manager to use, should not be NULL
 * @return        Error code or 0 on success
 *
 * @see uriEmulateCalloc
 * @see uriEmulateReallocarray
 * @see UriMemoryManager
 * @since 0.9.0
 */
URI_PUBLIC int uriTestMemoryManager(UriMemoryManager * memory);



#endif /* URI_BASE_H */
