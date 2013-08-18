dnl
dnl $Id$
dnl

dnl Suppose we need FlatFile if no support or only CDB is used.

AC_DEFUN([PHP_DBA_STD_BEGIN],[
  unset THIS_INCLUDE THIS_LIBS THIS_LFLAGS THIS_PREFIX THIS_RESULT
])

AC_DEFUN([PHP_TEMP_LDFLAGS],[
  old_LDFLAGS=$LDFLAGS
  LDFLAGS="$1 $LDFLAGS"
  old_LIBS=$LIBS
  LIBS="$2 $LIBS"
  $3
  LDFLAGS=$old_LDFLAGS
  LIBS=$old_LIBS
])

dnl Assign INCLUDE/LFLAGS from PREFIX
AC_DEFUN([PHP_DBA_STD_ASSIGN],[
  if test -n "$THIS_PREFIX" && test "$THIS_PREFIX" != "/usr"; then
    THIS_LFLAGS=$THIS_PREFIX/$PHP_LIBDIR
  fi
])

dnl Standard check
AC_DEFUN([PHP_DBA_STD_CHECK],[
  THIS_RESULT=yes
  if test -z "$THIS_INCLUDE"; then
    AC_MSG_ERROR([DBA: Could not find necessary header file(s).])
  fi
  if test -z "$THIS_LIBS"; then
    AC_MSG_ERROR([DBA: Could not find necessary library.])
  fi
])

dnl Attach THIS_x to DBA_x
AC_DEFUN([PHP_DBA_STD_ATTACH],[
  PHP_ADD_LIBRARY_WITH_PATH($THIS_LIBS, $THIS_LFLAGS, DBA_SHARED_LIBADD)
  unset THIS_INCLUDE THIS_LIBS THIS_LFLAGS THIS_PREFIX
])

dnl Print the result message
dnl parameters(name [, full name [, empty or error message]])
AC_DEFUN([PHP_DBA_STD_RESULT],[
  THIS_NAME=[]translit($1,a-z0-9-,A-Z0-9_)
  if test -n "$2"; then
    THIS_FULL_NAME="$2"
  else
    THIS_FULL_NAME="$THIS_NAME"
  fi
  AC_MSG_CHECKING([for $THIS_FULL_NAME support])
  if test -n "$3"; then
    AC_MSG_ERROR($3)
  fi
  if test "$THIS_RESULT" = "yes" || test "$THIS_RESULT" = "builtin"; then
    HAVE_DBA=1
    eval HAVE_$THIS_NAME=1
    AC_MSG_RESULT([$THIS_RESULT])
  else
    AC_MSG_RESULT(no)
  fi
  unset THIS_RESULT THIS_NAME THIS_FULL_NAME
])

dnl
dnl Options
dnl

PHP_ARG_ENABLE(dba,,
[  --enable-dba            Build DBA with bundled modules. To build shared DBA
                          extension use --enable-dba=shared])

PHP_ARG_WITH(qdbm,,
[  --with-qdbm[=DIR]         DBA: QDBM support], no, no)

PHP_ARG_WITH(gdbm,,
[  --with-gdbm[=DIR]         DBA: GDBM support], no, no)

PHP_ARG_WITH(ndbm,,
[  --with-ndbm[=DIR]         DBA: NDBM support], no, no)

PHP_ARG_WITH(db4,,
[  --with-db4[=DIR]          DBA: Oracle Berkeley DB 4.x or 5.x support], no, no)

PHP_ARG_WITH(db3,,
[  --with-db3[=DIR]          DBA: Oracle Berkeley DB 3.x support], no, no)

PHP_ARG_WITH(db2,,
[  --with-db2[=DIR]          DBA: Oracle Berkeley DB 2.x support], no, no)

PHP_ARG_WITH(db1,,
[  --with-db1[=DIR]          DBA: Oracle Berkeley DB 1.x support/emulation], no, no)

PHP_ARG_WITH(dbm,,
[  --with-dbm[=DIR]          DBA: DBM support], no, no)

PHP_ARG_WITH(tcadb,,
[  --with-tcadb[=DIR]        DBA: Tokyo Cabinet abstract DB support], no, no)


dnl
dnl Library checks
dnl

# QDBM
if test "$PHP_QDBM" != "no"; then
  PHP_DBA_STD_BEGIN
  for i in $PHP_QDBM /usr/local /usr; do
    if test -f "$i/include/depot.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/include/depot.h
      break
    fi
  done

  if test -n "$THIS_INCLUDE"; then
    for LIB in qdbm; do
      PHP_CHECK_LIBRARY($LIB, dpopen, [
        AC_DEFINE_UNQUOTED(QDBM_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
        AC_DEFINE(DBA_QDBM, 1, [ ])
        THIS_LIBS=$LIB
      ], [], [-L$THIS_PREFIX/$PHP_LIBDIR])
      if test -n "$THIS_LIBS"; then
        break
      fi
    done
  fi

  PHP_DBA_STD_ASSIGN
  PHP_DBA_STD_CHECK
  PHP_DBA_STD_ATTACH
fi
PHP_DBA_STD_RESULT(qdbm)

# GDBM
if test "$PHP_GDBM" != "no"; then
  PHP_DBA_STD_BEGIN
  if test "$HAVE_QDBM" = "1"; then
    PHP_DBA_STD_RESULT(gdbm, gdbm, [You cannot combine --with-gdbm with --with-qdbm])
  fi
  for i in $PHP_GDBM /usr/local /usr; do
    if test -f "$i/include/gdbm.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/include/gdbm.h
      break
    fi
  done

  if test -n "$THIS_INCLUDE"; then
    PHP_CHECK_LIBRARY(gdbm, gdbm_open, [
      AC_DEFINE_UNQUOTED(GDBM_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
      AC_DEFINE(DBA_GDBM, 1, [ ]) 
      THIS_LIBS=gdbm
    ], [], [-L$THIS_PREFIX/$PHP_LIBDIR])
  fi
    
  PHP_DBA_STD_ASSIGN
  PHP_DBA_STD_CHECK
  PHP_DBA_STD_ATTACH
fi
PHP_DBA_STD_RESULT(gdbm)

# NDBM
if test "$PHP_NDBM" != "no"; then
  PHP_DBA_STD_BEGIN
  for i in $PHP_NDBM /usr/local /usr; do
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
      PHP_CHECK_LIBRARY($LIB, dbm_open, [
        AC_DEFINE_UNQUOTED(NDBM_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
        AC_DEFINE(DBA_NDBM, 1, [ ]) 
        THIS_LIBS=$LIB
      ], [], [-L$THIS_PREFIX/$PHP_LIBDIR])
      if test -n "$THIS_LIBS"; then
        break
      fi
    done
  fi

  PHP_DBA_STD_ASSIGN
  PHP_DBA_STD_CHECK
  PHP_DBA_STD_ATTACH
fi
PHP_DBA_STD_RESULT(ndbm)

dnl TCADB
if test "$PHP_TCADB" != "no"; then
  PHP_DBA_STD_BEGIN
  for i in $PHP_TCADB /usr/local /usr; do
	if test -f "$i/include/tcadb.h"; then
	  THIS_PREFIX=$i
	  PHP_ADD_INCLUDE($THIS_PREFIX/include)
	  THIS_INCLUDE=$i/include/tcadb.h
	  break
	fi
  done

  if test -n "$THIS_INCLUDE"; then
	for LIB in tokyocabinet; do
	  PHP_CHECK_LIBRARY($LIB, tcadbopen, [
		AC_DEFINE_UNQUOTED(TCADB_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
		AC_DEFINE(DBA_TCADB, 1, [ ])
		THIS_LIBS=$LIB
	  ], [], [-L$THIS_PREFIX/$PHP_LIBDIR])
	  if test -n "$THIS_LIBS"; then
		break
	  fi
	done
  fi

  PHP_DBA_STD_ASSIGN
  PHP_DBA_STD_CHECK
  PHP_DBA_STD_ATTACH
fi
PHP_DBA_STD_RESULT(tcadb)

dnl Berkeley specific (library and version test)
dnl parameters(version, library list, function)
AC_DEFUN([PHP_DBA_DB_CHECK],[
  if test -z "$THIS_INCLUDE"; then
    AC_MSG_ERROR([DBA: Could not find necessary header file(s).])
  fi
  for LIB in $2; do
    if test -f $THIS_PREFIX/$PHP_LIBDIR/lib$LIB.a || test -f $THIS_PREFIX/$PHP_LIBDIR/lib$LIB.$SHLIB_SUFFIX_NAME; then
      lib_found="";
      PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/$PHP_LIBDIR, -l$LIB,[
        AC_TRY_LINK([
#include "$THIS_INCLUDE"
        ],[
          $3;
        ],[
          AC_EGREP_CPP(yes,[
#include "$THIS_INCLUDE"
#if DB_VERSION_MAJOR == $1 || ($1 == 4 && DB_VERSION_MAJOR == 5)
            yes
#endif
          ],[
            THIS_LIBS=$LIB
            lib_found=1
          ])
        ])
      ])
      if test -n "$lib_found"; then
        lib_found="";
        break;
      fi
    fi
  done
  if test -z "$THIS_LIBS"; then
    AC_MSG_CHECKING([for DB$1 major version])
    AC_MSG_ERROR([Header contains different version])
  fi
  if test "$1" = "4"; then
    AC_MSG_CHECKING([for DB4 minor version and patch level])
    AC_EGREP_CPP(yes,[
#include "$THIS_INCLUDE"
#if DB_VERSION_MAJOR > 4 || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR != 1) || (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR == 1 && DB_VERSION_PATCH >= 25)
      yes
#endif
    ],[
      AC_MSG_RESULT(ok)
    ],[
      AC_MSG_ERROR([Version 4.1 requires patch level 25])
    ])
  fi
  if test "$ext_shared" = "yes"; then
    AC_MSG_CHECKING([if dba can be used as shared extension])
    AC_EGREP_CPP(yes,[
#include "$THIS_INCLUDE"
#if DB_VERSION_MAJOR > 3 || (DB_VERSION_MAJOR == 3 && DB_VERSION_MINOR > 2)
      yes
#endif
    ],[
      AC_MSG_RESULT(yes)
    ],[
      AC_MSG_ERROR([At least version 3.3 is required])
    ])
  fi
  if test -n "$THIS_LIBS"; then
    AC_DEFINE(DBA_DB$1, 1, [ ]) 
    if test -n "$THIS_INCLUDE"; then
      AC_DEFINE_UNQUOTED(DB$1_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
    fi
  else
    AC_MSG_ERROR([DBA: Could not find necessary library.])
  fi
  THIS_RESULT=yes
  DB$1_LIBS=$THIS_LIBS
  DB$1_PREFIX=$THIS_PREFIX
  DB$1_INCLUDE=$THIS_INCLUDE
  PHP_DBA_STD_ASSIGN
  PHP_DBA_STD_ATTACH
])

# DB4
if test "$PHP_DB4" != "no"; then
  PHP_DBA_STD_BEGIN
  dbdp4="/usr/local/BerkeleyDB.4."
  dbdp5="/usr/local/BerkeleyDB.5."
  for i in $PHP_DB4 ${dbdp5}1 ${dbdp5}0 ${dbdp4}8 ${dbdp4}7 ${dbdp4}6 ${dbdp4}5 ${dbdp4}4 ${dbdp4}3 ${dbdp4}2 ${dbdp4}1 ${dbdp}0 /usr/local /usr; do
    if test -f "$i/db5/db.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/db5/db.h
      break
    elif test -f "$i/db4/db.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/db4/db.h
      break
    elif test -f "$i/include/db5.1/db.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/include/db5.1/db.h
      break
    elif test -f "$i/include/db5.0/db.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/include/db5.0/db.h
      break
    elif test -f "$i/include/db4.8/db.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/include/db4.8/db.h
      break
    elif test -f "$i/include/db4.7/db.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/include/db4.7/db.h
      break
    elif test -f "$i/include/db4.6/db.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/include/db4.6/db.h
      break
    elif test -f "$i/include/db4.5/db.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/include/db4.5/db.h
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
  PHP_DBA_DB_CHECK(4, db-5.1 db-5.0 db-4.8 db-4.7 db-4.6 db-4.5 db-4.4 db-4.3 db-4.2 db-4.1 db-4.0 db-4 db4 db, [(void)db_create((DB**)0, (DB_ENV*)0, 0)])
fi
PHP_DBA_STD_RESULT(db4,Berkeley DB4)

# DB3
if test "$PHP_DB3" != "no"; then
  PHP_DBA_STD_BEGIN
  if test "$HAVE_DB4" = "1"; then
    PHP_DBA_STD_RESULT(db3, Berkeley DB3, [You cannot combine --with-db3 with --with-db4])
  fi
  for i in $PHP_DB3 /usr/local/BerkeleyDB.3.3 /usr/local/BerkeleyDB.3.2 /usr/local/BerkeleyDB.3.1 /usr/local/BerkeleyDB.3.0 /usr/local /usr; do
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
PHP_DBA_STD_RESULT(db3,Berkeley DB3)

# DB2
if test "$PHP_DB2" != "no"; then
  PHP_DBA_STD_BEGIN
  if test "$HAVE_DB3" = "1" || test "$HAVE_DB4" = "1"; then
    PHP_DBA_STD_RESULT(db2, Berkeley DB2, [You cannot combine --with-db2 with --with-db3 or --with-db4])
  fi
  for i in $PHP_DB2 $PHP_DB2/BerkeleyDB /usr/BerkeleyDB /usr/local /usr; do
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
  PHP_DBA_DB_CHECK(2, db-2 db2 db,  [(void)db_appinit("", NULL, (DB_ENV*)0, 0)])
fi
PHP_DBA_STD_RESULT(db2, Berkeley DB2)

# DB1
if test "$PHP_DB1" != "no"; then
  PHP_DBA_STD_BEGIN
  AC_MSG_CHECKING([for DB1 in library])
  if test "$HAVE_DB4" = "1"; then
    THIS_VERSION=4
    THIS_LIBS=$DB4_LIBS
    THIS_PREFIX=$DB4_PREFIX
  elif test "$HAVE_DB3" = "1"; then
    THIS_LIBS=$DB3_LIBS
    THIS_PREFIX=$DB3_PREFIX
  elif test "$HAVE_DB2" = "1"; then
    THIS_VERSION=2
    THIS_LIBS=$DB2_LIBS
    THIS_PREFIX=$DB2_PREFIX
  fi
  if test "$HAVE_DB4" = "1" || test "$HAVE_DB3" = "1" || test "$HAVE_DB2" = "1"; then
    AC_DEFINE_UNQUOTED(DB1_VERSION, "Berkeley DB 1.85 emulation in DB$THIS_VERSION", [ ])
    for i in db$THIS_VERSION/db_185.h include/db$THIS_VERSION/db_185.h include/db/db_185.h; do
      if test -f "$THIS_PREFIX/$i"; then
        THIS_INCLUDE=$THIS_PREFIX/$i
        break
      fi
    done
  else
    AC_DEFINE_UNQUOTED(DB1_VERSION, "Unknown DB1", [ ])
    for i in $PHP_DB1 /usr/local /usr; do
      if test -f "$i/db1/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/db1/db.h
        break
      elif test -f "$i/include/db1/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db1/db.h
        break
      elif test -f "$i/include/db.h"; then
        THIS_PREFIX=$i
        THIS_INCLUDE=$i/include/db.h
        break
      fi
    done
    THIS_LIBS=db
  fi
  AC_MSG_RESULT([$THIS_LIBS])
  AC_MSG_CHECKING([for DB1 in header])
  AC_MSG_RESULT([$THIS_INCLUDE])
  if test -n "$THIS_INCLUDE"; then
    PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/$PHP_LIBDIR, -l$THIS_LIBS,[
      AC_TRY_LINK([
#include "$THIS_INCLUDE"
      ],[
        DB * dbp = dbopen("", 0, 0, DB_HASH, 0);
      ],[
        AC_DEFINE_UNQUOTED(DB1_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
        AC_DEFINE(DBA_DB1, 1, [ ])
        THIS_RESULT=yes
      ],[
        THIS_RESULT=no
      ])
    ])
  fi
  PHP_DBA_STD_ASSIGN
  PHP_DBA_STD_CHECK
  PHP_DBA_STD_ATTACH
fi
PHP_DBA_STD_RESULT(db1, DB1)

# DBM
if test "$PHP_DBM" != "no"; then
  PHP_DBA_STD_BEGIN
  if test "$HAVE_QDBM" = "1"; then
    PHP_DBA_STD_RESULT(dbm, dbm, [You cannot combine --with-dbm with --with-qdbm])
  fi
  for i in $PHP_DBM /usr/local /usr; do
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
      PHP_CHECK_LIBRARY($LIB, dbminit, [
        AC_MSG_CHECKING(for DBM using GDBM)
        AC_DEFINE_UNQUOTED(DBM_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
        if test "$LIB" = "gdbm"; then
          AC_DEFINE_UNQUOTED(DBM_VERSION, "GDBM", [ ])
          AC_MSG_RESULT(yes)
        else
          AC_DEFINE_UNQUOTED(DBM_VERSION, "DBM", [ ])
          AC_MSG_RESULT(no)
        fi
        AC_DEFINE(DBA_DBM, 1, [ ]) 
        THIS_LIBS=$LIB
      ], [], [-L$THIS_PREFIX/$PHP_LIBDIR])
      if test -n "$THIS_LIBS"; then
        break
      fi
    done
  fi
  
  PHP_DBA_STD_ASSIGN
  PHP_DBA_STD_CHECK
  PHP_DBA_STD_ATTACH
fi
PHP_DBA_STD_RESULT(dbm)

dnl
dnl Bundled modules that should be enabled by default if any other option is enabled
dnl
if test "$PHP_DBA" != "no" || test "$HAVE_DBA" = "1" || test "$with_cdb" = "yes" || test "$enable_inifile" = "yes" || test "$enable_flatfile" = "yes"; then
  php_dba_enable=yes
else
  php_dba_enable=no
fi

PHP_ARG_WITH(cdb,,
[  --without-cdb[=DIR]       DBA: CDB support (bundled)], $php_dba_enable, no)

PHP_ARG_ENABLE(inifile,,
[  --disable-inifile       DBA: INI support (bundled)], $php_dba_enable, no)

PHP_ARG_ENABLE(flatfile,,
[  --disable-flatfile      DBA: FlatFile support (bundled)], $php_dba_enable, no)

# CDB
if test "$PHP_CDB" = "yes"; then
  AC_DEFINE(DBA_CDB_BUILTIN, 1, [ ])
  AC_DEFINE(DBA_CDB_MAKE, 1, [ ])
  AC_DEFINE(DBA_CDB, 1, [ ])
  cdb_sources="libcdb/cdb.c libcdb/cdb_make.c libcdb/uint32.c"
  THIS_RESULT="builtin"
elif test "$PHP_CDB" != "no"; then
  PHP_DBA_STD_BEGIN
  for i in $PHP_CDB /usr/local /usr; do
    if test -f "$i/include/cdb.h"; then
      THIS_PREFIX=$i
      THIS_INCLUDE=$i/include/cdb.h
      break
    fi
  done

  if test -n "$THIS_INCLUDE"; then
    for LIB in cdb c; do
      PHP_CHECK_LIBRARY($LIB, cdb_read, [
        AC_DEFINE_UNQUOTED(CDB_INCLUDE_FILE, "$THIS_INCLUDE", [ ])
        AC_DEFINE(DBA_CDB, 1, [ ]) 
        THIS_LIBS=$LIB
      ], [], [-L$THIS_PREFIX/$PHP_LIBDIR])
      if test -n "$THIS_LIBS"; then
        break
      fi
    done
  fi

  PHP_DBA_STD_ASSIGN
  PHP_DBA_STD_CHECK
  PHP_DBA_STD_ATTACH
fi
PHP_DBA_STD_RESULT(cdb)

# INIFILE
if test "$PHP_INIFILE" != "no"; then
  AC_DEFINE(DBA_INIFILE, 1, [ ])
  ini_sources="libinifile/inifile.c"
  THIS_RESULT="builtin"
fi
PHP_DBA_STD_RESULT(inifile, [INI File])

# FLATFILE
if test "$PHP_FLATFILE" != "no"; then
  AC_DEFINE(DBA_FLATFILE, 1, [ ])
  flat_sources="libflatfile/flatfile.c"
  THIS_RESULT="builtin"
fi
PHP_DBA_STD_RESULT(FlatFile, FlatFile)

dnl
dnl Extension setup
dnl 
AC_MSG_CHECKING([whether to enable DBA interface])
if test "$HAVE_DBA" = "1"; then
  if test "$ext_shared" = "yes"; then
    AC_MSG_RESULT([yes, shared])
  else
    AC_MSG_RESULT([yes])
  fi
  AC_DEFINE(HAVE_DBA, 1, [ ])
  PHP_NEW_EXTENSION(dba, dba.c dba_cdb.c dba_dbm.c dba_gdbm.c dba_ndbm.c dba_db1.c dba_db2.c dba_db3.c dba_db4.c dba_flatfile.c dba_inifile.c dba_qdbm.c dba_tcadb.c $cdb_sources $flat_sources $ini_sources, $ext_shared)
  PHP_ADD_BUILD_DIR($ext_builddir/libinifile)
  PHP_ADD_BUILD_DIR($ext_builddir/libcdb)
  PHP_ADD_BUILD_DIR($ext_builddir/libflatfile)
  PHP_SUBST(DBA_SHARED_LIBADD)
else
  AC_MSG_RESULT(no)
fi
