%{

/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

%}

%x ST_IN_SCRIPTING
%x ST_DOUBLE_QUOTES
%x ST_SINGLE_QUOTE
%x ST_BACKQUOTE
%x ST_HEREDOC
%x ST_LOOKING_FOR_PROPERTY
%x ST_LOOKING_FOR_VARNAME
%x ST_COMMENT
%option stack

%{

#if WIN32|WINNT
#include <winsock.h>
#include <io.h>
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

#ifdef ZTS
# include <fstream.h>
# ifdef HAVE_STDIOSTR_H
#  include <stdiostr.h>
# endif
# ifdef HAVE_STDIOSTREAM_H
#  include <stdiostream.h>
# endif
# if WIN32||WINNT
#  include <strstrea.h>
# else
#  include <strstream.h>
# endif
#endif

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef ZTS
#define YY_DECL int ZendFlexLexer::lex_scan(zval *zendlval CLS_DC)
#else
#define YY_DECL int lex_scan(zval *zendlval CLS_DC)
#endif
#define ECHO { ZEND_WRITE( yytext, yyleng ); }

#ifdef ZTS
#  define MY_INPUT yyinput
#else
#  define MY_INPUT input
#endif

#if defined(ZTS) && !defined(HAVE_CLASS_ISTDIOSTREAM)
class istdiostream : public istream
{
private:
  stdiobuf _file;
public:
  istdiostream (FILE* __f) : istream(), _file(__f) { init(&_file); }
};
#endif

#define YY_FATAL_ERROR zend_fatal_scanner_error

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
		

#define ZEND_IS_OCT(c)  ((c)>='0' && (c)<'8')
#define ZEND_IS_HEX(c)  (((c)>='0' && (c)<='9') || ((c)>='a' && (c)<='f') || ((c)>='A' && (c)<='F'))


void zend_fatal_scanner_error(char *message)
{
	zend_error(E_COMPILE_ERROR, message);
}

BEGIN_EXTERN_C()
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
END_EXTERN_C()


static inline void save_lexical_state(zend_lex_state *lex_state CLS_DC)
{
#ifndef ZTS
	memcpy(&lex_state->buffer_state,&YY_CURRENT_BUFFER,sizeof(YY_BUFFER_STATE));
	lex_state->in = yyin;
	lex_state->lineno = CG(zend_lineno);
	lex_state->state = YYSTATE;
	lex_state->filename = zend_get_compiled_filename();
#else
	lex_state->ZFL = CG(ZFL);
#endif
}


inline void restore_lexical_state(zend_lex_state *lex_state CLS_DC)
{
#ifndef ZTS
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
#else
	delete((ZendFlexLexer *) CG(ZFL));
	CG(ZFL) = lex_state->ZFL;
#endif
}


BEGIN_EXTERN_C()


ZEND_API void zend_open_file_dtor(void *f)
{
#ifdef ZTS
	delete *((istream **) f);
#else
	fclose(*((FILE **) f));
#endif
}


ZEND_API void zend_close_file_handle(zend_file_handle *file_handle CLS_DC)
{
#ifdef ZTS
	zend_llist_del_element(&CG(open_files), &file_handle->handle.is);
#else
	zend_llist_del_element(&CG(open_files), &file_handle->handle.fp);
#endif
}


ZEND_API int open_file_for_scanning(zend_file_handle *file_handle CLS_DC)
{
#ifndef ZTS
	switch (file_handle->type) {
		case ZEND_HANDLE_FILENAME:
			file_handle->handle.fp = zend_fopen(file_handle->filename);
			break;
		case ZEND_HANDLE_FD:
			file_handle->handle.fp = fdopen(file_handle->handle.fd, "r");
			break;
		case ZEND_HANDLE_FP:
			file_handle->handle.fp = file_handle->handle.fp;
			break;
	}
	if (!file_handle->handle.fp) {
		return FAILURE;
	}
	if (file_handle->handle.fp != stdin) {
		zend_llist_add_element(&CG(open_files), &file_handle->handle.fp);
	}
	file_handle->type = ZEND_HANDLE_FP;	
	/* Reset the scanner for scanning the new file */
	yyin = file_handle->handle.fp;
	yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
	BEGIN(INITIAL);
#else
	switch (file_handle->type) {
		case ZEND_HANDLE_FD:
			file_handle->handle.is = new ifstream(file_handle->handle.fd);
			break;
		case ZEND_HANDLE_FILENAME:
			{
				file_handle->handle.fp = zend_fopen(file_handle->filename);
				if (!file_handle->handle.fp) {
					return FAILURE;
				}
				file_handle->handle.is = new istdiostream(file_handle->handle.fp);
				break;
			}
		case ZEND_HANDLE_FP:
			if (file_handle->handle.fp==stdin) {
				file_handle->handle.is = (ifstream *) &cin;
			} else {
				if (!file_handle->handle.fp) {
					return FAILURE;
				}
				file_handle->handle.is = new istdiostream(file_handle->handle.fp);
			}	
			break;
	}
	if (file_handle->handle.is->fail()) {
		delete file_handle->handle.is;
		return FAILURE;
	}
	if (file_handle->handle.is != &cin) {
		zend_llist_add_element(&CG(open_files), &file_handle->handle.is);
	}
	file_handle->type = ZEND_HANDLE_ISTREAM;

	CG(ZFL) = new ZendFlexLexer;
	CG(ZFL)->switch_streams(file_handle->handle.is, &cout);
#endif
	zend_set_compiled_filename(file_handle->filename);
	CG(zend_lineno) = 1;
	return SUCCESS;
}
END_EXTERN_C()


ZEND_API zend_op_array *compile_files(int mark_as_ref CLS_DC, int file_count, ...)
{
	va_list files;
	zend_op_array *op_array;

	va_start(files, file_count);
	op_array = v_compile_files(mark_as_ref CLS_CC, file_count, files);
	va_end(files);
	return op_array;
}


ZEND_API zend_op_array *v_compile_files(int mark_as_ref CLS_DC, int file_count, va_list files)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	zend_op_array *original_active_op_array = CG(active_op_array);
	zend_op_array *retval=NULL;
	zend_file_handle *file_handle;
	int i;
	int compiler_result;
	int compiled_files=0;

	init_op_array(op_array, INITIAL_OP_ARRAY_SIZE);
	save_lexical_state(&original_lex_state CLS_CC);

	retval = op_array; /* success oriented */
	for (i=0; i<file_count; i++) {
		file_handle = va_arg(files, zend_file_handle *);
		if (!file_handle) {
			continue;
		}
		if (open_file_for_scanning(file_handle CLS_CC)==FAILURE) {
			zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
		} else {
			CG(active_op_array) = op_array;
			compiler_result = zendparse(CLS_C);
			zend_close_file_handle(file_handle CLS_CC);
			restore_lexical_state(&original_lex_state CLS_CC);
			CG(active_op_array) = original_active_op_array;
			if (compiler_result==1) { /* parser error */
				CG(unclean_shutdown) = 1;
				retval = NULL;
				break;
			}
			compiled_files++;
		}
	}

	if (retval) {
		if (compiled_files>0) {
			pass_two(op_array);
			if (mark_as_ref) {
				pass_include_eval(op_array);
			}
		} else {
			destroy_op_array(op_array);
			efree(op_array);
			retval = NULL;
		}
	}
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
	retval = zend_compile_files(1 CLS_CC, 1, &file_handle);
	if (filename==&tmp) {
		zval_dtor(&tmp);
	}
	return retval;
}


static inline int prepare_string_for_scanning(zval *str CLS_DC)
{
#ifndef ZTS
	/* enforce two trailing NULLs for flex... */
	str->value.str.val = (char *) erealloc(str->value.str.val,str->value.str.len+2);

	str->value.str.val[str->value.str.len+1]=0;

	yyin=NULL;
	yy_scan_buffer(str->value.str.val, str->value.str.len+2);
#else
	istrstream *input_stream = new istrstream(str->value.str.val, str->value.str.len);
	CG(ZFL) = new ZendFlexLexer;

	CG(ZFL)->switch_streams(input_stream, &cout);
#endif
	return SUCCESS;
}


zend_op_array *compile_string(zval *source_string CLS_DC)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	zend_op_array *original_active_op_array = CG(active_op_array);
	zend_op_array *retval;
	zval tmp;
	int compiler_result;

	if (source_string->value.str.len==0) {
		efree(op_array);
		return NULL;
	}
	tmp = *source_string;
	zval_copy_ctor(&tmp);
	convert_to_string(&tmp);
	source_string = &tmp;

	init_op_array(op_array, INITIAL_OP_ARRAY_SIZE);
	op_array->type = ZEND_EVAL_CODE;
	save_lexical_state(&original_lex_state CLS_CC);
	if (prepare_string_for_scanning(source_string CLS_CC)==FAILURE) {
		destroy_op_array(op_array);
		efree(op_array);
		retval = NULL;
	} else {
		CG(active_op_array) = op_array;
#ifndef ZTS
		BEGIN(ST_IN_SCRIPTING);
#else
		CG(ZFL)->BeginState(ST_IN_SCRIPTING);
#endif
		compiler_result = zendparse(CLS_C);
		restore_lexical_state(&original_lex_state CLS_CC);
		CG(active_op_array) = original_active_op_array;
		
		if (compiler_result==1) {
			CG(unclean_shutdown)=1;
			retval = NULL;
		} else {
			pass_two(op_array);
			pass_include_eval(op_array);
			retval = op_array;
		}
	}
	zval_dtor(&tmp);
	return retval;
}


BEGIN_EXTERN_C()
int require_filename(char *filename CLS_DC)
{
	zend_file_handle file_handle;

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = filename;
	if (require_file(&file_handle CLS_CC)==FAILURE) {
		zend_bailout();
		return FAILURE; /* will never get here */
	}
	return SUCCESS;
}


int require_file(zend_file_handle *file_handle CLS_DC)
{
	zend_lex_state original_lex_state;
	int compiler_result;

	save_lexical_state(&original_lex_state CLS_CC);
	if (open_file_for_scanning(file_handle CLS_CC)==FAILURE) {
		zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
		return FAILURE;
	}
	compiler_result = zendparse(CLS_C);
	zend_close_file_handle(file_handle CLS_CC);
	restore_lexical_state(&original_lex_state CLS_CC);
	if (compiler_result==1) {
		zend_bailout();
	}
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
	zend_close_file_handle(&file_handle CLS_CC);
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
	if (prepare_string_for_scanning(str CLS_CC)==FAILURE) {
		return FAILURE;
	}
	zend_highlight(syntax_highlighter_ini);
	restore_lexical_state(&original_lex_state CLS_CC);
	zval_dtor(str);
	return SUCCESS;
}
END_EXTERN_C()

#ifdef ZTS
BEGIN_EXTERN_C()
int lex_scan(zval *zendlval CLS_DC)
{
	return CG(ZFL)->lex_scan(zendlval CLS_CC);
}


const char *zend_get_zendtext(CLS_D)
{
	return CG(ZFL)->YYText();
}


int zend_get_zendleng(CLS_D)
{
	return CG(ZFL)->YYLeng();
}
END_EXTERN_C()


void ZendFlexLexer::BeginState(int state)
{
	BEGIN(state);
}


ZendFlexLexer::~ZendFlexLexer()
{
	if (yy_start_stack) {
		yy_flex_free(yy_start_stack);
	}
}


int yyFlexLexer::yylex()
{
	fprintf(stderr, "Error:  yyFlexLexer::yylex() called\n");
	return -1;
}

#endif


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

<ST_IN_SCRIPTING>"exit" {
	return T_EXIT;
}

<ST_IN_SCRIPTING>"die" {
	return T_EXIT;
}

<ST_IN_SCRIPTING>"old_function" {
	return T_OLD_FUNCTION;
}

<ST_IN_SCRIPTING>"function"|"cfunction" {
	return T_FUNCTION;
}

<ST_IN_SCRIPTING>"const" {
	return T_CONST;
}

<ST_IN_SCRIPTING>"return" {
	return T_RETURN;
}

<ST_IN_SCRIPTING>"if" {
	return T_IF;
}

<ST_IN_SCRIPTING>"elseif" {
	return T_ELSEIF;
}

<ST_IN_SCRIPTING>"endif" {
	return T_ENDIF;
}

<ST_IN_SCRIPTING>"else" {
	return T_ELSE;
}

<ST_IN_SCRIPTING>"while" {
	return T_WHILE;
}

<ST_IN_SCRIPTING>"endwhile" {
	return T_ENDWHILE;
}

<ST_IN_SCRIPTING>"do" {
	return T_DO;
}

<ST_IN_SCRIPTING>"for" {
	return T_FOR;
}

<ST_IN_SCRIPTING>"endfor" {
	return T_ENDFOR;
}

<ST_IN_SCRIPTING>"foreach" {
	return T_FOREACH;
}

<ST_IN_SCRIPTING>"endforeach" {
	return T_ENDFOREACH;
}

<ST_IN_SCRIPTING>"as" {
	return T_AS;
}

<ST_IN_SCRIPTING>"switch" {
	return T_SWITCH;
}

<ST_IN_SCRIPTING>"endswitch" {
	return T_ENDSWITCH;
}

<ST_IN_SCRIPTING>"case" {
	return T_CASE;
}

<ST_IN_SCRIPTING>"default" {
	return T_DEFAULT;
}

<ST_IN_SCRIPTING>"break" {
	return T_BREAK;
}

<ST_IN_SCRIPTING>"continue" {
	return T_CONTINUE;
}

<ST_IN_SCRIPTING>"echo" {
	return T_ECHO;
}

<ST_IN_SCRIPTING>"print" {
	return T_PRINT;
}

<ST_IN_SCRIPTING>"class" {
	return T_CLASS;
}

<ST_IN_SCRIPTING>"extends" {
	return T_EXTENDS;
}

<ST_IN_SCRIPTING,ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"->" {
	yy_push_state(ST_LOOKING_FOR_PROPERTY);
	return T_OBJECT_OPERATOR;
}

<ST_LOOKING_FOR_PROPERTY>{LABEL} {
	yy_pop_state();
	zendlval->value.str.val = (char *)estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return T_STRING;
}

<ST_LOOKING_FOR_PROPERTY>. {
	/*unput(yytext[0]);*/
	yyless(0);
	yy_pop_state();
}

<ST_IN_SCRIPTING>"::" {
	return T_PAAMAYIM_NEKUDOTAYIM;
}

<ST_IN_SCRIPTING>"new" {
	return T_NEW;
}

<ST_IN_SCRIPTING>"var" {
	return T_VAR;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("int"|"integer"){TABS_AND_SPACES}")" {
	return T_INT_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("real"|"double"|"float"){TABS_AND_SPACES}")" {
	return T_DOUBLE_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"string"{TABS_AND_SPACES}")" {
	return T_STRING_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"array"{TABS_AND_SPACES}")" {
	return T_ARRAY_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"object"{TABS_AND_SPACES}")" {
	return T_OBJECT_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("bool"|"boolean"){TABS_AND_SPACES}")" {
	return T_BOOL_CAST;
}

<ST_IN_SCRIPTING>"eval" {
	return T_EVAL;
}

<ST_IN_SCRIPTING>"include" {
	return T_INCLUDE;
}

<ST_IN_SCRIPTING>"require" {
	return T_REQUIRE;
}

<ST_IN_SCRIPTING>"global" {
	return T_GLOBAL;
}

<ST_IN_SCRIPTING>"isset" {
	return T_ISSET;
}

<ST_IN_SCRIPTING>"empty" {
	return T_EMPTY;
}

<ST_IN_SCRIPTING>"static" {
	return T_STATIC;
}

<ST_IN_SCRIPTING>"unset" {
	return T_UNSET;
}

<ST_IN_SCRIPTING>"=>" {
	return T_DOUBLE_ARROW;
}

<ST_IN_SCRIPTING>"list" {
	return T_LIST;
}

<ST_IN_SCRIPTING>"array" {
	return T_ARRAY;
}

<ST_IN_SCRIPTING>"++" {
	return T_INC;
}

<ST_IN_SCRIPTING>"--" {
	return T_DEC;
}

<ST_IN_SCRIPTING>"===" {
	return T_IS_IDENTICAL;
}

<ST_IN_SCRIPTING>"==" {
	return T_IS_EQUAL;
}

<ST_IN_SCRIPTING>"!="|"<>" {
	return T_IS_NOT_EQUAL;
}

<ST_IN_SCRIPTING>"<=" {
	return T_IS_SMALLER_OR_EQUAL;
}

<ST_IN_SCRIPTING>">=" {
	return T_IS_GREATER_OR_EQUAL;
}

<ST_IN_SCRIPTING>"+=" {
	return T_PLUS_EQUAL;
}

<ST_IN_SCRIPTING>"-=" {
	return T_MINUS_EQUAL;
}

<ST_IN_SCRIPTING>"*=" {
	return T_MUL_EQUAL;
}

<ST_IN_SCRIPTING>"/=" {
	return T_DIV_EQUAL;
}

<ST_IN_SCRIPTING>".=" {
	return T_CONCAT_EQUAL;
}


<ST_IN_SCRIPTING>"%=" {
	return T_MOD_EQUAL;
}

<ST_IN_SCRIPTING>"<<=" {
	return T_SL_EQUAL;
}

<ST_IN_SCRIPTING>">>=" {
	return T_SR_EQUAL;
}

<ST_IN_SCRIPTING>"&=" {
	return T_AND_EQUAL;
}

<ST_IN_SCRIPTING>"|=" {
	return T_OR_EQUAL;
}

<ST_IN_SCRIPTING>"^=" {
	return XT_OR_EQUAL;
}

<ST_IN_SCRIPTING>"||" {
	return T_BOOLEAN_OR;
}

<ST_IN_SCRIPTING>"&&" {
	return T_BOOLEAN_AND;
}

<ST_IN_SCRIPTING>"OR" {
	return T_LOGICAL_OR;
}

<ST_IN_SCRIPTING>"AND" {
	return T_LOGICAL_AND;
}

<ST_IN_SCRIPTING>"XOR" {
	return T_LOGICAL_XOR;
}

<ST_IN_SCRIPTING>"<<" {
	return T_SL;
}

<ST_IN_SCRIPTING>">>" {
	return T_SR;
}

<ST_IN_SCRIPTING>{TOKENS} {
	return yytext[0];
}


<ST_IN_SCRIPTING>"{" {
	yy_push_state(ST_IN_SCRIPTING);
	return '{';
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"${" {
	yy_push_state(ST_LOOKING_FOR_VARNAME);
	return T_DOLLAR_OPEN_CURLY_BRACES;
}


<ST_IN_SCRIPTING>"}" {
	/* This is a temporary fix which is dependant on flex and it's implementation */
	if (yy_start_stack_ptr) {
		yy_pop_state();
	}
	return '}';
}


<ST_LOOKING_FOR_VARNAME>{LABEL} {
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	yy_pop_state();
	yy_push_state(ST_IN_SCRIPTING);
	return T_STRING_VARNAME;
}


<ST_LOOKING_FOR_VARNAME>. {
	/*unput(yytext[0]);*/
	yyless(0);
	yy_pop_state();
	yy_push_state(ST_IN_SCRIPTING);
}


<ST_IN_SCRIPTING>{LNUM}|{HNUM} {
	errno = 0;
	zendlval->value.lval = strtol(yytext, NULL, 0);
	if (errno == ERANGE) { /* overflow */
		zendlval->value.dval = strtod(yytext,NULL);
		zendlval->type = IS_DOUBLE;
		return T_DNUMBER;
	} else {
		zendlval->type = IS_LONG;
		return T_LNUMBER;
	}
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>{LNUM}|{HNUM} { /* treat numbers (almost) as strings inside encapsulated strings */
	zendlval->value.str.val = (char *)estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return T_NUM_STRING;
}

<ST_IN_SCRIPTING>{DNUM}|{EXPONENT_DNUM} {
	zendlval->value.dval = strtod(yytext,NULL);
	zendlval->type = IS_DOUBLE;
	return T_DNUMBER;
}

<ST_IN_SCRIPTING>"__LINE__" {
	zendlval->value.lval = CG(zend_lineno);
	zendlval->type = IS_LONG;
	return T_LINE;
}

<ST_IN_SCRIPTING>"__FILE__" {
	char *filename = zend_get_compiled_filename();
	
	if (!filename) {
		filename = "";
	}
	zendlval->value.str.len = strlen(filename);
	zendlval->value.str.val = estrndup(filename,zendlval->value.str.len);
	zendlval->type = IS_STRING;
	return T_FILE;
}


<INITIAL>(([^<]|"<"[^?%s<]){1,400})|"<s"|"<" {
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINES(yytext,yyleng);
	return T_INLINE_HTML;
}

<INITIAL>"<?"|"<script"{WHITESPACE}+"language"{WHITESPACE}*"="{WHITESPACE}*("php"|"\"php\""|"\'php\'"){WHITESPACE}*">" {
	HANDLE_NEWLINES(yytext,yyleng);
	if (CG(short_tags) || yyleng>2) { /* yyleng>2 means it's not <? but <script> */
		zendlval->value.str.val = yytext; /* no copying - intentional */
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		BEGIN(ST_IN_SCRIPTING);
		return T_OPEN_TAG;
	} else {
		zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		return T_INLINE_HTML;
	}
}


<INITIAL>"<%="|"<?=" {
	if ((yytext[1]=='%' && CG(asp_tags))
		|| (yytext[1]=='?' && CG(short_tags))) {
		zendlval->value.str.val = yytext; /* no copying - intentional */
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		BEGIN(ST_IN_SCRIPTING);
		return T_OPEN_TAG_WITH_ECHO;
	} else {
		zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		return T_INLINE_HTML;
	}
}


<INITIAL>"<%" {
	if (CG(asp_tags)) {
		zendlval->value.str.val = yytext; /* no copying - intentional */
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		BEGIN(ST_IN_SCRIPTING);
	} else {
		zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		return T_INLINE_HTML;
	}
}


<INITIAL>"<?php"[ \n\r\t] {
	zendlval->value.str.val = yytext; /* no copying - intentional */
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINE(yytext[yyleng-1]);
	BEGIN(ST_IN_SCRIPTING);
	return T_OPEN_TAG;
}


<INITIAL>"<?php_track_vars?>"([\n]|"\r\n")? {
	zend_message_dispatcher(ZMSG_ENABLE_TRACK_VARS, NULL);
	HANDLE_NEWLINE(yytext[yyleng-1]);
	return T_PHP_TRACK_VARS;
}

<ST_IN_SCRIPTING,ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE>"$"{LABEL} {
	zendlval->value.str.val = (char *)estrndup(yytext+1, yyleng-1);
	zendlval->value.str.len = yyleng-1;
	zendlval->type = IS_STRING;
	return T_VARIABLE;
}


<ST_IN_SCRIPTING>{LABEL} {
	zendlval->value.str.val = (char *)estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return T_STRING;
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>{LABEL} {
	zendlval->value.str.val = (char *)estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return T_STRING;
}


<ST_IN_SCRIPTING>{WHITESPACE} {
	zendlval->value.str.val = yytext; /* no copying - intentional */
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINES(yytext,yyleng);
	return T_WHITESPACE;
}


<ST_IN_SCRIPTING>([#]|"//")([^\n\r?]|"?"[^>\n\r])*("?\n"|"?\r\n")? { /* eat one line comments */
	HANDLE_NEWLINE(yytext[yyleng-1]);
	return T_COMMENT;
}

<ST_IN_SCRIPTING>"/*" {
	CG(comment_start_line) = CG(zend_lineno);
	BEGIN(ST_COMMENT);
	yymore();
}


<ST_COMMENT>[^*]+ {
	yymore();
}

<ST_COMMENT>"*/" {
	HANDLE_NEWLINES(yytext, yyleng);
	BEGIN(ST_IN_SCRIPTING);
	return T_COMMENT;
}

<ST_COMMENT>"*" {
	yymore();
}

<ST_IN_SCRIPTING>("?>"|"</script"{WHITESPACE}*">")([\n]|"\r\n")? {
	zendlval->value.str.val = yytext; /* no copying - intentional */
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINES(yytext, yyleng);
	BEGIN(INITIAL);
	return T_CLOSE_TAG;  /* implicit ';' at php-end tag */
}


<ST_IN_SCRIPTING>"%>"([\n]|"\r\n")? {
	zendlval->value.str.val = yytext; /* no copying - intentional */
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	HANDLE_NEWLINES(yytext,yyleng);
	if (CG(asp_tags)) {
		BEGIN(INITIAL);
		return T_CLOSE_TAG;  /* implicit ';' at php-end tag */
	} else {
		return T_INLINE_HTML;
	}
}


<ST_IN_SCRIPTING>(["]([^$"\\]|("\\".))*["]) {
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
				case '$':
				case '"':
					*t++ = *s;
					zendlval->value.str.len--;
					break;
				default:
					/* check for an octal */
					if (ZEND_IS_OCT(*s)) {
						char octal_buf[4] = { 0, 0, 0, 0 };

						octal_buf[0] = *s;
						zendlval->value.str.len--;
						if ((s+1)<end && ZEND_IS_OCT(*(s+1))) {
							octal_buf[1] = *(++s);
							zendlval->value.str.len--;
							if ((s+1)<end && ZEND_IS_OCT(*(s+1))) {
								octal_buf[2] = *(++s);
								zendlval->value.str.len--;
							}
						}
						*t++ = (char) strtol(octal_buf, NULL, 8);
					} else if (*s=='x' && (s+1)<end && ZEND_IS_HEX(*(s+1))) {
						char hex_buf[3] = { 0, 0, 0};

						zendlval->value.str.len--; /* for the 'x' */

						hex_buf[0] = *(++s);
						zendlval->value.str.len--;
						if ((s+1)<end && ZEND_IS_HEX(*(s+1))) {
							hex_buf[1] = *(++s);
							zendlval->value.str.len--;
						}
						*t++ = (char) strtol(hex_buf, NULL, 16);
					} else {
						*t++ = '\\';
						*t++ = *s;
					}
					break;
			}
			s++;
		} else {
			*t++ = *s++;
		}
	}
	*t = 0;
	
	return T_CONSTANT_ENCAPSED_STRING;
}


<ST_IN_SCRIPTING>([']([^'\\]|("\\".))*[']) {
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
				case '\\':
				case '\'':
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
	
	return T_CONSTANT_ENCAPSED_STRING;
}


<ST_IN_SCRIPTING>["] {
	BEGIN(ST_DOUBLE_QUOTES);
	return '\"';
}


<ST_IN_SCRIPTING>"<<<"{TABS_AND_SPACES}{LABEL}("\r")?"\n" {
	char *s;
	CG(zend_lineno)++;
	CG(heredoc_len) = yyleng-3-1-(yytext[yyleng-2]=='\r'?1:0);
	s = yytext+3;
	while ((*s == ' ') || (*s == '\t')) {
		s++;
		CG(heredoc_len)--;
	}
	CG(heredoc) = estrndup(s, CG(heredoc_len));
	BEGIN(ST_HEREDOC);
	return T_HEREDOC;
}
	

<ST_IN_SCRIPTING>[`] {
	BEGIN(ST_BACKQUOTE);
	return '`';
}


<ST_IN_SCRIPTING>['] {
	BEGIN(ST_SINGLE_QUOTE);
	return '\'';
}


<ST_HEREDOC>^{LABEL}(";")?("\r")?"\n" {
	int label_len;
	unsigned char unput_semicolon;

	CG(zend_lineno)++;
	if (yytext[yyleng-2]=='\r') {
		label_len = yyleng-2;
	} else {
		label_len = yyleng-1;
	}
	
	if (yytext[label_len-1]==';') {
		label_len--;
		unput_semicolon=1;
	} else{
		unput_semicolon=0;
	}

	if (label_len==CG(heredoc_len) && !memcmp(yytext, CG(heredoc), label_len)) {
		if (unput_semicolon) {
			unput(';');
		}
		efree(CG(heredoc));
		CG(heredoc)=NULL;
		CG(heredoc_len)=0;
		BEGIN(ST_IN_SCRIPTING);
		return T_HEREDOC;
	} else {
		zendlval->value.str.val = (char *)estrndup(yytext, yyleng);
		zendlval->value.str.len = yyleng;
		zendlval->type = IS_STRING;
		return T_STRING;
	}
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>{ESCAPED_AND_WHITESPACE} {
	HANDLE_NEWLINES(yytext,yyleng);
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return T_ENCAPSED_AND_WHITESPACE;
}

<ST_SINGLE_QUOTE>([^'\\]|\\[^'\\])+ {
	HANDLE_NEWLINES(yytext,yyleng);
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return T_ENCAPSED_AND_WHITESPACE;
}


<ST_DOUBLE_QUOTES>[`]+ {
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return T_ENCAPSED_AND_WHITESPACE;
}


<ST_BACKQUOTE>["]+ {
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return T_ENCAPSED_AND_WHITESPACE;
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"$"[^a-zA-Z_\x7f-\xff{] {
	zendlval->value.chval = yytext[0];
	if (yyleng == 2) {
		yyless(1);
		/*unput(yytext[1]);
		yytext[1] = 0;
		yyleng--;*/
	}
	return T_CHARACTER;	
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>{ENCAPSED_TOKENS} {
	zendlval->value.chval = yytext[0];
	return yytext[0];
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"{$" {
	zendlval->value.chval = yytext[0];
	yy_push_state(ST_IN_SCRIPTING);
	/*unput('$');*/
	yyless(1);
	return T_CURLY_OPEN;
}


<ST_SINGLE_QUOTE>"\\'" {
	zendlval->value.chval='\'';
	return T_CHARACTER;
}

<ST_SINGLE_QUOTE>"\\\\" {
	zendlval->value.chval='\\';
	return T_CHARACTER;
}

<ST_DOUBLE_QUOTES>"\\\"" {
	zendlval->value.chval='"';
	return T_CHARACTER;
}

<ST_BACKQUOTE>"\\`" {
	zendlval->value.chval='`';
	return T_CHARACTER;
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"\\"[0-7]{1,3} {
	zendlval->value.chval = (char) strtol (yytext+1, NULL, 8);
	return T_CHARACTER;
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"\\x"[0-9A-Fa-f]{1,2} {
	zendlval->value.chval = (char) strtol (yytext+2, NULL, 16);
	return T_CHARACTER;
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"\\". {
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
			return T_BAD_CHARACTER;
			break;
	}
	return T_CHARACTER;
}


<ST_HEREDOC>["'`]+ {
	zendlval->value.str.val = (char *) estrndup(yytext, yyleng);
	zendlval->value.str.len = yyleng;
	zendlval->type = IS_STRING;
	return T_ENCAPSED_AND_WHITESPACE;
}


<ST_DOUBLE_QUOTES>["] {
	BEGIN(ST_IN_SCRIPTING);
	return '\"';
}


<ST_BACKQUOTE>[`] {
	BEGIN(ST_IN_SCRIPTING);
	return '`';
}


<ST_SINGLE_QUOTE>['] {
	BEGIN(ST_IN_SCRIPTING);
	return '\'';
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,INITIAL,ST_IN_SCRIPTING,ST_LOOKING_FOR_PROPERTY><<EOF>> {
	return 0;
}

<ST_COMMENT><<EOF>> {
	zend_error(E_COMPILE_WARNING,"Unterminated comment starting line %d", CG(comment_start_line));
	return 0;
}
	


<ST_IN_SCRIPTING,INITIAL,ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_SINGLE_QUOTE,ST_HEREDOC>. {
	zend_error(E_COMPILE_WARNING,"Unexpected character in input:  '%c' (ASCII=%d) state=%d",yytext[0],yytext[0],YYSTATE);
}
