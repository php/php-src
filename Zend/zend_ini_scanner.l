/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_API.h"
#include "zend_globals.h"
#include <zend_ini_parser.h>
#include "zend_ini_scanner.h"

#ifdef YYDEBUG
#undef YYDEBUG
#endif

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
	while (yyleng) {                                 \
		if (yytext[0] == ' ' || yytext[0] == '\t') { \
			SCNG(yy_text)++;                         \
			yyleng--;                                \
		} else {                                     \
			break;                                   \
		}                                            \
	}

/* Eat trailing whitespace + extra char */
#define EAT_TRAILING_WHITESPACE_EX(ch)              \
	while (yyleng && (                              \
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

#define zend_ini_copy_value(retval, str, len)	\
	ZVAL_NEW_STR(retval, zend_string_init(str, len, 1))


#define RETURN_TOKEN(type, str, len) {                       \
	if (SCNG(scanner_mode) == ZEND_INI_SCANNER_TYPED) {      \
		zend_ini_copy_typed_value(ini_lval, type, str, len); \
	} else {                                                 \
		zend_ini_copy_value(ini_lval, str, len);             \
	}                                                        \
	return type;                                             \
}

static inline int convert_to_number(zval *retval, const char *str, const int str_len)
{
	zend_uchar type;
	int overflow;
	zend_long lval;
	double dval;

	if ((type = is_numeric_string_ex(str, str_len, &lval, &dval, 0, &overflow)) != 0) {
		if (type == IS_LONG) {
			ZVAL_LONG(retval, lval);
			return SUCCESS;
		} else if (type == IS_DOUBLE && !overflow) {
			ZVAL_DOUBLE(retval, dval);
			return SUCCESS;
		}
	}

	return FAILURE;
}

static void zend_ini_copy_typed_value(zval *retval, const int type, const char *str, int len)
{
	switch (type) {
		case BOOL_FALSE:
		case BOOL_TRUE:
			ZVAL_BOOL(retval, type == BOOL_TRUE);
			break;

		case NULL_NULL:
			ZVAL_NULL(retval);
			break;

		case TC_NUMBER:
			if (convert_to_number(retval, str, len) == SUCCESS) {
				break;
			}
			/* intentional fall-through */
		default:
			zend_ini_copy_value(retval, str, len);
	}
}

static void _yy_push_state(int new_state)
{
	zend_stack_push(&SCNG(state_stack), (void *) &YYGETCONDITION());
	YYSETCONDITION(new_state);
}

#define yy_push_state(state_and_tsrm) _yy_push_state(yyc##state_and_tsrm)

static void yy_pop_state(void)
{
	int *stack_state = zend_stack_top(&SCNG(state_stack));
	YYSETCONDITION(*stack_state);
	zend_stack_del_top(&SCNG(state_stack));
}

static void yy_scan_buffer(char *str, unsigned int len)
{
	YYCURSOR = (YYCTYPE*)str;
	SCNG(yy_start) = YYCURSOR;
	YYLIMIT  = YYCURSOR + len;
}

#define ini_filename SCNG(filename)

/* {{{ init_ini_scanner()
*/
static int init_ini_scanner(int scanner_mode, zend_file_handle *fh)
{
	/* Sanity check */
	if (scanner_mode != ZEND_INI_SCANNER_NORMAL && scanner_mode != ZEND_INI_SCANNER_RAW && scanner_mode != ZEND_INI_SCANNER_TYPED) {
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

	zend_stack_init(&SCNG(state_stack), sizeof(int));
	BEGIN(INITIAL);

	return SUCCESS;
}
/* }}} */

/* {{{ shutdown_ini_scanner()
*/
void shutdown_ini_scanner(void)
{
	zend_stack_destroy(&SCNG(state_stack));
	if (ini_filename) {
		free(ini_filename);
	}
}
/* }}} */

/* {{{ zend_ini_scanner_get_lineno()
*/
ZEND_COLD int zend_ini_scanner_get_lineno(void)
{
	return SCNG(lineno);
}
/* }}} */

/* {{{ zend_ini_scanner_get_filename()
*/
ZEND_COLD char *zend_ini_scanner_get_filename(void)
{
	return ini_filename ? ini_filename : "Unknown";
}
/* }}} */

/* {{{ zend_ini_open_file_for_scanning()
*/
int zend_ini_open_file_for_scanning(zend_file_handle *fh, int scanner_mode)
{
	char *buf;
	size_t size;

	if (zend_stream_fixup(fh, &buf, &size) == FAILURE) {
		return FAILURE;
	}

	if (init_ini_scanner(scanner_mode, fh) == FAILURE) {
		zend_file_handle_dtor(fh);
		return FAILURE;
	}

	yy_scan_buffer(buf, (unsigned int)size);

	return SUCCESS;
}
/* }}} */

/* {{{ zend_ini_prepare_string_for_scanning()
*/
int zend_ini_prepare_string_for_scanning(char *str, int scanner_mode)
{
	int len = (int)strlen(str);

	if (init_ini_scanner(scanner_mode, NULL) == FAILURE) {
		return FAILURE;
	}

	yy_scan_buffer(str, len);

	return SUCCESS;
}
/* }}} */

/* {{{ zend_ini_escape_string()
 */
static void zend_ini_escape_string(zval *lval, char *str, int len, char quote_type)
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

int ini_lex(zval *ini_lval)
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
		yy_push_state(ST_SECTION_RAW);
	} else {
		yy_push_state(ST_SECTION_VALUE);
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
	yy_push_state(ST_OFFSET);

	RETURN_TOKEN(TC_OFFSET, yytext, yyleng);
}

<ST_OFFSET>{TABS_AND_SPACES}*"]" { /* End of section or an option offset */
	BEGIN(INITIAL);
	return ']';
}

<ST_DOUBLE_QUOTES,ST_SECTION_VALUE,ST_VALUE,ST_OFFSET>{DOLLAR_CURLY} { /* Variable start */
	yy_push_state(ST_VARNAME);
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
	yy_pop_state();
	return '}';
}

<INITIAL,ST_VALUE>("true"|"on"|"yes"){TABS_AND_SPACES}* { /* TRUE value (when used outside option value/offset this causes parse error!) */
	RETURN_TOKEN(BOOL_TRUE, "1", 1);
}

<INITIAL,ST_VALUE>("false"|"off"|"no"|"none"){TABS_AND_SPACES}* { /* FALSE value (when used outside option value/offset this causes parse error!)*/
	RETURN_TOKEN(BOOL_FALSE, "", 0);
}

<INITIAL,ST_VALUE>("null"){TABS_AND_SPACES}* {
	RETURN_TOKEN(NULL_NULL, "", 0);
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
		yy_push_state(ST_RAW);
	} else {
		yy_push_state(ST_VALUE);
	}
	return '=';
}

<ST_RAW>{RAW_VALUE_CHARS} { /* Raw value, only used when SCNG(scanner_mode) == ZEND_INI_SCANNER_RAW. */
	unsigned char *sc = NULL;
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
	if (yyleng > 1 && yytext[0] == '"' && yytext[yyleng - 1] == '"') {
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
	yy_push_state(ST_DOUBLE_QUOTES);
	return '"';
}

<ST_DOUBLE_QUOTES>["]{TABS_AND_SPACES}* { /* Double quoted '"' string ends */
	yy_pop_state();
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

	zend_ini_escape_string(ini_lval, yytext, yyleng, '"');
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

<ST_VALUE,ST_RAW>[^] { /* End of option value (if EOF is reached before EOL */
	BEGIN(INITIAL);
	return END_OF_LINE;
}

<*>[^] {
	return 0;
}

*/
}
