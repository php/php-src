/*
 * xmlIO.h : interface for the I/O interfaces used by the parser
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 */

#ifndef __XML_IO_H__
#define __XML_IO_H__

#include <stdio.h>
#include <libxml/xmlversion.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Those are the functions and datatypes for the parser input
 * I/O structures.
 */

/**
 * xmlInputMatchCallback:
 * @filename: the filename or URI
 *
 * Callback used in the I/O Input API to detect if the current handler 
 * can provide input fonctionnalities for this resource.
 *
 * Returns 1 if yes and 0 if another Input module should be used
 */
typedef int (*xmlInputMatchCallback) (char const *filename);
/**
 * xmlInputOpenCallback:
 * @filename: the filename or URI
 *
 * Callback used in the I/O Input API to open the resource
 *
 * Returns an Input context or NULL in case or error
 */
typedef void * (*xmlInputOpenCallback) (char const *filename);
/**
 * xmlInputReadCallback:
 * @context:  an Input context
 * @buffer:  the buffer to store data read
 * @len:  the length of the buffer in bytes
 *
 * Callback used in the I/O Input API to read the resource
 *
 * Returns the number of bytes read or -1 in case of error
 */
typedef int (*xmlInputReadCallback) (void * context, char * buffer, int len);
/**
 * xmlInputCloseCallback:
 * @context:  an Input context
 *
 * Callback used in the I/O Input API to close the resource
 *
 * Returns 0 or -1 in case of error
 */
typedef int (*xmlInputCloseCallback) (void * context);

/*
 * Those are the functions and datatypes for the library output
 * I/O structures.
 */

/**
 * xmlOutputMatchCallback:
 * @filename: the filename or URI
 *
 * Callback used in the I/O Output API to detect if the current handler 
 * can provide output fonctionnalities for this resource.
 *
 * Returns 1 if yes and 0 if another Output module should be used
 */
typedef int (*xmlOutputMatchCallback) (char const *filename);
/**
 * xmlOutputOpenCallback:
 * @filename: the filename or URI
 *
 * Callback used in the I/O Output API to open the resource
 *
 * Returns an Output context or NULL in case or error
 */
typedef void * (*xmlOutputOpenCallback) (char const *filename);
/**
 * xmlOutputWriteCallback:
 * @context:  an Output context
 * @buffer:  the buffer of data to write
 * @len:  the length of the buffer in bytes
 *
 * Callback used in the I/O Output API to write to the resource
 *
 * Returns the number of bytes written or -1 in case of error
 */
typedef int (*xmlOutputWriteCallback) (void * context, const char * buffer,
                                       int len);
/**
 * xmlOutputCloseCallback:
 * @context:  an Output context
 *
 * Callback used in the I/O Output API to close the resource
 *
 * Returns 0 or -1 in case of error
 */
typedef int (*xmlOutputCloseCallback) (void * context);

#ifdef __cplusplus
}
#endif

#include <libxml/globals.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/encoding.h>

#ifdef __cplusplus
extern "C" {
#endif
struct _xmlParserInputBuffer {
    void*                  context;
    xmlInputReadCallback   readcallback;
    xmlInputCloseCallback  closecallback;
    
    xmlCharEncodingHandlerPtr encoder; /* I18N conversions to UTF-8 */
    
    xmlBufferPtr buffer;    /* Local buffer encoded in UTF-8 */
    xmlBufferPtr raw;       /* if encoder != NULL buffer for raw input */
};


struct _xmlOutputBuffer {
    void*                   context;
    xmlOutputWriteCallback  writecallback;
    xmlOutputCloseCallback  closecallback;
    
    xmlCharEncodingHandlerPtr encoder; /* I18N conversions to UTF-8 */
    
    xmlBufferPtr buffer;    /* Local buffer encoded in UTF-8 or ISOLatin */
    xmlBufferPtr conv;      /* if encoder != NULL buffer for output */
    int written;            /* total number of byte written */
};

/*
 * Interfaces for input
 */
void	xmlCleanupInputCallbacks		(void);
void	xmlCleanupOutputCallbacks		(void);

void	xmlRegisterDefaultInputCallbacks	(void);
xmlParserInputBufferPtr
	xmlAllocParserInputBuffer		(xmlCharEncoding enc);

xmlParserInputBufferPtr
	xmlParserInputBufferCreateFilename	(const char *URI,
                                                 xmlCharEncoding enc);
xmlParserInputBufferPtr
	xmlParserInputBufferCreateFile		(FILE *file,
                                                 xmlCharEncoding enc);
xmlParserInputBufferPtr
	xmlParserInputBufferCreateFd		(int fd,
	                                         xmlCharEncoding enc);
xmlParserInputBufferPtr
	xmlParserInputBufferCreateMem		(const char *mem, int size,
	                                         xmlCharEncoding enc);
xmlParserInputBufferPtr
	xmlParserInputBufferCreateIO		(xmlInputReadCallback   ioread,
						 xmlInputCloseCallback  ioclose,
						 void *ioctx,
	                                         xmlCharEncoding enc);
int	xmlParserInputBufferRead		(xmlParserInputBufferPtr in,
						 int len);
int	xmlParserInputBufferGrow		(xmlParserInputBufferPtr in,
						 int len);
int	xmlParserInputBufferPush		(xmlParserInputBufferPtr in,
						 int len,
						 const char *buf);
void	xmlFreeParserInputBuffer		(xmlParserInputBufferPtr in);
char *	xmlParserGetDirectory			(const char *filename);

int     xmlRegisterInputCallbacks		(xmlInputMatchCallback matchFunc,
						 xmlInputOpenCallback openFunc,
						 xmlInputReadCallback readFunc,
						 xmlInputCloseCallback closeFunc);
/*
 * Interfaces for output
 */
void	xmlRegisterDefaultOutputCallbacks(void);
xmlOutputBufferPtr
	xmlAllocOutputBuffer		(xmlCharEncodingHandlerPtr encoder);

xmlOutputBufferPtr
	xmlOutputBufferCreateFilename	(const char *URI,
					 xmlCharEncodingHandlerPtr encoder,
					 int compression);

xmlOutputBufferPtr
	xmlOutputBufferCreateFile	(FILE *file,
					 xmlCharEncodingHandlerPtr encoder);

xmlOutputBufferPtr
	xmlOutputBufferCreateFd		(int fd,
					 xmlCharEncodingHandlerPtr encoder);

xmlOutputBufferPtr
	xmlOutputBufferCreateIO		(xmlOutputWriteCallback   iowrite,
					 xmlOutputCloseCallback  ioclose,
					 void *ioctx,
					 xmlCharEncodingHandlerPtr encoder);

int	xmlOutputBufferWrite		(xmlOutputBufferPtr out,
					 int len,
					 const char *buf);
int	xmlOutputBufferWriteString	(xmlOutputBufferPtr out,
					 const char *str);

int	xmlOutputBufferFlush		(xmlOutputBufferPtr out);
int	xmlOutputBufferClose		(xmlOutputBufferPtr out);

int     xmlRegisterOutputCallbacks	(xmlOutputMatchCallback matchFunc,
					 xmlOutputOpenCallback openFunc,
					 xmlOutputWriteCallback writeFunc,
					 xmlOutputCloseCallback closeFunc);

/*  This function only exists if HTTP support built into the library  */
#ifdef LIBXML_HTTP_ENABLED
void *	xmlIOHTTPOpenW			(const char * post_uri,
					 int   compression );
void	xmlRegisterHTTPPostCallbacks	(void );
#endif

/*
 * A predefined entity loader disabling network accesses
 */
xmlParserInputPtr xmlNoNetExternalEntityLoader(const char *URL,
					 const char *ID,
					 xmlParserCtxtPtr ctxt);

xmlChar *xmlNormalizeWindowsPath	(const xmlChar *path);

int	xmlCheckFilename		(const char *path);
/**
 * Default 'file://' protocol callbacks 
 */
int	xmlFileMatch 			(const char *filename);
void *	xmlFileOpen 			(const char *filename);
int	xmlFileRead 			(void * context, 
					 char * buffer, 
					 int len);
int	xmlFileClose 			(void * context);

/**
 * Default 'http://' protocol callbacks 
 */
#ifdef LIBXML_HTTP_ENABLED
int	xmlIOHTTPMatch 			(const char *filename);
void *	xmlIOHTTPOpen 			(const char *filename);
int 	xmlIOHTTPRead			(void * context, 
					 char * buffer, 
					 int len);
int	xmlIOHTTPClose 			(void * context);
#endif /* LIBXML_HTTP_ENABLED */

/**
 * Default 'ftp://' protocol callbacks 
 */
#ifdef LIBXML_FTP_ENABLED 
int	xmlIOFTPMatch 			(const char *filename);
void *	xmlIOFTPOpen 			(const char *filename);
int 	xmlIOFTPRead			(void * context, 
					 char * buffer, 
					 int len);
int 	xmlIOFTPClose 			(void * context);
#endif /* LIBXML_FTP_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __XML_IO_H__ */
