/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */
#define IS_EXT_MODULE
#if COMPILE_DL
# if PHP_31
#  include "../phpdl.h"
# else
#  include "dl/phpdl.h"
# endif
#endif
#include "php.h"
#include "php3_xml.h"
#include "zend_variables.h"

#if defined(THREAD_SAFE) && !PHP_31
#undef THREAD_SAFE
#endif

#if HAVE_LIBEXPAT

# if !(WIN32|WINNT)
#  include "build-defs.h"
# endif
# include "snprintf.h"
# include "ext/standard/head.h"
# include "php3_string.h"

/* Short-term TODO list:
 * - Implement XML_ExternalEntityParserCreate()
 */

/* Long-term TODO list:
 * - Fix the expat library so you can install your own memory manager
 *   functions
 */

/* Known bugs:
 * - Weird things happen with <![CDATA[]]> sections.
 */

/* {{{ thread safety stuff */

# ifdef THREAD_SAFE
#  define XML_GLOBAL(a) xml_globals->a
#  define XML_TLS_VARS xml_global_struct *xml_globals = TlsGetValue(XMLTls); 

void *xml_mutex;
DWORD XMLTls;
static int numthreads=0;

typedef struct xml_global_struct {
	xml_module php3_xml_module;
} xml_global_struct;

# else /* !defined(THREAD_SAFE) */
#  define XML_GLOBAL(a) a
#  define XML_TLS_VARS

xml_module php3_xml_module;

# endif /* defined(THREAD_SAFE) */

/* }}} */
/* {{{ dynamically loadable module stuff */

# if COMPILE_DL
DLEXPORT php3_module_entry *get_module() { return &xml_module_entry; };
# endif /* COMPILE_DL */

/* }}} */
/* {{{ function prototypes */

int php3_minit_xml(INIT_FUNC_ARGS);
int php3_rinit_xml(INIT_FUNC_ARGS);
int php3_mshutdown_xml(SHUTDOWN_FUNC_ARGS);
int php3_rshutdown_xml(SHUTDOWN_FUNC_ARGS);
void php3_info_xml(void);

static xml_parser *xml_get_parser(int, const char *, HashTable *);
static void xml_destroy_parser(xml_parser *);
static void xml_set_handler(char **, pval *);
inline static unsigned short xml_encode_iso_8859_1(unsigned char);
inline static char xml_decode_iso_8859_1(unsigned short);
inline static unsigned short xml_encode_us_ascii(char);
inline static char xml_decode_us_ascii(unsigned short);
static XML_Char *xml_utf8_encode(const char *, int, int *, const XML_Char *);
static char *xml_utf8_decode(const XML_Char *, int, int *, const XML_Char *);
static pval *xml_call_handler(xml_parser *, char *, int, pval **);
static pval *php3i_xmlcharpval(const XML_Char *, int, const XML_Char *);
static int php3i_xmlcharlen(const XML_Char *);

void php3i_xml_startElementHandler(void *, const char *, const char **);
void php3i_xml_endElementHandler(void *, const char *);
void php3i_xml_characterDataHandler(void *, const XML_Char *, int);
void php3i_xml_processingInstructionHandler(void *, const XML_Char *, const XML_Char *);
void php3i_xml_defaultHandler(void *, const XML_Char *, int);
void php3i_xml_unparsedEntityDeclHandler(void *, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *);
void php3i_xml_notationDeclHandler(void *, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *);
int  php3i_xml_externalEntityRefHandler(XML_Parser, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *);

/* }}} */
/* {{{ extension definition structures */

function_entry xml_functions[] = {
    PHP_FE(xml_parser_create, NULL)
    PHP_FE(xml_set_element_handler, NULL)
	PHP_FE(xml_set_character_data_handler, NULL)
	PHP_FE(xml_set_processing_instruction_handler, NULL)
	PHP_FE(xml_set_default_handler, NULL)
	PHP_FE(xml_set_unparsed_entity_decl_handler, NULL)
	PHP_FE(xml_set_notation_decl_handler, NULL)
	PHP_FE(xml_set_external_entity_ref_handler, NULL)
    PHP_FE(xml_parse, NULL)
    PHP_FE(xml_get_error_code, NULL)
    PHP_FE(xml_error_string, NULL)
    PHP_FE(xml_get_current_line_number, NULL)
    PHP_FE(xml_get_current_column_number, NULL)
    PHP_FE(xml_get_current_byte_index, NULL)
    PHP_FE(xml_parser_free, NULL)
	PHP_FE(xml_parser_set_option, NULL)
	PHP_FE(xml_parser_get_option, NULL)
	PHP_FE(utf8_encode, NULL)
	PHP_FE(utf8_decode, NULL)
    {NULL, NULL, NULL}
};

php3_module_entry xml_module_entry = {
    "XML",                /* extension name */
    xml_functions,        /* extension function list */
    php3_minit_xml,       /* extension-wide startup function */
    php3_mshutdown_xml,   /* extension-wide shutdown function */
    php3_rinit_xml,       /* per-request startup function */
    php3_rshutdown_xml,   /* per-request shutdown function */
    php3_info_xml,        /* information function */
    STANDARD_MODULE_PROPERTIES
};

/* All the encoding functions are set to NULL right now, since all
 * the encoding is currently done internally by expat/xmltok.
 */
xml_encoding xml_encodings[] = {
	{ "ISO-8859-1", xml_decode_iso_8859_1, xml_encode_iso_8859_1 },
	{ "US-ASCII",   xml_decode_us_ascii,   xml_encode_us_ascii   },
	{ "UTF-8",      NULL,                  NULL                  },
    { NULL,         NULL,                  NULL                  }
};

/* }}} */
/* {{{ startup, shutdown and info functions */

    /* {{{ php3_minit_xml */

int php3_minit_xml(INIT_FUNC_ARGS)
{
#if defined(THREAD_SAFE)
	xml_global_struct *xml_globals;
	PHP3_MUTEX_ALLOC(xml_mutex);
	PHP3_MUTEX_LOCK(xml_mutex);
	numthreads++;
	if (numthreads==1){
		if (!PHP3_TLS_PROC_STARTUP(XMLTls)){
			PHP3_MUTEX_UNLOCK(xml_mutex);
			PHP3_MUTEX_FREE(xml_mutex);
			return FAILURE;
		}
	}
	PHP3_MUTEX_UNLOCK(xml_mutex);
	if(!PHP3_TLS_THREAD_INIT(XMLTls,xml_globals,xml_global_struct)){
		PHP3_MUTEX_FREE(xml_mutex);
		return FAILURE;
	}
#endif

	XML_GLOBAL(php3_xml_module).le_xml_parser =
		register_list_destructors(xml_destroy_parser, NULL);

	XML_GLOBAL(php3_xml_module).default_encoding = "ISO-8859-1";

	REGISTER_LONG_CONSTANT("XML_ERROR_NONE", XML_ERROR_NONE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_NO_MEMORY", XML_ERROR_NO_MEMORY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_SYNTAX", XML_ERROR_SYNTAX, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_NO_ELEMENTS", XML_ERROR_NO_ELEMENTS, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_INVALID_TOKEN", XML_ERROR_INVALID_TOKEN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNCLOSED_TOKEN", XML_ERROR_UNCLOSED_TOKEN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_PARTIAL_CHAR", XML_ERROR_PARTIAL_CHAR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_TAG_MISMATCH", XML_ERROR_TAG_MISMATCH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_DUPLICATE_ATTRIBUTE", XML_ERROR_DUPLICATE_ATTRIBUTE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_JUNK_AFTER_DOC_ELEMENT", XML_ERROR_JUNK_AFTER_DOC_ELEMENT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_PARAM_ENTITY_REF", XML_ERROR_PARAM_ENTITY_REF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNDEFINED_ENTITY", XML_ERROR_UNDEFINED_ENTITY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_RECURSIVE_ENTITY_REF", XML_ERROR_RECURSIVE_ENTITY_REF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_ASYNC_ENTITY", XML_ERROR_ASYNC_ENTITY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_BAD_CHAR_REF", XML_ERROR_BAD_CHAR_REF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_BINARY_ENTITY_REF", XML_ERROR_BINARY_ENTITY_REF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF", XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_MISPLACED_XML_PI", XML_ERROR_MISPLACED_XML_PI, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNKNOWN_ENCODING", XML_ERROR_UNKNOWN_ENCODING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_INCORRECT_ENCODING", XML_ERROR_INCORRECT_ENCODING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNCLOSED_CDATA_SECTION", XML_ERROR_UNCLOSED_CDATA_SECTION, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_EXTERNAL_ENTITY_HANDLING", XML_ERROR_EXTERNAL_ENTITY_HANDLING, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("XML_OPTION_CASE_FOLDING", PHP3_XML_OPTION_CASE_FOLDING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_OPTION_TARGET_ENCODING", PHP3_XML_OPTION_TARGET_ENCODING, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
}

/* }}} */
    /* {{{ php3_rinit_xml */

int php3_rinit_xml(INIT_FUNC_ARGS)
{
    return SUCCESS;
}

/* }}} */
    /* {{{ php3_mshutdown_xml() */

int php3_mshutdown_xml(SHUTDOWN_FUNC_ARGS)
{
	XML_TLS_VARS;
#ifdef THREAD_SAFE
	PHP3_TLS_THREAD_FREE(xml_globals);
	PHP3_MUTEX_LOCK(xml_mutex);
	numthreads--;
	if (numthreads<1) {
		PHP3_TLS_PROC_SHUTDOWN(XMLTls);
		PHP3_MUTEX_UNLOCK(xml_mutex);
		PHP3_MUTEX_FREE(xml_mutex);
		return SUCCESS;
	}
	PHP3_MUTEX_UNLOCK(xml_mutex);
#endif
	return SUCCESS;
}

/* }}} */
    /* {{{ php3_rshutdown_xml() */

int php3_rshutdown_xml(SHUTDOWN_FUNC_ARGS)
{
    return SUCCESS;
}

/* }}} */
    /* {{{ php3_info_xml() */

void php3_info_xml()
{
		PUTS("XML support active");
}

/* }}} */

/* }}} */
/* {{{ extension-internal functions */

    /* {{{ xml_get_parser() */

static xml_parser *
xml_get_parser(int parser_ind, const char *func, HashTable *list)
{
	int type;
	xml_parser *parser;
	XML_TLS_VARS;

	parser = (xml_parser *)php3_list_find(parser_ind, &type);
	if (!parser || type != XML_GLOBAL(php3_xml_module).le_xml_parser) {
		php3_error(E_WARNING, "%s: invalid XML parser %d", func, parser_ind);
		return (xml_parser *)NULL;
	}
	return parser;
}

/* }}} */
    /* {{{ xml_destroy_parser() */

static void
xml_destroy_parser(xml_parser *parser)
{
	XML_TLS_VARS;
  
	if (parser->parser) {
		XML_ParserFree(parser->parser);
	}
	if (parser->startElementHandler) {
		efree(parser->startElementHandler);
	}
	if (parser->endElementHandler) {
		efree(parser->endElementHandler);
	}
	if (parser->characterDataHandler) {
		efree(parser->characterDataHandler);
	}
	if (parser->processingInstructionHandler) {
		efree(parser->processingInstructionHandler);
	}
	if (parser->defaultHandler) {
		efree(parser->defaultHandler);
	}
	if (parser->unparsedEntityDeclHandler) {
		efree(parser->unparsedEntityDeclHandler);
	}
	if (parser->notationDeclHandler) {
		efree(parser->notationDeclHandler);
	}
	if (parser->externalEntityRefHandler) {
		efree(parser->externalEntityRefHandler);
	}
	if (parser->unknownEncodingHandler) {
		efree(parser->unknownEncodingHandler);
	}
	if (parser->baseURI) {
		efree(parser->baseURI);
	}

	efree(parser);
}

/* }}} */
    /* {{{ xml_set_handler() */

static void
xml_set_handler(char **nameBufp, pval *data)
{
	if (data->value.str.len > 0) {
		if (*nameBufp != NULL) {
			efree(*nameBufp);
		}
		*nameBufp = php3i_pval_strdup(data);
	} else {
		if (*nameBufp != NULL) {
			efree(*nameBufp);
		}
		*nameBufp = NULL;
	}
}

/* }}} */
    /* {{{ xml_call_handler() */

static pval *
xml_call_handler(xml_parser *parser, char *funcName, int argc, pval **argv)
{
	if (parser && funcName) {
		pval *retval, *func;
		int i;
		HashTable *function_table;

		func = php3i_string_pval(funcName);
		retval = emalloc(sizeof(pval));
		/* We cannot call internal variables from a function module as
		   it breaks any chance of compiling it as a module on windows.
		   Instead, we create a callback function. */
		function_table=php3i_get_function_table();
		if (call_user_function(function_table, NULL, func, retval, argc, argv) == FAILURE) {
			php3tls_pval_destructor(retval);
			efree(retval);
			return NULL;
		}
		php3tls_pval_destructor(func);
		efree(func);
		for (i = 0; i < argc; i++) {
			php3tls_pval_destructor(argv[i]);
			efree(argv[i]);
		}
		return retval;
	}
	return NULL;
}

/* }}} */
    /* {{{ xml_encode_iso_8859_1() */

inline static unsigned short
xml_encode_iso_8859_1(unsigned char c)
{
	php3_printf("c=%d ", c);
	return (unsigned short)c;
}

/* }}} */
    /* {{{ xml_decode_iso_8859_1() */

inline static char
xml_decode_iso_8859_1(unsigned short c)
{
	return (char)(c > 0xff ? '?' : c);
}

/* }}} */
    /* {{{ xml_encode_us_ascii() */

inline static unsigned short
xml_encode_us_ascii(char c)
{
	return (unsigned short)c;
}

/* }}} */
    /* {{{ xml_decode_us_ascii() */

inline static char
xml_decode_us_ascii(unsigned short c)
{
	return (char)(c > 0x7f ? '?' : c);
}

/* }}} */
    /* {{{ xml_get_encoding() */

static xml_encoding *
xml_get_encoding(const XML_Char *name)
{
	xml_encoding *enc = &xml_encodings[0];

	while (enc && enc->name) {
		if (strcasecmp(name, enc->name) == 0) {
			return enc;
		}
		enc++;
	}
	return NULL;
}

/* }}} */
    /* {{{ xml_utf8_encode */
static XML_Char *
xml_utf8_encode(const char *s, int len, int *newlen, const XML_Char *encoding)
{
	int pos = len;
	char *newbuf;
	unsigned short c;
	unsigned short (*encoder)(char) = NULL;
	xml_encoding *enc = xml_get_encoding(encoding);

	*newlen = 0;
	if (enc) {
		encoder = enc->encoding_function;
	} else {
		/* If the target encoding was unknown, fail */
		return NULL;
	}
	if (encoder == NULL) {
		/* If no encoder function was specified, return the data as-is.
		 */
		newbuf = emalloc(len);
		memcpy(newbuf, s, len);
		*newlen = len;
		return newbuf;
	}
	/* This is the theoretical max (will never get beyond len * 2 as long
	 * as we are converting from single-byte characters, though) */
	newbuf = emalloc(len * 4);
	while (pos > 0) {
		c = encoder ? encoder((unsigned char)(*s)) : (unsigned short)(*s);
		if (c < 0x80) {
			newbuf[(*newlen)++] = c;
		} else if (c < 0x800) {
			newbuf[(*newlen)++] = (0xc0 | (c >> 6));
			newbuf[(*newlen)++] = (0x80 | (c & 0x3f));
		} else if (c < 0x10000) {
			newbuf[(*newlen)++] = (0xe0 | (c >> 12));
			newbuf[(*newlen)++] = (0xc0 | ((c >> 6) & 0x3f));
			newbuf[(*newlen)++] = (0x80 | (c & 0x3f));
		} else if (c < 0x200000) {
			newbuf[(*newlen)++] = (0xf0 | (c >> 18));
			newbuf[(*newlen)++] = (0xe0 | ((c >> 12) & 0x3f));
			newbuf[(*newlen)++] = (0xc0 | ((c >> 6) & 0x3f));
			newbuf[(*newlen)++] = (0x80 | (c & 0x3f));
		}
		pos--;
		s++;
    }
	if (*newlen < len * 4) {
		newbuf = erealloc(newbuf, *newlen);
	}
	return newbuf;
}
/* }}} */
    /* {{{ xml_utf8_decode */
static char *
xml_utf8_decode(const XML_Char *s, int len, int *newlen, const XML_Char *encoding)
{
	int pos = len;
	char *newbuf = emalloc(len);
	unsigned short c;
	char (*decoder)(unsigned short) = NULL;
	xml_encoding *enc = xml_get_encoding(encoding);

	*newlen = 0;
	if (enc) {
		decoder = enc->decoding_function;
	}
	if (decoder == NULL) {
		/* If the target encoding was unknown, or no decoder function
		 * was specified, return the UTF-8-encoded data as-is.
		 */
		memcpy(newbuf, s, len);
		*newlen = len;
		return newbuf;
	}
	while (pos > 0) {
		c = (unsigned char)(*s);
		if (c >= 0xf0) { /* four bytes encoded, 21 bits */
			c = ((s[0]&7)<<18) | ((s[1]&63)<<12) | ((s[2]&63)<<6) | (s[3]&63);
			s += 4;
			pos -= 4;
		} else if (c >= 0xe0) { /* three bytes encoded, 16 bits */
			c = ((s[0]&63)<<12) | ((s[1]&63)<<6) | (s[2]&63);
			s += 3;
			pos -= 3;
		} else if (c >= 0xc0) { /* two bytes encoded, 11 bits */
			c = ((s[0]&63)<<6) | (s[1]&63);
			s += 2;
			pos -= 2;
		} else {
			s++;
			pos--;
		}
		newbuf[*newlen] = decoder ? decoder(c) : c;
		++*newlen;
    }
	if (*newlen < len) {
		newbuf = erealloc(newbuf, *newlen);
	}
	return newbuf;
}
/* }}} */
    /* {{{ php3i_xmlcharpval() */

static pval *php3i_xmlcharpval(const XML_Char *s, int len, const XML_Char *encoding)
{
	pval *ret = emalloc(sizeof(pval));

	if (s == NULL) {
		var_reset(ret);
		return ret;
	}
	if (len == 0) {
		len = php3i_xmlcharlen(s);
	}
	ret->type = IS_STRING;
	ret->value.str.val = xml_utf8_decode(s, len, &ret->value.str.len, encoding);
	return ret;
}

/* }}} */
    /* {{{ php3i_xmlcharlen() */

static int php3i_xmlcharlen(const XML_Char *s)
{
	int len = 0;

	while (*s) {
		len++;
		s++;
	}
	return len;
}

/* }}} */
    /* {{{ php3i_xml_startElementHandler() */

void php3i_xml_startElementHandler(void *userData, const char *name,
								   const char **attributes)
{
	xml_parser *parser = (xml_parser *)userData;
	XML_TLS_VARS;

	if (parser && parser->startElementHandler) {
		pval *retval, *args[3];

		if (parser->case_folding) {
			name = _php3_strtoupper(estrdup(name));
		}
		args[0] = php3i_long_pval(parser->index);
		args[1] = php3i_string_pval(name);
		args[2] = emalloc(sizeof(pval));
		array_init(args[2]);
		while (attributes && *attributes) {
			char *key = (char *)attributes[0];
			char *value = (char *)attributes[1];
			if (parser->case_folding) {
				key = _php3_strtoupper(estrdup(key));
			}
			add_assoc_string(args[2], key, value, 1);
			if (parser->case_folding) {
				efree(key);
			}
			attributes += 2;
		}
		if ((retval = xml_call_handler(parser, parser->startElementHandler, 3, args))) {
			php3tls_pval_destructor(retval);
			efree(retval);
		}
		if (parser->case_folding) {
			efree((char *)name);
		}
	}
}

/* }}} */
    /* {{{ php3i_xml_endElementHandler() */

void php3i_xml_endElementHandler(void *userData, const char *name)
{
	xml_parser *parser = (xml_parser *)userData;
	XML_TLS_VARS;

	if (parser && parser->endElementHandler) {
		pval *retval, *args[2];

		if (parser->case_folding) {
			name = _php3_strtoupper(estrdup(name));
		}
		args[0] = php3i_long_pval(parser->index);
		args[1] = php3i_string_pval(name);
		if ((retval = xml_call_handler(parser, parser->endElementHandler, 2, args))) {
			php3tls_pval_destructor(retval);
			efree(retval);
		}
		if (parser->case_folding) {
			efree((char *)name);
		}
	}
}

/* }}} */
    /* {{{ php3i_xml_characterDataHandler() */

void php3i_xml_characterDataHandler(void *userData, const XML_Char *s, int len)
{
	xml_parser *parser = (xml_parser *)userData;
	XML_TLS_VARS;

	if (parser && parser->characterDataHandler) {
		pval *retval, *args[2];

		args[0] = php3i_long_pval(parser->index);
		args[1] = php3i_xmlcharpval(s, len, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->characterDataHandler, 2, args))) {
			php3tls_pval_destructor(retval);
			efree(retval);
		}
	}
}

/* }}} */
    /* {{{ php3i_xml_processingInstructionHandler() */

void php3i_xml_processingInstructionHandler(void *userData,
											const XML_Char *target,
											const XML_Char *data)
{
	xml_parser *parser = (xml_parser *)userData;
	XML_TLS_VARS;

	if (parser && parser->processingInstructionHandler) {
		pval *retval, *args[3];

		args[0] = php3i_long_pval(parser->index);
		args[1] = php3i_xmlcharpval(target, 0, parser->target_encoding);
		args[2] = php3i_xmlcharpval(data, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->processingInstructionHandler, 3, args))) {
			php3tls_pval_destructor(retval);
			efree(retval);
		}
	}
}

/* }}} */
    /* {{{ php3i_xml_defaultHandler() */

void php3i_xml_defaultHandler(void *userData, const XML_Char *s, int len)
{
	xml_parser *parser = (xml_parser *)userData;
	XML_TLS_VARS;

	if (parser && parser->defaultHandler) {
		pval *retval, *args[2];

		args[0] = php3i_long_pval(parser->index);
		args[1] = php3i_xmlcharpval(s, len, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->defaultHandler, 2, args))) {
			php3tls_pval_destructor(retval);
			efree(retval);
		}
	}
}

/* }}} */
    /* {{{ php3i_xml_unparsedEntityDeclHandler() */

void php3i_xml_unparsedEntityDeclHandler(void *userData,
										 const XML_Char *entityName,
										 const XML_Char *base,
										 const XML_Char *systemId,
										 const XML_Char *publicId,
										 const XML_Char *notationName)
{
	xml_parser *parser = (xml_parser *)userData;
	XML_TLS_VARS;

	if (parser && parser->unparsedEntityDeclHandler) {
		pval *retval, *args[5];

		args[0] = php3i_long_pval(parser->index);
		args[1] = php3i_xmlcharpval(entityName, 0, parser->target_encoding);
		args[2] = php3i_xmlcharpval(base, 0, parser->target_encoding);
		args[3] = php3i_xmlcharpval(systemId, 0, parser->target_encoding);
		args[4] = php3i_xmlcharpval(publicId, 0, parser->target_encoding);
		args[5] = php3i_xmlcharpval(notationName, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->unparsedEntityDeclHandler, 6, args))) {
			php3tls_pval_destructor(retval);
			efree(retval);
		}
	}
}

/* }}} */
    /* {{{ php3i_xml_notationDeclHandler() */

void
php3i_xml_notationDeclHandler(void *userData,
							  const XML_Char *notationName,
							  const XML_Char *base,
							  const XML_Char *systemId,
							  const XML_Char *publicId)
{
	xml_parser *parser = (xml_parser *)userData;
	XML_TLS_VARS;

	if (parser && parser->notationDeclHandler) {
		pval *retval, *args[5];

		args[0] = php3i_long_pval(parser->index);
		args[1] = php3i_xmlcharpval(notationName, 0, parser->target_encoding);
		args[2] = php3i_xmlcharpval(base, 0, parser->target_encoding);
		args[3] = php3i_xmlcharpval(systemId, 0, parser->target_encoding);
		args[4] = php3i_xmlcharpval(publicId, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->notationDeclHandler, 5, args))) {
			php3tls_pval_destructor(retval);
			efree(retval);
		}
	}
}

/* }}} */
    /* {{{ php3i_xml_externalEntityRefHandler() */

int
php3i_xml_externalEntityRefHandler(XML_Parser parserPtr,
								   const XML_Char *openEntityNames,
								   const XML_Char *base,
								   const XML_Char *systemId,
								   const XML_Char *publicId)
{
	xml_parser *parser = XML_GetUserData(parserPtr);
	int ret = 0; /* abort if no handler is set (should be configurable?) */
	XML_TLS_VARS;

	if (parser && parser->externalEntityRefHandler) {
		pval *retval, *args[5];

		args[0] = php3i_long_pval(parser->index);
		args[1] = php3i_xmlcharpval(openEntityNames, 0, parser->target_encoding);
		args[2] = php3i_xmlcharpval(base, 0, parser->target_encoding);
		args[3] = php3i_xmlcharpval(systemId, 0, parser->target_encoding);
		args[4] = php3i_xmlcharpval(publicId, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->externalEntityRefHandler, 5, args))) {
			convert_to_long(retval);
			ret = retval->value.lval;
			efree(retval);
		} else {
			ret = 0;
		}
	}
	return ret;
}

/* }}} */

/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

/* {{{ int    xml_parser_create() */

PHP_FUNCTION(xml_parser_create)
{
	xml_parser *parser;
	int id, argc;
	pval *encodingArg = NULL;
	XML_Char *encoding;
	char thisfunc[] = "xml_parser_create";
	XML_TLS_VARS;

	argc = ARG_COUNT(ht);

	if (argc > 1 || getParameters(ht, argc, &encodingArg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc == 1) {
		convert_to_string(encodingArg);
		/* The supported encoding types are hardcoded here because
		 * we are limited to the encodings supported by expat/xmltok.
		 */
		if (strncasecmp(encodingArg->value.str.val, "ISO-8859-1",
						encodingArg->value.str.len) == 0) {
			encoding = "ISO-8859-1";
		} else if (strncasecmp(encodingArg->value.str.val, "UTF-8",
						encodingArg->value.str.len) == 0) {
			encoding = "UTF-8";
		} else if (strncasecmp(encodingArg->value.str.val, "US-ASCII",
						encodingArg->value.str.len) == 0) {
			encoding = "US-ASCII";
		} else { /* UTF-16 not supported */
			php3_error(E_WARNING, "%s: unsupported source encoding \"%s\"",
					   thisfunc, encodingArg->value.str.val);
			RETURN_FALSE;
		}
	} else {
		encoding = XML_GLOBAL(php3_xml_module).default_encoding;
	}

	parser = ecalloc(sizeof(xml_parser), 1);
	parser->parser = XML_ParserCreate(encoding);
	parser->target_encoding = encoding;
	XML_SetUserData(parser->parser, parser);
	XML_SetElementHandler(parser->parser, php3i_xml_startElementHandler, php3i_xml_endElementHandler);
	XML_SetCharacterDataHandler(parser->parser, php3i_xml_characterDataHandler);
	XML_SetProcessingInstructionHandler(parser->parser, php3i_xml_processingInstructionHandler);
	XML_SetDefaultHandler(parser->parser, php3i_xml_defaultHandler);
	XML_SetUnparsedEntityDeclHandler(parser->parser, php3i_xml_unparsedEntityDeclHandler);
	XML_SetNotationDeclHandler(parser->parser, php3i_xml_notationDeclHandler);
	XML_SetExternalEntityRefHandler(parser->parser, php3i_xml_externalEntityRefHandler);
	id = php3_list_insert(parser, XML_GLOBAL(php3_xml_module).le_xml_parser);
	parser = xml_get_parser(id, thisfunc, list);
	parser->index = id;
	parser->case_folding = 1;

	RETVAL_LONG(id);
}

/* }}} */
/* {{{ int    xml_set_element_handler(int pind, string shdl, string ehdl) */

PHP_FUNCTION(xml_set_element_handler)
{
	xml_parser *parser;
	pval *pind, *shdl, *ehdl;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 3 ||
		getParameters(ht, 3, &pind, &shdl, &ehdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	convert_to_string(shdl);
	convert_to_string(ehdl);
	parser = xml_get_parser(pind->value.lval, "xml_set_element_handler", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	xml_set_handler(&parser->startElementHandler, shdl);
	xml_set_handler(&parser->endElementHandler, ehdl);
	RETVAL_TRUE;
}

/* }}} */
/* {{{ int    xml_set_character_data_handler(int pind, string hdl) */

PHP_FUNCTION(xml_set_character_data_handler)
{
	xml_parser *parser;
	pval *pind, *hdl;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	convert_to_string(hdl);
	parser = xml_get_parser(pind->value.lval, "xml_set_character_data_handler", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	xml_set_handler(&parser->characterDataHandler, hdl);
	RETVAL_TRUE;
}

/* }}} */
/* {{{ int    xml_set_processing_instruction_handler(int pind, string hdl) */

PHP_FUNCTION(xml_set_processing_instruction_handler)
{
	xml_parser *parser;
	pval *pind, *hdl;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	convert_to_string(hdl);
	parser = xml_get_parser(pind->value.lval, "xml_set_processing_instruction_handler", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	xml_set_handler(&parser->processingInstructionHandler, hdl);
	RETVAL_TRUE;
}

/* }}} */
/* {{{ int    xml_set_default_handler(int pind, string hdl) */

PHP_FUNCTION(xml_set_default_handler)
{
	xml_parser *parser;
	pval *pind, *hdl;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	convert_to_string(hdl);
	parser = xml_get_parser(pind->value.lval, "xml_set_default_handler", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	xml_set_handler(&parser->defaultHandler, hdl);
	RETVAL_TRUE;
}

/* }}} */
/* {{{ int    xml_set_unparsed_entity_decl_handler(int pind, string hdl) */

PHP_FUNCTION(xml_set_unparsed_entity_decl_handler)
{
	xml_parser *parser;
	pval *pind, *hdl;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	convert_to_string(hdl);
	parser = xml_get_parser(pind->value.lval, "xml_set_unparsed_entity_decl_handler", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	xml_set_handler(&parser->unparsedEntityDeclHandler, hdl);
	RETVAL_TRUE;
}

/* }}} */
/* {{{ int    xml_set_notation_decl_handler(int pind, string hdl) */

PHP_FUNCTION(xml_set_notation_decl_handler)
{
	xml_parser *parser;
	pval *pind, *hdl;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	convert_to_string(hdl);
	parser = xml_get_parser(pind->value.lval, "xml_set_notation_decl_handler", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	xml_set_handler(&parser->notationDeclHandler, hdl);
	RETVAL_TRUE;
}

/* }}} */
/* {{{ int    xml_set_external_entity_ref_handler(int pind, string hdl) */

PHP_FUNCTION(xml_set_external_entity_ref_handler)
{
	xml_parser *parser;
	pval *pind, *hdl;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &pind, &hdl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	convert_to_string(hdl);
	parser = xml_get_parser(pind->value.lval, "xml_set_external_entity_ref_handler", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	xml_set_handler(&parser->externalEntityRefHandler, hdl);
	RETVAL_TRUE;
}

/* }}} */
/* {{{ int    xml_parse(int pind, string data[, int isFinal]) */

PHP_FUNCTION(xml_parse)
{
	xml_parser *parser;
	pval *pind, *data, *final;
	int argc, isFinal, ret;
	XML_TLS_VARS;

	argc = ARG_COUNT(ht);
	if (argc < 2 || argc > 3 || getParameters(ht, argc, &pind, &data, &final) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	convert_to_string(data);
	if (argc == 3) {
		convert_to_long(final);
		isFinal = final->value.lval;
	} else {
		isFinal = 0;
	}
	parser = xml_get_parser(pind->value.lval, "XML_Parse", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	fflush(stdout);
	ret = XML_Parse(parser->parser, data->value.str.val, data->value.str.len, isFinal);
	RETVAL_LONG(ret);
}

/* }}} */
/* {{{ int    xml_get_error_code(int pind) */

PHP_FUNCTION(xml_get_error_code)
{
	xml_parser *parser;
	pval *pind;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &pind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	parser = xml_get_parser(pind->value.lval, "xml_get_error_code", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	RETVAL_LONG((long)XML_GetErrorCode(parser->parser));
}

/* }}} */
/* {{{ string xml_error_string(int code) */

PHP_FUNCTION(xml_error_string)
{
	pval *code;
	char *str;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &code) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(code);
	str = (char *)XML_ErrorString((int)code->value.lval);
	if (str) {
		RETVAL_STRING(str, 1);
	}
}

/* }}} */
/* {{{ int    xml_get_current_line_number(int pind) */

PHP_FUNCTION(xml_get_current_line_number)
{
	xml_parser *parser;
	pval *pind;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &pind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	parser = xml_get_parser(pind->value.lval, "xml_get_current_line_number", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	RETVAL_LONG(XML_GetCurrentLineNumber(parser->parser));
}

/* }}} */
/* {{{ int    xml_get_current_column_number(int pind) */

PHP_FUNCTION(xml_get_current_column_number)
{
	xml_parser *parser;
	pval *pind;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &pind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	parser = xml_get_parser(pind->value.lval, "xml_get_current_column_number", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	RETVAL_LONG(XML_GetCurrentColumnNumber(parser->parser));
}

/* }}} */
/* {{{ int    xml_get_current_byte_index(int pind) */

PHP_FUNCTION(xml_get_current_byte_index)
{
	xml_parser *parser;
	pval *pind;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &pind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	parser = xml_get_parser(pind->value.lval, "xml_get_current_byte_index", list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	RETVAL_LONG(XML_GetCurrentByteIndex(parser->parser));
}

/* }}} */
/* {{{ int    xml_parser_free(int pind) */

PHP_FUNCTION(xml_parser_free)
{
	pval *pind;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &pind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	if (php3_list_delete(pind->value.lval) == FAILURE) {
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}

/* }}} */
/* {{{ int    xml_parser_set_option(int pind, int option, mixed value) */

PHP_FUNCTION(xml_parser_set_option)
{
	xml_parser *parser;
	pval *pind, *opt, *val;
	char thisfunc[] = "xml_parser_set_option";
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &pind, &opt, &val) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	convert_to_long(opt);
	parser = xml_get_parser(pind->value.lval, thisfunc, list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	switch (opt->value.lval) {
		case PHP3_XML_OPTION_CASE_FOLDING:
			convert_to_long(val);
			parser->case_folding = val->value.lval;
			break;
		case PHP3_XML_OPTION_TARGET_ENCODING: {
			xml_encoding *enc = xml_get_encoding(val->value.str.val);
			if (enc == NULL) {
				php3_error(E_WARNING, "%s: unsupported target encoding \"%s\"",
						   thisfunc, val->value.str.val);
				RETURN_FALSE;
			}
			parser->target_encoding = enc->name;
			break;
		}
		default:
			php3_error(E_WARNING, "%s: unknown option", thisfunc);
			RETURN_FALSE;
			break;
	}
	RETVAL_TRUE;
}

/* }}} */
/* {{{ int    xml_parser_get_option(int pind, int option) */

PHP_FUNCTION(xml_parser_get_option)
{
	xml_parser *parser;
	pval *pind, *opt;
	char thisfunc[] = "xml_parser_get_option";
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &pind, &opt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(pind);
	convert_to_long(opt);
	parser = xml_get_parser(pind->value.lval, thisfunc, list);
	if (parser == NULL) {
		RETURN_FALSE;
	}
	switch (opt->value.lval) {
		case PHP3_XML_OPTION_CASE_FOLDING:
			RETURN_LONG(parser->case_folding);
			break;
		case PHP3_XML_OPTION_TARGET_ENCODING:
			RETURN_STRING(parser->target_encoding, 1);
			break;
		default:
			php3_error(E_WARNING, "%s: unknown option", thisfunc);
			RETURN_FALSE;
			break;
	}
	RETVAL_FALSE;
}

/* }}} */
/* {{{ string utf8_encode(string data) */

PHP_FUNCTION(utf8_encode)
{
	pval *arg;
	XML_Char *encoded;
	int len;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);
	encoded = xml_utf8_encode(arg->value.str.val, arg->value.str.len, &len, "ISO-8859-1");
	if (encoded == NULL) {
		RETURN_FALSE;
	}
	RETVAL_STRINGL(encoded, len, 0);
}

/* }}} */
/* {{{ string utf8_decode(string data) */

PHP_FUNCTION(utf8_decode)
{
	pval *arg;
	XML_Char *decoded;
	int len;
	XML_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);
	decoded = xml_utf8_decode(arg->value.str.val, arg->value.str.len, &len, "ISO-8859-1");
	if (decoded == NULL) {
		RETURN_FALSE;
	}
	RETVAL_STRINGL(decoded, len, 0);
}

/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
