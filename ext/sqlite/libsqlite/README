This directory contains source code to 

    SQLite: An Embeddable SQL Database Engine

To compile the project, first create a directory in which to place
the build products.  It is recommended, but not required, that the
build directory be separate from the source directory.  Cd into the
build directory and then from the build directory run the configure
script found at the root of the source tree.  Then run "make".

For example:

    tar xzf sqlite.tar.gz    ;#  Unpack the source tree into "sqlite"
    mkdir bld                ;#  Build will occur in a sibling directory
    cd bld                   ;#  Change to the build directory
    ../sqlite/configure      ;#  Run the configure script
    make                     ;#  Run the makefile.

The configure script uses autoconf 2.50 and libtool.  If the configure
script does not work out for you, there is a generic makefile named
"Makefile.linux-gcc" in the top directory of the source tree that you
can copy and edit to suite your needs.  Comments on the generic makefile
show what changes are needed.

The linux binaries on the website are created using the generic makefile,
not the configure script.  The configure script is unmaintained.  (You
can volunteer to take over maintenance of the configure script, if you want!)
The windows binaries on the website are created using MinGW32 configured
as a cross-compiler running under Linux.  For details, see the ./publish.sh
script at the top-level of the source tree.

Contacts:

   http://www.sqlite.org/
   http://www.hwaci.com/sw/sqlite/
   http://groups.yahoo.com/group/sqlite/
   drh@hwaci.com
