dnl
dnl $Id$
dnl

dnl Suppose we need FlatFile if no support or only CDB is used.

AC_DEFUN(PHP_DBA_STD_BEGIN,[
  unset THIS_INCLUDE THIS_LIBS THIS_LFLAGS THIS_PREFIX THIS_RESULT
])

AC_DEFUN(PHP_TEMP_LDFLAGS,[
  old_LDFLAGS=$LDFLAGS
  LDFLAGS="$1 $LDFLAGS"
  $2
  LDFLAGS=$old_LDFLAGS
])

dnl Assign INCLUDE/LFLAGS from PREFIX
AC_DEFUN(PHP_DBA_STD_ASSIGN,[
  if test -n "$THIS_PREFIX" && test "$THIS_PREFIX" != "/usr"; then
    THIS_LFLAGS=$THIS_PREFIX/lib
  fi
])

dnl Standard check
AC_DEFUN(PHP_DBA_STD_CHECK,[
  THIS_RESULT="yes"
  if test -z "$THIS_INCLUDE"; then
    AC_MSG_ERROR(cannot find necessary header file(s))
  fi
  if test -z "$THIS_LIBS"; then
    AC_MSG_ERROR(cannot find necessary library)
  fi
])

dnl Attach THIS_x to DBA_x
AC_DEFUN(PHP_DBA_STD_ATTACH,[
  PHP_ADD_LIBRARY_WITH_PATH($THIS_LIBS, $THIS_LFLAGS, DBA_SHARED_LIBADD)
  unset THIS_INCLUDE THIS_LIBS THIS_LFLAGS THIS_PREFIX
])

dnl Print the result message
dnl parameters(name [, full name [, empty or error message]])
AC_DEFUN(AC_DBA_STD_RESULT,[
  THIS_NAME=[]translit($1,a-z0-9-,A-Z0-9_)
  if test -n "$2"; then
    THIS_FULL_NAME="$2"
  else
    THIS_FULL_NAME="$THIS_NAME"
  fi
  AC_MSG_CHECKING(for $THIS_FULL_NAME support)
  if test -n "$3"; then
    AC_MSG_ERROR($3)
  fi
  if test "$THIS_RESULT" = "yes" -o "$THIS_RESULT" = "builtin"; then
    HAVE_DBA=1
    eval HAVE_$THIS_NAME=1
    AC_MSG_RESULT($THIS_RESULT)
  else
    AC_MSG_RESULT(no)
  fi
  unset THIS_RESULT THIS_NAME THIS_FULL_NAME
])

PHP_ARG_ENABLE(dba,whether to enable DBA,
[  --enable-dba            Build DBA with builtin modules])

AC_ARG_WITH(gdbm,
[  --with-gdbm[=DIR]         DBA: Include GDBM support],[
  if test "$withval" != "no"; then
    PHP_DBA_STD_BEGIN
    for i in $withval /usr/local /usr; do
      if test -f "$i/include/gdbm.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/gdbm.h
        break
      fi
    done

    if test -n "$THIS_INCLUDE"; then
      unset ac_cv_lib_gdbm_gdbm_open
      PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
        AC_CHECK_LIB(gdbm, gdbm_open, [
          AC_DEFINE_UNQUOTED(GDBM_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
          AC_DEFINE(DBA_GDBM, 1, [ ]) 
          THIS_LIBS=gdbm
        ])
      ])
    fi
    
    PHP_DBA_STD_ASSIGN
    PHP_DBA_STD_CHECK
    PHP_DBA_STD_ATTACH
  fi
])
AC_DBA_STD_RESULT(gdbm)

AC_ARG_WITH(ndbm,
[  --with-ndbm[=DIR]         DBA: Include NDBM support],[
  if test "$withval" != "no"; then
    PHP_DBA_STD_BEGIN
    for i in $withval /usr/local /usr; do
      if test -f "$i/include/ndbm.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/ndbm.h
        break
      elif test -f "$i/include/db1/ndbm.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db1/ndbm.h
        break
      fi
    done
    
    if test -n "$THIS_INCLUDE"; then
      for LIB in ndbm db1 c; do
        PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
          AC_CHECK_LIB($LIB, dbm_open, [
            AC_DEFINE_UNQUOTED(NDBM_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
            AC_DEFINE(DBA_NDBM,1, [ ]) 
            THIS_LIBS=$LIB
            break
          ])
        ])
      done
    fi

    PHP_DBA_STD_ASSIGN
    PHP_DBA_STD_CHECK
    PHP_DBA_STD_ATTACH
  fi
])
AC_DBA_STD_RESULT(ndbm)

dnl Berkeley specific (library and version test)
dnl parameters(version, library list, function)
AC_DEFUN(PHP_DBA_DB_CHECK,[
  for LIB in $2; do
    if test -f $THIS_PREFIX/lib/lib$LIB.a -o -f $THIS_PREFIX/lib/lib$LIB.$SHLIB_SUFFIX_NAME; then
      PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib -l$LIB,[
        AC_TRY_LINK([
#include "$THIS_INCLUDE"
        ],[
          $3;
        ],[
          AC_EGREP_CPP(yes,[
#include "$THIS_INCLUDE"
#if DB_VERSION_MAJOR == $1
            yes
#endif
          ],[
            THIS_LIBS=$LIB
            break
          ])
        ])
      ])
    fi
  done
  if test -z "$THIS_LIBS"; then
	AC_MSG_CHECKING(for db$1 major version)
    AC_MSG_ERROR(Header contains different version)
  fi
  if test "$1" = "4"; then
    AC_MSG_CHECKING(for db4 minor version and patch level)
    AC_EGREP_CPP(yes,[
#include "$THIS_INCLUDE"
#if DB_VERSION_MINOR != 1 || DB_VERSION_PATCH >= 25
      yes
#endif
    ],[
      AC_MSG_RESULT(ok)
    ],[
      AC_MSG_ERROR(Version 4.1 requires patch level 25)
    ])
  fi
  if test "$ext_shared" = "yes"; then
    AC_MSG_CHECKING(if db can be used as shared extension)
    AC_EGREP_CPP(yes,[
#include "$THIS_INCLUDE"
#if DB_VERSION_MAJOR > 3 || (DB_VERSION_MAJOR == 3 && DB_VERSION_MINOR > 2)
      yes
#endif
    ],[
      AC_MSG_RESULT(yes)
    ],[
      AC_MSG_ERROR(At least version 3.3 is required)
    ])
  fi
  if test -n "$THIS_LIBS"; then
    AC_DEFINE(DBA_DB$1, 1, [ ]) 
    if test -n "$THIS_INCLUDE"; then
      AC_DEFINE_UNQUOTED(DB$1_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
    fi
  fi
  PHP_DBA_STD_ASSIGN
  PHP_DBA_STD_CHECK
  PHP_DBA_STD_ATTACH
])

AC_ARG_WITH(db4,
[  --with-db4[=DIR]          DBA: Include Berkeley DB4 support],[
  if test "$withval" != "no"; then
    PHP_DBA_STD_BEGIN
    for i in $withval /usr/local/BerkeleyDB.4.1 /usr/local/BerkeleyDB.4.0 /usr/local /usr; do
      if test -f "$i/db4/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/db4/db.h
        break
      elif test -f "$i/include/db4/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db4/db.h
        break
      elif test -f "$i/include/db/db4.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db/db4.h
        break
      elif test -f "$i/include/db4.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db4.h
        break
      elif test -f "$i/include/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db.h
        break
      fi
    done
    PHP_DBA_DB_CHECK(4, db-4.1 db-4.0 db-4 db4 db, [(void)db_create((DB**)0, (DB_ENV*)0, 0)])
  fi
])
AC_DBA_STD_RESULT(db4,Berkeley DB4)

AC_ARG_WITH(db3,
[  --with-db3[=DIR]          DBA: Include Berkeley DB3 support],[
  if test "$withval" != "no"; then
    PHP_DBA_STD_BEGIN
    if test "$HAVE_DB4" = "1"; then
      AC_DBA_STD_RESULT(db3,Berkeley DB3,You cannot combine --with-db3 with --with-db4)
    fi
    for i in $withval /usr/local/BerkeleyDB.3.3 /usr/local/BerkeleyDB.3.2 /usr/local/BerkeleyDB.3.1 /usr/local/BerkeleyDB.3.0 /usr/local /usr; do
      if test -f "$i/db3/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db3/db.h
        break
      elif test -f "$i/include/db3/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db3/db.h
        break
      elif test -f "$i/include/db/db3.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db/db3.h
        break
      elif test -f "$i/include/db3.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db3.h
        break
      elif test -f "$i/include/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db.h
        break
      fi
    done
    PHP_DBA_DB_CHECK(3, db-3.3 db-3.2 db-3.1 db-3.0 db-3 db3 db, [(void)db_create((DB**)0, (DB_ENV*)0, 0)])
  fi
      ])
AC_DBA_STD_RESULT(db3,Berkeley DB3)

AC_ARG_WITH(db2,
[  --with-db2[=DIR]          DBA: Include Berkeley DB2 support],[
  if test "$withval" != "no"; then
    PHP_DBA_STD_BEGIN
    if test "$HAVE_DB3" = "1" -o "$HAVE_DB4" = "1"; then
      AC_DBA_STD_RESULT(db2,Berkeley DB2,You cannot combine --with-db2 with --with-db3 or --with-db4)
    fi
    for i in $withval $withval/BerkeleyDB /usr/BerkeleyDB /usr/local /usr; do
      if test -f "$i/db2/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/db2/db.h
        break
      elif test -f "$i/include/db2/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db2/db.h
        break
      elif test -f "$i/include/db/db2.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db/db2.h
        break
      elif test -f "$i/include/db2.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db2.h
        break
      elif test -f "$i/include/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db.h
        break
      fi
    done
    PHP_DBA_DB_CHECK(2, db-2 db2 db, [(void)db_appinit("", NULL, (DB_ENV*)0, 0)])
  fi
])
AC_DBA_STD_RESULT(db2,Berkeley DB2)

AC_ARG_WITH(dbm,
[  --with-dbm[=DIR]          DBA: Include DBM support],[
  if test "$withval" != "no"; then
    PHP_DBA_STD_BEGIN
    for i in $withval /usr/local /usr; do
      if test -f "$i/include/dbm.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/dbm.h
        break
      elif test -f "$i/include/gdbm/dbm.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/gdbm/dbm.h
        break
      fi
    done

    if test -n "$THIS_INCLUDE"; then
      for LIB in dbm c gdbm; do
        PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
          AC_CHECK_LIB($LIB, dbminit, [
            AC_MSG_CHECKING(for DBM using GDBM)
            AC_DEFINE_UNQUOTED(DBM_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
            if test "$LIB" = "gdbm"; then
              AC_DEFINE_UNQUOTED(DBM_VERSION, "GDBM", [ ])
              AC_MSG_RESULT(yes)
            else
              AC_DEFINE_UNQUOTED(DBM_VERSION, "DBM", [ ])
              AC_MSG_RESULT(no)
            fi
            AC_DEFINE(DBA_DBM,1,[ ]) 
            THIS_LIBS=$LIB
          ])
          if test -n "$THIS_LIBS"; then
            break
          fi
        ])
      done
    fi
    
    PHP_DBA_STD_ASSIGN
    PHP_DBA_STD_CHECK
    PHP_DBA_STD_ATTACH
  fi
])
AC_DBA_STD_RESULT(dbm)

AC_DEFUN(PHP_DBA_BUILTIN_CDB,[
  AC_DEFINE(DBA_CDB_BUILTIN, 1, [ ])
  AC_DEFINE(DBA_CDB_MAKE, 1, [ ])
  AC_DEFINE(DBA_CDB, 1, [ ])
  cdb_sources="libcdb/cdb.c libcdb/cdb_make.c libcdb/uint32.c"
  THIS_RESULT="builtin"
])

AC_ARG_WITH(cdb,
[  --with-cdb[=DIR]          DBA: Include CDB support],[
  if test "$withval" = "yes" -o "$HAVE_DBA" = "1"; then
    PHP_DBA_BUILTIN_CDB
  elif test "$withval" != "no"; then
    PHP_DBA_STD_BEGIN
    for i in $withval /usr/local /usr; do
      if test -f "$i/include/cdb.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/cdb.h
        break
      fi
    done

    if test -n "$THIS_INCLUDE"; then
      for LIB in cdb c; do
        PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
          AC_CHECK_LIB($LIB, cdb_read, [
            AC_DEFINE_UNQUOTED(CDB_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
            AC_DEFINE(DBA_CDB,1,[ ]) 
            THIS_LIBS=$LIB
            break
          ])
        ])
      done
    fi
    
    PHP_DBA_STD_ASSIGN
    PHP_DBA_STD_CHECK
    PHP_DBA_STD_ATTACH
  fi
],[
  if test "$PHP_DBA" != "no" -o "$HAVE_DBA" = "1"; then
    PHP_DBA_BUILTIN_CDB
  fi
])
AC_DBA_STD_RESULT(cdb)

AC_DEFUN(PHP_DBA_BUILTIN_FLATFILE,[
  AC_DEFINE(DBA_FLATFILE, 1, [ ])
  flat_sources="libflatfile/flatfile.c"
  THIS_RESULT="builtin"
])

dnl
dnl FlatFile check must be the last one.
dnl
AC_ARG_WITH(flatfile,
[  --with-flatfile           DBA: Include FlatFile support],[
  if test "$withval" != "no"; then
    PHP_DBA_BUILTIN_FLATFILE
  fi
],[
  if test "$PHP_DBA" != "no" -o "$HAVE_DBA" = "1"; then
    PHP_DBA_BUILTIN_FLATFILE
  fi
])
AC_DBA_STD_RESULT(FlatFile,FlatFile)

AC_MSG_CHECKING(whether to enable DBA interface)
if test "$HAVE_DBA" = "1"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_DBA, 1, [ ])
  PHP_NEW_EXTENSION(dba, dba.c dba_cdb.c dba_db2.c dba_dbm.c dba_gdbm.c dba_ndbm.c dba_db3.c dba_db4.c dba_flatfile.c $cdb_sources $flat_sources, $ext_shared)
  PHP_ADD_BUILD_DIR($ext_builddir/libcdb)
  PHP_ADD_BUILD_DIR($ext_builddir/libflatfile)
  PHP_SUBST(DBA_SHARED_LIBADD)
else
  AC_MSG_RESULT(no)
fi
