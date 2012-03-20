/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse syckparse
#define yylex   sycklex
#define yyerror syckerror
#define yylval  sycklval
#define yychar  syckchar
#define yydebug syckdebug
#define yynerrs sycknerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     YAML_ANCHOR = 258,
     YAML_ALIAS = 259,
     YAML_TRANSFER = 260,
     YAML_TAGURI = 261,
     YAML_ITRANSFER = 262,
     YAML_WORD = 263,
     YAML_PLAIN = 264,
     YAML_BLOCK = 265,
     YAML_DOCSEP = 266,
     YAML_IOPEN = 267,
     YAML_INDENT = 268,
     YAML_IEND = 269
   };
#endif
#define YAML_ANCHOR 258
#define YAML_ALIAS 259
#define YAML_TRANSFER 260
#define YAML_TAGURI 261
#define YAML_ITRANSFER 262
#define YAML_WORD 263
#define YAML_PLAIN 264
#define YAML_BLOCK 265
#define YAML_DOCSEP 266
#define YAML_IOPEN 267
#define YAML_INDENT 268
#define YAML_IEND 269




/* Copy the first part of user declarations.  */
#line 14 "gram.y"


#include "syck.h"

void apply_seq_in_map( SyckParser *parser, SyckNode *n );

#define YYPARSE_PARAM   parser
#define YYLEX_PARAM     parser

#define NULL_NODE(parser, node) \
        SyckNode *node = syck_new_str( "", scalar_plain ); \
        if ( ((SyckParser *)parser)->taguri_expansion == 1 ) \
        { \
            node->type_id = syck_taguri( YAML_DOMAIN, "null", 4 ); \
        } \
        else \
        { \
            node->type_id = syck_strndup( "null", 4 ); \
        }


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 35 "gram.y"
typedef union YYSTYPE {
    SYMID nodeId;
    SyckNode *nodeData;
    char *name;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 140 "gram.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 152 "gram.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  52
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   396

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  23
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  29
/* YYNRULES -- Number of rules. */
#define YYNRULES  79
/* YYNRULES -- Number of states. */
#define YYNSTATES  128

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   269

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    21,    15,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    16,     2,
       2,     2,     2,    22,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    17,     2,    18,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    19,     2,    20,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    11,    13,    15,    18,
      21,    24,    28,    30,    32,    36,    37,    40,    43,    46,
      49,    51,    54,    56,    58,    60,    63,    66,    69,    72,
      75,    77,    79,    81,    85,    87,    89,    91,    93,    95,
      99,   103,   106,   110,   113,   117,   120,   124,   127,   129,
     133,   136,   140,   143,   145,   149,   151,   153,   157,   161,
     165,   168,   172,   175,   179,   182,   184,   188,   190,   194,
     196,   200,   204,   207,   211,   215,   218,   220,   224,   226
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      24,     0,    -1,    25,    -1,    11,    27,    -1,    -1,    33,
      -1,    26,    -1,    34,    -1,     5,    26,    -1,     6,    26,
      -1,     3,    26,    -1,    29,    26,    32,    -1,    25,    -1,
      28,    -1,    29,    28,    30,    -1,    -1,     7,    28,    -1,
       5,    28,    -1,     6,    28,    -1,     3,    28,    -1,    12,
      -1,    29,    13,    -1,    14,    -1,    13,    -1,    14,    -1,
      31,    32,    -1,     5,    33,    -1,     6,    33,    -1,     7,
      33,    -1,     3,    33,    -1,     4,    -1,     8,    -1,     9,
      -1,    29,    33,    32,    -1,    10,    -1,    35,    -1,    39,
      -1,    42,    -1,    49,    -1,    29,    37,    30,    -1,    29,
      38,    30,    -1,    15,    27,    -1,     5,    31,    38,    -1,
       5,    37,    -1,     6,    31,    38,    -1,     6,    37,    -1,
       3,    31,    38,    -1,     3,    37,    -1,    36,    -1,    38,
      31,    36,    -1,    38,    31,    -1,    17,    40,    18,    -1,
      17,    18,    -1,    41,    -1,    40,    21,    41,    -1,    25,
      -1,    48,    -1,    29,    43,    30,    -1,    29,    47,    30,
      -1,     5,    31,    47,    -1,     5,    43,    -1,     6,    31,
      47,    -1,     6,    43,    -1,     3,    31,    47,    -1,     3,
      43,    -1,    33,    -1,    22,    25,    31,    -1,    27,    -1,
      44,    16,    45,    -1,    46,    -1,    47,    31,    36,    -1,
      47,    31,    46,    -1,    47,    31,    -1,    25,    16,    27,
      -1,    19,    50,    20,    -1,    19,    20,    -1,    51,    -1,
      50,    21,    51,    -1,    25,    -1,    48,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    56,    56,    60,    65,    70,    71,    74,    75,    80,
      85,    94,   100,   101,   104,   109,   113,   121,   126,   131,
     145,   146,   149,   152,   155,   156,   164,   169,   174,   182,
     186,   194,   207,   208,   218,   219,   220,   221,   222,   228,
     232,   238,   244,   249,   254,   259,   264,   268,   274,   278,
     283,   292,   296,   302,   306,   313,   314,   320,   325,   332,
     337,   342,   347,   352,   356,   362,   363,   369,   379,   396,
     397,   409,   417,   426,   434,   438,   444,   445,   454,   461
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "YAML_ANCHOR", "YAML_ALIAS",
  "YAML_TRANSFER", "YAML_TAGURI", "YAML_ITRANSFER", "YAML_WORD",
  "YAML_PLAIN", "YAML_BLOCK", "YAML_DOCSEP", "YAML_IOPEN", "YAML_INDENT",
  "YAML_IEND", "'-'", "':'", "'['", "']'", "'{'", "'}'", "','", "'?'",
  "$accept", "doc", "atom", "ind_rep", "atom_or_empty", "empty",
  "indent_open", "indent_end", "indent_sep", "indent_flex_end", "word_rep",
  "struct_rep", "implicit_seq", "basic_seq", "top_imp_seq",
  "in_implicit_seq", "inline_seq", "in_inline_seq", "inline_seq_atom",
  "implicit_map", "top_imp_map", "complex_key", "complex_value",
  "complex_mapping", "in_implicit_map", "basic_mapping", "inline_map",
  "in_inline_map", "inline_map_atom", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,    45,    58,    91,    93,   123,
     125,    44,    63
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    23,    24,    24,    24,    25,    25,    26,    26,    26,
      26,    26,    27,    27,    28,    28,    28,    28,    28,    28,
      29,    29,    30,    31,    32,    32,    33,    33,    33,    33,
      33,    33,    33,    33,    34,    34,    34,    34,    34,    35,
      35,    36,    37,    37,    37,    37,    37,    37,    38,    38,
      38,    39,    39,    40,    40,    41,    41,    42,    42,    43,
      43,    43,    43,    43,    43,    44,    44,    45,    46,    47,
      47,    47,    47,    48,    49,    49,    50,    50,    51,    51
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     1,     2,     2,
       2,     3,     1,     1,     3,     0,     2,     2,     2,     2,
       1,     2,     1,     1,     1,     2,     2,     2,     2,     2,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     3,
       3,     2,     3,     2,     3,     2,     3,     2,     1,     3,
       2,     3,     2,     1,     3,     1,     1,     3,     3,     3,
       2,     3,     2,     3,     2,     1,     3,     1,     3,     1,
       3,     3,     2,     3,     3,     2,     1,     3,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       4,     0,    30,     0,     0,     0,    31,    32,    34,    15,
      20,     0,     0,     0,     2,     6,     0,     5,     7,    35,
      36,    37,    38,    10,    29,     8,    26,     9,    27,     0,
       0,     0,     0,    28,    15,    15,    15,    15,    12,     3,
      13,    15,    52,    55,     0,    53,    56,    75,    78,    79,
       0,    76,     1,     0,     0,     0,    21,    15,     0,     0,
      65,    48,     0,     0,     0,     0,    69,     0,     0,    19,
      17,    18,    15,    15,    15,    16,    15,    15,    15,    15,
       0,    15,    51,     0,    74,     0,    23,     0,    47,    64,
       0,    43,    60,     0,    45,    62,    41,     0,    24,     0,
      11,    33,    22,    39,    40,    50,    57,    15,    58,    72,
      14,    73,    54,    77,    65,    46,    63,    42,    59,    44,
      61,    66,    25,    49,    67,    68,    70,    71
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,    13,    38,    15,    39,    40,    16,   103,    99,   101,
      17,    18,    19,    61,    62,    63,    20,    44,    45,    21,
      64,    65,   125,    66,    67,    46,    22,    50,    51
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -97
static const short int yypact[] =
{
     250,   318,   -97,   318,   318,   374,   -97,   -97,   -97,   335,
     -97,   267,   232,     7,   -97,   -97,   192,   -97,   -97,   -97,
     -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   374,
     374,   374,   352,   -97,   335,   335,   335,   384,   -97,   -97,
     -97,   212,   -97,    10,     0,   -97,   -97,   -97,    10,   -97,
      -4,   -97,   -97,   284,   284,   284,   -97,   335,   318,    30,
      30,   -97,    -2,    36,    -2,    16,   -97,    36,    30,   -97,
     -97,   -97,   384,   384,   384,   -97,   363,   301,   301,   301,
      -2,   335,   -97,   318,   -97,   318,   -97,   158,   -97,   -97,
     158,   -97,   -97,   158,   -97,   -97,   -97,    24,   -97,    30,
     -97,   -97,   -97,   -97,   -97,    26,   -97,   335,   -97,   158,
     -97,   -97,   -97,   -97,   -97,    24,    24,    24,    24,    24,
      24,   -97,   -97,   -97,   -97,   -97,   -97,   -97
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -97,   -97,     8,    81,   -56,   109,    33,   -53,    74,   -54,
      -1,   -97,   -97,   -96,   -31,   -32,   -97,   -97,   -44,   -97,
      77,   -97,   -97,   -52,     9,    -6,   -97,   -97,   -29
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      24,    96,    26,    28,    33,   100,    49,    52,    14,   123,
     104,   106,   102,   126,   108,    60,    84,    85,    82,    43,
      48,    83,    88,    91,    94,   111,    81,   110,    24,    26,
      28,    68,   107,    24,    26,    28,    33,    86,    32,   112,
      60,    57,    41,    86,    98,   122,    88,    91,    94,    86,
     102,   124,    24,    26,    28,   115,   113,   127,   117,     0,
       0,   119,    32,    32,    32,    32,    97,    41,    41,    41,
      76,    24,    26,    28,    41,    68,    24,    26,    28,    49,
       0,     0,    23,     0,    25,    27,   114,     0,     0,   114,
      41,    43,   114,    48,     0,     0,   116,    59,     0,   118,
       0,     0,   120,     0,     0,    76,    76,    76,   114,    76,
      41,    41,    41,     0,    41,    23,    25,    27,     0,     0,
      32,     0,    59,    32,     0,     0,    32,    87,    90,    93,
      89,    92,    95,     0,    23,    25,    27,   105,     0,     0,
      41,   109,    32,    69,    70,    71,    75,     0,     0,     0,
      80,    87,    90,    93,    89,    92,    95,     0,    23,    25,
      27,    29,     2,    30,    31,     5,     6,     7,     0,     0,
      10,   121,     0,    57,     0,     0,     0,     0,     0,     0,
      58,    69,    70,    71,     0,    80,    69,    70,    71,   105,
     109,   105,   109,   105,   109,    53,     2,    54,    55,     5,
       6,     7,     8,     0,    10,    56,     0,    57,     0,    11,
       0,    12,     0,     0,    58,    77,     2,    78,    79,    37,
       6,     7,     8,     0,    10,    56,     0,    57,     0,    11,
       0,    12,     0,     0,    58,     1,     2,     3,     4,     5,
       6,     7,     8,     0,    10,     0,     0,     0,     0,    11,
       0,    12,    47,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,     0,     0,     0,     0,    11,     0,    12,
       1,     2,     3,     4,     5,     6,     7,     8,     0,    10,
       0,     0,     0,     0,    11,    42,    12,    53,     2,    54,
      55,     5,     6,     7,     8,     0,    10,    86,     0,     0,
       0,    11,     0,    12,    77,     2,    78,    79,    37,     6,
       7,     8,     0,    10,    86,     0,     0,     0,    11,     0,
      12,     1,     2,     3,     4,     5,     6,     7,     8,     0,
      10,     0,     0,     0,     0,    11,     0,    12,    34,     2,
      35,    36,    37,     6,     7,     8,     0,    10,     0,     0,
       0,     0,    11,     0,    12,    29,     2,    30,    31,     5,
       6,     7,     0,     0,    10,    56,    72,     2,    73,    74,
      37,     6,     7,     0,     0,    10,    56,    29,     2,    30,
      31,     5,     6,     7,     0,     0,    10,    72,     2,    73,
      74,    37,     6,     7,     0,     0,    10
};

static const yysigned_char yycheck[] =
{
       1,    57,     3,     4,     5,    59,    12,     0,     0,   105,
      63,    64,    14,   109,    67,    16,    20,    21,    18,    11,
      12,    21,    53,    54,    55,    81,    16,    80,    29,    30,
      31,    32,    16,    34,    35,    36,    37,    13,     5,    83,
      41,    15,     9,    13,    14,    99,    77,    78,    79,    13,
      14,   107,    53,    54,    55,    87,    85,   109,    90,    -1,
      -1,    93,    29,    30,    31,    32,    58,    34,    35,    36,
      37,    72,    73,    74,    41,    76,    77,    78,    79,    85,
      -1,    -1,     1,    -1,     3,     4,    87,    -1,    -1,    90,
      57,    83,    93,    85,    -1,    -1,    87,    16,    -1,    90,
      -1,    -1,    93,    -1,    -1,    72,    73,    74,   109,    76,
      77,    78,    79,    -1,    81,    34,    35,    36,    -1,    -1,
      87,    -1,    41,    90,    -1,    -1,    93,    53,    54,    55,
      53,    54,    55,    -1,    53,    54,    55,    63,    -1,    -1,
     107,    67,   109,    34,    35,    36,    37,    -1,    -1,    -1,
      41,    77,    78,    79,    77,    78,    79,    -1,    77,    78,
      79,     3,     4,     5,     6,     7,     8,     9,    -1,    -1,
      12,    97,    -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    72,    73,    74,    -1,    76,    77,    78,    79,   115,
     116,   117,   118,   119,   120,     3,     4,     5,     6,     7,
       8,     9,    10,    -1,    12,    13,    -1,    15,    -1,    17,
      -1,    19,    -1,    -1,    22,     3,     4,     5,     6,     7,
       8,     9,    10,    -1,    12,    13,    -1,    15,    -1,    17,
      -1,    19,    -1,    -1,    22,     3,     4,     5,     6,     7,
       8,     9,    10,    -1,    12,    -1,    -1,    -1,    -1,    17,
      -1,    19,    20,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    -1,    -1,    -1,    -1,    17,    -1,    19,
       3,     4,     5,     6,     7,     8,     9,    10,    -1,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,     3,     4,     5,
       6,     7,     8,     9,    10,    -1,    12,    13,    -1,    -1,
      -1,    17,    -1,    19,     3,     4,     5,     6,     7,     8,
       9,    10,    -1,    12,    13,    -1,    -1,    -1,    17,    -1,
      19,     3,     4,     5,     6,     7,     8,     9,    10,    -1,
      12,    -1,    -1,    -1,    -1,    17,    -1,    19,     3,     4,
       5,     6,     7,     8,     9,    10,    -1,    12,    -1,    -1,
      -1,    -1,    17,    -1,    19,     3,     4,     5,     6,     7,
       8,     9,    -1,    -1,    12,    13,     3,     4,     5,     6,
       7,     8,     9,    -1,    -1,    12,    13,     3,     4,     5,
       6,     7,     8,     9,    -1,    -1,    12,     3,     4,     5,
       6,     7,     8,     9,    -1,    -1,    12
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    17,    19,    24,    25,    26,    29,    33,    34,    35,
      39,    42,    49,    26,    33,    26,    33,    26,    33,     3,
       5,     6,    29,    33,     3,     5,     6,     7,    25,    27,
      28,    29,    18,    25,    40,    41,    48,    20,    25,    48,
      50,    51,     0,     3,     5,     6,    13,    15,    22,    26,
      33,    36,    37,    38,    43,    44,    46,    47,    33,    28,
      28,    28,     3,     5,     6,    28,    29,     3,     5,     6,
      28,    16,    18,    21,    20,    21,    13,    31,    37,    43,
      31,    37,    43,    31,    37,    43,    27,    25,    14,    31,
      32,    32,    14,    30,    30,    31,    30,    16,    30,    31,
      30,    27,    41,    51,    33,    38,    47,    38,    47,    38,
      47,    31,    32,    36,    27,    45,    36,    46
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 57 "gram.y"
    {
           ((SyckParser *)parser)->root = syck_hdlr_add_node( (SyckParser *)parser, yyvsp[0].nodeData );
        }
    break;

  case 3:
#line 61 "gram.y"
    {
           ((SyckParser *)parser)->root = syck_hdlr_add_node( (SyckParser *)parser, yyvsp[0].nodeData );
        }
    break;

  case 4:
#line 65 "gram.y"
    {
           ((SyckParser *)parser)->eof = 1;
        }
    break;

  case 8:
#line 76 "gram.y"
    {
            syck_add_transfer( yyvsp[-1].name, yyvsp[0].nodeData, ((SyckParser *)parser)->taguri_expansion );
            yyval.nodeData = yyvsp[0].nodeData;
        }
    break;

  case 9:
#line 81 "gram.y"
    {
            syck_add_transfer( yyvsp[-1].name, yyvsp[0].nodeData, 0 );
            yyval.nodeData = yyvsp[0].nodeData;
        }
    break;

  case 10:
#line 86 "gram.y"
    {
           /*
            * _Anchors_: The language binding must keep a separate symbol table
            * for anchors.  The actual ID in the symbol table is returned to the
            * higher nodes, though.
            */
           yyval.nodeData = syck_hdlr_add_anchor( (SyckParser *)parser, yyvsp[-1].name, yyvsp[0].nodeData );
        }
    break;

  case 11:
#line 95 "gram.y"
    {
           yyval.nodeData = yyvsp[-1].nodeData;
        }
    break;

  case 14:
#line 105 "gram.y"
    {
                    yyval.nodeData = yyvsp[-1].nodeData;
                }
    break;

  case 15:
#line 109 "gram.y"
    {
                    NULL_NODE( parser, n );
                    yyval.nodeData = n;
                }
    break;

  case 16:
#line 114 "gram.y"
    {
                   if ( ((SyckParser *)parser)->implicit_typing == 1 )
                   {
                      try_tag_implicit( yyvsp[0].nodeData, ((SyckParser *)parser)->taguri_expansion );
                   }
                   yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 17:
#line 122 "gram.y"
    {
                    syck_add_transfer( yyvsp[-1].name, yyvsp[0].nodeData, ((SyckParser *)parser)->taguri_expansion );
                    yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 18:
#line 127 "gram.y"
    {
                    syck_add_transfer( yyvsp[-1].name, yyvsp[0].nodeData, 0 );
                    yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 19:
#line 132 "gram.y"
    {
                   /*
                    * _Anchors_: The language binding must keep a separate symbol table
                    * for anchors.  The actual ID in the symbol table is returned to the
                    * higher nodes, though.
                    */
                   yyval.nodeData = syck_hdlr_add_anchor( (SyckParser *)parser, yyvsp[-1].name, yyvsp[0].nodeData );
                }
    break;

  case 26:
#line 165 "gram.y"
    {
               syck_add_transfer( yyvsp[-1].name, yyvsp[0].nodeData, ((SyckParser *)parser)->taguri_expansion );
               yyval.nodeData = yyvsp[0].nodeData;
            }
    break;

  case 27:
#line 170 "gram.y"
    {
               syck_add_transfer( yyvsp[-1].name, yyvsp[0].nodeData, 0 );
               yyval.nodeData = yyvsp[0].nodeData;
            }
    break;

  case 28:
#line 175 "gram.y"
    {
               if ( ((SyckParser *)parser)->implicit_typing == 1 )
               {
                  try_tag_implicit( yyvsp[0].nodeData, ((SyckParser *)parser)->taguri_expansion );
               }
               yyval.nodeData = yyvsp[0].nodeData;
            }
    break;

  case 29:
#line 183 "gram.y"
    {
               yyval.nodeData = syck_hdlr_add_anchor( (SyckParser *)parser, yyvsp[-1].name, yyvsp[0].nodeData );
            }
    break;

  case 30:
#line 187 "gram.y"
    {
               /*
                * _Aliases_: The anchor symbol table is scanned for the anchor name.
                * The anchor's ID in the language's symbol table is returned.
                */
               yyval.nodeData = syck_hdlr_get_anchor( (SyckParser *)parser, yyvsp[0].name );
            }
    break;

  case 31:
#line 195 "gram.y"
    {
               SyckNode *n = yyvsp[0].nodeData;
               if ( ((SyckParser *)parser)->taguri_expansion == 1 )
               {
                   n->type_id = syck_taguri( YAML_DOMAIN, "str", 3 );
               }
               else
               {
                   n->type_id = syck_strndup( "str", 3 );
               }
               yyval.nodeData = n;
            }
    break;

  case 33:
#line 209 "gram.y"
    {
               yyval.nodeData = yyvsp[-1].nodeData;
            }
    break;

  case 39:
#line 229 "gram.y"
    {
                    yyval.nodeData = yyvsp[-1].nodeData;
                }
    break;

  case 40:
#line 233 "gram.y"
    {
                    yyval.nodeData = yyvsp[-1].nodeData;
                }
    break;

  case 41:
#line 239 "gram.y"
    {
                    yyval.nodeId = syck_hdlr_add_node( (SyckParser *)parser, yyvsp[0].nodeData );
                }
    break;

  case 42:
#line 245 "gram.y"
    {
                    syck_add_transfer( yyvsp[-2].name, yyvsp[0].nodeData, ((SyckParser *)parser)->taguri_expansion );
                    yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 43:
#line 250 "gram.y"
    {
                    syck_add_transfer( yyvsp[-1].name, yyvsp[0].nodeData, ((SyckParser *)parser)->taguri_expansion );
                    yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 44:
#line 255 "gram.y"
    {
                    syck_add_transfer( yyvsp[-2].name, yyvsp[0].nodeData, 0 );
                    yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 45:
#line 260 "gram.y"
    {
                    syck_add_transfer( yyvsp[-1].name, yyvsp[0].nodeData, 0 );
                    yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 46:
#line 265 "gram.y"
    {
                    yyval.nodeData = syck_hdlr_add_anchor( (SyckParser *)parser, yyvsp[-2].name, yyvsp[0].nodeData );
                }
    break;

  case 47:
#line 269 "gram.y"
    {
                    yyval.nodeData = syck_hdlr_add_anchor( (SyckParser *)parser, yyvsp[-1].name, yyvsp[0].nodeData );
                }
    break;

  case 48:
#line 275 "gram.y"
    {
                    yyval.nodeData = syck_new_seq( yyvsp[0].nodeId );
                }
    break;

  case 49:
#line 279 "gram.y"
    {
                    syck_seq_add( yyvsp[-2].nodeData, yyvsp[0].nodeId );
                    yyval.nodeData = yyvsp[-2].nodeData;
				}
    break;

  case 50:
#line 284 "gram.y"
    {
                    yyval.nodeData = yyvsp[-1].nodeData;
				}
    break;

  case 51:
#line 293 "gram.y"
    {
                    yyval.nodeData = yyvsp[-1].nodeData;
                }
    break;

  case 52:
#line 297 "gram.y"
    {
                    yyval.nodeData = syck_alloc_seq();
                }
    break;

  case 53:
#line 303 "gram.y"
    {
                    yyval.nodeData = syck_new_seq( syck_hdlr_add_node( (SyckParser *)parser, yyvsp[0].nodeData ) );
                }
    break;

  case 54:
#line 307 "gram.y"
    {
                    syck_seq_add( yyvsp[-2].nodeData, syck_hdlr_add_node( (SyckParser *)parser, yyvsp[0].nodeData ) );
                    yyval.nodeData = yyvsp[-2].nodeData;
				}
    break;

  case 57:
#line 321 "gram.y"
    {
                    apply_seq_in_map( (SyckParser *)parser, yyvsp[-1].nodeData );
                    yyval.nodeData = yyvsp[-1].nodeData;
                }
    break;

  case 58:
#line 326 "gram.y"
    {
                    apply_seq_in_map( (SyckParser *)parser, yyvsp[-1].nodeData );
                    yyval.nodeData = yyvsp[-1].nodeData;
                }
    break;

  case 59:
#line 333 "gram.y"
    {
                    syck_add_transfer( yyvsp[-2].name, yyvsp[0].nodeData, ((SyckParser *)parser)->taguri_expansion );
                    yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 60:
#line 338 "gram.y"
    {
                    syck_add_transfer( yyvsp[-1].name, yyvsp[0].nodeData, ((SyckParser *)parser)->taguri_expansion );
                    yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 61:
#line 343 "gram.y"
    {
                    syck_add_transfer( yyvsp[-2].name, yyvsp[0].nodeData, 0 );
                    yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 62:
#line 348 "gram.y"
    {
                    syck_add_transfer( yyvsp[-1].name, yyvsp[0].nodeData, 0 );
                    yyval.nodeData = yyvsp[0].nodeData;
                }
    break;

  case 63:
#line 353 "gram.y"
    {
                    yyval.nodeData = syck_hdlr_add_anchor( (SyckParser *)parser, yyvsp[-2].name, yyvsp[0].nodeData );
                }
    break;

  case 64:
#line 357 "gram.y"
    {
                    yyval.nodeData = syck_hdlr_add_anchor( (SyckParser *)parser, yyvsp[-1].name, yyvsp[0].nodeData );
                }
    break;

  case 66:
#line 364 "gram.y"
    {
                    yyval.nodeData = yyvsp[-1].nodeData;
                }
    break;

  case 68:
#line 380 "gram.y"
    {
                    yyval.nodeData = syck_new_map(
                        syck_hdlr_add_node( (SyckParser *)parser, yyvsp[-2].nodeData ),
                        syck_hdlr_add_node( (SyckParser *)parser, yyvsp[0].nodeData ) );
                }
    break;

  case 70:
#line 398 "gram.y"
    {
                    if ( yyvsp[-2].nodeData->shortcut == NULL )
                    {
                        yyvsp[-2].nodeData->shortcut = syck_new_seq( yyvsp[0].nodeId );
                    }
                    else
                    {
                        syck_seq_add( yyvsp[-2].nodeData->shortcut, yyvsp[0].nodeId );
                    }
                    yyval.nodeData = yyvsp[-2].nodeData;
                }
    break;

  case 71:
#line 410 "gram.y"
    {
                    apply_seq_in_map( (SyckParser *)parser, yyvsp[-2].nodeData );
                    syck_map_update( yyvsp[-2].nodeData, yyvsp[0].nodeData );
                    syck_free_node( yyvsp[0].nodeData );
                    yyvsp[0].nodeData = NULL;
                    yyval.nodeData = yyvsp[-2].nodeData;
                }
    break;

  case 72:
#line 418 "gram.y"
    {
                    yyval.nodeData = yyvsp[-1].nodeData;
                }
    break;

  case 73:
#line 427 "gram.y"
    {
                    yyval.nodeData = syck_new_map(
                        syck_hdlr_add_node( (SyckParser *)parser, yyvsp[-2].nodeData ),
                        syck_hdlr_add_node( (SyckParser *)parser, yyvsp[0].nodeData ) );
                }
    break;

  case 74:
#line 435 "gram.y"
    {
                    yyval.nodeData = yyvsp[-1].nodeData;
                }
    break;

  case 75:
#line 439 "gram.y"
    {
                    yyval.nodeData = syck_alloc_map();
                }
    break;

  case 77:
#line 446 "gram.y"
    {
                    syck_map_update( yyvsp[-2].nodeData, yyvsp[0].nodeData );
                    syck_free_node( yyvsp[0].nodeData );
                    yyvsp[0].nodeData = NULL;
                    yyval.nodeData = yyvsp[-2].nodeData;
				}
    break;

  case 78:
#line 455 "gram.y"
    {
                    NULL_NODE( parser, n );
                    yyval.nodeData = syck_new_map(
                        syck_hdlr_add_node( (SyckParser *)parser, yyvsp[0].nodeData ),
                        syck_hdlr_add_node( (SyckParser *)parser, n ) );
                }
    break;


    }

/* Line 1010 of yacc.c.  */
#line 1651 "gram.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 464 "gram.y"


void
apply_seq_in_map( SyckParser *parser, SyckNode *n )
{
    long map_len;
    if ( n->shortcut == NULL )
    {
        return;
    }

    map_len = syck_map_count( n );
    syck_map_assign( n, map_value, map_len - 1,
        syck_hdlr_add_node( parser, n->shortcut ) );

    n->shortcut = NULL;
}


