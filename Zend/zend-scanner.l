%{

/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

%}

%x IN_SCRIPTING
%x DOUBLE_QUOTES
%x SINGLE_QUOTE
%x BACKQUOTE
%x HEREDOC
%x LOOKING_FOR_PROPERTY
%option stack

%{

#if WIN32|WINNT
#include <winsock.h>
#endif

#include <errno.h>

#include "zend.h"
#include "zend_alloc.h"
#include "zend_compile.h"
#include "zend-scanner.h"
#include "zend_highlight.h"
#include "zend_constants.h"
#include "zend_variables.h"
#include "zend_operators.h"

#if HAVE_STDARG_H
#include <stdarg.h>
#endif

#define YY_DECL int lex_scan(zval *zendlval CLS_DC)
#define ECHO { ZEND_WRITE( yytext, yyleng ); }

#ifdef __cplusplus
#  define MY_INPUT yyinput
#else
#  define MY_INPUT input
#endif


#ifndef YY_TLS_VARS
/* NOTE, YY_TLS_VARS CANNOT have a semicolon after in
   in the code or it will break compilation in msvc5 */
#define YY_TLS_VARS
#endif

#ifndef TLS_VARS /* just to make sure ;) */
#define TLS_VARS
#endif

#define HANDLE_NEWLINES(s,l) \
do { \
	char *p = (s),*boundary = p+(l); \
\
	while(p<boundary) { \
		if (*p++=='\n') { \
			CG(zend_lineno)++; \
		} \
	} \
} while(0)

#define HANDLE_NEWLINE(c) \
{ \
	if (c=='\n') { \
		CG(zend_lineno)++; \
	} \
}
		

void startup_scanner(CLS_D)
{
	CG(heredoc) = NULL;
	CG(heredoc_len)=0;
}


void shutdown_scanner(CLS_D)
{
	if (CG(heredoc)) {
		efree(CG(heredoc));
		CG(heredoc_len)=0;
	}
}


void reset_scanner(CLS_D)
{
	YY_TLS_VARS

	BEGIN(INITIAL);
	CG(zend_lineno)=1;
}


static inline void save_lexical_state(zend_lex_state *lex_state CLS_DC)
{
	memcpy(&lex_state->buffer_state,&YY_CURRENT_BUFFER,sizeof(YY_BUFFER_STATE));
	lex_state->in = yyin;
	lex_state->lineno = CG(zend_lineno);
	lex_state->state = YYSTATE;
	lex_state->filename = zend_get_compiled_filename();
}


static inline void restore_lexical_state(zend_lex_state *lex_state CLS_DC)
{
	YY_BUFFER_STATE original_buffer_state = YY_CURRENT_BUFFER;

	if (lex_state->buffer_state) {
		yy_switch_to_buffer(lex_state->buffer_state);
	} else {
		YY_CURRENT_BUFFER = NULL;
	}
		
	yy_delete_buffer(original_buffer_state);
	yyin = lex_state->in;
	CG(zend_lineno) = lex_state->lineno;
	BEGIN(lex_state->state);
	zend_restore_compiled_filename(lex_state->filename);
}


inline int open_file_for_scanning(zend_file_handle *file_handle CLS_DC)
{
	FILE *tmp;
	YY_BUFFER_STATE buffer_state = YY_CURRENT_BUFFER;

	switch (file_handle->type) {
		case ZEND_HANDLE_FILENAME:
			tmp = zend_fopen(file_handle->filename);
			break;
		case ZEND_HANDLE_FD:
			tmp = fdopen(file_handle->handle.fd, "r");
			break;
		case ZEND_HANDLE_FP:
			tmp = file_handle->handle.fp;
			break;
	}
	if (!tmp) {
		return FAILURE;
	}
	
	/* Reset the scanner for scanning the new file */
	yyin = tmp;
	yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
	CG(zend_lineno) = 1;
	BEGIN(INITIAL);
	
	zend_set_compiled_filename(file_handle->filename);
	return SUCCESS;
}


zend_op_array *compile_files(int mark_as_ref CLS_DC, int file_count, ...)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	zend_op_array *original_active_op_array = CG(active_op_array);
	zend_op_array *retval=NULL;
	zend_file_handle *file_handle;
	va_list files;
	int i;

	va_start(files, file_count);

	init_op_array(op_array, INITIAL_OP_ARRAY_SIZE);
	save_lexical_state(&original_lex_state CLS_CC);

	for (i=0; i<file_count; i++) {
		file_handle = va_arg(files, zend_file_handle *);
		if (!file_handle) {
			continue;
		}
		if (open_file_for_scanning(file_handle CLS_CC)==FAILURE) {
			zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
			destroy_op_array(op_array);
			efree(op_array);
			retval = NULL;
			break;
		} else {
			CG(active_op_array) = op_array;
			if (zendparse(CLS_C)==1) {
				retval = NULL;
				break;
			} else {
				fclose(yyin);
				restore_lexical_state(&original_lex_state CLS_CC);
				CG(active_op_array) = original_active_op_array;
				retval = op_array;
			}
		}
	}
	if (retval) {
		pass_two(op_array);
		if (mark_as_ref) {
			pass_include_eval(op_array);
		}
	}
	va_end(files);
	return retval;
}


zend_op_array *compile_filename(zval *filename CLS_DC)
{
	zend_file_handle file_handle;
	zval tmp;
	zend_op_array *retval;

	if (filename->type != IS_STRING) {
		tmp = *filename;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		filename = &tmp;
	}
	file_handle.filename = filename->value.str.val;
	file_handle.type = ZEND_HANDLE_FILENAME;
	retval = compile_files(0 CLS_CC, 1, &file_handle);
	if (filename==&tmp) {
		zval_dtor(&tmp);
	}
	return retval;
}


static inline int prepare_string_for_scanning(zval *str)
{
	/* enforce two trailing NULLs for flex... */
	str->value.str.val = (char *) erealloc(str->value.str.val,str->value.str.len+2);
	str->value.str.val[str->value.str.len+1]=0;

	yyin=NULL;
	yy_scan_buffer(str->value.str.val, str->value.str.len+2);
	return SUCCESS;
}


zend_op_array *compile_string(zval *source_string CLS_DC)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	zend_op_array *original_active_op_array = CG(active_op_array);
	zend_op_array *retval;
	zval tmp;

	tmp = *source_string;
	zval_copy_ctor(&tmp);
	convert_to_string(&tmp);
	source_string = &tmp;

	init_op_array(op_array, INITIAL_OP_ARRAY_SIZE);
	save_lexical_state(&original_lex_state CLS_CC);
	if (prepare_string_for_scanning(source_string)==FAILURE) {
		destroy_op_array(op_array);
		efree(op_array);
		retval = NULL;
	} else {
		CG(active_op_array) = op_array;
		BEGIN(IN_SCRIPTING);
		if (zendparse(CLS_C)==1) {
			retval = NULL;
		} else {
			pass_two(op_array);
			pass_include_eval(op_array);
			restore_lexical_state(&original_lex_state CLS_CC);
			CG(active_op_array) = original_active_op_array;
			retval = op_array;
		}
	}

	zval_dtor(&tmp);

	return retval;
}


int require_filename(char *filename CLS_DC)
{
	zend_file_handle file_handle;

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = filename;
	return require_file(&file_handle CLS_CC);
}


int require_file(zend_file_handle *file_handle CLS_DC)
{
	zend_lex_state original_lex_state;

	save_lexical_state(&original_lex_state CLS_CC);
	if (open_file_for_scanning(file_handle CLS_CC)==FAILURE) {
		zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
		return FAILURE;
	}
	zendparse(CLS_C);
	fclose(yyin);
	restore_lexical_state(&original_lex_state CLS_CC);
	return SUCCESS;
}

int highlight_file(char *filename, zend_syntax_highlighter_ini *syntax_highlighter_ini)
{
	zend_lex_state original_lex_state;
	zend_file_handle file_handle;
	CLS_FETCH();

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = filename;
	save_lexical_state(&original_lex_state CLS_CC);
	if (open_file_for_scanning(&file_handle CLS_CC)==FAILURE) {
		zend_message_dispatcher(ZMSG_FAILED_HIGHLIGHT_FOPEN, filename);
		return FAILURE;
	}
	zend_highlight(syntax_highlighter_ini);
	fclose(yyin);
	restore_lexical_state(&original_lex_state CLS_CC);
	return SUCCESS;
}


int highlight_string(zval *str, zend_syntax_highlighter_ini *syntax_highlighter_ini)
{
	zend_lex_state original_lex_state;
	zval tmp = *str;
	CLS_FETCH();

	str = &tmp;
	zval_copy_ctor(str);
	save_lexical_state(&original_lex_state CLS_CC);
	if (prepare_string_for_scanning(str)==FAILURE) {
		return FAILURE;
	}
	zend_highlight(syntax_highlighter_ini);
	restore_lexical_state(&original_lex_state CLS_CC);
	zval_dtor(str);
	return SUCCESS;
}


/* redefine YY_INPUT to handle urls for win32*/
#if 0 /*WIN32|WINNT*/
#define YY_INPUT(buf,result,max_size) \
	if ( yyin->_tmpfname != "url" ){ \
	if ( yy_current_buffer->yy_is_interactive ) \
		{ \
		int c = '*', n; \
		for ( n = 0; n < max_size && \
			     (c = getc( yyin )) != EOF && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		if ( c == EOF && ferror( yyin ) ) \
			YY_FATAL_ERROR( "input in flex scanner failed" ); \
		result = n; \
		} \
	else if ( ((result = fread( buf, 1, max_size, yyin )) == 0) \
		  && ferror( yyin ) ) \
		YY_FATAL_ERROR( "input in flex scanner failed" ); \
	}else{ /* this is a url */ \
	int recv_char=0,socketd=0; \
	/*memset(buf,0,max_size);*/ \
	socketd=yyin->_file; \
	if ( yy_current_buffer->yy_is_interactive ) \
		{ \
		int c = '*', n; \
		for ( n = 0; n < max_size && \
			     ( (recv_char=recv( socketd,(char *)&c,1,0 ))) >0 && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		if ( recv_char == SOCKET_ERROR ) \
			YY_FATAL_ERROR( "input from url in flex scanner failed" ); \
		result = n; \
		} \
	else if ((result = recv( socketd, (char *)buf, max_size, 0)) == SOCKET_ERROR) \
		  	YY_FATAL_ERROR( "input from url read in flex scanner failed" ); \
	}
#endif

%}

LNUM	[0-9]+
DNUM	([0-9]*[\.][0-9]+)|([0-9]+[\.][0-9]*)
EXPONENT_DNUM	(({LNUM}|{DNUM})[eE][+-]?{LNUM})
HNUM	"0x"[0-9a-fA-F]+
LABEL	[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*
WHITESPACE [ \n\r\t]+
TABS_AND_SPACES [ \t]*
TOKENS [;:,.\[\]()|^&+-/*=%!~$<>?@]
ENCAPSED_TOKENS [\[\]{}$]
ESCAPED_AND_WHITESPACE [\n\t\r #'.:;,()|^&+-/*=%!~<>?@]+

%option noyylineno
%option noyywrap
%%
%{
TLS_VARS;
%}

<IN_SCRIPTING>"exit" {
	return T_EXIT;
}

<IN_SCRIPTING>"die" {
	return T_EXIT;
}

<IN_SCRIPTING>"old_function" {
	return OLD_FUNCTION;
}

<IN_SCRIPTING>"function"|"cfunction" {
	return FUNCTION;
}

<IN_SCRIPTING>"const" {
	return ZEND_CONST;
}

<IN_SCRIPTING>"return" {
	return RETURN;
}

<IN_SCRIPTING>"if" {
	return IF;
}

<IN_SCRIPTING>"elseif" {
	return ELSEIF;
}

<IN_SCRIPTING>"endif" {
	return ENDIF;
}

<IN_SCRIPTING>"else" {
	return ELSE;
}

<IN_SCRIPTING>"while" {
	return WHILE;
}

<IN_SCRIPTING>"endwhile" {
	return ENDWHILE;
}

<IN_SCRIPTING>"do" {
	return DO;
}

<IN_SCRIPTING>"for" {
	return FOR;
}

<IN_SCRIPTING>"endfor" {
	return ENDFOR;
}

<IN_SCRIPTING>"foreach" {
	return ZEND_FOREACH;
}

<IN_SCRIPTING>"endforeach" {
	return T_ENDFOREACH;
}

<IN_SCRIPTING>"as" {
	return ZEND_AS;
}

<IN_SCRIPTING>"switch" {
	return SWITCH;
}

<IN_SCRIPTING>"endswitch" {
	return ENDSWITCH;
}

<IN_SCRIPTING>"case" {
	return CASE;
}

<IN_SCRIPTING>"default" {
	return DEFAULT;
}

<IN_SCRIPTING>"break" {
	return BREAK;
}

<IN_SCRIPTING>"continue" {
	return CONTINUE;
}

<IN_SCRIPTING>"echo" {
	return ZEND_ECHO;
}

<IN_SCRIPTING>"print" {
	return ZEND_PRINT;
}

<IN_SCRIPTING>"class" {
	return CLASS;
}

<IN_SCRIPTING>"extends" {
	return EXTENDS;
}

<IN_SCRIPTING,DOUBLE_QUOTES,BACKQUOTE,HEREDOC>"->" {
	yy_push_state(LOOKING_FOR_PROPERTY);
	return ZEND_OBJECT_OPERATOR;
}

<LOOKING_FOR_PROPERTY>{LABEL} {
	yy_pop_state();
	zendlval->value.str.val = (char *)estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return STRING;
}

<LOOKING_FOR_PROPERTY>. {
	unput(yytext[0]);
	yy_pop_state();
}

<IN_SCRIPTING>"::" {
	return T_PAAMAYIM_NEKUDOTAYIM;
}

<IN_SCRIPTING>"new" {
	return NEW;
}

<IN_SCRIPTING>"var" {
	return VAR;
}

<IN_SCRIPTING>"("{TABS_AND_SPACES}("int"|"integer"){TABS_AND_SPACES}")" {
	return INT_CAST;
}

<IN_SCRIPTING>"("{TABS_AND_SPACES}("real"|"double"|"float"){TABS_AND_SPACES}")" {
	return DOUBLE_CAST;
}

<IN_SCRIPTING>"("{TABS_AND_SPACES}"string"{TABS_AND_SPACES}")" {
	return STRING_CAST;
}

<IN_SCRIPTING>"("{TABS_AND_SPACES}"array"{TABS_AND_SPACES}")" {
	return ARRAY_CAST;
}

<IN_SCRIPTING>"("{TABS_AND_SPACES}"object"{TABS_AND_SPACES}")" {
	return OBJECT_CAST;
}

<IN_SCRIPTING>"eval" {
	return EVAL;
}

<IN_SCRIPTING>"include" {
	return INCLUDE;
}

<IN_SCRIPTING>"require" {
	return REQUIRE;
}

<IN_SCRIPTING>"global" {
	return ZEND_GLOBAL;
}

<IN_SCRIPTING>"isset" {
	return T_ISSET;
}

<IN_SCRIPTING>"empty" {
	return T_EMPTY;
}

<IN_SCRIPTING>"static" {
	return ZEND_STATIC;
}

<IN_SCRIPTING>"unset" {
	return ZEND_UNSET;
}

<IN_SCRIPTING>"=>" {
	return ZEND_DOUBLE_ARROW;
}

<IN_SCRIPTING>"list" {
	return ZEND_LIST;
}

<IN_SCRIPTING>"array" {
	return ZEND_ARRAY;
}

<IN_SCRIPTING>"++" {
	return INCREMENT;
}

<IN_SCRIPTING>"--" {
	return DECREMENT;
}

<IN_SCRIPTING>"==" {
	return IS_EQUAL;
}

<IN_SCRIPTING>"!="|"<>" {
	return IS_NOT_EQUAL;
}

<IN_SCRIPTING>"<=" {
	return IS_SMALLER_OR_EQUAL;
}

<IN_SCRIPTING>">=" {
	return IS_GREATER_OR_EQUAL;
}

<IN_SCRIPTING>"+=" {
	return PLUS_EQUAL;
}

<IN_SCRIPTING>"-=" {
	return MINUS_EQUAL;
}

<IN_SCRIPTING>"*=" {
	return MUL_EQUAL;
}

<IN_SCRIPTING>"/=" {
	return DIV_EQUAL;
}

<IN_SCRIPTING>".=" {
	return CONCAT_EQUAL;
}


<IN_SCRIPTING>"%=" {
	return MOD_EQUAL;
}

<IN_SCRIPTING>"<<=" {
	return SHIFT_LEFT_EQUAL;
}

<IN_SCRIPTING>">>=" {
	return SHIFT_RIGHT_EQUAL;
}

<IN_SCRIPTING>"&=" {
	return AND_EQUAL;
}

<IN_SCRIPTING>"|=" {
	return OR_EQUAL;
}

<IN_SCRIPTING>"^=" {
	return XOR_EQUAL;
}

<IN_SCRIPTING>"||" {
	return BOOLEAN_OR;
}

<IN_SCRIPTING>"&&" {
	return BOOLEAN_AND;
}

<IN_SCRIPTING>"OR" {
	return LOGICAL_OR;
}

<IN_SCRIPTING>"AND" {
	return LOGICAL_AND;
}

<IN_SCRIPTING>"XOR" {
	return LOGICAL_XOR;
}

<IN_SCRIPTING>"<<" {
	return SHIFT_LEFT;
}

<IN_SCRIPTING>">>" {
	return SHIFT_RIGHT;
}

<IN_SCRIPTING>{TOKENS} {
	return yytext[0];
}


<IN_SCRIPTING>"{" {
	yy_push_state(IN_SCRIPTING);
	return '{';
}


<DOUBLE_QUOTES,BACKQUOTE,HEREDOC>"${" {
	yy_push_state(IN_SCRIPTING);
	return DOLLAR_OPEN_CURLY_BRACES;
}


<IN_SCRIPTING>"}" {
	yy_pop_state();
	return '}';
}



<IN_SCRIPTING>{LNUM}|{HNUM} {
	errno = 0;
	zendlval->value.lval = strtol(yytext, NULL, 0);
	if (errno == ERANGE) { /* overflow */
		zendlval->value.dval = strtod(yytext,NULL);
		zendlval->type = IS_DOUBLE;
		return DNUMBER;
	} else {
		zendlval->type = IS_LONG;
		return LNUMBER;
	}
}

<DOUBLE_QUOTES,BACKQUOTE,HEREDOC>{LNUM}|{HNUM} { /* treat numbers (almost) as strings inside encapsulated strings */
	zendlval->value.str.val = (char *)estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return NUM_STRING;
}

<IN_SCRIPTING>{DNUM}|{EXPONENT_DNUM} {
	zendlval->value.dval = strtod(yytext,NULL);
	zendlval->type = IS_DOUBLE;
	return DNUMBER;
}

<IN_SCRIPTING>"__LINE__" {
	zendlval->value.lval = CG(zend_lineno);
	zendlval->type = IS_LONG;
	return ZEND_LINE;
}

<IN_SCRIPTING>"__FILE__" {
	char *filename = zend_get_compiled_filename();
	
	zendlval->value.str.len = strlen(filename);
	zendlval->value.str.val = estrndup(filename,zendlval->value.str.len);
	zendlval->type = IS_STRING;
	return ZEND_FILE;
}


<INITIAL>(([^<]|"<"[^?%s<]){1,400})|"<s"|"<" {
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINES(yytext,yyleng);
	return INLINE_HTML;
}

<INITIAL>"<?"|"<script"{WHITESPACE}+"language"{WHITESPACE}*"="{WHITESPACE}*("php"|"\"php\""|"\'php\'"){WHITESPACE}*">" {
	HANDLE_NEWLINES(yytext,yyleng);
	if (CG(short_tags) || yyleng>2) { /* yyleng>2 means it's not <? but <script> */
		zendlval->value.str.val = yytext; /* no copying - intentional */
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		BEGIN(IN_SCRIPTING);
		return PHP_OPEN_TAG;
	} else {
		zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		return INLINE_HTML;
	}
}


<INITIAL>"<%=" {
	HANDLE_NEWLINES(yytext,yyleng);
	if (CG(asp_tags)) {
		zendlval->value.str.val = yytext; /* no copying - intentional */
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		BEGIN(IN_SCRIPTING);
		return PHP_OPEN_TAG_WITH_ECHO;
	} else {
		zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		return INLINE_HTML;
	}
}


<INITIAL>"<%" {
	HANDLE_NEWLINES(yytext,yyleng);
	if (CG(asp_tags)) {
		zendlval->value.str.val = yytext; /* no copying - intentional */
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		BEGIN(IN_SCRIPTING);
	} else {
		zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		return INLINE_HTML;
	}
}


<INITIAL>"<?php"[ \n\r\t] {
	zendlval->value.str.val = yytext; /* no copying - intentional */
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINE(yytext[yyleng-1]);
	BEGIN(IN_SCRIPTING);
	return PHP_OPEN_TAG;
}


<INITIAL>"<?php_track_vars?>"([\n]|"\r\n")? {
	zend_message_dispatcher(ZMSG_ENABLE_TRACK_VARS, NULL);
	HANDLE_NEWLINE(yytext[yyleng-1]);
	return T_PHP_TRACK_VARS;
}

<IN_SCRIPTING,DOUBLE_QUOTES,HEREDOC>"$"{LABEL} {
	zendlval->value.str.val = (char *)estrndup(yytext+1, yyleng-1);
	zendlval->value.str.len = yyleng-1;
	zendlval->type = IS_STRING;
	return VARIABLE;
}


<IN_SCRIPTING>{LABEL} {
	zendlval->value.str.val = (char *)estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return STRING;
}


<DOUBLE_QUOTES,BACKQUOTE,HEREDOC>{LABEL} {
	zendlval->value.str.val = (char *)estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return STRING;
}


<IN_SCRIPTING>{WHITESPACE} {
	zendlval->value.str.val = yytext; /* no copying - intentional */
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINES(yytext,yyleng);
	return T_WHITESPACE;
}


<IN_SCRIPTING>([#]|"//")([^\n\r?]|"?"[^>\n\r])*("?\n"|"?\r\n")? { /* eat one line comments */
	HANDLE_NEWLINE(yytext[yyleng-1]);
	return ZEND_COMMENT;
}

<IN_SCRIPTING>"/*"([^*]|"*"[^/])*(("*/")?) {
	HANDLE_NEWLINES(yytext, yyleng);
	return ZEND_COMMENT;
}


<IN_SCRIPTING>("?>"|"</script"{WHITESPACE}*">")([\n]|"\r\n")? {
	zendlval->value.str.val = yytext; /* no copying - intentional */
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINES(yytext, yyleng);
	BEGIN(INITIAL);
	return PHP_CLOSE_TAG;  /* implicit ';' at php-end tag */
}


<IN_SCRIPTING>"%>"([\n]|"\r\n")? {
	zendlval->value.str.val = yytext; /* no copying - intentional */
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINES(yytext,yyleng);
	if (CG(asp_tags)) {
		BEGIN(INITIAL);
		return PHP_CLOSE_TAG;  /* implicit ';' at php-end tag */
	} else {
		return INLINE_HTML;
	}
}


<IN_SCRIPTING>(["]([^"$\\]|([\\]+[^\\$]))*["])|([']([^'\\]|([\\]+['\\]))*['])  {
	register char *s, *t;
	char *end;
	
	zendlval->value.str.val = estrndup(yytext+1, yyleng-2);
	zendlval->value.str.len = yyleng-2;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINES(yytext,yyleng);
	
	/* convert escape sequences */
	s = t = zendlval->value.str.val;
	end = s+zendlval->value.str.len;
	while (s<end) {
		if (*s=='\\') {
			s++;
			if (s>=end) {
				continue;
			}
			switch(*s) {
				case 'n':
					*t++ = '\n';
					zendlval->value.str.len--;
					break;
				case 'r':
					*t++ = '\r';
					zendlval->value.str.len--;
					break;
				case 't':
					*t++ = '\t';
					zendlval->value.str.len--;
					break;
				case '\\':
					*t++ = '\\';
					zendlval->value.str.len--;
					break;
				case '$':
				case '"':
					*t++ = *s;
					zendlval->value.str.len--;
					break;
				default:
					*t++ = '\\';
					*t++ = *s;
					break;
			}
			s++;
		} else {
			*t++ = *s++;
		}
	}
	*t = 0;
	
	return CONSTANT_ENCAPSED_STRING;
}


<IN_SCRIPTING>["] {
	BEGIN(DOUBLE_QUOTES);
	return '\"';
}


<IN_SCRIPTING>"<<"{LABEL}("\r")?"\n" {
	CG(heredoc_len) = yyleng-2-1-(yytext[yyleng-2]=='\r'?1:0);
	CG(heredoc) = estrndup(yytext+2, CG(heredoc_len));
	BEGIN(HEREDOC);
	return ZEND_HEREDOC;
}
	

<IN_SCRIPTING>[`] {
	BEGIN(BACKQUOTE);
	return '`';
}


<IN_SCRIPTING>['] {
	BEGIN(SINGLE_QUOTE);
	return '\'';
}


<HEREDOC>^{LABEL}("\r")?"\n" {
	int label_len;

	if (yytext[yyleng-2]=='\r') {
		label_len = yyleng-2;
	} else {
		label_len = yyleng-1;
	}

	if (label_len==CG(heredoc_len) && !memcmp(yytext, CG(heredoc), label_len)) {
		efree(CG(heredoc));
		CG(heredoc)=NULL;
		CG(heredoc_len)=0;
		BEGIN(IN_SCRIPTING);
		return ZEND_HEREDOC;
	} else {
		zendlval->value.str.val = (char *)estrndup(yytext, yyleng);
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		return STRING;
	}
}


<DOUBLE_QUOTES,BACKQUOTE,HEREDOC>{ESCAPED_AND_WHITESPACE} {
	HANDLE_NEWLINES(yytext,yyleng);
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}

<SINGLE_QUOTE>([^'\\]|\\[^'\\])+ {
	HANDLE_NEWLINES(yytext,yyleng);
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}


<DOUBLE_QUOTES>[`]+ {
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}


<BACKQUOTE>["]+ {
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}


<DOUBLE_QUOTES,BACKQUOTE,HEREDOC>"$"[^a-zA-Z_\x7f-\xff{] {
	zendlval->value.chval = yytext[0];
	if (yyleng == 2) {
		unput(yytext[1]);
		yytext[1] = 0;
		yyleng--;
	}
	return CHARACTER;	
}


<DOUBLE_QUOTES,BACKQUOTE,HEREDOC>{ENCAPSED_TOKENS} {
	zendlval->value.chval = yytext[0];
	return yytext[0];
}

<DOUBLE_QUOTES,BACKQUOTE,HEREDOC>"{$" {
	zendlval->value.chval = yytext[0];
	yy_push_state(IN_SCRIPTING);
	unput('$');
	return T_CURLY_OPEN;
}

<SINGLE_QUOTE>"\\'" {
	zendlval->value.chval='\'';
	return CHARACTER;
}

<SINGLE_QUOTE>"\\\\" {
	zendlval->value.chval='\\';
	return CHARACTER;
}

<DOUBLE_QUOTES>"\\\"" {
	zendlval->value.chval='"';
	return CHARACTER;
}

<BACKQUOTE>"\\`" {
	zendlval->value.chval='`';
	return CHARACTER;
}

<DOUBLE_QUOTES,BACKQUOTE,HEREDOC>"\\"[0-7]{1,3} {
	zendlval->value.chval = (char) strtol (yytext+1, NULL, 8);
	return CHARACTER;
}

<DOUBLE_QUOTES,BACKQUOTE,HEREDOC>"\\x"[0-9A-Fa-f]{1,2} {
	zendlval->value.chval = (char) strtol (yytext+2, NULL, 16);
	return CHARACTER;
}

<DOUBLE_QUOTES,BACKQUOTE>"\\". {
	switch (yytext[1]) {
		case 'n':
			zendlval->value.chval='\n';
			break;
		case 't':
			zendlval->value.chval='\t';
			break;
		case 'r':
			zendlval->value.chval='\r';
			break;
		case '\\':
			zendlval->value.chval='\\';
			break;
		case '$':
			zendlval->value.chval=yytext[1];
			break;
		case '{':
			zendlval->value.chval=yytext[1];
			break;
		default:
			zendlval->value.str.val = estrndup(yytext,yyleng);
			zendlval->value.str.len = yyleng;
			zendlval->type = IS_STRING;
			return BAD_CHARACTER;
			break;
	}
	return CHARACTER;
}


<HEREDOC>"\\$"|"\\{" {
	zendlval->value.chval = yytext[1];
	return CHARACTER;
}


<HEREDOC>["'`]+ {
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}


<DOUBLE_QUOTES>["] {
	BEGIN(IN_SCRIPTING);
	return '\"';
}


<BACKQUOTE>[`] {
	BEGIN(IN_SCRIPTING);
	return '`';
}


<SINGLE_QUOTE>['] {
	BEGIN(IN_SCRIPTING);
	return '\'';
}


<DOUBLE_QUOTES,BACKQUOTE,INITIAL,IN_SCRIPTING,LOOKING_FOR_PROPERTY><<EOF>> {
	return 0;
}




<IN_SCRIPTING,INITIAL,DOUBLE_QUOTES,BACKQUOTE,SINGLE_QUOTE>. {
	zend_error(E_COMPILE_WARNING,"Unexpected character in input:  '%c' (ASCII=%d) state=%d",yytext[0],yytext[0],YYSTATE);
}
