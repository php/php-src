#ifndef FIDDLE_H
#define FIDDLE_H

#include <ruby.h>
#include <errno.h>

#if defined(_WIN32)
#include <windows.h>
#endif

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#ifdef USE_HEADER_HACKS
#include <ffi/ffi.h>
#else
#include <ffi.h>
#endif

#undef ffi_type_uchar
#undef ffi_type_schar
#undef ffi_type_ushort
#undef ffi_type_sshort
#undef ffi_type_uint
#undef ffi_type_sint
#undef ffi_type_ulong
#undef ffi_type_slong

#if CHAR_BIT == 8
# define ffi_type_uchar ffi_type_uint8
# define ffi_type_schar ffi_type_sint8
#else
# error "CHAR_BIT not supported"
#endif

# if SIZEOF_SHORT == 2
#  define ffi_type_ushort ffi_type_uint16
#  define ffi_type_sshort ffi_type_sint16
# elif SIZEOF_SHORT == 4
#  define ffi_type_ushort ffi_type_uint32
#  define ffi_type_sshort ffi_type_sint32
# else
#  error "short size not supported"
# endif

# if SIZEOF_INT == 2
#  define ffi_type_uint	ffi_type_uint16
#  define ffi_type_sint	ffi_type_sint16
# elif SIZEOF_INT == 4
#  define ffi_type_uint	ffi_type_uint32
#  define ffi_type_sint	ffi_type_sint32
# elif SIZEOF_INT == 8
#  define ffi_type_uint	ffi_type_uint64
#  define ffi_type_sint	ffi_type_sint64
# else
#  error "int size not supported"
# endif

# if SIZEOF_LONG == 4
#  define ffi_type_ulong ffi_type_uint32
#  define ffi_type_slong ffi_type_sint32
# elif SIZEOF_LONG == 8
#  define ffi_type_ulong ffi_type_uint64
#  define ffi_type_slong ffi_type_sint64
# else
#  error "long size not supported"
# endif

#if HAVE_LONG_LONG
# if SIZEOF_LONG_LONG == 8
#   define ffi_type_slong_long ffi_type_sint64
#   define ffi_type_ulong_long ffi_type_uint64
# else
#  error "long long size not supported"
# endif
#endif

#include <closure.h>
#include <conversions.h>
#include <function.h>

/* FIXME
 * These constants need to match up with DL. We need to refactor this to use
 * the DL header files or vice versa.
 */

#define TYPE_VOID  0
#define TYPE_VOIDP 1
#define TYPE_CHAR  2
#define TYPE_SHORT 3
#define TYPE_INT   4
#define TYPE_LONG  5
#if HAVE_LONG_LONG
#define TYPE_LONG_LONG 6
#endif
#define TYPE_FLOAT 7
#define TYPE_DOUBLE 8

extern VALUE mFiddle;

#endif
/* vim: set noet sws=4 sw=4: */
