dnl $Id$
dnl config.m4 for extension pdo_odbc

AC_DEFUN(PDO_ODBC_HELP_TEXT, [[
 --with-pdo-odbc=flavour,dir  Add support for "flavour" ODBC drivers,
                              looking for include and lib dirs under "dir"
         
        flavour can be one of:
           ibm-db2, unixODBC

        You may omit the ,dir part to use a reasonable default for
        the flavour you have selected. e.g.:
            --with-pdo-odbc=unixODBC
        will check for unixODBC under /usr/local

        This extension will always be created as a shared extension
        named pdo_odbc.so
]])

PHP_ARG_WITH(pdo-odbc,  ODBC v3 driver for PDO, PDO_ODBC_HELP_TEXT)

AC_DEFUN([PDO_ODBC_CHECK_HEADER],[
  if test -f $PDO_ODBC_INCDIR/$1 ; then
    AC_DEFINE([HAVE_]translit($1,a-z_.-,A-Z___), 1, [ ])
  fi
])
                                  
AC_MSG_CHECKING(which ODBC flavour you want)
if test "$PHP_PDO_ODBC" != "no" && test "$PHP_PDO_ODBC" != "yes" ; then
  pdo_odbc_flavour=`echo $withval | cut -d, -f1`
  pdo_odbc_dir=`echo $withval | cut -d, -f2`

  if test "$pdo_odbc_dir" = "$withval" ; then
    pdo_odbc_dir=""
  fi

  case $pdo_odbc_flavour in
    ibm-db2)
        pdo_odbc_def_libdir=/home/db2inst/sqllib/lib
        pdo_odbc_def_incdir=/home/db2inst1/sqllib/include
        pdo_odbc_def_lib=-ldb2
        ;;

    unixODBC)
        pdo_odbc_def_libdir=/usr/local/lib
        pdo_odbc_def_incdir=/usr/local/include
        pdo_odbc_def_lib=-lodbc
        ;;

      *)
        AC_MSG_ERROR(Unknown ODBC flavour $pdo_odbc_flavour
PDO_ODBC_HELP_TEXT      
)
        ;;
  esac

  if test "$pdo_odbc_dir" != "" ; then
    PDO_ODBC_INCDIR="$pdo_odbc_dir/include"
    PDO_ODBC_LIBDIR="$pdo_odbc_dir/lib"
  else
    PDO_ODBC_INCDIR="$pdo_odbc_def_incdir"
    PDO_ODBC_LIBDIR="$pdo_odbc_def_libdir"
  fi
  
  AC_MSG_RESULT([$pdo_odbc_flavour
          libs       $PDO_ODBC_LIBDIR,
          headers    $PDO_ODBC_INCDIR])

  if ! test -d "$PDO_ODBC_LIBDIR" ; then
    AC_MSG_WARN([library dir $PDO_ODBC_LIBDIR does not exist])
  fi

  dnl yick time
  PDO_ODBC_CHECK_HEADER(odbc.h)
  PDO_ODBC_CHECK_HEADER(iodbc.h)
  PDO_ODBC_CHECK_HEADER(sqlunix.h)
  PDO_ODBC_CHECK_HEADER(sqltypes.h)
  PDO_ODBC_CHECK_HEADER(sqlucode.h)
  PDO_ODBC_CHECK_HEADER(sql.h)
  PDO_ODBC_CHECK_HEADER(isql.h)
  PDO_ODBC_CHECK_HEADER(sqlext.h)
  PDO_ODBC_CHECK_HEADER(isqlext.h)
  PDO_ODBC_CHECK_HEADER(udbcext.h)
  PDO_ODBC_CHECK_HEADER(sqlcli1.h)
  PDO_ODBC_CHECK_HEADER(LibraryManager.h)
  PDO_ODBC_CHECK_HEADER(cli0core.h)
  PDO_ODBC_CHECK_HEADER(cli0ext.h)
  PDO_ODBC_CHECK_HEADER(cli0cli.h)
  PDO_ODBC_CHECK_HEADER(cli0defs.h)
  PDO_ODBC_CHECK_HEADER(cli0env.h)

  PDO_ODBC_INCLUDE="-I$PDO_ODBC_INCDIR -DPDO_ODBC_TYPE=\\\"$pdo_odbc_flavour\\\""
  PDO_ODBC_LFLAGS="-L$PDO_ODBC_LIBDIR"
  PDO_ODBC_LIBS="$pdo_odbc_def_lib"

  LDFLAGS="$PDO_ODBC_LFLAGS $PDO_ODBC_LIBS -lm -ldl"
  AC_TRY_LINK_FUNC([SQLAllocHandle],[],[
    AC_MSG_ERROR([[Your ODBC libraries either do not exist, or do not appear to be ODBC3 compatible]])
  ])
  LDFLAGS=$save_old_LDFLAGS
  PHP_EVAL_LIBLINE($PDO_ODBC_LIBS $PDO_ODBC_LFLAGS, [PDO_ODBC_SHARED_LIBADD])
  PHP_SUBST(PDO_ODBC_SHARED_LIBADD)

  PHP_NEW_EXTENSION(pdo_odbc, pdo_odbc.c odbc_driver.c odbc_stmt.c, yes,,-I\$prefix/include/php/ext $PDO_ODBC_INCLUDE)
else
  AC_MSG_ERROR(
Unknown ODBC flavour $PHP_PDO_ODBC
PDO_ODBC_HELP_TEXT
)
fi

