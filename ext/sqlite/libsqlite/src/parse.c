/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 33 "ext/sqlite/libsqlite/src/parse.y"

#include "sqliteInt.h"
#include "parse.h"

/*
** An instance of this structure holds information about the
** LIMIT clause of a SELECT statement.
*/
struct LimitVal {
  int limit;    /* The LIMIT value.  -1 if there is no limit */
  int offset;   /* The OFFSET.  0 if there is none */
};

/*
** An instance of the following structure describes the event of a
** TRIGGER.  "a" is the event type, one of TK_UPDATE, TK_INSERT,
** TK_DELETE, or TK_INSTEAD.  If the event is of the form
**
**      UPDATE ON (a,b,c)
**
** Then the "b" IdList records the list "a,b,c".
*/
struct TrigEvent { int a; IdList * b; };

#line 33 "ext/sqlite/libsqlite/src/parse.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    sqliteParserTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is sqliteParserTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    sqliteParserARG_SDECL     A static variable declaration for the %extra_argument
**    sqliteParserARG_PDECL     A parameter declaration for the %extra_argument
**    sqliteParserARG_STORE     Code to store %extra_argument into yypParser
**    sqliteParserARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 221
#define YYACTIONTYPE unsigned short int
#define sqliteParserTOKENTYPE Token
typedef union {
  int yyinit;
  sqliteParserTOKENTYPE yy0;
  TriggerStep * yy19;
  struct LimitVal yy124;
  Select* yy179;
  Expr * yy182;
  Expr* yy242;
  struct TrigEvent yy290;
  SrcList* yy307;
  IdList* yy320;
  ExprList* yy322;
  int yy372;
  struct {int value; int mask;} yy407;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define sqliteParserARG_SDECL Parse *pParse;
#define sqliteParserARG_PDECL ,Parse *pParse
#define sqliteParserARG_FETCH Parse *pParse = yypParser->pParse
#define sqliteParserARG_STORE yypParser->pParse = pParse
#define YYNSTATE 563
#define YYNRULE 293
#define YYFALLBACK 1
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
#define YY_ACTTAB_COUNT (1090)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   186,  561,  483,   69,   67,   70,   68,   64,   63,   62,
 /*    10 */    61,   58,   57,   56,   55,   54,   53,  181,  180,  179,
 /*    20 */   514,  421,  334,  420,  468,  515,   64,   63,   62,   61,
 /*    30 */    58,   57,   56,   55,   54,   53,    9,  423,  422,   71,
 /*    40 */    72,  129,   65,   66,  513,  510,  305,   52,  138,   69,
 /*    50 */    67,   70,   68,   64,   63,   62,   61,   58,   57,   56,
 /*    60 */    55,   54,   53,  448,  469,  175,  482,  514,  470,  344,
 /*    70 */   342,   36,  515,   58,   57,   56,   55,   54,   53,    8,
 /*    80 */   341,  281,  285,  307,  437,  178,   71,   72,  129,   65,
 /*    90 */    66,  513,  510,  305,   52,  138,   69,   67,   70,   68,
 /*   100 */    64,   63,   62,   61,   58,   57,   56,   55,   54,   53,
 /*   110 */   130,  362,  360,  508,  507,  267,  551,  436,  298,  297,
 /*   120 */   369,  368,   50,  128,  543,   29,  266,  449,  537,  447,
 /*   130 */   591,  528,  442,  441,  187,  132,  514,  536,   47,   48,
 /*   140 */   472,  515,  122,  427,  331,  409,   49,  371,  370,  518,
 /*   150 */   328,  363,  517,  520,   45,   71,   72,  129,   65,   66,
 /*   160 */   513,  510,  305,   52,  138,   69,   67,   70,   68,   64,
 /*   170 */    63,   62,   61,   58,   57,   56,   55,   54,   53,  185,
 /*   180 */   550,  549,  512,  175,  467,  516,   18,  344,  342,   36,
 /*   190 */   544,  175,  320,  230,  231,  344,  342,   36,  341,   56,
 /*   200 */    55,   54,   53,  212,  531,  514,  341,  551,    3,  213,
 /*   210 */   515,    2,  551,   73,    7,  551,  184,  132,  551,  172,
 /*   220 */   551,  309,  348,   42,   71,   72,  129,   65,   66,  513,
 /*   230 */   510,  305,   52,  138,   69,   67,   70,   68,   64,   63,
 /*   240 */    62,   61,   58,   57,   56,   55,   54,   53,  243,  197,
 /*   250 */   282,  358,  268,  373,  264,  372,  183,  241,  436,  169,
 /*   260 */   356,  171,  269,  240,  471,  426,   29,  446,  506,  514,
 /*   270 */   445,  550,  549,  494,  515,  354,  550,  549,  359,  550,
 /*   280 */   549,  144,  550,  549,  550,  549,  592,  309,   71,   72,
 /*   290 */   129,   65,   66,  513,  510,  305,   52,  138,   69,   67,
 /*   300 */    70,   68,   64,   63,   62,   61,   58,   57,   56,   55,
 /*   310 */    54,   53,  514,  857,   82,  377,    1,  515,  268,  373,
 /*   320 */   264,  372,  183,  241,  362,   12,  508,  507,  500,  240,
 /*   330 */    17,   71,   72,  129,   65,   66,  513,  510,  305,   52,
 /*   340 */   138,   69,   67,   70,   68,   64,   63,   62,   61,   58,
 /*   350 */    57,   56,   55,   54,   53,  362,  182,  508,  507,  514,
 /*   360 */   362,  527,  508,  507,  515,  563,  429,  463,  182,  444,
 /*   370 */   375,  338,  443,  430,  379,  378,  593,  156,   71,   72,
 /*   380 */   129,   65,   66,  513,  510,  305,   52,  138,   69,   67,
 /*   390 */    70,   68,   64,   63,   62,   61,   58,   57,   56,   55,
 /*   400 */    54,   53,  514,  526,  542,  450,  534,  515,  286,  493,
 /*   410 */   453,   17,  478,  240,   80,   11,  533,  153,  194,  155,
 /*   420 */   286,   71,   51,  129,   65,   66,  513,  510,  305,   52,
 /*   430 */   138,   69,   67,   70,   68,   64,   63,   62,   61,   58,
 /*   440 */    57,   56,   55,   54,   53,  514,  195,  466,  160,   17,
 /*   450 */   515,  454,  490,   80,  459,  440,  460,  176,  239,  238,
 /*   460 */    80,   80,  562,    1,   71,   40,  129,   65,   66,  513,
 /*   470 */   510,  305,   52,  138,   69,   67,   70,   68,   64,   63,
 /*   480 */    62,   61,   58,   57,   56,   55,   54,   53,  514,  365,
 /*   490 */   154,   19,  339,  515,   80,  232,  405,   80,  165,  404,
 /*   500 */   193,   32,  396,   13,   32,   86,  414,  108,   72,  129,
 /*   510 */    65,   66,  513,  510,  305,   52,  138,   69,   67,   70,
 /*   520 */    68,   64,   63,   62,   61,   58,   57,   56,   55,   54,
 /*   530 */    53,  514,  551,  365,  483,  192,  515,  488,  323,  207,
 /*   540 */   366,  249,  177,  186,   87,  483,  483,   46,   38,   44,
 /*   550 */   458,  108,  129,   65,   66,  513,  510,  305,   52,  138,
 /*   560 */    69,   67,   70,   68,   64,   63,   62,   61,   58,   57,
 /*   570 */    56,   55,   54,   53,  274,  457,  272,  271,  270,   23,
 /*   580 */     8,  551,  211,  412,  307,  257,  365,  385,  201,   31,
 /*   590 */   217,  388,  141,  205,  387,  219,  550,  549,  482,  511,
 /*   600 */   215,  376,  560,  134,   90,  477,  214,  514,  392,  482,
 /*   610 */   482,  152,  515,  360,  203,  212,  409,  531,  800,  284,
 /*   620 */   365,  145,  505,   50,  300,  365,  365,  173,  321,  212,
 /*   630 */   487,  137,  135,    8,   41,  136,  531,  307,   93,   47,
 /*   640 */    48,  346,  316,  106,  106,  550,  549,   49,  371,  370,
 /*   650 */   518,  509,  531,  517,  520,  504,  531,  531,  162,  495,
 /*   660 */   170,  317,  503,  319,  223,  231,  360,  551,  502,  283,
 /*   670 */   162,  207,  557,  486,  212,  191,   50,   10,  289,  304,
 /*   680 */   303,  556,  207,  531,    8,  531,  516,   18,  307,  498,
 /*   690 */   498,  189,   47,   48,  393,  531,  555,   28,  302,  554,
 /*   700 */    49,  371,  370,  518,  484,  480,  517,  520,  322,  299,
 /*   710 */   553,  418,  365,  323,   17,  365,  365,  360,  416,  207,
 /*   720 */   322,  417,  207,  418,  327,  212,  480,   50,  207,  326,
 /*   730 */   106,  550,  549,  106,  105,  247,  407,  475,  332,  516,
 /*   740 */    18,  326,  365,   47,   48,  207,  295,  365,  475,  294,
 /*   750 */   158,   49,  371,  370,  518,  293,  473,  517,  520,  485,
 /*   760 */   106,  391,  390,  202,  148,   93,  351,  480,  204,  301,
 /*   770 */   333,  190,  291,  541,   60,  531,  498,  252,  453,  498,
 /*   780 */   365,  365,  290,  365,  501,  475,  365,   79,  475,  531,
 /*   790 */   516,   18,  379,  378,  475,  365,  465,  245,   89,  112,
 /*   800 */   365,  109,  365,  131,  121,  288,  499,  365,  365,  439,
 /*   810 */   365,  475,  365,  120,  365,  365,  343,  365,  119,  365,
 /*   820 */   118,  365,  365,  365,  365,  117,  116,  365,  126,  365,
 /*   830 */   125,  365,  124,  123,  365,  115,  365,  114,  431,  140,
 /*   840 */   139,  255,  254,  365,  365,  253,  365,  280,  365,  107,
 /*   850 */   365,  365,  113,  365,  111,   26,  365,  365,  365,  365,
 /*   860 */   365,  279,  278,  365,  277,  365,   92,  365,  104,  103,
 /*   870 */   365,   91,  365,  365,  102,  101,  110,  100,   99,  347,
 /*   880 */    25,   98,  340,   30,   24,   97,  266,  174,   96,   85,
 /*   890 */    95,   94,  166,  292,   78,  165,  415,   14,  163,   60,
 /*   900 */   164,   22,    6,  408,    5,   77,   34,   33,  159,   16,
 /*   910 */   157,  151,   75,  149,   15,  146,  313,  312,  395,  384,
 /*   920 */   143,   20,   60,  206,   21,  273,  198,  559,  375,  548,
 /*   930 */   547,  546,  374,    4,  540,  539,  538,  308,  535,  532,
 /*   940 */   530,  212,  261,   38,  260,  352,  259,   39,  258,  367,
 /*   950 */   529,  196,  210,  256,  521,  522,   53,   53,  209,   43,
 /*   960 */   496,  188,  492,  208,  256,   81,  246,   37,  479,  349,
 /*   970 */   244,   37,  474,  464,  276,   27,  452,  451,  433,  432,
 /*   980 */   275,  235,  234,  335,  424,   35,  329,  413,  410,  127,
 /*   990 */   161,   84,   76,  403,   38,  400,  188,  399,  224,  398,
 /*  1000 */    38,  150,  318,  220,   83,  147,  315,  200,  381,  383,
 /*  1010 */   199,  142,  545,  265,   88,  262,  523,  361,  491,  476,
 /*  1020 */   463,  406,  397,  287,  389,  386,  310,  382,  552,   74,
 /*  1030 */   306,  525,  524,  364,  519,  357,  355,  353,  497,  489,
 /*  1040 */   481,  263,  242,  462,  461,  456,  455,  438,  296,  345,
 /*  1050 */   434,  237,  425,  337,  168,  167,  336,  236,  419,  330,
 /*  1060 */   233,  325,  324,  229,  228,  402,  401,  227,  226,  225,
 /*  1070 */   222,  221,  218,  314,  394,  311,  216,  380,  251,  250,
 /*  1080 */   133,  350,  248,  364,  558,   59,  435,  411,  428,  212,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    21,    9,   23,   70,   71,   72,   73,   74,   75,   76,
 /*    10 */    77,   78,   79,   80,   81,   82,   83,  100,  101,  102,
 /*    20 */    41,  100,  101,  102,   20,   46,   74,   75,   76,   77,
 /*    30 */    78,   79,   80,   81,   82,   83,   19,   55,   56,   60,
 /*    40 */    61,   62,   63,   64,   65,   66,   67,   68,   69,   70,
 /*    50 */    71,   72,   73,   74,   75,   76,   77,   78,   79,   80,
 /*    60 */    81,   82,   83,   23,  108,   90,   87,   41,  112,   94,
 /*    70 */    95,   96,   46,   78,   79,   80,   81,   82,   83,   19,
 /*    80 */   105,  149,  143,   23,  152,  153,   60,   61,   62,   63,
 /*    90 */    64,   65,   66,   67,   68,   69,   70,   71,   72,   73,
 /*   100 */    74,   75,   76,   77,   78,   79,   80,   81,   82,   83,
 /*   110 */    31,  107,   52,  109,  110,   93,   23,  140,   78,   79,
 /*   120 */    78,   79,   62,   22,  147,  148,  104,   87,   34,   89,
 /*   130 */   113,   89,   92,   93,  183,  184,   41,   43,   78,   79,
 /*   140 */    80,   46,  165,  166,  205,   53,   86,   87,   88,   89,
 /*   150 */   211,   62,   92,   93,  128,   60,   61,   62,   63,   64,
 /*   160 */    65,   66,   67,   68,   69,   70,   71,   72,   73,   74,
 /*   170 */    75,   76,   77,   78,   79,   80,   81,   82,   83,  146,
 /*   180 */    87,   88,   93,   90,   20,  125,  126,   94,   95,   96,
 /*   190 */    20,   90,  100,  101,  102,   94,   95,   96,  105,   80,
 /*   200 */    81,   82,   83,  111,  171,   41,  105,   23,   19,   48,
 /*   210 */    46,   19,   23,   19,   19,   23,  183,  184,   23,   17,
 /*   220 */    23,   62,  189,  128,   60,   61,   62,   63,   64,   65,
 /*   230 */    66,   67,   68,   69,   70,   71,   72,   73,   74,   75,
 /*   240 */    76,   77,   78,   79,   80,   81,   82,   83,   20,   90,
 /*   250 */    91,   15,   93,   94,   95,   96,   97,   98,  140,   57,
 /*   260 */    24,   59,  144,  104,   80,  147,  148,   89,   20,   41,
 /*   270 */    92,   87,   88,   20,   46,   39,   87,   88,   42,   87,
 /*   280 */    88,   19,   87,   88,   87,   88,  113,   62,   60,   61,
 /*   290 */    62,   63,   64,   65,   66,   67,   68,   69,   70,   71,
 /*   300 */    72,   73,   74,   75,   76,   77,   78,   79,   80,   81,
 /*   310 */    82,   83,   41,  132,  133,  134,  135,   46,   93,   94,
 /*   320 */    95,   96,   97,   98,  107,   63,  109,  110,   20,  104,
 /*   330 */    22,   60,   61,   62,   63,   64,   65,   66,   67,   68,
 /*   340 */    69,   70,   71,   72,   73,   74,   75,   76,   77,   78,
 /*   350 */    79,   80,   81,   82,   83,  107,   47,  109,  110,   41,
 /*   360 */   107,   89,  109,  110,   46,    0,  161,  162,   47,   89,
 /*   370 */    99,   62,   92,  168,    9,   10,  113,   17,   60,   61,
 /*   380 */    62,   63,   64,   65,   66,   67,   68,   69,   70,   71,
 /*   390 */    72,   73,   74,   75,   76,   77,   78,   79,   80,   81,
 /*   400 */    82,   83,   41,   89,  155,  156,   26,   46,   99,   20,
 /*   410 */   161,   22,   20,  104,   22,  118,   36,   57,   22,   59,
 /*   420 */    99,   60,   61,   62,   63,   64,   65,   66,   67,   68,
 /*   430 */    69,   70,   71,   72,   73,   74,   75,   76,   77,   78,
 /*   440 */    79,   80,   81,   82,   83,   41,   50,   20,   22,   22,
 /*   450 */    46,   20,   22,   22,   91,   20,   93,   22,   20,   20,
 /*   460 */    22,   22,  134,  135,   60,   61,   62,   63,   64,   65,
 /*   470 */    66,   67,   68,   69,   70,   71,   72,   73,   74,   75,
 /*   480 */    76,   77,   78,   79,   80,   81,   82,   83,   41,  140,
 /*   490 */   130,   22,   20,   46,   22,   20,   20,   22,   22,   20,
 /*   500 */   113,   22,   20,   19,   22,   21,   18,  158,   61,   62,
 /*   510 */    63,   64,   65,   66,   67,   68,   69,   70,   71,   72,
 /*   520 */    73,   74,   75,   76,   77,   78,   79,   80,   81,   82,
 /*   530 */    83,   41,   23,  140,   23,  113,   46,   22,  140,  140,
 /*   540 */   191,  192,   19,   21,  114,   23,   23,  127,  122,  129,
 /*   550 */    29,  158,   62,   63,   64,   65,   66,   67,   68,   69,
 /*   560 */    70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
 /*   570 */    80,   81,   82,   83,   11,   54,   13,   14,   15,   16,
 /*   580 */    19,   23,  174,   95,   23,  192,  140,   78,   79,  181,
 /*   590 */    27,   89,  146,  195,   92,   32,   87,   88,   87,   93,
 /*   600 */    37,  136,  137,   88,  158,  206,  141,   41,   99,   87,
 /*   610 */    87,  146,   46,   52,   51,  111,   53,  171,  130,   19,
 /*   620 */   140,   58,   14,   62,  103,  140,  140,  146,  124,  111,
 /*   630 */   115,  146,  146,   19,   68,   69,  171,   23,  158,   78,
 /*   640 */    79,   80,  124,  158,  158,   87,   88,   86,   87,   88,
 /*   650 */    89,  108,  171,   92,   93,   20,  171,  171,  146,   93,
 /*   660 */   146,  196,   20,  100,  101,  102,   52,   23,   20,  106,
 /*   670 */   146,  140,   15,  115,  111,   22,   62,  118,  198,  194,
 /*   680 */   194,   24,  140,  171,   19,  171,  125,  126,   23,  204,
 /*   690 */   204,   22,   78,   79,  140,  171,   39,   19,  167,   42,
 /*   700 */    86,   87,   88,   89,  115,  152,   92,   93,  196,  167,
 /*   710 */    53,  140,  140,  140,   22,  140,  140,   52,   25,  140,
 /*   720 */   196,   28,  140,  140,  212,  111,  152,   62,  140,  217,
 /*   730 */   158,   87,   88,  158,  158,  182,  212,  206,   45,  125,
 /*   740 */   126,  217,  140,   78,   79,  140,  167,  140,  206,  167,
 /*   750 */   146,   86,   87,   88,   89,  167,  182,   92,   93,  115,
 /*   760 */   158,  207,  208,  209,  146,  158,  194,  152,  195,  194,
 /*   770 */   199,   22,  167,  156,  200,  171,  204,  201,  161,  204,
 /*   780 */   140,  140,  199,  140,   20,  206,  140,   20,  206,  171,
 /*   790 */   125,  126,    9,   10,  206,  140,   20,  182,  158,  158,
 /*   800 */   140,  158,  140,  113,  158,  198,  204,  140,  140,   20,
 /*   810 */   140,  206,  140,  158,  140,  140,   48,  140,  158,  140,
 /*   820 */   158,  140,  140,  140,  140,  158,  158,  140,  158,  140,
 /*   830 */   158,  140,  158,  158,  140,  158,  140,  158,  139,  158,
 /*   840 */   158,  158,  158,  140,  140,  158,  140,  158,  140,  158,
 /*   850 */   140,  140,  158,  140,  158,   19,  140,  140,  140,  140,
 /*   860 */   140,  158,  158,  140,  158,  140,  158,  140,  158,  158,
 /*   870 */   140,  158,  140,  140,  158,  158,  158,  158,  158,  140,
 /*   880 */    19,  158,   48,  158,   19,  158,  104,   97,  158,   21,
 /*   890 */   158,  158,   99,   38,   49,   22,   49,  158,   99,  200,
 /*   900 */   130,   19,   11,   14,    9,  103,   63,   63,  123,   19,
 /*   910 */   114,  114,  103,  123,   19,  114,  116,   35,   87,   20,
 /*   920 */    21,  150,  200,  160,  160,  138,   12,  139,   99,  138,
 /*   930 */   138,  138,  145,   22,  139,  139,  164,   44,  139,  139,
 /*   940 */   171,  111,  176,  122,  177,  119,  178,  120,  179,  117,
 /*   950 */   180,  121,  193,   98,  151,   23,   83,   83,  202,  127,
 /*   960 */   186,  113,  186,  193,   98,  186,  187,   99,  188,  116,
 /*   970 */   187,   99,  188,  139,  159,   19,  151,  164,  139,  139,
 /*   980 */   159,  186,  215,   40,  216,  127,  186,  139,  169,   60,
 /*   990 */   169,  197,   19,  176,  122,  186,  113,  186,  186,  176,
 /*  1000 */   122,  169,  186,  186,  197,  169,  186,  218,   33,  219,
 /*  1010 */   116,  218,  142,  157,  173,  175,  157,  203,  157,  157,
 /*  1020 */   162,  176,  176,  152,  210,  210,  152,  152,  140,  140,
 /*  1030 */   154,  154,  154,  140,  140,  140,  140,  140,  140,  185,
 /*  1040 */   140,  172,  140,  140,  163,  163,  163,  152,  154,  154,
 /*  1050 */   140,  140,  140,  140,  140,  213,  214,  140,  140,  140,
 /*  1060 */   140,  140,  140,  140,  140,  140,  140,  140,  140,  140,
 /*  1070 */   140,  140,  140,  140,  140,  140,  140,  140,  140,  140,
 /*  1080 */   140,  140,  140,  140,  170,  200,  166,  170,  166,  111,
};
#define YY_SHIFT_USE_DFLT (-84)
#define YY_SHIFT_COUNT (376)
#define YY_SHIFT_MIN   (-83)
#define YY_SHIFT_MAX   (978)
static const short yy_shift_ofst[] = {
 /*     0 */   783,  563,  614,  614,   93,   92,   92,  978,  614,  561,
 /*    10 */   665,  665,  509,  197,  -21,  665,  665,  665,  665,  665,
 /*    20 */   159,  309,  197,  488,  197,  197,  197,  197,  197,  511,
 /*    30 */   271,   60,  665,  665,  665,  665,  665,  665,  665,  665,
 /*    40 */   665,  665,  665,  665,  665,  665,  665,  665,  665,  665,
 /*    50 */   665,  665,  665,  665,  665,  665,  665,  665,  665,  665,
 /*    60 */   665,  665,  665,  665,  665,  665,  665,  665,  665,  665,
 /*    70 */   665,  665,  665,  665,  225,  197,  197,  197,  197,  522,
 /*    80 */   197,  522,  365,  518,  504,  978,  978,  -84,  -84,  228,
 /*    90 */   164,   95,   26,  318,  318,  318,  318,  318,  318,  318,
 /*   100 */   318,  404,  318,  318,  318,  318,  318,  361,  318,  447,
 /*   110 */   490,  490,  490,  -67,  -67,  -67,  -67,  -67,  -48,  -48,
 /*   120 */   -48,  -48,  101,   -5,   -5,   -5,   -5,  657,  -25,  566,
 /*   130 */   657,  184,  195,  644,  558,  253,  192,  248,  189,  119,
 /*   140 */   119,    4,  197,  197,  197,  197,  197,  197,  217,  197,
 /*   150 */   197,  197,  217,  197,  197,  197,  197,  197,  217,  197,
 /*   160 */   197,  197,  217,  197,  197,  197,  197,  -79,  693,  197,
 /*   170 */   217,  197,  197,  217,  197,  197,   42,   42,  523,  521,
 /*   180 */   521,  521,  197,  197,  515,  217,  197,  515,  197,  197,
 /*   190 */   197,  197,  197,  197,   42,   42,   42,  197,  197,  511,
 /*   200 */   511,  502,  502,  511,  426,  426,  321,  380,  380,  420,
 /*   210 */   380,  430,  -44,  380,  484,  975,  894,  975,  883,  929,
 /*   220 */   973,  883,  883,  929,  878,  883,  883,  883,  872,  973,
 /*   230 */   929,  929,  829,  848,  858,  943,  848,  956,  829,  829,
 /*   240 */   893,  932,  956,  829,  853,  872,  853,  868,  848,  866,
 /*   250 */   848,  848,  832,  874,  874,  873,  932,  855,  830,  832,
 /*   260 */   827,  826,  821,  830,  829,  829,  893,  829,  829,  911,
 /*   270 */   914,  914,  914,  829,  914,  -84,  -84,  -84,  -84,  -84,
 /*   280 */   -84,  -84,   40,  360,  236,  202,  -83,  262,  482,  479,
 /*   290 */   476,  475,  -18,  472,  439,  438,  435,  280,  178,  431,
 /*   300 */   363,  427,  392,  389,  308,   89,  396,   17,   94,   22,
 /*   310 */   899,  899,  831,  882,  800,  801,  895,  790,  809,  797,
 /*   320 */   796,  890,  785,  844,  843,  802,  895,  889,  891,  882,
 /*   330 */   799,  770,  847,  873,  845,  855,  793,  868,  782,  790,
 /*   340 */   865,  834,  861,  836,  768,  789,  776,  690,  767,  678,
 /*   350 */   589,  692,  559,  764,  669,  648,  749,  642,  653,  635,
 /*   360 */   600,  608,  543,  506,  422,  387,  469,  297,  314,  272,
 /*   370 */   263,  173,  194,  161,  170,   79,   -8,
};
#define YY_REDUCE_USE_DFLT (-69)
#define YY_REDUCE_COUNT (281)
#define YY_REDUCE_MIN   (-68)
#define YY_REDUCE_MAX   (943)
static const short yy_reduce_ofst[] = {
 /*     0 */   181,  465,  486,  485,  -23,  524,  512,   33,  446,  575,
 /*    10 */   572,  349,  554,  118,  574,  607,  480,  602,  576,  393,
 /*    20 */   249,  205,  605,  -61,  588,  582,  579,  542,  531,  -68,
 /*    30 */   699,  739,  733,  732,  730,  727,  725,  723,  720,  719,
 /*    40 */   718,  717,  716,  713,  711,  710,  708,  706,  704,  703,
 /*    50 */   696,  694,  691,  689,  687,  684,  683,  682,  681,  679,
 /*    60 */   677,  675,  674,  672,  670,  668,  667,  662,  660,  655,
 /*    70 */   646,  643,  641,  640,  617,  573,  583,  398,  571,  615,
 /*    80 */   399,  553,  328,  618,  604,  514,  481,  -49,  408,  722,
 /*    90 */   722,  722,  722,  722,  722,  722,  722,  722,  722,  722,
 /*   100 */   722,  722,  722,  722,  722,  722,  722,  722,  722,  722,
 /*   110 */   722,  722,  722,  722,  722,  722,  722,  722,  722,  722,
 /*   120 */   722,  722,  922,  722,  722,  722,  722,  917,  920,  885,
 /*   130 */   914,  943,  942,  941,  940,  869,  939,  869,  938,  722,
 /*   140 */   722,  869,  937,  936,  935,  934,  933,  932,  869,  931,
 /*   150 */   930,  929,  869,  928,  927,  926,  925,  924,  869,  923,
 /*   160 */   922,  921,  869,  920,  919,  918,  917,  842,  842,  914,
 /*   170 */   869,  913,  912,  869,  911,  910,  895,  894,  895,  883,
 /*   180 */   882,  881,  903,  902,  854,  869,  900,  854,  898,  897,
 /*   190 */   896,  895,  894,  893,  878,  877,  876,  889,  888,  875,
 /*   200 */   874,  815,  814,  871,  846,  845,  858,  862,  861,  814,
 /*   210 */   859,  840,  841,  856,  870,  793,  790,  789,  820,  836,
 /*   220 */   807,  817,  816,  832,  823,  812,  811,  809,  817,  794,
 /*   230 */   821,  819,  848,  800,  768,  767,  795,  821,  840,  839,
 /*   240 */   813,  825,  815,  834,  784,  783,  780,  779,  779,  770,
 /*   250 */   776,  774,  756,  722,  722,  722,  803,  759,  770,  769,
 /*   260 */   768,  767,  766,  769,  800,  799,  772,  796,  795,  787,
 /*   270 */   793,  792,  791,  788,  787,  764,  763,  722,  722,  722,
 /*   280 */   722,  771,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   570,  856,  797,  797,  856,  839,  839,  685,  856,  797,
 /*    10 */   797,  856,  822,  856,  681,  856,  856,  797,  793,  856,
 /*    20 */   586,  649,  856,  581,  856,  856,  856,  856,  856,  594,
 /*    30 */   651,  856,  856,  856,  856,  856,  856,  856,  856,  856,
 /*    40 */   856,  856,  856,  856,  856,  856,  856,  856,  856,  856,
 /*    50 */   856,  856,  856,  856,  856,  856,  856,  856,  856,  856,
 /*    60 */   856,  856,  856,  856,  856,  856,  856,  856,  856,  856,
 /*    70 */   856,  856,  856,  856,  856,  856,  856,  856,  856,  681,
 /*    80 */   856,  681,  570,  856,  856,  856,  856,  685,  675,  856,
 /*    90 */   856,  856,  856,  730,  729,  724,  723,  837,  697,  721,
 /*   100 */   714,  856,  789,  790,  788,  792,  796,  856,  705,  748,
 /*   110 */   780,  774,  747,  779,  760,  759,  754,  753,  752,  751,
 /*   120 */   750,  749,  640,  758,  757,  756,  755,  856,  856,  856,
 /*   130 */   856,  856,  856,  856,  856,  856,  856,  856,  856,  764,
 /*   140 */   763,  856,  856,  856,  856,  809,  856,  856,  726,  856,
 /*   150 */   856,  856,  663,  856,  856,  856,  856,  856,  842,  856,
 /*   160 */   856,  856,  844,  856,  856,  856,  856,  856,  828,  856,
 /*   170 */   661,  856,  856,  583,  856,  856,  856,  856,  595,  856,
 /*   180 */   856,  856,  856,  856,  689,  688,  856,  683,  856,  856,
 /*   190 */   856,  856,  856,  856,  856,  856,  856,  856,  573,  856,
 /*   200 */   856,  856,  856,  856,  720,  720,  621,  708,  708,  791,
 /*   210 */   708,  682,  673,  708,  856,  854,  852,  854,  690,  653,
 /*   220 */   731,  690,  690,  653,  720,  690,  690,  690,  720,  731,
 /*   230 */   653,  653,  651,  690,  836,  833,  690,  801,  651,  651,
 /*   240 */   636,  856,  801,  651,  700,  698,  700,  698,  690,  709,
 /*   250 */   690,  690,  856,  767,  766,  765,  856,  709,  715,  701,
 /*   260 */   713,  711,  720,  856,  651,  651,  636,  651,  651,  639,
 /*   270 */   572,  572,  572,  651,  572,  624,  624,  777,  776,  775,
 /*   280 */   768,  604,  856,  856,  856,  856,  856,  816,  856,  856,
 /*   290 */   856,  856,  856,  856,  856,  856,  856,  856,  856,  856,
 /*   300 */   856,  856,  856,  856,  856,  856,  716,  737,  856,  856,
 /*   310 */   856,  856,  856,  856,  808,  856,  856,  856,  856,  856,
 /*   320 */   856,  856,  856,  856,  856,  856,  856,  856,  856,  856,
 /*   330 */   856,  856,  856,  832,  831,  856,  856,  856,  856,  856,
 /*   340 */   856,  856,  856,  856,  856,  856,  856,  856,  856,  856,
 /*   350 */   856,  712,  856,  856,  856,  856,  856,  856,  856,  856,
 /*   360 */   856,  856,  666,  856,  739,  856,  702,  856,  856,  856,
 /*   370 */   738,  743,  856,  856,  856,  856,  856,  565,  569,  567,
 /*   380 */   855,  853,  851,  850,  815,  821,  818,  820,  819,  817,
 /*   390 */   814,  813,  812,  811,  810,  807,  725,  722,  719,  849,
 /*   400 */   806,  662,  660,  843,  841,  732,  840,  838,  823,  728,
 /*   410 */   727,  654,  799,  798,  580,  827,  826,  825,  734,  733,
 /*   420 */   830,  829,  835,  834,  824,  579,  585,  643,  642,  650,
 /*   430 */   648,  647,  646,  645,  644,  641,  587,  598,  599,  597,
 /*   440 */   596,  615,  612,  614,  611,  613,  610,  609,  608,  607,
 /*   450 */   606,  635,  623,  622,  802,  629,  628,  633,  632,  631,
 /*   460 */   630,  627,  626,  625,  620,  746,  745,  735,  778,  672,
 /*   470 */   671,  678,  677,  676,  687,  804,  805,  803,  699,  686,
 /*   480 */   680,  679,  590,  589,  696,  695,  694,  693,  692,  684,
 /*   490 */   674,  704,  786,  783,  784,  772,  785,  691,  795,  794,
 /*   500 */   781,  848,  847,  846,  845,  787,  782,  669,  668,  667,
 /*   510 */   771,  773,  770,  769,  762,  761,  744,  742,  741,  740,
 /*   520 */   736,  710,  588,  703,  718,  717,  602,  601,  600,  670,
 /*   530 */   665,  664,  619,  707,  706,  618,  638,  637,  634,  617,
 /*   540 */   616,  605,  603,  584,  582,  578,  577,  576,  575,  593,
 /*   550 */   592,  591,  574,  659,  658,  657,  656,  655,  652,  571,
 /*   560 */   568,  566,  564,
};

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
    0,  /*          $ => nothing */
    0,  /* END_OF_FILE => nothing */
    0,  /*    ILLEGAL => nothing */
    0,  /*      SPACE => nothing */
    0,  /* UNCLOSED_STRING => nothing */
    0,  /*    COMMENT => nothing */
    0,  /*   FUNCTION => nothing */
    0,  /*     COLUMN => nothing */
    0,  /* AGG_FUNCTION => nothing */
    0,  /*       SEMI => nothing */
   23,  /*    EXPLAIN => ID */
   23,  /*      BEGIN => ID */
    0,  /* TRANSACTION => nothing */
    0,  /*     COMMIT => nothing */
   23,  /*        END => ID */
    0,  /*   ROLLBACK => nothing */
    0,  /*     CREATE => nothing */
    0,  /*      TABLE => nothing */
   23,  /*       TEMP => ID */
    0,  /*         LP => nothing */
    0,  /*         RP => nothing */
    0,  /*         AS => nothing */
    0,  /*      COMMA => nothing */
    0,  /*         ID => nothing */
   23,  /*      ABORT => ID */
   23,  /*      AFTER => ID */
   23,  /*        ASC => ID */
   23,  /*     ATTACH => ID */
   23,  /*     BEFORE => ID */
   23,  /*    CASCADE => ID */
   23,  /*    CLUSTER => ID */
   23,  /*   CONFLICT => ID */
   23,  /*       COPY => ID */
   23,  /*   DATABASE => ID */
   23,  /*   DEFERRED => ID */
   23,  /* DELIMITERS => ID */
   23,  /*       DESC => ID */
   23,  /*     DETACH => ID */
   23,  /*       EACH => ID */
   23,  /*       FAIL => ID */
   23,  /*        FOR => ID */
   23,  /*       GLOB => ID */
   23,  /*     IGNORE => ID */
   23,  /*  IMMEDIATE => ID */
   23,  /*  INITIALLY => ID */
   23,  /*    INSTEAD => ID */
   23,  /*       LIKE => ID */
   23,  /*      MATCH => ID */
   23,  /*        KEY => ID */
   23,  /*         OF => ID */
   23,  /*     OFFSET => ID */
   23,  /*     PRAGMA => ID */
   23,  /*      RAISE => ID */
   23,  /*    REPLACE => ID */
   23,  /*   RESTRICT => ID */
   23,  /*        ROW => ID */
   23,  /*  STATEMENT => ID */
   23,  /*    TRIGGER => ID */
   23,  /*     VACUUM => ID */
   23,  /*       VIEW => ID */
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  sqliteParserARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void sqliteParserTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "END_OF_FILE",   "ILLEGAL",       "SPACE",       
  "UNCLOSED_STRING",  "COMMENT",       "FUNCTION",      "COLUMN",      
  "AGG_FUNCTION",  "SEMI",          "EXPLAIN",       "BEGIN",       
  "TRANSACTION",   "COMMIT",        "END",           "ROLLBACK",    
  "CREATE",        "TABLE",         "TEMP",          "LP",          
  "RP",            "AS",            "COMMA",         "ID",          
  "ABORT",         "AFTER",         "ASC",           "ATTACH",      
  "BEFORE",        "CASCADE",       "CLUSTER",       "CONFLICT",    
  "COPY",          "DATABASE",      "DEFERRED",      "DELIMITERS",  
  "DESC",          "DETACH",        "EACH",          "FAIL",        
  "FOR",           "GLOB",          "IGNORE",        "IMMEDIATE",   
  "INITIALLY",     "INSTEAD",       "LIKE",          "MATCH",       
  "KEY",           "OF",            "OFFSET",        "PRAGMA",      
  "RAISE",         "REPLACE",       "RESTRICT",      "ROW",         
  "STATEMENT",     "TRIGGER",       "VACUUM",        "VIEW",        
  "OR",            "AND",           "NOT",           "EQ",          
  "NE",            "ISNULL",        "NOTNULL",       "IS",          
  "BETWEEN",       "IN",            "GT",            "GE",          
  "LT",            "LE",            "BITAND",        "BITOR",       
  "LSHIFT",        "RSHIFT",        "PLUS",          "MINUS",       
  "STAR",          "SLASH",         "REM",           "CONCAT",      
  "UMINUS",        "UPLUS",         "BITNOT",        "STRING",      
  "JOIN_KW",       "INTEGER",       "CONSTRAINT",    "DEFAULT",     
  "FLOAT",         "NULL",          "PRIMARY",       "UNIQUE",      
  "CHECK",         "REFERENCES",    "COLLATE",       "ON",          
  "DELETE",        "UPDATE",        "INSERT",        "SET",         
  "DEFERRABLE",    "FOREIGN",       "DROP",          "UNION",       
  "ALL",           "INTERSECT",     "EXCEPT",        "SELECT",      
  "DISTINCT",      "DOT",           "FROM",          "JOIN",        
  "USING",         "ORDER",         "BY",            "GROUP",       
  "HAVING",        "LIMIT",         "WHERE",         "INTO",        
  "VALUES",        "VARIABLE",      "CASE",          "WHEN",        
  "THEN",          "ELSE",          "INDEX",         "error",       
  "input",         "cmdlist",       "ecmd",          "explain",     
  "cmdx",          "cmd",           "trans_opt",     "onconf",      
  "nm",            "create_table",  "create_table_args",  "temp",        
  "columnlist",    "conslist_opt",  "select",        "column",      
  "columnid",      "type",          "carglist",      "id",          
  "ids",           "typename",      "signed",        "carg",        
  "ccons",         "sortorder",     "expr",          "idxlist_opt", 
  "refargs",       "defer_subclause",  "refarg",        "refact",      
  "init_deferred_pred_opt",  "conslist",      "tcons",         "idxlist",     
  "defer_subclause_opt",  "orconf",        "resolvetype",   "oneselect",   
  "multiselect_op",  "distinct",      "selcollist",    "from",        
  "where_opt",     "groupby_opt",   "having_opt",    "orderby_opt", 
  "limit_opt",     "sclp",          "as",            "seltablist",  
  "stl_prefix",    "joinop",        "dbnm",          "on_opt",      
  "using_opt",     "seltablist_paren",  "joinop2",       "sortlist",    
  "sortitem",      "collate",       "exprlist",      "setlist",     
  "insert_cmd",    "inscollist_opt",  "itemlist",      "inscollist",  
  "likeop",        "case_operand",  "case_exprlist",  "case_else",   
  "expritem",      "uniqueflag",    "idxitem",       "plus_num",    
  "minus_num",     "plus_opt",      "number",        "trigger_decl",
  "trigger_cmd_list",  "trigger_time",  "trigger_event",  "foreach_clause",
  "when_clause",   "trigger_cmd",   "database_kw_opt",  "key_opt",     
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "input ::= cmdlist",
 /*   1 */ "cmdlist ::= cmdlist ecmd",
 /*   2 */ "cmdlist ::= ecmd",
 /*   3 */ "ecmd ::= explain cmdx SEMI",
 /*   4 */ "ecmd ::= SEMI",
 /*   5 */ "cmdx ::= cmd",
 /*   6 */ "explain ::= EXPLAIN",
 /*   7 */ "explain ::=",
 /*   8 */ "cmd ::= BEGIN trans_opt onconf",
 /*   9 */ "trans_opt ::=",
 /*  10 */ "trans_opt ::= TRANSACTION",
 /*  11 */ "trans_opt ::= TRANSACTION nm",
 /*  12 */ "cmd ::= COMMIT trans_opt",
 /*  13 */ "cmd ::= END trans_opt",
 /*  14 */ "cmd ::= ROLLBACK trans_opt",
 /*  15 */ "cmd ::= create_table create_table_args",
 /*  16 */ "create_table ::= CREATE temp TABLE nm",
 /*  17 */ "temp ::= TEMP",
 /*  18 */ "temp ::=",
 /*  19 */ "create_table_args ::= LP columnlist conslist_opt RP",
 /*  20 */ "create_table_args ::= AS select",
 /*  21 */ "columnlist ::= columnlist COMMA column",
 /*  22 */ "columnlist ::= column",
 /*  23 */ "column ::= columnid type carglist",
 /*  24 */ "columnid ::= nm",
 /*  25 */ "id ::= ID",
 /*  26 */ "ids ::= ID",
 /*  27 */ "ids ::= STRING",
 /*  28 */ "nm ::= ID",
 /*  29 */ "nm ::= STRING",
 /*  30 */ "nm ::= JOIN_KW",
 /*  31 */ "type ::=",
 /*  32 */ "type ::= typename",
 /*  33 */ "type ::= typename LP signed RP",
 /*  34 */ "type ::= typename LP signed COMMA signed RP",
 /*  35 */ "typename ::= ids",
 /*  36 */ "typename ::= typename ids",
 /*  37 */ "signed ::= INTEGER",
 /*  38 */ "signed ::= PLUS INTEGER",
 /*  39 */ "signed ::= MINUS INTEGER",
 /*  40 */ "carglist ::= carglist carg",
 /*  41 */ "carglist ::=",
 /*  42 */ "carg ::= CONSTRAINT nm ccons",
 /*  43 */ "carg ::= ccons",
 /*  44 */ "carg ::= DEFAULT STRING",
 /*  45 */ "carg ::= DEFAULT ID",
 /*  46 */ "carg ::= DEFAULT INTEGER",
 /*  47 */ "carg ::= DEFAULT PLUS INTEGER",
 /*  48 */ "carg ::= DEFAULT MINUS INTEGER",
 /*  49 */ "carg ::= DEFAULT FLOAT",
 /*  50 */ "carg ::= DEFAULT PLUS FLOAT",
 /*  51 */ "carg ::= DEFAULT MINUS FLOAT",
 /*  52 */ "carg ::= DEFAULT NULL",
 /*  53 */ "ccons ::= NULL onconf",
 /*  54 */ "ccons ::= NOT NULL onconf",
 /*  55 */ "ccons ::= PRIMARY KEY sortorder onconf",
 /*  56 */ "ccons ::= UNIQUE onconf",
 /*  57 */ "ccons ::= CHECK LP expr RP onconf",
 /*  58 */ "ccons ::= REFERENCES nm idxlist_opt refargs",
 /*  59 */ "ccons ::= defer_subclause",
 /*  60 */ "ccons ::= COLLATE id",
 /*  61 */ "refargs ::=",
 /*  62 */ "refargs ::= refargs refarg",
 /*  63 */ "refarg ::= MATCH nm",
 /*  64 */ "refarg ::= ON DELETE refact",
 /*  65 */ "refarg ::= ON UPDATE refact",
 /*  66 */ "refarg ::= ON INSERT refact",
 /*  67 */ "refact ::= SET NULL",
 /*  68 */ "refact ::= SET DEFAULT",
 /*  69 */ "refact ::= CASCADE",
 /*  70 */ "refact ::= RESTRICT",
 /*  71 */ "defer_subclause ::= NOT DEFERRABLE init_deferred_pred_opt",
 /*  72 */ "defer_subclause ::= DEFERRABLE init_deferred_pred_opt",
 /*  73 */ "init_deferred_pred_opt ::=",
 /*  74 */ "init_deferred_pred_opt ::= INITIALLY DEFERRED",
 /*  75 */ "init_deferred_pred_opt ::= INITIALLY IMMEDIATE",
 /*  76 */ "conslist_opt ::=",
 /*  77 */ "conslist_opt ::= COMMA conslist",
 /*  78 */ "conslist ::= conslist COMMA tcons",
 /*  79 */ "conslist ::= conslist tcons",
 /*  80 */ "conslist ::= tcons",
 /*  81 */ "tcons ::= CONSTRAINT nm",
 /*  82 */ "tcons ::= PRIMARY KEY LP idxlist RP onconf",
 /*  83 */ "tcons ::= UNIQUE LP idxlist RP onconf",
 /*  84 */ "tcons ::= CHECK expr onconf",
 /*  85 */ "tcons ::= FOREIGN KEY LP idxlist RP REFERENCES nm idxlist_opt refargs defer_subclause_opt",
 /*  86 */ "defer_subclause_opt ::=",
 /*  87 */ "defer_subclause_opt ::= defer_subclause",
 /*  88 */ "onconf ::=",
 /*  89 */ "onconf ::= ON CONFLICT resolvetype",
 /*  90 */ "orconf ::=",
 /*  91 */ "orconf ::= OR resolvetype",
 /*  92 */ "resolvetype ::= ROLLBACK",
 /*  93 */ "resolvetype ::= ABORT",
 /*  94 */ "resolvetype ::= FAIL",
 /*  95 */ "resolvetype ::= IGNORE",
 /*  96 */ "resolvetype ::= REPLACE",
 /*  97 */ "cmd ::= DROP TABLE nm",
 /*  98 */ "cmd ::= CREATE temp VIEW nm AS select",
 /*  99 */ "cmd ::= DROP VIEW nm",
 /* 100 */ "cmd ::= select",
 /* 101 */ "select ::= oneselect",
 /* 102 */ "select ::= select multiselect_op oneselect",
 /* 103 */ "multiselect_op ::= UNION",
 /* 104 */ "multiselect_op ::= UNION ALL",
 /* 105 */ "multiselect_op ::= INTERSECT",
 /* 106 */ "multiselect_op ::= EXCEPT",
 /* 107 */ "oneselect ::= SELECT distinct selcollist from where_opt groupby_opt having_opt orderby_opt limit_opt",
 /* 108 */ "distinct ::= DISTINCT",
 /* 109 */ "distinct ::= ALL",
 /* 110 */ "distinct ::=",
 /* 111 */ "sclp ::= selcollist COMMA",
 /* 112 */ "sclp ::=",
 /* 113 */ "selcollist ::= sclp expr as",
 /* 114 */ "selcollist ::= sclp STAR",
 /* 115 */ "selcollist ::= sclp nm DOT STAR",
 /* 116 */ "as ::= AS nm",
 /* 117 */ "as ::= ids",
 /* 118 */ "as ::=",
 /* 119 */ "from ::=",
 /* 120 */ "from ::= FROM seltablist",
 /* 121 */ "stl_prefix ::= seltablist joinop",
 /* 122 */ "stl_prefix ::=",
 /* 123 */ "seltablist ::= stl_prefix nm dbnm as on_opt using_opt",
 /* 124 */ "seltablist ::= stl_prefix LP seltablist_paren RP as on_opt using_opt",
 /* 125 */ "seltablist_paren ::= select",
 /* 126 */ "seltablist_paren ::= seltablist",
 /* 127 */ "dbnm ::=",
 /* 128 */ "dbnm ::= DOT nm",
 /* 129 */ "joinop ::= COMMA",
 /* 130 */ "joinop ::= JOIN",
 /* 131 */ "joinop ::= JOIN_KW JOIN",
 /* 132 */ "joinop ::= JOIN_KW nm JOIN",
 /* 133 */ "joinop ::= JOIN_KW nm nm JOIN",
 /* 134 */ "on_opt ::= ON expr",
 /* 135 */ "on_opt ::=",
 /* 136 */ "using_opt ::= USING LP idxlist RP",
 /* 137 */ "using_opt ::=",
 /* 138 */ "orderby_opt ::=",
 /* 139 */ "orderby_opt ::= ORDER BY sortlist",
 /* 140 */ "sortlist ::= sortlist COMMA sortitem collate sortorder",
 /* 141 */ "sortlist ::= sortitem collate sortorder",
 /* 142 */ "sortitem ::= expr",
 /* 143 */ "sortorder ::= ASC",
 /* 144 */ "sortorder ::= DESC",
 /* 145 */ "sortorder ::=",
 /* 146 */ "collate ::=",
 /* 147 */ "collate ::= COLLATE id",
 /* 148 */ "groupby_opt ::=",
 /* 149 */ "groupby_opt ::= GROUP BY exprlist",
 /* 150 */ "having_opt ::=",
 /* 151 */ "having_opt ::= HAVING expr",
 /* 152 */ "limit_opt ::=",
 /* 153 */ "limit_opt ::= LIMIT signed",
 /* 154 */ "limit_opt ::= LIMIT signed OFFSET signed",
 /* 155 */ "limit_opt ::= LIMIT signed COMMA signed",
 /* 156 */ "cmd ::= DELETE FROM nm dbnm where_opt",
 /* 157 */ "where_opt ::=",
 /* 158 */ "where_opt ::= WHERE expr",
 /* 159 */ "cmd ::= UPDATE orconf nm dbnm SET setlist where_opt",
 /* 160 */ "setlist ::= setlist COMMA nm EQ expr",
 /* 161 */ "setlist ::= nm EQ expr",
 /* 162 */ "cmd ::= insert_cmd INTO nm dbnm inscollist_opt VALUES LP itemlist RP",
 /* 163 */ "cmd ::= insert_cmd INTO nm dbnm inscollist_opt select",
 /* 164 */ "insert_cmd ::= INSERT orconf",
 /* 165 */ "insert_cmd ::= REPLACE",
 /* 166 */ "itemlist ::= itemlist COMMA expr",
 /* 167 */ "itemlist ::= expr",
 /* 168 */ "inscollist_opt ::=",
 /* 169 */ "inscollist_opt ::= LP inscollist RP",
 /* 170 */ "inscollist ::= inscollist COMMA nm",
 /* 171 */ "inscollist ::= nm",
 /* 172 */ "expr ::= LP expr RP",
 /* 173 */ "expr ::= NULL",
 /* 174 */ "expr ::= ID",
 /* 175 */ "expr ::= JOIN_KW",
 /* 176 */ "expr ::= nm DOT nm",
 /* 177 */ "expr ::= nm DOT nm DOT nm",
 /* 178 */ "expr ::= INTEGER",
 /* 179 */ "expr ::= FLOAT",
 /* 180 */ "expr ::= STRING",
 /* 181 */ "expr ::= VARIABLE",
 /* 182 */ "expr ::= ID LP exprlist RP",
 /* 183 */ "expr ::= ID LP STAR RP",
 /* 184 */ "expr ::= expr AND expr",
 /* 185 */ "expr ::= expr OR expr",
 /* 186 */ "expr ::= expr LT expr",
 /* 187 */ "expr ::= expr GT expr",
 /* 188 */ "expr ::= expr LE expr",
 /* 189 */ "expr ::= expr GE expr",
 /* 190 */ "expr ::= expr NE expr",
 /* 191 */ "expr ::= expr EQ expr",
 /* 192 */ "expr ::= expr BITAND expr",
 /* 193 */ "expr ::= expr BITOR expr",
 /* 194 */ "expr ::= expr LSHIFT expr",
 /* 195 */ "expr ::= expr RSHIFT expr",
 /* 196 */ "expr ::= expr likeop expr",
 /* 197 */ "expr ::= expr NOT likeop expr",
 /* 198 */ "likeop ::= LIKE",
 /* 199 */ "likeop ::= GLOB",
 /* 200 */ "expr ::= expr PLUS expr",
 /* 201 */ "expr ::= expr MINUS expr",
 /* 202 */ "expr ::= expr STAR expr",
 /* 203 */ "expr ::= expr SLASH expr",
 /* 204 */ "expr ::= expr REM expr",
 /* 205 */ "expr ::= expr CONCAT expr",
 /* 206 */ "expr ::= expr ISNULL",
 /* 207 */ "expr ::= expr IS NULL",
 /* 208 */ "expr ::= expr NOTNULL",
 /* 209 */ "expr ::= expr NOT NULL",
 /* 210 */ "expr ::= expr IS NOT NULL",
 /* 211 */ "expr ::= NOT expr",
 /* 212 */ "expr ::= BITNOT expr",
 /* 213 */ "expr ::= MINUS expr",
 /* 214 */ "expr ::= PLUS expr",
 /* 215 */ "expr ::= LP select RP",
 /* 216 */ "expr ::= expr BETWEEN expr AND expr",
 /* 217 */ "expr ::= expr NOT BETWEEN expr AND expr",
 /* 218 */ "expr ::= expr IN LP exprlist RP",
 /* 219 */ "expr ::= expr IN LP select RP",
 /* 220 */ "expr ::= expr NOT IN LP exprlist RP",
 /* 221 */ "expr ::= expr NOT IN LP select RP",
 /* 222 */ "expr ::= expr IN nm dbnm",
 /* 223 */ "expr ::= expr NOT IN nm dbnm",
 /* 224 */ "expr ::= CASE case_operand case_exprlist case_else END",
 /* 225 */ "case_exprlist ::= case_exprlist WHEN expr THEN expr",
 /* 226 */ "case_exprlist ::= WHEN expr THEN expr",
 /* 227 */ "case_else ::= ELSE expr",
 /* 228 */ "case_else ::=",
 /* 229 */ "case_operand ::= expr",
 /* 230 */ "case_operand ::=",
 /* 231 */ "exprlist ::= exprlist COMMA expritem",
 /* 232 */ "exprlist ::= expritem",
 /* 233 */ "expritem ::= expr",
 /* 234 */ "expritem ::=",
 /* 235 */ "cmd ::= CREATE uniqueflag INDEX nm ON nm dbnm LP idxlist RP onconf",
 /* 236 */ "uniqueflag ::= UNIQUE",
 /* 237 */ "uniqueflag ::=",
 /* 238 */ "idxlist_opt ::=",
 /* 239 */ "idxlist_opt ::= LP idxlist RP",
 /* 240 */ "idxlist ::= idxlist COMMA idxitem",
 /* 241 */ "idxlist ::= idxitem",
 /* 242 */ "idxitem ::= nm sortorder",
 /* 243 */ "cmd ::= DROP INDEX nm dbnm",
 /* 244 */ "cmd ::= COPY orconf nm dbnm FROM nm USING DELIMITERS STRING",
 /* 245 */ "cmd ::= COPY orconf nm dbnm FROM nm",
 /* 246 */ "cmd ::= VACUUM",
 /* 247 */ "cmd ::= VACUUM nm",
 /* 248 */ "cmd ::= PRAGMA ids EQ nm",
 /* 249 */ "cmd ::= PRAGMA ids EQ ON",
 /* 250 */ "cmd ::= PRAGMA ids EQ plus_num",
 /* 251 */ "cmd ::= PRAGMA ids EQ minus_num",
 /* 252 */ "cmd ::= PRAGMA ids LP nm RP",
 /* 253 */ "cmd ::= PRAGMA ids",
 /* 254 */ "plus_num ::= plus_opt number",
 /* 255 */ "minus_num ::= MINUS number",
 /* 256 */ "number ::= INTEGER",
 /* 257 */ "number ::= FLOAT",
 /* 258 */ "plus_opt ::= PLUS",
 /* 259 */ "plus_opt ::=",
 /* 260 */ "cmd ::= CREATE trigger_decl BEGIN trigger_cmd_list END",
 /* 261 */ "trigger_decl ::= temp TRIGGER nm trigger_time trigger_event ON nm dbnm foreach_clause when_clause",
 /* 262 */ "trigger_time ::= BEFORE",
 /* 263 */ "trigger_time ::= AFTER",
 /* 264 */ "trigger_time ::= INSTEAD OF",
 /* 265 */ "trigger_time ::=",
 /* 266 */ "trigger_event ::= DELETE",
 /* 267 */ "trigger_event ::= INSERT",
 /* 268 */ "trigger_event ::= UPDATE",
 /* 269 */ "trigger_event ::= UPDATE OF inscollist",
 /* 270 */ "foreach_clause ::=",
 /* 271 */ "foreach_clause ::= FOR EACH ROW",
 /* 272 */ "foreach_clause ::= FOR EACH STATEMENT",
 /* 273 */ "when_clause ::=",
 /* 274 */ "when_clause ::= WHEN expr",
 /* 275 */ "trigger_cmd_list ::= trigger_cmd SEMI trigger_cmd_list",
 /* 276 */ "trigger_cmd_list ::=",
 /* 277 */ "trigger_cmd ::= UPDATE orconf nm SET setlist where_opt",
 /* 278 */ "trigger_cmd ::= insert_cmd INTO nm inscollist_opt VALUES LP itemlist RP",
 /* 279 */ "trigger_cmd ::= insert_cmd INTO nm inscollist_opt select",
 /* 280 */ "trigger_cmd ::= DELETE FROM nm where_opt",
 /* 281 */ "trigger_cmd ::= select",
 /* 282 */ "expr ::= RAISE LP IGNORE RP",
 /* 283 */ "expr ::= RAISE LP ROLLBACK COMMA nm RP",
 /* 284 */ "expr ::= RAISE LP ABORT COMMA nm RP",
 /* 285 */ "expr ::= RAISE LP FAIL COMMA nm RP",
 /* 286 */ "cmd ::= DROP TRIGGER nm dbnm",
 /* 287 */ "cmd ::= ATTACH database_kw_opt ids AS nm key_opt",
 /* 288 */ "key_opt ::= USING ids",
 /* 289 */ "key_opt ::=",
 /* 290 */ "database_kw_opt ::= DATABASE",
 /* 291 */ "database_kw_opt ::=",
 /* 292 */ "cmd ::= DETACH database_kw_opt nm",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to sqliteParser and sqliteParserFree.
*/
void *sqliteParserAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  sqliteParserARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    case 146: /* select */
    case 171: /* oneselect */
    case 189: /* seltablist_paren */
{
#line 286 "ext/sqlite/libsqlite/src/parse.y"
sqliteSelectDelete((yypminor->yy179));
#line 1131 "ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 158: /* expr */
    case 176: /* where_opt */
    case 178: /* having_opt */
    case 187: /* on_opt */
    case 192: /* sortitem */
    case 204: /* expritem */
{
#line 533 "ext/sqlite/libsqlite/src/parse.y"
sqliteExprDelete((yypminor->yy242));
#line 1143 "ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 159: /* idxlist_opt */
    case 167: /* idxlist */
    case 188: /* using_opt */
    case 197: /* inscollist_opt */
    case 199: /* inscollist */
{
#line 746 "ext/sqlite/libsqlite/src/parse.y"
sqliteIdListDelete((yypminor->yy320));
#line 1154 "ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 174: /* selcollist */
    case 177: /* groupby_opt */
    case 179: /* orderby_opt */
    case 181: /* sclp */
    case 191: /* sortlist */
    case 194: /* exprlist */
    case 195: /* setlist */
    case 198: /* itemlist */
    case 202: /* case_exprlist */
{
#line 322 "ext/sqlite/libsqlite/src/parse.y"
sqliteExprListDelete((yypminor->yy322));
#line 1169 "ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 175: /* from */
    case 183: /* seltablist */
    case 184: /* stl_prefix */
{
#line 353 "ext/sqlite/libsqlite/src/parse.y"
sqliteSrcListDelete((yypminor->yy307));
#line 1178 "ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 212: /* trigger_cmd_list */
    case 217: /* trigger_cmd */
{
#line 828 "ext/sqlite/libsqlite/src/parse.y"
sqliteDeleteTriggerStep((yypminor->yy19));
#line 1186 "ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 214: /* trigger_event */
{
#line 812 "ext/sqlite/libsqlite/src/parse.y"
sqliteIdListDelete((yypminor->yy290).b);
#line 1193 "ext/sqlite/libsqlite/src/parse.c"
}
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from sqliteParserAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void sqliteParserFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int sqliteParserStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_COUNT
   || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
          j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
          j<YY_ACTTAB_COUNT &&
#endif
          yy_lookahead[j]==YYWILDCARD
        ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   sqliteParserARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   sqliteParserARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 132, 1 },
  { 133, 2 },
  { 133, 1 },
  { 134, 3 },
  { 134, 1 },
  { 136, 1 },
  { 135, 1 },
  { 135, 0 },
  { 137, 3 },
  { 138, 0 },
  { 138, 1 },
  { 138, 2 },
  { 137, 2 },
  { 137, 2 },
  { 137, 2 },
  { 137, 2 },
  { 141, 4 },
  { 143, 1 },
  { 143, 0 },
  { 142, 4 },
  { 142, 2 },
  { 144, 3 },
  { 144, 1 },
  { 147, 3 },
  { 148, 1 },
  { 151, 1 },
  { 152, 1 },
  { 152, 1 },
  { 140, 1 },
  { 140, 1 },
  { 140, 1 },
  { 149, 0 },
  { 149, 1 },
  { 149, 4 },
  { 149, 6 },
  { 153, 1 },
  { 153, 2 },
  { 154, 1 },
  { 154, 2 },
  { 154, 2 },
  { 150, 2 },
  { 150, 0 },
  { 155, 3 },
  { 155, 1 },
  { 155, 2 },
  { 155, 2 },
  { 155, 2 },
  { 155, 3 },
  { 155, 3 },
  { 155, 2 },
  { 155, 3 },
  { 155, 3 },
  { 155, 2 },
  { 156, 2 },
  { 156, 3 },
  { 156, 4 },
  { 156, 2 },
  { 156, 5 },
  { 156, 4 },
  { 156, 1 },
  { 156, 2 },
  { 160, 0 },
  { 160, 2 },
  { 162, 2 },
  { 162, 3 },
  { 162, 3 },
  { 162, 3 },
  { 163, 2 },
  { 163, 2 },
  { 163, 1 },
  { 163, 1 },
  { 161, 3 },
  { 161, 2 },
  { 164, 0 },
  { 164, 2 },
  { 164, 2 },
  { 145, 0 },
  { 145, 2 },
  { 165, 3 },
  { 165, 2 },
  { 165, 1 },
  { 166, 2 },
  { 166, 6 },
  { 166, 5 },
  { 166, 3 },
  { 166, 10 },
  { 168, 0 },
  { 168, 1 },
  { 139, 0 },
  { 139, 3 },
  { 169, 0 },
  { 169, 2 },
  { 170, 1 },
  { 170, 1 },
  { 170, 1 },
  { 170, 1 },
  { 170, 1 },
  { 137, 3 },
  { 137, 6 },
  { 137, 3 },
  { 137, 1 },
  { 146, 1 },
  { 146, 3 },
  { 172, 1 },
  { 172, 2 },
  { 172, 1 },
  { 172, 1 },
  { 171, 9 },
  { 173, 1 },
  { 173, 1 },
  { 173, 0 },
  { 181, 2 },
  { 181, 0 },
  { 174, 3 },
  { 174, 2 },
  { 174, 4 },
  { 182, 2 },
  { 182, 1 },
  { 182, 0 },
  { 175, 0 },
  { 175, 2 },
  { 184, 2 },
  { 184, 0 },
  { 183, 6 },
  { 183, 7 },
  { 189, 1 },
  { 189, 1 },
  { 186, 0 },
  { 186, 2 },
  { 185, 1 },
  { 185, 1 },
  { 185, 2 },
  { 185, 3 },
  { 185, 4 },
  { 187, 2 },
  { 187, 0 },
  { 188, 4 },
  { 188, 0 },
  { 179, 0 },
  { 179, 3 },
  { 191, 5 },
  { 191, 3 },
  { 192, 1 },
  { 157, 1 },
  { 157, 1 },
  { 157, 0 },
  { 193, 0 },
  { 193, 2 },
  { 177, 0 },
  { 177, 3 },
  { 178, 0 },
  { 178, 2 },
  { 180, 0 },
  { 180, 2 },
  { 180, 4 },
  { 180, 4 },
  { 137, 5 },
  { 176, 0 },
  { 176, 2 },
  { 137, 7 },
  { 195, 5 },
  { 195, 3 },
  { 137, 9 },
  { 137, 6 },
  { 196, 2 },
  { 196, 1 },
  { 198, 3 },
  { 198, 1 },
  { 197, 0 },
  { 197, 3 },
  { 199, 3 },
  { 199, 1 },
  { 158, 3 },
  { 158, 1 },
  { 158, 1 },
  { 158, 1 },
  { 158, 3 },
  { 158, 5 },
  { 158, 1 },
  { 158, 1 },
  { 158, 1 },
  { 158, 1 },
  { 158, 4 },
  { 158, 4 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 4 },
  { 200, 1 },
  { 200, 1 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 3 },
  { 158, 2 },
  { 158, 3 },
  { 158, 2 },
  { 158, 3 },
  { 158, 4 },
  { 158, 2 },
  { 158, 2 },
  { 158, 2 },
  { 158, 2 },
  { 158, 3 },
  { 158, 5 },
  { 158, 6 },
  { 158, 5 },
  { 158, 5 },
  { 158, 6 },
  { 158, 6 },
  { 158, 4 },
  { 158, 5 },
  { 158, 5 },
  { 202, 5 },
  { 202, 4 },
  { 203, 2 },
  { 203, 0 },
  { 201, 1 },
  { 201, 0 },
  { 194, 3 },
  { 194, 1 },
  { 204, 1 },
  { 204, 0 },
  { 137, 11 },
  { 205, 1 },
  { 205, 0 },
  { 159, 0 },
  { 159, 3 },
  { 167, 3 },
  { 167, 1 },
  { 206, 2 },
  { 137, 4 },
  { 137, 9 },
  { 137, 6 },
  { 137, 1 },
  { 137, 2 },
  { 137, 4 },
  { 137, 4 },
  { 137, 4 },
  { 137, 4 },
  { 137, 5 },
  { 137, 2 },
  { 207, 2 },
  { 208, 2 },
  { 210, 1 },
  { 210, 1 },
  { 209, 1 },
  { 209, 0 },
  { 137, 5 },
  { 211, 10 },
  { 213, 1 },
  { 213, 1 },
  { 213, 2 },
  { 213, 0 },
  { 214, 1 },
  { 214, 1 },
  { 214, 1 },
  { 214, 3 },
  { 215, 0 },
  { 215, 3 },
  { 215, 3 },
  { 216, 0 },
  { 216, 2 },
  { 212, 3 },
  { 212, 0 },
  { 217, 6 },
  { 217, 8 },
  { 217, 5 },
  { 217, 4 },
  { 217, 1 },
  { 158, 4 },
  { 158, 6 },
  { 158, 6 },
  { 158, 6 },
  { 137, 4 },
  { 137, 6 },
  { 219, 2 },
  { 219, 0 },
  { 218, 1 },
  { 218, 0 },
  { 137, 3 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  sqliteParserARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 5: /* cmdx ::= cmd */
#line 72 "ext/sqlite/libsqlite/src/parse.y"
{ sqliteExec(pParse); }
#line 1781 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 6: /* explain ::= EXPLAIN */
#line 73 "ext/sqlite/libsqlite/src/parse.y"
{ sqliteBeginParse(pParse, 1); }
#line 1786 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 7: /* explain ::= */
#line 74 "ext/sqlite/libsqlite/src/parse.y"
{ sqliteBeginParse(pParse, 0); }
#line 1791 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 8: /* cmd ::= BEGIN trans_opt onconf */
#line 79 "ext/sqlite/libsqlite/src/parse.y"
{sqliteBeginTransaction(pParse,yymsp[0].minor.yy372);}
#line 1796 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 12: /* cmd ::= COMMIT trans_opt */
      case 13: /* cmd ::= END trans_opt */ yytestcase(yyruleno==13);
#line 83 "ext/sqlite/libsqlite/src/parse.y"
{sqliteCommitTransaction(pParse);}
#line 1802 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 14: /* cmd ::= ROLLBACK trans_opt */
#line 85 "ext/sqlite/libsqlite/src/parse.y"
{sqliteRollbackTransaction(pParse);}
#line 1807 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 16: /* create_table ::= CREATE temp TABLE nm */
#line 90 "ext/sqlite/libsqlite/src/parse.y"
{
   sqliteStartTable(pParse,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0,yymsp[-2].minor.yy372,0);
}
#line 1814 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 17: /* temp ::= TEMP */
      case 74: /* init_deferred_pred_opt ::= INITIALLY DEFERRED */ yytestcase(yyruleno==74);
      case 108: /* distinct ::= DISTINCT */ yytestcase(yyruleno==108);
#line 94 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = 1;}
#line 1821 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 18: /* temp ::= */
      case 73: /* init_deferred_pred_opt ::= */ yytestcase(yyruleno==73);
      case 75: /* init_deferred_pred_opt ::= INITIALLY IMMEDIATE */ yytestcase(yyruleno==75);
      case 86: /* defer_subclause_opt ::= */ yytestcase(yyruleno==86);
      case 109: /* distinct ::= ALL */ yytestcase(yyruleno==109);
      case 110: /* distinct ::= */ yytestcase(yyruleno==110);
#line 95 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = 0;}
#line 1831 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 19: /* create_table_args ::= LP columnlist conslist_opt RP */
#line 96 "ext/sqlite/libsqlite/src/parse.y"
{
  sqliteEndTable(pParse,&yymsp[0].minor.yy0,0);
}
#line 1838 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 20: /* create_table_args ::= AS select */
#line 99 "ext/sqlite/libsqlite/src/parse.y"
{
  sqliteEndTable(pParse,0,yymsp[0].minor.yy179);
  sqliteSelectDelete(yymsp[0].minor.yy179);
}
#line 1846 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 24: /* columnid ::= nm */
#line 111 "ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumn(pParse,&yymsp[0].minor.yy0);}
#line 1851 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 25: /* id ::= ID */
      case 26: /* ids ::= ID */ yytestcase(yyruleno==26);
      case 27: /* ids ::= STRING */ yytestcase(yyruleno==27);
      case 28: /* nm ::= ID */ yytestcase(yyruleno==28);
      case 29: /* nm ::= STRING */ yytestcase(yyruleno==29);
      case 30: /* nm ::= JOIN_KW */ yytestcase(yyruleno==30);
      case 35: /* typename ::= ids */ yytestcase(yyruleno==35);
      case 128: /* dbnm ::= DOT nm */ yytestcase(yyruleno==128);
      case 254: /* plus_num ::= plus_opt number */ yytestcase(yyruleno==254);
      case 255: /* minus_num ::= MINUS number */ yytestcase(yyruleno==255);
      case 256: /* number ::= INTEGER */ yytestcase(yyruleno==256);
      case 257: /* number ::= FLOAT */ yytestcase(yyruleno==257);
#line 117 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy0 = yymsp[0].minor.yy0;}
#line 1867 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 32: /* type ::= typename */
#line 160 "ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[0].minor.yy0,&yymsp[0].minor.yy0);}
#line 1872 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 33: /* type ::= typename LP signed RP */
#line 161 "ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);}
#line 1877 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 34: /* type ::= typename LP signed COMMA signed RP */
#line 163 "ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[-5].minor.yy0,&yymsp[0].minor.yy0);}
#line 1882 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 36: /* typename ::= typename ids */
      case 242: /* idxitem ::= nm sortorder */ yytestcase(yyruleno==242);
#line 166 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy0 = yymsp[-1].minor.yy0;}
#line 1888 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 37: /* signed ::= INTEGER */
      case 38: /* signed ::= PLUS INTEGER */ yytestcase(yyruleno==38);
#line 168 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = atoi(yymsp[0].minor.yy0.z); }
#line 1894 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 39: /* signed ::= MINUS INTEGER */
#line 170 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = -atoi(yymsp[0].minor.yy0.z); }
#line 1899 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 44: /* carg ::= DEFAULT STRING */
      case 45: /* carg ::= DEFAULT ID */ yytestcase(yyruleno==45);
      case 46: /* carg ::= DEFAULT INTEGER */ yytestcase(yyruleno==46);
      case 47: /* carg ::= DEFAULT PLUS INTEGER */ yytestcase(yyruleno==47);
      case 49: /* carg ::= DEFAULT FLOAT */ yytestcase(yyruleno==49);
      case 50: /* carg ::= DEFAULT PLUS FLOAT */ yytestcase(yyruleno==50);
#line 175 "ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 1909 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 48: /* carg ::= DEFAULT MINUS INTEGER */
      case 51: /* carg ::= DEFAULT MINUS FLOAT */ yytestcase(yyruleno==51);
#line 179 "ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,1);}
#line 1915 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 54: /* ccons ::= NOT NULL onconf */
#line 189 "ext/sqlite/libsqlite/src/parse.y"
{sqliteAddNotNull(pParse, yymsp[0].minor.yy372);}
#line 1920 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 55: /* ccons ::= PRIMARY KEY sortorder onconf */
#line 190 "ext/sqlite/libsqlite/src/parse.y"
{sqliteAddPrimaryKey(pParse,0,yymsp[0].minor.yy372);}
#line 1925 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 56: /* ccons ::= UNIQUE onconf */
#line 191 "ext/sqlite/libsqlite/src/parse.y"
{sqliteCreateIndex(pParse,0,0,0,yymsp[0].minor.yy372,0,0);}
#line 1930 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 57: /* ccons ::= CHECK LP expr RP onconf */
#line 192 "ext/sqlite/libsqlite/src/parse.y"
{
  yy_destructor(yypParser,158,&yymsp[-2].minor);
}
#line 1937 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 58: /* ccons ::= REFERENCES nm idxlist_opt refargs */
#line 194 "ext/sqlite/libsqlite/src/parse.y"
{sqliteCreateForeignKey(pParse,0,&yymsp[-2].minor.yy0,yymsp[-1].minor.yy320,yymsp[0].minor.yy372);}
#line 1942 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 59: /* ccons ::= defer_subclause */
#line 195 "ext/sqlite/libsqlite/src/parse.y"
{sqliteDeferForeignKey(pParse,yymsp[0].minor.yy372);}
#line 1947 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 60: /* ccons ::= COLLATE id */
#line 196 "ext/sqlite/libsqlite/src/parse.y"
{
   sqliteAddCollateType(pParse, sqliteCollateType(yymsp[0].minor.yy0.z, yymsp[0].minor.yy0.n));
}
#line 1954 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 61: /* refargs ::= */
#line 206 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Restrict * 0x010101; }
#line 1959 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 62: /* refargs ::= refargs refarg */
#line 207 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = (yymsp[-1].minor.yy372 & yymsp[0].minor.yy407.mask) | yymsp[0].minor.yy407.value; }
#line 1964 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 63: /* refarg ::= MATCH nm */
#line 209 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy407.value = 0;     yygotominor.yy407.mask = 0x000000; }
#line 1969 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 64: /* refarg ::= ON DELETE refact */
#line 210 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy407.value = yymsp[0].minor.yy372;     yygotominor.yy407.mask = 0x0000ff; }
#line 1974 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 65: /* refarg ::= ON UPDATE refact */
#line 211 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy407.value = yymsp[0].minor.yy372<<8;  yygotominor.yy407.mask = 0x00ff00; }
#line 1979 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 66: /* refarg ::= ON INSERT refact */
#line 212 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy407.value = yymsp[0].minor.yy372<<16; yygotominor.yy407.mask = 0xff0000; }
#line 1984 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 67: /* refact ::= SET NULL */
#line 214 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_SetNull; }
#line 1989 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 68: /* refact ::= SET DEFAULT */
#line 215 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_SetDflt; }
#line 1994 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 69: /* refact ::= CASCADE */
#line 216 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Cascade; }
#line 1999 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 70: /* refact ::= RESTRICT */
#line 217 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Restrict; }
#line 2004 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 71: /* defer_subclause ::= NOT DEFERRABLE init_deferred_pred_opt */
      case 72: /* defer_subclause ::= DEFERRABLE init_deferred_pred_opt */ yytestcase(yyruleno==72);
      case 87: /* defer_subclause_opt ::= defer_subclause */ yytestcase(yyruleno==87);
      case 164: /* insert_cmd ::= INSERT orconf */ yytestcase(yyruleno==164);
#line 219 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = yymsp[0].minor.yy372;}
#line 2012 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 82: /* tcons ::= PRIMARY KEY LP idxlist RP onconf */
#line 236 "ext/sqlite/libsqlite/src/parse.y"
{sqliteAddPrimaryKey(pParse,yymsp[-2].minor.yy320,yymsp[0].minor.yy372);}
#line 2017 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 83: /* tcons ::= UNIQUE LP idxlist RP onconf */
#line 238 "ext/sqlite/libsqlite/src/parse.y"
{sqliteCreateIndex(pParse,0,0,yymsp[-2].minor.yy320,yymsp[0].minor.yy372,0,0);}
#line 2022 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 84: /* tcons ::= CHECK expr onconf */
#line 239 "ext/sqlite/libsqlite/src/parse.y"
{
  yy_destructor(yypParser,158,&yymsp[-1].minor);
}
#line 2029 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 85: /* tcons ::= FOREIGN KEY LP idxlist RP REFERENCES nm idxlist_opt refargs defer_subclause_opt */
#line 241 "ext/sqlite/libsqlite/src/parse.y"
{
    sqliteCreateForeignKey(pParse, yymsp[-6].minor.yy320, &yymsp[-3].minor.yy0, yymsp[-2].minor.yy320, yymsp[-1].minor.yy372);
    sqliteDeferForeignKey(pParse, yymsp[0].minor.yy372);
}
#line 2037 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 88: /* onconf ::= */
      case 90: /* orconf ::= */ yytestcase(yyruleno==90);
#line 255 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Default; }
#line 2043 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 89: /* onconf ::= ON CONFLICT resolvetype */
      case 91: /* orconf ::= OR resolvetype */ yytestcase(yyruleno==91);
#line 256 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = yymsp[0].minor.yy372; }
#line 2049 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 92: /* resolvetype ::= ROLLBACK */
#line 259 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Rollback; }
#line 2054 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 93: /* resolvetype ::= ABORT */
      case 236: /* uniqueflag ::= UNIQUE */ yytestcase(yyruleno==236);
#line 260 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Abort; }
#line 2060 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 94: /* resolvetype ::= FAIL */
#line 261 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Fail; }
#line 2065 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 95: /* resolvetype ::= IGNORE */
#line 262 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Ignore; }
#line 2070 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 96: /* resolvetype ::= REPLACE */
#line 263 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Replace; }
#line 2075 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 97: /* cmd ::= DROP TABLE nm */
#line 267 "ext/sqlite/libsqlite/src/parse.y"
{sqliteDropTable(pParse,&yymsp[0].minor.yy0,0);}
#line 2080 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 98: /* cmd ::= CREATE temp VIEW nm AS select */
#line 271 "ext/sqlite/libsqlite/src/parse.y"
{
  sqliteCreateView(pParse, &yymsp[-5].minor.yy0, &yymsp[-2].minor.yy0, yymsp[0].minor.yy179, yymsp[-4].minor.yy372);
}
#line 2087 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 99: /* cmd ::= DROP VIEW nm */
#line 274 "ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDropTable(pParse, &yymsp[0].minor.yy0, 1);
}
#line 2094 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 100: /* cmd ::= select */
#line 280 "ext/sqlite/libsqlite/src/parse.y"
{
  sqliteSelect(pParse, yymsp[0].minor.yy179, SRT_Callback, 0, 0, 0, 0);
  sqliteSelectDelete(yymsp[0].minor.yy179);
}
#line 2102 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 101: /* select ::= oneselect */
      case 125: /* seltablist_paren ::= select */ yytestcase(yyruleno==125);
#line 290 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy179 = yymsp[0].minor.yy179;}
#line 2108 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 102: /* select ::= select multiselect_op oneselect */
#line 291 "ext/sqlite/libsqlite/src/parse.y"
{
  if( yymsp[0].minor.yy179 ){
    yymsp[0].minor.yy179->op = yymsp[-1].minor.yy372;
    yymsp[0].minor.yy179->pPrior = yymsp[-2].minor.yy179;
  }
  yygotominor.yy179 = yymsp[0].minor.yy179;
}
#line 2119 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 103: /* multiselect_op ::= UNION */
#line 299 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_UNION;}
#line 2124 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 104: /* multiselect_op ::= UNION ALL */
#line 300 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_ALL;}
#line 2129 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 105: /* multiselect_op ::= INTERSECT */
#line 301 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_INTERSECT;}
#line 2134 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 106: /* multiselect_op ::= EXCEPT */
#line 302 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_EXCEPT;}
#line 2139 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 107: /* oneselect ::= SELECT distinct selcollist from where_opt groupby_opt having_opt orderby_opt limit_opt */
#line 304 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy179 = sqliteSelectNew(yymsp[-6].minor.yy322,yymsp[-5].minor.yy307,yymsp[-4].minor.yy242,yymsp[-3].minor.yy322,yymsp[-2].minor.yy242,yymsp[-1].minor.yy322,yymsp[-7].minor.yy372,yymsp[0].minor.yy124.limit,yymsp[0].minor.yy124.offset);
}
#line 2146 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 111: /* sclp ::= selcollist COMMA */
#line 325 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = yymsp[-1].minor.yy322;}
#line 2151 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 112: /* sclp ::= */
      case 138: /* orderby_opt ::= */ yytestcase(yyruleno==138);
      case 148: /* groupby_opt ::= */ yytestcase(yyruleno==148);
#line 326 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = 0;}
#line 2158 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 113: /* selcollist ::= sclp expr as */
#line 327 "ext/sqlite/libsqlite/src/parse.y"
{
   yygotominor.yy322 = sqliteExprListAppend(yymsp[-2].minor.yy322,yymsp[-1].minor.yy242,yymsp[0].minor.yy0.n?&yymsp[0].minor.yy0:0);
}
#line 2165 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 114: /* selcollist ::= sclp STAR */
#line 330 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy322 = sqliteExprListAppend(yymsp[-1].minor.yy322, sqliteExpr(TK_ALL, 0, 0, 0), 0);
}
#line 2172 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 115: /* selcollist ::= sclp nm DOT STAR */
#line 333 "ext/sqlite/libsqlite/src/parse.y"
{
  Expr *pRight = sqliteExpr(TK_ALL, 0, 0, 0);
  Expr *pLeft = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy0);
  yygotominor.yy322 = sqliteExprListAppend(yymsp[-3].minor.yy322, sqliteExpr(TK_DOT, pLeft, pRight, 0), 0);
}
#line 2181 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 116: /* as ::= AS nm */
      case 117: /* as ::= ids */ yytestcase(yyruleno==117);
      case 288: /* key_opt ::= USING ids */ yytestcase(yyruleno==288);
#line 343 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 2188 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 118: /* as ::= */
#line 345 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy0.n = 0; }
#line 2193 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 119: /* from ::= */
#line 357 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy307 = sqliteMalloc(sizeof(*yygotominor.yy307));}
#line 2198 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 120: /* from ::= FROM seltablist */
#line 358 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy307 = yymsp[0].minor.yy307;}
#line 2203 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 121: /* stl_prefix ::= seltablist joinop */
#line 363 "ext/sqlite/libsqlite/src/parse.y"
{
   yygotominor.yy307 = yymsp[-1].minor.yy307;
   if( yygotominor.yy307 && yygotominor.yy307->nSrc>0 ) yygotominor.yy307->a[yygotominor.yy307->nSrc-1].jointype = yymsp[0].minor.yy372;
}
#line 2211 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 122: /* stl_prefix ::= */
#line 367 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy307 = 0;}
#line 2216 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 123: /* seltablist ::= stl_prefix nm dbnm as on_opt using_opt */
#line 368 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy307 = sqliteSrcListAppend(yymsp[-5].minor.yy307,&yymsp[-4].minor.yy0,&yymsp[-3].minor.yy0);
  if( yymsp[-2].minor.yy0.n ) sqliteSrcListAddAlias(yygotominor.yy307,&yymsp[-2].minor.yy0);
  if( yymsp[-1].minor.yy242 ){
    if( yygotominor.yy307 && yygotominor.yy307->nSrc>1 ){ yygotominor.yy307->a[yygotominor.yy307->nSrc-2].pOn = yymsp[-1].minor.yy242; }
    else { sqliteExprDelete(yymsp[-1].minor.yy242); }
  }
  if( yymsp[0].minor.yy320 ){
    if( yygotominor.yy307 && yygotominor.yy307->nSrc>1 ){ yygotominor.yy307->a[yygotominor.yy307->nSrc-2].pUsing = yymsp[0].minor.yy320; }
    else { sqliteIdListDelete(yymsp[0].minor.yy320); }
  }
}
#line 2232 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 124: /* seltablist ::= stl_prefix LP seltablist_paren RP as on_opt using_opt */
#line 381 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy307 = sqliteSrcListAppend(yymsp[-6].minor.yy307,0,0);
  yygotominor.yy307->a[yygotominor.yy307->nSrc-1].pSelect = yymsp[-4].minor.yy179;
  if( yymsp[-2].minor.yy0.n ) sqliteSrcListAddAlias(yygotominor.yy307,&yymsp[-2].minor.yy0);
  if( yymsp[-1].minor.yy242 ){
    if( yygotominor.yy307 && yygotominor.yy307->nSrc>1 ){ yygotominor.yy307->a[yygotominor.yy307->nSrc-2].pOn = yymsp[-1].minor.yy242; }
    else { sqliteExprDelete(yymsp[-1].minor.yy242); }
  }
  if( yymsp[0].minor.yy320 ){
    if( yygotominor.yy307 && yygotominor.yy307->nSrc>1 ){ yygotominor.yy307->a[yygotominor.yy307->nSrc-2].pUsing = yymsp[0].minor.yy320; }
    else { sqliteIdListDelete(yymsp[0].minor.yy320); }
  }
}
#line 2249 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 126: /* seltablist_paren ::= seltablist */
#line 402 "ext/sqlite/libsqlite/src/parse.y"
{
   yygotominor.yy179 = sqliteSelectNew(0,yymsp[0].minor.yy307,0,0,0,0,0,-1,0);
}
#line 2256 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 127: /* dbnm ::= */
#line 407 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy0.z=0; yygotominor.yy0.n=0;}
#line 2261 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 129: /* joinop ::= COMMA */
      case 130: /* joinop ::= JOIN */ yytestcase(yyruleno==130);
#line 412 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = JT_INNER; }
#line 2267 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 131: /* joinop ::= JOIN_KW JOIN */
#line 414 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = sqliteJoinType(pParse,&yymsp[-1].minor.yy0,0,0); }
#line 2272 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 132: /* joinop ::= JOIN_KW nm JOIN */
#line 415 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = sqliteJoinType(pParse,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0,0); }
#line 2277 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 133: /* joinop ::= JOIN_KW nm nm JOIN */
#line 417 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = sqliteJoinType(pParse,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0); }
#line 2282 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 134: /* on_opt ::= ON expr */
      case 142: /* sortitem ::= expr */ yytestcase(yyruleno==142);
      case 151: /* having_opt ::= HAVING expr */ yytestcase(yyruleno==151);
      case 158: /* where_opt ::= WHERE expr */ yytestcase(yyruleno==158);
      case 227: /* case_else ::= ELSE expr */ yytestcase(yyruleno==227);
      case 229: /* case_operand ::= expr */ yytestcase(yyruleno==229);
      case 233: /* expritem ::= expr */ yytestcase(yyruleno==233);
#line 421 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = yymsp[0].minor.yy242;}
#line 2293 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 135: /* on_opt ::= */
      case 150: /* having_opt ::= */ yytestcase(yyruleno==150);
      case 157: /* where_opt ::= */ yytestcase(yyruleno==157);
      case 228: /* case_else ::= */ yytestcase(yyruleno==228);
      case 230: /* case_operand ::= */ yytestcase(yyruleno==230);
      case 234: /* expritem ::= */ yytestcase(yyruleno==234);
#line 422 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = 0;}
#line 2303 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 136: /* using_opt ::= USING LP idxlist RP */
      case 169: /* inscollist_opt ::= LP inscollist RP */ yytestcase(yyruleno==169);
      case 239: /* idxlist_opt ::= LP idxlist RP */ yytestcase(yyruleno==239);
#line 426 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy320 = yymsp[-1].minor.yy320;}
#line 2310 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 137: /* using_opt ::= */
      case 168: /* inscollist_opt ::= */ yytestcase(yyruleno==168);
      case 238: /* idxlist_opt ::= */ yytestcase(yyruleno==238);
#line 427 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy320 = 0;}
#line 2317 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 139: /* orderby_opt ::= ORDER BY sortlist */
      case 149: /* groupby_opt ::= GROUP BY exprlist */ yytestcase(yyruleno==149);
#line 438 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = yymsp[0].minor.yy322;}
#line 2323 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 140: /* sortlist ::= sortlist COMMA sortitem collate sortorder */
#line 439 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy322 = sqliteExprListAppend(yymsp[-4].minor.yy322,yymsp[-2].minor.yy242,0);
  if( yygotominor.yy322 ) yygotominor.yy322->a[yygotominor.yy322->nExpr-1].sortOrder = yymsp[-1].minor.yy372+yymsp[0].minor.yy372;
}
#line 2331 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 141: /* sortlist ::= sortitem collate sortorder */
#line 443 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy322 = sqliteExprListAppend(0,yymsp[-2].minor.yy242,0);
  if( yygotominor.yy322 ) yygotominor.yy322->a[0].sortOrder = yymsp[-1].minor.yy372+yymsp[0].minor.yy372;
}
#line 2339 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 143: /* sortorder ::= ASC */
      case 145: /* sortorder ::= */ yytestcase(yyruleno==145);
#line 452 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = SQLITE_SO_ASC;}
#line 2345 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 144: /* sortorder ::= DESC */
#line 453 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = SQLITE_SO_DESC;}
#line 2350 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 146: /* collate ::= */
#line 455 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = SQLITE_SO_UNK;}
#line 2355 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 147: /* collate ::= COLLATE id */
#line 456 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = sqliteCollateType(yymsp[0].minor.yy0.z, yymsp[0].minor.yy0.n);}
#line 2360 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 152: /* limit_opt ::= */
#line 469 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy124.limit = -1; yygotominor.yy124.offset = 0;}
#line 2365 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 153: /* limit_opt ::= LIMIT signed */
#line 470 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy124.limit = yymsp[0].minor.yy372; yygotominor.yy124.offset = 0;}
#line 2370 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 154: /* limit_opt ::= LIMIT signed OFFSET signed */
#line 472 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy124.limit = yymsp[-2].minor.yy372; yygotominor.yy124.offset = yymsp[0].minor.yy372;}
#line 2375 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 155: /* limit_opt ::= LIMIT signed COMMA signed */
#line 474 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy124.limit = yymsp[0].minor.yy372; yygotominor.yy124.offset = yymsp[-2].minor.yy372;}
#line 2380 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 156: /* cmd ::= DELETE FROM nm dbnm where_opt */
#line 478 "ext/sqlite/libsqlite/src/parse.y"
{
   sqliteDeleteFrom(pParse, sqliteSrcListAppend(0,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0), yymsp[0].minor.yy242);
}
#line 2387 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 159: /* cmd ::= UPDATE orconf nm dbnm SET setlist where_opt */
#line 494 "ext/sqlite/libsqlite/src/parse.y"
{sqliteUpdate(pParse,sqliteSrcListAppend(0,&yymsp[-4].minor.yy0,&yymsp[-3].minor.yy0),yymsp[-1].minor.yy322,yymsp[0].minor.yy242,yymsp[-5].minor.yy372);}
#line 2392 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 160: /* setlist ::= setlist COMMA nm EQ expr */
#line 497 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = sqliteExprListAppend(yymsp[-4].minor.yy322,yymsp[0].minor.yy242,&yymsp[-2].minor.yy0);}
#line 2397 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 161: /* setlist ::= nm EQ expr */
#line 498 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = sqliteExprListAppend(0,yymsp[0].minor.yy242,&yymsp[-2].minor.yy0);}
#line 2402 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 162: /* cmd ::= insert_cmd INTO nm dbnm inscollist_opt VALUES LP itemlist RP */
#line 504 "ext/sqlite/libsqlite/src/parse.y"
{sqliteInsert(pParse, sqliteSrcListAppend(0,&yymsp[-6].minor.yy0,&yymsp[-5].minor.yy0), yymsp[-1].minor.yy322, 0, yymsp[-4].minor.yy320, yymsp[-8].minor.yy372);}
#line 2407 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 163: /* cmd ::= insert_cmd INTO nm dbnm inscollist_opt select */
#line 506 "ext/sqlite/libsqlite/src/parse.y"
{sqliteInsert(pParse, sqliteSrcListAppend(0,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0), 0, yymsp[0].minor.yy179, yymsp[-1].minor.yy320, yymsp[-5].minor.yy372);}
#line 2412 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 165: /* insert_cmd ::= REPLACE */
#line 510 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = OE_Replace;}
#line 2417 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 166: /* itemlist ::= itemlist COMMA expr */
      case 231: /* exprlist ::= exprlist COMMA expritem */ yytestcase(yyruleno==231);
#line 516 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = sqliteExprListAppend(yymsp[-2].minor.yy322,yymsp[0].minor.yy242,0);}
#line 2423 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 167: /* itemlist ::= expr */
      case 232: /* exprlist ::= expritem */ yytestcase(yyruleno==232);
#line 517 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = sqliteExprListAppend(0,yymsp[0].minor.yy242,0);}
#line 2429 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 170: /* inscollist ::= inscollist COMMA nm */
      case 240: /* idxlist ::= idxlist COMMA idxitem */ yytestcase(yyruleno==240);
#line 526 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy320 = sqliteIdListAppend(yymsp[-2].minor.yy320,&yymsp[0].minor.yy0);}
#line 2435 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 171: /* inscollist ::= nm */
      case 241: /* idxlist ::= idxitem */ yytestcase(yyruleno==241);
#line 527 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy320 = sqliteIdListAppend(0,&yymsp[0].minor.yy0);}
#line 2441 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 172: /* expr ::= LP expr RP */
#line 535 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = yymsp[-1].minor.yy242; sqliteExprSpan(yygotominor.yy242,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0); }
#line 2446 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 173: /* expr ::= NULL */
#line 536 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_NULL, 0, 0, &yymsp[0].minor.yy0);}
#line 2451 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 174: /* expr ::= ID */
      case 175: /* expr ::= JOIN_KW */ yytestcase(yyruleno==175);
#line 537 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy0);}
#line 2457 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 176: /* expr ::= nm DOT nm */
#line 539 "ext/sqlite/libsqlite/src/parse.y"
{
  Expr *temp1 = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy0);
  Expr *temp2 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy0);
  yygotominor.yy242 = sqliteExpr(TK_DOT, temp1, temp2, 0);
}
#line 2466 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 177: /* expr ::= nm DOT nm DOT nm */
#line 544 "ext/sqlite/libsqlite/src/parse.y"
{
  Expr *temp1 = sqliteExpr(TK_ID, 0, 0, &yymsp[-4].minor.yy0);
  Expr *temp2 = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy0);
  Expr *temp3 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy0);
  Expr *temp4 = sqliteExpr(TK_DOT, temp2, temp3, 0);
  yygotominor.yy242 = sqliteExpr(TK_DOT, temp1, temp4, 0);
}
#line 2477 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 178: /* expr ::= INTEGER */
#line 551 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_INTEGER, 0, 0, &yymsp[0].minor.yy0);}
#line 2482 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 179: /* expr ::= FLOAT */
#line 552 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_FLOAT, 0, 0, &yymsp[0].minor.yy0);}
#line 2487 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 180: /* expr ::= STRING */
#line 553 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_STRING, 0, 0, &yymsp[0].minor.yy0);}
#line 2492 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 181: /* expr ::= VARIABLE */
#line 554 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_VARIABLE, 0, 0, &yymsp[0].minor.yy0);
  if( yygotominor.yy242 ) yygotominor.yy242->iTable = ++pParse->nVar;
}
#line 2500 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 182: /* expr ::= ID LP exprlist RP */
#line 558 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExprFunction(yymsp[-1].minor.yy322, &yymsp[-3].minor.yy0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);
}
#line 2508 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 183: /* expr ::= ID LP STAR RP */
#line 562 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExprFunction(0, &yymsp[-3].minor.yy0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);
}
#line 2516 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 184: /* expr ::= expr AND expr */
#line 566 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_AND, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2521 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 185: /* expr ::= expr OR expr */
#line 567 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_OR, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2526 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 186: /* expr ::= expr LT expr */
#line 568 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_LT, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2531 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 187: /* expr ::= expr GT expr */
#line 569 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_GT, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2536 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 188: /* expr ::= expr LE expr */
#line 570 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_LE, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2541 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 189: /* expr ::= expr GE expr */
#line 571 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_GE, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2546 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 190: /* expr ::= expr NE expr */
#line 572 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_NE, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2551 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 191: /* expr ::= expr EQ expr */
#line 573 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_EQ, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2556 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 192: /* expr ::= expr BITAND expr */
#line 574 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_BITAND, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2561 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 193: /* expr ::= expr BITOR expr */
#line 575 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_BITOR, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2566 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 194: /* expr ::= expr LSHIFT expr */
#line 576 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_LSHIFT, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2571 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 195: /* expr ::= expr RSHIFT expr */
#line 577 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_RSHIFT, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2576 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 196: /* expr ::= expr likeop expr */
#line 578 "ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[0].minor.yy242, 0);
  pList = sqliteExprListAppend(pList, yymsp[-2].minor.yy242, 0);
  yygotominor.yy242 = sqliteExprFunction(pList, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->op = yymsp[-1].minor.yy372;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-2].minor.yy242->span, &yymsp[0].minor.yy242->span);
}
#line 2587 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 197: /* expr ::= expr NOT likeop expr */
#line 585 "ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[0].minor.yy242, 0);
  pList = sqliteExprListAppend(pList, yymsp[-3].minor.yy242, 0);
  yygotominor.yy242 = sqliteExprFunction(pList, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->op = yymsp[-1].minor.yy372;
  yygotominor.yy242 = sqliteExpr(TK_NOT, yygotominor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-3].minor.yy242->span,&yymsp[0].minor.yy242->span);
}
#line 2599 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 198: /* likeop ::= LIKE */
#line 594 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_LIKE;}
#line 2604 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 199: /* likeop ::= GLOB */
#line 595 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_GLOB;}
#line 2609 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 200: /* expr ::= expr PLUS expr */
#line 596 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_PLUS, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2614 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 201: /* expr ::= expr MINUS expr */
#line 597 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_MINUS, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2619 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 202: /* expr ::= expr STAR expr */
#line 598 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_STAR, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2624 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 203: /* expr ::= expr SLASH expr */
#line 599 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_SLASH, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2629 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 204: /* expr ::= expr REM expr */
#line 600 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_REM, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2634 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 205: /* expr ::= expr CONCAT expr */
#line 601 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_CONCAT, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2639 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 206: /* expr ::= expr ISNULL */
#line 602 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_ISNULL, yymsp[-1].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2647 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 207: /* expr ::= expr IS NULL */
#line 606 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_ISNULL, yymsp[-2].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-2].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2655 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 208: /* expr ::= expr NOTNULL */
#line 610 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_NOTNULL, yymsp[-1].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2663 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 209: /* expr ::= expr NOT NULL */
#line 614 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_NOTNULL, yymsp[-2].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-2].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2671 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 210: /* expr ::= expr IS NOT NULL */
#line 618 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_NOTNULL, yymsp[-3].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-3].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2679 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 211: /* expr ::= NOT expr */
#line 622 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_NOT, yymsp[0].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy242->span);
}
#line 2687 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 212: /* expr ::= BITNOT expr */
#line 626 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_BITNOT, yymsp[0].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy242->span);
}
#line 2695 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 213: /* expr ::= MINUS expr */
#line 630 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_UMINUS, yymsp[0].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy242->span);
}
#line 2703 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 214: /* expr ::= PLUS expr */
#line 634 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_UPLUS, yymsp[0].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy242->span);
}
#line 2711 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 215: /* expr ::= LP select RP */
#line 638 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_SELECT, 0, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pSelect = yymsp[-1].minor.yy179;
  sqliteExprSpan(yygotominor.yy242,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0);
}
#line 2720 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 216: /* expr ::= expr BETWEEN expr AND expr */
#line 643 "ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[-2].minor.yy242, 0);
  pList = sqliteExprListAppend(pList, yymsp[0].minor.yy242, 0);
  yygotominor.yy242 = sqliteExpr(TK_BETWEEN, yymsp[-4].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pList = pList;
  sqliteExprSpan(yygotominor.yy242,&yymsp[-4].minor.yy242->span,&yymsp[0].minor.yy242->span);
}
#line 2731 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 217: /* expr ::= expr NOT BETWEEN expr AND expr */
#line 650 "ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[-2].minor.yy242, 0);
  pList = sqliteExprListAppend(pList, yymsp[0].minor.yy242, 0);
  yygotominor.yy242 = sqliteExpr(TK_BETWEEN, yymsp[-5].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pList = pList;
  yygotominor.yy242 = sqliteExpr(TK_NOT, yygotominor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-5].minor.yy242->span,&yymsp[0].minor.yy242->span);
}
#line 2743 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 218: /* expr ::= expr IN LP exprlist RP */
#line 658 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-4].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pList = yymsp[-1].minor.yy322;
  sqliteExprSpan(yygotominor.yy242,&yymsp[-4].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2752 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 219: /* expr ::= expr IN LP select RP */
#line 663 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-4].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pSelect = yymsp[-1].minor.yy179;
  sqliteExprSpan(yygotominor.yy242,&yymsp[-4].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2761 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 220: /* expr ::= expr NOT IN LP exprlist RP */
#line 668 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-5].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pList = yymsp[-1].minor.yy322;
  yygotominor.yy242 = sqliteExpr(TK_NOT, yygotominor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-5].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2771 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 221: /* expr ::= expr NOT IN LP select RP */
#line 674 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-5].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pSelect = yymsp[-1].minor.yy179;
  yygotominor.yy242 = sqliteExpr(TK_NOT, yygotominor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-5].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2781 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 222: /* expr ::= expr IN nm dbnm */
#line 680 "ext/sqlite/libsqlite/src/parse.y"
{
  SrcList *pSrc = sqliteSrcListAppend(0, &yymsp[-1].minor.yy0, &yymsp[0].minor.yy0);
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-3].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pSelect = sqliteSelectNew(0,pSrc,0,0,0,0,0,-1,0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-3].minor.yy242->span,yymsp[0].minor.yy0.z?&yymsp[0].minor.yy0:&yymsp[-1].minor.yy0);
}
#line 2791 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 223: /* expr ::= expr NOT IN nm dbnm */
#line 686 "ext/sqlite/libsqlite/src/parse.y"
{
  SrcList *pSrc = sqliteSrcListAppend(0, &yymsp[-1].minor.yy0, &yymsp[0].minor.yy0);
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-4].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pSelect = sqliteSelectNew(0,pSrc,0,0,0,0,0,-1,0);
  yygotominor.yy242 = sqliteExpr(TK_NOT, yygotominor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-4].minor.yy242->span,yymsp[0].minor.yy0.z?&yymsp[0].minor.yy0:&yymsp[-1].minor.yy0);
}
#line 2802 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 224: /* expr ::= CASE case_operand case_exprlist case_else END */
#line 696 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_CASE, yymsp[-3].minor.yy242, yymsp[-1].minor.yy242, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pList = yymsp[-2].minor.yy322;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-4].minor.yy0, &yymsp[0].minor.yy0);
}
#line 2811 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 225: /* case_exprlist ::= case_exprlist WHEN expr THEN expr */
#line 703 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy322 = sqliteExprListAppend(yymsp[-4].minor.yy322, yymsp[-2].minor.yy242, 0);
  yygotominor.yy322 = sqliteExprListAppend(yygotominor.yy322, yymsp[0].minor.yy242, 0);
}
#line 2819 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 226: /* case_exprlist ::= WHEN expr THEN expr */
#line 707 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy322 = sqliteExprListAppend(0, yymsp[-2].minor.yy242, 0);
  yygotominor.yy322 = sqliteExprListAppend(yygotominor.yy322, yymsp[0].minor.yy242, 0);
}
#line 2827 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 235: /* cmd ::= CREATE uniqueflag INDEX nm ON nm dbnm LP idxlist RP onconf */
#line 732 "ext/sqlite/libsqlite/src/parse.y"
{
  SrcList *pSrc = sqliteSrcListAppend(0, &yymsp[-5].minor.yy0, &yymsp[-4].minor.yy0);
  if( yymsp[-9].minor.yy372!=OE_None ) yymsp[-9].minor.yy372 = yymsp[0].minor.yy372;
  if( yymsp[-9].minor.yy372==OE_Default) yymsp[-9].minor.yy372 = OE_Abort;
  sqliteCreateIndex(pParse, &yymsp[-7].minor.yy0, pSrc, yymsp[-2].minor.yy320, yymsp[-9].minor.yy372, &yymsp[-10].minor.yy0, &yymsp[-1].minor.yy0);
}
#line 2837 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 237: /* uniqueflag ::= */
#line 741 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_None; }
#line 2842 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 243: /* cmd ::= DROP INDEX nm dbnm */
#line 758 "ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDropIndex(pParse, sqliteSrcListAppend(0,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0));
}
#line 2849 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 244: /* cmd ::= COPY orconf nm dbnm FROM nm USING DELIMITERS STRING */
#line 766 "ext/sqlite/libsqlite/src/parse.y"
{sqliteCopy(pParse,sqliteSrcListAppend(0,&yymsp[-6].minor.yy0,&yymsp[-5].minor.yy0),&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0,yymsp[-7].minor.yy372);}
#line 2854 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 245: /* cmd ::= COPY orconf nm dbnm FROM nm */
#line 768 "ext/sqlite/libsqlite/src/parse.y"
{sqliteCopy(pParse,sqliteSrcListAppend(0,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0),&yymsp[0].minor.yy0,0,yymsp[-4].minor.yy372);}
#line 2859 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 246: /* cmd ::= VACUUM */
#line 772 "ext/sqlite/libsqlite/src/parse.y"
{sqliteVacuum(pParse,0);}
#line 2864 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 247: /* cmd ::= VACUUM nm */
#line 773 "ext/sqlite/libsqlite/src/parse.y"
{sqliteVacuum(pParse,&yymsp[0].minor.yy0);}
#line 2869 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 248: /* cmd ::= PRAGMA ids EQ nm */
      case 249: /* cmd ::= PRAGMA ids EQ ON */ yytestcase(yyruleno==249);
      case 250: /* cmd ::= PRAGMA ids EQ plus_num */ yytestcase(yyruleno==250);
#line 777 "ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0,0);}
#line 2876 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 251: /* cmd ::= PRAGMA ids EQ minus_num */
#line 780 "ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0,1);}
#line 2881 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 252: /* cmd ::= PRAGMA ids LP nm RP */
#line 781 "ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-3].minor.yy0,&yymsp[-1].minor.yy0,0);}
#line 2886 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 253: /* cmd ::= PRAGMA ids */
#line 782 "ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[0].minor.yy0,&yymsp[0].minor.yy0,0);}
#line 2891 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 260: /* cmd ::= CREATE trigger_decl BEGIN trigger_cmd_list END */
#line 792 "ext/sqlite/libsqlite/src/parse.y"
{
  Token all;
  all.z = yymsp[-4].minor.yy0.z;
  all.n = (yymsp[0].minor.yy0.z - yymsp[-4].minor.yy0.z) + yymsp[0].minor.yy0.n;
  sqliteFinishTrigger(pParse, yymsp[-1].minor.yy19, &all);
}
#line 2901 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 261: /* trigger_decl ::= temp TRIGGER nm trigger_time trigger_event ON nm dbnm foreach_clause when_clause */
#line 800 "ext/sqlite/libsqlite/src/parse.y"
{
  SrcList *pTab = sqliteSrcListAppend(0, &yymsp[-3].minor.yy0, &yymsp[-2].minor.yy0);
  sqliteBeginTrigger(pParse, &yymsp[-7].minor.yy0, yymsp[-6].minor.yy372, yymsp[-5].minor.yy290.a, yymsp[-5].minor.yy290.b, pTab, yymsp[-1].minor.yy372, yymsp[0].minor.yy182, yymsp[-9].minor.yy372);
}
#line 2909 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 262: /* trigger_time ::= BEFORE */
      case 265: /* trigger_time ::= */ yytestcase(yyruleno==265);
#line 806 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = TK_BEFORE; }
#line 2915 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 263: /* trigger_time ::= AFTER */
#line 807 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = TK_AFTER;  }
#line 2920 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 264: /* trigger_time ::= INSTEAD OF */
#line 808 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = TK_INSTEAD;}
#line 2925 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 266: /* trigger_event ::= DELETE */
#line 813 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy290.a = TK_DELETE; yygotominor.yy290.b = 0; }
#line 2930 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 267: /* trigger_event ::= INSERT */
#line 814 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy290.a = TK_INSERT; yygotominor.yy290.b = 0; }
#line 2935 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 268: /* trigger_event ::= UPDATE */
#line 815 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy290.a = TK_UPDATE; yygotominor.yy290.b = 0;}
#line 2940 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 269: /* trigger_event ::= UPDATE OF inscollist */
#line 816 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy290.a = TK_UPDATE; yygotominor.yy290.b = yymsp[0].minor.yy320; }
#line 2945 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 270: /* foreach_clause ::= */
      case 271: /* foreach_clause ::= FOR EACH ROW */ yytestcase(yyruleno==271);
#line 819 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = TK_ROW; }
#line 2951 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 272: /* foreach_clause ::= FOR EACH STATEMENT */
#line 821 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = TK_STATEMENT; }
#line 2956 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 273: /* when_clause ::= */
#line 824 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy182 = 0; }
#line 2961 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 274: /* when_clause ::= WHEN expr */
#line 825 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy182 = yymsp[0].minor.yy242; }
#line 2966 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 275: /* trigger_cmd_list ::= trigger_cmd SEMI trigger_cmd_list */
#line 829 "ext/sqlite/libsqlite/src/parse.y"
{
  yymsp[-2].minor.yy19->pNext = yymsp[0].minor.yy19;
  yygotominor.yy19 = yymsp[-2].minor.yy19;
}
#line 2974 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 276: /* trigger_cmd_list ::= */
#line 833 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy19 = 0; }
#line 2979 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 277: /* trigger_cmd ::= UPDATE orconf nm SET setlist where_opt */
#line 839 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy19 = sqliteTriggerUpdateStep(&yymsp[-3].minor.yy0, yymsp[-1].minor.yy322, yymsp[0].minor.yy242, yymsp[-4].minor.yy372); }
#line 2984 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 278: /* trigger_cmd ::= insert_cmd INTO nm inscollist_opt VALUES LP itemlist RP */
#line 844 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy19 = sqliteTriggerInsertStep(&yymsp[-5].minor.yy0, yymsp[-4].minor.yy320, yymsp[-1].minor.yy322, 0, yymsp[-7].minor.yy372);}
#line 2989 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 279: /* trigger_cmd ::= insert_cmd INTO nm inscollist_opt select */
#line 847 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy19 = sqliteTriggerInsertStep(&yymsp[-2].minor.yy0, yymsp[-1].minor.yy320, 0, yymsp[0].minor.yy179, yymsp[-4].minor.yy372);}
#line 2994 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 280: /* trigger_cmd ::= DELETE FROM nm where_opt */
#line 851 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy19 = sqliteTriggerDeleteStep(&yymsp[-1].minor.yy0, yymsp[0].minor.yy242);}
#line 2999 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 281: /* trigger_cmd ::= select */
#line 854 "ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy19 = sqliteTriggerSelectStep(yymsp[0].minor.yy179); }
#line 3004 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 282: /* expr ::= RAISE LP IGNORE RP */
#line 857 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_RAISE, 0, 0, 0); 
  yygotominor.yy242->iColumn = OE_Ignore;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-3].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3013 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 283: /* expr ::= RAISE LP ROLLBACK COMMA nm RP */
#line 862 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy0); 
  yygotominor.yy242->iColumn = OE_Rollback;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3022 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 284: /* expr ::= RAISE LP ABORT COMMA nm RP */
#line 867 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy0); 
  yygotominor.yy242->iColumn = OE_Abort;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3031 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 285: /* expr ::= RAISE LP FAIL COMMA nm RP */
#line 872 "ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy0); 
  yygotominor.yy242->iColumn = OE_Fail;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3040 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 286: /* cmd ::= DROP TRIGGER nm dbnm */
#line 879 "ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDropTrigger(pParse,sqliteSrcListAppend(0,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0));
}
#line 3047 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 287: /* cmd ::= ATTACH database_kw_opt ids AS nm key_opt */
#line 884 "ext/sqlite/libsqlite/src/parse.y"
{
  sqliteAttach(pParse, &yymsp[-3].minor.yy0, &yymsp[-1].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3054 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 289: /* key_opt ::= */
#line 889 "ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy0.z = 0; yygotominor.yy0.n = 0; }
#line 3059 "ext/sqlite/libsqlite/src/parse.c"
        break;
      case 292: /* cmd ::= DETACH database_kw_opt nm */
#line 895 "ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDetach(pParse, &yymsp[0].minor.yy0);
}
#line 3066 "ext/sqlite/libsqlite/src/parse.c"
        break;
      default:
      /* (0) input ::= cmdlist */ yytestcase(yyruleno==0);
      /* (1) cmdlist ::= cmdlist ecmd */ yytestcase(yyruleno==1);
      /* (2) cmdlist ::= ecmd */ yytestcase(yyruleno==2);
      /* (3) ecmd ::= explain cmdx SEMI */ yytestcase(yyruleno==3);
      /* (4) ecmd ::= SEMI */ yytestcase(yyruleno==4);
      /* (9) trans_opt ::= */ yytestcase(yyruleno==9);
      /* (10) trans_opt ::= TRANSACTION */ yytestcase(yyruleno==10);
      /* (11) trans_opt ::= TRANSACTION nm */ yytestcase(yyruleno==11);
      /* (15) cmd ::= create_table create_table_args */ yytestcase(yyruleno==15);
      /* (21) columnlist ::= columnlist COMMA column */ yytestcase(yyruleno==21);
      /* (22) columnlist ::= column */ yytestcase(yyruleno==22);
      /* (23) column ::= columnid type carglist */ yytestcase(yyruleno==23);
      /* (31) type ::= */ yytestcase(yyruleno==31);
      /* (40) carglist ::= carglist carg */ yytestcase(yyruleno==40);
      /* (41) carglist ::= */ yytestcase(yyruleno==41);
      /* (42) carg ::= CONSTRAINT nm ccons */ yytestcase(yyruleno==42);
      /* (43) carg ::= ccons */ yytestcase(yyruleno==43);
      /* (52) carg ::= DEFAULT NULL */ yytestcase(yyruleno==52);
      /* (53) ccons ::= NULL onconf */ yytestcase(yyruleno==53);
      /* (76) conslist_opt ::= */ yytestcase(yyruleno==76);
      /* (77) conslist_opt ::= COMMA conslist */ yytestcase(yyruleno==77);
      /* (78) conslist ::= conslist COMMA tcons */ yytestcase(yyruleno==78);
      /* (79) conslist ::= conslist tcons */ yytestcase(yyruleno==79);
      /* (80) conslist ::= tcons */ yytestcase(yyruleno==80);
      /* (81) tcons ::= CONSTRAINT nm */ yytestcase(yyruleno==81);
      /* (258) plus_opt ::= PLUS */ yytestcase(yyruleno==258);
      /* (259) plus_opt ::= */ yytestcase(yyruleno==259);
      /* (290) database_kw_opt ::= DATABASE */ yytestcase(yyruleno==290);
      /* (291) database_kw_opt ::= */ yytestcase(yyruleno==291);
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  sqliteParserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  sqliteParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  sqliteParserARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 23 "ext/sqlite/libsqlite/src/parse.y"

  if( pParse->zErrMsg==0 ){
    if( TOKEN.z[0] ){
      sqliteErrorMsg(pParse, "near \"%T\": syntax error", &TOKEN);
    }else{
      sqliteErrorMsg(pParse, "incomplete SQL statement");
    }
  }
#line 3166 "ext/sqlite/libsqlite/src/parse.c"
  sqliteParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  sqliteParserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  sqliteParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "sqliteParserAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void sqliteParser(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  sqliteParserTOKENTYPE yyminor       /* The value for the token */
  sqliteParserARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  sqliteParserARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor,yyminorunion);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
