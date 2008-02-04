%{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2006 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   |          Jani Taskinen <jani@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#define DEBUG_CFG_SCANNER 0

#define yyleng SCNG(yy_leng)
#define yytext SCNG(yy_text)
#define yytext_ptr SCNG(yy_text)
#define yyin SCNG(yy_in)
#define yyout SCNG(yy_out)

/* How it works (for the core ini directives):
 * ===========================================
 *
 * 1. Scanner scans file for tokens and passes them to parser.
 * 2. Parser parses the tokens and passes the name/value pairs to the callback
 *    function which stores them in the configuration hash table.
 * 3. Later REGISTER_INI_ENTRIES() is called which triggers the actual
 *    registering of ini entries and uses zend_get_configuration_directive()
 *    to fetch the previously stored name/value pair from configuration hash table
 *    and registers the static ini entries which match the name to the value
 *    into EG(ini_directives) hash table.
 * 4. PATH section entries are used per-request from down to top, each overriding
 *    previous if one exists. zend_alter_ini_entry() is called for each entry.
 *    Settings in PATH section are ZEND_INI_SYSTEM accessible and thus mimics the
 *    php_admin_* directives used within Apache httpd.conf when PHP is compiled as
 *    module for Apache.
 * 5. User defined ini files (like .htaccess for apache) are parsed for each request and
 *    stored in separate hash defined by SAPI.
 */
 
/* TODO: (ordered by importance :-)
 * ===============================================================================
 *
 *  - Separate constant lookup totally from plain strings (using CONSTANT pattern)
 *  - Add #if .. #else .. #endif and ==, !=, <, > , <=, >= operators
 *  - Add #include "some.ini"
 *  - Allow variables to refer to options also when using parse_ini_file()
 *
 */

/* These are not needed when yymore() is not used */
/*
#define yy_last_accepting_state SCNG(_yy_last_accepting_state)
#define yy_last_accepting_cpos SCNG(_yy_last_accepting_cpos)
#define yy_more_flag SCNG(_yy_more_flag)
#define yy_more_len SCNG(_yy_more_len)
*/

%}

%x ST_DOUBLE_QUOTES
%x ST_OFFSET
%x ST_RAW
%x ST_SECTION_RAW
%x ST_SECTION_VALUE
%x ST_VALUE
%x ST_VARNAME
%option stack

%{

#include <errno.h>
#include "zend.h"
#include "zend_globals.h"
#include <zend_ini_parser.h>
#include "zend_ini_scanner.h"

#define YY_DECL int ini_lex(zval *ini_lval TSRMLS_DC)

#define YY_INPUT(buf, result, max_size)                                    \
	if ( ((result = zend_stream_read(yyin, buf, max_size TSRMLS_CC)) == 0) \
		&& zend_stream_ferror( yyin TSRMLS_CC) )                           \
		YY_FATAL_ERROR( "input in flex scanner failed" );

/* Globals Macros */
#define SCNG	INI_SCNG
#ifdef ZTS
ZEND_API ts_rsrc_id ini_scanner_globals_id;
#else
ZEND_API zend_scanner_globals ini_scanner_globals;
#endif

/* Eat trailing whitespace + extra char */
#define EAT_TRAILING_WHITESPACE_EX(ch)              \
	while (yyleng > 0 && (                          \
		(ch != 'X' && yytext[yyleng - 1] ==  ch) || \
		yytext[yyleng - 1] == '\n' ||               \
		yytext[yyleng - 1] == '\r' ||               \
		yytext[yyleng - 1] == '\t' ||               \
		yytext[yyleng - 1] == ' ')                  \
	) {                                             \
		yyleng--;                                   \
		yytext[yyleng]=0;                           \
	}

/* Eat trailing whitespace */
#define EAT_TRAILING_WHITESPACE()	EAT_TRAILING_WHITESPACE_EX('X')

#define zend_ini_copy_value(retval, str, len) {  \
	Z_STRVAL_P(retval) = zend_strndup(str, len); \
	Z_STRLEN_P(retval) = len;                    \
	Z_TYPE_P(retval) = IS_STRING;                \
}

#define RETURN_TOKEN(type, str, len) {           \
	zend_ini_copy_value(ini_lval, str, len);     \
	return type;                                 \
}

static char *ini_filename;

/* {{{ init_ini_scanner()
*/
static void init_ini_scanner(TSRMLS_D)
{
	SCNG(lineno) = 1;
	SCNG(scanner_mode) = ZEND_INI_SCANNER_NORMAL;
	SCNG(yy_start_stack_ptr) = 0;
	SCNG(yy_start_stack_depth) = 0;
	SCNG(current_buffer) = NULL;
}
/* }}} */

/* {{{ shutdown_ini_scanner()
*/
void shutdown_ini_scanner(TSRMLS_D)
{
	if (SCNG(yy_start_stack)) {
		yy_flex_free(SCNG(yy_start_stack));
		SCNG(yy_start_stack) = NULL;
	}
	yy_delete_buffer(SCNG(current_buffer) TSRMLS_CC);
	if (ini_filename) {
		free(ini_filename);
	}
}
/* }}} */

/* {{{ zend_ini_scanner_get_lineno()
*/
int zend_ini_scanner_get_lineno(TSRMLS_D)
{
	return SCNG(lineno);
}
/* }}} */

/* {{{ zend_ini_scanner_get_filename()
*/
char *zend_ini_scanner_get_filename(TSRMLS_D)
{
	return ini_filename;
}
/* }}} */

/* {{{ zend_ini_open_file_for_scanning()
*/
int zend_ini_open_file_for_scanning(zend_file_handle *fh, int scanner_mode TSRMLS_DC)
{
	if (FAILURE == zend_stream_fixup(fh TSRMLS_CC)) {
		return FAILURE;
	}

	init_ini_scanner(TSRMLS_C);
	SCNG(scanner_mode) = scanner_mode;
	yyin = fh;
	yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE TSRMLS_CC) TSRMLS_CC);
	ini_filename = zend_strndup(fh->filename, strlen(fh->filename));
	return SUCCESS;
}
/* }}} */

/* {{{ zend_ini_prepare_string_for_scanning()
*/
int zend_ini_prepare_string_for_scanning(char *str, int scanner_mode TSRMLS_DC)
{
	int len = strlen(str);

	init_ini_scanner(TSRMLS_C);
	SCNG(scanner_mode) = scanner_mode;
	yyin = NULL;
	yy_scan_buffer(str, len + 2 TSRMLS_CC);
	ini_filename = NULL;
	return SUCCESS;
}
/* }}} */

/* {{{ zend_ini_close_file()
*/
void zend_ini_close_file(zend_file_handle *fh TSRMLS_DC)
{
	zend_stream_close(fh);
}
/* }}} */

/* {{{ zend_ini_escape_string()
 */
static void zend_ini_escape_string(zval *lval, char *str, int len, char quote_type TSRMLS_DC)
{
	register char *s, *t;
	char *end;

	zend_ini_copy_value(lval, str, len);

	/* convert escape sequences */
	s = t = Z_STRVAL_P(lval);
	end = s + Z_STRLEN_P(lval);

	while (s < end) {
		if (*s == '\\') {
			s++;
			if (s >= end) {
				continue;
			}
			switch (*s) {
				case 'n':
					*t++ = '\n';
					Z_STRLEN_P(lval)--;
					break;
				case 'r':
					*t++ = '\r';
					Z_STRLEN_P(lval)--;
					break;
				case 't':
					*t++ = '\t';
					Z_STRLEN_P(lval)--;
					break;
				case '"':
					if (*s != quote_type) {
						*t++ = '\\';
						*t++ = *s;
						break;
					}
				case '\\':
				case '$':
					*t++ = *s;
					Z_STRLEN_P(lval)--;
					break;
				default:
					*t++ = '\\';
					*t++ = *s;
					break;
			}
		} else {
			*t++ = *s;
		}
		if (*s == '\n' || (*s == '\r' && (*(s+1) != '\n'))) {
			SCNG(lineno)++;
		}
		s++;
	}
	*t = 0;
}
/* }}} */

%}

LNUM [0-9]+
DNUM ([0-9]*[\.][0-9]+)|([0-9]+[\.][0-9]*)
NUMBER [-]?{LNUM}|{DNUM}
ANY_CHAR (.|[\n\t])
NEWLINE	("\r"|"\n"|"\r\n")
TABS_AND_SPACES [ \t]
WHITESPACE [ \t]+
CONSTANT [a-zA-Z][a-zA-Z0-9_]*
LABEL [a-zA-Z0-9][a-zA-Z0-9._-]*
TOKENS [:,.\[\]"'()|^&+-/*=%$!~<>?@{}]
OPERATORS [&|~()!]
DOLLAR_CURLY "${"

SECTION_RAW_CHARS [^\]\n\r]
SINGLE_QUOTED_CHARS [^']
RAW_VALUE_CHARS [^=\n\r;]

/* Allow using ${foobar} in sections, quoted strings and values */
LITERAL_DOLLAR ("$"([^a-zA-Z0-9{]|("\\"{ANY_CHAR})))
VALUE_CHARS         ([^$= \t\n\r;&|~()!"']|{LITERAL_DOLLAR})
SECTION_VALUE_CHARS ([^$\n\r;"'\]\\]|("\\"{ANY_CHAR})|{LITERAL_DOLLAR})
DOUBLE_QUOTES_CHARS ([^$"\\]|("\\"{ANY_CHAR})|{LITERAL_DOLLAR})

/* " */

%option nounput
%option noyywrap
%option noyylineno
%option noyy_top_state
%option never-interactive

%%

<INITIAL>"[" { /* Section start */
	/* Enter section data lookup state */
	if (SCNG(scanner_mode) == ZEND_INI_SCANNER_RAW) {
		yy_push_state(ST_SECTION_RAW TSRMLS_CC);
	} else {
		yy_push_state(ST_SECTION_VALUE TSRMLS_CC);
	}
	return TC_SECTION;
}

<ST_VALUE,ST_SECTION_VALUE,ST_OFFSET>"'"{SINGLE_QUOTED_CHARS}+"'" { /* Raw string */
	/* Eat leading and trailing single quotes */
	if (yytext[0] == '\'' && yytext[yyleng - 1] == '\'') {
		yytext++;
		yyleng = yyleng - 2;
		yytext[yyleng] = 0; 
	}
	RETURN_TOKEN(TC_RAW, yytext, yyleng);
}

<ST_SECTION_RAW,ST_SECTION_VALUE>"]"{TABS_AND_SPACES}*{NEWLINE}? { /* End of section */
	BEGIN(INITIAL);
	SCNG(lineno)++;
	return ']';
}

<INITIAL>{LABEL}"["{TABS_AND_SPACES}* { /* Start of option with offset */
	/* Eat trailing whitespace and [ */
	EAT_TRAILING_WHITESPACE_EX('[');

	/* Enter offset lookup state */
	yy_push_state(ST_OFFSET TSRMLS_CC);
	
	RETURN_TOKEN(TC_OFFSET, yytext, yyleng);
}

<ST_OFFSET>{TABS_AND_SPACES}*"]" { /* End of section or an option offset */
	BEGIN(INITIAL);
	return ']';
}

<ST_DOUBLE_QUOTES,ST_SECTION_VALUE,ST_VALUE,ST_OFFSET>{DOLLAR_CURLY} { /* Variable start */
	yy_push_state(ST_VARNAME TSRMLS_CC);
	return TC_DOLLAR_CURLY;
}

<ST_VARNAME>{LABEL} { /* Variable name */
	RETURN_TOKEN(TC_VARNAME, yytext, yyleng);
}

<ST_VARNAME>"}" { /* Variable end */
	yy_pop_state(TSRMLS_C);
	return '}';
}

<INITIAL,ST_VALUE>("true"|"on"|"yes"){TABS_AND_SPACES}* { /* TRUE value (when used outside option value/offset this causes parse error!) */
	RETURN_TOKEN(BOOL_TRUE, "1", 1);
}

<INITIAL,ST_VALUE>("false"|"off"|"no"|"none"|"null"){TABS_AND_SPACES}* { /* FALSE value (when used outside option value/offset this causes parse error!)*/
	RETURN_TOKEN(BOOL_FALSE, "", 0);
}

<INITIAL>{LABEL} { /* Get option name */
	RETURN_TOKEN(TC_LABEL, yytext, yyleng);
}

<INITIAL>{TABS_AND_SPACES}*[=]{TABS_AND_SPACES}* { /* Start option value */ 
	if (SCNG(scanner_mode) == ZEND_INI_SCANNER_RAW) {
		yy_push_state(ST_RAW TSRMLS_CC);
	} else {
		yy_push_state(ST_VALUE TSRMLS_CC);
	}
	return '=';
}

<ST_RAW>{RAW_VALUE_CHARS}+ { /* Raw value, only used when SCNG(scanner_mode) == ZEND_INI_SCANNER_RAW. */
	/* Eat leading and trailing double quotes */
	if (yytext[0] == '"' && yytext[yyleng - 1] == '"') {
		yytext++;
		yyleng = yyleng - 2;
		yytext[yyleng] = 0; 
	}
	RETURN_TOKEN(TC_RAW, yytext, yyleng);
}

<ST_SECTION_RAW>{SECTION_RAW_CHARS}+ { /* Raw value, only used when SCNG(scanner_mode) == ZEND_INI_SCANNER_RAW. */
	RETURN_TOKEN(TC_RAW, yytext, yyleng);
}

<ST_VALUE,ST_RAW>{TABS_AND_SPACES}*{NEWLINE} { /* End of option value */
	BEGIN(INITIAL);
	SCNG(lineno)++;
	return END_OF_LINE;
}

<ST_SECTION_VALUE,ST_VALUE,ST_OFFSET>{CONSTANT} { /* Get constant option value */
	RETURN_TOKEN(TC_CONSTANT, yytext, yyleng);
}

<ST_SECTION_VALUE,ST_VALUE,ST_OFFSET>{NUMBER} { /* Get number option value as string */
	RETURN_TOKEN(TC_NUMBER, yytext, yyleng);
}

<INITIAL>{TOKENS} { /* Disallow these chars outside option values */
	return yytext[0];
}

<ST_VALUE>{OPERATORS}{TABS_AND_SPACES}* { /* Boolean operators */
	return yytext[0];
}

<ST_VALUE>[=] { /* Make = used in option value to trigger error */
	yyless(yyleng - 1);
	BEGIN(INITIAL);
	return END_OF_LINE;
}

<ST_VALUE>{VALUE_CHARS}+ { /* Get everything else as option/offset value */
	RETURN_TOKEN(TC_STRING, yytext, yyleng);
}

<ST_SECTION_VALUE,ST_OFFSET>{SECTION_VALUE_CHARS}+ { /* Get rest as section/offset value */
	RETURN_TOKEN(TC_STRING, yytext, yyleng);
}

<ST_SECTION_VALUE,ST_VALUE,ST_OFFSET>{TABS_AND_SPACES}*["] { /* Double quoted '"' string start */
	yy_push_state(ST_DOUBLE_QUOTES TSRMLS_CC);
	return '"';
}

<ST_DOUBLE_QUOTES>{DOUBLE_QUOTES_CHARS}+ { /* Escape double quoted string contents */
	zend_ini_escape_string(ini_lval, yytext, yyleng, '"' TSRMLS_CC);
	return TC_QUOTED_STRING;
}

<ST_DOUBLE_QUOTES>["]{TABS_AND_SPACES}* { /* Double quoted '"' string ends */
	yy_pop_state(TSRMLS_C);
	return '"';
}

<ST_SECTION_VALUE,ST_VALUE,ST_OFFSET>{WHITESPACE} {
	RETURN_TOKEN(TC_WHITESPACE, yytext, yyleng);
}

<INITIAL,ST_RAW>{TABS_AND_SPACES}+ {
	/* eat whitespace */
}

<INITIAL>{TABS_AND_SPACES}*{NEWLINE} {
	SCNG(lineno)++;
	return END_OF_LINE;
}

<INITIAL,ST_VALUE,ST_RAW>{TABS_AND_SPACES}*[;][^\r\n]*{NEWLINE} { /* Comment */
	BEGIN(INITIAL);
	SCNG(lineno)++;
	return END_OF_LINE;
}

<ST_VALUE,ST_RAW><<EOF>> { /* End of option value (if EOF is reached before EOL */
	BEGIN(INITIAL);
	return END_OF_LINE;
}

<<EOF>> {
#if DEBUG_CFG_SCANNER
	while (YYSTATE != INITIAL) {
		switch (YYSTATE) {
		case INITIAL:
			break;

		case ST_DOUBLE_QUOTES:
			fprintf(stderr, "ERROR: Unterminated ini option value double quotes\n");
			break;

		case ST_OFFSET:
			fprintf(stderr, "ERROR: Unterminated ini option offset\n");
			break;

		case ST_RAW:
			fprintf(stderr, "ERROR: Unterminated raw ini option value\n");
			break;

		case ST_SECTION_RAW:
			fprintf(stderr, "ERROR: Unterminated raw ini section value\n");
			break;

		case ST_SECTION_VALUE:
			fprintf(stderr, "ERROR: Unterminated ini section value\n");
			break;

		case ST_VALUE:
			fprintf(stderr, "ERROR: Unterminated ini option value\n");
			break;

		case ST_VARNAME:
			fprintf(stderr, "ERROR: Unterminated ini variable\n");
			break;

		default:
			fprintf(stderr, "BUG: Unknown state (%d)\n", YYSTATE);
			break;
		}
		yy_pop_state(TSRMLS_C);
	}
#endif
	yyterminate();
}
