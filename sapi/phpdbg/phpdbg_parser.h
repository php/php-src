/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison interface for Yacc-like parsers in C

      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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

#ifndef YY_PHPDBG_SAPI_PHPDBG_PHPDBG_PARSER_H_INCLUDED
# define YY_PHPDBG_SAPI_PHPDBG_PHPDBG_PARSER_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int phpdbg_debug;
#endif
/* "%code requires" blocks.  */
/* Line 2060 of yacc.c  */
#line 36 "sapi/phpdbg/phpdbg_parser.y"

#include "phpdbg.h"
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif


/* Line 2060 of yacc.c  */
#line 55 "sapi/phpdbg/phpdbg_parser.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_EVAL = 258,
     T_RUN = 259,
     T_SHELL = 260,
     T_IF = 261,
     T_TRUTHY = 262,
     T_FALSY = 263,
     T_STRING = 264,
     T_COLON = 265,
     T_DCOLON = 266,
     T_POUND = 267,
     T_SEPARATOR = 268,
     T_PROTO = 269,
     T_DIGITS = 270,
     T_LITERAL = 271,
     T_ADDR = 272,
     T_OPCODE = 273,
     T_ID = 274,
     T_INPUT = 275,
     T_UNEXPECTED = 276,
     T_REQ_ID = 277
   };
#endif
/* Tokens.  */
#define T_EVAL 258
#define T_RUN 259
#define T_SHELL 260
#define T_IF 261
#define T_TRUTHY 262
#define T_FALSY 263
#define T_STRING 264
#define T_COLON 265
#define T_DCOLON 266
#define T_POUND 267
#define T_SEPARATOR 268
#define T_PROTO 269
#define T_DIGITS 270
#define T_LITERAL 271
#define T_ADDR 272
#define T_OPCODE 273
#define T_ID 274
#define T_INPUT 275
#define T_UNEXPECTED 276
#define T_REQ_ID 277



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int phpdbg_parse (void *YYPARSE_PARAM);
#else
int phpdbg_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int phpdbg_parse (void);
#else
int phpdbg_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_PHPDBG_SAPI_PHPDBG_PHPDBG_PARSER_H_INCLUDED  */
