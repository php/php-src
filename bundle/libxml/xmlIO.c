/*
 * xmlIO.c : implementation of the I/O interfaces used by the parser
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 * 14 Nov 2000 ht - for VMS, truncated name of long functions to under 32 char
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif


#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

/* Figure a portable way to know if a file is a directory. */
#ifndef HAVE_STAT
#  ifdef HAVE__STAT
     /* MS C library seems to define stat and _stat. The definition
        is identical. Still, mapping them to each other causes a warning. */
#    ifndef _MSC_VER
#      define stat(x,y) _stat(x,y)
#    endif
#    define HAVE_STAT
#  endif
#endif
#ifdef HAVE_STAT
#  ifndef S_ISDIR
#    ifdef _S_ISDIR
#      define S_ISDIR(x) _S_ISDIR(x)
#    else
#      ifdef S_IFDIR
#        ifndef S_IFMT
#          ifdef _S_IFMT
#            define S_IFMT _S_IFMT
#          endif
#        endif
#        ifdef S_IFMT
#          define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#        endif
#      endif
#    endif
#  endif
#endif

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlIO.h>
#include <libxml/uri.h>
#include <libxml/nanohttp.h>
#include <libxml/nanoftp.h>
#include <libxml/xmlerror.h>
#ifdef LIBXML_CATALOG_ENABLED
#include <libxml/catalog.h>
#endif
#include <libxml/globals.h>

/* #define VERBOSE_FAILURE */
/* #define DEBUG_EXTERNAL_ENTITIES */
/* #define DEBUG_INPUT */

#ifdef DEBUG_INPUT
#define MINLEN 40
#else
#define MINLEN 4000
#endif

/*
 * Input I/O callback sets
 */
typedef struct _xmlInputCallback {
    xmlInputMatchCallback matchcallback;
    xmlInputOpenCallback opencallback;
    xmlInputReadCallback readcallback;
    xmlInputCloseCallback closecallback;
} xmlInputCallback;

#define MAX_INPUT_CALLBACK 15

static xmlInputCallback xmlInputCallbackTable[MAX_INPUT_CALLBACK];
static int xmlInputCallbackNr = 0;
static int xmlInputCallbackInitialized = 0;

/*
 * Output I/O callback sets
 */
typedef struct _xmlOutputCallback {
    xmlOutputMatchCallback matchcallback;
    xmlOutputOpenCallback opencallback;
    xmlOutputWriteCallback writecallback;
    xmlOutputCloseCallback closecallback;
} xmlOutputCallback;

#define MAX_OUTPUT_CALLBACK 15

static xmlOutputCallback xmlOutputCallbackTable[MAX_OUTPUT_CALLBACK];
static int xmlOutputCallbackNr = 0;
static int xmlOutputCallbackInitialized = 0;

/************************************************************************
 *									*
 *		Handling of Windows file paths				*
 *									*
 ************************************************************************/

#define IS_WINDOWS_PATH(p) 					\
	((p != NULL) &&						\
	 (((p[0] >= 'a') && (p[0] <= 'z')) ||			\
	  ((p[0] >= 'A') && (p[0] <= 'Z'))) &&			\
	 (p[1] == ':') && ((p[2] == '/') || (p[2] == '\\')))


/**
 * xmlNormalizeWindowsPath:
 * @path:  a windows path like "C:/foo/bar"
 *
 * Normalize a Windows path to make an URL from it
 *
 * Returns a new URI which must be freed by the caller or NULL
 *   in case of error
 */
xmlChar *
xmlNormalizeWindowsPath(const xmlChar *path)
{
    int len, i = 0, j;
    xmlChar *ret;

    if (path == NULL)
	return(NULL);

    len = xmlStrlen(path);
    if (!IS_WINDOWS_PATH(path)) {
	ret = xmlStrdup(path);
	if (ret == NULL)
	    return(NULL);
	j = 0;
    } else {
	ret = xmlMalloc(len + 10);
	if (ret == NULL)
	    return(NULL);
	ret[0] = 'f';
	ret[1] = 'i';
	ret[2] = 'l';
	ret[3] = 'e';
	ret[4] = ':';
	ret[5] = '/';
	ret[6] = '/';
	ret[7] = '/';
	j = 8;
    }

    while (i < len) {
	/* TODO: UTF8 conversion + URI escaping ??? */
	if (path[i] == '\\')
	    ret[j] = '/';
	else
	    ret[j] = path[i];
	i++;
	j++;
    }
    ret[j] = 0;

    return(ret);
}

/**
 * xmlCleanupInputCallbacks:
 *
 * clears the entire input callback table. this includes the
 * compiled-in I/O. 
 */
void
xmlCleanupInputCallbacks(void)
{
    int i;

    if (!xmlInputCallbackInitialized)
        return;

    for (i = xmlInputCallbackNr - 1; i >= 0; i--) {
        xmlInputCallbackTable[i].matchcallback = NULL;
        xmlInputCallbackTable[i].opencallback = NULL;
        xmlInputCallbackTable[i].readcallback = NULL;
        xmlInputCallbackTable[i].closecallback = NULL;
    }
    xmlInputCallbackInitialized = 0;

    xmlInputCallbackNr = 0;
    xmlInputCallbackInitialized = 0;
}

/**
 * xmlCleanupOutputCallbacks:
 *
 * clears the entire output callback table. this includes the
 * compiled-in I/O callbacks. 
 */
void
xmlCleanupOutputCallbacks(void)
{
    int i;

    if (!xmlOutputCallbackInitialized)
        return;

    for (i = xmlOutputCallbackNr - 1; i >= 0; i--) {
        xmlOutputCallbackTable[i].matchcallback = NULL;
        xmlOutputCallbackTable[i].opencallback = NULL;
        xmlOutputCallbackTable[i].writecallback = NULL;
        xmlOutputCallbackTable[i].closecallback = NULL;
    }
    xmlOutputCallbackInitialized = 0;

    xmlOutputCallbackNr = 0;
    xmlOutputCallbackInitialized = 0;
}

/************************************************************************
 *									*
 *		Standard I/O for file accesses				*
 *									*
 ************************************************************************/

/**
 * xmlCheckFilename:
 * @path:  the path to check
 *
 * function checks to see if @path is a valid source
 * (file, socket...) for XML.
 *
 * if stat is not available on the target machine,
 * returns 1.  if stat fails, returns 0 (if calling
 * stat on the filename fails, it can't be right).
 * if stat succeeds and the file is a directory,
 * returns 2.  otherwise returns 1.
 */

int
xmlCheckFilename (const char *path)
{
#ifdef HAVE_STAT
    struct stat stat_buffer;

    if (stat(path, &stat_buffer) == -1)
        return 0;

#ifdef S_ISDIR
    if (S_ISDIR(stat_buffer.st_mode)) {
        return 2;
    }
#endif
#endif
    return 1;
}

static int
xmlNop(void) {
    return(0);
}

/**
 * xmlFdRead:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to read
 *
 * Read @len bytes to @buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlFdRead (void * context, char * buffer, int len) {
    return(read((int) (long) context, &buffer[0], len));
}

/**
 * xmlFdWrite:
 * @context:  the I/O context
 * @buffer:  where to get data
 * @len:  number of bytes to write
 *
 * Write @len bytes from @buffer to the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlFdWrite (void * context, const char * buffer, int len) {
    return(write((int) (long) context, &buffer[0], len));
}

/**
 * xmlFdClose:
 * @context:  the I/O context
 *
 * Close an I/O channel
 *
 * Returns 0 in case of success and error code otherwise
 */
static int
xmlFdClose (void * context) {
    return ( close((int) (long) context) );
}

/**
 * xmlFileMatch:
 * @filename:  the URI for matching
 *
 * input from FILE *
 *
 * Returns 1 if matches, 0 otherwise
 */
int
xmlFileMatch (const char *filename ATTRIBUTE_UNUSED) {
    return(1);
}

/**
 * xmlFileOpen:
 * @filename:  the URI for matching
 *
 * input from FILE *, supports compressed input
 * if @filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 */
void *
xmlFileOpen (const char *filename) {
    const char *path = NULL;
    FILE *fd;

    if (!strcmp(filename, "-")) {
	fd = stdin;
	return((void *) fd);
    }

    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &filename[17];
#else
	path = &filename[16];
#endif
    else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8)) {
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &filename[8];
#else
	path = &filename[7];
#endif
    } else 
	path = filename;

    if (path == NULL)
	return(NULL);
    if (!xmlCheckFilename(path))
        return(NULL);

#if defined(WIN32) || defined (__CYGWIN__)
    fd = fopen(path, "rb");
#else
    fd = fopen(path, "r");
#endif /* WIN32 */
    return((void *) fd);
}

/**
 * xmlFileOpenW:
 * @filename:  the URI for matching
 *
 * output to from FILE *,
 * if @filename is "-" then the standard output is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void *
xmlFileOpenW (const char *filename) {
    const char *path = NULL;
    FILE *fd;

    if (!strcmp(filename, "-")) {
	fd = stdout;
	return((void *) fd);
    }

    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &filename[17];
#else
	path = &filename[16];
#endif
    else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8)) {
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &filename[8];
#else
	path = &filename[7];
#endif
    } else 
	path = filename;

    if (path == NULL)
	return(NULL);

    fd = fopen(path, "w");
    return((void *) fd);
}

/**
 * xmlFileRead:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Read @len bytes to @buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
int
xmlFileRead (void * context, char * buffer, int len) {
    return(fread(&buffer[0], 1,  len, (FILE *) context));
}

/**
 * xmlFileWrite:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Write @len bytes from @buffer to the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlFileWrite (void * context, const char * buffer, int len) {
    return(fwrite(&buffer[0], 1,  len, (FILE *) context));
}

/**
 * xmlFileClose:
 * @context:  the I/O context
 *
 * Close an I/O channel
 *
 * Returns 0 or -1 in case of error
 */
int
xmlFileClose (void * context) {
    FILE *fil;

    fil = (FILE *) context;
    if (fil == stdin)
	return(0);
    if (fil == stdout)
	return(0);
    if (fil == stderr)
	return(0);
    return ( ( fclose((FILE *) context) == EOF ) ? -1 : 0 );
}

/**
 * xmlFileFlush:
 * @context:  the I/O context
 *
 * Flush an I/O channel
 */
static int
xmlFileFlush (void * context) {
    return ( ( fflush((FILE *) context) == EOF ) ? -1 : 0 );
}

#ifdef HAVE_ZLIB_H
/************************************************************************
 *									*
 *		I/O for compressed file accesses			*
 *									*
 ************************************************************************/
/**
 * xmlGzfileMatch:
 * @filename:  the URI for matching
 *
 * input from compressed file test
 *
 * Returns 1 if matches, 0 otherwise
 */
static int
xmlGzfileMatch (const char *filename ATTRIBUTE_UNUSED) {
    return(1);
}

/**
 * xmlGzfileOpen:
 * @filename:  the URI for matching
 *
 * input from compressed file open
 * if @filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void *
xmlGzfileOpen (const char *filename) {
    const char *path = NULL;
    gzFile fd;

    if (!strcmp(filename, "-")) {
        fd = gzdopen(dup(0), "rb");
	return((void *) fd);
    }

    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &filename[17];
#else
	path = &filename[16];
#endif
    else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8)) {
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &filename[8];
#else
	path = &filename[7];
#endif
    } else 
	path = filename;

    if (path == NULL)
	return(NULL);
    if (!xmlCheckFilename(path))
        return(NULL);

    fd = gzopen(path, "rb");
    return((void *) fd);
}

/**
 * xmlGzfileOpenW:
 * @filename:  the URI for matching
 * @compression:  the compression factor (0 - 9 included)
 *
 * input from compressed file open
 * if @filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void *
xmlGzfileOpenW (const char *filename, int compression) {
    const char *path = NULL;
    char mode[15];
    gzFile fd;

    snprintf(mode, sizeof(mode), "wb%d", compression);
    if (!strcmp(filename, "-")) {
        fd = gzdopen(dup(1), mode);
	return((void *) fd);
    }

    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &filename[17];
#else
	path = &filename[16];
#endif
    else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8)) {
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &filename[8];
#else
	path = &filename[7];
#endif
    } else 
	path = filename;

    if (path == NULL)
	return(NULL);

    fd = gzopen(path, mode);
    return((void *) fd);
}

/**
 * xmlGzfileRead:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Read @len bytes to @buffer from the compressed I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlGzfileRead (void * context, char * buffer, int len) {
    return(gzread((gzFile) context, &buffer[0], len));
}

/**
 * xmlGzfileWrite:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Write @len bytes from @buffer to the compressed I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlGzfileWrite (void * context, const char * buffer, int len) {
    return(gzwrite((gzFile) context, (char *) &buffer[0], len));
}

/**
 * xmlGzfileClose:
 * @context:  the I/O context
 *
 * Close a compressed I/O channel
 */
static int
xmlGzfileClose (void * context) {
    return ( ( gzclose((gzFile) context) == Z_OK ) ? 0 : -1 );
}
#endif /* HAVE_ZLIB_H */

#ifdef LIBXML_HTTP_ENABLED
/************************************************************************
 *									*
 *			I/O for HTTP file accesses			*
 *									*
 ************************************************************************/

typedef struct xmlIOHTTPWriteCtxt_
{
    int			compression;

    char *		uri;

    void *		doc_buff;

} xmlIOHTTPWriteCtxt, *xmlIOHTTPWriteCtxtPtr;

#ifdef HAVE_ZLIB_H

#define DFLT_WBITS		( -15 )
#define DFLT_MEM_LVL		( 8 )
#define GZ_MAGIC1		( 0x1f )
#define GZ_MAGIC2		( 0x8b )
#define LXML_ZLIB_OS_CODE	( 0x03 )
#define INIT_HTTP_BUFF_SIZE	( 32768 )
#define DFLT_ZLIB_RATIO		( 5 )

/*
**  Data structure and functions to work with sending compressed data
**  via HTTP.
*/

typedef struct xmlZMemBuff_
{
   unsigned long	size;
   unsigned long	crc;

   unsigned char *	zbuff;
   z_stream		zctrl;

} xmlZMemBuff, *xmlZMemBuffPtr;

/**
 * append_reverse_ulong
 * @buff:  Compressed memory buffer
 * @data:  Unsigned long to append
 *
 * Append a unsigned long in reverse byte order to the end of the
 * memory buffer.
 */
static void
append_reverse_ulong( xmlZMemBuff * buff, unsigned long data ) {

    int		idx;

    if ( buff == NULL )
	return;

    /*
    **  This is plagiarized from putLong in gzio.c (zlib source) where
    **  the number "4" is hardcoded.  If zlib is ever patched to 
    **  support 64 bit file sizes, this code would need to be patched
    **  as well.
    */

    for ( idx = 0; idx < 4; idx++ ) {
	*buff->zctrl.next_out = ( data & 0xff );
	data >>= 8;
	buff->zctrl.next_out++;
    }

    return;
}

/**
 *
 * xmlFreeZMemBuff
 * @buff:  The memory buffer context to clear
 *
 * Release all the resources associated with the compressed memory buffer.
 */
static void
xmlFreeZMemBuff( xmlZMemBuffPtr buff ) {
    
    int z_err;

    if ( buff == NULL )
	return;

    xmlFree( buff->zbuff );
    z_err = deflateEnd( &buff->zctrl );
#ifdef DEBUG_HTTP
    if ( z_err != Z_OK )
	xmlGenericError( xmlGenericErrorContext,
			"xmlFreeZMemBuff:  Error releasing zlib context:  %d\n",
			z_err );
#endif

    xmlFree( buff );
    return;
}

/**
 * xmlCreateZMemBuff
 *@compression:	Compression value to use
 *
 * Create a memory buffer to hold the compressed XML document.  The
 * compressed document in memory will end up being identical to what
 * would be created if gzopen/gzwrite/gzclose were being used to 
 * write the document to disk.  The code for the header/trailer data to
 * the compression is plagiarized from the zlib source files.
 */
static void *
xmlCreateZMemBuff( int compression ) {

    int			z_err;
    int			hdr_lgth;
    xmlZMemBuffPtr	buff = NULL;

    if ( ( compression < 1 ) || ( compression > 9 ) )
	return ( NULL );

    /*  Create the control and data areas  */

    buff = xmlMalloc( sizeof( xmlZMemBuff ) );
    if ( buff == NULL ) {
	xmlGenericError( xmlGenericErrorContext,
			"xmlCreateZMemBuff:  %s\n",
			"Failure allocating buffer context." );
	return ( NULL );
    }

    (void)memset( buff, 0, sizeof( xmlZMemBuff ) );
    buff->size = INIT_HTTP_BUFF_SIZE;
    buff->zbuff = xmlMalloc( buff->size );
    if ( buff->zbuff == NULL ) {
	xmlFreeZMemBuff( buff );
	xmlGenericError( xmlGenericErrorContext,
			"xmlCreateZMemBuff:  %s\n",
			"Failure allocating data buffer." );
	return ( NULL );
    }

    z_err = deflateInit2( &buff->zctrl, compression, Z_DEFLATED,
			    DFLT_WBITS, DFLT_MEM_LVL, Z_DEFAULT_STRATEGY );
    if ( z_err != Z_OK ) {
	xmlFreeZMemBuff( buff );
	buff = NULL;
	xmlGenericError( xmlGenericErrorContext,
			"xmlCreateZMemBuff:  %s %d\n",
			"Error initializing compression context.  ZLIB error:",
			z_err );
	return ( NULL );
    }

    /*  Set the header data.  The CRC will be needed for the trailer  */
    buff->crc = crc32( 0L, Z_NULL, 0 );
    hdr_lgth = snprintf( (char *)buff->zbuff, buff->size,
			"%c%c%c%c%c%c%c%c%c%c",
			GZ_MAGIC1, GZ_MAGIC2, Z_DEFLATED, 
			0, 0, 0, 0, 0, 0, LXML_ZLIB_OS_CODE );
    buff->zctrl.next_out  = buff->zbuff + hdr_lgth;
    buff->zctrl.avail_out = buff->size - hdr_lgth;

    return ( buff );
}

/**
 * xmlZMemBuffExtend
 * @buff:  Buffer used to compress and consolidate data.
 * @ext_amt:   Number of bytes to extend the buffer.
 *
 * Extend the internal buffer used to store the compressed data by the
 * specified amount.
 *
 * Returns 0 on success or -1 on failure to extend the buffer.  On failure
 * the original buffer still exists at the original size.
 */
static int
xmlZMemBuffExtend( xmlZMemBuffPtr buff, size_t ext_amt ) {

    int			rc = -1;
    size_t		new_size;
    size_t		cur_used;

    unsigned char *	tmp_ptr = NULL;

    if ( buff == NULL )
	return ( -1 );

    else if ( ext_amt == 0 )
	return ( 0 );

    cur_used = buff->zctrl.next_out - buff->zbuff;
    new_size = buff->size + ext_amt;

#ifdef DEBUG_HTTP
    if ( cur_used > new_size ) 
	xmlGenericError( xmlGenericErrorContext,
			"xmlZMemBuffExtend:  %s\n%s %d bytes.\n",
			"Buffer overwrite detected during compressed memory",
			"buffer extension.  Overflowed by", 
			(cur_used - new_size ) );
#endif

    tmp_ptr = xmlRealloc( buff->zbuff, new_size );
    if ( tmp_ptr != NULL ) {
	rc = 0;
	buff->size  = new_size;
	buff->zbuff = tmp_ptr;
	buff->zctrl.next_out  = tmp_ptr + cur_used;
	buff->zctrl.avail_out = new_size - cur_used;
    }
    else {
	xmlGenericError( xmlGenericErrorContext,
			"xmlZMemBuffExtend:  %s %lu bytes.\n",
			"Allocation failure extending output buffer to",
			new_size );
    }

    return ( rc );
}

/**
 * xmlZMemBuffAppend
 * @buff:  Buffer used to compress and consolidate data
 * @src:   Uncompressed source content to append to buffer
 * @len:   Length of source data to append to buffer
 *
 * Compress and append data to the internal buffer.  The data buffer
 * will be expanded if needed to store the additional data.
 *
 * Returns the number of bytes appended to the buffer or -1 on error.
 */
static int
xmlZMemBuffAppend( xmlZMemBuffPtr buff, const char * src, int len ) {

    int		z_err;
    size_t	min_accept;

    if ( ( buff == NULL ) || ( src == NULL ) )
	return ( -1 );

    buff->zctrl.avail_in = len;
    buff->zctrl.next_in  = (unsigned char *)src;
    while ( buff->zctrl.avail_in > 0 ) {
	/*
	**  Extend the buffer prior to deflate call if a reasonable amount
	**  of output buffer space is not available.
	*/
	min_accept = buff->zctrl.avail_in / DFLT_ZLIB_RATIO;
	if ( buff->zctrl.avail_out <= min_accept ) {
	    if ( xmlZMemBuffExtend( buff, buff->size ) == -1 )
		return ( -1 );
	}

	z_err = deflate( &buff->zctrl, Z_NO_FLUSH );
	if ( z_err != Z_OK ) {
	    xmlGenericError( xmlGenericErrorContext,
			"xmlZMemBuffAppend:  %s %d %s - %d",
			"Compression error while appending",
			len, "bytes to buffer.  ZLIB error", z_err );
	    return ( -1 );
	}
    }

    buff->crc = crc32( buff->crc, (unsigned char *)src, len );

    return ( len );
}

/**
 * xmlZMemBuffGetContent
 * @buff:  Compressed memory content buffer
 * @data_ref:  Pointer reference to point to compressed content
 *
 * Flushes the compression buffers, appends gzip file trailers and
 * returns the compressed content and length of the compressed data.
 * NOTE:  The gzip trailer code here is plagiarized from zlib source.
 *
 * Returns the length of the compressed data or -1 on error.
 */
static int
xmlZMemBuffGetContent( xmlZMemBuffPtr buff, char ** data_ref ) {

    int		zlgth = -1;
    int		z_err;

    if ( ( buff == NULL ) || ( data_ref == NULL ) )
	return ( -1 );

    /*  Need to loop until compression output buffers are flushed  */

    do
    {
	z_err = deflate( &buff->zctrl, Z_FINISH );
	if ( z_err == Z_OK ) {
	    /*  In this case Z_OK means more buffer space needed  */

	    if ( xmlZMemBuffExtend( buff, buff->size ) == -1 )
		return ( -1 );
	}
    }
    while ( z_err == Z_OK );

    /*  If the compression state is not Z_STREAM_END, some error occurred  */

    if ( z_err == Z_STREAM_END ) {

	/*  Need to append the gzip data trailer  */

	if ( buff->zctrl.avail_out < ( 2 * sizeof( unsigned long ) ) ) {
	    if ( xmlZMemBuffExtend(buff, (2 * sizeof(unsigned long))) == -1 )
		return ( -1 );
	}

	/*
	**  For whatever reason, the CRC and length data are pushed out
	**  in reverse byte order.  So a memcpy can't be used here.
	*/

	append_reverse_ulong( buff, buff->crc );
	append_reverse_ulong( buff, buff->zctrl.total_in );

	zlgth = buff->zctrl.next_out - buff->zbuff;
	*data_ref = (char *)buff->zbuff;
    }

    else
	xmlGenericError( xmlGenericErrorContext,
			"xmlZMemBuffGetContent:  %s - %d\n",
			"Error flushing zlib buffers.  Error code", z_err );
    
    return ( zlgth );
}
#endif  /*  HAVE_ZLIB_H  */

/**
 * xmlFreeHTTPWriteCtxt
 * @ctxt:  Context to cleanup
 *
 * Free allocated memory and reclaim system resources.
 *
 * No return value.
 */
static void
xmlFreeHTTPWriteCtxt( xmlIOHTTPWriteCtxtPtr ctxt )
{
    if ( ctxt->uri != NULL )
	xmlFree( ctxt->uri );

    if ( ctxt->doc_buff != NULL ) {

#ifdef HAVE_ZLIB_H
	if ( ctxt->compression > 0 ) {
	    xmlFreeZMemBuff( ctxt->doc_buff );
	}
	else
#endif
	{
	    xmlOutputBufferClose( ctxt->doc_buff );
	}
    }

    xmlFree( ctxt );
    return;
}


/**
 * xmlIOHTTPMatch:
 * @filename:  the URI for matching
 *
 * check if the URI matches an HTTP one
 *
 * Returns 1 if matches, 0 otherwise
 */
int
xmlIOHTTPMatch (const char *filename) {
    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "http://", 7))
	return(1);
    return(0);
}

/**
 * xmlIOHTTPOpen:
 * @filename:  the URI for matching
 *
 * open an HTTP I/O channel
 *
 * Returns an I/O context or NULL in case of error
 */
void *
xmlIOHTTPOpen (const char *filename) {
    return(xmlNanoHTTPOpen(filename, NULL));
}

/**
 * xmlIOHTTPOpenW:
 * @post_uri:  The destination URI for the document
 * @compression:  The compression desired for the document.
 *
 * Open a temporary buffer to collect the document for a subsequent HTTP POST
 * request.  Non-static as is called from the output buffer creation routine.
 *
 * Returns an I/O context or NULL in case of error.
 */

void *
xmlIOHTTPOpenW(const char *post_uri, int compression)
{

    xmlIOHTTPWriteCtxtPtr ctxt = NULL;

    if (post_uri == NULL)
        return (NULL);

    ctxt = xmlMalloc(sizeof(xmlIOHTTPWriteCtxt));
    if (ctxt == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		    "xmlIOHTTPOpenW:  Failed to create output HTTP context.\n");
        return (NULL);
    }

    (void) memset(ctxt, 0, sizeof(xmlIOHTTPWriteCtxt));

    ctxt->uri = (char *) xmlStrdup((const xmlChar *)post_uri);
    if (ctxt->uri == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		    "xmlIOHTTPOpenW:  Failed to duplicate destination URI.\n");
        xmlFreeHTTPWriteCtxt(ctxt);
        return (NULL);
    }

    /*
     * **  Since the document length is required for an HTTP post,
     * **  need to put the document into a buffer.  A memory buffer
     * **  is being used to avoid pushing the data to disk and back.
     */

#ifdef HAVE_ZLIB_H
    if ((compression > 0) && (compression <= 9)) {

        ctxt->compression = compression;
        ctxt->doc_buff = xmlCreateZMemBuff(compression);
    } else
#endif
    {
        /*  Any character conversions should have been done before this  */

        ctxt->doc_buff = xmlAllocOutputBuffer(NULL);
    }

    if (ctxt->doc_buff == NULL) {
        xmlFreeHTTPWriteCtxt(ctxt);
        ctxt = NULL;
    }

    return (ctxt);
}
				
/**
 * xmlIOHTTPDfltOpenW
 * @post_uri:  The destination URI for this document.
 *
 * Calls xmlIOHTTPOpenW with no compression to set up for a subsequent
 * HTTP post command.  This function should generally not be used as
 * the open callback is short circuited in xmlOutputBufferCreateFile.
 *
 * Returns a pointer to the new IO context.
 */
static void *
xmlIOHTTPDfltOpenW( const char * post_uri ) {
    return ( xmlIOHTTPOpenW( post_uri, 0 ) );
}

/**
 * xmlIOHTTPRead:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Read @len bytes to @buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
int 
xmlIOHTTPRead(void * context, char * buffer, int len) {
    return(xmlNanoHTTPRead(context, &buffer[0], len));
}

/**
 * xmlIOHTTPWrite
 * @context:  previously opened writing context
 * @buffer:   data to output to temporary buffer
 * @len:      bytes to output
 *
 * Collect data from memory buffer into a temporary file for later
 * processing.
 *
 * Returns number of bytes written.
 */

static int
xmlIOHTTPWrite( void * context, const char * buffer, int len ) { 

    xmlIOHTTPWriteCtxtPtr	ctxt = context;

    if ( ( ctxt == NULL ) || ( ctxt->doc_buff == NULL ) || ( buffer == NULL ) )
	return ( -1 );

    if ( len > 0 ) {

	/*  Use gzwrite or fwrite as previously setup in the open call  */

#ifdef HAVE_ZLIB_H
	if ( ctxt->compression > 0 ) 
	    len = xmlZMemBuffAppend( ctxt->doc_buff, buffer, len );

	else
#endif
	    len = xmlOutputBufferWrite( ctxt->doc_buff, len, buffer );

	if ( len < 0 ) {
	    xmlGenericError( xmlGenericErrorContext,
			"xmlIOHTTPWrite:  %s\n%s '%s'.\n",
			"Error appending to internal buffer.",
			"Error sending document to URI",
			ctxt->uri );
	}
    }

    return ( len );
}


/**
 * xmlIOHTTPClose:
 * @context:  the I/O context
 *
 * Close an HTTP I/O channel
 *
 * Returns 0
 */
int
xmlIOHTTPClose (void * context) {
    xmlNanoHTTPClose(context);
    return 0;
}

/**
 * xmlIOHTTCloseWrite
 * @context:  The I/O context
 * @http_mthd: The HTTP method to be used when sending the data
 *
 * Close the transmit HTTP I/O channel and actually send the data.
 */
static int
xmlIOHTTPCloseWrite( void * context, const char * http_mthd ) {

    int				close_rc = -1;
    int				http_rtn = 0;
    int				content_lgth = 0;
    xmlIOHTTPWriteCtxtPtr	ctxt = context;

    char *			http_content = NULL;
    char *			content_encoding = NULL;
    char *			content_type = (char *) "text/xml";
    void *			http_ctxt = NULL;

    if ( ( ctxt == NULL ) || ( http_mthd == NULL ) )
	return ( -1 );

    /*  Retrieve the content from the appropriate buffer  */

#ifdef HAVE_ZLIB_H

    if ( ctxt->compression > 0 ) {
	content_lgth = xmlZMemBuffGetContent( ctxt->doc_buff, &http_content );
	content_encoding = (char *) "Content-Encoding: gzip";
    }
    else
#endif
    {
	/*  Pull the data out of the memory output buffer  */

	xmlOutputBufferPtr	dctxt = ctxt->doc_buff;
	http_content = (char *)dctxt->buffer->content;
	content_lgth = dctxt->buffer->use;
    }

    if ( http_content == NULL ) {
	xmlGenericError( xmlGenericErrorContext,
			"xmlIOHTTPCloseWrite:  %s '%s' %s '%s'.\n",
			"Error retrieving content.\nUnable to",
			http_mthd, "data to URI", ctxt->uri );
    }

    else {

	http_ctxt = xmlNanoHTTPMethod( ctxt->uri, http_mthd, http_content,
					&content_type, content_encoding, 
					content_lgth );

	if ( http_ctxt != NULL ) {
#ifdef DEBUG_HTTP
	    /*  If testing/debugging - dump reply with request content  */

	    FILE *	tst_file = NULL;
	    char	buffer[ 4096 ];
	    char *	dump_name = NULL;
	    int		avail;

	    xmlGenericError( xmlGenericErrorContext,
			"xmlNanoHTTPCloseWrite:  HTTP %s to\n%s returned %d.\n",
			http_mthd, ctxt->uri,
			xmlNanoHTTPReturnCode( http_ctxt ) );

	    /*
	    **  Since either content or reply may be gzipped,
	    **  dump them to separate files instead of the 
	    **  standard error context.
	    */

	    dump_name = tempnam( NULL, "lxml" );
	    if ( dump_name != NULL ) {
		(void)snprintf( buffer, sizeof(buffer), "%s.content", dump_name );

		tst_file = fopen( buffer, "w" );
		if ( tst_file != NULL ) {
		    xmlGenericError( xmlGenericErrorContext,
			"Transmitted content saved in file:  %s\n", buffer );

		    fwrite( http_content, sizeof( char ),
					content_lgth, tst_file );
		    fclose( tst_file );
		}

		(void)snprintf( buffer, sizeof(buffer), "%s.reply", dump_name );
		tst_file = fopen( buffer, "w" );
		if ( tst_file != NULL ) {
		    xmlGenericError( xmlGenericErrorContext,
			"Reply content saved in file:  %s\n", buffer );


		    while ( (avail = xmlNanoHTTPRead( http_ctxt,
					buffer, sizeof( buffer ) )) > 0 ) {

			fwrite( buffer, sizeof( char ), avail, tst_file );
		    }

		    fclose( tst_file );
		}

		free( dump_name );
	    }
#endif  /*  DEBUG_HTTP  */

	    http_rtn = xmlNanoHTTPReturnCode( http_ctxt );
	    if ( ( http_rtn >= 200 ) && ( http_rtn < 300 ) )
		close_rc = 0;
	    else
		xmlGenericError( xmlGenericErrorContext,
			    "xmlIOHTTPCloseWrite: HTTP '%s' of %d %s\n'%s' %s %d\n",
			    http_mthd, content_lgth,
			    "bytes to URI", ctxt->uri,
			    "failed.  HTTP return code:", http_rtn );

	    xmlNanoHTTPClose( http_ctxt );
	    xmlFree( content_type );
	}
    }

    /*  Final cleanups  */

    xmlFreeHTTPWriteCtxt( ctxt );

    return ( close_rc );
}

/**
 * xmlIOHTTPClosePut
 *
 * @context:  The I/O context
 *
 * Close the transmit HTTP I/O channel and actually send data using a PUT
 * HTTP method.
 */
static int
xmlIOHTTPClosePut( void * ctxt ) {
    return ( xmlIOHTTPCloseWrite( ctxt, "PUT" ) );
}


/**
 * xmlIOHTTPClosePost
 *
 * @context:  The I/O context
 *
 * Close the transmit HTTP I/O channel and actually send data using a POST
 * HTTP method.
 */
static int
xmlIOHTTPClosePost( void * ctxt ) {
    return ( xmlIOHTTPCloseWrite( ctxt, "POST" ) );
}

#endif /* LIBXML_HTTP_ENABLED */

#ifdef LIBXML_FTP_ENABLED
/************************************************************************
 *									*
 *			I/O for FTP file accesses			*
 *									*
 ************************************************************************/
/**
 * xmlIOFTPMatch:
 * @filename:  the URI for matching
 *
 * check if the URI matches an FTP one
 *
 * Returns 1 if matches, 0 otherwise
 */
int
xmlIOFTPMatch (const char *filename) {
    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "ftp://", 6))
	return(1);
    return(0);
}

/**
 * xmlIOFTPOpen:
 * @filename:  the URI for matching
 *
 * open an FTP I/O channel
 *
 * Returns an I/O context or NULL in case of error
 */
void *
xmlIOFTPOpen (const char *filename) {
    return(xmlNanoFTPOpen(filename));
}

/**
 * xmlIOFTPRead:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Read @len bytes to @buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
int 
xmlIOFTPRead(void * context, char * buffer, int len) {
    return(xmlNanoFTPRead(context, &buffer[0], len));
}

/**
 * xmlIOFTPClose:
 * @context:  the I/O context
 *
 * Close an FTP I/O channel
 *
 * Returns 0
 */
int
xmlIOFTPClose (void * context) {
    return ( xmlNanoFTPClose(context) );
}
#endif /* LIBXML_FTP_ENABLED */


/**
 * xmlRegisterInputCallbacks:
 * @matchFunc:  the xmlInputMatchCallback
 * @openFunc:  the xmlInputOpenCallback
 * @readFunc:  the xmlInputReadCallback
 * @closeFunc:  the xmlInputCloseCallback
 *
 * Register a new set of I/O callback for handling parser input.
 *
 * Returns the registered handler number or -1 in case of error
 */
int
xmlRegisterInputCallbacks(xmlInputMatchCallback matchFunc,
	xmlInputOpenCallback openFunc, xmlInputReadCallback readFunc,
	xmlInputCloseCallback closeFunc) {
    if (xmlInputCallbackNr >= MAX_INPUT_CALLBACK) {
	return(-1);
    }
    xmlInputCallbackTable[xmlInputCallbackNr].matchcallback = matchFunc;
    xmlInputCallbackTable[xmlInputCallbackNr].opencallback = openFunc;
    xmlInputCallbackTable[xmlInputCallbackNr].readcallback = readFunc;
    xmlInputCallbackTable[xmlInputCallbackNr].closecallback = closeFunc;
    return(xmlInputCallbackNr++);
}

/**
 * xmlRegisterOutputCallbacks:
 * @matchFunc:  the xmlOutputMatchCallback
 * @openFunc:  the xmlOutputOpenCallback
 * @writeFunc:  the xmlOutputWriteCallback
 * @closeFunc:  the xmlOutputCloseCallback
 *
 * Register a new set of I/O callback for handling output.
 *
 * Returns the registered handler number or -1 in case of error
 */
int
xmlRegisterOutputCallbacks(xmlOutputMatchCallback matchFunc,
	xmlOutputOpenCallback openFunc, xmlOutputWriteCallback writeFunc,
	xmlOutputCloseCallback closeFunc) {
    if (xmlOutputCallbackNr >= MAX_INPUT_CALLBACK) {
	return(-1);
    }
    xmlOutputCallbackTable[xmlOutputCallbackNr].matchcallback = matchFunc;
    xmlOutputCallbackTable[xmlOutputCallbackNr].opencallback = openFunc;
    xmlOutputCallbackTable[xmlOutputCallbackNr].writecallback = writeFunc;
    xmlOutputCallbackTable[xmlOutputCallbackNr].closecallback = closeFunc;
    return(xmlOutputCallbackNr++);
}

/**
 * xmlRegisterDefaultInputCallbacks:
 *
 * Registers the default compiled-in I/O handlers.
 */
void
xmlRegisterDefaultInputCallbacks
(void) {
    if (xmlInputCallbackInitialized)
	return;

    xmlRegisterInputCallbacks(xmlFileMatch, xmlFileOpen,
	                      xmlFileRead, xmlFileClose);
#ifdef HAVE_ZLIB_H
    xmlRegisterInputCallbacks(xmlGzfileMatch, xmlGzfileOpen,
	                      xmlGzfileRead, xmlGzfileClose);
#endif /* HAVE_ZLIB_H */

#ifdef LIBXML_HTTP_ENABLED
    xmlRegisterInputCallbacks(xmlIOHTTPMatch, xmlIOHTTPOpen,
	                      xmlIOHTTPRead, xmlIOHTTPClose);
#endif /* LIBXML_HTTP_ENABLED */

#ifdef LIBXML_FTP_ENABLED
    xmlRegisterInputCallbacks(xmlIOFTPMatch, xmlIOFTPOpen,
	                      xmlIOFTPRead, xmlIOFTPClose);
#endif /* LIBXML_FTP_ENABLED */
    xmlInputCallbackInitialized = 1;
}

/**
 * xmlRegisterDefaultOutputCallbacks:
 *
 * Registers the default compiled-in I/O handlers.
 */
void
xmlRegisterDefaultOutputCallbacks
(void) {
    if (xmlOutputCallbackInitialized)
	return;

    xmlRegisterOutputCallbacks(xmlFileMatch, xmlFileOpenW,
	                      xmlFileWrite, xmlFileClose);

#ifdef LIBXML_HTTP_ENABLED
    xmlRegisterOutputCallbacks(xmlIOHTTPMatch, xmlIOHTTPDfltOpenW,
	                       xmlIOHTTPWrite, xmlIOHTTPClosePut);
#endif

/*********************************
 No way a-priori to distinguish between gzipped files from
 uncompressed ones except opening if existing then closing
 and saving with same compression ratio ... a pain.

#ifdef HAVE_ZLIB_H
    xmlRegisterOutputCallbacks(xmlGzfileMatch, xmlGzfileOpen,
	                       xmlGzfileWrite, xmlGzfileClose);
#endif

 Nor FTP PUT ....
#ifdef LIBXML_FTP_ENABLED
    xmlRegisterOutputCallbacks(xmlIOFTPMatch, xmlIOFTPOpen,
	                       xmlIOFTPWrite, xmlIOFTPClose);
#endif
 **********************************/
    xmlOutputCallbackInitialized = 1;
}

#ifdef LIBXML_HTTP_ENABLED
/**
 * xmlRegisterHTTPPostCallbacks:
 *
 * By default, libxml submits HTTP output requests using the "PUT" method.
 * Calling this method changes the HTTP output method to use the "POST"
 * method instead.
 *
 */
void
xmlRegisterHTTPPostCallbacks( void ) {

    /*  Register defaults if not done previously  */

    if ( xmlOutputCallbackInitialized == 0 )
	xmlRegisterDefaultOutputCallbacks( );

    xmlRegisterOutputCallbacks(xmlIOHTTPMatch, xmlIOHTTPDfltOpenW,
	                       xmlIOHTTPWrite, xmlIOHTTPClosePost);
    return;
}
#endif

/**
 * xmlAllocParserInputBuffer:
 * @enc:  the charset encoding if known
 *
 * Create a buffered parser input for progressive parsing
 *
 * Returns the new parser input or NULL
 */
xmlParserInputBufferPtr
xmlAllocParserInputBuffer(xmlCharEncoding enc) {
    xmlParserInputBufferPtr ret;

    ret = (xmlParserInputBufferPtr) xmlMalloc(sizeof(xmlParserInputBuffer));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlAllocParserInputBuffer : out of memory!\n");
	return(NULL);
    }
    memset(ret, 0, (size_t) sizeof(xmlParserInputBuffer));
    ret->buffer = xmlBufferCreate();
    if (ret->buffer == NULL) {
        xmlFree(ret);
	return(NULL);
    }
    ret->buffer->alloc = XML_BUFFER_ALLOC_DOUBLEIT;
    ret->encoder = xmlGetCharEncodingHandler(enc);
    if (ret->encoder != NULL)
        ret->raw = xmlBufferCreate();
    else
        ret->raw = NULL;
    ret->readcallback = NULL;
    ret->closecallback = NULL;
    ret->context = NULL;

    return(ret);
}

/**
 * xmlAllocOutputBuffer:
 * @encoder:  the encoding converter or NULL
 *
 * Create a buffered parser output
 *
 * Returns the new parser output or NULL
 */
xmlOutputBufferPtr
xmlAllocOutputBuffer(xmlCharEncodingHandlerPtr encoder) {
    xmlOutputBufferPtr ret;

    ret = (xmlOutputBufferPtr) xmlMalloc(sizeof(xmlOutputBuffer));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlAllocOutputBuffer : out of memory!\n");
	return(NULL);
    }
    memset(ret, 0, (size_t) sizeof(xmlOutputBuffer));
    ret->buffer = xmlBufferCreate();
    if (ret->buffer == NULL) {
        xmlFree(ret);
	return(NULL);
    }
    ret->buffer->alloc = XML_BUFFER_ALLOC_DOUBLEIT;
    ret->encoder = encoder;
    if (encoder != NULL) {
        ret->conv = xmlBufferCreateSize(4000);
	/*
	 * This call is designed to initiate the encoder state
	 */
	xmlCharEncOutFunc(encoder, ret->conv, NULL); 
    } else
        ret->conv = NULL;
    ret->writecallback = NULL;
    ret->closecallback = NULL;
    ret->context = NULL;
    ret->written = 0;

    return(ret);
}

/**
 * xmlFreeParserInputBuffer:
 * @in:  a buffered parser input
 *
 * Free up the memory used by a buffered parser input
 */
void
xmlFreeParserInputBuffer(xmlParserInputBufferPtr in) {
    if (in->raw) {
        xmlBufferFree(in->raw);
	in->raw = NULL;
    }
    if (in->encoder != NULL) {
        xmlCharEncCloseFunc(in->encoder);
    }
    if (in->closecallback != NULL) {
	in->closecallback(in->context);
    }
    if (in->buffer != NULL) {
        xmlBufferFree(in->buffer);
	in->buffer = NULL;
    }

    xmlFree(in);
}

/**
 * xmlOutputBufferClose:
 * @out:  a buffered output
 *
 * flushes and close the output I/O channel
 * and free up all the associated resources
 *
 * Returns the number of byte written or -1 in case of error.
 */
int
xmlOutputBufferClose(xmlOutputBufferPtr out) {
    int written;
    int err_rc = 0;

    if (out == NULL)
        return(-1);
    if (out->writecallback != NULL)
	xmlOutputBufferFlush(out);
    if (out->closecallback != NULL) {
	err_rc = out->closecallback(out->context);
    }
    written = out->written;
    if (out->conv) {
        xmlBufferFree(out->conv);
	out->conv = NULL;
    }
    if (out->encoder != NULL) {
        xmlCharEncCloseFunc(out->encoder);
    }
    if (out->buffer != NULL) {
        xmlBufferFree(out->buffer);
	out->buffer = NULL;
    }

    xmlFree(out);
    return( ( err_rc == 0 ) ? written : err_rc );
}

/**
 * xmlParserInputBufferCreateFname:
 * @URI:  a C string containing the URI or filename
 * @enc:  the charset encoding if known
 *
 * Returns the new parser input or NULL
 */
/**
 * xmlParserInputBufferCreateFilename:
 * @URI:  a C string containing the URI or filename
 * @enc:  the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing of a file
 * If filename is "-' then we use stdin as the input.
 * Automatic support for ZLIB/Compress compressed document is provided
 * by default if found at compile-time.
 * Do an encoding check if enc == XML_CHAR_ENCODING_NONE
 *
 * Returns the new parser input or NULL
 */
xmlParserInputBufferPtr
xmlParserInputBufferCreateFilename
(const char *URI, xmlCharEncoding enc) {
    xmlParserInputBufferPtr ret;
    int i = 0;
    void *context = NULL;
    char *unescaped;
    char *normalized;

    if (xmlInputCallbackInitialized == 0)
	xmlRegisterDefaultInputCallbacks();

    if (URI == NULL) return(NULL);
    normalized = (char *) xmlNormalizeWindowsPath((const xmlChar *)URI);
    if (normalized == NULL) return(NULL);

#ifdef LIBXML_CATALOG_ENABLED
#endif

    /*
     * Try to find one of the input accept method accepting that scheme
     * Go in reverse to give precedence to user defined handlers.
     * try with an unescaped version of the URI
     */
    unescaped = xmlURIUnescapeString((char *) normalized, 0, NULL);
    if (unescaped != NULL) {
	for (i = xmlInputCallbackNr - 1;i >= 0;i--) {
	    if ((xmlInputCallbackTable[i].matchcallback != NULL) &&
		(xmlInputCallbackTable[i].matchcallback(unescaped) != 0)) {
		context = xmlInputCallbackTable[i].opencallback(unescaped);
		if (context != NULL)
		    break;
	    }
	}
	xmlFree(unescaped);
    }

    /*
     * If this failed try with a non-escaped URI this may be a strange
     * filename
     */
    if (context == NULL) {
	for (i = xmlInputCallbackNr - 1;i >= 0;i--) {
	    if ((xmlInputCallbackTable[i].matchcallback != NULL) &&
		(xmlInputCallbackTable[i].matchcallback(URI) != 0)) {
		context = xmlInputCallbackTable[i].opencallback(normalized);
		if (context != NULL)
		    break;
	    }
	}
    }
    xmlFree(normalized);
    if (context == NULL) {
	return(NULL);
    }

    /*
     * Allocate the Input buffer front-end.
     */
    ret = xmlAllocParserInputBuffer(enc);
    if (ret != NULL) {
	ret->context = context;
	ret->readcallback = xmlInputCallbackTable[i].readcallback;
	ret->closecallback = xmlInputCallbackTable[i].closecallback;
    }
    return(ret);
}

/**
 * xmlOutputBufferCreateFilename:
 * @URI:  a C string containing the URI or filename
 * @encoder:  the encoding converter or NULL
 * @compression:  the compression ration (0 none, 9 max).
 *
 * Create a buffered  output for the progressive saving of a file
 * If filename is "-' then we use stdout as the output.
 * Automatic support for ZLIB/Compress compressed document is provided
 * by default if found at compile-time.
 * TODO: currently if compression is set, the library only support
 *       writing to a local file.
 *
 * Returns the new output or NULL
 */
xmlOutputBufferPtr
xmlOutputBufferCreateFilename(const char *URI,
                              xmlCharEncodingHandlerPtr encoder,
			      int compression) {
    xmlOutputBufferPtr ret;
    int i = 0;
    void *context = NULL;
    char *unescaped;
    char *normalized;

    int is_http_uri = 0;	/*   Can't change if HTTP disabled  */

    if (xmlOutputCallbackInitialized == 0)
	xmlRegisterDefaultOutputCallbacks();

    if (URI == NULL) return(NULL);
    normalized = (char *) xmlNormalizeWindowsPath((const xmlChar *)URI);
    if (normalized == NULL) return(NULL);

#ifdef LIBXML_HTTP_ENABLED
    /*  Need to prevent HTTP URI's from falling into zlib short circuit  */

    is_http_uri = xmlIOHTTPMatch( normalized );
#endif


    /*
     * Try to find one of the output accept method accepting that scheme
     * Go in reverse to give precedence to user defined handlers.
     * try with an unescaped version of the URI
     */
    unescaped = xmlURIUnescapeString(normalized, 0, NULL);
    if (unescaped != NULL) {
#ifdef HAVE_ZLIB_H
	if ((compression > 0) && (compression <= 9) && (is_http_uri == 0)) {
	    context = xmlGzfileOpenW(unescaped, compression);
	    if (context != NULL) {
		ret = xmlAllocOutputBuffer(encoder);
		if (ret != NULL) {
		    ret->context = context;
		    ret->writecallback = xmlGzfileWrite;
		    ret->closecallback = xmlGzfileClose;
		}
		xmlFree(unescaped);
		xmlFree(normalized);
		return(ret);
	    }
	}
#endif
	for (i = xmlOutputCallbackNr - 1;i >= 0;i--) {
	    if ((xmlOutputCallbackTable[i].matchcallback != NULL) &&
		(xmlOutputCallbackTable[i].matchcallback(unescaped) != 0)) {
#if defined(LIBXML_HTTP_ENABLED) && defined(HAVE_ZLIB_H)
		/*  Need to pass compression parameter into HTTP open calls  */
		if (xmlOutputCallbackTable[i].matchcallback == xmlIOHTTPMatch)
		    context = xmlIOHTTPOpenW(unescaped, compression);
		else
#endif
		    context = xmlOutputCallbackTable[i].opencallback(unescaped);
		if (context != NULL)
		    break;
	    }
	}
	xmlFree(unescaped);
    }

    /*
     * If this failed try with a non-escaped URI this may be a strange
     * filename
     */
    if (context == NULL) {
#ifdef HAVE_ZLIB_H
	if ((compression > 0) && (compression <= 9) && (is_http_uri == 0)) {
	    context = xmlGzfileOpenW(normalized, compression);
	    if (context != NULL) {
		ret = xmlAllocOutputBuffer(encoder);
		if (ret != NULL) {
		    ret->context = context;
		    ret->writecallback = xmlGzfileWrite;
		    ret->closecallback = xmlGzfileClose;
		}
		xmlFree(normalized);
		return(ret);
	    }
	}
#endif
	for (i = xmlOutputCallbackNr - 1;i >= 0;i--) {
	    if ((xmlOutputCallbackTable[i].matchcallback != NULL) &&
		(xmlOutputCallbackTable[i].matchcallback(normalized) != 0)) {
#if defined(LIBXML_HTTP_ENABLED) && defined(HAVE_ZLIB_H)
		/*  Need to pass compression parameter into HTTP open calls  */
		if (xmlOutputCallbackTable[i].matchcallback == xmlIOHTTPMatch)
		    context = xmlIOHTTPOpenW(URI, compression);
		else
#endif
		    context = xmlOutputCallbackTable[i].opencallback(URI);
		if (context != NULL)
		    break;
	    }
	}
    }
    xmlFree(normalized);

    if (context == NULL) {
	return(NULL);
    }

    /*
     * Allocate the Output buffer front-end.
     */
    ret = xmlAllocOutputBuffer(encoder);
    if (ret != NULL) {
	ret->context = context;
	ret->writecallback = xmlOutputCallbackTable[i].writecallback;
	ret->closecallback = xmlOutputCallbackTable[i].closecallback;
    }
    return(ret);
}

/**
 * xmlParserInputBufferCreateFile:
 * @file:  a FILE* 
 * @enc:  the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing of a FILE *
 * buffered C I/O
 *
 * Returns the new parser input or NULL
 */
xmlParserInputBufferPtr
xmlParserInputBufferCreateFile(FILE *file, xmlCharEncoding enc) {
    xmlParserInputBufferPtr ret;

    if (xmlInputCallbackInitialized == 0)
	xmlRegisterDefaultInputCallbacks();

    if (file == NULL) return(NULL);

    ret = xmlAllocParserInputBuffer(enc);
    if (ret != NULL) {
        ret->context = file;
	ret->readcallback = xmlFileRead;
	ret->closecallback = xmlFileFlush;
    }

    return(ret);
}

/**
 * xmlOutputBufferCreateFile:
 * @file:  a FILE* 
 * @encoder:  the encoding converter or NULL
 *
 * Create a buffered output for the progressive saving to a FILE *
 * buffered C I/O
 *
 * Returns the new parser output or NULL
 */
xmlOutputBufferPtr
xmlOutputBufferCreateFile(FILE *file, xmlCharEncodingHandlerPtr encoder) {
    xmlOutputBufferPtr ret;

    if (xmlOutputCallbackInitialized == 0)
	xmlRegisterDefaultOutputCallbacks();

    if (file == NULL) return(NULL);

    ret = xmlAllocOutputBuffer(encoder);
    if (ret != NULL) {
        ret->context = file;
	ret->writecallback = xmlFileWrite;
	ret->closecallback = xmlFileFlush;
    }

    return(ret);
}

/**
 * xmlParserInputBufferCreateFd:
 * @fd:  a file descriptor number
 * @enc:  the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing for the input
 * from a file descriptor
 *
 * Returns the new parser input or NULL
 */
xmlParserInputBufferPtr
xmlParserInputBufferCreateFd(int fd, xmlCharEncoding enc) {
    xmlParserInputBufferPtr ret;

    if (fd < 0) return(NULL);

    ret = xmlAllocParserInputBuffer(enc);
    if (ret != NULL) {
        ret->context = (void *) (long) fd;
	ret->readcallback = xmlFdRead;
	ret->closecallback = xmlFdClose;
    }

    return(ret);
}

/**
 * xmlParserInputBufferCreateMem:
 * @mem:  the memory input
 * @size:  the length of the memory block
 * @enc:  the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing for the input
 * from a memory area.
 *
 * Returns the new parser input or NULL
 */
xmlParserInputBufferPtr
xmlParserInputBufferCreateMem(const char *mem, int size, xmlCharEncoding enc) {
    xmlParserInputBufferPtr ret;

    if (size <= 0) return(NULL);
    if (mem == NULL) return(NULL);

    ret = xmlAllocParserInputBuffer(enc);
    if (ret != NULL) {
        ret->context = (void *) mem;
	ret->readcallback = (xmlInputReadCallback) xmlNop;
	ret->closecallback = NULL;
	xmlBufferAdd(ret->buffer, (const xmlChar *) mem, size);
    }

    return(ret);
}

/**
 * xmlOutputBufferCreateFd:
 * @fd:  a file descriptor number
 * @encoder:  the encoding converter or NULL
 *
 * Create a buffered output for the progressive saving 
 * to a file descriptor
 *
 * Returns the new parser output or NULL
 */
xmlOutputBufferPtr
xmlOutputBufferCreateFd(int fd, xmlCharEncodingHandlerPtr encoder) {
    xmlOutputBufferPtr ret;

    if (fd < 0) return(NULL);

    ret = xmlAllocOutputBuffer(encoder);
    if (ret != NULL) {
        ret->context = (void *) (long) fd;
	ret->writecallback = xmlFdWrite;
	ret->closecallback = NULL;
    }

    return(ret);
}

/**
 * xmlParserInputBufferCreateIO:
 * @ioread:  an I/O read function
 * @ioclose:  an I/O close function
 * @ioctx:  an I/O handler
 * @enc:  the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing for the input
 * from an I/O handler
 *
 * Returns the new parser input or NULL
 */
xmlParserInputBufferPtr
xmlParserInputBufferCreateIO(xmlInputReadCallback   ioread,
	 xmlInputCloseCallback  ioclose, void *ioctx, xmlCharEncoding enc) {
    xmlParserInputBufferPtr ret;

    if (ioread == NULL) return(NULL);

    ret = xmlAllocParserInputBuffer(enc);
    if (ret != NULL) {
        ret->context = (void *) ioctx;
	ret->readcallback = ioread;
	ret->closecallback = ioclose;
    }

    return(ret);
}

/**
 * xmlOutputBufferCreateIO:
 * @iowrite:  an I/O write function
 * @ioclose:  an I/O close function
 * @ioctx:  an I/O handler
 * @encoder:  the charset encoding if known
 *
 * Create a buffered output for the progressive saving
 * to an I/O handler
 *
 * Returns the new parser output or NULL
 */
xmlOutputBufferPtr
xmlOutputBufferCreateIO(xmlOutputWriteCallback   iowrite,
	 xmlOutputCloseCallback  ioclose, void *ioctx,
	 xmlCharEncodingHandlerPtr encoder) {
    xmlOutputBufferPtr ret;

    if (iowrite == NULL) return(NULL);

    ret = xmlAllocOutputBuffer(encoder);
    if (ret != NULL) {
        ret->context = (void *) ioctx;
	ret->writecallback = iowrite;
	ret->closecallback = ioclose;
    }

    return(ret);
}

/**
 * xmlParserInputBufferPush:
 * @in:  a buffered parser input
 * @len:  the size in bytes of the array.
 * @buf:  an char array
 *
 * Push the content of the arry in the input buffer
 * This routine handle the I18N transcoding to internal UTF-8
 * This is used when operating the parser in progressive (push) mode.
 *
 * Returns the number of chars read and stored in the buffer, or -1
 *         in case of error.
 */
int
xmlParserInputBufferPush(xmlParserInputBufferPtr in,
	                 int len, const char *buf) {
    int nbchars = 0;

    if (len < 0) return(0);
    if (in->encoder != NULL) {
        /*
	 * Store the data in the incoming raw buffer
	 */
        if (in->raw == NULL) {
	    in->raw = xmlBufferCreate();
	}
	xmlBufferAdd(in->raw, (const xmlChar *) buf, len);

	/*
	 * convert as much as possible to the parser reading buffer.
	 */
	nbchars = xmlCharEncInFunc(in->encoder, in->buffer, in->raw);
	if (nbchars < 0) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlParserInputBufferPush: encoder error\n");
	    return(-1);
	}
    } else {
	nbchars = len;
        xmlBufferAdd(in->buffer, (xmlChar *) buf, nbchars);
    }
#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext,
	    "I/O: pushed %d chars, buffer %d/%d\n",
            nbchars, in->buffer->use, in->buffer->size);
#endif
    return(nbchars);
}

/**
 * endOfInput:
 *
 * When reading from an Input channel indicated end of file or error
 * don't reread from it again.
 */
static int
endOfInput (void * context ATTRIBUTE_UNUSED,
	    char * buffer ATTRIBUTE_UNUSED,
	    int len ATTRIBUTE_UNUSED) {
    return(0);
}

/**
 * xmlParserInputBufferGrow:
 * @in:  a buffered parser input
 * @len:  indicative value of the amount of chars to read
 *
 * Grow up the content of the input buffer, the old data are preserved
 * This routine handle the I18N transcoding to internal UTF-8
 * This routine is used when operating the parser in normal (pull) mode
 *
 * TODO: one should be able to remove one extra copy by copying directly
 *       onto in->buffer or in->raw
 *
 * Returns the number of chars read and stored in the buffer, or -1
 *         in case of error.
 */
int
xmlParserInputBufferGrow(xmlParserInputBufferPtr in, int len) {
    char *buffer = NULL;
    int res = 0;
    int nbchars = 0;
    int buffree;
    unsigned int needSize;

    if ((len <= MINLEN) && (len != 4)) 
        len = MINLEN;
    buffree = in->buffer->size - in->buffer->use;
    if (buffree <= 0) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInputBufferGrow : buffer full !\n");
	return(0);
    }
    if (len > buffree) 
        len = buffree;

    needSize = in->buffer->use + len + 1;
    if (needSize > in->buffer->size){
        if (!xmlBufferResize(in->buffer, needSize)){
            xmlGenericError(xmlGenericErrorContext,
		    "xmlBufferAdd : out of memory!\n");
            return(0);
        }
    }
    buffer = (char *)&in->buffer->content[in->buffer->use];

    /*
     * Call the read method for this I/O type.
     */
    if (in->readcallback != NULL) {
	res = in->readcallback(in->context, &buffer[0], len);
	if (res <= 0)
	    in->readcallback = endOfInput;
    } else {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInputBufferGrow : no input !\n");
	return(-1);
    }
    if (res < 0) {
	return(-1);
    }
    len = res;
    if (in->encoder != NULL) {
        /*
	 * Store the data in the incoming raw buffer
	 */
        if (in->raw == NULL) {
	    in->raw = xmlBufferCreate();
	}
	xmlBufferAdd(in->raw, (const xmlChar *) buffer, len);

	/*
	 * convert as much as possible to the parser reading buffer.
	 */
	nbchars = xmlCharEncInFunc(in->encoder, in->buffer, in->raw);
	if (nbchars < 0) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlParserInputBufferGrow: encoder error\n");
	    return(-1);
	}
    } else {
	nbchars = len;
    	in->buffer->use += nbchars;
	buffer[nbchars] = 0;
    }
#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext,
	    "I/O: read %d chars, buffer %d/%d\n",
            nbchars, in->buffer->use, in->buffer->size);
#endif
    return(nbchars);
}

/**
 * xmlParserInputBufferRead:
 * @in:  a buffered parser input
 * @len:  indicative value of the amount of chars to read
 *
 * Refresh the content of the input buffer, the old data are considered
 * consumed
 * This routine handle the I18N transcoding to internal UTF-8
 *
 * Returns the number of chars read and stored in the buffer, or -1
 *         in case of error.
 */
int
xmlParserInputBufferRead(xmlParserInputBufferPtr in, int len) {
    /* xmlBufferEmpty(in->buffer); */
    if (in->readcallback != NULL)
	return(xmlParserInputBufferGrow(in, len));
    else
        return(-1);
}

/**
 * xmlOutputBufferWrite:
 * @out:  a buffered parser output
 * @len:  the size in bytes of the array.
 * @buf:  an char array
 *
 * Write the content of the array in the output I/O buffer
 * This routine handle the I18N transcoding from internal UTF-8
 * The buffer is lossless, i.e. will store in case of partial
 * or delayed writes.
 *
 * Returns the number of chars immediately written, or -1
 *         in case of error.
 */
int
xmlOutputBufferWrite(xmlOutputBufferPtr out, int len, const char *buf) {
    int nbchars = 0; /* number of chars to output to I/O */
    int ret;         /* return from function call */
    int written = 0; /* number of char written to I/O so far */
    int chunk;       /* number of byte curreent processed from buf */

    if (len < 0) return(0);

    do {
	chunk = len;
	if (chunk > 4 * MINLEN)
	    chunk = 4 * MINLEN;

	/*
	 * first handle encoding stuff.
	 */
	if (out->encoder != NULL) {
	    /*
	     * Store the data in the incoming raw buffer
	     */
	    if (out->conv == NULL) {
		out->conv = xmlBufferCreate();
	    }
	    xmlBufferAdd(out->buffer, (const xmlChar *) buf, chunk);

	    if ((out->buffer->use < MINLEN) && (chunk == len))
		goto done;

	    /*
	     * convert as much as possible to the parser reading buffer.
	     */
	    ret = xmlCharEncOutFunc(out->encoder, out->conv, out->buffer);
	    if (ret < 0) {
		xmlGenericError(xmlGenericErrorContext,
			"xmlOutputBufferWrite: encoder error\n");
		return(-1);
	    }
	    nbchars = out->conv->use;
	} else {
	    xmlBufferAdd(out->buffer, (const xmlChar *) buf, chunk);
	    nbchars = out->buffer->use;
	}
	buf += chunk;
	len -= chunk;

	if ((nbchars < MINLEN) && (len <= 0))
	    goto done;

	if (out->writecallback) {
	    /*
	     * second write the stuff to the I/O channel
	     */
	    if (out->encoder != NULL) {
		ret = out->writecallback(out->context, 
				 (const char *)out->conv->content, nbchars);
		if (ret >= 0)
		    xmlBufferShrink(out->conv, ret);
	    } else {
		ret = out->writecallback(out->context, 
				 (const char *)out->buffer->content, nbchars);
		if (ret >= 0)
		    xmlBufferShrink(out->buffer, ret);
	    }
	    if (ret < 0) {
		xmlGenericError(xmlGenericErrorContext,
			"I/O: error %d writing %d bytes\n", ret, nbchars);
		return(ret);
	    }
	    out->written += ret;
	}
	written += nbchars;
    } while (len > 0);

done:
#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext,
	    "I/O: wrote %d chars\n", written);
#endif
    return(written);
}

/**
 * xmlOutputBufferWriteString:
 * @out:  a buffered parser output
 * @str:  a zero terminated C string
 *
 * Write the content of the string in the output I/O buffer
 * This routine handle the I18N transcoding from internal UTF-8
 * The buffer is lossless, i.e. will store in case of partial
 * or delayed writes.
 *
 * Returns the number of chars immediately written, or -1
 *         in case of error.
 */
int
xmlOutputBufferWriteString(xmlOutputBufferPtr out, const char *str) {
    int len;
    
    if (str == NULL)
        return(-1);
    len = strlen(str);

    if (len > 0)
	return(xmlOutputBufferWrite(out, len, str));
    return(len);
}

/**
 * xmlOutputBufferFlush:
 * @out:  a buffered output
 *
 * flushes the output I/O channel
 *
 * Returns the number of byte written or -1 in case of error.
 */
int
xmlOutputBufferFlush(xmlOutputBufferPtr out) {
    int nbchars = 0, ret = 0;

    /*
     * first handle encoding stuff.
     */
    if ((out->conv != NULL) && (out->encoder != NULL)) {
	/*
	 * convert as much as possible to the parser reading buffer.
	 */
	nbchars = xmlCharEncOutFunc(out->encoder, out->conv, out->buffer);
	if (nbchars < 0) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlOutputBufferFlush: encoder error\n");
	    return(-1);
	}
    }

    /*
     * second flush the stuff to the I/O channel
     */
    if ((out->conv != NULL) && (out->encoder != NULL) &&
	(out->writecallback != NULL)) {
	ret = out->writecallback(out->context,
	           (const char *)out->conv->content, out->conv->use);
	if (ret >= 0)
	    xmlBufferShrink(out->conv, ret);
    } else if (out->writecallback != NULL) {
	ret = out->writecallback(out->context,
	           (const char *)out->buffer->content, out->buffer->use);
	if (ret >= 0)
	    xmlBufferShrink(out->buffer, ret);
    }
    if (ret < 0) {
        xmlGenericError(xmlGenericErrorContext,
		"I/O: error %d flushing %d bytes\n", ret, nbchars);
	return(ret);
    }
    out->written += ret;

#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext,
	    "I/O: flushed %d chars\n", ret);
#endif
    return(ret);
}

/**
 * xmlParserGetDirectory:
 * @filename:  the path to a file
 *
 * lookup the directory for that file
 *
 * Returns a new allocated string containing the directory, or NULL.
 */
char *
xmlParserGetDirectory(const char *filename) {
    char *ret = NULL;
    char dir[1024];
    char *cur;
    char sep = '/';

#ifdef _WIN32_WCE  /* easy way by now ... wince does not have dirs! */
    return NULL;
#endif

    if (xmlInputCallbackInitialized == 0)
	xmlRegisterDefaultInputCallbacks();

    if (filename == NULL) return(NULL);
#if defined(WIN32) && !defined(__CYGWIN__)
    sep = '\\';
#endif

    strncpy(dir, filename, 1023);
    dir[1023] = 0;
    cur = &dir[strlen(dir)];
    while (cur > dir) {
         if (*cur == sep) break;
	 cur --;
    }
    if (*cur == sep) {
        if (cur == dir) dir[1] = 0;
	else *cur = 0;
	ret = xmlMemStrdup(dir);
    } else {
        if (getcwd(dir, 1024) != NULL) {
	    dir[1023] = 0;
	    ret = xmlMemStrdup(dir);
	}
    }
    return(ret);
}

/****************************************************************
 *								*
 *		External entities loading			*
 *								*
 ****************************************************************/

#ifdef LIBXML_CATALOG_ENABLED
static int xmlSysIDExists(const char *URL) {
#ifdef HAVE_STAT
    int ret;
    struct stat info;
    const char *path;

    if (URL == NULL)
	return(0);

    if (!xmlStrncasecmp(BAD_CAST URL, BAD_CAST "file://localhost/", 17))
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &URL[17];
#else
	path = &URL[16];
#endif
    else if (!xmlStrncasecmp(BAD_CAST URL, BAD_CAST "file:///", 8)) {
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &URL[8];
#else
	path = &URL[7];
#endif
    } else 
	path = URL;
    ret = stat(path, &info);
    if (ret == 0)
	return(1);
#endif
    return(0);
}
#endif

/**
 * xmlDefaultExternalEntityLoader:
 * @URL:  the URL for the entity to load
 * @ID:  the System ID for the entity to load
 * @ctxt:  the context in which the entity is called or NULL
 *
 * By default we don't load external entitites, yet.
 *
 * Returns a new allocated xmlParserInputPtr, or NULL.
 */
static
xmlParserInputPtr
xmlDefaultExternalEntityLoader(const char *URL, const char *ID,
                               xmlParserCtxtPtr ctxt) {
    xmlParserInputPtr ret = NULL;
    xmlChar *resource = NULL;
#ifdef LIBXML_CATALOG_ENABLED
    xmlCatalogAllow pref;
#endif

#ifdef DEBUG_EXTERNAL_ENTITIES
    xmlGenericError(xmlGenericErrorContext,
	    "xmlDefaultExternalEntityLoader(%s, xxx)\n", URL);
#endif
#ifdef LIBXML_CATALOG_ENABLED
    /*
     * If the resource doesn't exists as a file,
     * try to load it from the resource pointed in the catalogs
     */
    pref = xmlCatalogGetDefaults();

    if ((pref != XML_CATA_ALLOW_NONE) && (!xmlSysIDExists(URL))) {
	/*
	 * Do a local lookup
	 */
	if ((ctxt->catalogs != NULL) &&
	    ((pref == XML_CATA_ALLOW_ALL) ||
	     (pref == XML_CATA_ALLOW_DOCUMENT))) {
	    resource = xmlCatalogLocalResolve(ctxt->catalogs,
					      (const xmlChar *)ID,
					      (const xmlChar *)URL);
        }
	/*
	 * Try a global lookup
	 */
	if ((resource == NULL) &&
	    ((pref == XML_CATA_ALLOW_ALL) ||
	     (pref == XML_CATA_ALLOW_GLOBAL))) {
	    resource = xmlCatalogResolve((const xmlChar *)ID,
					 (const xmlChar *)URL);
	}
	if ((resource == NULL) && (URL != NULL))
	    resource = xmlStrdup((const xmlChar *) URL);

	/*
	 * TODO: do an URI lookup on the reference
	 */
	if ((resource != NULL) && (!xmlSysIDExists((const char *)resource))) {
	    xmlChar *tmp = NULL;

	    if ((ctxt->catalogs != NULL) &&
		((pref == XML_CATA_ALLOW_ALL) ||
		 (pref == XML_CATA_ALLOW_DOCUMENT))) {
		tmp = xmlCatalogLocalResolveURI(ctxt->catalogs, resource);
	    }
	    if ((tmp == NULL) &&
		((pref == XML_CATA_ALLOW_ALL) ||
	         (pref == XML_CATA_ALLOW_GLOBAL))) {
		tmp = xmlCatalogResolveURI(resource);
	    }

	    if (tmp != NULL) {
		xmlFree(resource);
		resource = tmp;
	    }
	}
    }
#endif

    if (resource == NULL)
	resource = (xmlChar *) URL;

    if (resource == NULL) {
	if (ID == NULL)
	    ID = "NULL";
	if ((ctxt->validate) && (ctxt->sax != NULL) && 
            (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt,
		    "failed to load external entity \"%s\"\n", ID);
	else if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt,
		    "failed to load external entity \"%s\"\n", ID);
        return(NULL);
    }
    ret = xmlNewInputFromFile(ctxt, (const char *)resource);
    if (ret == NULL) {
	if ((ctxt->validate) && (ctxt->sax != NULL) && 
            (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt,
		    "failed to load external entity \"%s\"\n", resource);
	else if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt,
		    "failed to load external entity \"%s\"\n", resource);
    }
    if ((resource != NULL) && (resource != (xmlChar *) URL))
	xmlFree(resource);
    return(ret);
}

static xmlExternalEntityLoader xmlCurrentExternalEntityLoader =
       xmlDefaultExternalEntityLoader;

/**
 * xmlSetExternalEntityLoader:
 * @f:  the new entity resolver function
 *
 * Changes the defaultexternal entity resolver function for the application
 */
void
xmlSetExternalEntityLoader(xmlExternalEntityLoader f) {
    xmlCurrentExternalEntityLoader = f;
}

/**
 * xmlGetExternalEntityLoader:
 *
 * Get the default external entity resolver function for the application
 *
 * Returns the xmlExternalEntityLoader function pointer
 */
xmlExternalEntityLoader
xmlGetExternalEntityLoader(void) {
    return(xmlCurrentExternalEntityLoader);
}

/**
 * xmlLoadExternalEntity:
 * @URL:  the URL for the entity to load
 * @ID:  the Public ID for the entity to load
 * @ctxt:  the context in which the entity is called or NULL
 *
 * Load an external entity, note that the use of this function for
 * unparsed entities may generate problems
 * TODO: a more generic External entity API must be designed
 *
 * Returns the xmlParserInputPtr or NULL
 */
xmlParserInputPtr
xmlLoadExternalEntity(const char *URL, const char *ID,
                      xmlParserCtxtPtr ctxt) {
    return(xmlCurrentExternalEntityLoader(URL, ID, ctxt));
}

/************************************************************************
 * 									*
 * 		Disabling Network access				*
 * 									*
 ************************************************************************/

#ifdef LIBXML_CATALOG_ENABLED
static int
xmlNoNetExists(const char *URL)
{
#ifdef HAVE_STAT
    int ret;
    struct stat info;
    const char *path;

    if (URL == NULL)
        return (0);

    if (!xmlStrncasecmp(BAD_CAST URL, BAD_CAST "file://localhost/", 17))
#if defined (_WIN32) && !defined(__CYGWIN__)
	path = &URL[17];
#else
	path = &URL[16];
#endif
    else if (!xmlStrncasecmp(BAD_CAST URL, BAD_CAST "file:///", 8)) {
#if defined (_WIN32) && !defined(__CYGWIN__)
        path = &URL[8];
#else
        path = &URL[7];
#endif
    } else
        path = URL;
    ret = stat(path, &info);
    if (ret == 0)
        return (1);
#endif
    return (0);
}
#endif

/**
 * xmlNoNetExternalEntityLoader:
 * @URL:  the URL for the entity to load
 * @ID:  the System ID for the entity to load
 * @ctxt:  the context in which the entity is called or NULL
 *
 * A specific entity loader disabling network accesses, though still
 * allowing local catalog accesses for resolution.
 *
 * Returns a new allocated xmlParserInputPtr, or NULL.
 */
xmlParserInputPtr
xmlNoNetExternalEntityLoader(const char *URL, const char *ID,
                             xmlParserCtxtPtr ctxt) {
    xmlParserInputPtr input = NULL;
    xmlChar *resource = NULL;

#ifdef LIBXML_CATALOG_ENABLED
    xmlCatalogAllow pref;

    /*
     * If the resource doesn't exists as a file,
     * try to load it from the resource pointed in the catalogs
     */
    pref = xmlCatalogGetDefaults();

    if ((pref != XML_CATA_ALLOW_NONE) && (!xmlNoNetExists(URL))) {
	/*
	 * Do a local lookup
	 */
	if ((ctxt->catalogs != NULL) &&
	    ((pref == XML_CATA_ALLOW_ALL) ||
	     (pref == XML_CATA_ALLOW_DOCUMENT))) {
	    resource = xmlCatalogLocalResolve(ctxt->catalogs,
					      (const xmlChar *)ID,
					      (const xmlChar *)URL);
        }
	/*
	 * Try a global lookup
	 */
	if ((resource == NULL) &&
	    ((pref == XML_CATA_ALLOW_ALL) ||
	     (pref == XML_CATA_ALLOW_GLOBAL))) {
	    resource = xmlCatalogResolve((const xmlChar *)ID,
					 (const xmlChar *)URL);
	}
	if ((resource == NULL) && (URL != NULL))
	    resource = xmlStrdup((const xmlChar *) URL);

	/*
	 * TODO: do an URI lookup on the reference
	 */
	if ((resource != NULL) && (!xmlNoNetExists((const char *)resource))) {
	    xmlChar *tmp = NULL;

	    if ((ctxt->catalogs != NULL) &&
		((pref == XML_CATA_ALLOW_ALL) ||
		 (pref == XML_CATA_ALLOW_DOCUMENT))) {
		tmp = xmlCatalogLocalResolveURI(ctxt->catalogs, resource);
	    }
	    if ((tmp == NULL) &&
		((pref == XML_CATA_ALLOW_ALL) ||
	         (pref == XML_CATA_ALLOW_GLOBAL))) {
		tmp = xmlCatalogResolveURI(resource);
	    }

	    if (tmp != NULL) {
		xmlFree(resource);
		resource = tmp;
	    }
	}
    }
#endif
    if (resource == NULL)
	resource = (xmlChar *) URL;

    if (resource != NULL) {
        if ((!xmlStrncasecmp(BAD_CAST resource, BAD_CAST "ftp://", 6)) ||
            (!xmlStrncasecmp(BAD_CAST resource, BAD_CAST "http://", 7))) {
	    xmlGenericError(xmlGenericErrorContext,
	            "Attempt to load network entity %s \n", resource);

	    if (resource != (xmlChar *) URL)
		xmlFree(resource);
	    return(NULL);
	}
    }
    input = xmlDefaultExternalEntityLoader((const char *) resource, ID, ctxt);
    if (resource != (xmlChar *) URL)
	xmlFree(resource);
    return(input);
}

