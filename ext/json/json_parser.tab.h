/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_PHP_JSON_YY_HOME_JAKUB_PROG_PHP_MASTER_EXT_JSON_JSON_PARSER_TAB_H_INCLUDED
# define YY_PHP_JSON_YY_HOME_JAKUB_PROG_PHP_MASTER_EXT_JSON_JSON_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int php_json_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    PHP_JSON_T_NUL = 258,
    PHP_JSON_T_TRUE = 259,
    PHP_JSON_T_FALSE = 260,
    PHP_JSON_T_INT = 261,
    PHP_JSON_T_DOUBLE = 262,
    PHP_JSON_T_STRING = 263,
    PHP_JSON_T_ESTRING = 264,
    PHP_JSON_T_EOI = 265,
    PHP_JSON_T_ERROR = 266
  };
#endif
/* Tokens.  */
#define PHP_JSON_T_NUL 258
#define PHP_JSON_T_TRUE 259
#define PHP_JSON_T_FALSE 260
#define PHP_JSON_T_INT 261
#define PHP_JSON_T_DOUBLE 262
#define PHP_JSON_T_STRING 263
#define PHP_JSON_T_ESTRING 264
#define PHP_JSON_T_EOI 265
#define PHP_JSON_T_ERROR 266

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{


	zval value;
	struct {
		zend_string *key;
		zval val;
	} pair;


};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int php_json_yyparse (php_json_parser *parser);

#endif /* !YY_PHP_JSON_YY_HOME_JAKUB_PROG_PHP_MASTER_EXT_JSON_JSON_PARSER_TAB_H_INCLUDED  */
