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

#ifndef XmlTok_INCLUDED
#define XmlTok_INCLUDED 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XMLTOKAPI
#define XMLTOKAPI /* as nothing */
#endif

/* The following token may be returned by XmlContentTok */
#define XML_TOK_TRAILING_RSQB -5 /* ] or ]] at the end of the scan; might be start of
                                    illegal ]]> sequence */
/* The following tokens may be returned by both XmlPrologTok and XmlContentTok */
#define XML_TOK_NONE -4    /* The string to be scanned is empty */
#define XML_TOK_TRAILING_CR -3 /* A CR at the end of the scan;
                                  might be part of CRLF sequence */ 
#define XML_TOK_PARTIAL_CHAR -2 /* only part of a multibyte sequence */
#define XML_TOK_PARTIAL -1 /* only part of a token */
#define XML_TOK_INVALID 0

/* The following tokens are returned by XmlContentTok; some are also
  returned by XmlAttributeValueTok, XmlEntityTok, XmlCdataSectionTok */

#define XML_TOK_START_TAG_WITH_ATTS 1
#define XML_TOK_START_TAG_NO_ATTS 2
#define XML_TOK_EMPTY_ELEMENT_WITH_ATTS 3 /* empty element tag <e/> */
#define XML_TOK_EMPTY_ELEMENT_NO_ATTS 4
#define XML_TOK_END_TAG 5
#define XML_TOK_DATA_CHARS 6
#define XML_TOK_DATA_NEWLINE 7
#define XML_TOK_CDATA_SECT_OPEN 8
#define XML_TOK_ENTITY_REF 9
#define XML_TOK_CHAR_REF 10     /* numeric character reference */

/* The following tokens may be returned by both XmlPrologTok and XmlContentTok */
#define XML_TOK_PI 11      /* processing instruction */
#define XML_TOK_XML_DECL 12 /* XML decl or text decl */
#define XML_TOK_COMMENT 13
#define XML_TOK_BOM 14     /* Byte order mark */

/* The following tokens are returned only by XmlPrologTok */
#define XML_TOK_PROLOG_S 15
#define XML_TOK_DECL_OPEN 16 /* <!foo */
#define XML_TOK_DECL_CLOSE 17 /* > */
#define XML_TOK_NAME 18
#define XML_TOK_NMTOKEN 19
#define XML_TOK_POUND_NAME 20 /* #name */
#define XML_TOK_OR 21 /* | */
#define XML_TOK_PERCENT 22
#define XML_TOK_OPEN_PAREN 23
#define XML_TOK_CLOSE_PAREN 24
#define XML_TOK_OPEN_BRACKET 25
#define XML_TOK_CLOSE_BRACKET 26
#define XML_TOK_LITERAL 27
#define XML_TOK_PARAM_ENTITY_REF 28
#define XML_TOK_INSTANCE_START 29

/* The following occur only in element type declarations */
#define XML_TOK_NAME_QUESTION 30 /* name? */
#define XML_TOK_NAME_ASTERISK 31 /* name* */
#define XML_TOK_NAME_PLUS 32 /* name+ */
#define XML_TOK_COND_SECT_OPEN 33 /* <![ */
#define XML_TOK_COND_SECT_CLOSE 34 /* ]]> */
#define XML_TOK_CLOSE_PAREN_QUESTION 35 /* )? */
#define XML_TOK_CLOSE_PAREN_ASTERISK 36 /* )* */
#define XML_TOK_CLOSE_PAREN_PLUS 37 /* )+ */
#define XML_TOK_COMMA 38

/* The following token is returned only by XmlAttributeValueTok */
#define XML_TOK_ATTRIBUTE_VALUE_S 39

/* The following token is returned only by XmlCdataSectionTok */
#define XML_TOK_CDATA_SECT_CLOSE 40

/* With namespace processing this is returned by XmlPrologTok
   for a name with a colon. */
#define XML_TOK_PREFIXED_NAME 41

#define XML_N_STATES 3
#define XML_PROLOG_STATE 0
#define XML_CONTENT_STATE 1
#define XML_CDATA_SECTION_STATE 2

#define XML_N_LITERAL_TYPES 2
#define XML_ATTRIBUTE_VALUE_LITERAL 0
#define XML_ENTITY_VALUE_LITERAL 1

/* The size of the buffer passed to XmlUtf8Encode must be at least this. */
#define XML_UTF8_ENCODE_MAX 4
/* The size of the buffer passed to XmlUtf16Encode must be at least this. */
#define XML_UTF16_ENCODE_MAX 2

typedef struct position {
  /* first line and first column are 0 not 1 */
  unsigned long lineNumber;
  unsigned long columnNumber;
} POSITION;

typedef struct {
  const char *name;
  const char *valuePtr;
  const char *valueEnd;
  char normalized;
} ATTRIBUTE;

struct encoding;
typedef struct encoding ENCODING;

struct encoding {
  int (*scanners[XML_N_STATES])(const ENCODING *,
			        const char *,
			        const char *,
			        const char **);
  int (*literalScanners[XML_N_LITERAL_TYPES])(const ENCODING *,
					      const char *,
					      const char *,
					      const char **);
  int (*sameName)(const ENCODING *,
	          const char *, const char *);
  int (*nameMatchesAscii)(const ENCODING *,
			  const char *, const char *);
  int (*nameLength)(const ENCODING *, const char *);
  const char *(*skipS)(const ENCODING *, const char *);
  int (*getAtts)(const ENCODING *enc, const char *ptr,
	         int attsMax, ATTRIBUTE *atts);
  int (*charRefNumber)(const ENCODING *enc, const char *ptr);
  int (*predefinedEntityName)(const ENCODING *, const char *, const char *);
  void (*updatePosition)(const ENCODING *,
			 const char *ptr,
			 const char *end,
			 POSITION *);
  int (*isPublicId)(const ENCODING *enc, const char *ptr, const char *end,
		    const char **badPtr);
  void (*utf8Convert)(const ENCODING *enc,
		      const char **fromP,
		      const char *fromLim,
		      char **toP,
		      const char *toLim);
  void (*utf16Convert)(const ENCODING *enc,
		       const char **fromP,
		       const char *fromLim,
		       unsigned short **toP,
		       const unsigned short *toLim);
  int minBytesPerChar;
  char isUtf8;
  char isUtf16;
};

/*
Scan the string starting at ptr until the end of the next complete token,
but do not scan past eptr.  Return an integer giving the type of token.

Return XML_TOK_NONE when ptr == eptr; nextTokPtr will not be set.

Return XML_TOK_PARTIAL when the string does not contain a complete token;
nextTokPtr will not be set.

Return XML_TOK_INVALID when the string does not start a valid token; nextTokPtr
will be set to point to the character which made the token invalid.

Otherwise the string starts with a valid token; nextTokPtr will be set to point
to the character following the end of that token.

Each data character counts as a single token, but adjacent data characters
may be returned together.  Similarly for characters in the prolog outside
literals, comments and processing instructions.
*/


#define XmlTok(enc, state, ptr, end, nextTokPtr) \
  (((enc)->scanners[state])(enc, ptr, end, nextTokPtr))

#define XmlPrologTok(enc, ptr, end, nextTokPtr) \
   XmlTok(enc, XML_PROLOG_STATE, ptr, end, nextTokPtr)

#define XmlContentTok(enc, ptr, end, nextTokPtr) \
   XmlTok(enc, XML_CONTENT_STATE, ptr, end, nextTokPtr)

#define XmlCdataSectionTok(enc, ptr, end, nextTokPtr) \
   XmlTok(enc, XML_CDATA_SECTION_STATE, ptr, end, nextTokPtr)

/* This is used for performing a 2nd-level tokenization on
the content of a literal that has already been returned by XmlTok. */ 

#define XmlLiteralTok(enc, literalType, ptr, end, nextTokPtr) \
  (((enc)->literalScanners[literalType])(enc, ptr, end, nextTokPtr))

#define XmlAttributeValueTok(enc, ptr, end, nextTokPtr) \
   XmlLiteralTok(enc, XML_ATTRIBUTE_VALUE_LITERAL, ptr, end, nextTokPtr)

#define XmlEntityValueTok(enc, ptr, end, nextTokPtr) \
   XmlLiteralTok(enc, XML_ENTITY_VALUE_LITERAL, ptr, end, nextTokPtr)

#define XmlSameName(enc, ptr1, ptr2) (((enc)->sameName)(enc, ptr1, ptr2))

#define XmlNameMatchesAscii(enc, ptr1, ptr2) \
  (((enc)->nameMatchesAscii)(enc, ptr1, ptr2))

#define XmlNameLength(enc, ptr) \
  (((enc)->nameLength)(enc, ptr))

#define XmlSkipS(enc, ptr) \
  (((enc)->skipS)(enc, ptr))

#define XmlGetAttributes(enc, ptr, attsMax, atts) \
  (((enc)->getAtts)(enc, ptr, attsMax, atts))

#define XmlCharRefNumber(enc, ptr) \
  (((enc)->charRefNumber)(enc, ptr))

#define XmlPredefinedEntityName(enc, ptr, end) \
  (((enc)->predefinedEntityName)(enc, ptr, end))

#define XmlUpdatePosition(enc, ptr, end, pos) \
  (((enc)->updatePosition)(enc, ptr, end, pos))

#define XmlIsPublicId(enc, ptr, end, badPtr) \
  (((enc)->isPublicId)(enc, ptr, end, badPtr))

#define XmlUtf8Convert(enc, fromP, fromLim, toP, toLim) \
  (((enc)->utf8Convert)(enc, fromP, fromLim, toP, toLim))

#define XmlUtf16Convert(enc, fromP, fromLim, toP, toLim) \
  (((enc)->utf16Convert)(enc, fromP, fromLim, toP, toLim))

typedef struct {
  ENCODING initEnc;
  const ENCODING **encPtr;
} INIT_ENCODING;

int XMLTOKAPI XmlParseXmlDecl(int isGeneralTextEntity,
			      const ENCODING *enc,
			      const char *ptr,
	  		      const char *end,
			      const char **badPtr,
			      const char **versionPtr,
			      const char **encodingNamePtr,
			      const ENCODING **namedEncodingPtr,
			      int *standalonePtr);

int XMLTOKAPI XmlInitEncoding(INIT_ENCODING *, const ENCODING **, const char *name);
const ENCODING XMLTOKAPI *XmlGetUtf8InternalEncoding();
const ENCODING XMLTOKAPI *XmlGetUtf16InternalEncoding();
int XMLTOKAPI XmlUtf8Encode(int charNumber, char *buf);
int XMLTOKAPI XmlUtf16Encode(int charNumber, unsigned short *buf);

int XMLTOKAPI XmlSizeOfUnknownEncoding();
ENCODING XMLTOKAPI *
XmlInitUnknownEncoding(void *mem,
		       int *table,
		       int (*conv)(void *userData, const char *p),
		       void *userData);

int XMLTOKAPI XmlParseXmlDeclNS(int isGeneralTextEntity,
			        const ENCODING *enc,
			        const char *ptr,
	  		        const char *end,
			        const char **badPtr,
			        const char **versionPtr,
			        const char **encodingNamePtr,
			        const ENCODING **namedEncodingPtr,
			        int *standalonePtr);
int XMLTOKAPI XmlInitEncodingNS(INIT_ENCODING *, const ENCODING **, const char *name);
const ENCODING XMLTOKAPI *XmlGetUtf8InternalEncodingNS();
const ENCODING XMLTOKAPI *XmlGetUtf16InternalEncodingNS();
ENCODING XMLTOKAPI *
XmlInitUnknownEncodingNS(void *mem,
		         int *table,
		         int (*conv)(void *userData, const char *p),
		         void *userData);
#ifdef __cplusplus
}
#endif

#endif /* not XmlTok_INCLUDED */
