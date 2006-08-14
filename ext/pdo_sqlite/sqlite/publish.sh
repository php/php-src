#!/bin/sh
#
# This script is used to compile SQLite and all its documentation and
# ship everything up to the SQLite website.  This script will only work
# on the system "zadok" at the Hwaci offices.  But others might find
# the script useful as an example.
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

# Start by building an sqlite shell for linux.
#
make clean
make sqlite3
strip sqlite3
mv sqlite3 sqlite3-$VERS.bin
gzip sqlite3-$VERS.bin
mv sqlite3-$VERS.bin.gz doc

# Build a source archive useful for windows.
#
make target_source
cd tsrc
zip ../doc/sqlite-source-$VERSW.zip *
cd ..

# Build the sqlite.so and tclsqlite.so shared libraries
# under Linux
#
. $srcdir/mkso.sh
cd tsrc
mv tclsqlite3.so tclsqlite-$VERS.so
gzip tclsqlite-$VERS.so
mv tclsqlite-$VERS.so.gz ../doc
mv sqlite3.so sqlite-$VERS.so
gzip sqlite-$VERS.so
mv sqlite-$VERS.so.gz ../doc
cd ..

# Build the tclsqlite3.dll and sqlite3.dll shared libraries.
#
. $srcdir/mkdll.sh
cd tsrc
echo zip ../doc/tclsqlite-$VERSW.zip tclsqlite3.dll
zip ../doc/tclsqlite-$VERSW.zip tclsqlite3.dll
echo zip ../doc/sqlitedll-$VERSW.zip sqlite3.dll sqlite3.def
zip ../doc/sqlitedll-$VERSW.zip sqlite3.dll sqlite3.def
cd ..

# Build the sqlite.exe executable for windows.
#
make target_source
cd tsrc
rm tclsqlite.c
OPTS='-DSTATIC_BUILD=1 -DNDEBUG=1'
i386-mingw32msvc-gcc -O2 $OPTS -I. -I$TCLDIR *.c -o sqlite3.exe
zip ../doc/sqlite-$VERSW.zip sqlite3.exe
cd ..

# Construct a tarball of the source tree
#
ORIGIN=`pwd`
cd $srcdir
cd ..
mv sqlite sqlite-$VERS
EXCLUDE=`find sqlite-$VERS -print | grep CVS | sed 's,^, --exclude ,'`
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
make doc
cd doc
chmod 644 *.gz
