/**
 * uri.c: library of generic URI related routines 
 *
 * Reference: RFC 2396
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#ifndef __XML_URI_H__
#define __XML_URI_H__

#include <libxml/tree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * xmlURI:
 *
 * A parsed URI reference. This is a struct containing the various fields
 * as described in RFC 2396 but separated for further processing.
 */
typedef struct _xmlURI xmlURI;
typedef xmlURI *xmlURIPtr;
struct _xmlURI {
    char *scheme;	/* the URI scheme */
    char *opaque;	/* opaque part */
    char *authority;	/* the authority part */
    char *server;	/* the server part */
    char *user;		/* the user part */
    int port;		/* the port number */
    char *path;		/* the path string */
    char *query;	/* the query string */
    char *fragment;	/* the fragment identifier */
    int  cleanup;	/* parsing potentially unclean URI */
};

/*
 * This function is in tree.h:
 * xmlChar *	xmlNodeGetBase	(xmlDocPtr doc,
 *                               xmlNodePtr cur);
 */
xmlURIPtr	xmlCreateURI		(void);
xmlChar *	xmlBuildURI		(const xmlChar *URI,
	                        	 const xmlChar *base);
xmlURIPtr	xmlParseURI		(const char *str);
int		xmlParseURIReference	(xmlURIPtr uri,
					 const char *str);
xmlChar *	xmlSaveUri		(xmlURIPtr uri);
void		xmlPrintURI		(FILE *stream,
					 xmlURIPtr uri);
xmlChar *       xmlURIEscapeStr         (const xmlChar *str,
 					 const xmlChar *list);
char *		xmlURIUnescapeString	(const char *str,
					 int len,
					 char *target);
int		xmlNormalizeURIPath	(char *path);
xmlChar *	xmlURIEscape		(const xmlChar *str);
void		xmlFreeURI		(xmlURIPtr uri);

#ifdef __cplusplus
}
#endif
#endif /* __XML_URI_H__ */
