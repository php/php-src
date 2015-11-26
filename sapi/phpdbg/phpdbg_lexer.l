/*
 * phpdbg_lexer.l
 */

#include "phpdbg.h"
#include "phpdbg_cmd.h"

#include "phpdbg_parser.h"

#define LEX(v) (PHPDBG_G(lexer).v)

#define YYCTYPE unsigned char
#define YYSETCONDITION(x) LEX(state) = x;
#define YYGETCONDITION() LEX(state)
#define YYCURSOR LEX(cursor)
#define YYMARKER LEX(marker)
#define YYCTXMARKER LEX(ctxmarker)
#define yyleng LEX(len)
#define yytext ((char*) LEX(text))
#undef YYDEBUG
#define YYDEBUG(a, b)
#define YYFILL(n)

#define NORMAL 0
#define PRE_RAW 1
#define RAW 2
#define INITIAL 3

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

void phpdbg_init_lexer (phpdbg_param_t *stack, char *input) {
	PHPDBG_G(parser_stack) = stack;

	YYSETCONDITION(INITIAL);

	LEX(text) = YYCURSOR = (unsigned char *) input;
	LEX(len) = strlen(input);
}

static int unescape_string(char *s) {
	switch (*s) {
		case '\'':
		case '\"': {
			char start = *s;
			size_t off = 1;
			do {
				if (s[off] == '\\') {
					off++;
				}
				*s = s[off];
			} while ((++s)[off] != start);
			return off + 1;
		}
	}

	return 0;
}

int phpdbg_lex (phpdbg_param_t* yylval) {

restart:
	LEX(text) = YYCURSOR;

/*!re2c
re2c:yyfill:check = 0;
T_TRUE      'true'
T_YES       'yes'
T_ON        'on'
T_ENABLED   'enabled'
T_FALSE     'false'
T_NO        'no'
T_OFF       'off'
T_DISABLED  'disabled'
T_EVAL      'ev'
T_SHELL     'sh'
T_IF        'if'
T_RUN       'run'
T_RUN_SHORT "r"
WS          [ \r\t]+
DIGITS      [-]?[0-9\.]+
ID          [^ \r\n\t:#\000]+
GENERIC_ID  ([^ \r\n\t:#\000"']|":\\")+|["]([^\n\000"\\]|"\\\\"|"\\"["])+["]|[']([^\n\000'\\]|"\\\\"|"\\"['])+[']
ADDR        [0][x][a-fA-F0-9]+
OPCODE      (ZEND_|zend_)([A-Za-z])+
INPUT       ([^\n\000#"']|["]([^\n\000"\\]|"\\\\"|"\\"["])+["]|[']([^\n\000'\\]|"\\\\"|"\\"['])+['])+

<!*> := yyleng = (size_t) YYCURSOR - (size_t) yytext;

<*>[\n\000] {
	return 0;
}

<PRE_RAW, NORMAL>"-r"{WS}?{DIGITS} {
	char *text = yytext + 2;
	while (*++text < '0');
	yylval->num = atoi(text);
	return T_REQ_ID;
}

<NORMAL>{T_IF}{WS} {
	YYSETCONDITION(RAW);
	phpdbg_init_param(yylval, EMPTY_PARAM);
	return T_IF;
}

<NORMAL>"#"/{DIGITS} {
	return T_POUND;
}

<*>"#" {
	YYSETCONDITION(INITIAL);
	return T_SEPARATOR;
}

<NORMAL>"::" {
	return T_DCOLON;
}

<NORMAL>":"/[^\\] {
	return T_COLON;
}

<NORMAL>{ID}"://" {
	phpdbg_init_param(yylval, STR_PARAM);
	yylval->str = estrndup(yytext, yyleng);
	yylval->len = yyleng;
	return T_PROTO;
}

<NORMAL>({T_YES}|{T_ON}|{T_ENABLED}|{T_TRUE}){WS} {
	phpdbg_init_param(yylval, NUMERIC_PARAM);
	yylval->num = 1;
	return T_TRUTHY;
}

<NORMAL>({T_NO}|{T_OFF}|{T_DISABLED}|{T_FALSE}){WS} {
	phpdbg_init_param(yylval, NUMERIC_PARAM);
	yylval->num = 0;
	return T_FALSY;
}

<NORMAL>{DIGITS} {
	phpdbg_init_param(yylval, NUMERIC_PARAM);
	yylval->num = atoi(yytext);
	return T_DIGITS;
}

<NORMAL>{ADDR} {
	phpdbg_init_param(yylval, ADDR_PARAM);
	yylval->addr = strtoul(yytext, 0, 16);
	return T_ADDR;
}

<NORMAL>{OPCODE} {
	phpdbg_init_param(yylval, OP_PARAM);
	yylval->str = estrndup(yytext, yyleng);
	yylval->len = yyleng;
	return T_OPCODE;
}

<NORMAL>{GENERIC_ID} {
	phpdbg_init_param(yylval, STR_PARAM);
	yylval->str = estrndup(yytext, yyleng - unescape_string(yytext));
	yylval->len = yyleng;
	return T_ID;
}

<RAW>{INPUT} {
	phpdbg_init_param(yylval, STR_PARAM);
	yylval->str = estrndup(yytext, yyleng - unescape_string(yytext));
	yylval->len = yyleng;
	return T_INPUT;
}

<*>{WS} {
	/* ignore whitespace */

	goto restart;
}

<INITIAL>{T_EVAL}{WS} {
	YYSETCONDITION(PRE_RAW);
	phpdbg_init_param(yylval, EMPTY_PARAM);
	return T_EVAL;
}

<INITIAL>{T_SHELL}{WS} {
	YYSETCONDITION(PRE_RAW);
	phpdbg_init_param(yylval, EMPTY_PARAM);
	return T_SHELL;
}

<INITIAL>({T_RUN}|{T_RUN_SHORT}){WS} {
	YYSETCONDITION(PRE_RAW);
	phpdbg_init_param(yylval, EMPTY_PARAM);
	return T_RUN;
}

<PRE_RAW>[^ ] {
	YYSETCONDITION(RAW);

	YYCURSOR = LEX(text);
	goto restart;
}

<INITIAL>[^ ] {
	YYSETCONDITION(NORMAL);

	YYCURSOR = LEX(text);
	goto restart;
}

*/
}
