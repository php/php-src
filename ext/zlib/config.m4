dnl $Id$



PHP_ARG_WITH(zlib,whether to include zlib support,
[  --with-zlib[=DIR]       Include zlib support (requires zlib >= 1.0.9).
                          DIR is the zlib install directory,
                          defaults to /usr.])

AC_DEFUN(AC_TEMP_LDFLAGS,[
  old_LDFLAGS="$LDFLAGS"
  LDFLAGS="$1 $LDFLAGS"
  $2
  LDFLAGS="$old_LDFLAGS"
])

if test "$PHP_ZLIB" != "no"; then
  PHP_EXTENSION(zlib, $ext_shared)
  for i in /usr/local /usr $PHP_ZLIB; do
    if test -f $i/include/zlib/zlib.h; then
      ZLIB_DIR=$i
      ZLIB_INCDIR=$i/include/zlib
    elif test -f $i/include/zlib.h; then
      ZLIB_DIR=$i
      ZLIB_INCDIR=$i/include
    fi
  done

  if test -z "$ZLIB_DIR"; then
    AC_MSG_ERROR(Cannot find libz)
  fi

  ZLIB_LIBDIR=$ZLIB_DIR/lib

  AC_TEMP_LDFLAGS(-L$ZLIB_LIBDIR,[
  AC_CHECK_LIB(z, gzgets, [AC_DEFINE(HAVE_ZLIB,1,[ ])],
    [AC_MSG_ERROR(Zlib module requires zlib >= 1.0.9.)])
  ])

  PHP_SUBST(ZLIB_SHARED_LIBADD)
  AC_ADD_LIBRARY_WITH_PATH(z, $ZLIB_LIBDIR, ZLIB_SHARED_LIBADD)
  
  AC_ADD_INCLUDE($ZLIB_INCDIR)

	dnl check for fopencookie() from glibc
	AC_CHECK_FUNC(fopencookie, [ have_glibc_fopencookie=yes ])

	if test "$have_glibc_fopencookie" = "yes" ; then
	  	dnl this comes in two flavors:
      dnl newer glibcs (since 2.1.2 ? )
      dnl have a type called cookie_io_functions_t
		  AC_TRY_COMPILE([ #define _GNU_SOURCE
                       #include <stdio.h>
									   ],
	                   [ cookie_io_functions_t cookie; ],
                     [ have_cookie_io_functions_t=yes ],
										 [ ] )

		  if test "$have_cookie_io_functions_t" = "yes" ; then
        cookie_io_functions_t=cookie_io_functions_t
	      have_fopen_cookie=yes
      else
  	    dnl older glibc versions (up to 2.1.2 ?)
        dnl call it _IO_cookie_io_functions_t
		    AC_TRY_COMPILE([ #define _GNU_SOURCE
                       #include <stdio.h>
									   ],
	                   [ _IO_cookie_io_functions_t cookie; ],
                     [ have_IO_cookie_io_functions_t=yes ],
										 [] )
		    if test "$have_cookie_io_functions_t" = "yes" ; then
          cookie_io_functions_t=_IO_cookie_io_functions_t
	        have_fopen_cookie=yes
		    fi
			fi

		  if test "$have_fopen_cookie" = "yes" ; then
		    AC_DEFINE(HAVE_FOPENCOOKIE, 1, [ ])
			  AC_DEFINE_UNQUOTED(COOKIE_IO_FUNCTIONS_T, $cookie_io_functions_t, [ ])
      fi      

  	fi

fi
