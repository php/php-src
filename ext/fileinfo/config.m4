PHP_ARG_ENABLE([fileinfo],
  [for fileinfo support],
  [AS_HELP_STRING([--disable-fileinfo],
    [Disable fileinfo support])],
  [yes])

if test "$PHP_FILEINFO" != "no"; then
  libmagic_sources=m4_normalize(["
    libmagic/apprentice.c
    libmagic/apptype.c
    libmagic/ascmagic.c
    libmagic/buffer.c
    libmagic/cdf_time.c
    libmagic/cdf.c
    libmagic/compress.c
    libmagic/der.c
    libmagic/encoding.c
    libmagic/fsmagic.c
    libmagic/funcs.c
    libmagic/is_csv.c
    libmagic/is_json.c
    libmagic/is_simh.c
    libmagic/is_tar.c
    libmagic/magic.c
    libmagic/print.c
    libmagic/readcdf.c
    libmagic/softmagic.c
  "])

  AC_CHECK_HEADERS([sys/sysmacros.h])

  AC_CHECK_FUNCS([strcasestr],,[
    AC_MSG_NOTICE([using libmagic strcasestr implementation])
    libmagic_sources="$libmagic_sources libmagic/strcasestr.c"
  ])

  AX_GCC_FUNC_ATTRIBUTE([visibility])

  PHP_NEW_EXTENSION([fileinfo],
    [fileinfo.c php_libmagic.c $libmagic_sources],
    [$ext_shared],,
    [-I@ext_srcdir@/libmagic])
  PHP_ADD_BUILD_DIR([$ext_builddir/libmagic])
  PHP_ADD_EXTENSION_DEP(fileinfo, pcre)

  AC_CHECK_FUNCS([utimes strndup])

  PHP_ADD_MAKEFILE_FRAGMENT
fi
