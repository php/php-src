/*
 * error.c: module displaying/handling XML parser errors
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <daniel@veillard.com>
 */

#define IN_LIBXML
#include "libxml.h"

#include <stdarg.h>
#include <libxml/parser.h>
#include <libxml/xmlerror.h>
#include <libxml/xmlmemory.h>
#include <libxml/globals.h>

void xmlGenericErrorDefaultFunc	(void *ctx ATTRIBUTE_UNUSED,
				 const char *msg,
				 ...);

#define XML_GET_VAR_STR(msg, str) {				\
    int       size;						\
    int       chars;						\
    char      *larger;						\
    va_list   ap;						\
								\
    str = (char *) xmlMalloc(150);				\
    if (str == NULL) 						\
	return;							\
								\
    size = 150;							\
								\
    while (1) {							\
	va_start(ap, msg);					\
  	chars = vsnprintf(str, size, msg, ap);			\
	va_end(ap);						\
	if ((chars > -1) && (chars < size))			\
	    break;						\
	if (chars > -1)						\
	    size += chars + 1;					\
	else							\
	    size += 100;					\
	if ((larger = (char *) xmlRealloc(str, size)) == NULL) {\
	    xmlFree(str);					\
	    return;						\
	}							\
	str = larger;						\
    }								\
}

/************************************************************************
 * 									*
 * 			Handling of out of context errors		*
 * 									*
 ************************************************************************/

/**
 * xmlGenericErrorDefaultFunc:
 * @ctx:  an error context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Default handler for out of context error messages.
 */
void
xmlGenericErrorDefaultFunc(void *ctx ATTRIBUTE_UNUSED, const char *msg, ...) {
    va_list args;

    if (xmlGenericErrorContext == NULL)
	xmlGenericErrorContext = (void *) stderr;

    va_start(args, msg);
    vfprintf((FILE *)xmlGenericErrorContext, msg, args);
    va_end(args);
}

/**
 * initGenericErrorDefaultFunc:
 * @handler:  the handler
 * 
 * Set or reset (if NULL) the default handler for generic errors
 */
void
initGenericErrorDefaultFunc(xmlGenericErrorFunc * handler)
{
    if (handler == NULL)
        xmlGenericError = xmlGenericErrorDefaultFunc;
    else
        (*handler) = xmlGenericErrorDefaultFunc;
}

/**
 * xmlSetGenericErrorFunc:
 * @ctx:  the new error handling context
 * @handler:  the new handler function
 *
 * Function to reset the handler and the error context for out of
 * context error messages.
 * This simply means that @handler will be called for subsequent
 * error messages while not parsing nor validating. And @ctx will
 * be passed as first argument to @handler
 * One can simply force messages to be emitted to another FILE * than
 * stderr by setting @ctx to this file handle and @handler to NULL.
 */
void
xmlSetGenericErrorFunc(void *ctx, xmlGenericErrorFunc handler) {
    xmlGenericErrorContext = ctx;
    if (handler != NULL)
	xmlGenericError = handler;
    else
	xmlGenericError = xmlGenericErrorDefaultFunc;
}

/************************************************************************
 * 									*
 * 			Handling of parsing errors			*
 * 									*
 ************************************************************************/

/**
 * xmlParserPrintFileInfo:
 * @input:  an xmlParserInputPtr input
 * 
 * Displays the associated file and line informations for the current input
 */

void
xmlParserPrintFileInfo(xmlParserInputPtr input) {
    if (input != NULL) {
	if (input->filename)
	    xmlGenericError(xmlGenericErrorContext,
		    "%s:%d: ", input->filename,
		    input->line);
	else
	    xmlGenericError(xmlGenericErrorContext,
		    "Entity: line %d: ", input->line);
    }
}

/**
 * xmlParserPrintFileContext:
 * @input:  an xmlParserInputPtr input
 * 
 * Displays current context within the input content for error tracking
 */

void
xmlParserPrintFileContext(xmlParserInputPtr input) {
    const xmlChar *cur, *base;
    int n;
    xmlChar  content[81];
    xmlChar *ctnt;

    if (input == NULL) return;
    cur = input->cur;
    base = input->base;
    /* skip backwards over any end-of-lines */
    while ((cur > base) && ((*cur == '\n') || (*cur == '\r'))) {
	cur--;
    }
    n = 0;
    /* search backwards for beginning-of-line maximum 80 characters */
    while ((n++ < 80) && (cur > base) && (*cur != '\n') && (*cur != '\r'))
        cur--;
    if ((*cur == '\n') || (*cur == '\r')) cur++;
	/* search forward for end-of-line maximum 80 characters */
    n = 0;
    ctnt = content;
    while ((*cur != 0) && (*cur != '\n') && (*cur != '\r') && (n < 79)) {
		*ctnt++ = *cur++;
	n++;
    }
    *ctnt = 0;
    xmlGenericError(xmlGenericErrorContext,"%s\n", content);
    /* create blank line with problem pointer */
    cur = input->cur;
    while ((cur > base) && ((*cur == '\n') || (*cur == '\r'))) {
		cur--;
	}
    n = 0;
    ctnt = content;
    while ((n++ < 79) && (cur > base) && (*cur != '\n') && (*cur != '\r')) {
	*ctnt++ = ' ';
	cur--;
    }
    if (ctnt > content) {
	*(--ctnt) = '^';
	*(++ctnt) = 0;
    } else {
	*ctnt = '^';
	*(++ctnt) = 0;
    }
    xmlGenericError(xmlGenericErrorContext,"%s\n", content);
}

#if 0
/**
 * xmlGetVarStr:
 * @msg:  the message format
 * @args:  a va_list argument list
 *
 * SGS contribution
 * Get an arbitrary-sized string for an error argument
 * The caller must free() the returned string
 */
static char *
xmlGetVarStr(const char * msg, va_list args) {
    int       size;
    int       length;
    int       chars, left;
    char      *str, *larger;
    va_list   ap;

    str = (char *) xmlMalloc(150);
    if (str == NULL)
      return(NULL);

    size = 150;
    length = 0;

    while (1) {
	left = size - length;
		    /* Try to print in the allocated space. */
	va_start(msg, ap);
  	chars = vsnprintf(str + length, left, msg, ap);
	va_end(ap);
			  /* If that worked, we're done. */
	if ((chars > -1) && (chars < left ))
	    break;
			  /* Else try again with more space. */
	if (chars > -1)         /* glibc 2.1 */
	    size += chars + 1;  /* precisely what is needed */
	else                    /* glibc 2.0 */
	    size += 100;
	if ((larger = (char *) xmlRealloc(str, size)) == NULL) {
	    xmlFree(str);
	    return(NULL);
	}
	str = larger;
    }
    return(str);
}
#endif

/**
 * xmlParserError:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format an error messages, gives file, line, position and
 * extra parameters.
 */
void
xmlParserError(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input = NULL;
    xmlParserInputPtr cur = NULL;
    char * str;

    if (ctxt != NULL) {
	input = ctxt->input;
	if ((input != NULL) && (input->filename == NULL) &&
	    (ctxt->inputNr > 1)) {
	    cur = input;
	    input = ctxt->inputTab[ctxt->inputNr - 2];
	}
	xmlParserPrintFileInfo(input);
    }

    xmlGenericError(xmlGenericErrorContext, "error: ");
    XML_GET_VAR_STR(msg, str);
    xmlGenericError(xmlGenericErrorContext, "%s", str);
    if (str != NULL)
	xmlFree(str);

    if (ctxt != NULL) {
	xmlParserPrintFileContext(input);
	if (cur != NULL) {
	    xmlParserPrintFileInfo(cur);
	    xmlGenericError(xmlGenericErrorContext, "\n");
	    xmlParserPrintFileContext(cur);
	}
    }
}

/**
 * xmlParserWarning:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format a warning messages, gives file, line, position and
 * extra parameters.
 */
void
xmlParserWarning(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input = NULL;
    xmlParserInputPtr cur = NULL;
    char * str;

    if (ctxt != NULL) {
	input = ctxt->input;
	if ((input != NULL) && (input->filename == NULL) &&
	    (ctxt->inputNr > 1)) {
	    cur = input;
	    input = ctxt->inputTab[ctxt->inputNr - 2];
	}
	xmlParserPrintFileInfo(input);
    }
        
    xmlGenericError(xmlGenericErrorContext, "warning: ");
    XML_GET_VAR_STR(msg, str);
    xmlGenericError(xmlGenericErrorContext, "%s", str);
    if (str != NULL)
	xmlFree(str);

    if (ctxt != NULL) {
	xmlParserPrintFileContext(input);
	if (cur != NULL) {
	    xmlParserPrintFileInfo(cur);
	    xmlGenericError(xmlGenericErrorContext, "\n");
	    xmlParserPrintFileContext(cur);
	}
    }
}

/************************************************************************
 * 									*
 * 			Handling of validation errors			*
 * 									*
 ************************************************************************/

/**
 * xmlParserValidityError:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format an validity error messages, gives file,
 * line, position and extra parameters.
 */
void
xmlParserValidityError(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input = NULL;
    char * str;
    int len = xmlStrlen((const xmlChar *) msg);
    static int had_info = 0;
    int need_context = 0;

    if ((len > 1) && (msg[len - 2] != ':')) {
	if (ctxt != NULL) {
	    input = ctxt->input;
	    if ((input->filename == NULL) && (ctxt->inputNr > 1))
		input = ctxt->inputTab[ctxt->inputNr - 2];
		
	    if (had_info == 0) {
		xmlParserPrintFileInfo(input);
	    }
	}
	xmlGenericError(xmlGenericErrorContext, "validity error: ");
	need_context = 1;
	had_info = 0;
    } else {
	had_info = 1;
    }

    XML_GET_VAR_STR(msg, str);
    xmlGenericError(xmlGenericErrorContext, "%s", str);
    if (str != NULL)
	xmlFree(str);

    if ((ctxt != NULL) && (input != NULL)) {
	xmlParserPrintFileContext(input);
    }
}

/**
 * xmlParserValidityWarning:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format a validity warning messages, gives file, line,
 * position and extra parameters.
 */
void
xmlParserValidityWarning(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input = NULL;
    char * str;
    int len = xmlStrlen((const xmlChar *) msg);

    if ((ctxt != NULL) && (len != 0) && (msg[len - 1] != ':')) {
	input = ctxt->input;
	if ((input->filename == NULL) && (ctxt->inputNr > 1))
	    input = ctxt->inputTab[ctxt->inputNr - 2];

	xmlParserPrintFileInfo(input);
    }
        
    xmlGenericError(xmlGenericErrorContext, "validity warning: ");
    XML_GET_VAR_STR(msg, str);
    xmlGenericError(xmlGenericErrorContext, "%s", str);
    if (str != NULL)
	xmlFree(str);

    if (ctxt != NULL) {
	xmlParserPrintFileContext(input);
    }
}


