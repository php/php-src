/*
The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is expat.

The Initial Developer of the Original Code is James Clark.
Portions created by James Clark are Copyright (C) 1998, 1999
James Clark. All Rights Reserved.

Contributor(s):

Alternatively, the contents of this file may be used under the terms
of the GNU General Public License (the "GPL"), in which case the
provisions of the GPL are applicable instead of those above.  If you
wish to allow use of your version of this file only under the terms of
the GPL and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting the provisions above and
replace them with the notice and other provisions required by the
GPL. If you do not delete the provisions above, a recipient may use
your version of this file under either the MPL or the GPL.
*/

#include "xmldef.h"
#include "xmlparse.h"

#ifdef XML_UNICODE
#define XML_ENCODE_MAX XML_UTF16_ENCODE_MAX
#define XmlConvert XmlUtf16Convert
#define XmlGetInternalEncoding XmlGetUtf16InternalEncoding
#define XmlGetInternalEncodingNS XmlGetUtf16InternalEncodingNS
#define XmlEncode XmlUtf16Encode
#define MUST_CONVERT(enc, s) (!(enc)->isUtf16 || (((unsigned long)s) & 1))
typedef unsigned short ICHAR;
#else
#define XML_ENCODE_MAX XML_UTF8_ENCODE_MAX
#define XmlConvert XmlUtf8Convert
#define XmlGetInternalEncoding XmlGetUtf8InternalEncoding
#define XmlGetInternalEncodingNS XmlGetUtf8InternalEncodingNS
#define XmlEncode XmlUtf8Encode
#define MUST_CONVERT(enc, s) (!(enc)->isUtf8)
typedef char ICHAR;
#endif


#ifndef XML_NS

#define XmlInitEncodingNS XmlInitEncoding
#define XmlInitUnknownEncodingNS XmlInitUnknownEncoding
#undef XmlGetInternalEncodingNS
#define XmlGetInternalEncodingNS XmlGetInternalEncoding
#define XmlParseXmlDeclNS XmlParseXmlDecl

#endif


#ifdef XML_UNICODE_WCHAR_T
#define XML_T(x) L ## x
#else
#define XML_T(x) x
#endif

/* Round up n to be a multiple of sz, where sz is a power of 2. */
#define ROUND_UP(n, sz) (((n) + ((sz) - 1)) & ~((sz) - 1))

#include "xmltok.h"
#include "xmlrole.h"
#include "hashtable.h"

#define INIT_TAG_BUF_SIZE 32  /* must be a multiple of sizeof(XML_Char) */
#define INIT_DATA_BUF_SIZE 1024
#define INIT_ATTS_SIZE 16
#define INIT_BLOCK_SIZE 1024
#define INIT_BUFFER_SIZE 1024

#define EXPAND_SPARE 24

typedef struct binding {
  struct prefix *prefix;
  struct binding *nextTagBinding;
  struct binding *prevPrefixBinding;
  const struct attribute_id *attId;
  XML_Char *uri;
  int uriLen;
  int uriAlloc;
} BINDING;

typedef struct prefix {
  const XML_Char *name;
  BINDING *binding;
} PREFIX;

typedef struct {
  const XML_Char *str;
  const XML_Char *localPart;
  int uriLen;
} TAG_NAME;

typedef struct tag {
  struct tag *parent;
  const char *rawName;
  int rawNameLength;
  TAG_NAME name;
  char *buf;
  char *bufEnd;
  BINDING *bindings;
} TAG;

typedef struct {
  const XML_Char *name;
  const XML_Char *textPtr;
  int textLen;
  const XML_Char *systemId;
  const XML_Char *base;
  const XML_Char *publicId;
  const XML_Char *notation;
  char open;
} ENTITY;

typedef struct block {
  struct block *next;
  int size;
  XML_Char s[1];
} BLOCK;

typedef struct {
  BLOCK *blocks;
  BLOCK *freeBlocks;
  const XML_Char *end;
  XML_Char *ptr;
  XML_Char *start;
} STRING_POOL;

/* The XML_Char before the name is used to determine whether
an attribute has been specified. */
typedef struct attribute_id {
  XML_Char *name;
  PREFIX *prefix;
  char maybeTokenized;
  char xmlns;
} ATTRIBUTE_ID;

typedef struct {
  const ATTRIBUTE_ID *id;
  char isCdata;
  const XML_Char *value;
} DEFAULT_ATTRIBUTE;

typedef struct {
  const XML_Char *name;
  PREFIX *prefix;
  int nDefaultAtts;
  int allocDefaultAtts;
  DEFAULT_ATTRIBUTE *defaultAtts;
} ELEMENT_TYPE;

typedef struct {
  HASH_TABLE generalEntities;
  HASH_TABLE elementTypes;
  HASH_TABLE attributeIds;
  HASH_TABLE prefixes;
  STRING_POOL pool;
  int complete;
  int standalone;
  const XML_Char *base;
  PREFIX defaultPrefix;
} DTD;

typedef struct open_internal_entity {
  const char *internalEventPtr;
  const char *internalEventEndPtr;
  struct open_internal_entity *next;
  ENTITY *entity;
} OPEN_INTERNAL_ENTITY;

typedef enum XML_Error Processor(XML_Parser parser,
				 const char *start,
				 const char *end,
				 const char **endPtr);

static Processor prologProcessor;
static Processor prologInitProcessor;
static Processor contentProcessor;
static Processor cdataSectionProcessor;
static Processor epilogProcessor;
static Processor errorProcessor;
static Processor externalEntityInitProcessor;
static Processor externalEntityInitProcessor2;
static Processor externalEntityInitProcessor3;
static Processor externalEntityContentProcessor;

static enum XML_Error
handleUnknownEncoding(XML_Parser parser, const XML_Char *encodingName);
static enum XML_Error
processXmlDecl(XML_Parser parser, int isGeneralTextEntity, const char *, const char *);
static enum XML_Error
initializeEncoding(XML_Parser parser);
static enum XML_Error
doContent(XML_Parser parser, int startTagLevel, const ENCODING *enc,
	  const char *start, const char *end, const char **endPtr);
static enum XML_Error
doCdataSection(XML_Parser parser, const ENCODING *, const char **startPtr, const char *end, const char **nextPtr);
static enum XML_Error storeAtts(XML_Parser parser, const ENCODING *, const char *s,
				TAG_NAME *tagNamePtr, BINDING **bindingsPtr);
static
int addBinding(XML_Parser parser, PREFIX *prefix, const ATTRIBUTE_ID *attId, const XML_Char *uri, BINDING **bindingsPtr);
static int
defineAttribute(ELEMENT_TYPE *type, ATTRIBUTE_ID *, int isCdata, const XML_Char *dfltValue);
static enum XML_Error
storeAttributeValue(XML_Parser parser, const ENCODING *, int isCdata, const char *, const char *,
		    STRING_POOL *);
static enum XML_Error
appendAttributeValue(XML_Parser parser, const ENCODING *, int isCdata, const char *, const char *,
		    STRING_POOL *);
static ATTRIBUTE_ID *
getAttributeId(XML_Parser parser, const ENCODING *enc, const char *start, const char *end);
static int setElementTypePrefix(XML_Parser parser, ELEMENT_TYPE *);
static enum XML_Error
storeEntityValue(XML_Parser parser, const char *start, const char *end);
static int
reportProcessingInstruction(XML_Parser parser, const ENCODING *enc, const char *start, const char *end);
static int
reportComment(XML_Parser parser, const ENCODING *enc, const char *start, const char *end);
static void
reportDefault(XML_Parser parser, const ENCODING *enc, const char *start, const char *end);

static const XML_Char *getContext(XML_Parser parser);
static int setContext(XML_Parser parser, const XML_Char *context);
static void normalizePublicId(XML_Char *s);
static int dtdInit(DTD *);
static void dtdDestroy(DTD *);
static int dtdCopy(DTD *newDtd, const DTD *oldDtd);
static void poolInit(STRING_POOL *);
static void poolClear(STRING_POOL *);
static void poolDestroy(STRING_POOL *);
static XML_Char *poolAppend(STRING_POOL *pool, const ENCODING *enc,
			    const char *ptr, const char *end);
static XML_Char *poolStoreString(STRING_POOL *pool, const ENCODING *enc,
				  const char *ptr, const char *end);
static int poolGrow(STRING_POOL *pool);
static const XML_Char *poolCopyString(STRING_POOL *pool, const XML_Char *s);
static const XML_Char *poolCopyStringN(STRING_POOL *pool, const XML_Char *s, int n);

#define poolStart(pool) ((pool)->start)
#define poolEnd(pool) ((pool)->ptr)
#define poolLength(pool) ((pool)->ptr - (pool)->start)
#define poolChop(pool) ((void)--(pool->ptr))
#define poolLastChar(pool) (((pool)->ptr)[-1])
#define poolDiscard(pool) ((pool)->ptr = (pool)->start)
#define poolFinish(pool) ((pool)->start = (pool)->ptr)
#define poolAppendChar(pool, c) \
  (((pool)->ptr == (pool)->end && !poolGrow(pool)) \
   ? 0 \
   : ((*((pool)->ptr)++ = c), 1))

typedef struct {
  /* The first member must be userData so that the XML_GetUserData macro works. */
  void *m_userData;
  void *m_handlerArg;
  char *m_buffer;
  /* first character to be parsed */
  const char *m_bufferPtr;
  /* past last character to be parsed */
  char *m_bufferEnd;
  /* allocated end of buffer */
  const char *m_bufferLim;
  long m_parseEndByteIndex;
  const char *m_parseEndPtr;
  XML_Char *m_dataBuf;
  XML_Char *m_dataBufEnd;
  XML_StartElementHandler m_startElementHandler;
  XML_EndElementHandler m_endElementHandler;
  XML_CharacterDataHandler m_characterDataHandler;
  XML_ProcessingInstructionHandler m_processingInstructionHandler;
  XML_CommentHandler m_commentHandler;
  XML_StartCdataSectionHandler m_startCdataSectionHandler;
  XML_EndCdataSectionHandler m_endCdataSectionHandler;
  XML_DefaultHandler m_defaultHandler;
  XML_UnparsedEntityDeclHandler m_unparsedEntityDeclHandler;
  XML_NotationDeclHandler m_notationDeclHandler;
  XML_StartNamespaceDeclHandler m_startNamespaceDeclHandler;
  XML_EndNamespaceDeclHandler m_endNamespaceDeclHandler;
  XML_NotStandaloneHandler m_notStandaloneHandler;
  XML_ExternalEntityRefHandler m_externalEntityRefHandler;
  void *m_externalEntityRefHandlerArg;
  XML_UnknownEncodingHandler m_unknownEncodingHandler;
  const ENCODING *m_encoding;
  INIT_ENCODING m_initEncoding;
  const XML_Char *m_protocolEncodingName;
  int m_ns;
  void *m_unknownEncodingMem;
  void *m_unknownEncodingData;
  void *m_unknownEncodingHandlerData;
  void (*m_unknownEncodingRelease)(void *);
  PROLOG_STATE m_prologState;
  Processor *m_processor;
  enum XML_Error m_errorCode;
  const char *m_eventPtr;
  const char *m_eventEndPtr;
  const char *m_positionPtr;
  OPEN_INTERNAL_ENTITY *m_openInternalEntities;
  int m_defaultExpandInternalEntities;
  int m_tagLevel;
  ENTITY *m_declEntity;
  const XML_Char *m_declNotationName;
  const XML_Char *m_declNotationPublicId;
  ELEMENT_TYPE *m_declElementType;
  ATTRIBUTE_ID *m_declAttributeId;
  char m_declAttributeIsCdata;
  DTD m_dtd;
  TAG *m_tagStack;
  TAG *m_freeTagList;
  BINDING *m_inheritedBindings;
  BINDING *m_freeBindingList;
  int m_attsSize;
  int m_nSpecifiedAtts;
  ATTRIBUTE *m_atts;
  POSITION m_position;
  STRING_POOL m_tempPool;
  STRING_POOL m_temp2Pool;
  char *m_groupConnector;
  unsigned m_groupSize;
  int m_hadExternalDoctype;
  XML_Char m_namespaceSeparator;
} Parser;

#define userData (((Parser *)parser)->m_userData)
#define handlerArg (((Parser *)parser)->m_handlerArg)
#define startElementHandler (((Parser *)parser)->m_startElementHandler)
#define endElementHandler (((Parser *)parser)->m_endElementHandler)
#define characterDataHandler (((Parser *)parser)->m_characterDataHandler)
#define processingInstructionHandler (((Parser *)parser)->m_processingInstructionHandler)
#define commentHandler (((Parser *)parser)->m_commentHandler)
#define startCdataSectionHandler (((Parser *)parser)->m_startCdataSectionHandler)
#define endCdataSectionHandler (((Parser *)parser)->m_endCdataSectionHandler)
#define defaultHandler (((Parser *)parser)->m_defaultHandler)
#define unparsedEntityDeclHandler (((Parser *)parser)->m_unparsedEntityDeclHandler)
#define notationDeclHandler (((Parser *)parser)->m_notationDeclHandler)
#define startNamespaceDeclHandler (((Parser *)parser)->m_startNamespaceDeclHandler)
#define endNamespaceDeclHandler (((Parser *)parser)->m_endNamespaceDeclHandler)
#define notStandaloneHandler (((Parser *)parser)->m_notStandaloneHandler)
#define externalEntityRefHandler (((Parser *)parser)->m_externalEntityRefHandler)
#define externalEntityRefHandlerArg (((Parser *)parser)->m_externalEntityRefHandlerArg)
#define unknownEncodingHandler (((Parser *)parser)->m_unknownEncodingHandler)
#define encoding (((Parser *)parser)->m_encoding)
#define initEncoding (((Parser *)parser)->m_initEncoding)
#define unknownEncodingMem (((Parser *)parser)->m_unknownEncodingMem)
#define unknownEncodingData (((Parser *)parser)->m_unknownEncodingData)
#define unknownEncodingHandlerData \
  (((Parser *)parser)->m_unknownEncodingHandlerData)
#define unknownEncodingRelease (((Parser *)parser)->m_unknownEncodingRelease)
#define protocolEncodingName (((Parser *)parser)->m_protocolEncodingName)
#define ns (((Parser *)parser)->m_ns)
#define prologState (((Parser *)parser)->m_prologState)
#define processor (((Parser *)parser)->m_processor)
#define errorCode (((Parser *)parser)->m_errorCode)
#define eventPtr (((Parser *)parser)->m_eventPtr)
#define eventEndPtr (((Parser *)parser)->m_eventEndPtr)
#define positionPtr (((Parser *)parser)->m_positionPtr)
#define position (((Parser *)parser)->m_position)
#define openInternalEntities (((Parser *)parser)->m_openInternalEntities)
#define defaultExpandInternalEntities (((Parser *)parser)->m_defaultExpandInternalEntities)
#define tagLevel (((Parser *)parser)->m_tagLevel)
#define buffer (((Parser *)parser)->m_buffer)
#define bufferPtr (((Parser *)parser)->m_bufferPtr)
#define bufferEnd (((Parser *)parser)->m_bufferEnd)
#define parseEndByteIndex (((Parser *)parser)->m_parseEndByteIndex)
#define parseEndPtr (((Parser *)parser)->m_parseEndPtr)
#define bufferLim (((Parser *)parser)->m_bufferLim)
#define dataBuf (((Parser *)parser)->m_dataBuf)
#define dataBufEnd (((Parser *)parser)->m_dataBufEnd)
#define dtd (((Parser *)parser)->m_dtd)
#define declEntity (((Parser *)parser)->m_declEntity)
#define declNotationName (((Parser *)parser)->m_declNotationName)
#define declNotationPublicId (((Parser *)parser)->m_declNotationPublicId)
#define declElementType (((Parser *)parser)->m_declElementType)
#define declAttributeId (((Parser *)parser)->m_declAttributeId)
#define declAttributeIsCdata (((Parser *)parser)->m_declAttributeIsCdata)
#define freeTagList (((Parser *)parser)->m_freeTagList)
#define freeBindingList (((Parser *)parser)->m_freeBindingList)
#define inheritedBindings (((Parser *)parser)->m_inheritedBindings)
#define tagStack (((Parser *)parser)->m_tagStack)
#define atts (((Parser *)parser)->m_atts)
#define attsSize (((Parser *)parser)->m_attsSize)
#define nSpecifiedAtts (((Parser *)parser)->m_nSpecifiedAtts)
#define tempPool (((Parser *)parser)->m_tempPool)
#define temp2Pool (((Parser *)parser)->m_temp2Pool)
#define groupConnector (((Parser *)parser)->m_groupConnector)
#define groupSize (((Parser *)parser)->m_groupSize)
#define hadExternalDoctype (((Parser *)parser)->m_hadExternalDoctype)
#define namespaceSeparator (((Parser *)parser)->m_namespaceSeparator)

#ifdef _MSC_VER
#ifdef _DEBUG
Parser *asParser(XML_Parser parser)
{
  return parser;
}
#endif
#endif

XML_Parser XML_ParserCreate(const XML_Char *encodingName)
{
  XML_Parser parser = malloc(sizeof(Parser));
  if (!parser)
    return parser;
  processor = prologInitProcessor;
  XmlPrologStateInit(&prologState);
  userData = 0;
  handlerArg = 0;
  startElementHandler = 0;
  endElementHandler = 0;
  characterDataHandler = 0;
  processingInstructionHandler = 0;
  commentHandler = 0;
  startCdataSectionHandler = 0;
  endCdataSectionHandler = 0;
  defaultHandler = 0;
  unparsedEntityDeclHandler = 0;
  notationDeclHandler = 0;
  startNamespaceDeclHandler = 0;
  endNamespaceDeclHandler = 0;
  notStandaloneHandler = 0;
  externalEntityRefHandler = 0;
  externalEntityRefHandlerArg = parser;
  unknownEncodingHandler = 0;
  buffer = 0;
  bufferPtr = 0;
  bufferEnd = 0;
  parseEndByteIndex = 0;
  parseEndPtr = 0;
  bufferLim = 0;
  declElementType = 0;
  declAttributeId = 0;
  declEntity = 0;
  declNotationName = 0;
  declNotationPublicId = 0;
  memset(&position, 0, sizeof(POSITION));
  errorCode = XML_ERROR_NONE;
  eventPtr = 0;
  eventEndPtr = 0;
  positionPtr = 0;
  openInternalEntities = 0;
  tagLevel = 0;
  tagStack = 0;
  freeTagList = 0;
  freeBindingList = 0;
  inheritedBindings = 0;
  attsSize = INIT_ATTS_SIZE;
  atts = malloc(attsSize * sizeof(ATTRIBUTE));
  nSpecifiedAtts = 0;
  dataBuf = malloc(INIT_DATA_BUF_SIZE * sizeof(XML_Char));
  groupSize = 0;
  groupConnector = 0;
  hadExternalDoctype = 0;
  unknownEncodingMem = 0;
  unknownEncodingRelease = 0;
  unknownEncodingData = 0;
  unknownEncodingHandlerData = 0;
  namespaceSeparator = '!';
  ns = 0;
  poolInit(&tempPool);
  poolInit(&temp2Pool);
  protocolEncodingName = encodingName ? poolCopyString(&tempPool, encodingName) : 0;
  if (!dtdInit(&dtd) || !atts || !dataBuf
      || (encodingName && !protocolEncodingName)) {
    XML_ParserFree(parser);
    return 0;
  }
  dataBufEnd = dataBuf + INIT_DATA_BUF_SIZE;
  XmlInitEncoding(&initEncoding, &encoding, 0);
  return parser;
}

XML_Parser XML_ParserCreateNS(const XML_Char *encodingName, XML_Char nsSep)
{
  static
  const XML_Char implicitContext[] = {
    XML_T('x'), XML_T('m'), XML_T('l'), XML_T('='),
    XML_T('h'), XML_T('t'), XML_T('t'), XML_T('p'), XML_T(':'),
    XML_T('/'), XML_T('/'), XML_T('w'), XML_T('w'), XML_T('w'),
    XML_T('.'), XML_T('w'), XML_T('3'),
    XML_T('.'), XML_T('o'), XML_T('r'), XML_T('g'),
    XML_T('/'), XML_T('X'), XML_T('M'), XML_T('L'),
    XML_T('/'), XML_T('1'), XML_T('9'), XML_T('9'), XML_T('8'),
    XML_T('/'), XML_T('n'), XML_T('a'), XML_T('m'), XML_T('e'),
    XML_T('s'), XML_T('p'), XML_T('a'), XML_T('c'), XML_T('e'),
    XML_T('\0')
  };

  XML_Parser parser = XML_ParserCreate(encodingName);
  if (parser) {
    XmlInitEncodingNS(&initEncoding, &encoding, 0);
    ns = 1;
    namespaceSeparator = nsSep;
  }
  if (!setContext(parser, implicitContext)) {
    XML_ParserFree(parser);
    return 0;
  }
  return parser;
}

int XML_SetEncoding(XML_Parser parser, const XML_Char *encodingName)
{
  if (!encodingName)
    protocolEncodingName = 0;
  else {
    protocolEncodingName = poolCopyString(&tempPool, encodingName);
    if (!protocolEncodingName)
      return 0;
  }
  return 1;
}

XML_Parser XML_ExternalEntityParserCreate(XML_Parser oldParser,
					  const XML_Char *context,
					  const XML_Char *encodingName)
{
  XML_Parser parser = oldParser;
  DTD *oldDtd = &dtd;
  XML_StartElementHandler oldStartElementHandler = startElementHandler;
  XML_EndElementHandler oldEndElementHandler = endElementHandler;
  XML_CharacterDataHandler oldCharacterDataHandler = characterDataHandler;
  XML_ProcessingInstructionHandler oldProcessingInstructionHandler = processingInstructionHandler;
  XML_CommentHandler oldCommentHandler = commentHandler;
  XML_StartCdataSectionHandler oldStartCdataSectionHandler = startCdataSectionHandler;
  XML_EndCdataSectionHandler oldEndCdataSectionHandler = endCdataSectionHandler;
  XML_DefaultHandler oldDefaultHandler = defaultHandler;
  XML_StartNamespaceDeclHandler oldStartNamespaceDeclHandler = startNamespaceDeclHandler;
  XML_EndNamespaceDeclHandler oldEndNamespaceDeclHandler = endNamespaceDeclHandler;
  XML_NotStandaloneHandler oldNotStandaloneHandler = notStandaloneHandler;
  XML_ExternalEntityRefHandler oldExternalEntityRefHandler = externalEntityRefHandler;
  XML_UnknownEncodingHandler oldUnknownEncodingHandler = unknownEncodingHandler;
  void *oldUserData = userData;
  void *oldHandlerArg = handlerArg;
  int oldDefaultExpandInternalEntities = defaultExpandInternalEntities;
  void *oldExternalEntityRefHandlerArg = externalEntityRefHandlerArg;
 
  parser = (ns
            ? XML_ParserCreateNS(encodingName, namespaceSeparator)
	    : XML_ParserCreate(encodingName));
  if (!parser)
    return 0;
  startElementHandler = oldStartElementHandler;
  endElementHandler = oldEndElementHandler;
  characterDataHandler = oldCharacterDataHandler;
  processingInstructionHandler = oldProcessingInstructionHandler;
  commentHandler = oldCommentHandler;
  startCdataSectionHandler = oldStartCdataSectionHandler;
  endCdataSectionHandler = oldEndCdataSectionHandler;
  defaultHandler = oldDefaultHandler;
  startNamespaceDeclHandler = oldStartNamespaceDeclHandler;
  endNamespaceDeclHandler = oldEndNamespaceDeclHandler;
  notStandaloneHandler = oldNotStandaloneHandler;
  externalEntityRefHandler = oldExternalEntityRefHandler;
  unknownEncodingHandler = oldUnknownEncodingHandler;
  userData = oldUserData;
  if (oldUserData == oldHandlerArg)
    handlerArg = userData;
  else
    handlerArg = parser;
  if (oldExternalEntityRefHandlerArg != oldParser)
    externalEntityRefHandlerArg = oldExternalEntityRefHandlerArg;
  defaultExpandInternalEntities = oldDefaultExpandInternalEntities;
  if (!dtdCopy(&dtd, oldDtd) || !setContext(parser, context)) {
    XML_ParserFree(parser);
    return 0;
  }
  processor = externalEntityInitProcessor;
  return parser;
}

static
void destroyBindings(BINDING *bindings)
{
  for (;;) {
    BINDING *b = bindings;
    if (!b)
      break;
    bindings = b->nextTagBinding;
    free(b->uri);
    free(b);
  }
}

void XML_ParserFree(XML_Parser parser)
{
  for (;;) {
    TAG *p;
    if (tagStack == 0) {
      if (freeTagList == 0)
	break;
      tagStack = freeTagList;
      freeTagList = 0;
    }
    p = tagStack;
    tagStack = tagStack->parent;
    free(p->buf);
    destroyBindings(p->bindings);
    free(p);
  }
  destroyBindings(freeBindingList);
  destroyBindings(inheritedBindings);
  poolDestroy(&tempPool);
  poolDestroy(&temp2Pool);
  dtdDestroy(&dtd);
  free((void *)atts);
  free(groupConnector);
  free(buffer);
  free(dataBuf);
  free(unknownEncodingMem);
  if (unknownEncodingRelease)
    unknownEncodingRelease(unknownEncodingData);
  free(parser);
}

void XML_UseParserAsHandlerArg(XML_Parser parser)
{
  handlerArg = parser;
}

void XML_SetUserData(XML_Parser parser, void *p)
{
  if (handlerArg == userData)
    handlerArg = userData = p;
  else
    userData = p;
}

int XML_SetBase(XML_Parser parser, const XML_Char *p)
{
  if (p) {
    p = poolCopyString(&dtd.pool, p);
    if (!p)
      return 0;
    dtd.base = p;
  }
  else
    dtd.base = 0;
  return 1;
}

const XML_Char *XML_GetBase(XML_Parser parser)
{
  return dtd.base;
}

int XML_GetSpecifiedAttributeCount(XML_Parser parser)
{
  return nSpecifiedAtts;
}

void XML_SetElementHandler(XML_Parser parser,
			   XML_StartElementHandler start,
			   XML_EndElementHandler end)
{
  startElementHandler = start;
  endElementHandler = end;
}

void XML_SetCharacterDataHandler(XML_Parser parser,
				 XML_CharacterDataHandler handler)
{
  characterDataHandler = handler;
}

void XML_SetProcessingInstructionHandler(XML_Parser parser,
					 XML_ProcessingInstructionHandler handler)
{
  processingInstructionHandler = handler;
}

void XML_SetCommentHandler(XML_Parser parser,
			   XML_CommentHandler handler)
{
  commentHandler = handler;
}

void XML_SetCdataSectionHandler(XML_Parser parser,
				XML_StartCdataSectionHandler start,
			        XML_EndCdataSectionHandler end)
{
  startCdataSectionHandler = start;
  endCdataSectionHandler = end;
}

void XML_SetDefaultHandler(XML_Parser parser,
			   XML_DefaultHandler handler)
{
  defaultHandler = handler;
  defaultExpandInternalEntities = 0;
}

void XML_SetDefaultHandlerExpand(XML_Parser parser,
				 XML_DefaultHandler handler)
{
  defaultHandler = handler;
  defaultExpandInternalEntities = 1;
}

void XML_SetUnparsedEntityDeclHandler(XML_Parser parser,
				      XML_UnparsedEntityDeclHandler handler)
{
  unparsedEntityDeclHandler = handler;
}

void XML_SetNotationDeclHandler(XML_Parser parser,
				XML_NotationDeclHandler handler)
{
  notationDeclHandler = handler;
}

void XML_SetNamespaceDeclHandler(XML_Parser parser,
				 XML_StartNamespaceDeclHandler start,
				 XML_EndNamespaceDeclHandler end)
{
  startNamespaceDeclHandler = start;
  endNamespaceDeclHandler = end;
}

void XML_SetNotStandaloneHandler(XML_Parser parser,
				 XML_NotStandaloneHandler handler)
{
  notStandaloneHandler = handler;
}

void XML_SetExternalEntityRefHandler(XML_Parser parser,
				     XML_ExternalEntityRefHandler handler)
{
  externalEntityRefHandler = handler;
}

void XML_SetExternalEntityRefHandlerArg(XML_Parser parser, void *arg)
{
  if (arg)
    externalEntityRefHandlerArg = arg;
  else
    externalEntityRefHandlerArg = parser;
}

void XML_SetUnknownEncodingHandler(XML_Parser parser,
				   XML_UnknownEncodingHandler handler,
				   void *data)
{
  unknownEncodingHandler = handler;
  unknownEncodingHandlerData = data;
}

int XML_Parse(XML_Parser parser, const char *s, int len, int isFinal)
{
  if (len == 0) {
    if (!isFinal)
      return 1;
    positionPtr = bufferPtr;
    errorCode = processor(parser, bufferPtr, parseEndPtr = bufferEnd, 0);
    if (errorCode == XML_ERROR_NONE)
      return 1;
    eventEndPtr = eventPtr;
    return 0;
  }
  else if (bufferPtr == bufferEnd) {
    const char *end;
    int nLeftOver;
    parseEndByteIndex += len;
    positionPtr = s;
    if (isFinal) {
      errorCode = processor(parser, s, parseEndPtr = s + len, 0);
      if (errorCode == XML_ERROR_NONE)
	return 1;
      eventEndPtr = eventPtr;
      return 0;
    }
    errorCode = processor(parser, s, parseEndPtr = s + len, &end);
    if (errorCode != XML_ERROR_NONE) {
      eventEndPtr = eventPtr;
      return 0;
    }
    XmlUpdatePosition(encoding, positionPtr, end, &position);
    nLeftOver = s + len - end;
    if (nLeftOver) {
      if (buffer == 0 || nLeftOver > bufferLim - buffer) {
	/* FIXME avoid integer overflow */
	buffer = buffer == 0 ? malloc(len * 2) : realloc(buffer, len * 2);
	if (!buffer) {
	  errorCode = XML_ERROR_NO_MEMORY;
	  eventPtr = eventEndPtr = 0;
	  return 0;
	}
	bufferLim = buffer + len * 2;
      }
      memcpy(buffer, end, nLeftOver);
      bufferPtr = buffer;
      bufferEnd = buffer + nLeftOver;
    }
    return 1;
  }
  else {
    memcpy(XML_GetBuffer(parser, len), s, len);
    return XML_ParseBuffer(parser, len, isFinal);
  }
}

int XML_ParseBuffer(XML_Parser parser, int len, int isFinal)
{
  const char *start = bufferPtr;
  positionPtr = start;
  bufferEnd += len;
  parseEndByteIndex += len;
  errorCode = processor(parser, start, parseEndPtr = bufferEnd,
			isFinal ? (const char **)0 : &bufferPtr);
  if (errorCode == XML_ERROR_NONE) {
    if (!isFinal)
      XmlUpdatePosition(encoding, positionPtr, bufferPtr, &position);
    return 1;
  }
  else {
    eventEndPtr = eventPtr;
    return 0;
  }
}

void *XML_GetBuffer(XML_Parser parser, int len)
{
  if (len > bufferLim - bufferEnd) {
    /* FIXME avoid integer overflow */
    int neededSize = len + (bufferEnd - bufferPtr);
    if (neededSize  <= bufferLim - buffer) {
      memmove(buffer, bufferPtr, bufferEnd - bufferPtr);
      bufferEnd = buffer + (bufferEnd - bufferPtr);
      bufferPtr = buffer;
    }
    else {
      char *newBuf;
      int bufferSize = bufferLim - bufferPtr;
      if (bufferSize == 0)
	bufferSize = INIT_BUFFER_SIZE;
      do {
	bufferSize *= 2;
      } while (bufferSize < neededSize);
      newBuf = malloc(bufferSize);
      if (newBuf == 0) {
	errorCode = XML_ERROR_NO_MEMORY;
	return 0;
      }
      bufferLim = newBuf + bufferSize;
      if (bufferPtr) {
	memcpy(newBuf, bufferPtr, bufferEnd - bufferPtr);
	free(buffer);
      }
      bufferEnd = newBuf + (bufferEnd - bufferPtr);
      bufferPtr = buffer = newBuf;
    }
  }
  return bufferEnd;
}

enum XML_Error XML_GetErrorCode(XML_Parser parser)
{
  return errorCode;
}

long XML_GetCurrentByteIndex(XML_Parser parser)
{
  if (eventPtr)
    return parseEndByteIndex - (parseEndPtr - eventPtr);
  return -1;
}

int XML_GetCurrentByteCount(XML_Parser parser)
{
  if (eventEndPtr && eventPtr)
    return eventEndPtr - eventPtr;
  return 0;
}

int XML_GetCurrentLineNumber(XML_Parser parser)
{
  if (eventPtr) {
    XmlUpdatePosition(encoding, positionPtr, eventPtr, &position);
    positionPtr = eventPtr;
  }
  return position.lineNumber + 1;
}

int XML_GetCurrentColumnNumber(XML_Parser parser)
{
  if (eventPtr) {
    XmlUpdatePosition(encoding, positionPtr, eventPtr, &position);
    positionPtr = eventPtr;
  }
  return position.columnNumber;
}

void XML_DefaultCurrent(XML_Parser parser)
{
  if (defaultHandler) {
    if (openInternalEntities)
      reportDefault(parser,
	            ns ? XmlGetInternalEncodingNS() : XmlGetInternalEncoding(),
		    openInternalEntities->internalEventPtr,
		    openInternalEntities->internalEventEndPtr);
    else
      reportDefault(parser, encoding, eventPtr, eventEndPtr);
  }
}

const XML_LChar *XML_ErrorString(int code)
{
  static const XML_LChar *message[] = {
    0,
    XML_T("out of memory"),
    XML_T("syntax error"),
    XML_T("no element found"),
    XML_T("not well-formed"),
    XML_T("unclosed token"),
    XML_T("unclosed token"),
    XML_T("mismatched tag"),
    XML_T("duplicate attribute"),
    XML_T("junk after document element"),
    XML_T("illegal parameter entity reference"),
    XML_T("undefined entity"),
    XML_T("recursive entity reference"),
    XML_T("asynchronous entity"),
    XML_T("reference to invalid character number"),
    XML_T("reference to binary entity"),
    XML_T("reference to external entity in attribute"),
    XML_T("xml processing instruction not at start of external entity"),
    XML_T("unknown encoding"),
    XML_T("encoding specified in XML declaration is incorrect"),
    XML_T("unclosed CDATA section"),
    XML_T("error in processing external entity reference"),
    XML_T("document is not standalone")
  };
  if (code > 0 && code < sizeof(message)/sizeof(message[0]))
    return message[code];
  return 0;
}

static
enum XML_Error contentProcessor(XML_Parser parser,
				const char *start,
				const char *end,
				const char **endPtr)
{
  return doContent(parser, 0, encoding, start, end, endPtr);
}

static
enum XML_Error externalEntityInitProcessor(XML_Parser parser,
					   const char *start,
					   const char *end,
					   const char **endPtr)
{
  enum XML_Error result = initializeEncoding(parser);
  if (result != XML_ERROR_NONE)
    return result;
  processor = externalEntityInitProcessor2;
  return externalEntityInitProcessor2(parser, start, end, endPtr);
}

static
enum XML_Error externalEntityInitProcessor2(XML_Parser parser,
					    const char *start,
					    const char *end,
					    const char **endPtr)
{
  const char *next;
  int tok = XmlContentTok(encoding, start, end, &next);
  switch (tok) {
  case XML_TOK_BOM:
    start = next;
    break;
  case XML_TOK_PARTIAL:
    if (endPtr) {
      *endPtr = start;
      return XML_ERROR_NONE;
    }
    eventPtr = start;
    return XML_ERROR_UNCLOSED_TOKEN;
  case XML_TOK_PARTIAL_CHAR:
    if (endPtr) {
      *endPtr = start;
      return XML_ERROR_NONE;
    }
    eventPtr = start;
    return XML_ERROR_PARTIAL_CHAR;
  }
  processor = externalEntityInitProcessor3;
  return externalEntityInitProcessor3(parser, start, end, endPtr);
}

static
enum XML_Error externalEntityInitProcessor3(XML_Parser parser,
					    const char *start,
					    const char *end,
					    const char **endPtr)
{
  const char *next;
  int tok = XmlContentTok(encoding, start, end, &next);
  switch (tok) {
  case XML_TOK_XML_DECL:
    {
      enum XML_Error result = processXmlDecl(parser, 1, start, next);
      if (result != XML_ERROR_NONE)
	return result;
      start = next;
    }
    break;
  case XML_TOK_PARTIAL:
    if (endPtr) {
      *endPtr = start;
      return XML_ERROR_NONE;
    }
    eventPtr = start;
    return XML_ERROR_UNCLOSED_TOKEN;
  case XML_TOK_PARTIAL_CHAR:
    if (endPtr) {
      *endPtr = start;
      return XML_ERROR_NONE;
    }
    eventPtr = start;
    return XML_ERROR_PARTIAL_CHAR;
  }
  processor = externalEntityContentProcessor;
  tagLevel = 1;
  return doContent(parser, 1, encoding, start, end, endPtr);
}

static
enum XML_Error externalEntityContentProcessor(XML_Parser parser,
					      const char *start,
					      const char *end,
					      const char **endPtr)
{
  return doContent(parser, 1, encoding, start, end, endPtr);
}

static enum XML_Error
doContent(XML_Parser parser,
	  int startTagLevel,
	  const ENCODING *enc,
	  const char *s,
	  const char *end,
	  const char **nextPtr)
{
  const ENCODING *internalEnc = ns ? XmlGetInternalEncodingNS() : XmlGetInternalEncoding();
  const char **eventPP;
  const char **eventEndPP;
  if (enc == encoding) {
    eventPP = &eventPtr;
    eventEndPP = &eventEndPtr;
  }
  else {
    eventPP = &(openInternalEntities->internalEventPtr);
    eventEndPP = &(openInternalEntities->internalEventEndPtr);
  }
  *eventPP = s;
  for (;;) {
    const char *next = s; /* XmlContentTok doesn't always set the last arg */
    int tok = XmlContentTok(enc, s, end, &next);
    *eventEndPP = next;
    switch (tok) {
    case XML_TOK_TRAILING_CR:
      if (nextPtr) {
	*nextPtr = s;
	return XML_ERROR_NONE;
      }
      *eventEndPP = end;
      if (characterDataHandler) {
	XML_Char c = 0xA;
	characterDataHandler(handlerArg, &c, 1);
      }
      else if (defaultHandler)
	reportDefault(parser, enc, s, end);
      if (startTagLevel == 0)
	return XML_ERROR_NO_ELEMENTS;
      if (tagLevel != startTagLevel)
	return XML_ERROR_ASYNC_ENTITY;
      return XML_ERROR_NONE;
    case XML_TOK_NONE:
      if (nextPtr) {
	*nextPtr = s;
	return XML_ERROR_NONE;
      }
      if (startTagLevel > 0) {
	if (tagLevel != startTagLevel)
	  return XML_ERROR_ASYNC_ENTITY;
	return XML_ERROR_NONE;
      }
      return XML_ERROR_NO_ELEMENTS;
    case XML_TOK_INVALID:
      *eventPP = next;
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_PARTIAL:
      if (nextPtr) {
	*nextPtr = s;
	return XML_ERROR_NONE;
      }
      return XML_ERROR_UNCLOSED_TOKEN;
    case XML_TOK_PARTIAL_CHAR:
      if (nextPtr) {
	*nextPtr = s;
	return XML_ERROR_NONE;
      }
      return XML_ERROR_PARTIAL_CHAR;
    case XML_TOK_ENTITY_REF:
      {
	const XML_Char *name;
	ENTITY *entity;
	XML_Char ch = XmlPredefinedEntityName(enc,
					      s + enc->minBytesPerChar,
					      next - enc->minBytesPerChar);
	if (ch) {
	  if (characterDataHandler)
	    characterDataHandler(handlerArg, &ch, 1);
	  else if (defaultHandler)
	    reportDefault(parser, enc, s, next);
	  break;
	}
	name = poolStoreString(&dtd.pool, enc,
				s + enc->minBytesPerChar,
				next - enc->minBytesPerChar);
	if (!name)
	  return XML_ERROR_NO_MEMORY;
	entity = (ENTITY *)lookup(&dtd.generalEntities, name, 0);
	poolDiscard(&dtd.pool);
	if (!entity) {
	  if (dtd.complete || dtd.standalone)
	    return XML_ERROR_UNDEFINED_ENTITY;
	  if (defaultHandler)
	    reportDefault(parser, enc, s, next);
	  break;
	}
	if (entity->open)
	  return XML_ERROR_RECURSIVE_ENTITY_REF;
	if (entity->notation)
	  return XML_ERROR_BINARY_ENTITY_REF;
	if (entity) {
	  if (entity->textPtr) {
	    enum XML_Error result;
	    OPEN_INTERNAL_ENTITY openEntity;
	    if (defaultHandler && !defaultExpandInternalEntities) {
	      reportDefault(parser, enc, s, next);
	      break;
	    }
	    entity->open = 1;
	    openEntity.next = openInternalEntities;
	    openInternalEntities = &openEntity;
	    openEntity.entity = entity;
	    openEntity.internalEventPtr = 0;
	    openEntity.internalEventEndPtr = 0;
	    result = doContent(parser,
			       tagLevel,
			       internalEnc,
			       (char *)entity->textPtr,
			       (char *)(entity->textPtr + entity->textLen),
			       0);
	    entity->open = 0;
	    openInternalEntities = openEntity.next;
	    if (result)
	      return result;
	  }
	  else if (externalEntityRefHandler) {
	    const XML_Char *context;
	    entity->open = 1;
	    context = getContext(parser);
	    entity->open = 0;
	    if (!context)
	      return XML_ERROR_NO_MEMORY;
	    if (!externalEntityRefHandler(externalEntityRefHandlerArg,
				          context,
					  dtd.base,
					  entity->systemId,
					  entity->publicId))
	      return XML_ERROR_EXTERNAL_ENTITY_HANDLING;
	    poolDiscard(&tempPool);
	  }
	  else if (defaultHandler)
	    reportDefault(parser, enc, s, next);
	}
	break;
      }
    case XML_TOK_START_TAG_WITH_ATTS:
      if (!startElementHandler) {
	enum XML_Error result = storeAtts(parser, enc, s, 0, 0);
	if (result)
	  return result;
      }
      /* fall through */
    case XML_TOK_START_TAG_NO_ATTS:
      {
	TAG *tag;
	if (freeTagList) {
	  tag = freeTagList;
	  freeTagList = freeTagList->parent;
	}
	else {
	  tag = malloc(sizeof(TAG));
	  if (!tag)
	    return XML_ERROR_NO_MEMORY;
	  tag->buf = malloc(INIT_TAG_BUF_SIZE);
	  if (!tag->buf)
	    return XML_ERROR_NO_MEMORY;
	  tag->bufEnd = tag->buf + INIT_TAG_BUF_SIZE;
	}
	tag->bindings = 0;
	tag->parent = tagStack;
	tagStack = tag;
	tag->name.localPart = 0;
	tag->rawName = s + enc->minBytesPerChar;
	tag->rawNameLength = XmlNameLength(enc, tag->rawName);
	if (nextPtr) {
	  /* Need to guarantee that:
	     tag->buf + ROUND_UP(tag->rawNameLength, sizeof(XML_Char)) <= tag->bufEnd - sizeof(XML_Char) */
	  if (tag->rawNameLength + (int)(sizeof(XML_Char) - 1) + (int)sizeof(XML_Char) > tag->bufEnd - tag->buf) {
	    int bufSize = tag->rawNameLength * 4;
	    bufSize = ROUND_UP(bufSize, sizeof(XML_Char));
	    tag->buf = realloc(tag->buf, bufSize);
	    if (!tag->buf)
	      return XML_ERROR_NO_MEMORY;
	    tag->bufEnd = tag->buf + bufSize;
	  }
	  memcpy(tag->buf, tag->rawName, tag->rawNameLength);
	  tag->rawName = tag->buf;
	}
	++tagLevel;
	if (startElementHandler) {
	  enum XML_Error result;
	  XML_Char *toPtr;
	  for (;;) {
	    const char *rawNameEnd = tag->rawName + tag->rawNameLength;
	    const char *fromPtr = tag->rawName;
	    int bufSize;
	    if (nextPtr)
	      toPtr = (XML_Char *)(tag->buf + ROUND_UP(tag->rawNameLength, sizeof(XML_Char)));
	    else
	      toPtr = (XML_Char *)tag->buf;
	    tag->name.str = toPtr;
	    XmlConvert(enc,
		       &fromPtr, rawNameEnd,
		       (ICHAR **)&toPtr, (ICHAR *)tag->bufEnd - 1);
	    if (fromPtr == rawNameEnd)
	      break;
	    bufSize = (tag->bufEnd - tag->buf) << 1;
	    tag->buf = realloc(tag->buf, bufSize);
	    if (!tag->buf)
	      return XML_ERROR_NO_MEMORY;
	    tag->bufEnd = tag->buf + bufSize;
	    if (nextPtr)
	      tag->rawName = tag->buf;
	  }
	  *toPtr = XML_T('\0');
	  result = storeAtts(parser, enc, s, &(tag->name), &(tag->bindings));
	  if (result)
	    return result;
	  startElementHandler(handlerArg, tag->name.str, (const XML_Char **)atts);
	  poolClear(&tempPool);
	}
	else {
	  tag->name.str = 0;
	  if (defaultHandler)
	    reportDefault(parser, enc, s, next);
	}
	break;
      }
    case XML_TOK_EMPTY_ELEMENT_WITH_ATTS:
      if (!startElementHandler) {
	enum XML_Error result = storeAtts(parser, enc, s, 0, 0);
	if (result)
	  return result;
      }
      /* fall through */
    case XML_TOK_EMPTY_ELEMENT_NO_ATTS:
      if (startElementHandler || endElementHandler) {
	const char *rawName = s + enc->minBytesPerChar;
	enum XML_Error result;
	BINDING *bindings = 0;
	TAG_NAME name;
	name.str = poolStoreString(&tempPool, enc, rawName,
				   rawName + XmlNameLength(enc, rawName));
	if (!name.str)
	  return XML_ERROR_NO_MEMORY;
	poolFinish(&tempPool);
	result = storeAtts(parser, enc, s, &name, &bindings);
	if (result)
	  return result;
	poolFinish(&tempPool);
	if (startElementHandler)
	  startElementHandler(handlerArg, name.str, (const XML_Char **)atts);
	if (endElementHandler) {
	  if (startElementHandler)
	    *eventPP = *eventEndPP;
	  endElementHandler(handlerArg, name.str);
	}
	poolClear(&tempPool);
	while (bindings) {
	  BINDING *b = bindings;
	  if (endNamespaceDeclHandler)
	    endNamespaceDeclHandler(handlerArg, b->prefix->name);
	  bindings = bindings->nextTagBinding;
	  b->nextTagBinding = freeBindingList;
	  freeBindingList = b;
	  b->prefix->binding = b->prevPrefixBinding;
	}
      }
      else if (defaultHandler)
	reportDefault(parser, enc, s, next);
      if (tagLevel == 0)
	return epilogProcessor(parser, next, end, nextPtr);
      break;
    case XML_TOK_END_TAG:
      if (tagLevel == startTagLevel)
        return XML_ERROR_ASYNC_ENTITY;
      else {
	int len;
	const char *rawName;
	TAG *tag = tagStack;
	tagStack = tag->parent;
	tag->parent = freeTagList;
	freeTagList = tag;
	rawName = s + enc->minBytesPerChar*2;
	len = XmlNameLength(enc, rawName);
	if (len != tag->rawNameLength
	    || memcmp(tag->rawName, rawName, len) != 0) {
	  *eventPP = rawName;
	  return XML_ERROR_TAG_MISMATCH;
	}
	--tagLevel;
	if (endElementHandler && tag->name.str) {
	  if (tag->name.localPart) {
	    XML_Char *to = (XML_Char *)tag->name.str + tag->name.uriLen;
	    const XML_Char *from = tag->name.localPart;
	    while ((*to++ = *from++) != 0)
	      ;
	  }
	  endElementHandler(handlerArg, tag->name.str);
	}
	else if (defaultHandler)
	  reportDefault(parser, enc, s, next);
	while (tag->bindings) {
	  BINDING *b = tag->bindings;
	  if (endNamespaceDeclHandler)
	    endNamespaceDeclHandler(handlerArg, b->prefix->name);
	  tag->bindings = tag->bindings->nextTagBinding;
	  b->nextTagBinding = freeBindingList;
	  freeBindingList = b;
	  b->prefix->binding = b->prevPrefixBinding;
	}
	if (tagLevel == 0)
	  return epilogProcessor(parser, next, end, nextPtr);
      }
      break;
    case XML_TOK_CHAR_REF:
      {
	int n = XmlCharRefNumber(enc, s);
	if (n < 0)
	  return XML_ERROR_BAD_CHAR_REF;
	if (characterDataHandler) {
	  XML_Char buf[XML_ENCODE_MAX];
	  characterDataHandler(handlerArg, buf, XmlEncode(n, (ICHAR *)buf));
	}
	else if (defaultHandler)
	  reportDefault(parser, enc, s, next);
      }
      break;
    case XML_TOK_XML_DECL:
      return XML_ERROR_MISPLACED_XML_PI;
    case XML_TOK_DATA_NEWLINE:
      if (characterDataHandler) {
	XML_Char c = 0xA;
	characterDataHandler(handlerArg, &c, 1);
      }
      else if (defaultHandler)
	reportDefault(parser, enc, s, next);
      break;
    case XML_TOK_CDATA_SECT_OPEN:
      {
	enum XML_Error result;
	if (startCdataSectionHandler)
  	  startCdataSectionHandler(handlerArg);
#if 0
	/* Suppose you doing a transformation on a document that involves
	   changing only the character data.  You set up a defaultHandler
	   and a characterDataHandler.  The defaultHandler simply copies
	   characters through.  The characterDataHandler does the transformation
	   and writes the characters out escaping them as necessary.  This case
	   will fail to work if we leave out the following two lines (because &
	   and < inside CDATA sections will be incorrectly escaped).

	   However, now we have a start/endCdataSectionHandler, so it seems
	   easier to let the user deal with this. */

	else if (characterDataHandler)
  	  characterDataHandler(handlerArg, dataBuf, 0);
#endif
	else if (defaultHandler)
	  reportDefault(parser, enc, s, next);
	result = doCdataSection(parser, enc, &next, end, nextPtr);
	if (!next) {
	  processor = cdataSectionProcessor;
	  return result;
	}
      }
      break;
    case XML_TOK_TRAILING_RSQB:
      if (nextPtr) {
	*nextPtr = s;
	return XML_ERROR_NONE;
      }
      if (characterDataHandler) {
	if (MUST_CONVERT(enc, s)) {
	  ICHAR *dataPtr = (ICHAR *)dataBuf;
	  XmlConvert(enc, &s, end, &dataPtr, (ICHAR *)dataBufEnd);
	  characterDataHandler(handlerArg, dataBuf, dataPtr - (ICHAR *)dataBuf);
	}
	else
	  characterDataHandler(handlerArg,
		  	       (XML_Char *)s,
			       (XML_Char *)end - (XML_Char *)s);
      }
      else if (defaultHandler)
	reportDefault(parser, enc, s, end);
      if (startTagLevel == 0) {
        *eventPP = end;
	return XML_ERROR_NO_ELEMENTS;
      }
      if (tagLevel != startTagLevel) {
	*eventPP = end;
	return XML_ERROR_ASYNC_ENTITY;
      }
      return XML_ERROR_NONE;
    case XML_TOK_DATA_CHARS:
      if (characterDataHandler) {
	if (MUST_CONVERT(enc, s)) {
	  for (;;) {
	    ICHAR *dataPtr = (ICHAR *)dataBuf;
	    XmlConvert(enc, &s, next, &dataPtr, (ICHAR *)dataBufEnd);
	    *eventEndPP = s;
	    characterDataHandler(handlerArg, dataBuf, dataPtr - (ICHAR *)dataBuf);
	    if (s == next)
	      break;
	    *eventPP = s;
	  }
	}
	else
	  characterDataHandler(handlerArg,
			       (XML_Char *)s,
			       (XML_Char *)next - (XML_Char *)s);
      }
      else if (defaultHandler)
	reportDefault(parser, enc, s, next);
      break;
    case XML_TOK_PI:
      if (!reportProcessingInstruction(parser, enc, s, next))
	return XML_ERROR_NO_MEMORY;
      break;
    case XML_TOK_COMMENT:
      if (!reportComment(parser, enc, s, next))
	return XML_ERROR_NO_MEMORY;
      break;
    default:
      if (defaultHandler)
	reportDefault(parser, enc, s, next);
      break;
    }
    *eventPP = s = next;
  }
  /* not reached */
}

/* If tagNamePtr is non-null, build a real list of attributes,
otherwise just check the attributes for well-formedness. */

static enum XML_Error storeAtts(XML_Parser parser, const ENCODING *enc,
				const char *s, TAG_NAME *tagNamePtr,
				BINDING **bindingsPtr)
{
  ELEMENT_TYPE *elementType = 0;
  int nDefaultAtts = 0;
  const XML_Char **appAtts;
  int attIndex = 0;
  int i;
  int n;
  int nPrefixes = 0;
  BINDING *binding;
  const XML_Char *localPart;

  if (tagNamePtr) {
    elementType = (ELEMENT_TYPE *)lookup(&dtd.elementTypes, tagNamePtr->str, 0);
    if (!elementType) {
      tagNamePtr->str = poolCopyString(&dtd.pool, tagNamePtr->str);
      if (!tagNamePtr->str)
	return XML_ERROR_NO_MEMORY;
      elementType = (ELEMENT_TYPE *)lookup(&dtd.elementTypes, tagNamePtr->str, sizeof(ELEMENT_TYPE));
      if (!elementType)
        return XML_ERROR_NO_MEMORY;
      if (ns && !setElementTypePrefix(parser, elementType))
        return XML_ERROR_NO_MEMORY;
    }
    nDefaultAtts = elementType->nDefaultAtts;
  }
  n = XmlGetAttributes(enc, s, attsSize, atts);
  if (n + nDefaultAtts > attsSize) {
    int oldAttsSize = attsSize;
    attsSize = n + nDefaultAtts + INIT_ATTS_SIZE;
    atts = realloc((void *)atts, attsSize * sizeof(ATTRIBUTE));
    if (!atts)
      return XML_ERROR_NO_MEMORY;
    if (n > oldAttsSize)
      XmlGetAttributes(enc, s, n, atts);
  }
  appAtts = (const XML_Char **)atts;
  for (i = 0; i < n; i++) {
    ATTRIBUTE_ID *attId = getAttributeId(parser, enc, atts[i].name,
					 atts[i].name
					 + XmlNameLength(enc, atts[i].name));
    if (!attId)
      return XML_ERROR_NO_MEMORY;
    if ((attId->name)[-1]) {
      if (enc == encoding)
	eventPtr = atts[i].name;
      return XML_ERROR_DUPLICATE_ATTRIBUTE;
    }
    (attId->name)[-1] = 1;
    appAtts[attIndex++] = attId->name;
    if (!atts[i].normalized) {
      enum XML_Error result;
      int isCdata = 1;

      if (attId->maybeTokenized) {
	int j;
	for (j = 0; j < nDefaultAtts; j++) {
	  if (attId == elementType->defaultAtts[j].id) {
	    isCdata = elementType->defaultAtts[j].isCdata;
	    break;
	  }
	}
      }

      result = storeAttributeValue(parser, enc, isCdata,
				   atts[i].valuePtr, atts[i].valueEnd,
			           &tempPool);
      if (result)
	return result;
      if (tagNamePtr) {
	appAtts[attIndex] = poolStart(&tempPool);
	poolFinish(&tempPool);
      }
      else
	poolDiscard(&tempPool);
    }
    else if (tagNamePtr) {
      appAtts[attIndex] = poolStoreString(&tempPool, enc, atts[i].valuePtr, atts[i].valueEnd);
      if (appAtts[attIndex] == 0)
	return XML_ERROR_NO_MEMORY;
      poolFinish(&tempPool);
    }
    if (attId->prefix && tagNamePtr) {
      if (attId->xmlns) {
        if (!addBinding(parser, attId->prefix, attId, appAtts[attIndex], bindingsPtr))
          return XML_ERROR_NO_MEMORY;
        --attIndex;
      }
      else {
        attIndex++;
        nPrefixes++;
        (attId->name)[-1] = 2;
      }
    }
    else
      attIndex++;
  }
  nSpecifiedAtts = attIndex;
  if (tagNamePtr) {
    int j;
    for (j = 0; j < nDefaultAtts; j++) {
      const DEFAULT_ATTRIBUTE *da = elementType->defaultAtts + j;
      if (!(da->id->name)[-1] && da->value) {
        if (da->id->prefix) {
          if (da->id->xmlns) {
	    if (!addBinding(parser, da->id->prefix, da->id, da->value, bindingsPtr))
	      return XML_ERROR_NO_MEMORY;
	  }
          else {
	    (da->id->name)[-1] = 2;
	    nPrefixes++;
  	    appAtts[attIndex++] = da->id->name;
	    appAtts[attIndex++] = da->value;
	  }
	}
	else {
	  (da->id->name)[-1] = 1;
	  appAtts[attIndex++] = da->id->name;
	  appAtts[attIndex++] = da->value;
	}
      }
    }
    appAtts[attIndex] = 0;
  }
  i = 0;
  if (nPrefixes) {
    for (; i < attIndex; i += 2) {
      if (appAtts[i][-1] == 2) {
        ATTRIBUTE_ID *id;
        ((XML_Char *)(appAtts[i]))[-1] = 0;
	id = (ATTRIBUTE_ID *)lookup(&dtd.attributeIds, appAtts[i], 0);
	if (id->prefix->binding) {
	  int j;
	  const BINDING *b = id->prefix->binding;
	  const XML_Char *s = appAtts[i];
	  for (j = 0; j < b->uriLen; j++) {
	    if (!poolAppendChar(&tempPool, b->uri[j]))
	      return XML_ERROR_NO_MEMORY;
	  }
	  while (*s++ != ':')
	    ;
	  do {
	    if (!poolAppendChar(&tempPool, *s))
	      return XML_ERROR_NO_MEMORY;
	  } while (*s++);
	  appAtts[i] = poolStart(&tempPool);
	  poolFinish(&tempPool);
	}
	if (!--nPrefixes)
	  break;
      }
      else
	((XML_Char *)(appAtts[i]))[-1] = 0;
    }
  }
  for (; i < attIndex; i += 2)
    ((XML_Char *)(appAtts[i]))[-1] = 0;
  if (!tagNamePtr)
    return XML_ERROR_NONE;
  for (binding = *bindingsPtr; binding; binding = binding->nextTagBinding)
    binding->attId->name[-1] = 0;
  if (elementType->prefix) {
    binding = elementType->prefix->binding;
    if (!binding)
      return XML_ERROR_NONE;
    localPart = tagNamePtr->str;
    while (*localPart++ != XML_T(':'))
      ;
  }
  else if (dtd.defaultPrefix.binding) {
    binding = dtd.defaultPrefix.binding;
    localPart = tagNamePtr->str;
  }
  else
    return XML_ERROR_NONE;
  tagNamePtr->localPart = localPart;
  tagNamePtr->uriLen = binding->uriLen;
  i = binding->uriLen;
  do {
    if (i == binding->uriAlloc) {
      binding->uri = realloc(binding->uri, binding->uriAlloc *= 2);
      if (!binding->uri)
	return XML_ERROR_NO_MEMORY;
    }
    binding->uri[i++] = *localPart;
  } while (*localPart++);
  tagNamePtr->str = binding->uri;
  return XML_ERROR_NONE;
}

static
int addBinding(XML_Parser parser, PREFIX *prefix, const ATTRIBUTE_ID *attId, const XML_Char *uri, BINDING **bindingsPtr)
{
  BINDING *b;
  int len;
  for (len = 0; uri[len]; len++)
    ;
  if (namespaceSeparator)
    len++;
  if (freeBindingList) {
    b = freeBindingList;
    if (len > b->uriAlloc) {
      b->uri = realloc(b->uri, len + EXPAND_SPARE);
      if (!b->uri)
	return 0;
      b->uriAlloc = len + EXPAND_SPARE;
    }
    freeBindingList = b->nextTagBinding;
  }
  else {
    b = malloc(sizeof(BINDING));
    if (!b)
      return 0;
    b->uri = malloc(sizeof(XML_Char) * len + EXPAND_SPARE);
    if (!b->uri) {
      free(b);
      return 0;
    }
    b->uriAlloc = len;
  }
  b->uriLen = len;
  memcpy(b->uri, uri, len * sizeof(XML_Char));
  if (namespaceSeparator)
    b->uri[len - 1] = namespaceSeparator;
  b->prefix = prefix;
  b->attId = attId;
  b->prevPrefixBinding = prefix->binding;
  if (*uri == XML_T('\0') && prefix == &dtd.defaultPrefix)
    prefix->binding = 0;
  else
    prefix->binding = b;
  b->nextTagBinding = *bindingsPtr;
  *bindingsPtr = b;
  if (startNamespaceDeclHandler)
    startNamespaceDeclHandler(handlerArg, prefix->name,
			      prefix->binding ? uri : 0);
  return 1;
}

/* The idea here is to avoid using stack for each CDATA section when
the whole file is parsed with one call. */

static
enum XML_Error cdataSectionProcessor(XML_Parser parser,
				     const char *start,
			    	     const char *end,
				     const char **endPtr)
{
  enum XML_Error result = doCdataSection(parser, encoding, &start, end, endPtr);
  if (start) {
    processor = contentProcessor;
    return contentProcessor(parser, start, end, endPtr);
  }
  return result;
}

/* startPtr gets set to non-null is the section is closed, and to null if
the section is not yet closed. */

static
enum XML_Error doCdataSection(XML_Parser parser,
			      const ENCODING *enc,
			      const char **startPtr,
			      const char *end,
			      const char **nextPtr)
{
  const char *s = *startPtr;
  const char **eventPP;
  const char **eventEndPP;
  if (enc == encoding) {
    eventPP = &eventPtr;
    *eventPP = s;
    eventEndPP = &eventEndPtr;
  }
  else {
    eventPP = &(openInternalEntities->internalEventPtr);
    eventEndPP = &(openInternalEntities->internalEventEndPtr);
  }
  *eventPP = s;
  *startPtr = 0;
  for (;;) {
    const char *next;
    int tok = XmlCdataSectionTok(enc, s, end, &next);
    *eventEndPP = next;
    switch (tok) {
    case XML_TOK_CDATA_SECT_CLOSE:
      if (endCdataSectionHandler)
	endCdataSectionHandler(handlerArg);
#if 0
      /* see comment under XML_TOK_CDATA_SECT_OPEN */
      else if (characterDataHandler)
	characterDataHandler(handlerArg, dataBuf, 0);
#endif
      else if (defaultHandler)
	reportDefault(parser, enc, s, next);
      *startPtr = next;
      return XML_ERROR_NONE;
    case XML_TOK_DATA_NEWLINE:
      if (characterDataHandler) {
	XML_Char c = 0xA;
	characterDataHandler(handlerArg, &c, 1);
      }
      else if (defaultHandler)
	reportDefault(parser, enc, s, next);
      break;
    case XML_TOK_DATA_CHARS:
      if (characterDataHandler) {
	if (MUST_CONVERT(enc, s)) {
	  for (;;) {
  	    ICHAR *dataPtr = (ICHAR *)dataBuf;
	    XmlConvert(enc, &s, next, &dataPtr, (ICHAR *)dataBufEnd);
	    *eventEndPP = next;
	    characterDataHandler(handlerArg, dataBuf, dataPtr - (ICHAR *)dataBuf);
	    if (s == next)
	      break;
	    *eventPP = s;
	  }
	}
	else
	  characterDataHandler(handlerArg,
		  	       (XML_Char *)s,
			       (XML_Char *)next - (XML_Char *)s);
      }
      else if (defaultHandler)
	reportDefault(parser, enc, s, next);
      break;
    case XML_TOK_INVALID:
      *eventPP = next;
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_PARTIAL_CHAR:
      if (nextPtr) {
	*nextPtr = s;
	return XML_ERROR_NONE;
      }
      return XML_ERROR_PARTIAL_CHAR;
    case XML_TOK_PARTIAL:
    case XML_TOK_NONE:
      if (nextPtr) {
	*nextPtr = s;
	return XML_ERROR_NONE;
      }
      return XML_ERROR_UNCLOSED_CDATA_SECTION;
    default:
      abort();
    }
    *eventPP = s = next;
  }
  /* not reached */
}

static enum XML_Error
initializeEncoding(XML_Parser parser)
{
  const char *s;
#ifdef XML_UNICODE
  char encodingBuf[128];
  if (!protocolEncodingName)
    s = 0;
  else {
    int i;
    for (i = 0; protocolEncodingName[i]; i++) {
      if (i == sizeof(encodingBuf) - 1
	  || protocolEncodingName[i] >= 0x80
	  || protocolEncodingName[i] < 0) {
	encodingBuf[0] = '\0';
	break;
      }
      encodingBuf[i] = (char)protocolEncodingName[i];
    }
    encodingBuf[i] = '\0';
    s = encodingBuf;
  }
#else
  s = protocolEncodingName;
#endif
  if ((ns ? XmlInitEncodingNS : XmlInitEncoding)(&initEncoding, &encoding, s))
    return XML_ERROR_NONE;
  return handleUnknownEncoding(parser, protocolEncodingName);
}

static enum XML_Error
processXmlDecl(XML_Parser parser, int isGeneralTextEntity,
	       const char *s, const char *next)
{
  const char *encodingName = 0;
  const ENCODING *newEncoding = 0;
  const char *version;
  int standalone = -1;
  if (!(ns
        ? XmlParseXmlDeclNS
	: XmlParseXmlDecl)(isGeneralTextEntity,
		           encoding,
		           s,
		           next,
		           &eventPtr,
		           &version,
		           &encodingName,
		           &newEncoding,
		           &standalone))
    return XML_ERROR_SYNTAX;
  if (!isGeneralTextEntity && standalone == 1)
    dtd.standalone = 1;
  if (defaultHandler)
    reportDefault(parser, encoding, s, next);
  if (!protocolEncodingName) {
    if (newEncoding) {
      if (newEncoding->minBytesPerChar != encoding->minBytesPerChar) {
	eventPtr = encodingName;
	return XML_ERROR_INCORRECT_ENCODING;
      }
      encoding = newEncoding;
    }
    else if (encodingName) {
      enum XML_Error result;
      const XML_Char *s = poolStoreString(&tempPool,
					  encoding,
					  encodingName,
					  encodingName
					  + XmlNameLength(encoding, encodingName));
      if (!s)
	return XML_ERROR_NO_MEMORY;
      result = handleUnknownEncoding(parser, s);
      poolDiscard(&tempPool);
      if (result == XML_ERROR_UNKNOWN_ENCODING)
	eventPtr = encodingName;
      return result;
    }
  }
  return XML_ERROR_NONE;
}

static enum XML_Error
handleUnknownEncoding(XML_Parser parser, const XML_Char *encodingName)
{
  if (unknownEncodingHandler) {
    XML_Encoding info;
    int i;
    for (i = 0; i < 256; i++)
      info.map[i] = -1;
    info.convert = 0;
    info.data = 0;
    info.release = 0;
    if (unknownEncodingHandler(unknownEncodingHandlerData, encodingName, &info)) {
      ENCODING *enc;
      unknownEncodingMem = malloc(XmlSizeOfUnknownEncoding());
      if (!unknownEncodingMem) {
	if (info.release)
	  info.release(info.data);
	return XML_ERROR_NO_MEMORY;
      }
      enc = (ns
	     ? XmlInitUnknownEncodingNS
	     : XmlInitUnknownEncoding)(unknownEncodingMem,
				       info.map,
				       info.convert,
				       info.data);
      if (enc) {
	unknownEncodingData = info.data;
	unknownEncodingRelease = info.release;
	encoding = enc;
	return XML_ERROR_NONE;
      }
    }
    if (info.release)
      info.release(info.data);
  }
  return XML_ERROR_UNKNOWN_ENCODING;
}

static enum XML_Error
prologInitProcessor(XML_Parser parser,
		    const char *s,
		    const char *end,
		    const char **nextPtr)
{
  enum XML_Error result = initializeEncoding(parser);
  if (result != XML_ERROR_NONE)
    return result;
  processor = prologProcessor;
  return prologProcessor(parser, s, end, nextPtr);
}

static enum XML_Error
prologProcessor(XML_Parser parser,
		const char *s,
		const char *end,
		const char **nextPtr)
{
  for (;;) {
    const char *next;
    int tok = XmlPrologTok(encoding, s, end, &next);
    if (tok <= 0) {
      if (nextPtr != 0 && tok != XML_TOK_INVALID) {
	*nextPtr = s;
	return XML_ERROR_NONE;
      }
      switch (tok) {
      case XML_TOK_INVALID:
	eventPtr = next;
	return XML_ERROR_INVALID_TOKEN;
      case XML_TOK_NONE:
	return XML_ERROR_NO_ELEMENTS;
      case XML_TOK_PARTIAL:
	return XML_ERROR_UNCLOSED_TOKEN;
      case XML_TOK_PARTIAL_CHAR:
	return XML_ERROR_PARTIAL_CHAR;
      case XML_TOK_TRAILING_CR:
	eventPtr = s + encoding->minBytesPerChar;
	return XML_ERROR_NO_ELEMENTS;
      default:
	abort();
      }
    }
    switch (XmlTokenRole(&prologState, tok, s, next, encoding)) {
    case XML_ROLE_XML_DECL:
      {
	enum XML_Error result = processXmlDecl(parser, 0, s, next);
	if (result != XML_ERROR_NONE)
	  return result;
      }
      break;
    case XML_ROLE_DOCTYPE_SYSTEM_ID:
      if (!dtd.standalone
	  && notStandaloneHandler
	  && !notStandaloneHandler(handlerArg))
	return XML_ERROR_NOT_STANDALONE;
      hadExternalDoctype = 1;
      break;
    case XML_ROLE_DOCTYPE_PUBLIC_ID:
    case XML_ROLE_ENTITY_PUBLIC_ID:
      if (!XmlIsPublicId(encoding, s, next, &eventPtr))
	return XML_ERROR_SYNTAX;
      if (declEntity) {
	XML_Char *tem = poolStoreString(&dtd.pool,
	                                encoding,
					s + encoding->minBytesPerChar,
	  				next - encoding->minBytesPerChar);
	if (!tem)
	  return XML_ERROR_NO_MEMORY;
	normalizePublicId(tem);
	declEntity->publicId = tem;
	poolFinish(&dtd.pool);
      }
      break;
    case XML_ROLE_INSTANCE_START:
      processor = contentProcessor;
      if (hadExternalDoctype)
	dtd.complete = 0;
      return contentProcessor(parser, s, end, nextPtr);
    case XML_ROLE_ATTLIST_ELEMENT_NAME:
      {
	const XML_Char *name = poolStoreString(&dtd.pool, encoding, s, next);
	if (!name)
	  return XML_ERROR_NO_MEMORY;
	declElementType = (ELEMENT_TYPE *)lookup(&dtd.elementTypes, name, sizeof(ELEMENT_TYPE));
	if (!declElementType)
	  return XML_ERROR_NO_MEMORY;
	if (declElementType->name != name)
	  poolDiscard(&dtd.pool);
	else {
	  poolFinish(&dtd.pool);
	  if (!setElementTypePrefix(parser, declElementType))
            return XML_ERROR_NO_MEMORY;
	}
	break;
      }
    case XML_ROLE_ATTRIBUTE_NAME:
      declAttributeId = getAttributeId(parser, encoding, s, next);
      if (!declAttributeId)
	return XML_ERROR_NO_MEMORY;
      declAttributeIsCdata = 0;
      break;
    case XML_ROLE_ATTRIBUTE_TYPE_CDATA:
      declAttributeIsCdata = 1;
      break;
    case XML_ROLE_IMPLIED_ATTRIBUTE_VALUE:
    case XML_ROLE_REQUIRED_ATTRIBUTE_VALUE:
      if (dtd.complete
	  && !defineAttribute(declElementType, declAttributeId, declAttributeIsCdata, 0))
	return XML_ERROR_NO_MEMORY;
      break;
    case XML_ROLE_DEFAULT_ATTRIBUTE_VALUE:
    case XML_ROLE_FIXED_ATTRIBUTE_VALUE:
      {
	const XML_Char *attVal;
	enum XML_Error result
	  = storeAttributeValue(parser, encoding, declAttributeIsCdata,
				s + encoding->minBytesPerChar,
			        next - encoding->minBytesPerChar,
			        &dtd.pool);
	if (result)
	  return result;
	attVal = poolStart(&dtd.pool);
	poolFinish(&dtd.pool);
	if (dtd.complete
	    && !defineAttribute(declElementType, declAttributeId, declAttributeIsCdata, attVal))
	  return XML_ERROR_NO_MEMORY;
	break;
      }
    case XML_ROLE_ENTITY_VALUE:
      {
	enum XML_Error result = storeEntityValue(parser, s, next);
	if (result != XML_ERROR_NONE)
	  return result;
      }
      break;
    case XML_ROLE_ENTITY_SYSTEM_ID:
      if (declEntity) {
	declEntity->systemId = poolStoreString(&dtd.pool, encoding,
	                                       s + encoding->minBytesPerChar,
	  				       next - encoding->minBytesPerChar);
	if (!declEntity->systemId)
	  return XML_ERROR_NO_MEMORY;
	declEntity->base = dtd.base;
	poolFinish(&dtd.pool);
      }
      break;
    case XML_ROLE_ENTITY_NOTATION_NAME:
      if (declEntity) {
	declEntity->notation = poolStoreString(&dtd.pool, encoding, s, next);
	if (!declEntity->notation)
	  return XML_ERROR_NO_MEMORY;
	poolFinish(&dtd.pool);
	if (unparsedEntityDeclHandler) {
	  eventPtr = eventEndPtr = s;
	  unparsedEntityDeclHandler(handlerArg,
				    declEntity->name,
				    declEntity->base,
				    declEntity->systemId,
				    declEntity->publicId,
				    declEntity->notation);
	}

      }
      break;
    case XML_ROLE_GENERAL_ENTITY_NAME:
      {
	const XML_Char *name;
	if (XmlPredefinedEntityName(encoding, s, next)) {
	  declEntity = 0;
	  break;
	}
	name = poolStoreString(&dtd.pool, encoding, s, next);
	if (!name)
	  return XML_ERROR_NO_MEMORY;
	if (dtd.complete) {
	  declEntity = (ENTITY *)lookup(&dtd.generalEntities, name, sizeof(ENTITY));
	  if (!declEntity)
	    return XML_ERROR_NO_MEMORY;
	  if (declEntity->name != name) {
	    poolDiscard(&dtd.pool);
	    declEntity = 0;
	  }
	  else
	    poolFinish(&dtd.pool);
	}
	else {
	  poolDiscard(&dtd.pool);
	  declEntity = 0;
	}
      }
      break;
    case XML_ROLE_PARAM_ENTITY_NAME:
      declEntity = 0;
      break;
    case XML_ROLE_NOTATION_NAME:
      declNotationPublicId = 0;
      declNotationName = 0;
      if (notationDeclHandler) {
	declNotationName = poolStoreString(&tempPool, encoding, s, next);
	if (!declNotationName)
	  return XML_ERROR_NO_MEMORY;
	poolFinish(&tempPool);
      }
      break;
    case XML_ROLE_NOTATION_PUBLIC_ID:
      if (!XmlIsPublicId(encoding, s, next, &eventPtr))
	return XML_ERROR_SYNTAX;
      if (declNotationName) {
	XML_Char *tem = poolStoreString(&tempPool,
	                                encoding,
					s + encoding->minBytesPerChar,
	  				next - encoding->minBytesPerChar);
	if (!tem)
	  return XML_ERROR_NO_MEMORY;
	normalizePublicId(tem);
	declNotationPublicId = tem;
	poolFinish(&tempPool);
      }
      break;
    case XML_ROLE_NOTATION_SYSTEM_ID:
      if (declNotationName && notationDeclHandler) {
	const XML_Char *systemId
	  = poolStoreString(&tempPool, encoding,
			    s + encoding->minBytesPerChar,
	  		    next - encoding->minBytesPerChar);
	if (!systemId)
	  return XML_ERROR_NO_MEMORY;
	eventPtr = eventEndPtr = s;
	notationDeclHandler(handlerArg,
			    declNotationName,
			    dtd.base,
			    systemId,
			    declNotationPublicId);
      }
      poolClear(&tempPool);
      break;
    case XML_ROLE_NOTATION_NO_SYSTEM_ID:
      if (declNotationPublicId && notationDeclHandler) {
	eventPtr = eventEndPtr = s;
	notationDeclHandler(handlerArg,
			    declNotationName,
			    dtd.base,
			    0,
			    declNotationPublicId);
      }
      poolClear(&tempPool);
      break;
    case XML_ROLE_ERROR:
      eventPtr = s;
      switch (tok) {
      case XML_TOK_PARAM_ENTITY_REF:
	return XML_ERROR_PARAM_ENTITY_REF;
      case XML_TOK_XML_DECL:
	return XML_ERROR_MISPLACED_XML_PI;
      default:
	return XML_ERROR_SYNTAX;
      }
    case XML_ROLE_GROUP_OPEN:
      if (prologState.level >= groupSize) {
	if (groupSize)
	  groupConnector = realloc(groupConnector, groupSize *= 2);
	else
	  groupConnector = malloc(groupSize = 32);
	if (!groupConnector)
	  return XML_ERROR_NO_MEMORY;
      }
      groupConnector[prologState.level] = 0;
      break;
    case XML_ROLE_GROUP_SEQUENCE:
      if (groupConnector[prologState.level] == '|') {
	eventPtr = s;
	return XML_ERROR_SYNTAX;
      }
      groupConnector[prologState.level] = ',';
      break;
    case XML_ROLE_GROUP_CHOICE:
      if (groupConnector[prologState.level] == ',') {
	eventPtr = s;
	return XML_ERROR_SYNTAX;
      }
      groupConnector[prologState.level] = '|';
      break;
    case XML_ROLE_PARAM_ENTITY_REF:
      if (!dtd.standalone
	  && notStandaloneHandler
	  && !notStandaloneHandler(handlerArg))
	return XML_ERROR_NOT_STANDALONE;
      dtd.complete = 0;
      break;
    case XML_ROLE_NONE:
      switch (tok) {
      case XML_TOK_PI:
	eventPtr = s;
	eventEndPtr = next;
	if (!reportProcessingInstruction(parser, encoding, s, next))
	  return XML_ERROR_NO_MEMORY;
	break;
      case XML_TOK_COMMENT:
	eventPtr = s;
	eventEndPtr = next;
	if (!reportComment(parser, encoding, s, next))
	  return XML_ERROR_NO_MEMORY;
	break;
      }
      break;
    }
    if (defaultHandler) {
      switch (tok) {
      case XML_TOK_PI:
      case XML_TOK_COMMENT:
      case XML_TOK_BOM:
      case XML_TOK_XML_DECL:
	break;
      default:
	eventPtr = s;
	eventEndPtr = next;
	reportDefault(parser, encoding, s, next);
      }
    }
    s = next;
  }
  /* not reached */
}

static
enum XML_Error epilogProcessor(XML_Parser parser,
			       const char *s,
			       const char *end,
			       const char **nextPtr)
{
  processor = epilogProcessor;
  eventPtr = s;
  for (;;) {
    const char *next;
    int tok = XmlPrologTok(encoding, s, end, &next);
    eventEndPtr = next;
    switch (tok) {
    case XML_TOK_TRAILING_CR:
      if (defaultHandler) {
	eventEndPtr = end;
	reportDefault(parser, encoding, s, end);
      }
      /* fall through */
    case XML_TOK_NONE:
      if (nextPtr)
	*nextPtr = end;
      return XML_ERROR_NONE;
    case XML_TOK_PROLOG_S:
      if (defaultHandler)
	reportDefault(parser, encoding, s, next);
      break;
    case XML_TOK_PI:
      if (!reportProcessingInstruction(parser, encoding, s, next))
	return XML_ERROR_NO_MEMORY;
      break;
    case XML_TOK_COMMENT:
      if (!reportComment(parser, encoding, s, next))
	return XML_ERROR_NO_MEMORY;
      break;
    case XML_TOK_INVALID:
      eventPtr = next;
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_PARTIAL:
      if (nextPtr) {
	*nextPtr = s;
	return XML_ERROR_NONE;
      }
      return XML_ERROR_UNCLOSED_TOKEN;
    case XML_TOK_PARTIAL_CHAR:
      if (nextPtr) {
	*nextPtr = s;
	return XML_ERROR_NONE;
      }
      return XML_ERROR_PARTIAL_CHAR;
    default:
      return XML_ERROR_JUNK_AFTER_DOC_ELEMENT;
    }
    eventPtr = s = next;
  }
}

static
enum XML_Error errorProcessor(XML_Parser parser,
			      const char *s,
			      const char *end,
			      const char **nextPtr)
{
  return errorCode;
}

static enum XML_Error
storeAttributeValue(XML_Parser parser, const ENCODING *enc, int isCdata,
		    const char *ptr, const char *end,
		    STRING_POOL *pool)
{
  enum XML_Error result = appendAttributeValue(parser, enc, isCdata, ptr, end, pool);
  if (result)
    return result;
  if (!isCdata && poolLength(pool) && poolLastChar(pool) == 0x20)
    poolChop(pool);
  if (!poolAppendChar(pool, XML_T('\0')))
    return XML_ERROR_NO_MEMORY;
  return XML_ERROR_NONE;
}

static enum XML_Error
appendAttributeValue(XML_Parser parser, const ENCODING *enc, int isCdata,
		     const char *ptr, const char *end,
		     STRING_POOL *pool)
{
  const ENCODING *internalEnc = ns ? XmlGetInternalEncodingNS() : XmlGetInternalEncoding();
  for (;;) {
    const char *next;
    int tok = XmlAttributeValueTok(enc, ptr, end, &next);
    switch (tok) {
    case XML_TOK_NONE:
      return XML_ERROR_NONE;
    case XML_TOK_INVALID:
      if (enc == encoding)
	eventPtr = next;
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_PARTIAL:
      if (enc == encoding)
	eventPtr = ptr;
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_CHAR_REF:
      {
	XML_Char buf[XML_ENCODE_MAX];
	int i;
	int n = XmlCharRefNumber(enc, ptr);
	if (n < 0) {
	  if (enc == encoding)
	    eventPtr = ptr;
      	  return XML_ERROR_BAD_CHAR_REF;
	}
	if (!isCdata
	    && n == 0x20 /* space */
	    && (poolLength(pool) == 0 || poolLastChar(pool) == 0x20))
	  break;
	n = XmlEncode(n, (ICHAR *)buf);
	if (!n) {
	  if (enc == encoding)
	    eventPtr = ptr;
	  return XML_ERROR_BAD_CHAR_REF;
	}
	for (i = 0; i < n; i++) {
	  if (!poolAppendChar(pool, buf[i]))
	    return XML_ERROR_NO_MEMORY;
	}
      }
      break;
    case XML_TOK_DATA_CHARS:
      if (!poolAppend(pool, enc, ptr, next))
	return XML_ERROR_NO_MEMORY;
      break;
      break;
    case XML_TOK_TRAILING_CR:
      next = ptr + enc->minBytesPerChar;
      /* fall through */
    case XML_TOK_ATTRIBUTE_VALUE_S:
    case XML_TOK_DATA_NEWLINE:
      if (!isCdata && (poolLength(pool) == 0 || poolLastChar(pool) == 0x20))
	break;
      if (!poolAppendChar(pool, 0x20))
	return XML_ERROR_NO_MEMORY;
      break;
    case XML_TOK_ENTITY_REF:
      {
	const XML_Char *name;
	ENTITY *entity;
	XML_Char ch = XmlPredefinedEntityName(enc,
					      ptr + enc->minBytesPerChar,
					      next - enc->minBytesPerChar);
	if (ch) {
	  if (!poolAppendChar(pool, ch))
  	    return XML_ERROR_NO_MEMORY;
	  break;
	}
	name = poolStoreString(&temp2Pool, enc,
			       ptr + enc->minBytesPerChar,
			       next - enc->minBytesPerChar);
	if (!name)
	  return XML_ERROR_NO_MEMORY;
	entity = (ENTITY *)lookup(&dtd.generalEntities, name, 0);
	poolDiscard(&temp2Pool);
	if (!entity) {
	  if (dtd.complete) {
	    if (enc == encoding)
	      eventPtr = ptr;
	    return XML_ERROR_UNDEFINED_ENTITY;
	  }
	}
	else if (entity->open) {
	  if (enc == encoding)
	    eventPtr = ptr;
	  return XML_ERROR_RECURSIVE_ENTITY_REF;
	}
	else if (entity->notation) {
	  if (enc == encoding)
	    eventPtr = ptr;
	  return XML_ERROR_BINARY_ENTITY_REF;
	}
	else if (!entity->textPtr) {
	  if (enc == encoding)
	    eventPtr = ptr;
  	  return XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF;
	}
	else {
	  enum XML_Error result;
	  const XML_Char *textEnd = entity->textPtr + entity->textLen;
	  entity->open = 1;
	  result = appendAttributeValue(parser, internalEnc, isCdata, (char *)entity->textPtr, (char *)textEnd, pool);
	  entity->open = 0;
	  if (result)
	    return result;
	}
      }
      break;
    default:
      abort();
    }
    ptr = next;
  }
  /* not reached */
}

static
enum XML_Error storeEntityValue(XML_Parser parser,
				const char *entityTextPtr,
				const char *entityTextEnd)
{
  const ENCODING *internalEnc = ns ? XmlGetInternalEncodingNS() : XmlGetInternalEncoding();
  STRING_POOL *pool = &(dtd.pool);
  entityTextPtr += encoding->minBytesPerChar;
  entityTextEnd -= encoding->minBytesPerChar;
  for (;;) {
    const char *next;
    int tok = XmlEntityValueTok(encoding, entityTextPtr, entityTextEnd, &next);
    switch (tok) {
    case XML_TOK_PARAM_ENTITY_REF:
      eventPtr = entityTextPtr;
      return XML_ERROR_SYNTAX;
    case XML_TOK_NONE:
      if (declEntity) {
	declEntity->textPtr = pool->start;
	declEntity->textLen = pool->ptr - pool->start;
	poolFinish(pool);
      }
      else
	poolDiscard(pool);
      return XML_ERROR_NONE;
    case XML_TOK_ENTITY_REF:
    case XML_TOK_DATA_CHARS:
      if (!poolAppend(pool, encoding, entityTextPtr, next))
	return XML_ERROR_NO_MEMORY;
      break;
    case XML_TOK_TRAILING_CR:
      next = entityTextPtr + encoding->minBytesPerChar;
      /* fall through */
    case XML_TOK_DATA_NEWLINE:
      if (pool->end == pool->ptr && !poolGrow(pool))
	return XML_ERROR_NO_MEMORY;
      *(pool->ptr)++ = 0xA;
      break;
    case XML_TOK_CHAR_REF:
      {
	XML_Char buf[XML_ENCODE_MAX];
	int i;
	int n = XmlCharRefNumber(encoding, entityTextPtr);
	if (n < 0) {
	  eventPtr = entityTextPtr;
	  return XML_ERROR_BAD_CHAR_REF;
	}
	n = XmlEncode(n, (ICHAR *)buf);
	if (!n) {
	  eventPtr = entityTextPtr;
	  return XML_ERROR_BAD_CHAR_REF;
	}
	for (i = 0; i < n; i++) {
	  if (pool->end == pool->ptr && !poolGrow(pool))
	    return XML_ERROR_NO_MEMORY;
	  *(pool->ptr)++ = buf[i];
	}
      }
      break;
    case XML_TOK_PARTIAL:
      eventPtr = entityTextPtr;
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_INVALID:
      eventPtr = next;
      return XML_ERROR_INVALID_TOKEN;
    default:
      abort();
    }
    entityTextPtr = next;
  }
  /* not reached */
}

static void
normalizeLines(XML_Char *s)
{
  XML_Char *p;
  for (;; s++) {
    if (*s == XML_T('\0'))
      return;
    if (*s == 0xD)
      break;
  }
  p = s;
  do {
    if (*s == 0xD) {
      *p++ = 0xA;
      if (*++s == 0xA)
        s++;
    }
    else
      *p++ = *s++;
  } while (*s);
  *p = XML_T('\0');
}

static int
reportProcessingInstruction(XML_Parser parser, const ENCODING *enc, const char *start, const char *end)
{
  const XML_Char *target;
  XML_Char *data;
  const char *tem;
  if (!processingInstructionHandler) {
    if (defaultHandler)
      reportDefault(parser, enc, start, end);
    return 1;
  }
  start += enc->minBytesPerChar * 2;
  tem = start + XmlNameLength(enc, start);
  target = poolStoreString(&tempPool, enc, start, tem);
  if (!target)
    return 0;
  poolFinish(&tempPool);
  data = poolStoreString(&tempPool, enc,
			XmlSkipS(enc, tem),
			end - enc->minBytesPerChar*2);
  if (!data)
    return 0;
  normalizeLines(data);
  processingInstructionHandler(handlerArg, target, data);
  poolClear(&tempPool);
  return 1;
}

static int
reportComment(XML_Parser parser, const ENCODING *enc, const char *start, const char *end)
{
  XML_Char *data;
  if (!commentHandler) {
    if (defaultHandler)
      reportDefault(parser, enc, start, end);
    return 1;
  }
  data = poolStoreString(&tempPool,
                         enc,
                         start + enc->minBytesPerChar * 4, 
			 end - enc->minBytesPerChar * 3);
  if (!data)
    return 0;
  normalizeLines(data);
  commentHandler(handlerArg, data);
  poolClear(&tempPool);
  return 1;
}

static void
reportDefault(XML_Parser parser, const ENCODING *enc, const char *s, const char *end)
{
  if (MUST_CONVERT(enc, s)) {
    const char **eventPP;
    const char **eventEndPP;
    if (enc == encoding) {
      eventPP = &eventPtr;
      eventEndPP = &eventEndPtr;
    }
    else {
      eventPP = &(openInternalEntities->internalEventPtr);
      eventEndPP = &(openInternalEntities->internalEventEndPtr);
    }
    do {
      ICHAR *dataPtr = (ICHAR *)dataBuf;
      XmlConvert(enc, &s, end, &dataPtr, (ICHAR *)dataBufEnd);
      *eventEndPP = s;
      defaultHandler(handlerArg, dataBuf, dataPtr - (ICHAR *)dataBuf);
      *eventPP = s;
    } while (s != end);
  }
  else
    defaultHandler(handlerArg, (XML_Char *)s, (XML_Char *)end - (XML_Char *)s);
}


static int
defineAttribute(ELEMENT_TYPE *type, ATTRIBUTE_ID *attId, int isCdata, const XML_Char *value)
{
  DEFAULT_ATTRIBUTE *att;
  if (type->nDefaultAtts == type->allocDefaultAtts) {
    if (type->allocDefaultAtts == 0) {
      type->allocDefaultAtts = 8;
      type->defaultAtts = malloc(type->allocDefaultAtts*sizeof(DEFAULT_ATTRIBUTE));
    }
    else {
      type->allocDefaultAtts *= 2;
      type->defaultAtts = realloc(type->defaultAtts,
				  type->allocDefaultAtts*sizeof(DEFAULT_ATTRIBUTE));
    }
    if (!type->defaultAtts)
      return 0;
  }
  att = type->defaultAtts + type->nDefaultAtts;
  att->id = attId;
  att->value = value;
  att->isCdata = isCdata;
  if (!isCdata)
    attId->maybeTokenized = 1;
  type->nDefaultAtts += 1;
  return 1;
}

static int setElementTypePrefix(XML_Parser parser, ELEMENT_TYPE *elementType)
{
  const XML_Char *name;
  for (name = elementType->name; *name; name++) {
    if (*name == XML_T(':')) {
      PREFIX *prefix;
      const XML_Char *s;
      for (s = elementType->name; s != name; s++) {
	if (!poolAppendChar(&dtd.pool, *s))
	  return 0;
      }
      if (!poolAppendChar(&dtd.pool, XML_T('\0')))
	return 0;
      prefix = (PREFIX *)lookup(&dtd.prefixes, poolStart(&dtd.pool), sizeof(PREFIX));
      if (!prefix)
	return 0;
      if (prefix->name == poolStart(&dtd.pool))
	poolFinish(&dtd.pool);
      else
	poolDiscard(&dtd.pool);
      elementType->prefix = prefix;

    }
  }
  return 1;
}

static ATTRIBUTE_ID *
getAttributeId(XML_Parser parser, const ENCODING *enc, const char *start, const char *end)
{
  ATTRIBUTE_ID *id;
  const XML_Char *name;
  if (!poolAppendChar(&dtd.pool, XML_T('\0')))
    return 0;
  name = poolStoreString(&dtd.pool, enc, start, end);
  if (!name)
    return 0;
  ++name;
  id = (ATTRIBUTE_ID *)lookup(&dtd.attributeIds, name, sizeof(ATTRIBUTE_ID));
  if (!id)
    return 0;
  if (id->name != name)
    poolDiscard(&dtd.pool);
  else {
    poolFinish(&dtd.pool);
    if (!ns)
      ;
    else if (name[0] == 'x'
	&& name[1] == 'm'
	&& name[2] == 'l'
	&& name[3] == 'n'
	&& name[4] == 's'
	&& (name[5] == XML_T('\0') || name[5] == XML_T(':'))) {
      if (name[5] == '\0')
	id->prefix = &dtd.defaultPrefix;
      else
	id->prefix = (PREFIX *)lookup(&dtd.prefixes, name + 6, sizeof(PREFIX));
      id->xmlns = 1;
    }
    else {
      int i;
      for (i = 0; name[i]; i++) {
	if (name[i] == XML_T(':')) {
	  int j;
	  for (j = 0; j < i; j++) {
	    if (!poolAppendChar(&dtd.pool, name[j]))
	      return 0;
	  }
	  if (!poolAppendChar(&dtd.pool, XML_T('\0')))
	    return 0;
	  id->prefix = (PREFIX *)lookup(&dtd.prefixes, poolStart(&dtd.pool), sizeof(PREFIX));
	  if (id->prefix->name == poolStart(&dtd.pool))
	    poolFinish(&dtd.pool);
	  else
	    poolDiscard(&dtd.pool);
	  break;
	}
      }
    }
  }
  return id;
}

#define CONTEXT_SEP XML_T('\f')

static
const XML_Char *getContext(XML_Parser parser)
{
  HASH_TABLE_ITER iter;
  int needSep = 0;

  if (dtd.defaultPrefix.binding) {
    int i;
    int len;
    if (!poolAppendChar(&tempPool, XML_T('=')))
      return 0;
    len = dtd.defaultPrefix.binding->uriLen;
    if (namespaceSeparator != XML_T('\0'))
      len--;
    for (i = 0; i < len; i++)
      if (!poolAppendChar(&tempPool, dtd.defaultPrefix.binding->uri[i]))
  	return 0;
    needSep = 1;
  }

  hashTableIterInit(&iter, &(dtd.prefixes));
  for (;;) {
    int i;
    int len;
    const XML_Char *s;
    PREFIX *prefix = (PREFIX *)hashTableIterNext(&iter);
    if (!prefix)
      break;
    if (!prefix->binding)
      continue;
    if (needSep && !poolAppendChar(&tempPool, CONTEXT_SEP))
      return 0;
    for (s = prefix->name; *s; s++)
      if (!poolAppendChar(&tempPool, *s))
        return 0;
    if (!poolAppendChar(&tempPool, XML_T('=')))
      return 0;
    len = prefix->binding->uriLen;
    if (namespaceSeparator != XML_T('\0'))
      len--;
    for (i = 0; i < len; i++)
      if (!poolAppendChar(&tempPool, prefix->binding->uri[i]))
        return 0;
    needSep = 1;
  }


  hashTableIterInit(&iter, &(dtd.generalEntities));
  for (;;) {
    const XML_Char *s;
    ENTITY *e = (ENTITY *)hashTableIterNext(&iter);
    if (!e)
      break;
    if (!e->open)
      continue;
    if (needSep && !poolAppendChar(&tempPool, CONTEXT_SEP))
      return 0;
    for (s = e->name; *s; s++)
      if (!poolAppendChar(&tempPool, *s))
        return 0;
    needSep = 1;
  }

  if (!poolAppendChar(&tempPool, XML_T('\0')))
    return 0;
  return tempPool.start;
}

static
int setContext(XML_Parser parser, const XML_Char *context)
{
  const XML_Char *s = context;

  while (*context != XML_T('\0')) {
    if (*s == CONTEXT_SEP || *s == XML_T('\0')) {
      ENTITY *e;
      if (!poolAppendChar(&tempPool, XML_T('\0')))
	return 0;
      e = (ENTITY *)lookup(&dtd.generalEntities, poolStart(&tempPool), 0);
      if (e)
	e->open = 1;
      if (*s != XML_T('\0'))
	s++;
      context = s;
      poolDiscard(&tempPool);
    }
    else if (*s == '=') {
      PREFIX *prefix;
      if (poolLength(&tempPool) == 0)
	prefix = &dtd.defaultPrefix;
      else {
	if (!poolAppendChar(&tempPool, XML_T('\0')))
	  return 0;
	prefix = (PREFIX *)lookup(&dtd.prefixes, poolStart(&tempPool), sizeof(PREFIX));
	if (!prefix)
	  return 0;
        if (prefix->name == poolStart(&tempPool))
          poolFinish(&tempPool);
        else
	  poolDiscard(&tempPool);
      }
      for (context = s + 1; *context != CONTEXT_SEP && *context != XML_T('\0'); context++)
        if (!poolAppendChar(&tempPool, *context))
          return 0;
      if (!poolAppendChar(&tempPool, XML_T('\0')))
	return 0;
      if (!addBinding(parser, prefix, 0, poolStart(&tempPool), &inheritedBindings))
	return 0;
      poolDiscard(&tempPool);
      if (*context != XML_T('\0'))
	++context;
      s = context;
    }
    else {
      if (!poolAppendChar(&tempPool, *s))
	return 0;
      s++;
    }
  }
  return 1;
}


static
void normalizePublicId(XML_Char *publicId)
{
  XML_Char *p = publicId;
  XML_Char *s;
  for (s = publicId; *s; s++) {
    switch (*s) {
    case 0x20:
    case 0xD:
    case 0xA:
      if (p != publicId && p[-1] != 0x20)
	*p++ = 0x20;
      break;
    default:
      *p++ = *s;
    }
  }
  if (p != publicId && p[-1] == 0x20)
    --p;
  *p = XML_T('\0');
}

static int dtdInit(DTD *p)
{
  poolInit(&(p->pool));
  hashTableInit(&(p->generalEntities));
  hashTableInit(&(p->elementTypes));
  hashTableInit(&(p->attributeIds));
  hashTableInit(&(p->prefixes));
  p->complete = 1;
  p->standalone = 0;
  p->base = 0;
  p->defaultPrefix.name = 0;
  p->defaultPrefix.binding = 0;
  return 1;
}

static void dtdDestroy(DTD *p)
{
  HASH_TABLE_ITER iter;
  hashTableIterInit(&iter, &(p->elementTypes));
  for (;;) {
    ELEMENT_TYPE *e = (ELEMENT_TYPE *)hashTableIterNext(&iter);
    if (!e)
      break;
    if (e->allocDefaultAtts != 0)
      free(e->defaultAtts);
  }
  hashTableDestroy(&(p->generalEntities));
  hashTableDestroy(&(p->elementTypes));
  hashTableDestroy(&(p->attributeIds));
  hashTableDestroy(&(p->prefixes));
  poolDestroy(&(p->pool));
}

/* Do a deep copy of the DTD.  Return 0 for out of memory; non-zero otherwise.
The new DTD has already been initialized. */

static int dtdCopy(DTD *newDtd, const DTD *oldDtd)
{
  HASH_TABLE_ITER iter;

  if (oldDtd->base) {
    const XML_Char *tem = poolCopyString(&(newDtd->pool), oldDtd->base);
    if (!tem)
      return 0;
    newDtd->base = tem;
  }

  /* Copy the prefix table. */

  hashTableIterInit(&iter, &(oldDtd->prefixes));
  for (;;) {
    const XML_Char *name;
    const PREFIX *oldP = (PREFIX *)hashTableIterNext(&iter);
    if (!oldP)
      break;
    name = poolCopyString(&(newDtd->pool), oldP->name);
    if (!name)
      return 0;
    if (!lookup(&(newDtd->prefixes), name, sizeof(PREFIX)))
      return 0;
  }

  hashTableIterInit(&iter, &(oldDtd->attributeIds));

  /* Copy the attribute id table. */

  for (;;) {
    ATTRIBUTE_ID *newA;
    const XML_Char *name;
    const ATTRIBUTE_ID *oldA = (ATTRIBUTE_ID *)hashTableIterNext(&iter);

    if (!oldA)
      break;
    /* Remember to allocate the scratch byte before the name. */
    if (!poolAppendChar(&(newDtd->pool), XML_T('\0')))
      return 0;
    name = poolCopyString(&(newDtd->pool), oldA->name);
    if (!name)
      return 0;
    ++name;
    newA = (ATTRIBUTE_ID *)lookup(&(newDtd->attributeIds), name, sizeof(ATTRIBUTE_ID));
    if (!newA)
      return 0;
    newA->maybeTokenized = oldA->maybeTokenized;
    if (oldA->prefix) {
      newA->xmlns = oldA->xmlns;
      if (oldA->prefix == &oldDtd->defaultPrefix)
	newA->prefix = &newDtd->defaultPrefix;
      else
	newA->prefix = (PREFIX *)lookup(&(newDtd->prefixes), oldA->prefix->name, 0);
    }
  }

  /* Copy the element type table. */

  hashTableIterInit(&iter, &(oldDtd->elementTypes));

  for (;;) {
    int i;
    ELEMENT_TYPE *newE;
    const XML_Char *name;
    const ELEMENT_TYPE *oldE = (ELEMENT_TYPE *)hashTableIterNext(&iter);
    if (!oldE)
      break;
    name = poolCopyString(&(newDtd->pool), oldE->name);
    if (!name)
      return 0;
    newE = (ELEMENT_TYPE *)lookup(&(newDtd->elementTypes), name, sizeof(ELEMENT_TYPE));
    if (!newE)
      return 0;
    if (oldE->nDefaultAtts) {
      newE->defaultAtts = (DEFAULT_ATTRIBUTE *)malloc(oldE->nDefaultAtts * sizeof(DEFAULT_ATTRIBUTE));
      if (!newE->defaultAtts)
	return 0;
    }
    newE->allocDefaultAtts = newE->nDefaultAtts = oldE->nDefaultAtts;
    if (oldE->prefix)
      newE->prefix = (PREFIX *)lookup(&(newDtd->prefixes), oldE->prefix->name, 0);
    for (i = 0; i < newE->nDefaultAtts; i++) {
      newE->defaultAtts[i].id = (ATTRIBUTE_ID *)lookup(&(newDtd->attributeIds), oldE->defaultAtts[i].id->name, 0);
      newE->defaultAtts[i].isCdata = oldE->defaultAtts[i].isCdata;
      if (oldE->defaultAtts[i].value) {
	newE->defaultAtts[i].value = poolCopyString(&(newDtd->pool), oldE->defaultAtts[i].value);
	if (!newE->defaultAtts[i].value)
  	  return 0;
      }
      else
	newE->defaultAtts[i].value = 0;
    }
  }

  /* Copy the entity table. */

  hashTableIterInit(&iter, &(oldDtd->generalEntities));

  for (;;) {
    ENTITY *newE;
    const XML_Char *name;
    const ENTITY *oldE = (ENTITY *)hashTableIterNext(&iter);
    if (!oldE)
      break;
    name = poolCopyString(&(newDtd->pool), oldE->name);
    if (!name)
      return 0;
    newE = (ENTITY *)lookup(&(newDtd->generalEntities), name, sizeof(ENTITY));
    if (!newE)
      return 0;
    if (oldE->systemId) {
      const XML_Char *tem = poolCopyString(&(newDtd->pool), oldE->systemId);
      if (!tem)
	return 0;
      newE->systemId = tem;
      if (oldE->base) {
	if (oldE->base == oldDtd->base)
	  newE->base = newDtd->base;
	tem = poolCopyString(&(newDtd->pool), oldE->base);
	if (!tem)
	  return 0;
	newE->base = tem;
      }
    }
    else {
      const XML_Char *tem = poolCopyStringN(&(newDtd->pool), oldE->textPtr, oldE->textLen);
      if (!tem)
	return 0;
      newE->textPtr = tem;
      newE->textLen = oldE->textLen;
    }
    if (oldE->notation) {
      const XML_Char *tem = poolCopyString(&(newDtd->pool), oldE->notation);
      if (!tem)
	return 0;
      newE->notation = tem;
    }
  }

  newDtd->complete = oldDtd->complete;
  newDtd->standalone = oldDtd->standalone;
  return 1;
}

static
void poolInit(STRING_POOL *pool)
{
  pool->blocks = 0;
  pool->freeBlocks = 0;
  pool->start = 0;
  pool->ptr = 0;
  pool->end = 0;
}

static
void poolClear(STRING_POOL *pool)
{
  if (!pool->freeBlocks)
    pool->freeBlocks = pool->blocks;
  else {
    BLOCK *p = pool->blocks;
    while (p) {
      BLOCK *tem = p->next;
      p->next = pool->freeBlocks;
      pool->freeBlocks = p;
      p = tem;
    }
  }
  pool->blocks = 0;
  pool->start = 0;
  pool->ptr = 0;
  pool->end = 0;
}

static
void poolDestroy(STRING_POOL *pool)
{
  BLOCK *p = pool->blocks;
  while (p) {
    BLOCK *tem = p->next;
    free(p);
    p = tem;
  }
  pool->blocks = 0;
  p = pool->freeBlocks;
  while (p) {
    BLOCK *tem = p->next;
    free(p);
    p = tem;
  }
  pool->freeBlocks = 0;
  pool->ptr = 0;
  pool->start = 0;
  pool->end = 0;
}

static
XML_Char *poolAppend(STRING_POOL *pool, const ENCODING *enc,
		     const char *ptr, const char *end)
{
  if (!pool->ptr && !poolGrow(pool))
    return 0;
  for (;;) {
    XmlConvert(enc, &ptr, end, (ICHAR **)&(pool->ptr), (ICHAR *)pool->end);
    if (ptr == end)
      break;
    if (!poolGrow(pool))
      return 0;
  }
  return pool->start;
}

static const XML_Char *poolCopyString(STRING_POOL *pool, const XML_Char *s)
{
  do {
    if (!poolAppendChar(pool, *s))
      return 0;
  } while (*s++);
  s = pool->start;
  poolFinish(pool);
  return s;
}

static const XML_Char *poolCopyStringN(STRING_POOL *pool, const XML_Char *s, int n)
{
  if (!pool->ptr && !poolGrow(pool))
    return 0;
  for (; n > 0; --n, s++) {
    if (!poolAppendChar(pool, *s))
      return 0;

  }
  s = pool->start;
  poolFinish(pool);
  return s;
}

static
XML_Char *poolStoreString(STRING_POOL *pool, const ENCODING *enc,
			  const char *ptr, const char *end)
{
  if (!poolAppend(pool, enc, ptr, end))
    return 0;
  if (pool->ptr == pool->end && !poolGrow(pool))
    return 0;
  *(pool->ptr)++ = 0;
  return pool->start;
}

static
int poolGrow(STRING_POOL *pool)
{
  if (pool->freeBlocks) {
    if (pool->start == 0) {
      pool->blocks = pool->freeBlocks;
      pool->freeBlocks = pool->freeBlocks->next;
      pool->blocks->next = 0;
      pool->start = pool->blocks->s;
      pool->end = pool->start + pool->blocks->size;
      pool->ptr = pool->start;
      return 1;
    }
    if (pool->end - pool->start < pool->freeBlocks->size) {
      BLOCK *tem = pool->freeBlocks->next;
      pool->freeBlocks->next = pool->blocks;
      pool->blocks = pool->freeBlocks;
      pool->freeBlocks = tem;
      memcpy(pool->blocks->s, pool->start, (pool->end - pool->start) * sizeof(XML_Char));
      pool->ptr = pool->blocks->s + (pool->ptr - pool->start);
      pool->start = pool->blocks->s;
      pool->end = pool->start + pool->blocks->size;
      return 1;
    }
  }
  if (pool->blocks && pool->start == pool->blocks->s) {
    int blockSize = (pool->end - pool->start)*2;
    pool->blocks = realloc(pool->blocks, offsetof(BLOCK, s) + blockSize * sizeof(XML_Char));
    if (!pool->blocks)
      return 0;
    pool->blocks->size = blockSize;
    pool->ptr = pool->blocks->s + (pool->ptr - pool->start);
    pool->start = pool->blocks->s;
    pool->end = pool->start + blockSize;
  }
  else {
    BLOCK *tem;
    int blockSize = pool->end - pool->start;
    if (blockSize < INIT_BLOCK_SIZE)
      blockSize = INIT_BLOCK_SIZE;
    else
      blockSize *= 2;
    tem = malloc(offsetof(BLOCK, s) + blockSize * sizeof(XML_Char));
    if (!tem)
      return 0;
    tem->size = blockSize;
    tem->next = pool->blocks;
    pool->blocks = tem;
    memcpy(tem->s, pool->start, (pool->ptr - pool->start) * sizeof(XML_Char));
    pool->ptr = tem->s + (pool->ptr - pool->start);
    pool->start = tem->s;
    pool->end = tem->s + blockSize;
  }
  return 1;
}
