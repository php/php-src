/*
 * DOCBparser.h : interface for a DocBook SGML non-verifying parser
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#ifndef __DOCB_PARSER_H__
#define __DOCB_PARSER_H__
#include <libxml/parser.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Most of the back-end structures from XML and SGML are shared.
 */
typedef xmlParserCtxt docbParserCtxt;
typedef xmlParserCtxtPtr docbParserCtxtPtr;
typedef xmlParserNodeInfo docbParserNodeInfo;
typedef xmlSAXHandler docbSAXHandler;
typedef xmlSAXHandlerPtr docbSAXHandlerPtr;
typedef xmlParserInput docbParserInput;
typedef xmlParserInputPtr docbParserInputPtr;
typedef xmlDocPtr docbDocPtr;
typedef xmlNodePtr docbNodePtr;

/*
 * There is only few public functions.
 */
int                    docbEncodeEntities(unsigned char *out,
                                        int *outlen,
                                        const unsigned char *in,
                                        int *inlen, int quoteChar);

docbDocPtr             docbSAXParseDoc (xmlChar *cur,
                                        const char *encoding,
                                        docbSAXHandlerPtr sax,
                                        void *userData);
docbDocPtr             docbParseDoc    (xmlChar *cur,
                                        const char *encoding);
docbDocPtr             docbSAXParseFile(const char *filename,
                                        const char *encoding,
                                        docbSAXHandlerPtr sax,
                                        void *userData);
docbDocPtr             docbParseFile   (const char *filename,
                                        const char *encoding);

/**
 * Interfaces for the Push mode.
 */
void                   docbFreeParserCtxt      (docbParserCtxtPtr ctxt);
docbParserCtxtPtr      docbCreatePushParserCtxt(docbSAXHandlerPtr sax,
                                                void *user_data,
                                                const char *chunk,
                                                int size,
                                                const char *filename,
                                                xmlCharEncoding enc);
int                    docbParseChunk          (docbParserCtxtPtr ctxt,
                                                const char *chunk,
                                                int size,
                                                int terminate);
docbParserCtxtPtr       docbCreateFileParserCtxt(const char *filename,
                                                const char *encoding);
int                     docbParseDocument       (docbParserCtxtPtr ctxt);

#ifdef __cplusplus
}
#endif

#endif /* __DOCB_PARSER_H__ */
