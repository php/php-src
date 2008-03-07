#!/bin/sh
#
# This script is used to compile SQLite into a DLL.
#
# Two separate DLLs are generated.  "sqlite3.dll" is the core
# library.  "tclsqlite3.dll" contains the TCL bindings and is the
# library that is loaded into TCL in order to run SQLite.
#
make sqlite3.c
PATH=$PATH:/opt/mingw/bin
TCLDIR=/home/drh/tcltk/846/win/846win
TCLSTUBLIB=$TCLDIR/libtcl84stub.a
OPTS='-DUSE_TCL_STUBS=1 -DSQLITE_THREADSAFE=1 -DBUILD_sqlite=1 -DOS_WIN=1'
OPTS="$OPTS -DSQLITE_ENABLE_FTS3=1"
CC="i386-mingw32msvc-gcc -Os $OPTS -Itsrc -I$TCLDIR"
NM="i386-mingw32msvc-nm"
CMD="$CC -c sqlite3.c"
echo $CMD
$CMD
CMD="$CC -c tclsqlite3.c"
echo $CMD
$CMD
echo 'EXPORTS' >tclsqlite3.def
$NM tclsqlite3.o | grep ' T ' >temp1
grep '_Init$' temp1 >temp2
grep '_SafeInit$' temp1 >>temp2
grep ' T _sqlite3_' temp1 >>temp2
echo 'EXPORTS' >tclsqlite3.def
sed 's/^.* T _//' temp2 | sort | uniq >>tclsqlite3.def
i386-mingw32msvc-dllwrap \
     --def tclsqlite3.def -v --export-all \
     --driver-name i386-mingw32msvc-gcc \
     --dlltool-name i386-mingw32msvc-dlltool \
     --as i386-mingw32msvc-as \
     --target i386-mingw32 \
     -dllname tclsqlite3.dll -lmsvcrt tclsqlite3.o $TCLSTUBLIB
$NM sqlite3.o | grep ' T ' >temp1
echo 'EXPORTS' >sqlite3.def
grep ' _sqlite3_' temp1 | sed 's/^.* _//' >>sqlite3.def
i386-mingw32msvc-dllwrap \
     --def sqlite3.def -v --export-all \
     --driver-name i386-mingw32msvc-gcc \
     --dlltool-name i386-mingw32msvc-dlltool \
     --as i386-mingw32msvc-as \
     --target i386-mingw32 \
     -dllname sqlite3.dll -lmsvcrt sqlite3.o
