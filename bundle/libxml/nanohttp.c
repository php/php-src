/*
 * nanohttp.c: minimalist HTTP GET implementation to fetch external subsets.
 *             focuses on size, streamability, reentrancy and portability
 *
 * This is clearly not a general purpose HTTP implementation
 * If you look for one, check:
 *         http://www.w3.org/Library/
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */
 
/* TODO add compression support, Send the Accept- , and decompress on the
        fly with ZLIB if found at compile-time */

#define NEED_SOCKETS
#define IN_LIBXML
#include "libxml.h"
/* Hack to get compilation to work -SH */
#include <errno.h>

#ifdef LIBXML_HTTP_ENABLED
#include <string.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_RESOLV_H
#ifdef HAVE_ARPA_NAMESER_H
#include <arpa/nameser.h>
#endif
#include <resolv.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h> 
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef SUPPORT_IP6
#include <resolv.h>
#endif

#ifdef VMS
#include <stropts>
#define SOCKLEN_T unsigned int
#define SOCKET int
#endif

#include <libxml/globals.h>
#include <libxml/xmlerror.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h> /* for xmlStr(n)casecmp() */
#include <libxml/nanohttp.h>
#include <libxml/globals.h>
#include <libxml/uri.h>

/**
 * A couple portability macros
 */
#ifndef _WINSOCKAPI_
#define closesocket(s) close(s)
#define SOCKET int
#endif

#ifndef SOCKLEN_T
#define SOCKLEN_T unsigned int
#endif
#ifndef SOCKET
#define SOCKET int
#endif

#ifdef STANDALONE
#define DEBUG_HTTP
#define xmlStrncasecmp(a, b, n) strncasecmp((char *)a, (char *)b, n)
#define xmlStrcasecmpi(a, b) strcasecmp((char *)a, (char *)b)
#endif

#define XML_NANO_HTTP_MAX_REDIR	10

#define XML_NANO_HTTP_CHUNK	4096

#define XML_NANO_HTTP_CLOSED	0
#define XML_NANO_HTTP_WRITE	1
#define XML_NANO_HTTP_READ	2
#define XML_NANO_HTTP_NONE	4

typedef struct xmlNanoHTTPCtxt {
    char *protocol;	/* the protocol name */
    char *hostname;	/* the host name */
    int port;		/* the port */
    char *path;		/* the path within the URL */
    SOCKET fd;		/* the file descriptor for the socket */
    int state;		/* WRITE / READ / CLOSED */
    char *out;		/* buffer sent (zero terminated) */
    char *outptr;	/* index within the buffer sent */
    char *in;		/* the receiving buffer */
    char *content;	/* the start of the content */
    char *inptr;	/* the next byte to read from network */
    char *inrptr;	/* the next byte to give back to the client */
    int inlen;		/* len of the input buffer */
    int last;		/* return code for last operation */
    int returnValue;	/* the protocol return value */
    int ContentLength;  /* specified content length from HTTP header */
    char *contentType;	/* the MIME type for the input */
    char *location;	/* the new URL in case of redirect */
    char *authHeader;	/* contents of {WWW,Proxy}-Authenticate header */
} xmlNanoHTTPCtxt, *xmlNanoHTTPCtxtPtr;

static int initialized = 0;
static char *proxy = NULL;	 /* the proxy name if any */
static int proxyPort;	/* the proxy port if any */
static unsigned int timeout = 60;/* the select() timeout in seconds */

int xmlNanoHTTPFetchContent( void * ctx, char ** ptr, int * len );
int xmlNanoHTTPContentLength( void * ctx );

/**
 * A portability function
 */
static int socket_errno(void) {
#ifdef _WINSOCKAPI_
    return(WSAGetLastError());
#else
    return(errno);
#endif
}

/**
 * xmlNanoHTTPInit:
 *
 * Initialize the HTTP protocol layer.
 * Currently it just checks for proxy informations
 */

void
xmlNanoHTTPInit(void) {
    const char *env;
#ifdef _WINSOCKAPI_
    WSADATA wsaData;    
#endif

    if (initialized)
	return;

#ifdef _WINSOCKAPI_
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	return;
#endif

    if (proxy == NULL) {
	proxyPort = 80;
	env = getenv("no_proxy");
	if (env != NULL)
	    goto done;
	env = getenv("http_proxy");
	if (env != NULL) {
	    xmlNanoHTTPScanProxy(env);
	    goto done;
	}
	env = getenv("HTTP_PROXY");
	if (env != NULL) {
	    xmlNanoHTTPScanProxy(env);
	    goto done;
	}
    }
done:
    initialized = 1;
}

/**
 * xmlNanoHTTPCleanup:
 *
 * Cleanup the HTTP protocol layer.
 */

void
xmlNanoHTTPCleanup(void) {
    if (proxy != NULL)
	xmlFree(proxy);
#ifdef _WINSOCKAPI_
    if (initialized)
	WSACleanup();
#endif
    initialized = 0;
    return;
}

/**
 * xmlNanoHTTPScanURL:
 * @ctxt:  an HTTP context
 * @URL:  The URL used to initialize the context
 *
 * (Re)Initialize an HTTP context by parsing the URL and finding
 * the protocol host port and path it indicates.
 */

static void
xmlNanoHTTPScanURL(xmlNanoHTTPCtxtPtr ctxt, const char *URL) {
    const char *cur = URL;
    char buf[4096];
    int indx = 0;
    int port = 0;

    if (ctxt->protocol != NULL) { 
        xmlFree(ctxt->protocol);
	ctxt->protocol = NULL;
    }
    if (ctxt->hostname != NULL) { 
        xmlFree(ctxt->hostname);
	ctxt->hostname = NULL;
    }
    if (ctxt->path != NULL) { 
        xmlFree(ctxt->path);
	ctxt->path = NULL;
    }
    if (URL == NULL) return;
    buf[indx] = 0;
    while (*cur != 0) {
        if ((cur[0] == ':') && (cur[1] == '/') && (cur[2] == '/')) {
	    buf[indx] = 0;
	    ctxt->protocol = xmlMemStrdup(buf);
	    indx = 0;
            cur += 3;
	    break;
	}
	buf[indx++] = *cur++;
    }
    if (*cur == 0) return;

    buf[indx] = 0;
    while (1) {
        if (cur[0] == ':') {
	    buf[indx] = 0;
	    ctxt->hostname = xmlMemStrdup(buf);
	    indx = 0;
	    cur += 1;
	    while ((*cur >= '0') && (*cur <= '9')) {
	        port *= 10;
		port += *cur - '0';
		cur++;
	    }
	    if (port != 0) ctxt->port = port;
	    while ((cur[0] != '/') && (*cur != 0)) 
	        cur++;
	    break;
	}
        if ((*cur == '/') || (*cur == 0)) {
	    buf[indx] = 0;
	    ctxt->hostname = xmlMemStrdup(buf);
	    indx = 0;
	    break;
	}
	buf[indx++] = *cur++;
    }
    if (*cur == 0) 
        ctxt->path = xmlMemStrdup("/");
    else {
        indx = 0;
        buf[indx] = 0;
	while (*cur != 0)
	    buf[indx++] = *cur++;
	buf[indx] = 0;
	ctxt->path = xmlMemStrdup(buf);
    }	
}

/**
 * xmlNanoHTTPScanProxy:
 * @URL:  The proxy URL used to initialize the proxy context
 *
 * (Re)Initialize the HTTP Proxy context by parsing the URL and finding
 * the protocol host port it indicates.
 * Should be like http://myproxy/ or http://myproxy:3128/
 * A NULL URL cleans up proxy informations.
 */

void
xmlNanoHTTPScanProxy(const char *URL) {
    const char *cur = URL;
    char buf[4096];
    int indx = 0;
    int port = 0;

    if (proxy != NULL) { 
        xmlFree(proxy);
	proxy = NULL;
    }
    if (proxyPort != 0) { 
	proxyPort = 0;
    }
#ifdef DEBUG_HTTP
    if (URL == NULL)
	xmlGenericError(xmlGenericErrorContext,
		"Removing HTTP proxy info\n");
    else
	xmlGenericError(xmlGenericErrorContext,
		"Using HTTP proxy %s\n", URL);
#endif
    if (URL == NULL) return;
    buf[indx] = 0;
    while (*cur != 0) {
        if ((cur[0] == ':') && (cur[1] == '/') && (cur[2] == '/')) {
	    buf[indx] = 0;
	    indx = 0;
            cur += 3;
	    break;
	}
	buf[indx++] = *cur++;
    }
    if (*cur == 0) return;

    buf[indx] = 0;
    while (1) {
        if (cur[0] == ':') {
	    buf[indx] = 0;
	    proxy = xmlMemStrdup(buf);
	    indx = 0;
	    cur += 1;
	    while ((*cur >= '0') && (*cur <= '9')) {
	        port *= 10;
		port += *cur - '0';
		cur++;
	    }
	    if (port != 0) proxyPort = port;
	    while ((cur[0] != '/') && (*cur != 0)) 
	        cur++;
	    break;
	}
        if ((*cur == '/') || (*cur == 0)) {
	    buf[indx] = 0;
	    proxy = xmlMemStrdup(buf);
	    indx = 0;
	    break;
	}
	buf[indx++] = *cur++;
    }
}

/**
 * xmlNanoHTTPNewCtxt:
 * @URL:  The URL used to initialize the context
 *
 * Allocate and initialize a new HTTP context.
 *
 * Returns an HTTP context or NULL in case of error.
 */

static xmlNanoHTTPCtxtPtr
xmlNanoHTTPNewCtxt(const char *URL) {
    xmlNanoHTTPCtxtPtr ret;
    xmlChar *escaped;

    ret = (xmlNanoHTTPCtxtPtr) xmlMalloc(sizeof(xmlNanoHTTPCtxt));
    if (ret == NULL) return(NULL);

    memset(ret, 0, sizeof(xmlNanoHTTPCtxt));
    ret->port = 80;
    ret->returnValue = 0;
    ret->fd = -1;
    ret->ContentLength = -1;

    escaped = xmlURIEscapeStr(BAD_CAST URL, BAD_CAST"@/:=?;#%&");
    if (escaped != NULL) {
	xmlNanoHTTPScanURL(ret, (const char *) escaped);
	xmlFree(escaped);
    } else {
	xmlNanoHTTPScanURL(ret, URL);
    }

    return(ret);
}

/**
 * xmlNanoHTTPFreeCtxt:
 * @ctxt:  an HTTP context
 *
 * Frees the context after closing the connection.
 */

static void
xmlNanoHTTPFreeCtxt(xmlNanoHTTPCtxtPtr ctxt) {
    if (ctxt == NULL) return;
    if (ctxt->hostname != NULL) xmlFree(ctxt->hostname);
    if (ctxt->protocol != NULL) xmlFree(ctxt->protocol);
    if (ctxt->path != NULL) xmlFree(ctxt->path);
    if (ctxt->out != NULL) xmlFree(ctxt->out);
    if (ctxt->in != NULL) xmlFree(ctxt->in);
    if (ctxt->contentType != NULL) xmlFree(ctxt->contentType);
    if (ctxt->location != NULL) xmlFree(ctxt->location);
    if (ctxt->authHeader != NULL) xmlFree(ctxt->authHeader);
    ctxt->state = XML_NANO_HTTP_NONE;
    if (ctxt->fd >= 0) closesocket(ctxt->fd);
    ctxt->fd = -1;
    xmlFree(ctxt);
}

/**
 * xmlNanoHTTPSend:
 * @ctxt:  an HTTP context
 *
 * Send the input needed to initiate the processing on the server side
 * Returns number of bytes sent or -1 on error.
 */

static int
xmlNanoHTTPSend(xmlNanoHTTPCtxtPtr ctxt, const char * xmt_ptr, int outlen) {

    int 	total_sent = 0;

    if ( (ctxt->state & XML_NANO_HTTP_WRITE) && (xmt_ptr != NULL ) ) {
        while (total_sent < outlen) {
            int nsent = send(ctxt->fd, xmt_ptr + total_sent,
                                      outlen - total_sent, 0);
            if (nsent>0)
                total_sent += nsent;
	    else if ( ( nsent == -1 ) && 
#if defined(EAGAIN) && EAGAIN != EWOULDBLOCK
	    	      ( socket_errno( ) != EAGAIN ) &&
#endif
		        ( socket_errno( ) != EWOULDBLOCK ) ) {
	        xmlGenericError( xmlGenericErrorContext,
				"xmlNanoHTTPSend error:  %s",
				strerror( socket_errno( ) ) );

		if ( total_sent == 0 )
		    total_sent = -1;
		break;
	    }
	    else {
	        /*
		**  No data sent
		**  Since non-blocking sockets are used, wait for 
		**  socket to be writable or default timeout prior
		**  to retrying.
		*/

		struct timeval	tv;
		fd_set		wfd;

		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		FD_ZERO( &wfd );
		FD_SET( ctxt->fd, &wfd );
		(void)select( ctxt->fd + 1, NULL, &wfd, NULL, &tv );
	    }
	}
    }

    return total_sent;
}

/**
 * xmlNanoHTTPRecv:
 * @ctxt:  an HTTP context
 *
 * Read information coming from the HTTP connection.
 * This is a blocking call (but it blocks in select(), not read()).
 *
 * Returns the number of byte read or -1 in case of error.
 */

static int
xmlNanoHTTPRecv(xmlNanoHTTPCtxtPtr ctxt) {
    fd_set rfd;
    struct timeval tv;


    while (ctxt->state & XML_NANO_HTTP_READ) {
	if (ctxt->in == NULL) {
	    ctxt->in = (char *) xmlMalloc(65000 * sizeof(char));
	    if (ctxt->in == NULL) {
	        ctxt->last = -1;
		xmlGenericError( xmlGenericErrorContext, 
			"xmlNanoHTTPRecv:  Error allocating input memory." );
		return(-1);
	    }
	    ctxt->inlen = 65000;
	    ctxt->inptr = ctxt->content = ctxt->inrptr = ctxt->in;
	}
	if (ctxt->inrptr > ctxt->in + XML_NANO_HTTP_CHUNK) {
	    int delta = ctxt->inrptr - ctxt->in;
	    int len = ctxt->inptr - ctxt->inrptr;
	    
	    memmove(ctxt->in, ctxt->inrptr, len);
	    ctxt->inrptr -= delta;
	    ctxt->content -= delta;
	    ctxt->inptr -= delta;
	}
        if ((ctxt->in + ctxt->inlen) < (ctxt->inptr + XML_NANO_HTTP_CHUNK)) {
	    int d_inptr = ctxt->inptr - ctxt->in;
	    int d_content = ctxt->content - ctxt->in;
	    int d_inrptr = ctxt->inrptr - ctxt->in;
	    char *	tmp_ptr = ctxt->in;

	    ctxt->inlen *= 2;
            ctxt->in = (char *) xmlRealloc(tmp_ptr, ctxt->inlen);
	    if (ctxt->in == NULL) {
	        xmlGenericError( xmlGenericErrorContext,
				"xmlNanoHTTPRecv:  %s %d bytes.",
				"Failed to realloc input buffer to",
				ctxt->inlen );
		xmlFree( tmp_ptr );
	        ctxt->last = -1;
		return(-1);
	    }
            ctxt->inptr = ctxt->in + d_inptr;
            ctxt->content = ctxt->in + d_content;
            ctxt->inrptr = ctxt->in + d_inrptr;
	}
	ctxt->last = recv(ctxt->fd, ctxt->inptr, XML_NANO_HTTP_CHUNK, 0);
	if (ctxt->last > 0) {
	    ctxt->inptr += ctxt->last;
	    return(ctxt->last);
	}
	if (ctxt->last == 0) {
	    return(0);
	}
	if (ctxt->last == -1) {
	    switch (socket_errno()) {
		case EINPROGRESS:
		case EWOULDBLOCK:
#if defined(EAGAIN) && EAGAIN != EWOULDBLOCK
		case EAGAIN:
#endif
		    break;

		case ECONNRESET:
		case ESHUTDOWN:
		    return ( 0 );

		default:
		    xmlGenericError( xmlGenericErrorContext,
		    		"xmlNanoHTTPRecv:  recv( ) failure - %s",
				strerror( socket_errno( ) ) );
		    return(-1);
	    }
	}

	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	FD_ZERO(&rfd);
	FD_SET(ctxt->fd, &rfd);
	
	if ( (select(ctxt->fd+1, &rfd, NULL, NULL, &tv)<1)
#if defined(EINTR)
		&& (errno != EINTR)
#endif
	)
		return(0);
    }
    return(0);
}

/**
 * xmlNanoHTTPReadLine:
 * @ctxt:  an HTTP context
 *
 * Read one line in the HTTP server output, usually for extracting
 * the HTTP protocol informations from the answer header.
 *
 * Returns a newly allocated string with a copy of the line, or NULL
 *         which indicate the end of the input.
 */

static char *
xmlNanoHTTPReadLine(xmlNanoHTTPCtxtPtr ctxt) {
    char buf[4096];
    char *bp = buf;
    int	rc;
    
    while (bp - buf < 4095) {
	if (ctxt->inrptr == ctxt->inptr) {
	    if ( (rc = xmlNanoHTTPRecv(ctxt)) == 0) {
		if (bp == buf)
		    return(NULL);
		else
		    *bp = 0;
		return(xmlMemStrdup(buf));
	    }
	    else if ( rc == -1 ) {
	        return ( NULL );
	    }
	}
	*bp = *ctxt->inrptr++;
	if (*bp == '\n') {
	    *bp = 0;
	    return(xmlMemStrdup(buf));
	}
	if (*bp != '\r')
	    bp++;
    }
    buf[4095] = 0;
    return(xmlMemStrdup(buf));
}


/**
 * xmlNanoHTTPScanAnswer:
 * @ctxt:  an HTTP context
 * @line:  an HTTP header line
 *
 * Try to extract useful informations from the server answer.
 * We currently parse and process:
 *  - The HTTP revision/ return code
 *  - The Content-Type
 *  - The Location for redirect processing.
 *
 * Returns -1 in case of failure, the file descriptor number otherwise
 */

static void
xmlNanoHTTPScanAnswer(xmlNanoHTTPCtxtPtr ctxt, const char *line) {
    const char *cur = line;

    if (line == NULL) return;

    if (!strncmp(line, "HTTP/", 5)) {
        int version = 0;
	int ret = 0;

	cur += 5;
	while ((*cur >= '0') && (*cur <= '9')) {
	    version *= 10;
	    version += *cur - '0';
	    cur++;
	}
	if (*cur == '.') {
	    cur++;
	    if ((*cur >= '0') && (*cur <= '9')) {
		version *= 10;
		version += *cur - '0';
		cur++;
	    }
	    while ((*cur >= '0') && (*cur <= '9'))
		cur++;
	} else
	    version *= 10;
	if ((*cur != ' ') && (*cur != '\t')) return;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	if ((*cur < '0') || (*cur > '9')) return;
	while ((*cur >= '0') && (*cur <= '9')) {
	    ret *= 10;
	    ret += *cur - '0';
	    cur++;
	}
	if ((*cur != 0) && (*cur != ' ') && (*cur != '\t')) return;
	ctxt->returnValue = ret;
    } else if (!xmlStrncasecmp(BAD_CAST line, BAD_CAST"Content-Type:", 13)) {
        cur += 13;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	if (ctxt->contentType != NULL)
	    xmlFree(ctxt->contentType);
	ctxt->contentType = xmlMemStrdup(cur);
    } else if (!xmlStrncasecmp(BAD_CAST line, BAD_CAST"ContentType:", 12)) {
        cur += 12;
	if (ctxt->contentType != NULL) return;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	ctxt->contentType = xmlMemStrdup(cur);
    } else if (!xmlStrncasecmp(BAD_CAST line, BAD_CAST"Location:", 9)) {
        cur += 9;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	if (ctxt->location != NULL)
	    xmlFree(ctxt->location);
	ctxt->location = xmlMemStrdup(cur);
    } else if (!xmlStrncasecmp(BAD_CAST line, BAD_CAST"WWW-Authenticate:", 17)) {
        cur += 17;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	if (ctxt->authHeader != NULL)
	    xmlFree(ctxt->authHeader);
	ctxt->authHeader = xmlMemStrdup(cur);
    } else if (!xmlStrncasecmp(BAD_CAST line, BAD_CAST"Proxy-Authenticate:", 19)) {
        cur += 19;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	if (ctxt->authHeader != NULL)
	    xmlFree(ctxt->authHeader);
	ctxt->authHeader = xmlMemStrdup(cur);
    } else if ( !xmlStrncasecmp( BAD_CAST line, BAD_CAST"Content-Length:", 15) ) {
	cur += 15;
	ctxt->ContentLength = strtol( cur, NULL, 10 );
    }
}

/**
 * xmlNanoHTTPConnectAttempt:
 * @addr:  a socket address structure
 *
 * Attempt a connection to the given IP:port endpoint. It forces
 * non-blocking semantic on the socket, and allow 60 seconds for
 * the host to answer.
 *
 * Returns -1 in case of failure, the file descriptor number otherwise
 */

static int
xmlNanoHTTPConnectAttempt(struct sockaddr *addr)
{
    SOCKET s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    fd_set wfd;
    struct timeval tv;
    int status;
    
    if (s==-1) {
#ifdef DEBUG_HTTP
	perror("socket");
#endif
	xmlGenericError( xmlGenericErrorContext,
			"xmlNanoHTTPConnectAttempt: %s - %s",
			"socket creation failure",
			strerror( socket_errno( ) ) );
	return(-1);
    }
    
#ifdef _WINSOCKAPI_
    {
	u_long one = 1;

	status = ioctlsocket(s, FIONBIO, &one) == SOCKET_ERROR ? -1 : 0;
    }
#else /* _WINSOCKAPI_ */
#if defined(VMS)
    {
	int enable = 1;
	status = ioctl(s, FIONBIO, &enable);
    }
#else /* VMS */
    if ((status = fcntl(s, F_GETFL, 0)) != -1) {
#ifdef O_NONBLOCK
	status |= O_NONBLOCK;
#else /* O_NONBLOCK */
#ifdef F_NDELAY
	status |= F_NDELAY;
#endif /* F_NDELAY */
#endif /* !O_NONBLOCK */
	status = fcntl(s, F_SETFL, status);
    }
    if (status < 0) {
#ifdef DEBUG_HTTP
	perror("nonblocking");
#endif
	xmlGenericError( xmlGenericErrorContext,
			"xmlNanoHTTPConnectAttempt:  %s - %s",
			"error setting non-blocking IO",
			strerror( socket_errno( ) ) );
	closesocket(s);
	return(-1);
    }
#endif /* !VMS */
#endif /* !_WINSOCKAPI_ */

    if ((connect(s, addr, sizeof(*addr))==-1)) {
	switch (socket_errno()) {
	    case EINPROGRESS:
	    case EWOULDBLOCK:
		break;
	    default:
		xmlGenericError( xmlGenericErrorContext,
				"xmlNanoHTTPConnectAttempt:  %s - %s",
				"error connecting to HTTP server",
				strerror( socket_errno( ) ) );
		closesocket(s);
		return(-1);
	}
    }	
    
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    
    FD_ZERO(&wfd);
    FD_SET(s, &wfd);
    
    switch(select(s+1, NULL, &wfd, NULL, &tv))
    {
	case 0:
	    /* Time out */
	    xmlGenericError( xmlGenericErrorContext, 
	    			"xmlNanoHTTPConnectAttempt: %s",
				"Connect attempt timed out." );
	    closesocket(s);
	    return(-1);
	case -1:
	    /* Ermm.. ?? */
	    xmlGenericError( xmlGenericErrorContext,
	    			"xmlNanoHTTPConnectAttempt: %s - %s",
				"Error connecting to host",
				strerror( socket_errno( ) ) );
	    closesocket(s);
	    return(-1);
    }

    if ( FD_ISSET(s, &wfd) ) {
	SOCKLEN_T len;
	len = sizeof(status);
	if (getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&status, &len) < 0 ) {
	    /* Solaris error code */
	    xmlGenericError( xmlGenericErrorContext,
	    			"xmlNanoHTTPConnectAttempt: %s - %s",
				"Error retrieving pending socket errors",
				strerror( socket_errno( ) ) );
	    return (-1);
	}
	if ( status ) {
	    closesocket(s);
	    errno = status;
	    xmlGenericError( xmlGenericErrorContext,
	    			"xmlNanoHTTPConnectAttempt: %s - %s",
				"Error connecting to remote host",
				strerror( status ) );
	    return (-1);
	}
    } else {
	/* pbm */
	xmlGenericError( xmlGenericErrorContext,
		"xmlNanoHTTPConnectAttempt:  %s\n",
		"Select returned, but descriptor not set for connection.\n" );
	closesocket(s);
	return (-1);
    }
    
    return(s);
}
 
/**
 * xmlNanoHTTPConnectHost:
 * @host:  the host name
 * @port:  the port number
 *
 * Attempt a connection to the given host:port endpoint. It tries
 * the multiple IP provided by the DNS if available.
 *
 * Returns -1 in case of failure, the file descriptor number otherwise
 */

static int
xmlNanoHTTPConnectHost(const char *host, int port)
{
    struct hostent *h;
    struct sockaddr *addr;
    struct in_addr ia;
    struct sockaddr_in sockin;

#ifdef SUPPORT_IP6
    struct in6_addr ia6;
    struct sockaddr_in6 sockin6;
#endif
    int i;
    int s;

#if defined(SUPPORT_IP6) && defined(RES_USE_INET6)
    if (!(_res.options & RES_INIT))
        res_init();
    _res.options |= RES_USE_INET6;
#endif
    h = gethostbyname(host);
    if (h == NULL) {

/*
 * Okay, I got fed up by the non-portability of this error message
 * extraction code. it work on Linux, if it work on your platform
 * and one want to enable it, send me the defined(foobar) needed
 */
#if defined(HAVE_NETDB_H) && defined(HOST_NOT_FOUND) && defined(linux)
        const char *h_err_txt = "";

        switch (h_errno) {
            case HOST_NOT_FOUND:
                h_err_txt = "Authoritive host not found";
                break;

            case TRY_AGAIN:
                h_err_txt =
                    "Non-authoritive host not found or server failure.";
                break;

            case NO_RECOVERY:
                h_err_txt =
                    "Non-recoverable errors:  FORMERR, REFUSED, or NOTIMP.";
                break;

            case NO_ADDRESS:
                h_err_txt =
                    "Valid name, no data record of requested type.";
                break;

            default:
                h_err_txt = "No error text defined.";
                break;
        }
        xmlGenericError(xmlGenericErrorContext,
                        "xmlNanoHTTPConnectHost:  %s '%s' - %s",
                        "Failed to resolve host", host, h_err_txt);
#else
        xmlGenericError(xmlGenericErrorContext,
                        "xmlNanoHTTPConnectHost:  %s '%s'",
                        "Failed to resolve host", host);
#endif
        return (-1);
    }

    for (i = 0; h->h_addr_list[i]; i++) {
        if (h->h_addrtype == AF_INET) {
            /* A records (IPv4) */
            memcpy(&ia, h->h_addr_list[i], h->h_length);
            sockin.sin_family = h->h_addrtype;
            sockin.sin_addr = ia;
            sockin.sin_port = htons(port);
            addr = (struct sockaddr *) &sockin;
#ifdef SUPPORT_IP6
        } else if (h->h_addrtype == AF_INET6) {
            /* AAAA records (IPv6) */
            memcpy(&ia6, h->h_addr_list[i], h->h_length);
            sockin6.sin_family = h->h_addrtype;
            sockin6.sin_addr = ia6;
            sockin6.sin_port = htons(port);
            addr = (struct sockaddr *) &sockin6;
#endif
        } else
            break;              /* for */

        s = xmlNanoHTTPConnectAttempt(addr);
        if (s != -1)
            return (s);
    }

#ifdef DEBUG_HTTP
    xmlGenericError(xmlGenericErrorContext,
                    "xmlNanoHTTPConnectHost:  unable to connect to '%s'.\n",
                    host);
#endif
    return (-1);
}


/**
 * xmlNanoHTTPOpen:
 * @URL:  The URL to load
 * @contentType:  if available the Content-Type information will be
 *                returned at that location
 *
 * This function try to open a connection to the indicated resource
 * via HTTP GET.
 *
 * Returns NULL in case of failure, otherwise a request handler.
 *     The contentType, if provided must be freed by the caller
 */

void*
xmlNanoHTTPOpen(const char *URL, char **contentType) {
    if (contentType != NULL) *contentType = NULL;
    return(xmlNanoHTTPMethod(URL, NULL, NULL, contentType, NULL, 0));
}

/**
 * xmlNanoHTTPOpenRedir:
 * @URL:  The URL to load
 * @contentType:  if available the Content-Type information will be
 *                returned at that location
 * @redir: if available the redirected URL will be returned
 *
 * This function try to open a connection to the indicated resource
 * via HTTP GET.
 *
 * Returns NULL in case of failure, otherwise a request handler.
 *     The contentType, if provided must be freed by the caller
 */

void*
xmlNanoHTTPOpenRedir(const char *URL, char **contentType, char **redir) {
    if (contentType != NULL) *contentType = NULL;
    if (redir != NULL) *redir = NULL;
    return(xmlNanoHTTPMethodRedir(URL, NULL, NULL, contentType, redir, NULL,0));
}

/**
 * xmlNanoHTTPRead:
 * @ctx:  the HTTP context
 * @dest:  a buffer
 * @len:  the buffer length
 *
 * This function tries to read @len bytes from the existing HTTP connection
 * and saves them in @dest. This is a blocking call.
 *
 * Returns the number of byte read. 0 is an indication of an end of connection.
 *         -1 indicates a parameter error.
 */
int
xmlNanoHTTPRead(void *ctx, void *dest, int len) {
    xmlNanoHTTPCtxtPtr ctxt = (xmlNanoHTTPCtxtPtr) ctx;

    if (ctx == NULL) return(-1);
    if (dest == NULL) return(-1);
    if (len <= 0) return(0);

    while (ctxt->inptr - ctxt->inrptr < len) {
        if (xmlNanoHTTPRecv(ctxt) <= 0) break;
    }
    if (ctxt->inptr - ctxt->inrptr < len)
        len = ctxt->inptr - ctxt->inrptr;
    memcpy(dest, ctxt->inrptr, len);
    ctxt->inrptr += len;
    return(len);
}

/**
 * xmlNanoHTTPClose:
 * @ctx:  the HTTP context
 *
 * This function closes an HTTP context, it ends up the connection and
 * free all data related to it.
 */
void
xmlNanoHTTPClose(void *ctx) {
    xmlNanoHTTPCtxtPtr ctxt = (xmlNanoHTTPCtxtPtr) ctx;

    if (ctx == NULL) return;

    xmlNanoHTTPFreeCtxt(ctxt);
}

/**
 * xmlNanoHTTPMethodRedir:
 * @URL:  The URL to load
 * @method:  the HTTP method to use
 * @input:  the input string if any
 * @contentType:  the Content-Type information IN and OUT
 * @redir:  the redirected URL OUT
 * @headers:  the extra headers
 * @ilen:  input length
 *
 * This function try to open a connection to the indicated resource
 * via HTTP using the given @method, adding the given extra headers
 * and the input buffer for the request content.
 *
 * Returns NULL in case of failure, otherwise a request handler.
 *     The contentType, or redir, if provided must be freed by the caller
 */

void*
xmlNanoHTTPMethodRedir(const char *URL, const char *method, const char *input,
                  char **contentType, char **redir,
		  const char *headers, int ilen ) {
    xmlNanoHTTPCtxtPtr ctxt;
    char *bp, *p;
    int blen, ret;
    int head;
    int xmt_bytes;
    int nbRedirects = 0;
    char *redirURL = NULL;
    
    if (URL == NULL) return(NULL);
    if (method == NULL) method = "GET";
    xmlNanoHTTPInit();

retry:
    if (redirURL == NULL)
	ctxt = xmlNanoHTTPNewCtxt(URL);
    else {
	ctxt = xmlNanoHTTPNewCtxt(redirURL);
    }

    if ( ctxt == NULL ) {
	xmlGenericError( xmlGenericErrorContext,
			"xmlNanoHTTPMethodRedir:  %s %s.",
			"Unable to allocate HTTP context to URI",
			( ( redirURL == NULL ) ? URL : redirURL ) );
	return ( NULL );
    }

    if ((ctxt->protocol == NULL) || (strcmp(ctxt->protocol, "http"))) {
	xmlGenericError( xmlGenericErrorContext,
			"xmlNanoHTTPMethodRedir:  %s - %s.",
			"Not a valid HTTP URI",
			( ( redirURL == NULL ) ? URL : redirURL ) );
        xmlNanoHTTPFreeCtxt(ctxt);
	if (redirURL != NULL) xmlFree(redirURL);
        return(NULL);
    }
    if (ctxt->hostname == NULL) {
	xmlGenericError( xmlGenericErrorContext,
			"xmlNanoHTTPMethodRedir:  %s - %s",
			"Failed to identify host in URI",
			( ( redirURL == NULL ) ? URL : redirURL ) );
        xmlNanoHTTPFreeCtxt(ctxt);
	if (redirURL != NULL) xmlFree(redirURL);
        return(NULL);
    }
    if (proxy) {
	blen = strlen(ctxt->hostname) * 2 + 16;
	ret = xmlNanoHTTPConnectHost(proxy, proxyPort);
    }
    else {
	blen = strlen(ctxt->hostname);
	ret = xmlNanoHTTPConnectHost(ctxt->hostname, ctxt->port);
    }
    if (ret < 0) {
        xmlNanoHTTPFreeCtxt(ctxt);
	if (redirURL != NULL) xmlFree(redirURL);
        return(NULL);
    }
    ctxt->fd = ret;

    if (input == NULL)
	ilen = 0;
    else
	blen += 36;

    if (headers != NULL)
	blen += strlen(headers) + 2;
    if (contentType && *contentType)
	blen += strlen(*contentType) + 16;
    blen += strlen(method) + strlen(ctxt->path) + 24;
    bp = xmlMalloc(blen);
    if ( bp == NULL ) {
        xmlNanoHTTPFreeCtxt( ctxt );
	xmlGenericError( xmlGenericErrorContext,
			"xmlNanoHTTPMethodRedir:  %s",
			"Error allocating HTTP header buffer." );
	return ( NULL );
    }

    p = bp;

    if (proxy) {
	if (ctxt->port != 80) {
	    p += snprintf( p, blen - (p - bp), "%s http://%s:%d%s", 
			method, ctxt->hostname,
		 	ctxt->port, ctxt->path );
	}
	else 
	    p += snprintf( p, blen - (p - bp), "%s http://%s%s", method,
	    		ctxt->hostname, ctxt->path);
    }
    else
	p += snprintf( p, blen - (p - bp), "%s %s", method, ctxt->path);

    p += snprintf( p, blen - (p - bp), " HTTP/1.0\r\nHost: %s\r\n", 
		    ctxt->hostname);

    if (contentType != NULL && *contentType) 
	p += snprintf(p, blen - (p - bp), "Content-Type: %s\r\n", *contentType);

    if (headers != NULL)
	p += snprintf( p, blen - (p - bp), "%s", headers );

    if (input != NULL)
	snprintf(p, blen - (p - bp), "Content-Length: %d\r\n\r\n", ilen );
    else
	snprintf(p, blen - (p - bp), "\r\n");

#ifdef DEBUG_HTTP
    xmlGenericError(xmlGenericErrorContext,
	    "-> %s%s", proxy? "(Proxy) " : "", bp);
    if ((blen -= strlen(bp)+1) < 0)
	xmlGenericError(xmlGenericErrorContext,
		"ERROR: overflowed buffer by %d bytes\n", -blen);
#endif
    ctxt->outptr = ctxt->out = bp;
    ctxt->state = XML_NANO_HTTP_WRITE;
    blen = strlen( ctxt->out );
    xmt_bytes = xmlNanoHTTPSend(ctxt, ctxt->out, blen );
#ifdef DEBUG_HTTP
    if ( xmt_bytes != blen )
        xmlGenericError( xmlGenericErrorContext,
			"xmlNanoHTTPMethodRedir:  Only %d of %d %s %s\n",
			xmt_bytes, blen,
			"bytes of HTTP headers sent to host",
			ctxt->hostname );
#endif

    if ( input != NULL ) {
        xmt_bytes = xmlNanoHTTPSend( ctxt, input, ilen );

#ifdef DEBUG_HTTP
	if ( xmt_bytes != ilen )
	    xmlGenericError( xmlGenericErrorContext,
	    		"xmlNanoHTTPMethodRedir:  Only %d of %d %s %s\n",
			xmt_bytes, ilen,
			"bytes of HTTP content sent to host",
			ctxt->hostname );
#endif
    }

    ctxt->state = XML_NANO_HTTP_READ;
    head = 1;

    while ((p = xmlNanoHTTPReadLine(ctxt)) != NULL) {
        if (head && (*p == 0)) {
	    head = 0;
	    ctxt->content = ctxt->inrptr;
	    xmlFree(p);
	    break;
	}
	xmlNanoHTTPScanAnswer(ctxt, p);

#ifdef DEBUG_HTTP
	xmlGenericError(xmlGenericErrorContext, "<- %s\n", p);
#endif
        xmlFree(p);
    }

    if ((ctxt->location != NULL) && (ctxt->returnValue >= 300) &&
        (ctxt->returnValue < 400)) {
#ifdef DEBUG_HTTP
	xmlGenericError(xmlGenericErrorContext,
		"\nRedirect to: %s\n", ctxt->location);
#endif
	while ( xmlNanoHTTPRecv(ctxt) > 0 ) ;
        if (nbRedirects < XML_NANO_HTTP_MAX_REDIR) {
	    nbRedirects++;
	    if (redirURL != NULL)
		xmlFree(redirURL);
	    redirURL = xmlMemStrdup(ctxt->location);
	    xmlNanoHTTPFreeCtxt(ctxt);
	    goto retry;
	}
	xmlNanoHTTPFreeCtxt(ctxt);
	if (redirURL != NULL) xmlFree(redirURL);
#ifdef DEBUG_HTTP
	xmlGenericError(xmlGenericErrorContext,
		"xmlNanoHTTPMethodRedir: Too many redirects, aborting ...\n");
#endif
	return(NULL);
    }

    if (contentType != NULL) {
	if (ctxt->contentType != NULL)
	    *contentType = xmlMemStrdup(ctxt->contentType);
	else
	    *contentType = NULL;
    }

    if ((redir != NULL) && (redirURL != NULL)) {
	*redir = redirURL;
    } else {
	if (redirURL != NULL)
	    xmlFree(redirURL);
	if (redir != NULL)
	    *redir = NULL;
    }

#ifdef DEBUG_HTTP
    if (ctxt->contentType != NULL)
	xmlGenericError(xmlGenericErrorContext,
		"\nCode %d, content-type '%s'\n\n",
	       ctxt->returnValue, ctxt->contentType);
    else
	xmlGenericError(xmlGenericErrorContext,
		"\nCode %d, no content-type\n\n",
	       ctxt->returnValue);
#endif

    return((void *) ctxt);
}

/**
 * xmlNanoHTTPMethod:
 * @URL:  The URL to load
 * @method:  the HTTP method to use
 * @input:  the input string if any
 * @contentType:  the Content-Type information IN and OUT
 * @headers:  the extra headers
 * @ilen:  input length
 *
 * This function try to open a connection to the indicated resource
 * via HTTP using the given @method, adding the given extra headers
 * and the input buffer for the request content.
 *
 * Returns NULL in case of failure, otherwise a request handler.
 *     The contentType, if provided must be freed by the caller
 */

void*
xmlNanoHTTPMethod(const char *URL, const char *method, const char *input,
                  char **contentType, const char *headers, int ilen) {
    return(xmlNanoHTTPMethodRedir(URL, method, input, contentType,
		                  NULL, headers, ilen));
}

/**
 * xmlNanoHTTPFetch:
 * @URL:  The URL to load
 * @filename:  the filename where the content should be saved
 * @contentType:  if available the Content-Type information will be
 *                returned at that location
 *
 * This function try to fetch the indicated resource via HTTP GET
 * and save it's content in the file.
 *
 * Returns -1 in case of failure, 0 incase of success. The contentType,
 *     if provided must be freed by the caller
 */
int
xmlNanoHTTPFetch(const char *URL, const char *filename, char **contentType) {
    void *ctxt = NULL;
    char *buf = NULL;
    int fd;
    int len;
    
    ctxt = xmlNanoHTTPOpen(URL, contentType);
    if (ctxt == NULL) return(-1);

    if (!strcmp(filename, "-")) 
        fd = 0;
    else {
        fd = open(filename, O_CREAT | O_WRONLY, 00644);
	if (fd < 0) {
	    xmlNanoHTTPClose(ctxt);
	    if ((contentType != NULL) && (*contentType != NULL)) {
	        xmlFree(*contentType);
		*contentType = NULL;
	    }
	    return(-1);
	}
    }

    xmlNanoHTTPFetchContent( ctxt, &buf, &len );
    if ( len > 0 ) {
	write(fd, buf, len);
    }

    xmlNanoHTTPClose(ctxt);
    close(fd);
    return(0);
}

/**
 * xmlNanoHTTPSave:
 * @ctxt:  the HTTP context
 * @filename:  the filename where the content should be saved
 *
 * This function saves the output of the HTTP transaction to a file
 * It closes and free the context at the end
 *
 * Returns -1 in case of failure, 0 incase of success.
 */
int
xmlNanoHTTPSave(void *ctxt, const char *filename) {
    char *buf = NULL;
    int fd;
    int len;
    
    if (ctxt == NULL) return(-1);

    if (!strcmp(filename, "-")) 
        fd = 0;
    else {
        fd = open(filename, O_CREAT | O_WRONLY);
	if (fd < 0) {
	    xmlNanoHTTPClose(ctxt);
	    return(-1);
	}
    }

    xmlNanoHTTPFetchContent( ctxt, &buf, &len );
    if ( len > 0 ) {
	write(fd, buf, len);
    }

    xmlNanoHTTPClose(ctxt);
    return(0);
}

/**
 * xmlNanoHTTPReturnCode:
 * @ctx:  the HTTP context
 *
 * Get the latest HTTP return code received
 *
 * Returns the HTTP return code for the request.
 */
int
xmlNanoHTTPReturnCode(void *ctx) {
    xmlNanoHTTPCtxtPtr ctxt = (xmlNanoHTTPCtxtPtr) ctx;

    if (ctxt == NULL) return(-1);

    return(ctxt->returnValue);
}

/**
 * xmlNanoHTTPAuthHeader:
 * @ctx:  the HTTP context
 *
 * Get the authentication header of an HTTP context
 *
 * Returns the stashed value of the WWW-Authenticate or Proxy-Authenticate
 * header.
 */
const char *
xmlNanoHTTPAuthHeader(void *ctx) {
    xmlNanoHTTPCtxtPtr ctxt = (xmlNanoHTTPCtxtPtr) ctx;

    if (ctxt == NULL) return(NULL);

    return(ctxt->authHeader);
}

/**
 * xmlNanoHTTPContentLength:
 * @ctx:  the HTTP context
 *
 * Provides the specified content length from the HTTP header.
 *
 * Return the specified content length from the HTTP header.  Note that
 * a value of -1 indicates that the content length element was not included in
 * the response header.
 */
int
xmlNanoHTTPContentLength( void * ctx ) {
    xmlNanoHTTPCtxtPtr	ctxt = ctx;

    return ( ( ctxt == NULL ) ? -1 : ctxt->ContentLength );
}

/**
 * xmlNanoHTTPFetchContent:
 * @ctx:  the HTTP context
 * @ptr:  pointer to set to the content buffer.
 * @len:  integer pointer to hold the length of the content
 *
 * Check if all the content was read
 *
 * Returns 0 if all the content was read and available, returns
 * -1 if received content length was less than specified or an error 
 * occurred.
 */
int
xmlNanoHTTPFetchContent( void * ctx, char ** ptr, int * len ) {
    xmlNanoHTTPCtxtPtr	ctxt = ctx;

    int			rc = 0;
    int			cur_lgth;
    int			rcvd_lgth;
    int			dummy_int;
    char *		dummy_ptr = NULL;

    /*  Dummy up return input parameters if not provided  */

    if ( len == NULL )
        len = &dummy_int;

    if ( ptr == NULL )
        ptr = &dummy_ptr;

    /*  But can't work without the context pointer  */

    if ( ( ctxt == NULL ) || ( ctxt->content == NULL ) ) {
        *len = 0;
	*ptr = NULL;
	return ( -1 );
    }

    rcvd_lgth = ctxt->inptr - ctxt->content;

    while ( (cur_lgth = xmlNanoHTTPRecv( ctxt )) > 0 ) {

	rcvd_lgth += cur_lgth;
	if ( (ctxt->ContentLength > 0) && (rcvd_lgth >= ctxt->ContentLength) )
	    break;
    }

    *ptr = ctxt->content;
    *len = rcvd_lgth;

    if ( ( ctxt->ContentLength > 0 ) && ( rcvd_lgth < ctxt->ContentLength ) )
        rc = -1;
    else if ( rcvd_lgth == 0 )
	rc = -1;

    return ( rc );
}

#ifdef STANDALONE
int main(int argc, char **argv) {
    char *contentType = NULL;

    if (argv[1] != NULL) {
	if (argv[2] != NULL) 
	    xmlNanoHTTPFetch(argv[1], argv[2], &contentType);
        else
	    xmlNanoHTTPFetch(argv[1], "-", &contentType);
	if (contentType != NULL) xmlFree(contentType);
    } else {
        xmlGenericError(xmlGenericErrorContext,
		"%s: minimal HTTP GET implementation\n", argv[0]);
        xmlGenericError(xmlGenericErrorContext,
		"\tusage %s [ URL [ filename ] ]\n", argv[0]);
    }
    xmlNanoHTTPCleanup();
    xmlMemoryDump();
    return(0);
}
#endif /* STANDALONE */
#else /* !LIBXML_HTTP_ENABLED */
#ifdef STANDALONE
#include <stdio.h>
int main(int argc, char **argv) {
    xmlGenericError(xmlGenericErrorContext,
	    "%s : HTTP support not compiled in\n", argv[0]);
    return(0);
}
#endif /* STANDALONE */
#endif /* LIBXML_HTTP_ENABLED */
