/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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
   |          Marcus Boerger <helly@php.net>                              |
   |          Nuno Lopes <nlopess@php.net>                                |
   |          Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <errno.h>
#include "zend.h"
#include "zend_globals.h"
#include <zend_ini_parser.h>
#include "zend_ini_scanner.h"

#if 0
# define YYDEBUG(s, c) printf("state: %d char: %c\n", s, c)
#else
# define YYDEBUG(s, c)
#endif

#include "zend_ini_scanner_defs.h"

#define YYCTYPE   unsigned char
/* allow the scanner to read one null byte after the end of the string (from ZEND_MMAP_AHEAD)
 * so that if will be able to terminate to match the current token (e.g. non-enclosed string) */
#define YYFILL(n) { if (YYCURSOR > YYLIMIT) return 0; }
#define YYCURSOR  SCNG(yy_cursor)
#define YYLIMIT   SCNG(yy_limit)
#define YYMARKER  SCNG(yy_marker)

#define YYGETCONDITION()  SCNG(yy_state)
#define YYSETCONDITION(s) SCNG(yy_state) = s

#define STATE(name)  yyc##name

/* emulate flex constructs */
#define BEGIN(state) YYSETCONDITION(STATE(state))
#define YYSTATE      YYGETCONDITION()
#define yytext       ((char*)SCNG(yy_text))
#define yyleng       SCNG(yy_leng)
#define yyless(x)    do {	YYCURSOR = (unsigned char*)yytext + x; \
							yyleng   = (unsigned int)x; } while(0)

/* #define yymore()     goto yymore_restart */

/* perform sanity check. If this message is triggered you should
   increase the ZEND_MMAP_AHEAD value in the zend_streams.h file */
/*!max:re2c */
#if ZEND_MMAP_AHEAD < (YYMAXFILL + 1)
# error ZEND_MMAP_AHEAD should be greater than YYMAXFILL
#endif


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

/* Globals Macros */
#define SCNG	INI_SCNG
#ifdef ZTS
ZEND_API ts_rsrc_id ini_scanner_globals_id;
#else
ZEND_API zend_ini_scanner_globals ini_scanner_globals;
#endif

/* Eat leading whitespace */
#define EAT_LEADING_WHITESPACE()                     \
	while (yytext[0]) {                              \
		if (yytext[0] == ' ' || yytext[0] == '\t') { \
			SCNG(yy_text)++;                         \
			yyleng--;                                \
		} else {                                     \
			break;                                   \
		}                                            \
	}

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

static void _yy_push_state(int new_state TSRMLS_DC)
{
	zend_stack_push(&SCNG(state_stack), (void *) &YYGETCONDITION(), sizeof(int));
	YYSETCONDITION(new_state);
}

#define yy_push_state(state_and_tsrm) _yy_push_state(yyc##state_and_tsrm)

static void yy_pop_state(TSRMLS_D)
{
	int *stack_state;
	zend_stack_top(&SCNG(state_stack), (void **) &stack_state);
	YYSETCONDITION(*stack_state);
	zend_stack_del_top(&SCNG(state_stack));
}

static void yy_scan_buffer(char *str, unsigned int len TSRMLS_DC)
{
	YYCURSOR = (YYCTYPE*)str;
	SCNG(yy_start) = YYCURSOR;
	YYLIMIT  = YYCURSOR + len;
}

#define ini_filename SCNG(filename)

/* {{{ init_ini_scanner()
*/
static int init_ini_scanner(int scanner_mode, zend_file_handle *fh TSRMLS_DC)
{
	/* Sanity check */
	if (scanner_mode != ZEND_INI_SCANNER_NORMAL && scanner_mode != ZEND_INI_SCANNER_RAW) {
		zend_error(E_WARNING, "Invalid scanner mode");
		return FAILURE;
	}

	SCNG(lineno) = 1;
	SCNG(scanner_mode) = scanner_mode;
	SCNG(yy_in) = fh;

	if (fh != NULL) {
		ini_filename = zend_strndup(fh->filename, strlen(fh->filename));
	} else {
		ini_filename = NULL;
	}

	zend_stack_init(&SCNG(state_stack));
	BEGIN(INITIAL);

	return SUCCESS;
}
/* }}} */

/* {{{ shutdown_ini_scanner()
*/
void shutdown_ini_scanner(TSRMLS_D)
{
	zend_stack_destroy(&SCNG(state_stack));
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
	return ini_filename ? ini_filename : "Unknown";
}
/* }}} */

/* {{{ zend_ini_open_file_for_scanning()
*/
int zend_ini_open_file_for_scanning(zend_file_handle *fh, int scanner_mode TSRMLS_DC)
{
	char *buf;
	size_t size;

	if (zend_stream_fixup(fh, &buf, &size TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}

	if (init_ini_scanner(scanner_mode, fh TSRMLS_CC) == FAILURE) {
		zend_file_handle_dtor(fh TSRMLS_CC);
		return FAILURE;
	}

	yy_scan_buffer(buf, size TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ zend_ini_prepare_string_for_scanning()
*/
int zend_ini_prepare_string_for_scanning(char *str, int scanner_mode TSRMLS_DC)
{
	int len = strlen(str);

	if (init_ini_scanner(scanner_mode, NULL TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}

	yy_scan_buffer(str, len TSRMLS_CC);

	return SUCCESS;
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
				*t++ = '\\';
				continue;
			}
			switch (*s) {
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

int ini_lex(zval *ini_lval TSRMLS_DC)
{
restart:
	SCNG(yy_text) = YYCURSOR;

/* yymore_restart: */
	/* detect EOF */
	if (YYCURSOR >= YYLIMIT) {
		if (YYSTATE == STATE(ST_VALUE) || YYSTATE == STATE(ST_RAW)) {
			BEGIN(INITIAL);
			return END_OF_LINE;
		}
		return 0;
	}

	/* Eat any UTF-8 BOM we find in the first 3 bytes */
	if (YYCURSOR == SCNG(yy_start) && YYCURSOR + 3 < YYLIMIT) {
		if (memcmp(YYCURSOR, "\xef\xbb\xbf", 3) == 0) {
			YYCURSOR += 3;
			goto restart;
		}
	}
/*!re2c
re2c:yyfill:check = 0;
LNUM [0-9]+
DNUM ([0-9]*[\.][0-9]+)|([0-9]+[\.][0-9]*)
NUMBER [-]?{LNUM}|{DNUM}
ANY_CHAR (.|[\n\t])
NEWLINE	("\r"|"\n"|"\r\n")
TABS_AND_SPACES [ \t]
WHITESPACE [ \t]+
CONSTANT [a-zA-Z_][a-zA-Z0-9_]*
LABEL [^=\n\r\t;&|^$~(){}!"\[]+
TOKENS [:,.\[\]"'()&|^+-/*=%$!~<>?@{}]
OPERATORS [&|^~()!]
DOLLAR_CURLY "${"

SECTION_RAW_CHARS [^\]\n\r]
SINGLE_QUOTED_CHARS [^']
RAW_VALUE_CHARS [^\n\r;\000]

LITERAL_DOLLAR ("$"([^{\000]|("\\"{ANY_CHAR})))
VALUE_CHARS         ([^$= \t\n\r;&|^~()!"'\000]|{LITERAL_DOLLAR})
SECTION_VALUE_CHARS ([^$\n\r;"'\]\\]|("\\"{ANY_CHAR})|{LITERAL_DOLLAR})

<!*> := yyleng = YYCURSOR - SCNG(yy_text);

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
		SCNG(yy_text)++;
		yyleng = yyleng - 2;
	}
	RETURN_TOKEN(TC_RAW, yytext, yyleng);
}

<ST_SECTION_RAW,ST_SECTION_VALUE>"]"{TABS_AND_SPACES}*{NEWLINE}? { /* End of section */
	BEGIN(INITIAL);
	SCNG(lineno)++;
	return ']';
}

<INITIAL>{LABEL}"["{TABS_AND_SPACES}* { /* Start of option with offset */
	/* Eat leading whitespace */
	EAT_LEADING_WHITESPACE();
	
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
	/* Eat leading whitespace */
	EAT_LEADING_WHITESPACE();

	/* Eat trailing whitespace */
	EAT_TRAILING_WHITESPACE();

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
	/* Eat leading whitespace */
	EAT_LEADING_WHITESPACE();

	/* Eat trailing whitespace */
	EAT_TRAILING_WHITESPACE();

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

<ST_RAW>{RAW_VALUE_CHARS} { /* Raw value, only used when SCNG(scanner_mode) == ZEND_INI_SCANNER_RAW. */
	char *sc = NULL;
	while (YYCURSOR < YYLIMIT) {
		switch (*YYCURSOR) {
			case '\n':
			case '\r':
				goto end_raw_value_chars;
				break;
			case ';':
				if (sc == NULL) {
					sc = YYCURSOR;
				}
				/* no break */
			default:
				YYCURSOR++;
				break;
		}
	}
end_raw_value_chars:
	yyleng = YYCURSOR - SCNG(yy_text);

	/* Eat trailing semicolons */
	while (yytext[yyleng - 1] == ';') {
		yyleng--;
	}

	/* Eat leading and trailing double quotes */
	if (yytext[0] == '"' && yytext[yyleng - 1] == '"') {
		SCNG(yy_text)++;
		yyleng = yyleng - 2;
	} else if (sc) {
		YYCURSOR = sc;
		yyleng = YYCURSOR - SCNG(yy_text);
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
	yyless(0);
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

<ST_DOUBLE_QUOTES>["]{TABS_AND_SPACES}* { /* Double quoted '"' string ends */
	yy_pop_state(TSRMLS_C);
	return '"';
}

<ST_DOUBLE_QUOTES>[^] { /* Escape double quoted string contents */
	if (YYCURSOR > YYLIMIT) {
		return 0;
	}
	
	while (YYCURSOR < YYLIMIT) {
		switch (*YYCURSOR++) {
			case '"':
				if (YYCURSOR < YYLIMIT && YYCURSOR[-2] == '\\' && *YYCURSOR != '\r' && *YYCURSOR != '\n') {
					continue;
				}
				break;
			case '$':
				if (*YYCURSOR == '{') {
					break;
				}
				continue;
			case '\\':
				if (YYCURSOR < YYLIMIT && *YYCURSOR != '"') {
					YYCURSOR++;
				}
				/* fall through */
			default:
				continue;
		}
		
		YYCURSOR--;
		break;
	}

	yyleng = YYCURSOR - SCNG(yy_text);
	
	zend_ini_escape_string(ini_lval, yytext, yyleng, '"' TSRMLS_CC);
	return TC_QUOTED_STRING;
}

<ST_SECTION_VALUE,ST_VALUE,ST_OFFSET>{WHITESPACE} {
	RETURN_TOKEN(TC_WHITESPACE, yytext, yyleng);
}

<INITIAL,ST_RAW>{TABS_AND_SPACES}+ {
	/* eat whitespace */
	goto restart;
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

<INITIAL>{TABS_AND_SPACES}*[#][^\r\n]*{NEWLINE} { /* #Comment */
	zend_error(E_DEPRECATED, "Comments starting with '#' are deprecated in %s on line %d", zend_ini_scanner_get_filename(TSRMLS_C), SCNG(lineno));
	BEGIN(INITIAL);
	SCNG(lineno)++;
	return END_OF_LINE;
}

<ST_VALUE,ST_RAW>[^] { /* End of option value (if EOF is reached before EOL */
	BEGIN(INITIAL);
	return END_OF_LINE;
}

<*>[^] {
	return 0;
}

*/
}
