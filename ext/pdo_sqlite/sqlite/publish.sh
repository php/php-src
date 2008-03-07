#!/bin/sh
#
# This script is used to compile SQLite and package everything up
# so that it is ready to move to the SQLite website.
#

# Set srcdir to the name of the directory that contains the publish.sh
# script.
#
srcdir=`echo "$0" | sed 's%\(^.*\)/[^/][^/]*$%\1%'`

# Get the makefile.
#
cp $srcdir/Makefile.linux-gcc ./Makefile
chmod +x $srcdir/install-sh

# Get the current version number - needed to help build filenames
#
VERS=`cat $srcdir/VERSION`
VERSW=`sed 's/\./_/g' $srcdir/VERSION`
echo "VERSIONS: $VERS $VERSW"

# Start by building an sqlite shell for linux.
#
make clean
make sqlite3.c
CFLAGS="-Os -DSQLITE_ENABLE_FTS3=1 -DSQLITE_THREADSAFE=0"
echo '***** '"COMPILING sqlite3-$VERS.bin..."
gcc $CFLAGS -Itsrc sqlite3.c tsrc/shell.c -o sqlite3 -ldl
strip sqlite3
mv sqlite3 sqlite3-$VERS.bin
gzip sqlite3-$VERS.bin
chmod 644 sqlite3-$VERS.bin.gz
mv sqlite3-$VERS.bin.gz doc

# Build a source archive useful for windows.
#
make target_source
cd tsrc
echo '***** BUILDING preprocessed source archives'
rm fts[12]* icu*
rm -f ../doc/sqlite-source-$VERSW.zip
zip ../doc/sqlite-source-$VERSW.zip *
cd ..
cp tsrc/sqlite3.h tsrc/sqlite3ext.h .
pwd
zip doc/sqlite-amalgamation-$VERSW.zip sqlite3.c sqlite3.h sqlite3ext.h

# Build the sqlite.so and tclsqlite.so shared libraries
# under Linux
#
TCLDIR=/home/drh/tcltk/846/linux/846linux
TCLSTUBLIB=$TCLDIR/libtclstub8.4g.a
CFLAGS="-Os -DSQLITE_ENABLE_FTS3=1 -DHAVE_LOCALTIME_R=1 -DHAVE_GMTIME_R=1"
echo '***** BUILDING shared libraries for linux'
gcc $CFLAGS -shared tclsqlite3.c $TCLSTUBLIB -o tclsqlite3.so -lpthread
strip tclsqlite3.so
chmod 644 tclsqlite3.so
mv tclsqlite3.so tclsqlite-$VERS.so
gzip tclsqlite-$VERS.so
mv tclsqlite-$VERS.so.gz doc
gcc $CFLAGS -shared sqlite3.c -o sqlite3.so -lpthread
strip sqlite3.so
chmod 644 sqlite3.so
mv sqlite3.so sqlite-$VERS.so
gzip sqlite-$VERS.so
mv sqlite-$VERS.so.gz doc


# Build the tclsqlite3.dll and sqlite3.dll shared libraries.
#
. $srcdir/mkdll.sh
echo '***** PACKAGING shared libraries for windows'
echo zip doc/tclsqlite-$VERSW.zip tclsqlite3.dll
zip doc/tclsqlite-$VERSW.zip tclsqlite3.dll
echo zip doc/sqlitedll-$VERSW.zip sqlite3.dll sqlite3.def
zip doc/sqlitedll-$VERSW.zip sqlite3.dll sqlite3.def

# Build the sqlite.exe executable for windows.
#
OPTS='-DSTATIC_BUILD=1 -DNDEBUG=1 -DSQLITE_THREADSAFE=0'
OPTS="$OPTS -DSQLITE_ENABLE_FTS3=1"
i386-mingw32msvc-gcc -Os $OPTS -Itsrc -I$TCLDIR sqlite3.c tsrc/shell.c \
      -o sqlite3.exe
zip doc/sqlite-$VERSW.zip sqlite3.exe

# Construct a tarball of the source tree
#
echo '***** BUILDING source archive'
ORIGIN=`pwd`
cd $srcdir
cd ..
mv sqlite sqlite-$VERS
EXCLUDE=`find sqlite-$VERS -print | egrep (CVS|www/|art/|doc/|contrib/) | sed 's,^, --exclude ,'`
tar czf $ORIGIN/doc/sqlite-$VERS.tar.gz $EXCLUDE sqlite-$VERS
mv sqlite-$VERS sqlite
cd $ORIGIN

#
# Build RPMS (binary) and Source RPM
#

# Make sure we are properly setup to build RPMs
#
echo "%HOME %{expand:%%(cd; pwd)}" > $HOME/.rpmmacros
echo "%_topdir %{HOME}/rpm" >> $HOME/.rpmmacros
mkdir $HOME/rpm
mkdir $HOME/rpm/BUILD
mkdir $HOME/rpm/SOURCES
mkdir $HOME/rpm/RPMS
mkdir $HOME/rpm/SRPMS
mkdir $HOME/rpm/SPECS

# create the spec file from the template
sed s/SQLITE_VERSION/$VERS/g $srcdir/spec.template > $HOME/rpm/SPECS/sqlite.spec

# copy the source tarball to the rpm directory
cp doc/sqlite-$VERS.tar.gz $HOME/rpm/SOURCES/.

# build all the rpms
rpm -ba $HOME/rpm/SPECS/sqlite.spec >& rpm-$vers.log

# copy the RPMs into the build directory.
mv $HOME/rpm/RPMS/i386/sqlite*-$vers*.rpm doc
mv $HOME/rpm/SRPMS/sqlite-$vers*.rpm doc

# Build the website
#
#cp $srcdir/../historical/* doc
#make doc
#cd doc
#chmod 644 *.gz
