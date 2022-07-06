PHP_ARG_ENABLE([fileinfo],
  [for fileinfo support],
  [AS_HELP_STRING([--disable-fileinfo],
    [Disable fileinfo support])],
  [yes])

if test "$PHP_FILEINFO" != "no"; then

  libmagic_sources=" \
    libmagic/apprentice.c libmagic/apptype.c libmagic/ascmagic.c \
    libmagic/cdf.c libmagic/cdf_time.c libmagic/compress.c \
    libmagic/encoding.c libmagic/fsmagic.c libmagic/funcs.c \
    libmagic/is_json.c libmagic/is_tar.c libmagic/magic.c libmagic/print.c \
    libmagic/readcdf.c libmagic/softmagic.c libmagic/der.c \
    libmagic/buffer.c libmagic/is_csv.c"

  AC_MSG_CHECKING([for strcasestr])
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <string.h>
#include <strings.h>
#include <stdlib.h>

int main(void)
{
        char *s0, *s1, *ret;

        s0 = (char *) malloc(42);
        s1 = (char *) malloc(8);

        memset(s0, 'X', 42);
        s0[24] = 'Y';
        s0[26] = 'Z';
        s0[41] = '\0';
        memset(s1, 'x', 8);
        s1[0] = 'y';
        s1[2] = 'Z';
        s1[7] = '\0';

        ret = strcasestr(s0, s1);

        return !(NULL != ret);
}
  ]])],[
    dnl using the platform implementation
    AC_MSG_RESULT(yes)
  ],[
    AC_MSG_RESULT(no)
    AC_MSG_NOTICE(using libmagic strcasestr implementation)
    libmagic_sources="$libmagic_sources libmagic/strcasestr.c"
  ],[AC_MSG_RESULT([skipped, cross-compiling])])

  PHP_NEW_EXTENSION(fileinfo, fileinfo.c $libmagic_sources, $ext_shared,,-I@ext_srcdir@/libmagic)
  PHP_ADD_BUILD_DIR($ext_builddir/libmagic)

  AC_CHECK_FUNCS([utimes strndup])

  PHP_ADD_MAKEFILE_FRAGMENT
fi
