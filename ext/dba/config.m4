dnl $Id$
dnl config.m4 for extension dba
dnl don't forget to call PHP_EXTENSION(dba)

AC_DEFUN(AC_TEMP_LDFLAGS,[
  old_LDFLAGS="$LDFLAGS"
  LDFLAGS="$1 $LDFLAGS"
  $2
  LDFLAGS="$old_LDFLAGS"
])


dnl Assign INCLUDE/LFLAGS from PREFIX
AC_DEFUN(AC_DBA_STD_ASSIGN,[
  if test "$THIS_PREFIX" != "" -a "$THIS_PREFIX" != "/usr"; then
    THIS_INCLUDE="$THIS_PREFIX/include"
    THIS_LFLAGS="$THIS_PREFIX/lib"
  fi
])

dnl Standard check
AC_DEFUN(AC_DBA_STD_CHECK,[
  THIS_RESULT="yes"
  if test "$THIS_PREFIX" != "/usr"; then
  if test "$THIS_INCLUDE" = "" ; then
    AC_MSG_ERROR(cannot find necessary header file(s))
  elif test "$THIS_LIBS" = "" ; then
    AC_MSG_ERROR(cannot find necessary library)
  fi
  fi
])

dnl Attach THIS_x to DBA_x
AC_DEFUN(AC_DBA_STD_ATTACH,[
  AC_ADD_INCLUDE($THIS_INCLUDE)
  AC_ADD_LIBRARY_WITH_PATH($THIS_LIBS, $THIS_LFLAGS)

  THIS_INCLUDE=""
  THIS_LIBS=""
  THIS_LFLAGS=""
  THIS_PREFIX=""
])

dnl Print the result message
AC_DEFUN(AC_DBA_STD_RESULT,[
  if test "$THIS_RESULT" = "yes"; then
    HAVE_DBA=1
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
  THIS_RESULT=""
])



AC_ARG_WITH(gdbm,
[  --with-gdbm[=DIR]       Include GDBM support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f "$i/include/gdbm.h"; then
        THIS_PREFIX="$i"
      fi
    done

    unset ac_cv_lib_gdbm_gdbm_open
    AC_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
    AC_CHECK_LIB(gdbm, gdbm_open, [AC_DEFINE(DBA_GDBM, 1, [ ]) THIS_LIBS="gdbm"])
    ])
    
    AC_DBA_STD_ASSIGN
    AC_DBA_STD_CHECK
    AC_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for GDBM support)
AC_DBA_STD_RESULT

AC_ARG_WITH(ndbm,
[  --with-ndbm[=DIR]       Include NDBM support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f "$i/include/db1/ndbm.h" ; then
        THIS_PREFIX="$i"
        NDBM_EXTRA="NDBM_DB1_NDBM_H"
      elif test -f "$i/include/ndbm.h" ; then
        THIS_PREFIX="$i"
        NDBM_EXTRA="NDBM_NDBM_H"
      fi
	done
    
    if test "$NDBM_EXTRA" != ""; then
      eval "AC_DEFINE($NDBM_EXTRA, 1)"
    fi

    for LIB in db1 ndbm c; do
      AC_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
      AC_CHECK_LIB($LIB, dbm_open, [AC_DEFINE(DBA_NDBM,1, [ ]) THIS_LIBS="$LIB"])
      ])
    done
    
    AC_DBA_STD_ASSIGN
    AC_DBA_STD_CHECK
    AC_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for NDBM support)
AC_DBA_STD_RESULT

AC_ARG_WITH(db2,
[  --with-db2[=DIR]        Include Berkeley DB2 support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr /usr/BerkeleyDB $withval; do
      if test -f "$i/db2/db.h"; then
        THIS_PREFIX="$i"
        DB2_EXTRA="db2"
      elif test -f "$i/include/db2/db.h"; then
        THIS_PREFIX="$i"
        DB2_EXTRA="DB2_DB2_DB_H"
      elif test -f "$i/include/db/db2.h"; then
        THIS_PREFIX="$i"
        DB2_EXTRA="DB2_DB_DB2_H"
      elif test -f "$i/include/db2.h"; then
        THIS_PREFIX="$i"
        DB2_EXTRA="DB2_DB2_H"
      elif test -f "$i/include/db.h" ; then
        THIS_PREFIX="$i"
        DB2_EXTRA="DB2_DB_H"
      fi
	done

    if test "$DB2_EXTRA" = "db2" ; then
      DBA_INCLUDE="$DBA_INCLUDE -I$THIS_PREFIX/db2"
      DB2_EXTRA="DB2_DB_H"
    fi
    
    if test -n "$DB2_EXTRA"; then
      eval "AC_DEFINE($DB2_EXTRA, 1)"
    fi

    for LIB in db db2 c; do
      AC_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
      AC_CHECK_LIB($LIB, db_appinit, [AC_DEFINE(DBA_DB2,1,[ ]) THIS_LIBS="$LIB"])
      ])
    done
    
    AC_DBA_STD_ASSIGN
    AC_DBA_STD_CHECK
    AC_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for Berkeley DB2 support)
AC_DBA_STD_RESULT

AC_ARG_WITH(db3,
[  --with-db3[=DIR]        Include Berkeley DB3 support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f "$i/include/db.h" ; then
        THIS_PREFIX="$i"
        DB3_EXTRA="DB3_DB_H"
      fi
	done

    if test -n "$DB3_EXTRA"; then
      eval "AC_DEFINE($DB3_EXTRA, 1)"
    fi

    for LIB in db; do
      AC_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
      AC_CHECK_LIB($LIB, db_create, [AC_DEFINE(DBA_DB3,1,[ ]) THIS_LIBS="$LIB"])
      ])
    done
    
    AC_DBA_STD_ASSIGN
    AC_DBA_STD_CHECK
    AC_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for Berkeley DB3 support)
AC_DBA_STD_RESULT

AC_ARG_WITH(dbm,
[  --with-dbm[=DIR]        Include DBM support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f "$i/include/dbm.h" ; then
        THIS_PREFIX="$i"
      fi
	done

    for LIB in db1 dbm c; do
      AC_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
      AC_CHECK_LIB($LIB, dbminit, [AC_DEFINE(DBA_DBM,1,[ ]) THIS_LIBS="$LIB"])
      ])
    done
    
    AC_DBA_STD_ASSIGN
    AC_DBA_STD_CHECK
    AC_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for DBM support)
AC_DBA_STD_RESULT

AC_ARG_WITH(cdb,
[  --with-cdb[=DIR]        Include CDB support],[
  if test "$withval" != "no"; then
    for i in /usr/local /usr $withval; do
      if test -f "$i/include/cdb.h" ; then
        THIS_PREFIX="$i"
      fi
	done

    for LIB in cdb c; do
      AC_TEMP_LDFLAGS(-L$THIS_PREFIX/lib,[
      AC_CHECK_LIB($LIB, cdb_bread, [AC_DEFINE(DBA_CDB,1,[ ]) THIS_LIBS="$LIB"])
      ])
    done
    
    AC_DBA_STD_ASSIGN
    AC_DBA_STD_CHECK
    AC_DBA_STD_ATTACH
  fi
])
AC_MSG_CHECKING(for CDB support)
AC_DBA_STD_RESULT

AC_MSG_CHECKING(whether to enable DBA interface)
if test "$HAVE_DBA" = "1"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_DBA, 1, [ ])
  PHP_EXTENSION(dba)
else
  AC_MSG_RESULT(no)
  AC_DEFINE(HAVE_DBA, 0, [ ])
fi

