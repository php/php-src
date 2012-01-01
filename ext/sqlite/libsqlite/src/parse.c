/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 33 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"

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

#line 33 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
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
#define YY_ACTTAB_COUNT (1143)
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
 /*   190 */   194,  175,  320,  230,  231,  344,  342,   36,  341,   56,
 /*   200 */    55,   54,   53,  212,  531,  514,  341,  544,  551,    3,
 /*   210 */   515,  551,  362,  551,  508,  507,  184,  132,  195,  213,
 /*   220 */   551,  309,  348,   42,   71,   72,  129,   65,   66,  513,
 /*   230 */   510,  305,   52,  138,   69,   67,   70,   68,   64,   63,
 /*   240 */    62,   61,   58,   57,   56,   55,   54,   53,  243,  197,
 /*   250 */   282,   73,  268,  373,  264,  372,  183,  241,    2,  358,
 /*   260 */     7,  563,  551,  240,  551,  471,  385,  201,  356,  514,
 /*   270 */   379,  378,  550,  549,  515,  550,  549,  550,  549,  500,
 /*   280 */   506,   17,  172,  354,  550,  549,  359,  392,   71,   72,
 /*   290 */   129,   65,   66,  513,  510,  305,   52,  138,   69,   67,
 /*   300 */    70,   68,   64,   63,   62,   61,   58,   57,   56,   55,
 /*   310 */    54,   53,  486,  365,  416,  551,  514,  417,  493,  494,
 /*   320 */    17,  515,  169,  478,  171,   80,  550,  549,  550,  549,
 /*   330 */   466,  108,   17,  144,  332,   71,   72,  129,   65,   66,
 /*   340 */   513,  510,  305,   52,  138,   69,   67,   70,   68,   64,
 /*   350 */    63,   62,   61,   58,   57,   56,   55,   54,   53,  446,
 /*   360 */   182,  488,  445,  514,  366,  249,  490,  362,  515,  508,
 /*   370 */   507,  429,  463,  444,  375,  338,  443,   12,  430,  550,
 /*   380 */   549,  592,   71,   72,  129,   65,   66,  513,  510,  305,
 /*   390 */    52,  138,   69,   67,   70,   68,   64,   63,   62,   61,
 /*   400 */    58,   57,   56,   55,   54,   53,  362,  485,  508,  507,
 /*   410 */   514,  459,  286,  460,  436,  515,  388,  240,  269,  387,
 /*   420 */   182,  426,   29,  857,   82,  377,    1,  134,  593,   71,
 /*   430 */    51,  129,   65,   66,  513,  510,  305,   52,  138,   69,
 /*   440 */    67,   70,   68,   64,   63,   62,   61,   58,   57,   56,
 /*   450 */    55,   54,   53,  551,  487,  177,  365,  514,   87,  483,
 /*   460 */   542,  450,  515,  454,  160,   80,  453,  440,  239,  176,
 /*   470 */    80,  238,  286,   80,  106,  458,   71,   40,  129,   65,
 /*   480 */    66,  513,  510,  305,   52,  138,   69,   67,   70,   68,
 /*   490 */    64,   63,   62,   61,   58,   57,   56,   55,   54,   53,
 /*   500 */   457,  393,  534,  418,  514,  186,  527,  483,  156,  515,
 /*   510 */   351,  323,  533,  339,  232,   80,   80,  550,  549,   46,
 /*   520 */   498,   44,  526,  482,   72,  129,   65,   66,  513,  510,
 /*   530 */   305,   52,  138,   69,   67,   70,   68,   64,   63,   62,
 /*   540 */    61,   58,   57,   56,   55,   54,   53,  365,  153,  300,
 /*   550 */   155,  514,  405,  431,  165,  404,  515,   32,  396,   13,
 /*   560 */    32,   86,  333,  323,   38,  106,  205,  414,  391,  390,
 /*   570 */   202,  482,  129,   65,   66,  513,  510,  305,   52,  138,
 /*   580 */    69,   67,   70,   68,   64,   63,   62,   61,   58,   57,
 /*   590 */    56,   55,   54,   53,  274,  309,  272,  271,  270,   23,
 /*   600 */     8,  301,  211,   11,  307,  418,  212,  365,  483,   31,
 /*   610 */   217,  498,  562,    1,   60,  219,  480,  365,  204,  321,
 /*   620 */   215,  154,  207,  137,  480,   93,  268,  373,  264,  372,
 /*   630 */   183,  241,  365,  360,  203,  106,  409,  240,  365,  207,
 /*   640 */   365,  145,  541,   50,  412,  365,  247,  453,  531,  302,
 /*   650 */   105,  135,  173,    8,  245,  170,  106,  307,  108,   47,
 /*   660 */    48,  346,   19,  106,  290,  289,  299,   49,  371,  370,
 /*   670 */   518,  304,  482,  517,  520,  158,  531,  531,  193,  800,
 /*   680 */   531,  498,  207,  319,  223,  231,  360,  207,  475,  283,
 /*   690 */   514,  162,  257,  252,  212,  515,   50,  365,  511,  303,
 /*   700 */   531,  557,  499,  192,    8,  475,  516,   18,  307,  498,
 /*   710 */   556,  365,   47,   48,  295,   93,  531,   41,  136,  207,
 /*   720 */    49,  371,  370,  518,  148,  555,  517,  520,  554,   89,
 /*   730 */   376,  560,  365,  162,  509,  214,  480,  360,  365,  553,
 /*   740 */   152,  322,  495,  505,  141,  212,  294,   50,  477,  531,
 /*   750 */   112,  284,  207,  475,  504,  288,   90,  327,  531,  516,
 /*   760 */    18,  207,  326,   47,   48,  531,  473,  212,  365,  531,
 /*   770 */   191,   49,  371,  370,  518,  365,  365,  517,  520,  293,
 /*   780 */   316,  503,  365,  322,   60,  475,  109,  365,  291,  190,
 /*   790 */   317,  365,  502,  121,  120,  365,  189,  365,  365,  407,
 /*   800 */   119,  365,  365,  501,  326,  118,  365,  365,  365,  117,
 /*   810 */   516,   18,  365,  116,  365,  126,  125,  365,  475,  124,
 /*   820 */   123,  365,  365,  365,  115,  114,  140,  475,  365,  365,
 /*   830 */   139,  365,  255,  365,  365,  254,  365,  365,  365,  253,
 /*   840 */   280,  107,  365,  365,   10,  365,  113,  111,  365,  279,
 /*   850 */   365,  278,  277,  365,   92,  104,  103,  365,  365,  365,
 /*   860 */    91,  102,  365,  101,  365,  365,  110,  347,  100,  379,
 /*   870 */   378,   99,   17,  484,   28,   98,   30,   97,   79,  131,
 /*   880 */    96,  465,   95,   94,  439,   14,  343,   26,   25,  340,
 /*   890 */    24,  174,  266,   85,  166,  292,   78,  165,  415,  164,
 /*   900 */   163,   22,    6,  408,    5,   77,   34,   33,  159,   16,
 /*   910 */   157,  151,   75,  149,  146,   15,  312,  395,  384,  313,
 /*   920 */   143,   20,  206,   60,   21,  273,  198,  375,  559,  548,
 /*   930 */   547,  546,    4,  212,  538,  261,  308,   38,  540,  530,
 /*   940 */   260,   39,  352,  374,  539,  535,  532,  196,  367,  210,
 /*   950 */   256,  259,  522,   53,  258,  521,  529,  209,  188,   43,
 /*   960 */    27,   37,  349,  496,  492,  452,   81,  276,  246,  275,
 /*   970 */   479,  244,  208,  464,  474,  451,  433,  335,  432,  413,
 /*   980 */   234,   35,  127,   76,  381,  858,  410,  161,  858,  199,
 /*   990 */   858,  858,  858,  858,  858,  858,  403,  858,  235,  858,
 /*  1000 */   858,  858,  329,  858,  150,  398,  858,  545,  858,  858,
 /*  1010 */   400,  399,  858,  224,  858,  858,   84,  147,  858,  318,
 /*  1020 */   265,  858,  220,  858,  858,  858,  858,  552,   74,  424,
 /*  1030 */   858,  523,  858,  858,  491,  476,  287,  315,  858,   83,
 /*  1040 */    88,  262,  463,  364,  519,  310,  382,  357,  355,  353,
 /*  1050 */   858,  306,  497,  481,  525,  406,  524,  242,  462,  397,
 /*  1060 */   434,  237,  858,  425,  337,  168,  858,  438,  461,  858,
 /*  1070 */   296,  200,  236,  419,  456,  383,  330,  142,  263,  361,
 /*  1080 */   345,  858,  455,  233,  325,  324,  489,  229,  228,  402,
 /*  1090 */   401,  858,  227,  226,  225,  389,  386,  222,  221,  218,
 /*  1100 */   314,  394,  311,  216,  558,  380,  251,  250,  133,  350,
 /*  1110 */   248,  858,  435,  858,  858,  858,  858,  858,  858,  858,
 /*  1120 */   858,  858,  428,  858,  858,  411,  858,  858,  858,  858,
 /*  1130 */    59,  858,  858,  858,  858,  858,  858,  858,  858,  858,
 /*  1140 */   167,  858,  336,
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
 /*   190 */    22,   90,  100,  101,  102,   94,   95,   96,  105,   80,
 /*   200 */    81,   82,   83,  111,  171,   41,  105,   20,   23,   19,
 /*   210 */    46,   23,  107,   23,  109,  110,  183,  184,   50,   48,
 /*   220 */    23,   62,  189,  128,   60,   61,   62,   63,   64,   65,
 /*   230 */    66,   67,   68,   69,   70,   71,   72,   73,   74,   75,
 /*   240 */    76,   77,   78,   79,   80,   81,   82,   83,   20,   90,
 /*   250 */    91,   19,   93,   94,   95,   96,   97,   98,   19,   15,
 /*   260 */    19,    0,   23,  104,   23,   80,   78,   79,   24,   41,
 /*   270 */     9,   10,   87,   88,   46,   87,   88,   87,   88,   20,
 /*   280 */    20,   22,   17,   39,   87,   88,   42,   99,   60,   61,
 /*   290 */    62,   63,   64,   65,   66,   67,   68,   69,   70,   71,
 /*   300 */    72,   73,   74,   75,   76,   77,   78,   79,   80,   81,
 /*   310 */    82,   83,  115,  140,   25,   23,   41,   28,   20,   20,
 /*   320 */    22,   46,   57,   20,   59,   22,   87,   88,   87,   88,
 /*   330 */    20,  158,   22,   19,   45,   60,   61,   62,   63,   64,
 /*   340 */    65,   66,   67,   68,   69,   70,   71,   72,   73,   74,
 /*   350 */    75,   76,   77,   78,   79,   80,   81,   82,   83,   89,
 /*   360 */    47,   22,   92,   41,  191,  192,   22,  107,   46,  109,
 /*   370 */   110,  161,  162,   89,   99,   62,   92,   63,  168,   87,
 /*   380 */    88,  113,   60,   61,   62,   63,   64,   65,   66,   67,
 /*   390 */    68,   69,   70,   71,   72,   73,   74,   75,   76,   77,
 /*   400 */    78,   79,   80,   81,   82,   83,  107,  115,  109,  110,
 /*   410 */    41,   91,   99,   93,  140,   46,   89,  104,  144,   92,
 /*   420 */    47,  147,  148,  132,  133,  134,  135,   88,  113,   60,
 /*   430 */    61,   62,   63,   64,   65,   66,   67,   68,   69,   70,
 /*   440 */    71,   72,   73,   74,   75,   76,   77,   78,   79,   80,
 /*   450 */    81,   82,   83,   23,  115,   19,  140,   41,  114,   23,
 /*   460 */   155,  156,   46,   20,   22,   22,  161,   20,   20,   22,
 /*   470 */    22,   20,   99,   22,  158,   29,   60,   61,   62,   63,
 /*   480 */    64,   65,   66,   67,   68,   69,   70,   71,   72,   73,
 /*   490 */    74,   75,   76,   77,   78,   79,   80,   81,   82,   83,
 /*   500 */    54,  140,   26,  140,   41,   21,   89,   23,   17,   46,
 /*   510 */   194,  140,   36,   20,   20,   22,   22,   87,   88,  127,
 /*   520 */   204,  129,   89,   87,   61,   62,   63,   64,   65,   66,
 /*   530 */    67,   68,   69,   70,   71,   72,   73,   74,   75,   76,
 /*   540 */    77,   78,   79,   80,   81,   82,   83,  140,   57,  103,
 /*   550 */    59,   41,   20,  139,   22,   20,   46,   22,   20,   19,
 /*   560 */    22,   21,  199,  140,  122,  158,  195,   18,  207,  208,
 /*   570 */   209,   87,   62,   63,   64,   65,   66,   67,   68,   69,
 /*   580 */    70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
 /*   590 */    80,   81,   82,   83,   11,   62,   13,   14,   15,   16,
 /*   600 */    19,  194,  174,  118,   23,  140,  111,  140,   23,  181,
 /*   610 */    27,  204,  134,  135,  200,   32,  152,  140,  195,  124,
 /*   620 */    37,  130,  140,  146,  152,  158,   93,   94,   95,   96,
 /*   630 */    97,   98,  140,   52,   51,  158,   53,  104,  140,  140,
 /*   640 */   140,   58,  156,   62,   95,  140,  182,  161,  171,  167,
 /*   650 */   158,  146,  146,   19,  182,  146,  158,   23,  158,   78,
 /*   660 */    79,   80,   22,  158,  199,  198,  167,   86,   87,   88,
 /*   670 */    89,  194,   87,   92,   93,  146,  171,  171,  113,  130,
 /*   680 */   171,  204,  140,  100,  101,  102,   52,  140,  206,  106,
 /*   690 */    41,  146,  192,  201,  111,   46,   62,  140,   93,  194,
 /*   700 */   171,   15,  204,  113,   19,  206,  125,  126,   23,  204,
 /*   710 */    24,  140,   78,   79,  167,  158,  171,   68,   69,  140,
 /*   720 */    86,   87,   88,   89,  146,   39,   92,   93,   42,  158,
 /*   730 */   136,  137,  140,  146,  108,  141,  152,   52,  140,   53,
 /*   740 */   146,  196,   93,   14,  146,  111,  167,   62,  206,  171,
 /*   750 */   158,   19,  140,  206,   20,  198,  158,  212,  171,  125,
 /*   760 */   126,  140,  217,   78,   79,  171,  182,  111,  140,  171,
 /*   770 */    22,   86,   87,   88,   89,  140,  140,   92,   93,  167,
 /*   780 */   124,   20,  140,  196,  200,  206,  158,  140,  167,   22,
 /*   790 */   196,  140,   20,  158,  158,  140,   22,  140,  140,  212,
 /*   800 */   158,  140,  140,   20,  217,  158,  140,  140,  140,  158,
 /*   810 */   125,  126,  140,  158,  140,  158,  158,  140,  206,  158,
 /*   820 */   158,  140,  140,  140,  158,  158,  158,  206,  140,  140,
 /*   830 */   158,  140,  158,  140,  140,  158,  140,  140,  140,  158,
 /*   840 */   158,  158,  140,  140,  118,  140,  158,  158,  140,  158,
 /*   850 */   140,  158,  158,  140,  158,  158,  158,  140,  140,  140,
 /*   860 */   158,  158,  140,  158,  140,  140,  158,  140,  158,    9,
 /*   870 */    10,  158,   22,  115,   19,  158,  158,  158,   20,  113,
 /*   880 */   158,   20,  158,  158,   20,  158,   48,   19,   19,   48,
 /*   890 */    19,   97,  104,   21,   99,   38,   49,   22,   49,  130,
 /*   900 */    99,   19,   11,   14,    9,  103,   63,   63,  123,   19,
 /*   910 */   114,  114,  103,  123,  114,   19,   35,   87,   20,  116,
 /*   920 */    21,  150,  160,  200,  160,  138,   12,   99,  139,  138,
 /*   930 */   138,  138,   22,  111,  164,  176,   44,  122,  139,  171,
 /*   940 */   177,  120,  119,  145,  139,  139,  139,  121,  117,  193,
 /*   950 */    98,  178,   23,   83,  179,  151,  180,  202,  113,  127,
 /*   960 */    19,   99,  116,  186,  186,  151,  186,  159,  187,  159,
 /*   970 */   188,  187,  193,  139,  188,  164,  139,   40,  139,  139,
 /*   980 */   215,  127,   60,   19,   33,  220,  169,  169,  220,  116,
 /*   990 */   220,  220,  220,  220,  220,  220,  176,  220,  186,  220,
 /*  1000 */   220,  220,  186,  220,  169,  176,  220,  142,  220,  220,
 /*  1010 */   186,  186,  220,  186,  220,  220,  197,  169,  220,  186,
 /*  1020 */   157,  220,  186,  220,  220,  220,  220,  140,  140,  216,
 /*  1030 */   220,  157,  220,  220,  157,  157,  152,  186,  220,  197,
 /*  1040 */   173,  175,  162,  140,  140,  152,  152,  140,  140,  140,
 /*  1050 */   220,  154,  140,  140,  154,  176,  154,  140,  140,  176,
 /*  1060 */   140,  140,  220,  140,  140,  140,  220,  152,  163,  220,
 /*  1070 */   154,  218,  140,  140,  163,  219,  140,  218,  172,  203,
 /*  1080 */   154,  220,  163,  140,  140,  140,  185,  140,  140,  140,
 /*  1090 */   140,  220,  140,  140,  140,  210,  210,  140,  140,  140,
 /*  1100 */   140,  140,  140,  140,  170,  140,  140,  140,  140,  140,
 /*  1110 */   140,  220,  166,  220,  220,  220,  220,  220,  220,  220,
 /*  1120 */   220,  220,  166,  220,  220,  170,  220,  220,  220,  220,
 /*  1130 */   200,  220,  220,  220,  220,  220,  220,  220,  220,  220,
 /*  1140 */   213,  220,  214,
};
#define YY_SHIFT_USE_DFLT (-84)
#define YY_SHIFT_COUNT (376)
#define YY_SHIFT_MIN   (-83)
#define YY_SHIFT_MAX   (964)
static const short yy_shift_ofst[] = {
 /*     0 */   860,  583,  634,  634,   93,   92,   92,  822,  634,  581,
 /*    10 */   685,  685,  188,  430,  -21,  685,  685,  685,  685,  685,
 /*    20 */   159,  313,  430,  549,  430,  430,  430,  430,  430,  585,
 /*    30 */   275,   60,  685,  685,  685,  685,  685,  685,  685,  685,
 /*    40 */   685,  685,  685,  685,  685,  685,  685,  685,  685,  685,
 /*    50 */   685,  685,  685,  685,  685,  685,  685,  685,  685,  685,
 /*    60 */   685,  685,  685,  685,  685,  685,  685,  685,  685,  685,
 /*    70 */   685,  685,  685,  685,  533,  430,  430,  430,  430,  484,
 /*    80 */   430,  484,  261,  656,  495,  822,  822,  -84,  -84,  228,
 /*    90 */   164,   95,   26,  322,  322,  322,  322,  322,  322,  322,
 /*   100 */   322,  416,  322,  322,  322,  322,  322,  369,  322,  463,
 /*   110 */   510,  510,  510,  -67,  -67,  -67,  -67,  -67,  -48,  -48,
 /*   120 */   -48,  -48,  101,   -5,   -5,   -5,   -5,  686,  -25,  649,
 /*   130 */   686,  185,  241,  292,  197,  299,  239,  260,  190,  119,
 /*   140 */   119,    4,  430,  430,  430,  430,  430,  430,  105,  430,
 /*   150 */   430,  430,  105,  430,  430,  430,  430,  430,  105,  430,
 /*   160 */   430,  430,  105,  430,  430,  430,  430,  -79,  289,  430,
 /*   170 */   105,  430,  430,  105,  430,  430,   42,   42,  436,  446,
 /*   180 */   446,  446,  430,  430,  339,  105,  430,  339,  430,  430,
 /*   190 */   430,  430,  430,  430,   42,   42,   42,  430,  430,  585,
 /*   200 */   585,  327,  327,  585,  442,  442,  373,  476,  476,  392,
 /*   210 */   476,  344,  -44,  476,  540,  951,  873,  951,  845,  922,
 /*   220 */   964,  845,  845,  922,  815,  845,  845,  845,  815,  964,
 /*   230 */   922,  922,  828,  845,  854,  937,  845,  941,  828,  828,
 /*   240 */   892,  929,  941,  828,  846,  862,  846,  862,  845,  852,
 /*   250 */   845,  845,  832,  870,  870,  870,  929,  852,  826,  831,
 /*   260 */   821,  823,  815,  822,  828,  828,  892,  828,  828,  910,
 /*   270 */   914,  914,  914,  828,  914,  -84,  -84,  -84,  -84,  -84,
 /*   280 */   -84,  -84,   40,  491,  244,  265,  -83,  314,  538,  535,
 /*   290 */   532,  494,  -18,  493,  451,  448,  447,  284,  270,  443,
 /*   300 */   320,  310,  303,  298,  259,   89,  168,   17,   94,   22,
 /*   310 */   899,  898,  830,  881,  803,  800,  896,  790,  809,  797,
 /*   320 */   796,  890,  785,  844,  843,  802,  895,  889,  891,  882,
 /*   330 */   801,  769,  849,  875,  847,  857,  795,  872,  788,  794,
 /*   340 */   871,  841,  869,  868,  838,  864,  861,  766,  858,  855,
 /*   350 */   758,  850,  726,  783,  774,  772,  767,  761,  748,  734,
 /*   360 */   732,  729,  626,  605,  590,  565,  640,  485,  433,  417,
 /*   370 */   315,  268,  232,  171,  187,   79,   -8,
};
#define YY_REDUCE_USE_DFLT (-69)
#define YY_REDUCE_COUNT (281)
#define YY_REDUCE_MIN   (-68)
#define YY_REDUCE_MAX   (970)
static const short yy_reduce_ofst[] = {
 /*     0 */   291,  594,  505,  477,  -23,  587,  545,   33,  598,  407,
 /*    10 */   316,  173,  361,  274,  584,  557,  467,  498,  492,  500,
 /*    20 */   305,  210,  621,  -61,  612,  579,  547,  499,  482,  -68,
 /*    30 */   414,  727,  725,  724,  722,  719,  718,  717,  713,  710,
 /*    40 */   708,  705,  703,  702,  698,  697,  696,  694,  693,  691,
 /*    50 */   689,  688,  683,  682,  681,  677,  674,  672,  668,  667,
 /*    60 */   666,  662,  661,  658,  657,  655,  651,  647,  642,  636,
 /*    70 */   635,  628,  592,  571,  486,  423,  465,  371,  363,  472,
 /*    80 */   542,  464,  478,  578,  529,  509,  506,  -49,  428,  723,
 /*    90 */   723,  723,  723,  723,  723,  723,  723,  723,  723,  723,
 /*   100 */   723,  723,  723,  723,  723,  723,  723,  723,  723,  723,
 /*   110 */   723,  723,  723,  723,  723,  723,  723,  723,  723,  723,
 /*   120 */   723,  723,  956,  723,  723,  723,  723,  955,  946,  930,
 /*   130 */   934,  903,  970,  969,  968,  906,  967,  906,  966,  723,
 /*   140 */   723,  906,  965,  963,  962,  961,  960,  959,  906,  958,
 /*   150 */   957,  954,  906,  953,  952,  950,  949,  948,  906,  947,
 /*   160 */   945,  944,  906,  943,  936,  933,  932,  928,  927,  925,
 /*   170 */   906,  924,  923,  906,  921,  920,  926,  916,  915,  919,
 /*   180 */   911,  905,  918,  917,  901,  906,  913,  901,  912,  909,
 /*   190 */   908,  907,  904,  903,  902,  900,  897,  888,  887,  894,
 /*   200 */   893,  886,  885,  884,  883,  879,  880,  878,  877,  876,
 /*   210 */   874,  866,  867,  863,  865,  859,  856,  853,  851,  848,
 /*   220 */   842,  836,  833,  835,  829,  827,  825,  824,  820,  819,
 /*   230 */   818,  817,  840,  816,  813,  765,  812,  810,  839,  837,
 /*   240 */   811,  814,  808,  834,  786,  784,  782,  781,  780,  779,
 /*   250 */   778,  777,  755,  723,  723,  723,  804,  756,  776,  775,
 /*   260 */   773,  763,  759,  768,  807,  806,  770,  805,  799,  798,
 /*   270 */   793,  792,  791,  789,  787,  764,  762,  723,  723,  723,
 /*   280 */   723,  771,
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
#line 286 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
sqliteSelectDelete((yypminor->yy179));
#line 1143 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 158: /* expr */
    case 176: /* where_opt */
    case 178: /* having_opt */
    case 187: /* on_opt */
    case 192: /* sortitem */
    case 204: /* expritem */
{
#line 533 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
sqliteExprDelete((yypminor->yy242));
#line 1155 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 159: /* idxlist_opt */
    case 167: /* idxlist */
    case 188: /* using_opt */
    case 197: /* inscollist_opt */
    case 199: /* inscollist */
{
#line 746 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
sqliteIdListDelete((yypminor->yy320));
#line 1166 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
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
#line 322 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
sqliteExprListDelete((yypminor->yy322));
#line 1181 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 175: /* from */
    case 183: /* seltablist */
    case 184: /* stl_prefix */
{
#line 353 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
sqliteSrcListDelete((yypminor->yy307));
#line 1190 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 212: /* trigger_cmd_list */
    case 217: /* trigger_cmd */
{
#line 828 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
sqliteDeleteTriggerStep((yypminor->yy19));
#line 1198 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
}
      break;
    case 214: /* trigger_event */
{
#line 812 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
sqliteIdListDelete((yypminor->yy290).b);
#line 1205 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
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
#line 72 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ sqliteExec(pParse); }
#line 1793 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 6: /* explain ::= EXPLAIN */
#line 73 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ sqliteBeginParse(pParse, 1); }
#line 1798 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 7: /* explain ::= */
#line 74 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ sqliteBeginParse(pParse, 0); }
#line 1803 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 8: /* cmd ::= BEGIN trans_opt onconf */
#line 79 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteBeginTransaction(pParse,yymsp[0].minor.yy372);}
#line 1808 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 12: /* cmd ::= COMMIT trans_opt */
      case 13: /* cmd ::= END trans_opt */ yytestcase(yyruleno==13);
#line 83 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteCommitTransaction(pParse);}
#line 1814 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 14: /* cmd ::= ROLLBACK trans_opt */
#line 85 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteRollbackTransaction(pParse);}
#line 1819 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 16: /* create_table ::= CREATE temp TABLE nm */
#line 90 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
   sqliteStartTable(pParse,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0,yymsp[-2].minor.yy372,0);
}
#line 1826 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 17: /* temp ::= TEMP */
      case 74: /* init_deferred_pred_opt ::= INITIALLY DEFERRED */ yytestcase(yyruleno==74);
      case 108: /* distinct ::= DISTINCT */ yytestcase(yyruleno==108);
#line 94 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = 1;}
#line 1833 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 18: /* temp ::= */
      case 73: /* init_deferred_pred_opt ::= */ yytestcase(yyruleno==73);
      case 75: /* init_deferred_pred_opt ::= INITIALLY IMMEDIATE */ yytestcase(yyruleno==75);
      case 86: /* defer_subclause_opt ::= */ yytestcase(yyruleno==86);
      case 109: /* distinct ::= ALL */ yytestcase(yyruleno==109);
      case 110: /* distinct ::= */ yytestcase(yyruleno==110);
#line 95 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = 0;}
#line 1843 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 19: /* create_table_args ::= LP columnlist conslist_opt RP */
#line 96 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteEndTable(pParse,&yymsp[0].minor.yy0,0);
}
#line 1850 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 20: /* create_table_args ::= AS select */
#line 99 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteEndTable(pParse,0,yymsp[0].minor.yy179);
  sqliteSelectDelete(yymsp[0].minor.yy179);
}
#line 1858 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 24: /* columnid ::= nm */
#line 111 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumn(pParse,&yymsp[0].minor.yy0);}
#line 1863 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
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
#line 117 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy0 = yymsp[0].minor.yy0;}
#line 1879 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 32: /* type ::= typename */
#line 160 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[0].minor.yy0,&yymsp[0].minor.yy0);}
#line 1884 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 33: /* type ::= typename LP signed RP */
#line 161 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);}
#line 1889 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 34: /* type ::= typename LP signed COMMA signed RP */
#line 163 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[-5].minor.yy0,&yymsp[0].minor.yy0);}
#line 1894 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 36: /* typename ::= typename ids */
      case 242: /* idxitem ::= nm sortorder */ yytestcase(yyruleno==242);
#line 166 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy0 = yymsp[-1].minor.yy0;}
#line 1900 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 37: /* signed ::= INTEGER */
      case 38: /* signed ::= PLUS INTEGER */ yytestcase(yyruleno==38);
#line 168 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = atoi(yymsp[0].minor.yy0.z); }
#line 1906 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 39: /* signed ::= MINUS INTEGER */
#line 170 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = -atoi(yymsp[0].minor.yy0.z); }
#line 1911 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 44: /* carg ::= DEFAULT STRING */
      case 45: /* carg ::= DEFAULT ID */ yytestcase(yyruleno==45);
      case 46: /* carg ::= DEFAULT INTEGER */ yytestcase(yyruleno==46);
      case 47: /* carg ::= DEFAULT PLUS INTEGER */ yytestcase(yyruleno==47);
      case 49: /* carg ::= DEFAULT FLOAT */ yytestcase(yyruleno==49);
      case 50: /* carg ::= DEFAULT PLUS FLOAT */ yytestcase(yyruleno==50);
#line 175 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 1921 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 48: /* carg ::= DEFAULT MINUS INTEGER */
      case 51: /* carg ::= DEFAULT MINUS FLOAT */ yytestcase(yyruleno==51);
#line 179 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,1);}
#line 1927 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 54: /* ccons ::= NOT NULL onconf */
#line 189 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddNotNull(pParse, yymsp[0].minor.yy372);}
#line 1932 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 55: /* ccons ::= PRIMARY KEY sortorder onconf */
#line 190 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddPrimaryKey(pParse,0,yymsp[0].minor.yy372);}
#line 1937 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 56: /* ccons ::= UNIQUE onconf */
#line 191 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteCreateIndex(pParse,0,0,0,yymsp[0].minor.yy372,0,0);}
#line 1942 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 57: /* ccons ::= CHECK LP expr RP onconf */
#line 192 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yy_destructor(yypParser,158,&yymsp[-2].minor);
}
#line 1949 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 58: /* ccons ::= REFERENCES nm idxlist_opt refargs */
#line 194 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteCreateForeignKey(pParse,0,&yymsp[-2].minor.yy0,yymsp[-1].minor.yy320,yymsp[0].minor.yy372);}
#line 1954 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 59: /* ccons ::= defer_subclause */
#line 195 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteDeferForeignKey(pParse,yymsp[0].minor.yy372);}
#line 1959 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 60: /* ccons ::= COLLATE id */
#line 196 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
   sqliteAddCollateType(pParse, sqliteCollateType(yymsp[0].minor.yy0.z, yymsp[0].minor.yy0.n));
}
#line 1966 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 61: /* refargs ::= */
#line 206 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Restrict * 0x010101; }
#line 1971 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 62: /* refargs ::= refargs refarg */
#line 207 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = (yymsp[-1].minor.yy372 & yymsp[0].minor.yy407.mask) | yymsp[0].minor.yy407.value; }
#line 1976 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 63: /* refarg ::= MATCH nm */
#line 209 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy407.value = 0;     yygotominor.yy407.mask = 0x000000; }
#line 1981 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 64: /* refarg ::= ON DELETE refact */
#line 210 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy407.value = yymsp[0].minor.yy372;     yygotominor.yy407.mask = 0x0000ff; }
#line 1986 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 65: /* refarg ::= ON UPDATE refact */
#line 211 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy407.value = yymsp[0].minor.yy372<<8;  yygotominor.yy407.mask = 0x00ff00; }
#line 1991 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 66: /* refarg ::= ON INSERT refact */
#line 212 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy407.value = yymsp[0].minor.yy372<<16; yygotominor.yy407.mask = 0xff0000; }
#line 1996 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 67: /* refact ::= SET NULL */
#line 214 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_SetNull; }
#line 2001 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 68: /* refact ::= SET DEFAULT */
#line 215 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_SetDflt; }
#line 2006 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 69: /* refact ::= CASCADE */
#line 216 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Cascade; }
#line 2011 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 70: /* refact ::= RESTRICT */
#line 217 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Restrict; }
#line 2016 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 71: /* defer_subclause ::= NOT DEFERRABLE init_deferred_pred_opt */
      case 72: /* defer_subclause ::= DEFERRABLE init_deferred_pred_opt */ yytestcase(yyruleno==72);
      case 87: /* defer_subclause_opt ::= defer_subclause */ yytestcase(yyruleno==87);
      case 164: /* insert_cmd ::= INSERT orconf */ yytestcase(yyruleno==164);
#line 219 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = yymsp[0].minor.yy372;}
#line 2024 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 82: /* tcons ::= PRIMARY KEY LP idxlist RP onconf */
#line 236 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddPrimaryKey(pParse,yymsp[-2].minor.yy320,yymsp[0].minor.yy372);}
#line 2029 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 83: /* tcons ::= UNIQUE LP idxlist RP onconf */
#line 238 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteCreateIndex(pParse,0,0,yymsp[-2].minor.yy320,yymsp[0].minor.yy372,0,0);}
#line 2034 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 84: /* tcons ::= CHECK expr onconf */
#line 239 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yy_destructor(yypParser,158,&yymsp[-1].minor);
}
#line 2041 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 85: /* tcons ::= FOREIGN KEY LP idxlist RP REFERENCES nm idxlist_opt refargs defer_subclause_opt */
#line 241 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
    sqliteCreateForeignKey(pParse, yymsp[-6].minor.yy320, &yymsp[-3].minor.yy0, yymsp[-2].minor.yy320, yymsp[-1].minor.yy372);
    sqliteDeferForeignKey(pParse, yymsp[0].minor.yy372);
}
#line 2049 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 88: /* onconf ::= */
      case 90: /* orconf ::= */ yytestcase(yyruleno==90);
#line 255 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Default; }
#line 2055 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 89: /* onconf ::= ON CONFLICT resolvetype */
      case 91: /* orconf ::= OR resolvetype */ yytestcase(yyruleno==91);
#line 256 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = yymsp[0].minor.yy372; }
#line 2061 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 92: /* resolvetype ::= ROLLBACK */
#line 259 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Rollback; }
#line 2066 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 93: /* resolvetype ::= ABORT */
      case 236: /* uniqueflag ::= UNIQUE */ yytestcase(yyruleno==236);
#line 260 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Abort; }
#line 2072 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 94: /* resolvetype ::= FAIL */
#line 261 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Fail; }
#line 2077 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 95: /* resolvetype ::= IGNORE */
#line 262 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Ignore; }
#line 2082 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 96: /* resolvetype ::= REPLACE */
#line 263 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_Replace; }
#line 2087 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 97: /* cmd ::= DROP TABLE nm */
#line 267 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteDropTable(pParse,&yymsp[0].minor.yy0,0);}
#line 2092 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 98: /* cmd ::= CREATE temp VIEW nm AS select */
#line 271 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteCreateView(pParse, &yymsp[-5].minor.yy0, &yymsp[-2].minor.yy0, yymsp[0].minor.yy179, yymsp[-4].minor.yy372);
}
#line 2099 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 99: /* cmd ::= DROP VIEW nm */
#line 274 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDropTable(pParse, &yymsp[0].minor.yy0, 1);
}
#line 2106 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 100: /* cmd ::= select */
#line 280 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteSelect(pParse, yymsp[0].minor.yy179, SRT_Callback, 0, 0, 0, 0);
  sqliteSelectDelete(yymsp[0].minor.yy179);
}
#line 2114 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 101: /* select ::= oneselect */
      case 125: /* seltablist_paren ::= select */ yytestcase(yyruleno==125);
#line 290 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy179 = yymsp[0].minor.yy179;}
#line 2120 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 102: /* select ::= select multiselect_op oneselect */
#line 291 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  if( yymsp[0].minor.yy179 ){
    yymsp[0].minor.yy179->op = yymsp[-1].minor.yy372;
    yymsp[0].minor.yy179->pPrior = yymsp[-2].minor.yy179;
  }
  yygotominor.yy179 = yymsp[0].minor.yy179;
}
#line 2131 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 103: /* multiselect_op ::= UNION */
#line 299 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_UNION;}
#line 2136 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 104: /* multiselect_op ::= UNION ALL */
#line 300 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_ALL;}
#line 2141 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 105: /* multiselect_op ::= INTERSECT */
#line 301 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_INTERSECT;}
#line 2146 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 106: /* multiselect_op ::= EXCEPT */
#line 302 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_EXCEPT;}
#line 2151 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 107: /* oneselect ::= SELECT distinct selcollist from where_opt groupby_opt having_opt orderby_opt limit_opt */
#line 304 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy179 = sqliteSelectNew(yymsp[-6].minor.yy322,yymsp[-5].minor.yy307,yymsp[-4].minor.yy242,yymsp[-3].minor.yy322,yymsp[-2].minor.yy242,yymsp[-1].minor.yy322,yymsp[-7].minor.yy372,yymsp[0].minor.yy124.limit,yymsp[0].minor.yy124.offset);
}
#line 2158 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 111: /* sclp ::= selcollist COMMA */
#line 325 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = yymsp[-1].minor.yy322;}
#line 2163 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 112: /* sclp ::= */
      case 138: /* orderby_opt ::= */ yytestcase(yyruleno==138);
      case 148: /* groupby_opt ::= */ yytestcase(yyruleno==148);
#line 326 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = 0;}
#line 2170 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 113: /* selcollist ::= sclp expr as */
#line 327 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
   yygotominor.yy322 = sqliteExprListAppend(yymsp[-2].minor.yy322,yymsp[-1].minor.yy242,yymsp[0].minor.yy0.n?&yymsp[0].minor.yy0:0);
}
#line 2177 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 114: /* selcollist ::= sclp STAR */
#line 330 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy322 = sqliteExprListAppend(yymsp[-1].minor.yy322, sqliteExpr(TK_ALL, 0, 0, 0), 0);
}
#line 2184 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 115: /* selcollist ::= sclp nm DOT STAR */
#line 333 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  Expr *pRight = sqliteExpr(TK_ALL, 0, 0, 0);
  Expr *pLeft = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy0);
  yygotominor.yy322 = sqliteExprListAppend(yymsp[-3].minor.yy322, sqliteExpr(TK_DOT, pLeft, pRight, 0), 0);
}
#line 2193 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 116: /* as ::= AS nm */
      case 117: /* as ::= ids */ yytestcase(yyruleno==117);
      case 288: /* key_opt ::= USING ids */ yytestcase(yyruleno==288);
#line 343 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 2200 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 118: /* as ::= */
#line 345 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy0.n = 0; }
#line 2205 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 119: /* from ::= */
#line 357 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy307 = sqliteMalloc(sizeof(*yygotominor.yy307));}
#line 2210 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 120: /* from ::= FROM seltablist */
#line 358 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy307 = yymsp[0].minor.yy307;}
#line 2215 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 121: /* stl_prefix ::= seltablist joinop */
#line 363 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
   yygotominor.yy307 = yymsp[-1].minor.yy307;
   if( yygotominor.yy307 && yygotominor.yy307->nSrc>0 ) yygotominor.yy307->a[yygotominor.yy307->nSrc-1].jointype = yymsp[0].minor.yy372;
}
#line 2223 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 122: /* stl_prefix ::= */
#line 367 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy307 = 0;}
#line 2228 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 123: /* seltablist ::= stl_prefix nm dbnm as on_opt using_opt */
#line 368 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
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
#line 2244 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 124: /* seltablist ::= stl_prefix LP seltablist_paren RP as on_opt using_opt */
#line 381 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
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
#line 2261 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 126: /* seltablist_paren ::= seltablist */
#line 402 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
   yygotominor.yy179 = sqliteSelectNew(0,yymsp[0].minor.yy307,0,0,0,0,0,-1,0);
}
#line 2268 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 127: /* dbnm ::= */
#line 407 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy0.z=0; yygotominor.yy0.n=0;}
#line 2273 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 129: /* joinop ::= COMMA */
      case 130: /* joinop ::= JOIN */ yytestcase(yyruleno==130);
#line 412 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = JT_INNER; }
#line 2279 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 131: /* joinop ::= JOIN_KW JOIN */
#line 414 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = sqliteJoinType(pParse,&yymsp[-1].minor.yy0,0,0); }
#line 2284 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 132: /* joinop ::= JOIN_KW nm JOIN */
#line 415 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = sqliteJoinType(pParse,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0,0); }
#line 2289 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 133: /* joinop ::= JOIN_KW nm nm JOIN */
#line 417 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = sqliteJoinType(pParse,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0); }
#line 2294 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 134: /* on_opt ::= ON expr */
      case 142: /* sortitem ::= expr */ yytestcase(yyruleno==142);
      case 151: /* having_opt ::= HAVING expr */ yytestcase(yyruleno==151);
      case 158: /* where_opt ::= WHERE expr */ yytestcase(yyruleno==158);
      case 227: /* case_else ::= ELSE expr */ yytestcase(yyruleno==227);
      case 229: /* case_operand ::= expr */ yytestcase(yyruleno==229);
      case 233: /* expritem ::= expr */ yytestcase(yyruleno==233);
#line 421 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = yymsp[0].minor.yy242;}
#line 2305 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 135: /* on_opt ::= */
      case 150: /* having_opt ::= */ yytestcase(yyruleno==150);
      case 157: /* where_opt ::= */ yytestcase(yyruleno==157);
      case 228: /* case_else ::= */ yytestcase(yyruleno==228);
      case 230: /* case_operand ::= */ yytestcase(yyruleno==230);
      case 234: /* expritem ::= */ yytestcase(yyruleno==234);
#line 422 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = 0;}
#line 2315 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 136: /* using_opt ::= USING LP idxlist RP */
      case 169: /* inscollist_opt ::= LP inscollist RP */ yytestcase(yyruleno==169);
      case 239: /* idxlist_opt ::= LP idxlist RP */ yytestcase(yyruleno==239);
#line 426 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy320 = yymsp[-1].minor.yy320;}
#line 2322 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 137: /* using_opt ::= */
      case 168: /* inscollist_opt ::= */ yytestcase(yyruleno==168);
      case 238: /* idxlist_opt ::= */ yytestcase(yyruleno==238);
#line 427 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy320 = 0;}
#line 2329 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 139: /* orderby_opt ::= ORDER BY sortlist */
      case 149: /* groupby_opt ::= GROUP BY exprlist */ yytestcase(yyruleno==149);
#line 438 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = yymsp[0].minor.yy322;}
#line 2335 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 140: /* sortlist ::= sortlist COMMA sortitem collate sortorder */
#line 439 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy322 = sqliteExprListAppend(yymsp[-4].minor.yy322,yymsp[-2].minor.yy242,0);
  if( yygotominor.yy322 ) yygotominor.yy322->a[yygotominor.yy322->nExpr-1].sortOrder = yymsp[-1].minor.yy372+yymsp[0].minor.yy372;
}
#line 2343 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 141: /* sortlist ::= sortitem collate sortorder */
#line 443 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy322 = sqliteExprListAppend(0,yymsp[-2].minor.yy242,0);
  if( yygotominor.yy322 ) yygotominor.yy322->a[0].sortOrder = yymsp[-1].minor.yy372+yymsp[0].minor.yy372;
}
#line 2351 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 143: /* sortorder ::= ASC */
      case 145: /* sortorder ::= */ yytestcase(yyruleno==145);
#line 452 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = SQLITE_SO_ASC;}
#line 2357 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 144: /* sortorder ::= DESC */
#line 453 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = SQLITE_SO_DESC;}
#line 2362 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 146: /* collate ::= */
#line 455 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = SQLITE_SO_UNK;}
#line 2367 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 147: /* collate ::= COLLATE id */
#line 456 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = sqliteCollateType(yymsp[0].minor.yy0.z, yymsp[0].minor.yy0.n);}
#line 2372 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 152: /* limit_opt ::= */
#line 469 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy124.limit = -1; yygotominor.yy124.offset = 0;}
#line 2377 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 153: /* limit_opt ::= LIMIT signed */
#line 470 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy124.limit = yymsp[0].minor.yy372; yygotominor.yy124.offset = 0;}
#line 2382 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 154: /* limit_opt ::= LIMIT signed OFFSET signed */
#line 472 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy124.limit = yymsp[-2].minor.yy372; yygotominor.yy124.offset = yymsp[0].minor.yy372;}
#line 2387 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 155: /* limit_opt ::= LIMIT signed COMMA signed */
#line 474 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy124.limit = yymsp[0].minor.yy372; yygotominor.yy124.offset = yymsp[-2].minor.yy372;}
#line 2392 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 156: /* cmd ::= DELETE FROM nm dbnm where_opt */
#line 478 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
   sqliteDeleteFrom(pParse, sqliteSrcListAppend(0,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0), yymsp[0].minor.yy242);
}
#line 2399 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 159: /* cmd ::= UPDATE orconf nm dbnm SET setlist where_opt */
#line 494 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteUpdate(pParse,sqliteSrcListAppend(0,&yymsp[-4].minor.yy0,&yymsp[-3].minor.yy0),yymsp[-1].minor.yy322,yymsp[0].minor.yy242,yymsp[-5].minor.yy372);}
#line 2404 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 160: /* setlist ::= setlist COMMA nm EQ expr */
#line 497 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = sqliteExprListAppend(yymsp[-4].minor.yy322,yymsp[0].minor.yy242,&yymsp[-2].minor.yy0);}
#line 2409 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 161: /* setlist ::= nm EQ expr */
#line 498 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = sqliteExprListAppend(0,yymsp[0].minor.yy242,&yymsp[-2].minor.yy0);}
#line 2414 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 162: /* cmd ::= insert_cmd INTO nm dbnm inscollist_opt VALUES LP itemlist RP */
#line 504 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteInsert(pParse, sqliteSrcListAppend(0,&yymsp[-6].minor.yy0,&yymsp[-5].minor.yy0), yymsp[-1].minor.yy322, 0, yymsp[-4].minor.yy320, yymsp[-8].minor.yy372);}
#line 2419 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 163: /* cmd ::= insert_cmd INTO nm dbnm inscollist_opt select */
#line 506 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteInsert(pParse, sqliteSrcListAppend(0,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0), 0, yymsp[0].minor.yy179, yymsp[-1].minor.yy320, yymsp[-5].minor.yy372);}
#line 2424 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 165: /* insert_cmd ::= REPLACE */
#line 510 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = OE_Replace;}
#line 2429 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 166: /* itemlist ::= itemlist COMMA expr */
      case 231: /* exprlist ::= exprlist COMMA expritem */ yytestcase(yyruleno==231);
#line 516 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = sqliteExprListAppend(yymsp[-2].minor.yy322,yymsp[0].minor.yy242,0);}
#line 2435 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 167: /* itemlist ::= expr */
      case 232: /* exprlist ::= expritem */ yytestcase(yyruleno==232);
#line 517 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy322 = sqliteExprListAppend(0,yymsp[0].minor.yy242,0);}
#line 2441 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 170: /* inscollist ::= inscollist COMMA nm */
      case 240: /* idxlist ::= idxlist COMMA idxitem */ yytestcase(yyruleno==240);
#line 526 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy320 = sqliteIdListAppend(yymsp[-2].minor.yy320,&yymsp[0].minor.yy0);}
#line 2447 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 171: /* inscollist ::= nm */
      case 241: /* idxlist ::= idxitem */ yytestcase(yyruleno==241);
#line 527 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy320 = sqliteIdListAppend(0,&yymsp[0].minor.yy0);}
#line 2453 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 172: /* expr ::= LP expr RP */
#line 535 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = yymsp[-1].minor.yy242; sqliteExprSpan(yygotominor.yy242,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0); }
#line 2458 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 173: /* expr ::= NULL */
#line 536 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_NULL, 0, 0, &yymsp[0].minor.yy0);}
#line 2463 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 174: /* expr ::= ID */
      case 175: /* expr ::= JOIN_KW */ yytestcase(yyruleno==175);
#line 537 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy0);}
#line 2469 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 176: /* expr ::= nm DOT nm */
#line 539 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  Expr *temp1 = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy0);
  Expr *temp2 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy0);
  yygotominor.yy242 = sqliteExpr(TK_DOT, temp1, temp2, 0);
}
#line 2478 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 177: /* expr ::= nm DOT nm DOT nm */
#line 544 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  Expr *temp1 = sqliteExpr(TK_ID, 0, 0, &yymsp[-4].minor.yy0);
  Expr *temp2 = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy0);
  Expr *temp3 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy0);
  Expr *temp4 = sqliteExpr(TK_DOT, temp2, temp3, 0);
  yygotominor.yy242 = sqliteExpr(TK_DOT, temp1, temp4, 0);
}
#line 2489 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 178: /* expr ::= INTEGER */
#line 551 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_INTEGER, 0, 0, &yymsp[0].minor.yy0);}
#line 2494 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 179: /* expr ::= FLOAT */
#line 552 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_FLOAT, 0, 0, &yymsp[0].minor.yy0);}
#line 2499 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 180: /* expr ::= STRING */
#line 553 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_STRING, 0, 0, &yymsp[0].minor.yy0);}
#line 2504 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 181: /* expr ::= VARIABLE */
#line 554 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_VARIABLE, 0, 0, &yymsp[0].minor.yy0);
  if( yygotominor.yy242 ) yygotominor.yy242->iTable = ++pParse->nVar;
}
#line 2512 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 182: /* expr ::= ID LP exprlist RP */
#line 558 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExprFunction(yymsp[-1].minor.yy322, &yymsp[-3].minor.yy0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);
}
#line 2520 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 183: /* expr ::= ID LP STAR RP */
#line 562 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExprFunction(0, &yymsp[-3].minor.yy0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);
}
#line 2528 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 184: /* expr ::= expr AND expr */
#line 566 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_AND, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2533 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 185: /* expr ::= expr OR expr */
#line 567 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_OR, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2538 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 186: /* expr ::= expr LT expr */
#line 568 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_LT, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2543 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 187: /* expr ::= expr GT expr */
#line 569 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_GT, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2548 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 188: /* expr ::= expr LE expr */
#line 570 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_LE, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2553 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 189: /* expr ::= expr GE expr */
#line 571 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_GE, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2558 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 190: /* expr ::= expr NE expr */
#line 572 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_NE, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2563 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 191: /* expr ::= expr EQ expr */
#line 573 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_EQ, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2568 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 192: /* expr ::= expr BITAND expr */
#line 574 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_BITAND, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2573 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 193: /* expr ::= expr BITOR expr */
#line 575 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_BITOR, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2578 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 194: /* expr ::= expr LSHIFT expr */
#line 576 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_LSHIFT, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2583 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 195: /* expr ::= expr RSHIFT expr */
#line 577 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_RSHIFT, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2588 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 196: /* expr ::= expr likeop expr */
#line 578 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[0].minor.yy242, 0);
  pList = sqliteExprListAppend(pList, yymsp[-2].minor.yy242, 0);
  yygotominor.yy242 = sqliteExprFunction(pList, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->op = yymsp[-1].minor.yy372;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-2].minor.yy242->span, &yymsp[0].minor.yy242->span);
}
#line 2599 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 197: /* expr ::= expr NOT likeop expr */
#line 585 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[0].minor.yy242, 0);
  pList = sqliteExprListAppend(pList, yymsp[-3].minor.yy242, 0);
  yygotominor.yy242 = sqliteExprFunction(pList, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->op = yymsp[-1].minor.yy372;
  yygotominor.yy242 = sqliteExpr(TK_NOT, yygotominor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-3].minor.yy242->span,&yymsp[0].minor.yy242->span);
}
#line 2611 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 198: /* likeop ::= LIKE */
#line 594 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_LIKE;}
#line 2616 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 199: /* likeop ::= GLOB */
#line 595 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy372 = TK_GLOB;}
#line 2621 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 200: /* expr ::= expr PLUS expr */
#line 596 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_PLUS, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2626 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 201: /* expr ::= expr MINUS expr */
#line 597 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_MINUS, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2631 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 202: /* expr ::= expr STAR expr */
#line 598 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_STAR, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2636 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 203: /* expr ::= expr SLASH expr */
#line 599 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_SLASH, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2641 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 204: /* expr ::= expr REM expr */
#line 600 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_REM, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2646 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 205: /* expr ::= expr CONCAT expr */
#line 601 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy242 = sqliteExpr(TK_CONCAT, yymsp[-2].minor.yy242, yymsp[0].minor.yy242, 0);}
#line 2651 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 206: /* expr ::= expr ISNULL */
#line 602 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_ISNULL, yymsp[-1].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2659 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 207: /* expr ::= expr IS NULL */
#line 606 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_ISNULL, yymsp[-2].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-2].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2667 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 208: /* expr ::= expr NOTNULL */
#line 610 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_NOTNULL, yymsp[-1].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2675 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 209: /* expr ::= expr NOT NULL */
#line 614 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_NOTNULL, yymsp[-2].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-2].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2683 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 210: /* expr ::= expr IS NOT NULL */
#line 618 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_NOTNULL, yymsp[-3].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-3].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2691 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 211: /* expr ::= NOT expr */
#line 622 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_NOT, yymsp[0].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy242->span);
}
#line 2699 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 212: /* expr ::= BITNOT expr */
#line 626 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_BITNOT, yymsp[0].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy242->span);
}
#line 2707 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 213: /* expr ::= MINUS expr */
#line 630 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_UMINUS, yymsp[0].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy242->span);
}
#line 2715 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 214: /* expr ::= PLUS expr */
#line 634 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_UPLUS, yymsp[0].minor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy242->span);
}
#line 2723 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 215: /* expr ::= LP select RP */
#line 638 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_SELECT, 0, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pSelect = yymsp[-1].minor.yy179;
  sqliteExprSpan(yygotominor.yy242,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0);
}
#line 2732 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 216: /* expr ::= expr BETWEEN expr AND expr */
#line 643 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[-2].minor.yy242, 0);
  pList = sqliteExprListAppend(pList, yymsp[0].minor.yy242, 0);
  yygotominor.yy242 = sqliteExpr(TK_BETWEEN, yymsp[-4].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pList = pList;
  sqliteExprSpan(yygotominor.yy242,&yymsp[-4].minor.yy242->span,&yymsp[0].minor.yy242->span);
}
#line 2743 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 217: /* expr ::= expr NOT BETWEEN expr AND expr */
#line 650 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[-2].minor.yy242, 0);
  pList = sqliteExprListAppend(pList, yymsp[0].minor.yy242, 0);
  yygotominor.yy242 = sqliteExpr(TK_BETWEEN, yymsp[-5].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pList = pList;
  yygotominor.yy242 = sqliteExpr(TK_NOT, yygotominor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-5].minor.yy242->span,&yymsp[0].minor.yy242->span);
}
#line 2755 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 218: /* expr ::= expr IN LP exprlist RP */
#line 658 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-4].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pList = yymsp[-1].minor.yy322;
  sqliteExprSpan(yygotominor.yy242,&yymsp[-4].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2764 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 219: /* expr ::= expr IN LP select RP */
#line 663 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-4].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pSelect = yymsp[-1].minor.yy179;
  sqliteExprSpan(yygotominor.yy242,&yymsp[-4].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2773 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 220: /* expr ::= expr NOT IN LP exprlist RP */
#line 668 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-5].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pList = yymsp[-1].minor.yy322;
  yygotominor.yy242 = sqliteExpr(TK_NOT, yygotominor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-5].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2783 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 221: /* expr ::= expr NOT IN LP select RP */
#line 674 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-5].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pSelect = yymsp[-1].minor.yy179;
  yygotominor.yy242 = sqliteExpr(TK_NOT, yygotominor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-5].minor.yy242->span,&yymsp[0].minor.yy0);
}
#line 2793 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 222: /* expr ::= expr IN nm dbnm */
#line 680 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  SrcList *pSrc = sqliteSrcListAppend(0, &yymsp[-1].minor.yy0, &yymsp[0].minor.yy0);
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-3].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pSelect = sqliteSelectNew(0,pSrc,0,0,0,0,0,-1,0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-3].minor.yy242->span,yymsp[0].minor.yy0.z?&yymsp[0].minor.yy0:&yymsp[-1].minor.yy0);
}
#line 2803 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 223: /* expr ::= expr NOT IN nm dbnm */
#line 686 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  SrcList *pSrc = sqliteSrcListAppend(0, &yymsp[-1].minor.yy0, &yymsp[0].minor.yy0);
  yygotominor.yy242 = sqliteExpr(TK_IN, yymsp[-4].minor.yy242, 0, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pSelect = sqliteSelectNew(0,pSrc,0,0,0,0,0,-1,0);
  yygotominor.yy242 = sqliteExpr(TK_NOT, yygotominor.yy242, 0, 0);
  sqliteExprSpan(yygotominor.yy242,&yymsp[-4].minor.yy242->span,yymsp[0].minor.yy0.z?&yymsp[0].minor.yy0:&yymsp[-1].minor.yy0);
}
#line 2814 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 224: /* expr ::= CASE case_operand case_exprlist case_else END */
#line 696 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_CASE, yymsp[-3].minor.yy242, yymsp[-1].minor.yy242, 0);
  if( yygotominor.yy242 ) yygotominor.yy242->pList = yymsp[-2].minor.yy322;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-4].minor.yy0, &yymsp[0].minor.yy0);
}
#line 2823 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 225: /* case_exprlist ::= case_exprlist WHEN expr THEN expr */
#line 703 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy322 = sqliteExprListAppend(yymsp[-4].minor.yy322, yymsp[-2].minor.yy242, 0);
  yygotominor.yy322 = sqliteExprListAppend(yygotominor.yy322, yymsp[0].minor.yy242, 0);
}
#line 2831 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 226: /* case_exprlist ::= WHEN expr THEN expr */
#line 707 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy322 = sqliteExprListAppend(0, yymsp[-2].minor.yy242, 0);
  yygotominor.yy322 = sqliteExprListAppend(yygotominor.yy322, yymsp[0].minor.yy242, 0);
}
#line 2839 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 235: /* cmd ::= CREATE uniqueflag INDEX nm ON nm dbnm LP idxlist RP onconf */
#line 732 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  SrcList *pSrc = sqliteSrcListAppend(0, &yymsp[-5].minor.yy0, &yymsp[-4].minor.yy0);
  if( yymsp[-9].minor.yy372!=OE_None ) yymsp[-9].minor.yy372 = yymsp[0].minor.yy372;
  if( yymsp[-9].minor.yy372==OE_Default) yymsp[-9].minor.yy372 = OE_Abort;
  sqliteCreateIndex(pParse, &yymsp[-7].minor.yy0, pSrc, yymsp[-2].minor.yy320, yymsp[-9].minor.yy372, &yymsp[-10].minor.yy0, &yymsp[-1].minor.yy0);
}
#line 2849 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 237: /* uniqueflag ::= */
#line 741 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = OE_None; }
#line 2854 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 243: /* cmd ::= DROP INDEX nm dbnm */
#line 758 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDropIndex(pParse, sqliteSrcListAppend(0,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0));
}
#line 2861 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 244: /* cmd ::= COPY orconf nm dbnm FROM nm USING DELIMITERS STRING */
#line 766 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteCopy(pParse,sqliteSrcListAppend(0,&yymsp[-6].minor.yy0,&yymsp[-5].minor.yy0),&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0,yymsp[-7].minor.yy372);}
#line 2866 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 245: /* cmd ::= COPY orconf nm dbnm FROM nm */
#line 768 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteCopy(pParse,sqliteSrcListAppend(0,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0),&yymsp[0].minor.yy0,0,yymsp[-4].minor.yy372);}
#line 2871 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 246: /* cmd ::= VACUUM */
#line 772 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteVacuum(pParse,0);}
#line 2876 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 247: /* cmd ::= VACUUM nm */
#line 773 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqliteVacuum(pParse,&yymsp[0].minor.yy0);}
#line 2881 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 248: /* cmd ::= PRAGMA ids EQ nm */
      case 249: /* cmd ::= PRAGMA ids EQ ON */ yytestcase(yyruleno==249);
      case 250: /* cmd ::= PRAGMA ids EQ plus_num */ yytestcase(yyruleno==250);
#line 777 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0,0);}
#line 2888 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 251: /* cmd ::= PRAGMA ids EQ minus_num */
#line 780 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0,1);}
#line 2893 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 252: /* cmd ::= PRAGMA ids LP nm RP */
#line 781 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-3].minor.yy0,&yymsp[-1].minor.yy0,0);}
#line 2898 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 253: /* cmd ::= PRAGMA ids */
#line 782 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[0].minor.yy0,&yymsp[0].minor.yy0,0);}
#line 2903 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 260: /* cmd ::= CREATE trigger_decl BEGIN trigger_cmd_list END */
#line 792 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  Token all;
  all.z = yymsp[-4].minor.yy0.z;
  all.n = (yymsp[0].minor.yy0.z - yymsp[-4].minor.yy0.z) + yymsp[0].minor.yy0.n;
  sqliteFinishTrigger(pParse, yymsp[-1].minor.yy19, &all);
}
#line 2913 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 261: /* trigger_decl ::= temp TRIGGER nm trigger_time trigger_event ON nm dbnm foreach_clause when_clause */
#line 800 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  SrcList *pTab = sqliteSrcListAppend(0, &yymsp[-3].minor.yy0, &yymsp[-2].minor.yy0);
  sqliteBeginTrigger(pParse, &yymsp[-7].minor.yy0, yymsp[-6].minor.yy372, yymsp[-5].minor.yy290.a, yymsp[-5].minor.yy290.b, pTab, yymsp[-1].minor.yy372, yymsp[0].minor.yy182, yymsp[-9].minor.yy372);
}
#line 2921 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 262: /* trigger_time ::= BEFORE */
      case 265: /* trigger_time ::= */ yytestcase(yyruleno==265);
#line 806 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = TK_BEFORE; }
#line 2927 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 263: /* trigger_time ::= AFTER */
#line 807 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = TK_AFTER;  }
#line 2932 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 264: /* trigger_time ::= INSTEAD OF */
#line 808 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = TK_INSTEAD;}
#line 2937 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 266: /* trigger_event ::= DELETE */
#line 813 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy290.a = TK_DELETE; yygotominor.yy290.b = 0; }
#line 2942 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 267: /* trigger_event ::= INSERT */
#line 814 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy290.a = TK_INSERT; yygotominor.yy290.b = 0; }
#line 2947 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 268: /* trigger_event ::= UPDATE */
#line 815 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy290.a = TK_UPDATE; yygotominor.yy290.b = 0;}
#line 2952 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 269: /* trigger_event ::= UPDATE OF inscollist */
#line 816 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy290.a = TK_UPDATE; yygotominor.yy290.b = yymsp[0].minor.yy320; }
#line 2957 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 270: /* foreach_clause ::= */
      case 271: /* foreach_clause ::= FOR EACH ROW */ yytestcase(yyruleno==271);
#line 819 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = TK_ROW; }
#line 2963 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 272: /* foreach_clause ::= FOR EACH STATEMENT */
#line 821 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy372 = TK_STATEMENT; }
#line 2968 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 273: /* when_clause ::= */
#line 824 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy182 = 0; }
#line 2973 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 274: /* when_clause ::= WHEN expr */
#line 825 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy182 = yymsp[0].minor.yy242; }
#line 2978 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 275: /* trigger_cmd_list ::= trigger_cmd SEMI trigger_cmd_list */
#line 829 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yymsp[-2].minor.yy19->pNext = yymsp[0].minor.yy19;
  yygotominor.yy19 = yymsp[-2].minor.yy19;
}
#line 2986 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 276: /* trigger_cmd_list ::= */
#line 833 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy19 = 0; }
#line 2991 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 277: /* trigger_cmd ::= UPDATE orconf nm SET setlist where_opt */
#line 839 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy19 = sqliteTriggerUpdateStep(&yymsp[-3].minor.yy0, yymsp[-1].minor.yy322, yymsp[0].minor.yy242, yymsp[-4].minor.yy372); }
#line 2996 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 278: /* trigger_cmd ::= insert_cmd INTO nm inscollist_opt VALUES LP itemlist RP */
#line 844 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy19 = sqliteTriggerInsertStep(&yymsp[-5].minor.yy0, yymsp[-4].minor.yy320, yymsp[-1].minor.yy322, 0, yymsp[-7].minor.yy372);}
#line 3001 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 279: /* trigger_cmd ::= insert_cmd INTO nm inscollist_opt select */
#line 847 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy19 = sqliteTriggerInsertStep(&yymsp[-2].minor.yy0, yymsp[-1].minor.yy320, 0, yymsp[0].minor.yy179, yymsp[-4].minor.yy372);}
#line 3006 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 280: /* trigger_cmd ::= DELETE FROM nm where_opt */
#line 851 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy19 = sqliteTriggerDeleteStep(&yymsp[-1].minor.yy0, yymsp[0].minor.yy242);}
#line 3011 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 281: /* trigger_cmd ::= select */
#line 854 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy19 = sqliteTriggerSelectStep(yymsp[0].minor.yy179); }
#line 3016 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 282: /* expr ::= RAISE LP IGNORE RP */
#line 857 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_RAISE, 0, 0, 0); 
  yygotominor.yy242->iColumn = OE_Ignore;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-3].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3025 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 283: /* expr ::= RAISE LP ROLLBACK COMMA nm RP */
#line 862 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy0); 
  yygotominor.yy242->iColumn = OE_Rollback;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3034 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 284: /* expr ::= RAISE LP ABORT COMMA nm RP */
#line 867 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy0); 
  yygotominor.yy242->iColumn = OE_Abort;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3043 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 285: /* expr ::= RAISE LP FAIL COMMA nm RP */
#line 872 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy242 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy0); 
  yygotominor.yy242->iColumn = OE_Fail;
  sqliteExprSpan(yygotominor.yy242, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3052 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 286: /* cmd ::= DROP TRIGGER nm dbnm */
#line 879 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDropTrigger(pParse,sqliteSrcListAppend(0,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0));
}
#line 3059 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 287: /* cmd ::= ATTACH database_kw_opt ids AS nm key_opt */
#line 884 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteAttach(pParse, &yymsp[-3].minor.yy0, &yymsp[-1].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3066 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 289: /* key_opt ::= */
#line 889 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy0.z = 0; yygotominor.yy0.n = 0; }
#line 3071 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 292: /* cmd ::= DETACH database_kw_opt nm */
#line 895 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDetach(pParse, &yymsp[0].minor.yy0);
}
#line 3078 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
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
#line 23 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.y"

  if( pParse->zErrMsg==0 ){
    if( TOKEN.z[0] ){
      sqliteErrorMsg(pParse, "near \"%T\": syntax error", &TOKEN);
    }else{
      sqliteErrorMsg(pParse, "incomplete SQL statement");
    }
  }
#line 3178 "/home/felipe/dev/php5/ext/sqlite/libsqlite/src/parse.c"
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
