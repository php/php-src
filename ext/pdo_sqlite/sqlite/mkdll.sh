#!/bin/sh
#
# This script is used to compile SQLite into a DLL.
#
# Two separate DLLs are generated.  "sqlite3.dll" is the core
# library.  "tclsqlite3.dll" contains the TCL bindings and is the
# library that is loaded into TCL in order to run SQLite.
#
make target_source
cd tsrc
PATH=$PATH:/opt/mingw/bin
TCLDIR=/home/drh/tcltk/846/win/846win
TCLSTUBLIB=$TCLDIR/libtcl84stub.a
OPTS='-DUSE_TCL_STUBS=1 -DNDEBUG=1 -DTHREADSAFE=1'
CC="i386-mingw32msvc-gcc -O2 $OPTS -I. -I$TCLDIR"
rm shell.c
for i in *.c; do
  CMD="$CC -c $i"
  echo $CMD
  $CMD
done
echo 'EXPORTS' >tclsqlite3.def
echo 'Tclsqlite3_Init' >>tclsqlite3.def
echo 'Sqlite3_Init' >>tclsqlite3.def
i386-mingw32msvc-dllwrap \
     --def tclsqlite3.def -v --export-all \
     --driver-name i386-mingw32msvc-gcc \
     --dlltool-name i386-mingw32msvc-dlltool \
     --as i386-mingw32msvc-as \
     --target i386-mingw32 \
     -dllname tclsqlite3.dll -lmsvcrt *.o $TCLSTUBLIB
i386-mingw32msvc-strip tclsqlite3.dll
rm tclsqlite.o
i386-mingw32msvc-dllwrap \
     --def sqlite3.def -v --export-all \
     --driver-name i386-mingw32msvc-gcc \
     --dlltool-name i386-mingw32msvc-dlltool \
     --as i386-mingw32msvc-as \
     --target i386-mingw32 \
     -dllname sqlite3.dll -lmsvcrt *.o
i386-mingw32msvc-strip sqlite3.dll
cd ..
