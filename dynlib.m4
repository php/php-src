#  +----------------------------------------------------------------------+
#  | PHP Version 4                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2002 The PHP Group                                |
#  +----------------------------------------------------------------------+
#  | This source file is subject to version 2.02 of the PHP license,      |
#  | that is bundled with this package in the file LICENSE, and is        |
#  | available at through the world-wide-web at                           |
#  | http://www.php.net/license/2_02.txt.                                 |
#  | If you did not receive a copy of the PHP license and are unable to   |
#  | obtain it through the world-wide-web, please send a note to          |
#  | license@php.net so we can mail you a copy immediately.               |
#  +----------------------------------------------------------------------+
#  | Author: Sascha Schumann <sascha@schumann.cx>                         |
#  +----------------------------------------------------------------------+
#
# $Id$ 
#



AC_DEFUN(LIB_SHARED_CONVENIENCE,[
  lib_target="\$(LTLIBRARY_NAME)"
  cat >>$1<<EOF
\$(LTLIBRARY_NAME): \$(LTLIBRARY_SHARED_OBJECTS) \$(LTLIBRARY_DEPENDENCIES)
	\$(SHARED_LIBTOOL) --mode=link \$(COMPILE) \$(LDFLAGS) -o \[$]@ \$(LTLIBRARY_LDFLAGS) \$(LTLIBRARY_OBJECTS) \$(LTLIBRARY_SHARED_LIBADD)

EOF
])

AC_DEFUN(LIB_SHARED_MODULE,[
  lib_target="\$(LTLIBRARY_SHARED_NAME)"
  cat >>$1<<EOF
\$(LTLIBRARY_SHARED_NAME): \$(LTLIBRARY_SHARED_OBJECTS) \$(LTLIBRARY_DEPENDENCIES)
	\$(SHARED_LIBTOOL) --mode=link \$(CC) \$(COMMON_FLAGS) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(LDFLAGS) -o \[$]@ -avoid-version -module -rpath \$(phplibdir) \$(LTLIBRARY_LDFLAGS) \$(LTLIBRARY_OBJECTS) \$(LTLIBRARY_SHARED_LIBADD)
	\$(SHARED_LIBTOOL) --mode=install cp \[$]@ \$(phplibdir)

EOF
])

AC_DEFUN(LIB_STATIC_CONVENIENCE,[
  lib_target="\$(LTLIBRARY_NAME)"
  cat >>$1<<EOF
\$(LTLIBRARY_NAME): \$(LTLIBRARY_OBJECTS) \$(LTLIBRARY_DEPENDENCIES)
	\$(LINK) \$(LTLIBRARY_LDFLAGS) \$(LTLIBRARY_OBJECTS) \$(LTLIBRARY_LIBADD)

EOF
])

dnl LIB_BUILD(path, shared, convenience)
dnl sets up path to build a shared/static convenience/module
AC_DEFUN(LIB_BUILD,[
  lib_makefile="$1/libs.mk"
  lib_target=""
  
  $php_shtool mkdir -p $1
  if test "$BSD_MAKEFILE" = "yes"; then
    lib_include_conf=".include \"\$(top_builddir)/config_vars.mk\""
  else
    lib_include_conf="include \$(top_builddir)/config_vars.mk"
  fi
  cat >$lib_makefile<<EOF
$lib_include_conf
LTLIBRARY_OBJECTS = \$(LTLIBRARY_SOURCES:.c=.lo) \$(LTLIBRARY_OBJECTS_X)
LTLIBRARY_SHARED_OBJECTS = \$(LTLIBRARY_OBJECTS:.lo=.slo)
EOF

  if test "$2" = "shared" || test "$2" = "yes"; then
    lib_build_shared=yes
    if test -n "$3"; then
dnl ---------------------------------------- Shared Convenience
      LIB_SHARED_CONVENIENCE($lib_makefile)
    else
dnl ---------------------------------------- Shared Module
      LIB_SHARED_MODULE($lib_makefile)
    fi
  else
dnl ---------------------------------------- Static Convenience = Static Module
    LIB_STATIC_CONVENIENCE($lib_makefile)
  fi

dnl ---------------------------------------- Generate build targets
  if test -n "$lib_target"; then
    cat >>$lib_makefile<<EOF
targets = $lib_target
EOF
  fi
])
