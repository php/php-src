/*
 * regexp.h : describes the basic API for libxml regular expressions handling
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */

#ifndef __XML_REGEXP_H__
#define __XML_REGEXP_H__

#include <libxml/xmlversion.h>

#ifdef LIBXML_REGEXP_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * xmlRegexpPtr:
 *
 * A libxml regular expression, they can actually be far more complex
 * thank the POSIX regex expressions.
 */
typedef struct _xmlRegexp xmlRegexp;
typedef xmlRegexp *xmlRegexpPtr;

/**
 * xmlRegExecCtxtPtr:
 *
 * A libxml progressive regular expression evaluation context
 */
typedef struct _xmlRegExecCtxt xmlRegExecCtxt;
typedef xmlRegExecCtxt *xmlRegExecCtxtPtr;

#ifdef __cplusplus
}
#endif 
#include <libxml/tree.h>
#ifdef __cplusplus
extern "C" {
#endif

/*
 * The POSIX like API
 */
xmlRegexpPtr		xmlRegexpCompile(const xmlChar *regexp);
void			xmlRegFreeRegexp(xmlRegexpPtr regexp);
int			xmlRegexpExec	(xmlRegexpPtr comp,
					 const xmlChar *value);
void			xmlRegexpPrint	(FILE *output,
					 xmlRegexpPtr regexp);
int			xmlRegexpIsDeterminist(xmlRegexpPtr comp);

/*
 * Callback function when doing a transition in the automata
 */
typedef void (*xmlRegExecCallbacks) (xmlRegExecCtxtPtr exec,
	                             const xmlChar *token,
				     void *transdata,
				     void *inputdata);

/*
 * The progressive API
 */
xmlRegExecCtxtPtr	xmlRegNewExecCtxt	(xmlRegexpPtr comp,
						 xmlRegExecCallbacks callback,
						 void *data);
void			xmlRegFreeExecCtxt	(xmlRegExecCtxtPtr exec);
int			xmlRegExecPushString	(xmlRegExecCtxtPtr exec,
						 const xmlChar *value,
						 void *data);

#ifdef __cplusplus
}
#endif 

#endif /* LIBXML_REGEXP_ENABLED */

#endif /*__XML_REGEXP_H__ */
