/*
 * debugXML.h : Interfaces to a set of routines used for debugging the tree
 *              produced by the XML parser.
 *
 * Daniel Veillard <daniel@veillard.com>
 */

#ifndef __DEBUG_XML__
#define __DEBUG_XML__
#include <stdio.h>
#include <libxml/tree.h>

#ifdef LIBXML_DEBUG_ENABLED

#include <libxml/xpath.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The standard Dump routines.
 */
void	xmlDebugDumpString	(FILE *output,
				 const xmlChar *str);
void	xmlDebugDumpAttr	(FILE *output,
				 xmlAttrPtr attr,
				 int depth);
void	xmlDebugDumpAttrList	(FILE *output,
				 xmlAttrPtr attr,
				 int depth);
void	xmlDebugDumpOneNode	(FILE *output,
				 xmlNodePtr node,
				 int depth);
void	xmlDebugDumpNode	(FILE *output,
				 xmlNodePtr node,
				 int depth);
void	xmlDebugDumpNodeList	(FILE *output,
				 xmlNodePtr node,
				 int depth);
void	xmlDebugDumpDocumentHead(FILE *output,
				 xmlDocPtr doc);
void	xmlDebugDumpDocument	(FILE *output,
				 xmlDocPtr doc);
void	xmlDebugDumpDTD		(FILE *output,
				 xmlDtdPtr dtd);
void	xmlDebugDumpEntities	(FILE *output,
				 xmlDocPtr doc);

void	xmlLsOneNode		(FILE *output, xmlNodePtr node);
int	xmlLsCountNode		(xmlNodePtr node);

LIBXML_DLL_IMPORT const char *xmlBoolToText	(int boolval);

/****************************************************************
 *								*
 *	 The XML shell related structures and functions		*
 *								*
 ****************************************************************/

/**
 * xmlShellReadlineFunc:
 * @prompt:  a string prompt
 *
 * This is a generic signature for the XML shell input function.
 *
 * Returns a string which will be freed by the Shell.
 */
typedef char * (* xmlShellReadlineFunc)(char *prompt);

/**
 * xmlShellCtxt:
 *
 * A debugging shell context.
 * TODO: add the defined function tables.
 */
typedef struct _xmlShellCtxt xmlShellCtxt;
typedef xmlShellCtxt *xmlShellCtxtPtr;
struct _xmlShellCtxt {
    char *filename;
    xmlDocPtr doc;
    xmlNodePtr node;
    xmlXPathContextPtr pctxt;
    int loaded;
    FILE *output;
    xmlShellReadlineFunc input;
};

/**
 * xmlShellCmd:
 * @ctxt:  a shell context
 * @arg:  a string argument
 * @node:  a first node
 * @node2:  a second node
 *
 * This is a generic signature for the XML shell functions.
 *
 * Returns an int, negative returns indicating errors.
 */
typedef int (* xmlShellCmd) (xmlShellCtxtPtr ctxt,
                             char *arg,
			     xmlNodePtr node,
			     xmlNodePtr node2);

void	xmlShellPrintXPathError	(int errorType,
				 const char *arg);
void	xmlShellPrintNode	(xmlNodePtr node);
void	xmlShellPrintXPathResult(xmlXPathObjectPtr list);
int	xmlShellList		(xmlShellCtxtPtr ctxt,
				 char *arg,
				 xmlNodePtr node,
				 xmlNodePtr node2);
int	xmlShellBase		(xmlShellCtxtPtr ctxt,
				 char *arg,
				 xmlNodePtr node,
				 xmlNodePtr node2);
int	xmlShellDir		(xmlShellCtxtPtr ctxt,
				 char *arg,
				 xmlNodePtr node,
				 xmlNodePtr node2);
int	xmlShellCat		(xmlShellCtxtPtr ctxt,
				 char *arg,
				 xmlNodePtr node,
				 xmlNodePtr node2);
int	xmlShellLoad		(xmlShellCtxtPtr ctxt,
				 char *filename,
				 xmlNodePtr node,
				 xmlNodePtr node2);
int	xmlShellWrite		(xmlShellCtxtPtr ctxt,
				 char *filename,
				 xmlNodePtr node,
				 xmlNodePtr node2);
int	xmlShellSave		(xmlShellCtxtPtr ctxt,
				 char *filename,
				 xmlNodePtr node,
				 xmlNodePtr node2);
int	xmlShellValidate	(xmlShellCtxtPtr ctxt,
				 char *dtd,
				 xmlNodePtr node,
				 xmlNodePtr node2);
int	xmlShellDu		(xmlShellCtxtPtr ctxt,
				 char *arg,
				 xmlNodePtr tree,
				 xmlNodePtr node2);
int	xmlShellPwd		(xmlShellCtxtPtr ctxt,
				 char *buffer,
				 xmlNodePtr node,
				 xmlNodePtr node2);

/*
 * The Shell interface.
 */
void	xmlShell		(xmlDocPtr doc,
				 char *filename,
				 xmlShellReadlineFunc input,
				 FILE *output);
			 
#ifdef __cplusplus
}
#endif

#endif /* LIBXML_DEBUG_ENABLED */
#endif /* __DEBUG_XML__ */
