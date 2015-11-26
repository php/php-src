# Local macros for automake & autoconf

AC_DEFUN([XMLRPC_FUNCTION_CHECKS],[

# Standard XMLRPC list
AC_CHECK_FUNCS( \
 strtoul strtoull snprintf \
 strstr strpbrk strerror\
 memcpy memmove)

])

AC_DEFUN([XMLRPC_HEADER_CHECKS],[
AC_HEADER_STDC
AC_CHECK_HEADERS(xmlparse.h xmltok.h stdlib.h strings.h string.h)
])

AC_DEFUN([XMLRPC_TYPE_CHECKS],[

AC_REQUIRE([AC_C_CONST])
AC_REQUIRE([AC_C_INLINE])
AC_CHECK_SIZEOF(char, 1)

AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(long, 4)
AC_CHECK_SIZEOF(long long, 8)
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_TYPE_UID_T


])
