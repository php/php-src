


AC_DEFUN(LIB_SHARED_CONVENIENCE,[
  lib_target="\$(LTLIBRARY_NAME)"
  cat >>$1<<EOF
\$(LTLIBRARY_NAME): \$(LTLIBRARY_SHARED_OBJECTS) \$(LTLIBRARY_DEPENDENCIES)
	\$(SHARED_LIBTOOL) --mode=link \$(CCLD) \$(CFLAGS) \$(EXTRA_CFLAGS) \$(LDFLAGS) -o \[$]@ \$(LTLIBRARY_LDFLAGS) \$(LTLIBRARY_OBJECTS) \$(LTLIBRARY_SHARED_LIBADD)

EOF
])

AC_DEFUN(LIB_SHARED_MODULE,[
  lib_target="\$(LTLIBRARY_SHARED_NAME)"
  cat >>$1<<EOF
\$(LTLIBRARY_SHARED_NAME): \$(LTLIBRARY_SHARED_OBJECTS) \$(LTLIBRARY_DEPENDENCIES)
	\$(SHARED_LIBTOOL) --mode=link \$(CCLD) \$(CFLAGS) \$(EXTRA_CFLAGS) \$(LDFLAGS) -o \[$]@ -avoid-version -module -rpath \$(phplibdir) \$(LTLIBRARY_LDFLAGS) \$(LTLIBRARY_OBJECTS) \$(LTLIBRARY_SHARED_LIBADD)
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
  cat >$lib_makefile<<EOF
LTLIBRARY_OBJECTS = \$(LTLIBRARY_SOURCES:.c=.lo)
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
