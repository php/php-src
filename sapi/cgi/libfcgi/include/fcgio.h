//
// Provides support for FastCGI via C++ iostreams.
//
// $Id$
//
// This work is based on routines written by George Feinberg. They
// have been mostly re-written and extensively changed by
// Michael Richards.
//
// Rewritten again with bug fixes and numerous enhancements by
// Michael Shell.
// 
// And rewritten again by Rob Saccoccio. 
//
// Special Thanks to Dietmar Kuehl for his help and the numerous custom
// streambuf examples on his web site.
//
// Copyright (c) 2000 Tux the Linux Penguin
// Copyright (c) 2001 Rob Saccoccio and Chelsea Networks
//
// You are free to use this software without charge or royalty
// as long as this notice is not removed or altered, and recognition
// is given to the author(s)
//
// This code is offered as-is without any warranty either expressed or
// implied; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  If it breaks, you get to keep 
// both halves.

#ifndef FCGIO_H
#define FCGIO_H

#include <iostream.h>

#include "fcgiapp.h"

#ifndef DLLAPI
#ifdef _WIN32
#define DLLAPI __declspec(dllimport)
#else
#define DLLAPI
#endif
#endif

/*
 *  fcgi_streambuf
 */
class fcgi_streambuf : public streambuf
{
public:

    // Note that if no buf is assigned (the default), iostream methods
    // such as peek(), unget() and putback() will fail.  If a buf is
    // assigned, I/O is a bit less effecient and output streams will
    // have to be flushed (or the streambuf destroyed) before the next 
    // call to "accept".
    DLLAPI fcgi_streambuf(FCGX_Stream * fcgx, char * buf, int len);
    
    DLLAPI fcgi_streambuf(char * buf, int len);
    
    DLLAPI fcgi_streambuf(FCGX_Stream * fcgx = NULL);

    DLLAPI ~fcgi_streambuf(void);

    DLLAPI int attach(FCGX_Stream * fcgx);

protected:

    // Consume the put area (if buffered) and c (if c is not EOF).
    DLLAPI virtual int overflow(int);

    // Flush the put area (if buffered) and the FCGX buffer to the client.
    DLLAPI virtual int sync();

    // Remove and return the current character.
    DLLAPI virtual int uflow();

    // Fill the get area (if buffered) and return the current character.
    DLLAPI virtual int underflow();

    // Use a buffer.  The only reasons that a buffer would be useful is
    // to support the use of the unget()/putback() or seek() methods.  Using
    // a buffer will result in less efficient I/O.  Note: the underlying
    // FastCGI library (FCGX) maintains its own input and output buffers.  
    DLLAPI virtual streambuf * setbuf(char * buf, int len);

    DLLAPI virtual int xsgetn(char * s, int n);
    DLLAPI virtual int xsputn(const char * s, int n);

private:

    FCGX_Stream * fcgx;

    // buf is just handy to have around
    char * buf;

    // this isn't kept by the base class
    int bufsize;
    
    void init(FCGX_Stream * fcgx, char * buf, int bufsize);

    void reset(void);
};

/*
 *  fcgi_istream - deprecated
 */
class fcgi_istream : public istream
{
public:

    // deprecated
    DLLAPI fcgi_istream(FCGX_Stream * fcgx = NULL);
    
    // deprecated
    DLLAPI ~fcgi_istream(void) {}

    // deprecated
    DLLAPI virtual void attach(FCGX_Stream * fcgx);

private:

    fcgi_streambuf fcgi_strmbuf;
};

/*
 *  fcgi_ostream - deprecated
 */
class fcgi_ostream : public ostream
{
public:
    
    // deprecated
    DLLAPI fcgi_ostream(FCGX_Stream * fcgx = NULL);
    
    // deprecated
    DLLAPI ~fcgi_ostream(void) {}

    // deprecated
    DLLAPI virtual void attach(FCGX_Stream *fcgx);

private:

    fcgi_streambuf fcgi_strmbuf;
};

#endif /* FCGIO_H */
