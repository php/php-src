dnl $Id$
dnl config.m4 for extension sqlite

PHP_ARG_WITH(sqlite, for sqlite support,
[  --with-sqlite             Include sqlite support])

if test "$PHP_SQLITE" != "no"; then

  if test "$PHP_SQLITE" != "yes"; then
	SEARCH_PATH="/usr/local /usr"
	SEARCH_FOR="/include/sqlite.h"
	if test -r $PHP_SQLITE/; then # path given as parameter
		SQLITE_DIR=$PHP_SQLITE
	else # search default path list
		AC_MSG_CHECKING([for sqlite files in default path])
		for i in $SEARCH_PATH ; do
		if test -r $i/$SEARCH_FOR; then
			SQLITE_DIR=$i
			AC_MSG_RESULT(found in $i)
		fi
		done
	fi
  
	if test -z "$SQLITE_DIR"; then
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([Please reinstall the sqlite distribution from http://www.sqlite.org])
	fi

	PHP_ADD_INCLUDE($SQLITE_DIR/include)

	LIBNAME=sqlite
	LIBSYMBOL=sqlite_open

	PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
	[
		PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SQLITE_DIR/lib, SQLITE_SHARED_LIBADD)
		AC_DEFINE(HAVE_SQLITELIB,1,[ ])
	],[
		AC_MSG_ERROR([wrong sqlite lib version or lib not found])
	],[
		-L$SQLITE_DIR/lib -lm -ldl
	])
 
	PHP_SUBST(SQLITE_SHARED_LIBADD)
	PHP_NEW_EXTENSION(sqlite, sqlite.c libsqlite/src/encode.c, $ext_shared)
  else
	# use bundled library

	PHP_SQLITE_CFLAGS="-I@ext_srcdir@/libsqlite/src"


	sources="libsqlite/src/opcodes.c
		libsqlite/src/parse.c libsqlite/src/encode.c \
		libsqlite/src/auth.c libsqlite/src/btree.c libsqlite/src/build.c \
		libsqlite/src/delete.c libsqlite/src/expr.c libsqlite/src/func.c \
		libsqlite/src/hash.c libsqlite/src/insert.c libsqlite/src/main.c \
		libsqlite/src/os.c libsqlite/src/pager.c \
		libsqlite/src/printf.c libsqlite/src/random.c \
		libsqlite/src/select.c libsqlite/src/table.c libsqlite/src/tokenize.c \
		libsqlite/src/update.c libsqlite/src/util.c libsqlite/src/vdbe.c \
		libsqlite/src/where.c libsqlite/src/trigger.c"
	
  	PHP_NEW_EXTENSION(sqlite, sqlite.c $sources, $ext_shared,,$PHP_SQLITE_CFLAGS)
	PHP_ADD_BUILD_DIR($ext_builddir/libsqlite)
	PHP_ADD_BUILD_DIR($ext_builddir/libsqlite/src)
	AC_CHECK_SIZEOF(char *,4)
	AC_DEFINE(SQLITE_PTR_SZ, SIZEOF_CHAR_P, [Size of a pointer])
	AC_DEFINE(OS_UNIX, 1, [if this is unix])
	AC_DEFINE(OS_WIN, 0, [if this is windows])
	dnl use latin 1 for now; the utf-8 handling in funcs.c uses assert(),
	dnl which is a bit silly and something we want to avoid
	SQLITE_ENCODING="iso8859"
	dnl SQLITE_ENCODING="UTF-8"
	dnl AC_DEFINE(SQLITE_UTF8,1,[if SQLite should use utf-8 encoding])
	AC_SUBST(SQLITE_ENCODING)

	AC_PATH_PROG(LEMON,lemon,no)
	AC_SUBST(LEMON)

	SQLITE_VERSION=`cat $ext_srcdir/libsqlite/VERSION`
	AC_SUBST(SQLITE_VERSION)

	if test "$ext_shared" = "no"; then
	  echo '#include "php_config.h"' > $ext_srcdir/libsqlite/src/config.h
	else
	  echo "#include \"$abs_builddir/config.h\"" > $ext_srcdir/libsqlite/src/config.h
	fi
	
	cat >> $ext_srcdir/libsqlite/src/config.h <<EOF
#if ZTS
# define THREADSAFE 1
#endif
#if !ZEND_DEBUG
# define NDEBUG
#endif
EOF

	sed -e s/--VERS--/`cat $ext_srcdir/libsqlite/VERSION`/ -e s/--ENCODING--/$SQLITE_ENCODING/ $ext_srcdir/libsqlite/src/sqlite.h.in >$ext_srcdir/libsqlite/src/sqlite.h

	PHP_ADD_MAKEFILE_FRAGMENT

  fi

  AC_CHECK_FUNCS(usleep nanosleep)

  AC_CHECK_HEADERS(time.h)
fi
