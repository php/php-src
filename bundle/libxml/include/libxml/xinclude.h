/*
 * xinclude.c : API to handle XInclude processing
 *
 * World Wide Web Consortium Working Draft 26 October 2000
 * http://www.w3.org/TR/2000/WD-xinclude-20001026
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#ifndef __XML_XINCLUDE_H__
#define __XML_XINCLUDE_H__

#include <libxml/tree.h>

#ifdef __cplusplus
extern "C" {
#endif

int	xmlXIncludeProcess	(xmlDocPtr doc);

#ifdef __cplusplus
}
#endif
#endif /* __XML_XINCLUDE_H__ */
