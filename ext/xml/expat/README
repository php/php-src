
			Expat, Release 1.95.2

This is expat, a C library for parsing XML, written by James Clark.
Expat is a stream-oriented XML parser.  This means that you register
handlers with the parser before starting the parse.  These handlers
are called when the parser discovers the associated structures in the
document being parsed.  A start tag is an example of the kind of
structures for which you may register handlers.

Windows users should use the expat_win32bin package, which includes
both precompiled libraries and executalbes, and source code for
developers.

Expat is free software.  You may copy, distribute, and modify it under
the terms of the License contained in the file COPYING distributed
with this package.  This license is the same as the MIT/X Consortium
license.

Versions of expat that have an odd minor version (the middle number in
the release above), are development releases and should be considered
as beta software.  Releases with even minor version numbers are
intended to be production grade software.

To build expat, you first run the configuration shell script in the
top level distribution directory:

	./configure

There are many options which you may provide to configure (which you
can discover by running configure with the --help option).  But the
one of most interest is the one that sets the installation directory.
By default, the configure script will set things up to install
libexpat into /usr/local/lib, expat.h into /usr/local/include, and
xmlwf into /usr/local/bin.  If, for example, you'd prefer to install
into /home/me/mystuff/lib, /home/me/mystuff/include, and
/home/me/mystuff/bin, you can tell configure about that with:

	./configure --prefix=/home/me/mystuff

After running the configure script, the "make" command will build
things and "make install" will install things into their proper
location.  Note that you need to have write permission into the
directories into which things will be installed.

When building for use with C++, you may need to add additional
compiler flags to support proper interaction with exceptions.  This
can be done by setting the CFLAGS environment variable.  For example,
when using GCC, you can use:

	CFLAGS=-fexceptions ./configure

Note for Solaris users:  The "ar" command is usually located in
"/usr/ccs/bin", which is not in the default PATH.  You will need to
add this to your path for the "make" command, and probably also switch
to GNU make (the "make" found in /usr/ccs/bin does not seem to work
properly -- appearantly it does not understand .PHONY directives).  If
you're using ksh or bash, use this command to build:

	PATH=/usr/ccs/bin:$PATH make

A reference manual is available in the file doc/reference.html in this
distribution.

The homepage for this project is http://expat.sourceforge.net/.  There
are links there to connect you to the bug reports page.  If you need
to report a bug when you don't have access to a browser, you may also
send a bug report by email to expat-bugs@lists.sourceforge.net.

Discussion related to the direction of future expat development takes
place on expat-discuss@lists.sourceforge.net.  Archives of this list
may be found at http://www.geocrawler.com/redir-sf.php3?list=expat-discuss.
