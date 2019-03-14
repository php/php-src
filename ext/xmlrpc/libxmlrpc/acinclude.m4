# Local macros for autoconf

AC_DEFUN([XMLRPC_FUNCTION_CHECKS],[

# Standard XMLRPC list
AC_CHECK_FUNCS(strtoul strtoull)
])

AC_DEFUN([XMLRPC_HEADER_CHECKS],[
AC_CHECK_HEADERS(xmlparse.h xmltok.h strings.h)
])
