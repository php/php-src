dnl
dnl $Id$
dnl

AC_DEFUN(PHP_TEMP_LDFLAGS,[
  old_LDFLAGS=$LDFLAGS
  LDFLAGS="$1 $LDFLAGS"
  $2
  LDFLAGS=$old_LDFLAGS
])


dnl Assign INCLUDE/LFLAGS from PREFIX
AC_DEFUN(PHP_DBA_STD_ASSIGN,[
  if test -n "$THIS_PREFIX" && test "$THIS_PREFIX" != "/usr"; then
    THIS_INCLUDE=$THIS_PREFIX/include
    THIS_LFLAGS=$THIS_PREFIX/lib
  fi
])

dnl Standard check
AC_DEFUN(PHP_DBA_STD_CHECK,[
  THIS_RESULT="yes"
  if test "$THIS_PREFIX" != "/usr" -a "$THIS_INCLUDE" = ""; then
    AC_MSG_ERROR(cannot find necessary header file(s))
  fi
  if test "$THIS_LIBS" = "" ; then
    AC_MSG_ERROR(cannot find necessary library)
  fi
])

dnl Attach THIS_x to DBA_x
AC_DEFUN(PHP_DBA_STD_ATTACH,[
  PHP_ADD_INCLUDE($THIS_INCLUDE)
  PHP_ADD_LIBRARY_WITH_PATH($THIS_LIBS, $THIS_LFLAGS, DBA_SHARED_LIBADD)
  unset THIS_INCLUDE THIS_LIBS THIS_LFLAGS THIS_PREFIX
])

dnl Print the result message
AC_DEFUN(AC_DBA_STD_RESULT,[
  if test "$THIS_RESULT" = "yes"; then
    HAVE_DBA=1
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
  unset THIS_RESULT
])

PHP_ARG_ENABLE(dba,whether to enable DBA,
[  --enable-dba=shared     Build DBA as a shared module])

AC_ARG_WITH(gdbm,
[  --with-gdbm[=DIR]       Include GDBM support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f "$i/include/gdbm.h"; then
        THIS_PREFIX="$i"
      fi
    done

    unset ac_cv_lib_gdbm_gdbm_open
    PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
    AC_CHECK_LIB(gdbm, gdbm_open, [AC_DEFINE(DBA_GDBM, 1, [ ]) THIS_LIBS=gdbm])
    ])
    
    PHP_DBA_STD_ASSIGN
    PHP_DBA_STD_CHECK
    PHP_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for GDBM support)
AC_DBA_STD_RESULT

AC_ARG_WITH(ndbm,
[  --with-ndbm[=DIR]       Include NDBM support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f "$i/include/db1/ndbm.h" ; then
        THIS_PREFIX=$i
        NDBM_EXTRA=db1/ndbm.h
      elif test -f "$i/include/ndbm.h" ; then
        THIS_PREFIX=$i
        NDBM_EXTRA=ndbm.h
      fi
	done
    
    if test "$NDBM_EXTRA" != ""; then
      AC_DEFINE_UNQUOTED(NDBM_INCLUDE_FILE, "$NDBM_EXTRA", [ ])
    fi

    for LIB in db1 ndbm c; do
      PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
      AC_CHECK_LIB($LIB, dbm_open, [AC_DEFINE(DBA_NDBM,1, [ ]) THIS_LIBS=$LIB])
      ])
    done
    
    PHP_DBA_STD_ASSIGN
    PHP_DBA_STD_CHECK
    PHP_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for NDBM support)
AC_DBA_STD_RESULT

AC_ARG_WITH(db2,
[  --with-db2[=DIR]        Include Berkeley DB2 support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr /usr/BerkeleyDB $withval/BerkeleyDB $withval; do
      if test -f "$i/db2/db.h"; then
        THIS_PREFIX=$i
        DB2_EXTRA=db2
      elif test -f "$i/include/db2/db.h"; then
        THIS_PREFIX=$i
        DB2_EXTRA=db2/db.h
      elif test -f "$i/include/db/db2.h"; then
        THIS_PREFIX=$i
        DB2_EXTRA=db/db2.h
      elif test -f "$i/include/db2.h"; then
        THIS_PREFIX=$i
        DB2_EXTRA=db2.h
      elif test -f "$i/include/db.h" ; then
        THIS_PREFIX=$i
        DB2_EXTRA=db.h
      fi
	done

    if test "$DB2_EXTRA" = "db2" ; then
      DBA_INCLUDE="$DBA_INCLUDE -I$THIS_PREFIX/db2"
      DB2_EXTRA=db.h
    fi
    
    if test -n "$DB2_EXTRA"; then
      AC_DEFINE_UNQUOTED(DB2_INCLUDE_FILE, "$DB2_EXTRA", [ ])
    fi

    for LIB in db db2 c; do
      PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
      AC_CHECK_LIB($LIB, db_appinit, [AC_DEFINE(DBA_DB2,1,[ ]) THIS_LIBS=$LIB])
      ])
    done
    
    PHP_DBA_STD_ASSIGN
    PHP_DBA_STD_CHECK
    PHP_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for Berkeley DB2 support)
AC_DBA_STD_RESULT

AC_ARG_WITH(db3,
[  --with-db3[=DIR]        Include Berkeley DB3 support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr /usr/local/BerkeleyDB.3.0 $withval; do
      if test -f "$i/db3/db.h"; then
        THIS_PREFIX=$i
        DB3_EXTRA=db3
      elif test -f "$i/include/db3/db.h"; then
        THIS_PREFIX=$i
        DB3_EXTRA=db3/db.h
      elif test -f "$i/include/db/db3.h"; then
        THIS_PREFIX=$i
        DB3_EXTRA=db/db3.h
      elif test -f "$i/include/db3.h"; then
        THIS_PREFIX=$i
        DB3_EXTRA=db3.h
      elif test -f "$i/include/db.h"; then
        THIS_PREFIX=$i
        DB3_EXTRA=db.h
      fi
    done

    if test -n "$DB3_EXTRA"; then
      AC_DEFINE_UNQUOTED(DB3_INCLUDE_FILE, "$DB3_EXTRA", [ ])
    fi

    for LIB in db-3 db3 db; do
      PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
      AC_CHECK_LIB($LIB, db_create, [AC_DEFINE(DBA_DB3,1,[ ]) THIS_LIBS=$LIB])
      ])
    done
    
    PHP_DBA_STD_ASSIGN
    PHP_DBA_STD_CHECK
    PHP_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for Berkeley DB3 support)
AC_DBA_STD_RESULT

AC_ARG_WITH(dbm,
[  --with-dbm[=DIR]        Include DBM support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f "$i/include/dbm.h" ; then
        THIS_PREFIX=$i
      fi
	done

    for LIB in db1 dbm c; do
      PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
      AC_CHECK_LIB($LIB, dbminit, [AC_DEFINE(DBA_DBM,1,[ ]) THIS_LIBS=$LIB])
      ])
    done
    
    PHP_DBA_STD_ASSIGN
    PHP_DBA_STD_CHECK
    PHP_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for DBM support)
AC_DBA_STD_RESULT

AC_ARG_WITH(cdb,
[  --with-cdb[=DIR]        Include CDB support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f "$i/include/cdb.h" ; then
        THIS_PREFIX=$i
      fi
	done

    for LIB in cdb c; do
      PHP_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
      AC_CHECK_LIB($LIB, cdb_read, [AC_DEFINE(DBA_CDB,1,[ ]) THIS_LIBS=$LIB])
      ])
    done
    
    PHP_DBA_STD_ASSIGN
    PHP_DBA_STD_CHECK
    PHP_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for CDB support)
AC_DBA_STD_RESULT

AC_MSG_CHECKING(whether to enable DBA interface)
if test "$HAVE_DBA" = "1"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_DBA, 1, [ ])
  PHP_NEW_EXTENSION(dba, dba.c dba_cdb.c dba_db2.c dba_dbm.c dba_gdbm.c 		dba_ndbm.c dba_db3.c, $ext_shared)
  PHP_SUBST(DBA_SHARED_LIBADD)
else
  AC_MSG_RESULT(no)
fi

