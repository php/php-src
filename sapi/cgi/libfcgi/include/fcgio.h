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

#include <iostream>

#include "fcgiapp.h"

#ifndef DLLAPI
#ifdef _WIN32
#define DLLAPI __declspec(dllimport)
#else
#define DLLAPI
#endif
#endif

#if ! HAVE_STREAMBUF_CHAR_TYPE
typedef char char_type;
#endif

/*
 *  fcgi_streambuf
 */
class DLLAPI fcgi_streambuf : public std::streambuf
{
public:

    // Note that if no buf is assigned (the default), iostream methods
    // such as peek(), unget() and putback() will fail.  If a buf is
    // assigned, I/O is a bit less effecient and output streams will
    // have to be flushed (or the streambuf destroyed) before the next 
    // call to "accept".
    fcgi_streambuf(FCGX_Stream * fcgx, char * buf, int len);
    
    fcgi_streambuf(char_type * buf, std::streamsize len);
    
    fcgi_streambuf(FCGX_Stream * fcgx = 0);

    ~fcgi_streambuf(void);

    int attach(FCGX_Stream * fcgx);

protected:

    // Consume the put area (if buffered) and c (if c is not EOF).
    virtual int overflow(int);

    // Flush the put area (if buffered) and the FCGX buffer to the client.
    virtual int sync();

    // Remove and return the current character.
    virtual int uflow();

    // Fill the get area (if buffered) and return the current character.
    virtual int underflow();

    // Use a buffer.  The only reasons that a buffer would be useful is
    // to support the use of the unget()/putback() or seek() methods.  Using
    // a buffer will result in less efficient I/O.  Note: the underlying
    // FastCGI library (FCGX) maintains its own input and output buffers.  
    virtual std::streambuf * setbuf(char_type * buf, std::streamsize len);

    virtual std::streamsize xsgetn(char_type * s, std::streamsize n);
    virtual std::streamsize xsputn(const char_type * s, std::streamsize n);

private:

    FCGX_Stream * fcgx;

    // buf is just handy to have around
    char_type * buf;

    // this isn't kept by the base class
    std::streamsize bufsize;
    
    void init(FCGX_Stream * fcgx, char_type * buf, std::streamsize bufsize);

    void reset(void);
};

/*
 *  fcgi_istream - deprecated
 */
class DLLAPI fcgi_istream : public std::istream
{
public:

    // deprecated
    fcgi_istream(FCGX_Stream * fcgx = 0);
    
    // deprecated
    ~fcgi_istream(void) {}

    // deprecated
    virtual void attach(FCGX_Stream * fcgx);

private:

    fcgi_streambuf fcgi_strmbuf;
};

/*
 *  fcgi_ostream - deprecated
 */
class DLLAPI fcgi_ostream : public std::ostream
{
public:
    
    // deprecated
    fcgi_ostream(FCGX_Stream * fcgx = 0);
    
    // deprecated
    ~fcgi_ostream(void) {}

    // deprecated
    virtual void attach(FCGX_Stream *fcgx);

private:

    fcgi_streambuf fcgi_strmbuf;
};

#endif /* FCGIO_H */
