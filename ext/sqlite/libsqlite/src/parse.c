
/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
#line 35 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"

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


#line 34 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
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
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.
**    sqliteParserARG_SDECL     A static variable declaration for the %extra_argument
**    sqliteParserARG_PDECL     A parameter declaration for the %extra_argument
**    sqliteParserARG_STORE     Code to store %extra_argument into yypParser
**    sqliteParserARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
/*  */
#define YYCODETYPE unsigned char
#define YYNOCODE 219
#define YYACTIONTYPE unsigned short int
#define sqliteParserTOKENTYPE Token
typedef union {
  sqliteParserTOKENTYPE yy0;
  Select* yy11;
  int yy52;
  ExprList* yy62;
  IdList* yy92;
  Token yy210;
  struct TrigEvent yy234;
  Expr * yy270;
  struct {int value; int mask;} yy279;
  struct LimitVal yy280;
  Expr* yy334;
  SrcList* yy335;
  TriggerStep * yy347;
  int yy437;
} YYMINORTYPE;
#define YYSTACKDEPTH 100
#define sqliteParserARG_SDECL Parse *pParse;
#define sqliteParserARG_PDECL ,Parse *pParse
#define sqliteParserARG_FETCH Parse *pParse = yypParser->pParse
#define sqliteParserARG_STORE yypParser->pParse = pParse
#define YYNSTATE 555
#define YYNRULE 287
#define YYERRORSYMBOL 155
#define YYERRSYMDT yy437
#define YYFALLBACK 1
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)
/* Next is the action table.  Each entry in this table contains
**
**  +  An integer which is the number representing the look-ahead
**     token
**
**  +  An integer indicating what action to take.  Number (N) between
**     0 and YYNSTATE-1 mean shift the look-ahead and go to state N.
**     Numbers between YYNSTATE and YYNSTATE+YYNRULE-1 mean reduce by
**     rule N-YYNSTATE.  Number YYNSTATE+YYNRULE means that a syntax
**     error has occurred.  Number YYNSTATE+YYNRULE+1 means the parser
**     accepts its input.
**
**  +  A pointer to the next entry with the same hash value.
**
** The action table is really a series of hash tables.  Each hash
** table contains a number of entries which is a power of two.  The
** "state" table (which follows) contains information about the starting
** point and size of each hash table.
*/
struct yyActionEntry {
  YYCODETYPE   lookahead;   /* The value of the look-ahead token */
  YYCODETYPE   next;        /* Next entry + 1. Zero at end of collision chain */
  YYACTIONTYPE action;      /* Action to take for this look-ahead */
};
typedef struct yyActionEntry yyActionEntry;
static const yyActionEntry yyActionTable[] = {
/* State 0 */
  { 156,   0,   3}, /*  1:                explain shift  3 */
  { 139,   0,   1}, /*  2:                cmdlist shift  1 */
  { 170,   0, 843}, /*  3:                  input accept */
  {  47,   0, 553}, /*  4:                EXPLAIN shift  553 */
  { 154,   0, 554}, /*  5:                   ecmd shift  554 */
  { 107,   4, 552}, /*  6:                   SEMI shift  552 */
/* State 1 */
  {   0,   0, 555}, /*  1:                      $ reduce 0 */
  { 156,   0,   3}, /*  2:                explain shift  3 */
  { 107,   4, 552}, /*  3:                   SEMI shift  552 */
  {  47,   0, 553}, /*  4:                EXPLAIN shift  553 */
  { 154,   0,   2}, /*  5:                   ecmd shift  2 */
/* State 3 */
  {  66,   0, 512}, /*  1:                 INSERT shift  512 */
  {  28,   0, 515}, /*  2:                   COPY shift  515 */
  {  24,   0,  23}, /*  3:                 COMMIT shift  23 */
  {  29,   0, 388}, /*  4:                 CREATE shift  388 */
  {   8,   0, 543}, /*  5:                 ATTACH shift  543 */
  {  34,   0, 490}, /*  6:                 DELETE shift  490 */
  { 138,   2,   6}, /*  7:                    cmd shift  6 */
  {  95,   4, 526}, /*  8:                 PRAGMA shift  526 */
  { 140,   5,   4}, /*  9:                   cmdx shift  4 */
  { 185,   0,  69}, /* 10:              oneselect shift  69 */
  {  10,   0,   7}, /* 11:                  BEGIN shift  7 */
  {  37,   0, 549}, /* 12:                 DETACH shift  549 */
  { 100,   6, 514}, /* 13:                REPLACE shift  514 */
  { 123,   0, 495}, /* 14:                 UPDATE shift  495 */
  { 102,   0,  27}, /* 15:               ROLLBACK shift  27 */
  { 147,  12,  29}, /* 16:           create_table shift  29 */
  { 126,   0, 524}, /* 17:                 VACUUM shift  524 */
  {  40,   0, 478}, /* 18:                   DROP shift  478 */
  { 106,  18,  73}, /* 19:                 SELECT shift  73 */
  { 173,   0, 502}, /* 20:             insert_cmd shift  502 */
  { 196,   0, 489}, /* 21:                 select shift  489 */
  {  43,   0,  25}, /* 22:                    END shift  25 */
/* State 4 */
  { 107,   0,   5}, /*  1:                   SEMI shift  5 */
/* State 6 */
  { 107,   0, 560}, /*  1:                   SEMI reduce 5 */
/* State 7 */
  { 206,   0,   8}, /*  1:              trans_opt shift  8 */
  { 117,   0,  18}, /*  2:            TRANSACTION shift  18 */
/* State 8 */
  {  90,   0,  10}, /*  1:                     ON shift  10 */
  { 184,   0,   9}, /*  2:                 onconf shift  9 */
  { 107,   0, 643}, /*  3:                   SEMI reduce 88 */
/* State 9 */
  { 107,   0, 563}, /*  1:                   SEMI reduce 8 */
/* State 10 */
  {  26,   0,  11}, /*  1:               CONFLICT shift  11 */
/* State 11 */
  { 102,   3,  13}, /*  1:               ROLLBACK shift  13 */
  { 193,   4,  12}, /*  2:            resolvetype shift  12 */
  {  60,   6,  16}, /*  3:                 IGNORE shift  16 */
  {   1,   0,  14}, /*  4:                  ABORT shift  14 */
  { 100,   0,  17}, /*  5:                REPLACE shift  17 */
  {  48,   0,  15}, /*  6:                   FAIL shift  15 */
/* State 18 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1,  19}, /*  2:                     nm shift  19 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 23 */
  { 117,   0,  18}, /*  1:            TRANSACTION shift  18 */
  { 107,   0, 564}, /*  2:                   SEMI reduce 9 */
  { 206,   2,  24}, /*  3:              trans_opt shift  24 */
/* State 24 */
  { 107,   0, 567}, /*  1:                   SEMI reduce 12 */
/* State 25 */
  { 117,   0,  18}, /*  1:            TRANSACTION shift  18 */
  { 107,   0, 564}, /*  2:                   SEMI reduce 9 */
  { 206,   2,  26}, /*  3:              trans_opt shift  26 */
/* State 26 */
  { 107,   0, 568}, /*  1:                   SEMI reduce 13 */
/* State 27 */
  { 117,   0,  18}, /*  1:            TRANSACTION shift  18 */
  { 107,   0, 564}, /*  2:                   SEMI reduce 9 */
  { 206,   2,  28}, /*  3:              trans_opt shift  28 */
/* State 28 */
  { 107,   0, 569}, /*  1:                   SEMI reduce 14 */
/* State 29 */
  {   6,   0, 386}, /*  1:                     AS shift  386 */
  { 148,   3,  30}, /*  2:      create_table_args shift  30 */
  {  79,   0,  31}, /*  3:                     LP shift  31 */
/* State 30 */
  { 107,   0, 570}, /*  1:                   SEMI reduce 15 */
/* State 31 */
  {  59,   0,  20}, /*  1:                     ID shift  20 */
  { 113,   0,  21}, /*  2:                 STRING shift  21 */
  { 142,   0, 385}, /*  3:                 column shift  385 */
  { 143,   1,  37}, /*  4:               columnid shift  37 */
  { 144,   6,  32}, /*  5:             columnlist shift  32 */
  {  74,   0,  22}, /*  6:                JOIN_KW shift  22 */
  { 181,   0, 351}, /*  7:                     nm shift  351 */
/* State 32 */
  { 104,   0, 631}, /*  1:                     RP reduce 76 */
  {  22,   0,  35}, /*  2:                  COMMA shift  35 */
  { 146,   1,  33}, /*  3:           conslist_opt shift  33 */
/* State 33 */
  { 104,   0,  34}, /*  1:                     RP shift  34 */
/* State 34 */
  { 107,   0, 574}, /*  1:                   SEMI reduce 19 */
/* State 35 */
  { 143,   0,  37}, /*  1:               columnid shift  37 */
  {  27,   0, 355}, /*  2:             CONSTRAINT shift  355 */
  { 145,   0, 352}, /*  3:               conslist shift  352 */
  {  96,   5, 357}, /*  4:                PRIMARY shift  357 */
  {  18,   0, 368}, /*  5:                  CHECK shift  368 */
  { 122,   4, 363}, /*  6:                 UNIQUE shift  363 */
  { 113,   9,  21}, /*  7:                 STRING shift  21 */
  {  59,   0,  20}, /*  8:                     ID shift  20 */
  {  74,   0,  22}, /*  9:                JOIN_KW shift  22 */
  { 204,   7, 384}, /* 10:                  tcons shift  384 */
  { 142,  12,  36}, /* 11:                 column shift  36 */
  {  51,   0, 371}, /* 12:                FOREIGN shift  371 */
  { 181,  11, 351}, /* 13:                     nm shift  351 */
/* State 37 */
  { 165,   0, 350}, /*  1:                    ids shift  350 */
  { 113,   0, 252}, /*  2:                 STRING shift  252 */
  { 212,   0,  38}, /*  3:                   type shift  38 */
  { 213,   2, 337}, /*  4:               typename shift  337 */
  {  59,   0, 251}, /*  5:                     ID shift  251 */
/* State 38 */
  { 133,   0,  39}, /*  1:               carglist shift  39 */
/* State 39 */
  {  96,   4,  54}, /*  1:                PRIMARY shift  54 */
  {  27,   0,  41}, /*  2:             CONSTRAINT shift  41 */
  { 132,   0,  40}, /*  3:                   carg shift  40 */
  {  31,   5, 325}, /*  4:                DEFAULT shift  325 */
  {  18,   0,  62}, /*  5:                  CHECK shift  62 */
  { 122,   1,  60}, /*  6:                 UNIQUE shift  60 */
  {  32,   0, 322}, /*  7:             DEFERRABLE shift  322 */
  { 137,  11, 324}, /*  8:                  ccons shift  324 */
  { 151,   0, 319}, /*  9:        defer_subclause shift  319 */
  {  87,   0,  44}, /* 10:                   NULL shift  44 */
  {  98,  12, 297}, /* 11:             REFERENCES shift  297 */
  {  85,  13,  46}, /* 12:                    NOT shift  46 */
  {  20,   0, 320}, /* 13:                COLLATE shift  320 */
/* State 41 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1,  42}, /*  2:                     nm shift  42 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 42 */
  {  20,   0, 320}, /*  1:                COLLATE shift  320 */
  { 151,   0, 319}, /*  2:        defer_subclause shift  319 */
  { 122,   4,  60}, /*  3:                 UNIQUE shift  60 */
  {  32,   0, 322}, /*  4:             DEFERRABLE shift  322 */
  {  87,   0,  44}, /*  5:                   NULL shift  44 */
  {  85,   0,  46}, /*  6:                    NOT shift  46 */
  {  96,   0,  54}, /*  7:                PRIMARY shift  54 */
  { 137,   5,  43}, /*  8:                  ccons shift  43 */
  {  98,  10, 297}, /*  9:             REFERENCES shift  297 */
  {  18,   0,  62}, /* 10:                  CHECK shift  62 */
/* State 44 */
  { 184,   2,  45}, /*  1:                 onconf shift  45 */
  {  90,   0,  10}, /*  2:                     ON shift  10 */
/* State 46 */
  {  32,   0,  49}, /*  1:             DEFERRABLE shift  49 */
  {  87,   0,  47}, /*  2:                   NULL shift  47 */
/* State 47 */
  { 184,   2,  48}, /*  1:                 onconf shift  48 */
  {  90,   0,  10}, /*  2:                     ON shift  10 */
/* State 49 */
  {  65,   0,  51}, /*  1:              INITIALLY shift  51 */
  { 169,   1,  50}, /*  2: init_deferred_pred_opt shift  50 */
/* State 51 */
  {  62,   0,  53}, /*  1:              IMMEDIATE shift  53 */
  {  33,   0,  52}, /*  2:               DEFERRED shift  52 */
/* State 54 */
  {  75,   0,  55}, /*  1:                    KEY shift  55 */
/* State 55 */
  {  36,   0,  59}, /*  1:                   DESC shift  59 */
  { 202,   3,  56}, /*  2:              sortorder shift  56 */
  {   7,   0,  58}, /*  3:                    ASC shift  58 */
/* State 56 */
  { 184,   2,  57}, /*  1:                 onconf shift  57 */
  {  90,   0,  10}, /*  2:                     ON shift  10 */
/* State 60 */
  { 184,   2,  61}, /*  1:                 onconf shift  61 */
  {  90,   0,  10}, /*  2:                     ON shift  10 */
/* State 62 */
  {  79,   0,  63}, /*  1:                     LP shift  63 */
/* State 63 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 294}, /*  8:                   expr shift  294 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 64 */
  {  39,   0, 583}, /*  1:                    DOT reduce 28 */
  {  79,   1,  65}, /*  2:                     LP shift  65 */
/* State 65 */
  {  59,   0,  64}, /*  1:                     ID shift  64 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  74,   0,  67}, /*  3:                JOIN_KW shift  67 */
  { 111,   0, 292}, /*  4:                   STAR shift  292 */
  {  94,   0, 176}, /*  5:                   PLUS shift  176 */
  { 113,   1,  66}, /*  6:                 STRING shift  66 */
  {  79,   0,  68}, /*  7:                     LP shift  68 */
  {  97,   7, 188}, /*  8:                  RAISE shift  188 */
  {  85,  10, 170}, /*  9:                    NOT shift  170 */
  {  49,  11, 169}, /* 10:                  FLOAT shift  169 */
  {  13,   0, 172}, /* 11:                 BITNOT shift  172 */
  {  83,   0, 174}, /* 12:                  MINUS shift  174 */
  {  68,   0, 168}, /* 13:                INTEGER shift  168 */
  { 157,   9, 167}, /* 14:                   expr shift  167 */
  { 158,  13, 214}, /* 15:               expritem shift  214 */
  { 159,  17, 290}, /* 16:               exprlist shift  290 */
  {  87,   0, 101}, /* 17:                   NULL shift  101 */
  {  17,   0, 178}, /* 18:                   CASE shift  178 */
/* State 66 */
  {  39,   0, 584}, /*  1:                    DOT reduce 29 */
/* State 67 */
  {  39,   0, 585}, /*  1:                    DOT reduce 30 */
/* State 68 */
  { 113,   4,  66}, /*  1:                 STRING shift  66 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  74,   0,  67}, /*  3:                JOIN_KW shift  67 */
  {  59,   0,  64}, /*  4:                     ID shift  64 */
  {  94,   0, 176}, /*  5:                   PLUS shift  176 */
  { 185,   1,  69}, /*  6:              oneselect shift  69 */
  {  79,   0,  68}, /*  7:                     LP shift  68 */
  {  97,   7, 188}, /*  8:                  RAISE shift  188 */
  {  85,  10, 170}, /*  9:                    NOT shift  170 */
  {  49,  11, 169}, /* 10:                  FLOAT shift  169 */
  {  13,   0, 172}, /* 11:                 BITNOT shift  172 */
  {  83,   0, 174}, /* 12:                  MINUS shift  174 */
  { 106,   0,  73}, /* 13:                 SELECT shift  73 */
  { 157,   9, 288}, /* 14:                   expr shift  288 */
  {  68,   0, 168}, /* 15:                INTEGER shift  168 */
  {  87,   0, 101}, /* 16:                   NULL shift  101 */
  { 196,  13,  70}, /* 17:                 select shift  70 */
  {  17,   0, 178}, /* 18:                   CASE shift  178 */
/* State 70 */
  { 180,   0,  71}, /*  1:         multiselect_op shift  71 */
  { 121,   3, 162}, /*  2:                  UNION shift  162 */
  {  46,   0, 165}, /*  3:                 EXCEPT shift  165 */
  {  69,   0, 164}, /*  4:              INTERSECT shift  164 */
  { 104,   4, 287}, /*  5:                     RP shift  287 */
/* State 71 */
  { 106,   0,  73}, /*  1:                 SELECT shift  73 */
  { 185,   0,  72}, /*  2:              oneselect shift  72 */
/* State 73 */
  { 153,   0,  74}, /*  1:               distinct shift  74 */
  {   4,   0, 286}, /*  2:                    ALL shift  286 */
  {  38,   0, 285}, /*  3:               DISTINCT shift  285 */
/* State 74 */
  { 194,   0, 278}, /*  1:                   sclp shift  278 */
  { 195,   0,  75}, /*  2:             selcollist shift  75 */
/* State 75 */
  {  22,   0, 236}, /*  1:                  COMMA shift  236 */
  {  52,   1, 237}, /*  2:                   FROM shift  237 */
  { 161,   0,  76}, /*  3:                   from shift  76 */
/* State 76 */
  { 130,   0, 234}, /*  1:                  WHERE shift  234 */
  { 217,   0,  77}, /*  2:              where_opt shift  77 */
/* State 77 */
  { 162,   2,  78}, /*  1:            groupby_opt shift  78 */
  {  56,   0, 231}, /*  2:                  GROUP shift  231 */
/* State 78 */
  {  58,   0, 229}, /*  1:                 HAVING shift  229 */
  { 163,   0,  79}, /*  2:             having_opt shift  79 */
/* State 79 */
  {  93,   0,  88}, /*  1:                  ORDER shift  88 */
  { 187,   1,  80}, /*  2:            orderby_opt shift  80 */
/* State 80 */
  { 178,   2,  81}, /*  1:              limit_opt shift  81 */
  {  78,   0,  82}, /*  2:                  LIMIT shift  82 */
/* State 82 */
  {  68,   0,  83}, /*  1:                INTEGER shift  83 */
/* State 83 */
  {  22,   0,  86}, /*  1:                  COMMA shift  86 */
  {  89,   0,  84}, /*  2:                 OFFSET shift  84 */
/* State 84 */
  {  68,   0,  85}, /*  1:                INTEGER shift  85 */
/* State 86 */
  {  68,   0,  87}, /*  1:                INTEGER shift  87 */
/* State 88 */
  {  15,   0,  89}, /*  1:                     BY shift  89 */
/* State 89 */
  {  85,   2, 170}, /*  1:                    NOT shift  170 */
  {  68,   4, 168}, /*  2:                INTEGER shift  168 */
  {  87,   0, 101}, /*  3:                   NULL shift  101 */
  {  17,   0, 178}, /*  4:                   CASE shift  178 */
  { 157,   0,  98}, /*  5:                   expr shift  98 */
  { 113,   8,  66}, /*  6:                 STRING shift  66 */
  {  74,   0,  67}, /*  7:                JOIN_KW shift  67 */
  {  79,   0,  68}, /*  8:                     LP shift  68 */
  {  59,   0,  64}, /*  9:                     ID shift  64 */
  {  94,   0, 176}, /* 10:                   PLUS shift  176 */
  {  13,   0, 172}, /* 11:                 BITNOT shift  172 */
  { 181,   6, 102}, /* 12:                     nm shift  102 */
  {  97,   0, 188}, /* 13:                  RAISE shift  188 */
  { 200,  11, 226}, /* 14:               sortitem shift  226 */
  { 201,   0,  90}, /* 15:               sortlist shift  90 */
  {  83,  17, 174}, /* 16:                  MINUS shift  174 */
  {  49,   0, 169}, /* 17:                  FLOAT shift  169 */
/* State 90 */
  {  22,   0,  91}, /*  1:                  COMMA shift  91 */
/* State 91 */
  {  97,   3, 188}, /*  1:                  RAISE shift  188 */
  { 113,   1,  66}, /*  2:                 STRING shift  66 */
  {  49,   7, 169}, /*  3:                  FLOAT shift  169 */
  {  83,   0, 174}, /*  4:                  MINUS shift  174 */
  {  68,   0, 168}, /*  5:                INTEGER shift  168 */
  { 181,  10, 102}, /*  6:                     nm shift  102 */
  {  17,   0, 178}, /*  7:                   CASE shift  178 */
  {  87,   0, 101}, /*  8:                   NULL shift  101 */
  { 200,   0,  92}, /*  9:               sortitem shift  92 */
  {  85,   0, 170}, /* 10:                    NOT shift  170 */
  {  74,   0,  67}, /* 11:                JOIN_KW shift  67 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  13,   0, 172}, /* 13:                 BITNOT shift  172 */
  { 157,  13,  98}, /* 14:                   expr shift  98 */
  {  94,   0, 176}, /* 15:                   PLUS shift  176 */
  {  79,   0,  68}, /* 16:                     LP shift  68 */
/* State 92 */
  {  20,   0,  95}, /*  1:                COLLATE shift  95 */
  { 141,   0,  93}, /*  2:                collate shift  93 */
/* State 93 */
  {  36,   0,  59}, /*  1:                   DESC shift  59 */
  { 202,   3,  94}, /*  2:              sortorder shift  94 */
  {   7,   0,  58}, /*  3:                    ASC shift  58 */
/* State 95 */
  { 164,   0,  97}, /*  1:                     id shift  97 */
  {  59,   0,  96}, /*  2:                     ID shift  96 */
/* State 98 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 100 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 107}, /*  8:                   expr shift  107 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 102 */
  {  39,   0, 103}, /*  1:                    DOT shift  103 */
/* State 103 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 104}, /*  2:                     nm shift  104 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 104 */
  {  39,   0, 105}, /*  1:                    DOT shift  105 */
/* State 105 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 106}, /*  2:                     nm shift  106 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 107 */
  {  54,   0, 116}, /*  1:                     GE shift  116 */
  { 105,   0, 128}, /*  2:                 RSHIFT shift  128 */
  {  80,   1, 126}, /*  3:                 LSHIFT shift  126 */
  {  81,   5, 110}, /*  4:                     LT shift  110 */
  {  55,   0, 136}, /*  5:                   GLOB shift  136 */
  { 109,  10, 143}, /*  6:                  SLASH shift  143 */
  {  84,   0, 118}, /*  7:                     NE shift  118 */
  { 111,  11, 141}, /*  8:                   STAR shift  141 */
  {  86,   0, 154}, /*  9:                NOTNULL shift  154 */
  {  83,  14, 139}, /* 10:                  MINUS shift  139 */
  {  85,   0, 132}, /* 11:                    NOT shift  132 */
  {  63,  16, 159}, /* 12:                     IN shift  159 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  57,   0, 112}, /* 14:                     GT shift  112 */
  {  92,  18,  99}, /* 15:      ORACLE_OUTER_JOIN shift  99 */
  {  11,   0, 155}, /* 16:                BETWEEN shift  155 */
  {  94,   0, 137}, /* 17:                   PLUS shift  137 */
  {  14,   0, 124}, /* 18:                  BITOR shift  124 */
  {  45,   0, 120}, /* 19:                     EQ shift  120 */
  {  71,  19, 150}, /* 20:                     IS shift  150 */
  {  72,   0, 149}, /* 21:                 ISNULL shift  149 */
  { 177,  23, 130}, /* 22:                 likeop shift  130 */
  {  99,   0, 145}, /* 23:                    REM shift  145 */
  {  25,   0, 147}, /* 24:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 25:                     LE shift  114 */
  {  77,  24, 135}, /* 26:                   LIKE shift  135 */
/* State 108 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 109}, /*  8:                   expr shift  109 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 109 */
  {  81,   7, 110}, /*  1:                     LT shift  110 */
  { 109,   8, 143}, /*  2:                  SLASH shift  143 */
  {  83,   0, 139}, /*  3:                  MINUS shift  139 */
  { 111,   9, 141}, /*  4:                   STAR shift  141 */
  {  85,   0, 132}, /*  5:                    NOT shift  132 */
  {  86,  11, 154}, /*  6:                NOTNULL shift  154 */
  {  54,   0, 116}, /*  7:                     GE shift  116 */
  {  55,   0, 136}, /*  8:                   GLOB shift  136 */
  {  84,  17, 118}, /*  9:                     NE shift  118 */
  {  63,   0, 159}, /* 10:                     IN shift  159 */
  {   5,   0, 100}, /* 11:                    AND shift  100 */
  {  92,  20,  99}, /* 12:      ORACLE_OUTER_JOIN shift  99 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  94,   0, 137}, /* 14:                   PLUS shift  137 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  { 177,   0, 130}, /* 16:                 likeop shift  130 */
  {  57,   0, 112}, /* 17:                     GT shift  112 */
  {  71,   0, 150}, /* 18:                     IS shift  150 */
  {  99,  21, 145}, /* 19:                    REM shift  145 */
  {  11,   0, 155}, /* 20:                BETWEEN shift  155 */
  {  72,  22, 149}, /* 21:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 22:                     EQ shift  120 */
  {  76,   0, 114}, /* 23:                     LE shift  114 */
  {  77,   0, 135}, /* 24:                   LIKE shift  135 */
  { 105,   0, 128}, /* 25:                 RSHIFT shift  128 */
  {  25,   0, 147}, /* 26:                 CONCAT shift  147 */
  {  80,   0, 126}, /* 27:                 LSHIFT shift  126 */
/* State 110 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 111}, /*  8:                   expr shift  111 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 111 */
  {  12,   0, 122}, /*  1:                 BITAND shift  122 */
  { 109,   5, 143}, /*  2:                  SLASH shift  143 */
  {  14,   0, 124}, /*  3:                  BITOR shift  124 */
  { 111,   6, 141}, /*  4:                   STAR shift  141 */
  {  25,   0, 147}, /*  5:                 CONCAT shift  147 */
  {  99,   0, 145}, /*  6:                    REM shift  145 */
  {  80,   0, 126}, /*  7:                 LSHIFT shift  126 */
  { 105,   0, 128}, /*  8:                 RSHIFT shift  128 */
  {  92,   7,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   8, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  83,   0, 139}, /* 12:                  MINUS shift  139 */
/* State 112 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 113}, /*  8:                   expr shift  113 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 113 */
  {  12,   0, 122}, /*  1:                 BITAND shift  122 */
  { 109,   5, 143}, /*  2:                  SLASH shift  143 */
  {  14,   0, 124}, /*  3:                  BITOR shift  124 */
  { 111,   6, 141}, /*  4:                   STAR shift  141 */
  {  25,   0, 147}, /*  5:                 CONCAT shift  147 */
  {  99,   0, 145}, /*  6:                    REM shift  145 */
  {  80,   0, 126}, /*  7:                 LSHIFT shift  126 */
  { 105,   0, 128}, /*  8:                 RSHIFT shift  128 */
  {  92,   7,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   8, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  83,   0, 139}, /* 12:                  MINUS shift  139 */
/* State 114 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 115}, /*  8:                   expr shift  115 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 115 */
  {  12,   0, 122}, /*  1:                 BITAND shift  122 */
  { 109,   5, 143}, /*  2:                  SLASH shift  143 */
  {  14,   0, 124}, /*  3:                  BITOR shift  124 */
  { 111,   6, 141}, /*  4:                   STAR shift  141 */
  {  25,   0, 147}, /*  5:                 CONCAT shift  147 */
  {  99,   0, 145}, /*  6:                    REM shift  145 */
  {  80,   0, 126}, /*  7:                 LSHIFT shift  126 */
  { 105,   0, 128}, /*  8:                 RSHIFT shift  128 */
  {  92,   7,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   8, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  83,   0, 139}, /* 12:                  MINUS shift  139 */
/* State 116 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 117}, /*  8:                   expr shift  117 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 117 */
  {  12,   0, 122}, /*  1:                 BITAND shift  122 */
  { 109,   5, 143}, /*  2:                  SLASH shift  143 */
  {  14,   0, 124}, /*  3:                  BITOR shift  124 */
  { 111,   6, 141}, /*  4:                   STAR shift  141 */
  {  25,   0, 147}, /*  5:                 CONCAT shift  147 */
  {  99,   0, 145}, /*  6:                    REM shift  145 */
  {  80,   0, 126}, /*  7:                 LSHIFT shift  126 */
  { 105,   0, 128}, /*  8:                 RSHIFT shift  128 */
  {  92,   7,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   8, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  83,   0, 139}, /* 12:                  MINUS shift  139 */
/* State 118 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 119}, /*  8:                   expr shift  119 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 119 */
  {  80,   0, 126}, /*  1:                 LSHIFT shift  126 */
  { 177,   3, 130}, /*  2:                 likeop shift  130 */
  {  81,   0, 110}, /*  3:                     LT shift  110 */
  {  99,   5, 145}, /*  4:                    REM shift  145 */
  {  83,   0, 139}, /*  5:                  MINUS shift  139 */
  {  57,   8, 112}, /*  6:                     GT shift  112 */
  {  54,   0, 116}, /*  7:                     GE shift  116 */
  {  25,   0, 147}, /*  8:                 CONCAT shift  147 */
  {  76,  11, 114}, /*  9:                     LE shift  114 */
  { 105,   6, 128}, /* 10:                 RSHIFT shift  128 */
  {  12,   0, 122}, /* 11:                 BITAND shift  122 */
  {  14,   0, 124}, /* 12:                  BITOR shift  124 */
  {  92,   9,  99}, /* 13:      ORACLE_OUTER_JOIN shift  99 */
  { 109,   0, 143}, /* 14:                  SLASH shift  143 */
  {  94,  12, 137}, /* 15:                   PLUS shift  137 */
  { 111,   0, 141}, /* 16:                   STAR shift  141 */
/* State 120 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 121}, /*  8:                   expr shift  121 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 121 */
  {  80,   0, 126}, /*  1:                 LSHIFT shift  126 */
  { 177,   3, 130}, /*  2:                 likeop shift  130 */
  {  81,   0, 110}, /*  3:                     LT shift  110 */
  {  99,   5, 145}, /*  4:                    REM shift  145 */
  {  83,   0, 139}, /*  5:                  MINUS shift  139 */
  {  57,   8, 112}, /*  6:                     GT shift  112 */
  {  54,   0, 116}, /*  7:                     GE shift  116 */
  {  25,   0, 147}, /*  8:                 CONCAT shift  147 */
  {  76,  11, 114}, /*  9:                     LE shift  114 */
  { 105,   6, 128}, /* 10:                 RSHIFT shift  128 */
  {  12,   0, 122}, /* 11:                 BITAND shift  122 */
  {  14,   0, 124}, /* 12:                  BITOR shift  124 */
  {  92,   9,  99}, /* 13:      ORACLE_OUTER_JOIN shift  99 */
  { 109,   0, 143}, /* 14:                  SLASH shift  143 */
  {  94,  12, 137}, /* 15:                   PLUS shift  137 */
  { 111,   0, 141}, /* 16:                   STAR shift  141 */
/* State 122 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 123}, /*  8:                   expr shift  123 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 123 */
  {  25,   0, 147}, /*  1:                 CONCAT shift  147 */
  { 177,   1, 130}, /*  2:                 likeop shift  130 */
  {  83,   0, 139}, /*  3:                  MINUS shift  139 */
  {  99,   3, 145}, /*  4:                    REM shift  145 */
  {  92,   0,  99}, /*  5:      ORACLE_OUTER_JOIN shift  99 */
  { 109,   0, 143}, /*  6:                  SLASH shift  143 */
  {  94,   0, 137}, /*  7:                   PLUS shift  137 */
  { 111,   0, 141}, /*  8:                   STAR shift  141 */
/* State 124 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 125}, /*  8:                   expr shift  125 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 125 */
  {  25,   0, 147}, /*  1:                 CONCAT shift  147 */
  { 177,   1, 130}, /*  2:                 likeop shift  130 */
  {  83,   0, 139}, /*  3:                  MINUS shift  139 */
  {  99,   3, 145}, /*  4:                    REM shift  145 */
  {  92,   0,  99}, /*  5:      ORACLE_OUTER_JOIN shift  99 */
  { 109,   0, 143}, /*  6:                  SLASH shift  143 */
  {  94,   0, 137}, /*  7:                   PLUS shift  137 */
  { 111,   0, 141}, /*  8:                   STAR shift  141 */
/* State 126 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 127}, /*  8:                   expr shift  127 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 127 */
  {  25,   0, 147}, /*  1:                 CONCAT shift  147 */
  { 177,   1, 130}, /*  2:                 likeop shift  130 */
  {  83,   0, 139}, /*  3:                  MINUS shift  139 */
  {  99,   3, 145}, /*  4:                    REM shift  145 */
  {  92,   0,  99}, /*  5:      ORACLE_OUTER_JOIN shift  99 */
  { 109,   0, 143}, /*  6:                  SLASH shift  143 */
  {  94,   0, 137}, /*  7:                   PLUS shift  137 */
  { 111,   0, 141}, /*  8:                   STAR shift  141 */
/* State 128 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 129}, /*  8:                   expr shift  129 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 129 */
  {  25,   0, 147}, /*  1:                 CONCAT shift  147 */
  { 177,   1, 130}, /*  2:                 likeop shift  130 */
  {  83,   0, 139}, /*  3:                  MINUS shift  139 */
  {  99,   3, 145}, /*  4:                    REM shift  145 */
  {  92,   0,  99}, /*  5:      ORACLE_OUTER_JOIN shift  99 */
  { 109,   0, 143}, /*  6:                  SLASH shift  143 */
  {  94,   0, 137}, /*  7:                   PLUS shift  137 */
  { 111,   0, 141}, /*  8:                   STAR shift  141 */
/* State 130 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 131}, /*  8:                   expr shift  131 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 131 */
  {  80,   0, 126}, /*  1:                 LSHIFT shift  126 */
  { 177,   3, 130}, /*  2:                 likeop shift  130 */
  {  81,   0, 110}, /*  3:                     LT shift  110 */
  {  99,   5, 145}, /*  4:                    REM shift  145 */
  {  83,   0, 139}, /*  5:                  MINUS shift  139 */
  {  57,   8, 112}, /*  6:                     GT shift  112 */
  {  54,   0, 116}, /*  7:                     GE shift  116 */
  {  25,   0, 147}, /*  8:                 CONCAT shift  147 */
  {  76,  11, 114}, /*  9:                     LE shift  114 */
  { 105,   6, 128}, /* 10:                 RSHIFT shift  128 */
  {  12,   0, 122}, /* 11:                 BITAND shift  122 */
  {  14,   0, 124}, /* 12:                  BITOR shift  124 */
  {  92,   9,  99}, /* 13:      ORACLE_OUTER_JOIN shift  99 */
  { 109,   0, 143}, /* 14:                  SLASH shift  143 */
  {  94,  12, 137}, /* 15:                   PLUS shift  137 */
  { 111,   0, 141}, /* 16:                   STAR shift  141 */
/* State 132 */
  {  87,   3, 215}, /*  1:                   NULL shift  215 */
  {  55,   0, 136}, /*  2:                   GLOB shift  136 */
  {  63,   0, 220}, /*  3:                     IN shift  220 */
  { 177,   1, 133}, /*  4:                 likeop shift  133 */
  {  11,   0, 216}, /*  5:                BETWEEN shift  216 */
  {  77,   5, 135}, /*  6:                   LIKE shift  135 */
/* State 133 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 134}, /*  8:                   expr shift  134 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 134 */
  {  80,   0, 126}, /*  1:                 LSHIFT shift  126 */
  { 177,   3, 130}, /*  2:                 likeop shift  130 */
  {  81,   0, 110}, /*  3:                     LT shift  110 */
  {  99,   5, 145}, /*  4:                    REM shift  145 */
  {  83,   0, 139}, /*  5:                  MINUS shift  139 */
  {  57,   8, 112}, /*  6:                     GT shift  112 */
  {  54,   0, 116}, /*  7:                     GE shift  116 */
  {  25,   0, 147}, /*  8:                 CONCAT shift  147 */
  {  76,  11, 114}, /*  9:                     LE shift  114 */
  { 105,   6, 128}, /* 10:                 RSHIFT shift  128 */
  {  12,   0, 122}, /* 11:                 BITAND shift  122 */
  {  14,   0, 124}, /* 12:                  BITOR shift  124 */
  {  92,   9,  99}, /* 13:      ORACLE_OUTER_JOIN shift  99 */
  { 109,   0, 143}, /* 14:                  SLASH shift  143 */
  {  94,  12, 137}, /* 15:                   PLUS shift  137 */
  { 111,   0, 141}, /* 16:                   STAR shift  141 */
/* State 137 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 138}, /*  8:                   expr shift  138 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 138 */
  {  25,   0, 147}, /*  1:                 CONCAT shift  147 */
  { 109,   1, 143}, /*  2:                  SLASH shift  143 */
  {  92,   0,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   5, 130}, /*  4:                 likeop shift  130 */
  { 111,   6, 141}, /*  5:                   STAR shift  141 */
  {  99,   0, 145}, /*  6:                    REM shift  145 */
/* State 139 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 140}, /*  8:                   expr shift  140 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 140 */
  {  25,   0, 147}, /*  1:                 CONCAT shift  147 */
  { 109,   1, 143}, /*  2:                  SLASH shift  143 */
  {  92,   0,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   5, 130}, /*  4:                 likeop shift  130 */
  { 111,   6, 141}, /*  5:                   STAR shift  141 */
  {  99,   0, 145}, /*  6:                    REM shift  145 */
/* State 141 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 142}, /*  8:                   expr shift  142 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 142 */
  { 177,   0, 130}, /*  1:                 likeop shift  130 */
  {  25,   0, 147}, /*  2:                 CONCAT shift  147 */
  {  92,   0,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
/* State 143 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 144}, /*  8:                   expr shift  144 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 144 */
  { 177,   0, 130}, /*  1:                 likeop shift  130 */
  {  25,   0, 147}, /*  2:                 CONCAT shift  147 */
  {  92,   0,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
/* State 145 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 146}, /*  8:                   expr shift  146 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 146 */
  { 177,   0, 130}, /*  1:                 likeop shift  130 */
  {  25,   0, 147}, /*  2:                 CONCAT shift  147 */
  {  92,   0,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
/* State 147 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 148}, /*  8:                   expr shift  148 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 148 */
  {  92,   0,  99}, /*  1:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /*  2:                 likeop shift  130 */
/* State 150 */
  {  85,   0, 152}, /*  1:                    NOT shift  152 */
  {  87,   1, 151}, /*  2:                   NULL shift  151 */
/* State 152 */
  {  87,   0, 153}, /*  1:                   NULL shift  153 */
/* State 155 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 156}, /*  8:                   expr shift  156 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 156 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 157}, /*  6:                    AND shift  157 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 157 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 158}, /*  8:                   expr shift  158 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 158 */
  {  80,   0, 126}, /*  1:                 LSHIFT shift  126 */
  { 177,   3, 130}, /*  2:                 likeop shift  130 */
  {  81,   0, 110}, /*  3:                     LT shift  110 */
  {  99,   5, 145}, /*  4:                    REM shift  145 */
  {  83,   0, 139}, /*  5:                  MINUS shift  139 */
  {  57,   8, 112}, /*  6:                     GT shift  112 */
  {  54,   0, 116}, /*  7:                     GE shift  116 */
  {  25,   0, 147}, /*  8:                 CONCAT shift  147 */
  {  76,  11, 114}, /*  9:                     LE shift  114 */
  { 105,   6, 128}, /* 10:                 RSHIFT shift  128 */
  {  12,   0, 122}, /* 11:                 BITAND shift  122 */
  {  14,   0, 124}, /* 12:                  BITOR shift  124 */
  {  92,   9,  99}, /* 13:      ORACLE_OUTER_JOIN shift  99 */
  { 109,   0, 143}, /* 14:                  SLASH shift  143 */
  {  94,  12, 137}, /* 15:                   PLUS shift  137 */
  { 111,   0, 141}, /* 16:                   STAR shift  141 */
/* State 159 */
  {  79,   0, 160}, /*  1:                     LP shift  160 */
/* State 160 */
  {  85,   0, 170}, /*  1:                    NOT shift  170 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  13,   0, 172}, /*  3:                 BITNOT shift  172 */
  {  83,   0, 174}, /*  4:                  MINUS shift  174 */
  {  74,   0,  67}, /*  5:                JOIN_KW shift  67 */
  { 185,   1,  69}, /*  6:              oneselect shift  69 */
  { 106,   0,  73}, /*  7:                 SELECT shift  73 */
  {  87,   0, 101}, /*  8:                   NULL shift  101 */
  {  68,   0, 168}, /*  9:                INTEGER shift  168 */
  {  49,   0, 169}, /* 10:                  FLOAT shift  169 */
  {  97,  12, 188}, /* 11:                  RAISE shift  188 */
  {  17,   0, 178}, /* 12:                   CASE shift  178 */
  {  79,  16,  68}, /* 13:                     LP shift  68 */
  { 113,   3,  66}, /* 14:                 STRING shift  66 */
  {  94,   5, 176}, /* 15:                   PLUS shift  176 */
  {  59,   0,  64}, /* 16:                     ID shift  64 */
  { 196,   0, 161}, /* 17:                 select shift  161 */
  { 157,  11, 167}, /* 18:                   expr shift  167 */
  { 158,   0, 214}, /* 19:               expritem shift  214 */
  { 159,  13, 210}, /* 20:               exprlist shift  210 */
/* State 161 */
  { 180,   0,  71}, /*  1:         multiselect_op shift  71 */
  { 121,   3, 162}, /*  2:                  UNION shift  162 */
  {  46,   0, 165}, /*  3:                 EXCEPT shift  165 */
  {  69,   0, 164}, /*  4:              INTERSECT shift  164 */
  { 104,   4, 166}, /*  5:                     RP shift  166 */
/* State 162 */
  { 106,   2, 658}, /*  1:                 SELECT reduce 103 */
  {   4,   0, 163}, /*  2:                    ALL shift  163 */
/* State 163 */
  { 106,   0, 659}, /*  1:                 SELECT reduce 104 */
/* State 164 */
  { 106,   0, 660}, /*  1:                 SELECT reduce 105 */
/* State 165 */
  { 106,   0, 661}, /*  1:                 SELECT reduce 106 */
/* State 167 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 170 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 171}, /*  8:                   expr shift  171 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 171 */
  {  54,   0, 116}, /*  1:                     GE shift  116 */
  { 105,   0, 128}, /*  2:                 RSHIFT shift  128 */
  {  80,   1, 126}, /*  3:                 LSHIFT shift  126 */
  {  81,   5, 110}, /*  4:                     LT shift  110 */
  {  55,   0, 136}, /*  5:                   GLOB shift  136 */
  { 109,  10, 143}, /*  6:                  SLASH shift  143 */
  {  84,   0, 118}, /*  7:                     NE shift  118 */
  { 111,  11, 141}, /*  8:                   STAR shift  141 */
  {  86,   0, 154}, /*  9:                NOTNULL shift  154 */
  {  83,  14, 139}, /* 10:                  MINUS shift  139 */
  {  85,   0, 132}, /* 11:                    NOT shift  132 */
  {  63,  16, 159}, /* 12:                     IN shift  159 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  57,   0, 112}, /* 14:                     GT shift  112 */
  {  92,  18,  99}, /* 15:      ORACLE_OUTER_JOIN shift  99 */
  {  11,   0, 155}, /* 16:                BETWEEN shift  155 */
  {  94,   0, 137}, /* 17:                   PLUS shift  137 */
  {  14,   0, 124}, /* 18:                  BITOR shift  124 */
  {  45,   0, 120}, /* 19:                     EQ shift  120 */
  {  71,  19, 150}, /* 20:                     IS shift  150 */
  {  72,   0, 149}, /* 21:                 ISNULL shift  149 */
  { 177,  23, 130}, /* 22:                 likeop shift  130 */
  {  99,   0, 145}, /* 23:                    REM shift  145 */
  {  25,   0, 147}, /* 24:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 25:                     LE shift  114 */
  {  77,  24, 135}, /* 26:                   LIKE shift  135 */
/* State 172 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 173}, /*  8:                   expr shift  173 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 173 */
  {  92,   0,  99}, /*  1:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /*  2:                 likeop shift  130 */
/* State 174 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 175}, /*  8:                   expr shift  175 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 175 */
  {  92,   0,  99}, /*  1:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /*  2:                 likeop shift  130 */
/* State 176 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 177}, /*  8:                   expr shift  177 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 177 */
  {  92,   0,  99}, /*  1:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /*  2:                 likeop shift  130 */
/* State 178 */
  { 136,   2, 180}, /*  1:           case_operand shift  180 */
  {  85,   4, 170}, /*  2:                    NOT shift  170 */
  {  87,   0, 101}, /*  3:                   NULL shift  101 */
  {  68,   6, 168}, /*  4:                INTEGER shift  168 */
  { 157,   0, 179}, /*  5:                   expr shift  179 */
  {  17,   0, 178}, /*  6:                   CASE shift  178 */
  {  74,   0,  67}, /*  7:                JOIN_KW shift  67 */
  { 113,  15,  66}, /*  8:                 STRING shift  66 */
  {  59,   0,  64}, /*  9:                     ID shift  64 */
  {  94,   0, 176}, /* 10:                   PLUS shift  176 */
  { 129,   0, 781}, /* 11:                   WHEN reduce 226 */
  { 181,   8, 102}, /* 12:                     nm shift  102 */
  {  97,   0, 188}, /* 13:                  RAISE shift  188 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  79,   0,  68}, /* 15:                     LP shift  68 */
  {  83,  17, 174}, /* 16:                  MINUS shift  174 */
  {  49,   0, 169}, /* 17:                  FLOAT shift  169 */
/* State 179 */
  {  63,   2, 159}, /*  1:                     IN shift  159 */
  {   5,   0, 100}, /*  2:                    AND shift  100 */
  {  12,   0, 122}, /*  3:                 BITAND shift  122 */
  { 177,   0, 130}, /*  4:                 likeop shift  130 */
  {  91,   0, 108}, /*  5:                     OR shift  108 */
  {  92,   1,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  94,   0, 137}, /*  8:                   PLUS shift  137 */
  {  14,   0, 124}, /*  9:                  BITOR shift  124 */
  {  76,   0, 114}, /* 10:                     LE shift  114 */
  {  80,   0, 126}, /* 11:                 LSHIFT shift  126 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  99,   3, 145}, /* 13:                    REM shift  145 */
  { 129,   7, 780}, /* 14:                   WHEN reduce 225 */
  {  72,   9, 149}, /* 15:                 ISNULL shift  149 */
  {  54,  18, 116}, /* 16:                     GE shift  116 */
  {  45,   0, 120}, /* 17:                     EQ shift  120 */
  {  25,   0, 147}, /* 18:                 CONCAT shift  147 */
  { 105,  10, 128}, /* 19:                 RSHIFT shift  128 */
  {  77,   0, 135}, /* 20:                   LIKE shift  135 */
  {  55,   0, 136}, /* 21:                   GLOB shift  136 */
  {  57,   0, 112}, /* 22:                     GT shift  112 */
  { 109,  11, 143}, /* 23:                  SLASH shift  143 */
  {  81,   0, 110}, /* 24:                     LT shift  110 */
  { 111,   0, 141}, /* 25:                   STAR shift  141 */
  {  83,  16, 139}, /* 26:                  MINUS shift  139 */
  {  84,  21, 118}, /* 27:                     NE shift  118 */
  {  85,   0, 132}, /* 28:                    NOT shift  132 */
  {  86,  22, 154}, /* 29:                NOTNULL shift  154 */
/* State 180 */
  { 129,   0, 206}, /*  1:                   WHEN shift  206 */
  { 135,   1, 181}, /*  2:          case_exprlist shift  181 */
/* State 181 */
  {  42,   0, 204}, /*  1:                   ELSE shift  204 */
  { 129,   0, 184}, /*  2:                   WHEN shift  184 */
  { 134,   1, 182}, /*  3:              case_else shift  182 */
  {  43,   0, 779}, /*  4:                    END reduce 224 */
/* State 182 */
  {  43,   0, 183}, /*  1:                    END shift  183 */
/* State 184 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 185}, /*  8:                   expr shift  185 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 185 */
  { 116,   0, 186}, /*  1:                   THEN shift  186 */
  {  63,   3, 159}, /*  2:                     IN shift  159 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  { 177,   0, 130}, /*  4:                 likeop shift  130 */
  {  91,   0, 108}, /*  5:                     OR shift  108 */
  {  92,   2,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  12,   0, 122}, /*  7:                 BITAND shift  122 */
  {  94,   0, 137}, /*  8:                   PLUS shift  137 */
  {  14,   0, 124}, /*  9:                  BITOR shift  124 */
  {  76,   0, 114}, /* 10:                     LE shift  114 */
  {  80,   0, 126}, /* 11:                 LSHIFT shift  126 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  99,   7, 145}, /* 13:                    REM shift  145 */
  {  71,   0, 150}, /* 14:                     IS shift  150 */
  {  72,   9, 149}, /* 15:                 ISNULL shift  149 */
  {  54,  18, 116}, /* 16:                     GE shift  116 */
  {  45,   0, 120}, /* 17:                     EQ shift  120 */
  {  25,   0, 147}, /* 18:                 CONCAT shift  147 */
  { 105,  10, 128}, /* 19:                 RSHIFT shift  128 */
  {  77,   0, 135}, /* 20:                   LIKE shift  135 */
  {  55,   0, 136}, /* 21:                   GLOB shift  136 */
  {  57,   0, 112}, /* 22:                     GT shift  112 */
  { 109,  11, 143}, /* 23:                  SLASH shift  143 */
  {  81,   0, 110}, /* 24:                     LT shift  110 */
  { 111,   0, 141}, /* 25:                   STAR shift  141 */
  {  83,  16, 139}, /* 26:                  MINUS shift  139 */
  {  84,  21, 118}, /* 27:                     NE shift  118 */
  {  85,   0, 132}, /* 28:                    NOT shift  132 */
  {  86,  22, 154}, /* 29:                NOTNULL shift  154 */
/* State 186 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 187}, /*  8:                   expr shift  187 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 187 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 188 */
  {  79,   0, 189}, /*  1:                     LP shift  189 */
/* State 189 */
  {  60,   4, 190}, /*  1:                 IGNORE shift  190 */
  {   1,   0, 196}, /*  2:                  ABORT shift  196 */
  { 102,   0, 192}, /*  3:               ROLLBACK shift  192 */
  {  48,   0, 200}, /*  4:                   FAIL shift  200 */
/* State 190 */
  { 104,   0, 191}, /*  1:                     RP shift  191 */
/* State 192 */
  {  22,   0, 193}, /*  1:                  COMMA shift  193 */
/* State 193 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 194}, /*  2:                     nm shift  194 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 194 */
  { 104,   0, 195}, /*  1:                     RP shift  195 */
/* State 196 */
  {  22,   0, 197}, /*  1:                  COMMA shift  197 */
/* State 197 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 198}, /*  2:                     nm shift  198 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 198 */
  { 104,   0, 199}, /*  1:                     RP shift  199 */
/* State 200 */
  {  22,   0, 201}, /*  1:                  COMMA shift  201 */
/* State 201 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 202}, /*  2:                     nm shift  202 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 202 */
  { 104,   0, 203}, /*  1:                     RP shift  203 */
/* State 204 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 205}, /*  8:                   expr shift  205 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 205 */
  {  63,   2, 159}, /*  1:                     IN shift  159 */
  {   5,   0, 100}, /*  2:                    AND shift  100 */
  {  12,   0, 122}, /*  3:                 BITAND shift  122 */
  { 177,   0, 130}, /*  4:                 likeop shift  130 */
  {  91,   0, 108}, /*  5:                     OR shift  108 */
  {  92,   1,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  43,   9, 778}, /*  7:                    END reduce 223 */
  {  94,   0, 137}, /*  8:                   PLUS shift  137 */
  {  14,   0, 124}, /*  9:                  BITOR shift  124 */
  {  76,   0, 114}, /* 10:                     LE shift  114 */
  {  80,   0, 126}, /* 11:                 LSHIFT shift  126 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  99,   3, 145}, /* 13:                    REM shift  145 */
  {  71,   0, 150}, /* 14:                     IS shift  150 */
  {  72,   7, 149}, /* 15:                 ISNULL shift  149 */
  {  54,  18, 116}, /* 16:                     GE shift  116 */
  {  45,   0, 120}, /* 17:                     EQ shift  120 */
  {  25,   0, 147}, /* 18:                 CONCAT shift  147 */
  { 105,  10, 128}, /* 19:                 RSHIFT shift  128 */
  {  77,   0, 135}, /* 20:                   LIKE shift  135 */
  {  55,   0, 136}, /* 21:                   GLOB shift  136 */
  {  57,   0, 112}, /* 22:                     GT shift  112 */
  { 109,  11, 143}, /* 23:                  SLASH shift  143 */
  {  81,   0, 110}, /* 24:                     LT shift  110 */
  { 111,   0, 141}, /* 25:                   STAR shift  141 */
  {  83,  16, 139}, /* 26:                  MINUS shift  139 */
  {  84,  21, 118}, /* 27:                     NE shift  118 */
  {  85,   0, 132}, /* 28:                    NOT shift  132 */
  {  86,  22, 154}, /* 29:                NOTNULL shift  154 */
/* State 206 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 207}, /*  8:                   expr shift  207 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 207 */
  { 116,   0, 208}, /*  1:                   THEN shift  208 */
  {  63,   3, 159}, /*  2:                     IN shift  159 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  { 177,   0, 130}, /*  4:                 likeop shift  130 */
  {  91,   0, 108}, /*  5:                     OR shift  108 */
  {  92,   2,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  12,   0, 122}, /*  7:                 BITAND shift  122 */
  {  94,   0, 137}, /*  8:                   PLUS shift  137 */
  {  14,   0, 124}, /*  9:                  BITOR shift  124 */
  {  76,   0, 114}, /* 10:                     LE shift  114 */
  {  80,   0, 126}, /* 11:                 LSHIFT shift  126 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  99,   7, 145}, /* 13:                    REM shift  145 */
  {  71,   0, 150}, /* 14:                     IS shift  150 */
  {  72,   9, 149}, /* 15:                 ISNULL shift  149 */
  {  54,  18, 116}, /* 16:                     GE shift  116 */
  {  45,   0, 120}, /* 17:                     EQ shift  120 */
  {  25,   0, 147}, /* 18:                 CONCAT shift  147 */
  { 105,  10, 128}, /* 19:                 RSHIFT shift  128 */
  {  77,   0, 135}, /* 20:                   LIKE shift  135 */
  {  55,   0, 136}, /* 21:                   GLOB shift  136 */
  {  57,   0, 112}, /* 22:                     GT shift  112 */
  { 109,  11, 143}, /* 23:                  SLASH shift  143 */
  {  81,   0, 110}, /* 24:                     LT shift  110 */
  { 111,   0, 141}, /* 25:                   STAR shift  141 */
  {  83,  16, 139}, /* 26:                  MINUS shift  139 */
  {  84,  21, 118}, /* 27:                     NE shift  118 */
  {  85,   0, 132}, /* 28:                    NOT shift  132 */
  {  86,  22, 154}, /* 29:                NOTNULL shift  154 */
/* State 208 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 209}, /*  8:                   expr shift  209 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 209 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 210 */
  { 104,   2, 211}, /*  1:                     RP shift  211 */
  {  22,   0, 212}, /*  2:                  COMMA shift  212 */
/* State 212 */
  {  97,   3, 188}, /*  1:                  RAISE shift  188 */
  { 113,   1,  66}, /*  2:                 STRING shift  66 */
  {  49,   7, 169}, /*  3:                  FLOAT shift  169 */
  {  83,   0, 174}, /*  4:                  MINUS shift  174 */
  {  68,   0, 168}, /*  5:                INTEGER shift  168 */
  { 181,   9, 102}, /*  6:                     nm shift  102 */
  {  17,   0, 178}, /*  7:                   CASE shift  178 */
  {  87,   0, 101}, /*  8:                   NULL shift  101 */
  {  85,   0, 170}, /*  9:                    NOT shift  170 */
  {  13,   0, 172}, /* 10:                 BITNOT shift  172 */
  {  74,   0,  67}, /* 11:                JOIN_KW shift  67 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  94,   0, 176}, /* 13:                   PLUS shift  176 */
  { 157,  10, 167}, /* 14:                   expr shift  167 */
  { 158,  13, 213}, /* 15:               expritem shift  213 */
  {  79,   0,  68}, /* 16:                     LP shift  68 */
/* State 216 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 217}, /*  8:                   expr shift  217 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 217 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 218}, /*  6:                    AND shift  218 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 218 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 219}, /*  8:                   expr shift  219 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 219 */
  {  54,   0, 116}, /*  1:                     GE shift  116 */
  { 105,   0, 128}, /*  2:                 RSHIFT shift  128 */
  {  80,   1, 126}, /*  3:                 LSHIFT shift  126 */
  {  81,   5, 110}, /*  4:                     LT shift  110 */
  {  55,   0, 136}, /*  5:                   GLOB shift  136 */
  { 109,  10, 143}, /*  6:                  SLASH shift  143 */
  {  84,   0, 118}, /*  7:                     NE shift  118 */
  { 111,  11, 141}, /*  8:                   STAR shift  141 */
  {  86,   0, 154}, /*  9:                NOTNULL shift  154 */
  {  83,  14, 139}, /* 10:                  MINUS shift  139 */
  {  85,   0, 132}, /* 11:                    NOT shift  132 */
  {  63,  16, 159}, /* 12:                     IN shift  159 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  57,   0, 112}, /* 14:                     GT shift  112 */
  {  92,  18,  99}, /* 15:      ORACLE_OUTER_JOIN shift  99 */
  {  11,   0, 155}, /* 16:                BETWEEN shift  155 */
  {  94,   0, 137}, /* 17:                   PLUS shift  137 */
  {  14,   0, 124}, /* 18:                  BITOR shift  124 */
  {  45,   0, 120}, /* 19:                     EQ shift  120 */
  {  71,  19, 150}, /* 20:                     IS shift  150 */
  {  72,   0, 149}, /* 21:                 ISNULL shift  149 */
  { 177,  23, 130}, /* 22:                 likeop shift  130 */
  {  99,   0, 145}, /* 23:                    REM shift  145 */
  {  25,   0, 147}, /* 24:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 25:                     LE shift  114 */
  {  77,  24, 135}, /* 26:                   LIKE shift  135 */
/* State 220 */
  {  79,   0, 221}, /*  1:                     LP shift  221 */
/* State 221 */
  {  85,   0, 170}, /*  1:                    NOT shift  170 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  13,   0, 172}, /*  3:                 BITNOT shift  172 */
  {  83,   0, 174}, /*  4:                  MINUS shift  174 */
  {  74,   0,  67}, /*  5:                JOIN_KW shift  67 */
  { 185,   1,  69}, /*  6:              oneselect shift  69 */
  { 106,   0,  73}, /*  7:                 SELECT shift  73 */
  {  87,   0, 101}, /*  8:                   NULL shift  101 */
  {  68,   0, 168}, /*  9:                INTEGER shift  168 */
  {  49,   0, 169}, /* 10:                  FLOAT shift  169 */
  {  97,  12, 188}, /* 11:                  RAISE shift  188 */
  {  17,   0, 178}, /* 12:                   CASE shift  178 */
  {  79,  16,  68}, /* 13:                     LP shift  68 */
  { 113,   3,  66}, /* 14:                 STRING shift  66 */
  {  94,   5, 176}, /* 15:                   PLUS shift  176 */
  {  59,   0,  64}, /* 16:                     ID shift  64 */
  { 196,   0, 222}, /* 17:                 select shift  222 */
  { 157,  11, 167}, /* 18:                   expr shift  167 */
  { 158,   0, 214}, /* 19:               expritem shift  214 */
  { 159,  13, 224}, /* 20:               exprlist shift  224 */
/* State 222 */
  { 180,   0,  71}, /*  1:         multiselect_op shift  71 */
  { 121,   3, 162}, /*  2:                  UNION shift  162 */
  {  46,   0, 165}, /*  3:                 EXCEPT shift  165 */
  {  69,   0, 164}, /*  4:              INTERSECT shift  164 */
  { 104,   4, 223}, /*  5:                     RP shift  223 */
/* State 224 */
  { 104,   2, 225}, /*  1:                     RP shift  225 */
  {  22,   0, 212}, /*  2:                  COMMA shift  212 */
/* State 226 */
  {  20,   0,  95}, /*  1:                COLLATE shift  95 */
  { 141,   0, 227}, /*  2:                collate shift  227 */
/* State 227 */
  {  36,   0,  59}, /*  1:                   DESC shift  59 */
  { 202,   3, 228}, /*  2:              sortorder shift  228 */
  {   7,   0,  58}, /*  3:                    ASC shift  58 */
/* State 229 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 230}, /*  8:                   expr shift  230 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 230 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 231 */
  {  15,   0, 232}, /*  1:                     BY shift  232 */
/* State 232 */
  {  85,   2, 170}, /*  1:                    NOT shift  170 */
  {  68,   4, 168}, /*  2:                INTEGER shift  168 */
  {  87,   0, 101}, /*  3:                   NULL shift  101 */
  {  17,   0, 178}, /*  4:                   CASE shift  178 */
  { 157,   0, 167}, /*  5:                   expr shift  167 */
  { 158,   0, 214}, /*  6:               expritem shift  214 */
  { 159,   8, 233}, /*  7:               exprlist shift  233 */
  {  74,   0,  67}, /*  8:                JOIN_KW shift  67 */
  {  59,   0,  64}, /*  9:                     ID shift  64 */
  {  94,   0, 176}, /* 10:                   PLUS shift  176 */
  { 113,  15,  66}, /* 11:                 STRING shift  66 */
  { 181,  11, 102}, /* 12:                     nm shift  102 */
  {  97,   0, 188}, /* 13:                  RAISE shift  188 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  79,   0,  68}, /* 15:                     LP shift  68 */
  {  83,  17, 174}, /* 16:                  MINUS shift  174 */
  {  49,   0, 169}, /* 17:                  FLOAT shift  169 */
/* State 233 */
  {  22,   0, 212}, /*  1:                  COMMA shift  212 */
/* State 234 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 235}, /*  8:                   expr shift  235 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 235 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 237 */
  { 197,   0, 238}, /*  1:             seltablist shift  238 */
  { 203,   1, 248}, /*  2:             stl_prefix shift  248 */
/* State 238 */
  {  22,   0, 240}, /*  1:                  COMMA shift  240 */
  {  73,   0, 241}, /*  2:                   JOIN shift  241 */
  {  74,   1, 242}, /*  3:                JOIN_KW shift  242 */
  { 175,   0, 239}, /*  4:                 joinop shift  239 */
/* State 242 */
  {  73,   0, 243}, /*  1:                   JOIN shift  243 */
  { 181,   0, 244}, /*  2:                     nm shift  244 */
  {  59,   0,  20}, /*  3:                     ID shift  20 */
  { 113,   1,  21}, /*  4:                 STRING shift  21 */
  {  74,   3,  22}, /*  5:                JOIN_KW shift  22 */
/* State 244 */
  {  73,   0, 245}, /*  1:                   JOIN shift  245 */
  { 181,   0, 246}, /*  2:                     nm shift  246 */
  {  59,   0,  20}, /*  3:                     ID shift  20 */
  { 113,   1,  21}, /*  4:                 STRING shift  21 */
  {  74,   3,  22}, /*  5:                JOIN_KW shift  22 */
/* State 246 */
  {  73,   0, 247}, /*  1:                   JOIN shift  247 */
/* State 248 */
  {  74,   3,  22}, /*  1:                JOIN_KW shift  22 */
  { 181,   0, 249}, /*  2:                     nm shift  249 */
  {  59,   0,  20}, /*  3:                     ID shift  20 */
  { 113,   0,  21}, /*  4:                 STRING shift  21 */
  {  79,   1, 272}, /*  5:                     LP shift  272 */
/* State 249 */
  { 150,   0, 250}, /*  1:                   dbnm shift  250 */
  {  39,   0, 270}, /*  2:                    DOT shift  270 */
/* State 250 */
  { 165,   0, 255}, /*  1:                    ids shift  255 */
  { 131,   3, 256}, /*  2:                     as shift  256 */
  {   6,   0, 253}, /*  3:                     AS shift  253 */
  { 113,   0, 252}, /*  4:                 STRING shift  252 */
  {  59,   0, 251}, /*  5:                     ID shift  251 */
/* State 253 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 254}, /*  2:                     nm shift  254 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 256 */
  {  90,   0, 268}, /*  1:                     ON shift  268 */
  { 183,   0, 257}, /*  2:                 on_opt shift  257 */
/* State 257 */
  { 125,   0, 259}, /*  1:                  USING shift  259 */
  { 215,   1, 258}, /*  2:              using_opt shift  258 */
/* State 259 */
  {  79,   0, 260}, /*  1:                     LP shift  260 */
/* State 260 */
  { 113,   4,  21}, /*  1:                 STRING shift  21 */
  { 181,   0, 265}, /*  2:                     nm shift  265 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
  { 166,   0, 267}, /*  5:                idxitem shift  267 */
  { 167,   1, 261}, /*  6:                idxlist shift  261 */
/* State 261 */
  { 104,   2, 262}, /*  1:                     RP shift  262 */
  {  22,   0, 263}, /*  2:                  COMMA shift  263 */
/* State 263 */
  { 166,   0, 264}, /*  1:                idxitem shift  264 */
  { 181,   1, 265}, /*  2:                     nm shift  265 */
  {  59,   0,  20}, /*  3:                     ID shift  20 */
  { 113,   0,  21}, /*  4:                 STRING shift  21 */
  {  74,   3,  22}, /*  5:                JOIN_KW shift  22 */
/* State 265 */
  {  36,   0,  59}, /*  1:                   DESC shift  59 */
  { 202,   3, 266}, /*  2:              sortorder shift  266 */
  {   7,   0,  58}, /*  3:                    ASC shift  58 */
/* State 268 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 269}, /*  8:                   expr shift  269 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 269 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 270 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 271}, /*  2:                     nm shift  271 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 272 */
  { 106,   0,  73}, /*  1:                 SELECT shift  73 */
  { 196,   1, 273}, /*  2:                 select shift  273 */
  { 185,   0,  69}, /*  3:              oneselect shift  69 */
/* State 273 */
  { 180,   0,  71}, /*  1:         multiselect_op shift  71 */
  { 121,   3, 162}, /*  2:                  UNION shift  162 */
  {  46,   0, 165}, /*  3:                 EXCEPT shift  165 */
  {  69,   0, 164}, /*  4:              INTERSECT shift  164 */
  { 104,   4, 274}, /*  5:                     RP shift  274 */
/* State 274 */
  { 165,   0, 255}, /*  1:                    ids shift  255 */
  { 131,   3, 275}, /*  2:                     as shift  275 */
  {   6,   0, 253}, /*  3:                     AS shift  253 */
  { 113,   0, 252}, /*  4:                 STRING shift  252 */
  {  59,   0, 251}, /*  5:                     ID shift  251 */
/* State 275 */
  {  90,   0, 268}, /*  1:                     ON shift  268 */
  { 183,   0, 276}, /*  2:                 on_opt shift  276 */
/* State 276 */
  { 125,   0, 259}, /*  1:                  USING shift  259 */
  { 215,   1, 277}, /*  2:              using_opt shift  277 */
/* State 278 */
  {  97,   3, 188}, /*  1:                  RAISE shift  188 */
  { 113,   1,  66}, /*  2:                 STRING shift  66 */
  {  49,   7, 169}, /*  3:                  FLOAT shift  169 */
  {  83,   0, 174}, /*  4:                  MINUS shift  174 */
  {  68,   0, 168}, /*  5:                INTEGER shift  168 */
  { 181,   9, 282}, /*  6:                     nm shift  282 */
  {  17,   0, 178}, /*  7:                   CASE shift  178 */
  {  87,   0, 101}, /*  8:                   NULL shift  101 */
  {  85,   0, 170}, /*  9:                    NOT shift  170 */
  {  13,   0, 172}, /* 10:                 BITNOT shift  172 */
  {  74,   0,  67}, /* 11:                JOIN_KW shift  67 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  79,   0,  68}, /* 13:                     LP shift  68 */
  { 157,  10, 279}, /* 14:                   expr shift  279 */
  {  94,   0, 176}, /* 15:                   PLUS shift  176 */
  { 111,  13, 281}, /* 16:                   STAR shift  281 */
/* State 279 */
  { 165,   2, 255}, /*  1:                    ids shift  255 */
  {  99,   0, 145}, /*  2:                    REM shift  145 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  72,   5, 149}, /*  4:                 ISNULL shift  149 */
  {   6,   0, 253}, /*  5:                     AS shift  253 */
  {  71,   3, 150}, /*  6:                     IS shift  150 */
  { 105,   4, 128}, /*  7:                 RSHIFT shift  128 */
  {  76,   0, 114}, /*  8:                     LE shift  114 */
  {  11,   0, 155}, /*  9:                BETWEEN shift  155 */
  { 111,  14, 141}, /* 10:                   STAR shift  141 */
  { 109,   8, 143}, /* 11:                  SLASH shift  143 */
  {  77,   9, 135}, /* 12:                   LIKE shift  135 */
  { 177,  10, 130}, /* 13:                 likeop shift  130 */
  {  45,  17, 120}, /* 14:                     EQ shift  120 */
  { 113,  24, 252}, /* 15:                 STRING shift  252 */
  {  81,   0, 110}, /* 16:                     LT shift  110 */
  {  12,   0, 122}, /* 17:                 BITAND shift  122 */
  {  83,   0, 139}, /* 18:                  MINUS shift  139 */
  {  84,   0, 118}, /* 19:                     NE shift  118 */
  {  85,   0, 132}, /* 20:                    NOT shift  132 */
  {  86,   0, 154}, /* 21:                NOTNULL shift  154 */
  {  54,   0, 116}, /* 22:                     GE shift  116 */
  {  55,   0, 136}, /* 23:                   GLOB shift  136 */
  {  80,  28, 126}, /* 24:                 LSHIFT shift  126 */
  {  57,   0, 112}, /* 25:                     GT shift  112 */
  {  91,  30, 108}, /* 26:                     OR shift  108 */
  {  92,  32,  99}, /* 27:      ORACLE_OUTER_JOIN shift  99 */
  {  14,   0, 124}, /* 28:                  BITOR shift  124 */
  {  94,   0, 137}, /* 29:                   PLUS shift  137 */
  {  25,   0, 147}, /* 30:                 CONCAT shift  147 */
  {  63,   0, 159}, /* 31:                     IN shift  159 */
  {  59,   0, 251}, /* 32:                     ID shift  251 */
  { 131,   0, 280}, /* 33:                     as shift  280 */
/* State 282 */
  {  39,   0, 283}, /*  1:                    DOT shift  283 */
/* State 283 */
  { 111,   0, 284}, /*  1:                   STAR shift  284 */
  { 181,   1, 104}, /*  2:                     nm shift  104 */
  {  59,   0,  20}, /*  3:                     ID shift  20 */
  { 113,   0,  21}, /*  4:                 STRING shift  21 */
  {  74,   3,  22}, /*  5:                JOIN_KW shift  22 */
/* State 288 */
  {  63,   2, 159}, /*  1:                     IN shift  159 */
  {   5,   0, 100}, /*  2:                    AND shift  100 */
  {  12,   0, 122}, /*  3:                 BITAND shift  122 */
  { 177,   0, 130}, /*  4:                 likeop shift  130 */
  {  91,   0, 108}, /*  5:                     OR shift  108 */
  {  92,   1,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  14,   0, 124}, /*  7:                  BITOR shift  124 */
  {  94,   0, 137}, /*  8:                   PLUS shift  137 */
  {  76,   0, 114}, /*  9:                     LE shift  114 */
  {  80,   0, 126}, /* 10:                 LSHIFT shift  126 */
  {  54,  16, 116}, /* 11:                     GE shift  116 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  99,   3, 145}, /* 13:                    REM shift  145 */
  {  71,   0, 150}, /* 14:                     IS shift  150 */
  {  72,   7, 149}, /* 15:                 ISNULL shift  149 */
  {  25,   0, 147}, /* 16:                 CONCAT shift  147 */
  {  45,   0, 120}, /* 17:                     EQ shift  120 */
  { 104,   0, 289}, /* 18:                     RP shift  289 */
  { 105,   9, 128}, /* 19:                 RSHIFT shift  128 */
  {  77,   0, 135}, /* 20:                   LIKE shift  135 */
  {  55,   0, 136}, /* 21:                   GLOB shift  136 */
  {  57,   0, 112}, /* 22:                     GT shift  112 */
  { 109,  10, 143}, /* 23:                  SLASH shift  143 */
  {  81,   0, 110}, /* 24:                     LT shift  110 */
  { 111,   0, 141}, /* 25:                   STAR shift  141 */
  {  83,  11, 139}, /* 26:                  MINUS shift  139 */
  {  84,  21, 118}, /* 27:                     NE shift  118 */
  {  85,   0, 132}, /* 28:                    NOT shift  132 */
  {  86,  22, 154}, /* 29:                NOTNULL shift  154 */
/* State 290 */
  { 104,   2, 291}, /*  1:                     RP shift  291 */
  {  22,   0, 212}, /*  2:                  COMMA shift  212 */
/* State 292 */
  { 104,   0, 293}, /*  1:                     RP shift  293 */
/* State 294 */
  {  63,   2, 159}, /*  1:                     IN shift  159 */
  {   5,   0, 100}, /*  2:                    AND shift  100 */
  {  12,   0, 122}, /*  3:                 BITAND shift  122 */
  { 177,   0, 130}, /*  4:                 likeop shift  130 */
  {  91,   0, 108}, /*  5:                     OR shift  108 */
  {  92,   1,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  14,   0, 124}, /*  7:                  BITOR shift  124 */
  {  94,   0, 137}, /*  8:                   PLUS shift  137 */
  {  76,   0, 114}, /*  9:                     LE shift  114 */
  {  80,   0, 126}, /* 10:                 LSHIFT shift  126 */
  {  54,  16, 116}, /* 11:                     GE shift  116 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  99,   3, 145}, /* 13:                    REM shift  145 */
  {  71,   0, 150}, /* 14:                     IS shift  150 */
  {  72,   7, 149}, /* 15:                 ISNULL shift  149 */
  {  25,   0, 147}, /* 16:                 CONCAT shift  147 */
  {  45,   0, 120}, /* 17:                     EQ shift  120 */
  { 104,   0, 295}, /* 18:                     RP shift  295 */
  { 105,   9, 128}, /* 19:                 RSHIFT shift  128 */
  {  77,   0, 135}, /* 20:                   LIKE shift  135 */
  {  55,   0, 136}, /* 21:                   GLOB shift  136 */
  {  57,   0, 112}, /* 22:                     GT shift  112 */
  { 109,  10, 143}, /* 23:                  SLASH shift  143 */
  {  81,   0, 110}, /* 24:                     LT shift  110 */
  { 111,   0, 141}, /* 25:                   STAR shift  141 */
  {  83,  11, 139}, /* 26:                  MINUS shift  139 */
  {  84,  21, 118}, /* 27:                     NE shift  118 */
  {  85,   0, 132}, /* 28:                    NOT shift  132 */
  {  86,  22, 154}, /* 29:                NOTNULL shift  154 */
/* State 295 */
  { 184,   2, 296}, /*  1:                 onconf shift  296 */
  {  90,   0,  10}, /*  2:                     ON shift  10 */
/* State 297 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 298}, /*  2:                     nm shift  298 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 298 */
  { 168,   0, 299}, /*  1:            idxlist_opt shift  299 */
  {  79,   0, 316}, /*  2:                     LP shift  316 */
/* State 299 */
  { 192,   0, 300}, /*  1:                refargs shift  300 */
/* State 300 */
  {  90,   0, 304}, /*  1:                     ON shift  304 */
  {  82,   0, 302}, /*  2:                  MATCH shift  302 */
  { 191,   0, 301}, /*  3:                 refarg shift  301 */
/* State 302 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 303}, /*  2:                     nm shift  303 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 304 */
  { 123,   3, 312}, /*  1:                 UPDATE shift  312 */
  {  34,   0, 305}, /*  2:                 DELETE shift  305 */
  {  66,   0, 314}, /*  3:                 INSERT shift  314 */
/* State 305 */
  { 108,   4, 307}, /*  1:                    SET shift  307 */
  { 101,   0, 311}, /*  2:               RESTRICT shift  311 */
  { 190,   0, 306}, /*  3:                 refact shift  306 */
  {  16,   0, 310}, /*  4:                CASCADE shift  310 */
/* State 307 */
  {  31,   0, 309}, /*  1:                DEFAULT shift  309 */
  {  87,   1, 308}, /*  2:                   NULL shift  308 */
/* State 312 */
  { 108,   4, 307}, /*  1:                    SET shift  307 */
  { 101,   0, 311}, /*  2:               RESTRICT shift  311 */
  { 190,   0, 313}, /*  3:                 refact shift  313 */
  {  16,   0, 310}, /*  4:                CASCADE shift  310 */
/* State 314 */
  { 108,   4, 307}, /*  1:                    SET shift  307 */
  { 101,   0, 311}, /*  2:               RESTRICT shift  311 */
  { 190,   0, 315}, /*  3:                 refact shift  315 */
  {  16,   0, 310}, /*  4:                CASCADE shift  310 */
/* State 316 */
  { 113,   4,  21}, /*  1:                 STRING shift  21 */
  { 181,   0, 265}, /*  2:                     nm shift  265 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
  { 166,   0, 267}, /*  5:                idxitem shift  267 */
  { 167,   1, 317}, /*  6:                idxlist shift  317 */
/* State 317 */
  { 104,   2, 318}, /*  1:                     RP shift  318 */
  {  22,   0, 263}, /*  2:                  COMMA shift  263 */
/* State 320 */
  { 164,   0, 321}, /*  1:                     id shift  321 */
  {  59,   0,  96}, /*  2:                     ID shift  96 */
/* State 322 */
  {  65,   0,  51}, /*  1:              INITIALLY shift  51 */
  { 169,   1, 323}, /*  2: init_deferred_pred_opt shift  323 */
/* State 325 */
  {  49,   0, 335}, /*  1:                  FLOAT shift  335 */
  { 113,   0, 326}, /*  2:                 STRING shift  326 */
  {  87,   5, 336}, /*  3:                   NULL shift  336 */
  {  94,   3, 329}, /*  4:                   PLUS shift  329 */
  {  59,   0, 327}, /*  5:                     ID shift  327 */
  {  68,   0, 328}, /*  6:                INTEGER shift  328 */
  {  83,   0, 332}, /*  7:                  MINUS shift  332 */
/* State 329 */
  {  68,   0, 330}, /*  1:                INTEGER shift  330 */
  {  49,   0, 331}, /*  2:                  FLOAT shift  331 */
/* State 332 */
  {  68,   0, 333}, /*  1:                INTEGER shift  333 */
  {  49,   0, 334}, /*  2:                  FLOAT shift  334 */
/* State 337 */
  { 113,   0, 252}, /*  1:                 STRING shift  252 */
  { 165,   1, 349}, /*  2:                    ids shift  349 */
  {  59,   0, 251}, /*  3:                     ID shift  251 */
  {  79,   3, 338}, /*  4:                     LP shift  338 */
/* State 338 */
  {  68,   0, 344}, /*  1:                INTEGER shift  344 */
  {  83,   0, 347}, /*  2:                  MINUS shift  347 */
  {  94,   0, 345}, /*  3:                   PLUS shift  345 */
  { 199,   2, 339}, /*  4:                 signed shift  339 */
/* State 339 */
  { 104,   2, 340}, /*  1:                     RP shift  340 */
  {  22,   0, 341}, /*  2:                  COMMA shift  341 */
/* State 341 */
  {  68,   0, 344}, /*  1:                INTEGER shift  344 */
  {  83,   0, 347}, /*  2:                  MINUS shift  347 */
  {  94,   0, 345}, /*  3:                   PLUS shift  345 */
  { 199,   2, 342}, /*  4:                 signed shift  342 */
/* State 342 */
  { 104,   0, 343}, /*  1:                     RP shift  343 */
/* State 345 */
  {  68,   0, 346}, /*  1:                INTEGER shift  346 */
/* State 347 */
  {  68,   0, 348}, /*  1:                INTEGER shift  348 */
/* State 352 */
  { 104,   2, 632}, /*  1:                     RP reduce 77 */
  {  96,   0, 357}, /*  2:                PRIMARY shift  357 */
  { 122,   6, 363}, /*  3:                 UNIQUE shift  363 */
  {  51,   8, 371}, /*  4:                FOREIGN shift  371 */
  { 204,   0, 383}, /*  5:                  tcons shift  383 */
  {  18,   0, 368}, /*  6:                  CHECK shift  368 */
  {  22,   0, 353}, /*  7:                  COMMA shift  353 */
  {  27,   0, 355}, /*  8:             CONSTRAINT shift  355 */
/* State 353 */
  { 204,   2, 354}, /*  1:                  tcons shift  354 */
  {  96,   5, 357}, /*  2:                PRIMARY shift  357 */
  { 122,   0, 363}, /*  3:                 UNIQUE shift  363 */
  {  51,   6, 371}, /*  4:                FOREIGN shift  371 */
  {  18,   0, 368}, /*  5:                  CHECK shift  368 */
  {  27,   0, 355}, /*  6:             CONSTRAINT shift  355 */
/* State 355 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 356}, /*  2:                     nm shift  356 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 357 */
  {  75,   0, 358}, /*  1:                    KEY shift  358 */
/* State 358 */
  {  79,   0, 359}, /*  1:                     LP shift  359 */
/* State 359 */
  { 113,   4,  21}, /*  1:                 STRING shift  21 */
  { 181,   0, 265}, /*  2:                     nm shift  265 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
  { 166,   0, 267}, /*  5:                idxitem shift  267 */
  { 167,   1, 360}, /*  6:                idxlist shift  360 */
/* State 360 */
  { 104,   2, 361}, /*  1:                     RP shift  361 */
  {  22,   0, 263}, /*  2:                  COMMA shift  263 */
/* State 361 */
  { 184,   2, 362}, /*  1:                 onconf shift  362 */
  {  90,   0,  10}, /*  2:                     ON shift  10 */
/* State 363 */
  {  79,   0, 364}, /*  1:                     LP shift  364 */
/* State 364 */
  { 113,   4,  21}, /*  1:                 STRING shift  21 */
  { 181,   0, 265}, /*  2:                     nm shift  265 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
  { 166,   0, 267}, /*  5:                idxitem shift  267 */
  { 167,   1, 365}, /*  6:                idxlist shift  365 */
/* State 365 */
  { 104,   2, 366}, /*  1:                     RP shift  366 */
  {  22,   0, 263}, /*  2:                  COMMA shift  263 */
/* State 366 */
  { 184,   2, 367}, /*  1:                 onconf shift  367 */
  {  90,   0,  10}, /*  2:                     ON shift  10 */
/* State 368 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 369}, /*  8:                   expr shift  369 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 369 */
  {  90,   0,  10}, /*  1:                     ON shift  10 */
  {  91,   0, 108}, /*  2:                     OR shift  108 */
  {  92,   0,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
  {  63,   0, 159}, /*  4:                     IN shift  159 */
  { 184,   7, 370}, /*  5:                 onconf shift  370 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  94,   0, 137}, /*  7:                   PLUS shift  137 */
  {  11,   0, 155}, /*  8:                BETWEEN shift  155 */
  {  12,   0, 122}, /*  9:                 BITAND shift  122 */
  {  99,   0, 145}, /* 10:                    REM shift  145 */
  {  45,   0, 120}, /* 11:                     EQ shift  120 */
  {  71,   8, 150}, /* 12:                     IS shift  150 */
  {  72,   9, 149}, /* 13:                 ISNULL shift  149 */
  {  81,   0, 110}, /* 14:                     LT shift  110 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  { 105,  11, 128}, /* 16:                 RSHIFT shift  128 */
  {  76,   0, 114}, /* 17:                     LE shift  114 */
  {  77,   0, 135}, /* 18:                   LIKE shift  135 */
  {  54,   0, 116}, /* 19:                     GE shift  116 */
  { 109,   0, 143}, /* 20:                  SLASH shift  143 */
  {  80,   0, 126}, /* 21:                 LSHIFT shift  126 */
  { 111,  14, 141}, /* 22:                   STAR shift  141 */
  {  55,  29, 136}, /* 23:                   GLOB shift  136 */
  {  83,   0, 139}, /* 24:                  MINUS shift  139 */
  {  84,  19, 118}, /* 25:                     NE shift  118 */
  {  85,  23, 132}, /* 26:                    NOT shift  132 */
  {  86,   0, 154}, /* 27:                NOTNULL shift  154 */
  { 177,  30, 130}, /* 28:                 likeop shift  130 */
  {  25,   0, 147}, /* 29:                 CONCAT shift  147 */
  {  57,   0, 112}, /* 30:                     GT shift  112 */
/* State 371 */
  {  75,   0, 372}, /*  1:                    KEY shift  372 */
/* State 372 */
  {  79,   0, 373}, /*  1:                     LP shift  373 */
/* State 373 */
  { 113,   4,  21}, /*  1:                 STRING shift  21 */
  { 181,   0, 265}, /*  2:                     nm shift  265 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
  { 166,   0, 267}, /*  5:                idxitem shift  267 */
  { 167,   1, 374}, /*  6:                idxlist shift  374 */
/* State 374 */
  { 104,   2, 375}, /*  1:                     RP shift  375 */
  {  22,   0, 263}, /*  2:                  COMMA shift  263 */
/* State 375 */
  {  98,   0, 376}, /*  1:             REFERENCES shift  376 */
/* State 376 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 377}, /*  2:                     nm shift  377 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 377 */
  { 168,   0, 378}, /*  1:            idxlist_opt shift  378 */
  {  79,   0, 316}, /*  2:                     LP shift  316 */
/* State 378 */
  { 192,   0, 379}, /*  1:                refargs shift  379 */
/* State 379 */
  {  32,   0, 322}, /*  1:             DEFERRABLE shift  322 */
  {  85,   0, 380}, /*  2:                    NOT shift  380 */
  { 191,   0, 301}, /*  3:                 refarg shift  301 */
  {  82,   0, 302}, /*  4:                  MATCH shift  302 */
  { 151,   1, 382}, /*  5:        defer_subclause shift  382 */
  { 152,   4, 381}, /*  6:    defer_subclause_opt shift  381 */
  {  90,   0, 304}, /*  7:                     ON shift  304 */
/* State 380 */
  {  32,   0,  49}, /*  1:             DEFERRABLE shift  49 */
/* State 386 */
  { 106,   0,  73}, /*  1:                 SELECT shift  73 */
  { 196,   1, 387}, /*  2:                 select shift  387 */
  { 185,   0,  69}, /*  3:              oneselect shift  69 */
/* State 387 */
  { 180,   0,  71}, /*  1:         multiselect_op shift  71 */
  { 121,   4, 162}, /*  2:                  UNION shift  162 */
  { 107,   0, 575}, /*  3:                   SEMI reduce 20 */
  {  46,   0, 165}, /*  4:                 EXCEPT shift  165 */
  {  69,   0, 164}, /*  5:              INTERSECT shift  164 */
/* State 388 */
  { 115,   0, 434}, /*  1:                   TEMP shift  434 */
  { 205,   1, 389}, /*  2:                   temp shift  389 */
  { 209,   0, 435}, /*  3:           trigger_decl shift  435 */
/* State 389 */
  { 114,   0, 390}, /*  1:                  TABLE shift  390 */
  { 122,   0, 406}, /*  2:                 UNIQUE shift  406 */
  { 128,   2, 392}, /*  3:                   VIEW shift  392 */
  { 118,   6, 407}, /*  4:                TRIGGER shift  407 */
  { 214,   4, 396}, /*  5:             uniqueflag shift  396 */
  {  64,   0, 788}, /*  6:                  INDEX reduce 233 */
/* State 390 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 391}, /*  2:                     nm shift  391 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 392 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 393}, /*  2:                     nm shift  393 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 393 */
  {   6,   0, 394}, /*  1:                     AS shift  394 */
/* State 394 */
  { 106,   0,  73}, /*  1:                 SELECT shift  73 */
  { 196,   1, 395}, /*  2:                 select shift  395 */
  { 185,   0,  69}, /*  3:              oneselect shift  69 */
/* State 395 */
  { 180,   0,  71}, /*  1:         multiselect_op shift  71 */
  { 121,   4, 162}, /*  2:                  UNION shift  162 */
  { 107,   0, 653}, /*  3:                   SEMI reduce 98 */
  {  46,   0, 165}, /*  4:                 EXCEPT shift  165 */
  {  69,   0, 164}, /*  5:              INTERSECT shift  164 */
/* State 396 */
  {  64,   0, 397}, /*  1:                  INDEX shift  397 */
/* State 397 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 398}, /*  2:                     nm shift  398 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 398 */
  {  90,   0, 399}, /*  1:                     ON shift  399 */
/* State 399 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 400}, /*  2:                     nm shift  400 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 400 */
  { 150,   3, 401}, /*  1:                   dbnm shift  401 */
  {  79,   0, 680}, /*  2:                     LP reduce 125 */
  {  39,   0, 270}, /*  3:                    DOT shift  270 */
/* State 401 */
  {  79,   0, 402}, /*  1:                     LP shift  402 */
/* State 402 */
  { 113,   4,  21}, /*  1:                 STRING shift  21 */
  { 181,   0, 265}, /*  2:                     nm shift  265 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
  { 166,   0, 267}, /*  5:                idxitem shift  267 */
  { 167,   1, 403}, /*  6:                idxlist shift  403 */
/* State 403 */
  { 104,   2, 404}, /*  1:                     RP shift  404 */
  {  22,   0, 263}, /*  2:                  COMMA shift  263 */
/* State 404 */
  {  90,   0,  10}, /*  1:                     ON shift  10 */
  { 184,   0, 405}, /*  2:                 onconf shift  405 */
  { 107,   0, 643}, /*  3:                   SEMI reduce 88 */
/* State 405 */
  { 107,   0, 786}, /*  1:                   SEMI reduce 231 */
/* State 406 */
  {  64,   0, 787}, /*  1:                  INDEX reduce 232 */
/* State 407 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 408}, /*  2:                     nm shift  408 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 408 */
  {  67,   0, 432}, /*  1:                INSTEAD shift  432 */
  {   9,   0, 430}, /*  2:                 BEFORE shift  430 */
  {   2,   0, 431}, /*  3:                  AFTER shift  431 */
  { 211,   1, 409}, /*  4:           trigger_time shift  409 */
/* State 409 */
  {  66,   2, 423}, /*  1:                 INSERT shift  423 */
  {  34,   0, 422}, /*  2:                 DELETE shift  422 */
  { 210,   1, 410}, /*  3:          trigger_event shift  410 */
  { 123,   0, 424}, /*  4:                 UPDATE shift  424 */
/* State 410 */
  {  90,   0, 411}, /*  1:                     ON shift  411 */
/* State 411 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 412}, /*  2:                     nm shift  412 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 412 */
  { 150,   0, 413}, /*  1:                   dbnm shift  413 */
  {  39,   0, 270}, /*  2:                    DOT shift  270 */
/* State 413 */
  { 160,   2, 414}, /*  1:         foreach_clause shift  414 */
  {  50,   0, 418}, /*  2:                    FOR shift  418 */
/* State 414 */
  { 216,   3, 415}, /*  1:            when_clause shift  415 */
  {  10,   0, 824}, /*  2:                  BEGIN reduce 269 */
  { 129,   0, 416}, /*  3:                   WHEN shift  416 */
/* State 415 */
  {  10,   0, 812}, /*  1:                  BEGIN reduce 257 */
/* State 416 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 417}, /*  8:                   expr shift  417 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 417 */
  {  63,   2, 159}, /*  1:                     IN shift  159 */
  {   5,   0, 100}, /*  2:                    AND shift  100 */
  {  12,   0, 122}, /*  3:                 BITAND shift  122 */
  { 177,   0, 130}, /*  4:                 likeop shift  130 */
  {  91,   0, 108}, /*  5:                     OR shift  108 */
  {  92,   1,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  14,   0, 124}, /*  7:                  BITOR shift  124 */
  {  94,   0, 137}, /*  8:                   PLUS shift  137 */
  {  76,   0, 114}, /*  9:                     LE shift  114 */
  {  80,   0, 126}, /* 10:                 LSHIFT shift  126 */
  {  10,   0, 825}, /* 11:                  BEGIN reduce 270 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  99,   3, 145}, /* 13:                    REM shift  145 */
  {  71,   0, 150}, /* 14:                     IS shift  150 */
  {  72,   7, 149}, /* 15:                 ISNULL shift  149 */
  {  54,  18, 116}, /* 16:                     GE shift  116 */
  {  45,   0, 120}, /* 17:                     EQ shift  120 */
  {  25,   0, 147}, /* 18:                 CONCAT shift  147 */
  { 105,   9, 128}, /* 19:                 RSHIFT shift  128 */
  {  77,   0, 135}, /* 20:                   LIKE shift  135 */
  {  55,   0, 136}, /* 21:                   GLOB shift  136 */
  {  57,   0, 112}, /* 22:                     GT shift  112 */
  { 109,  10, 143}, /* 23:                  SLASH shift  143 */
  {  81,   0, 110}, /* 24:                     LT shift  110 */
  { 111,   0, 141}, /* 25:                   STAR shift  141 */
  {  83,  16, 139}, /* 26:                  MINUS shift  139 */
  {  84,  21, 118}, /* 27:                     NE shift  118 */
  {  85,   0, 132}, /* 28:                    NOT shift  132 */
  {  86,  22, 154}, /* 29:                NOTNULL shift  154 */
/* State 418 */
  {  41,   0, 419}, /*  1:                   EACH shift  419 */
/* State 419 */
  { 112,   0, 421}, /*  1:              STATEMENT shift  421 */
  { 103,   0, 420}, /*  2:                    ROW shift  420 */
/* State 422 */
  {  90,   0, 817}, /*  1:                     ON reduce 262 */
/* State 423 */
  {  90,   0, 818}, /*  1:                     ON reduce 263 */
/* State 424 */
  {  90,   2, 819}, /*  1:                     ON reduce 264 */
  {  88,   0, 425}, /*  2:                     OF shift  425 */
/* State 425 */
  { 171,   0, 426}, /*  1:             inscollist shift  426 */
  { 181,   1, 429}, /*  2:                     nm shift  429 */
  {  59,   0,  20}, /*  3:                     ID shift  20 */
  { 113,   0,  21}, /*  4:                 STRING shift  21 */
  {  74,   3,  22}, /*  5:                JOIN_KW shift  22 */
/* State 426 */
  {  90,   2, 820}, /*  1:                     ON reduce 265 */
  {  22,   0, 427}, /*  2:                  COMMA shift  427 */
/* State 427 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 428}, /*  2:                     nm shift  428 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 432 */
  {  88,   0, 433}, /*  1:                     OF shift  433 */
/* State 435 */
  {  10,   0, 436}, /*  1:                  BEGIN shift  436 */
/* State 436 */
  { 207,   0, 440}, /*  1:            trigger_cmd shift  440 */
  { 208,   0, 438}, /*  2:       trigger_cmd_list shift  438 */
  { 106,   5,  73}, /*  3:                 SELECT shift  73 */
  {  66,   0, 458}, /*  4:                 INSERT shift  458 */
  {  43,   9, 827}, /*  5:                    END reduce 272 */
  { 185,   0,  69}, /*  6:              oneselect shift  69 */
  { 123,   0, 443}, /*  7:                 UPDATE shift  443 */
  { 196,   3, 437}, /*  8:                 select shift  437 */
  {  34,   0, 474}, /*  9:                 DELETE shift  474 */
/* State 437 */
  { 180,   0,  71}, /*  1:         multiselect_op shift  71 */
  { 121,   4, 162}, /*  2:                  UNION shift  162 */
  { 107,   0, 832}, /*  3:                   SEMI reduce 277 */
  {  46,   0, 165}, /*  4:                 EXCEPT shift  165 */
  {  69,   0, 164}, /*  5:              INTERSECT shift  164 */
/* State 438 */
  {  43,   0, 439}, /*  1:                    END shift  439 */
/* State 439 */
  { 107,   0, 811}, /*  1:                   SEMI reduce 256 */
/* State 440 */
  { 107,   0, 441}, /*  1:                   SEMI shift  441 */
/* State 441 */
  { 207,   0, 440}, /*  1:            trigger_cmd shift  440 */
  { 208,   0, 442}, /*  2:       trigger_cmd_list shift  442 */
  { 106,   5,  73}, /*  3:                 SELECT shift  73 */
  {  66,   0, 458}, /*  4:                 INSERT shift  458 */
  {  43,   9, 827}, /*  5:                    END reduce 272 */
  { 185,   0,  69}, /*  6:              oneselect shift  69 */
  { 123,   0, 443}, /*  7:                 UPDATE shift  443 */
  { 196,   3, 437}, /*  8:                 select shift  437 */
  {  34,   0, 474}, /*  9:                 DELETE shift  474 */
/* State 442 */
  {  43,   0, 826}, /*  1:                    END reduce 271 */
/* State 443 */
  { 186,   0, 446}, /*  1:                 orconf shift  446 */
  {  91,   0, 444}, /*  2:                     OR shift  444 */
/* State 444 */
  { 102,   3,  13}, /*  1:               ROLLBACK shift  13 */
  { 193,   4, 445}, /*  2:            resolvetype shift  445 */
  {  60,   6,  16}, /*  3:                 IGNORE shift  16 */
  {   1,   0,  14}, /*  4:                  ABORT shift  14 */
  { 100,   0,  17}, /*  5:                REPLACE shift  17 */
  {  48,   0,  15}, /*  6:                   FAIL shift  15 */
/* State 446 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 447}, /*  2:                     nm shift  447 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 447 */
  { 108,   0, 448}, /*  1:                    SET shift  448 */
/* State 448 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   0, 455}, /*  2:                     nm shift  455 */
  {  59,   0,  20}, /*  3:                     ID shift  20 */
  { 198,   1, 449}, /*  4:                setlist shift  449 */
  {  74,   3,  22}, /*  5:                JOIN_KW shift  22 */
/* State 449 */
  {  22,   0, 450}, /*  1:                  COMMA shift  450 */
  { 217,   0, 454}, /*  2:              where_opt shift  454 */
  { 130,   1, 234}, /*  3:                  WHERE shift  234 */
  { 107,   0, 710}, /*  4:                   SEMI reduce 155 */
/* State 450 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 451}, /*  2:                     nm shift  451 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 451 */
  {  45,   0, 452}, /*  1:                     EQ shift  452 */
/* State 452 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 453}, /*  8:                   expr shift  453 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 453 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 454 */
  { 107,   0, 828}, /*  1:                   SEMI reduce 273 */
/* State 455 */
  {  45,   0, 456}, /*  1:                     EQ shift  456 */
/* State 456 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 457}, /*  8:                   expr shift  457 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 457 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 458 */
  { 186,   0, 459}, /*  1:                 orconf shift  459 */
  {  91,   3, 444}, /*  2:                     OR shift  444 */
  {  70,   0, 645}, /*  3:                   INTO reduce 90 */
/* State 459 */
  {  70,   0, 460}, /*  1:                   INTO shift  460 */
/* State 460 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 461}, /*  2:                     nm shift  461 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 461 */
  { 172,   0, 465}, /*  1:         inscollist_opt shift  465 */
  {  79,   0, 462}, /*  2:                     LP shift  462 */
/* State 462 */
  { 171,   0, 463}, /*  1:             inscollist shift  463 */
  { 181,   1, 429}, /*  2:                     nm shift  429 */
  {  59,   0,  20}, /*  3:                     ID shift  20 */
  { 113,   0,  21}, /*  4:                 STRING shift  21 */
  {  74,   3,  22}, /*  5:                JOIN_KW shift  22 */
/* State 463 */
  { 104,   2, 464}, /*  1:                     RP shift  464 */
  {  22,   0, 427}, /*  2:                  COMMA shift  427 */
/* State 465 */
  { 196,   0, 466}, /*  1:                 select shift  466 */
  { 185,   0,  69}, /*  2:              oneselect shift  69 */
  { 106,   0,  73}, /*  3:                 SELECT shift  73 */
  { 127,   0, 467}, /*  4:                 VALUES shift  467 */
/* State 466 */
  { 180,   0,  71}, /*  1:         multiselect_op shift  71 */
  { 121,   4, 162}, /*  2:                  UNION shift  162 */
  { 107,   0, 830}, /*  3:                   SEMI reduce 275 */
  {  46,   0, 165}, /*  4:                 EXCEPT shift  165 */
  {  69,   0, 164}, /*  5:              INTERSECT shift  164 */
/* State 467 */
  {  79,   0, 468}, /*  1:                     LP shift  468 */
/* State 468 */
  {  97,   3, 188}, /*  1:                  RAISE shift  188 */
  { 113,   1,  66}, /*  2:                 STRING shift  66 */
  {  49,   7, 169}, /*  3:                  FLOAT shift  169 */
  {  83,   0, 174}, /*  4:                  MINUS shift  174 */
  {  68,   0, 168}, /*  5:                INTEGER shift  168 */
  { 181,   9, 102}, /*  6:                     nm shift  102 */
  {  17,   0, 178}, /*  7:                   CASE shift  178 */
  {  87,   0, 101}, /*  8:                   NULL shift  101 */
  {  85,   0, 170}, /*  9:                    NOT shift  170 */
  {  13,   0, 172}, /* 10:                 BITNOT shift  172 */
  {  74,   0,  67}, /* 11:                JOIN_KW shift  67 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  94,   0, 176}, /* 13:                   PLUS shift  176 */
  { 157,  10, 473}, /* 14:                   expr shift  473 */
  { 174,  13, 469}, /* 15:               itemlist shift  469 */
  {  79,   0,  68}, /* 16:                     LP shift  68 */
/* State 469 */
  { 104,   2, 472}, /*  1:                     RP shift  472 */
  {  22,   0, 470}, /*  2:                  COMMA shift  470 */
/* State 470 */
  {  79,   4,  68}, /*  1:                     LP shift  68 */
  { 181,   0, 102}, /*  2:                     nm shift  102 */
  {  17,   0, 178}, /*  3:                   CASE shift  178 */
  {  49,   0, 169}, /*  4:                  FLOAT shift  169 */
  {  94,   1, 176}, /*  5:                   PLUS shift  176 */
  {  97,   0, 188}, /*  6:                  RAISE shift  188 */
  {  83,  10, 174}, /*  7:                  MINUS shift  174 */
  { 157,   6, 471}, /*  8:                   expr shift  471 */
  { 113,   7,  66}, /*  9:                 STRING shift  66 */
  {  68,   0, 168}, /* 10:                INTEGER shift  168 */
  {  85,   0, 170}, /* 11:                    NOT shift  170 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  87,   0, 101}, /* 13:                   NULL shift  101 */
  {  13,   0, 172}, /* 14:                 BITNOT shift  172 */
  {  74,  12,  67}, /* 15:                JOIN_KW shift  67 */
/* State 471 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 472 */
  { 107,   0, 829}, /*  1:                   SEMI reduce 274 */
/* State 473 */
  {  84,   0, 118}, /*  1:                     NE shift  118 */
  {  85,   4, 132}, /*  2:                    NOT shift  132 */
  {  86,   0, 154}, /*  3:                NOTNULL shift  154 */
  {  57,   0, 112}, /*  4:                     GT shift  112 */
  {  63,   0, 159}, /*  5:                     IN shift  159 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  71,   0, 150}, /*  7:                     IS shift  150 */
  {  91,   5, 108}, /*  8:                     OR shift  108 */
  {  92,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  { 177,   0, 130}, /* 10:                 likeop shift  130 */
  {  94,   0, 137}, /* 11:                   PLUS shift  137 */
  {  11,   0, 155}, /* 12:                BETWEEN shift  155 */
  {  12,   0, 122}, /* 13:                 BITAND shift  122 */
  {  77,   0, 135}, /* 14:                   LIKE shift  135 */
  {  14,   0, 124}, /* 15:                  BITOR shift  124 */
  {  99,   7, 145}, /* 16:                    REM shift  145 */
  {  72,   0, 149}, /* 17:                 ISNULL shift  149 */
  {  45,   0, 120}, /* 18:                     EQ shift  120 */
  {  81,  20, 110}, /* 19:                     LT shift  110 */
  {  25,   0, 147}, /* 20:                 CONCAT shift  147 */
  {  76,   0, 114}, /* 21:                     LE shift  114 */
  { 105,  14, 128}, /* 22:                 RSHIFT shift  128 */
  {  83,  24, 139}, /* 23:                  MINUS shift  139 */
  {  55,   0, 136}, /* 24:                   GLOB shift  136 */
  {  80,   0, 126}, /* 25:                 LSHIFT shift  126 */
  { 109,  19, 143}, /* 26:                  SLASH shift  143 */
  {  54,   0, 116}, /* 27:                     GE shift  116 */
  { 111,  23, 141}, /* 28:                   STAR shift  141 */
/* State 474 */
  {  52,   0, 475}, /*  1:                   FROM shift  475 */
/* State 475 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 476}, /*  2:                     nm shift  476 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 476 */
  { 130,   0, 234}, /*  1:                  WHERE shift  234 */
  { 217,   1, 477}, /*  2:              where_opt shift  477 */
  { 107,   0, 710}, /*  3:                   SEMI reduce 155 */
/* State 477 */
  { 107,   0, 831}, /*  1:                   SEMI reduce 276 */
/* State 478 */
  { 128,   2, 481}, /*  1:                   VIEW shift  481 */
  {  64,   0, 483}, /*  2:                  INDEX shift  483 */
  { 118,   4, 486}, /*  3:                TRIGGER shift  486 */
  { 114,   0, 479}, /*  4:                  TABLE shift  479 */
/* State 479 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 480}, /*  2:                     nm shift  480 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 480 */
  { 107,   0, 652}, /*  1:                   SEMI reduce 97 */
/* State 481 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 482}, /*  2:                     nm shift  482 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 482 */
  { 107,   0, 654}, /*  1:                   SEMI reduce 99 */
/* State 483 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 484}, /*  2:                     nm shift  484 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 484 */
  { 150,   2, 485}, /*  1:                   dbnm shift  485 */
  {  39,   0, 270}, /*  2:                    DOT shift  270 */
  { 107,   0, 680}, /*  3:                   SEMI reduce 125 */
/* State 485 */
  { 107,   0, 794}, /*  1:                   SEMI reduce 239 */
/* State 486 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 487}, /*  2:                     nm shift  487 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 487 */
  { 150,   2, 488}, /*  1:                   dbnm shift  488 */
  {  39,   0, 270}, /*  2:                    DOT shift  270 */
  { 107,   0, 680}, /*  3:                   SEMI reduce 125 */
/* State 488 */
  { 107,   0, 837}, /*  1:                   SEMI reduce 282 */
/* State 489 */
  { 180,   0,  71}, /*  1:         multiselect_op shift  71 */
  { 121,   4, 162}, /*  2:                  UNION shift  162 */
  { 107,   0, 655}, /*  3:                   SEMI reduce 100 */
  {  46,   0, 165}, /*  4:                 EXCEPT shift  165 */
  {  69,   0, 164}, /*  5:              INTERSECT shift  164 */
/* State 490 */
  {  52,   0, 491}, /*  1:                   FROM shift  491 */
/* State 491 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 492}, /*  2:                     nm shift  492 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 492 */
  { 150,   0, 493}, /*  1:                   dbnm shift  493 */
  {  39,   0, 270}, /*  2:                    DOT shift  270 */
/* State 493 */
  { 130,   0, 234}, /*  1:                  WHERE shift  234 */
  { 217,   1, 494}, /*  2:              where_opt shift  494 */
  { 107,   0, 710}, /*  3:                   SEMI reduce 155 */
/* State 494 */
  { 107,   0, 709}, /*  1:                   SEMI reduce 154 */
/* State 495 */
  { 186,   0, 496}, /*  1:                 orconf shift  496 */
  {  91,   0, 444}, /*  2:                     OR shift  444 */
/* State 496 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 497}, /*  2:                     nm shift  497 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 497 */
  { 150,   2, 498}, /*  1:                   dbnm shift  498 */
  { 108,   3, 680}, /*  2:                    SET reduce 125 */
  {  39,   0, 270}, /*  3:                    DOT shift  270 */
/* State 498 */
  { 108,   0, 499}, /*  1:                    SET shift  499 */
/* State 499 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   0, 455}, /*  2:                     nm shift  455 */
  {  59,   0,  20}, /*  3:                     ID shift  20 */
  { 198,   1, 500}, /*  4:                setlist shift  500 */
  {  74,   3,  22}, /*  5:                JOIN_KW shift  22 */
/* State 500 */
  {  22,   0, 450}, /*  1:                  COMMA shift  450 */
  { 217,   0, 501}, /*  2:              where_opt shift  501 */
  { 130,   1, 234}, /*  3:                  WHERE shift  234 */
  { 107,   0, 710}, /*  4:                   SEMI reduce 155 */
/* State 501 */
  { 107,   0, 712}, /*  1:                   SEMI reduce 157 */
/* State 502 */
  {  70,   0, 503}, /*  1:                   INTO shift  503 */
/* State 503 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 504}, /*  2:                     nm shift  504 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 504 */
  { 150,   0, 505}, /*  1:                   dbnm shift  505 */
  {  39,   0, 270}, /*  2:                    DOT shift  270 */
/* State 505 */
  { 172,   0, 506}, /*  1:         inscollist_opt shift  506 */
  {  79,   0, 462}, /*  2:                     LP shift  462 */
/* State 506 */
  { 196,   0, 507}, /*  1:                 select shift  507 */
  { 185,   0,  69}, /*  2:              oneselect shift  69 */
  { 106,   0,  73}, /*  3:                 SELECT shift  73 */
  { 127,   0, 508}, /*  4:                 VALUES shift  508 */
/* State 507 */
  { 180,   0,  71}, /*  1:         multiselect_op shift  71 */
  { 121,   4, 162}, /*  2:                  UNION shift  162 */
  { 107,   0, 716}, /*  3:                   SEMI reduce 161 */
  {  46,   0, 165}, /*  4:                 EXCEPT shift  165 */
  {  69,   0, 164}, /*  5:              INTERSECT shift  164 */
/* State 508 */
  {  79,   0, 509}, /*  1:                     LP shift  509 */
/* State 509 */
  {  97,   3, 188}, /*  1:                  RAISE shift  188 */
  { 113,   1,  66}, /*  2:                 STRING shift  66 */
  {  49,   7, 169}, /*  3:                  FLOAT shift  169 */
  {  83,   0, 174}, /*  4:                  MINUS shift  174 */
  {  68,   0, 168}, /*  5:                INTEGER shift  168 */
  { 181,   9, 102}, /*  6:                     nm shift  102 */
  {  17,   0, 178}, /*  7:                   CASE shift  178 */
  {  87,   0, 101}, /*  8:                   NULL shift  101 */
  {  85,   0, 170}, /*  9:                    NOT shift  170 */
  {  13,   0, 172}, /* 10:                 BITNOT shift  172 */
  {  74,   0,  67}, /* 11:                JOIN_KW shift  67 */
  {  59,   0,  64}, /* 12:                     ID shift  64 */
  {  94,   0, 176}, /* 13:                   PLUS shift  176 */
  { 157,  10, 473}, /* 14:                   expr shift  473 */
  { 174,  13, 510}, /* 15:               itemlist shift  510 */
  {  79,   0,  68}, /* 16:                     LP shift  68 */
/* State 510 */
  { 104,   2, 511}, /*  1:                     RP shift  511 */
  {  22,   0, 470}, /*  2:                  COMMA shift  470 */
/* State 511 */
  { 107,   0, 715}, /*  1:                   SEMI reduce 160 */
/* State 512 */
  { 186,   0, 513}, /*  1:                 orconf shift  513 */
  {  91,   3, 444}, /*  2:                     OR shift  444 */
  {  70,   0, 645}, /*  3:                   INTO reduce 90 */
/* State 513 */
  {  70,   0, 717}, /*  1:                   INTO reduce 162 */
/* State 514 */
  {  70,   0, 718}, /*  1:                   INTO reduce 163 */
/* State 515 */
  { 186,   0, 516}, /*  1:                 orconf shift  516 */
  {  91,   0, 444}, /*  2:                     OR shift  444 */
/* State 516 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 517}, /*  2:                     nm shift  517 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 517 */
  { 150,   3, 518}, /*  1:                   dbnm shift  518 */
  {  52,   0, 680}, /*  2:                   FROM reduce 125 */
  {  39,   0, 270}, /*  3:                    DOT shift  270 */
/* State 518 */
  {  52,   0, 519}, /*  1:                   FROM shift  519 */
/* State 519 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 520}, /*  2:                     nm shift  520 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 520 */
  { 107,   0, 796}, /*  1:                   SEMI reduce 241 */
  { 125,   1, 521}, /*  2:                  USING shift  521 */
/* State 521 */
  {  35,   0, 522}, /*  1:             DELIMITERS shift  522 */
/* State 522 */
  { 113,   0, 523}, /*  1:                 STRING shift  523 */
/* State 523 */
  { 107,   0, 795}, /*  1:                   SEMI reduce 240 */
/* State 524 */
  {  59,   0,  20}, /*  1:                     ID shift  20 */
  { 181,   0, 525}, /*  2:                     nm shift  525 */
  { 107,   0, 797}, /*  3:                   SEMI reduce 242 */
  { 113,   0,  21}, /*  4:                 STRING shift  21 */
  {  74,   1,  22}, /*  5:                JOIN_KW shift  22 */
/* State 525 */
  { 107,   0, 798}, /*  1:                   SEMI reduce 243 */
/* State 526 */
  { 165,   0, 527}, /*  1:                    ids shift  527 */
  {  59,   0, 251}, /*  2:                     ID shift  251 */
  { 113,   2, 252}, /*  3:                 STRING shift  252 */
/* State 527 */
  {  45,   0, 528}, /*  1:                     EQ shift  528 */
  {  79,   0, 540}, /*  2:                     LP shift  540 */
  { 107,   0, 804}, /*  3:                   SEMI reduce 249 */
/* State 528 */
  {  90,   0, 530}, /*  1:                     ON shift  530 */
  { 181,   0, 529}, /*  2:                     nm shift  529 */
  {  83,   0, 537}, /*  3:                  MINUS shift  537 */
  { 113,   3,  21}, /*  4:                 STRING shift  21 */
  {  94,   6, 539}, /*  5:                   PLUS shift  539 */
  {  74,   0,  22}, /*  6:                JOIN_KW shift  22 */
  { 179,   8, 532}, /*  7:              minus_num shift  532 */
  {  59,   0,  20}, /*  8:                     ID shift  20 */
  { 188,   0, 531}, /*  9:               plus_num shift  531 */
  { 189,   7, 533}, /* 10:               plus_opt shift  533 */
/* State 529 */
  { 107,   0, 799}, /*  1:                   SEMI reduce 244 */
/* State 530 */
  { 107,   0, 800}, /*  1:                   SEMI reduce 245 */
/* State 531 */
  { 107,   0, 801}, /*  1:                   SEMI reduce 246 */
/* State 532 */
  { 107,   0, 802}, /*  1:                   SEMI reduce 247 */
/* State 533 */
  {  68,   0, 535}, /*  1:                INTEGER shift  535 */
  {  49,   0, 536}, /*  2:                  FLOAT shift  536 */
  { 182,   1, 534}, /*  3:                 number shift  534 */
/* State 534 */
  { 107,   0, 805}, /*  1:                   SEMI reduce 250 */
/* State 535 */
  { 107,   0, 807}, /*  1:                   SEMI reduce 252 */
/* State 536 */
  { 107,   0, 808}, /*  1:                   SEMI reduce 253 */
/* State 537 */
  {  68,   0, 535}, /*  1:                INTEGER shift  535 */
  {  49,   0, 536}, /*  2:                  FLOAT shift  536 */
  { 182,   1, 538}, /*  3:                 number shift  538 */
/* State 538 */
  { 107,   0, 806}, /*  1:                   SEMI reduce 251 */
/* State 540 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 541}, /*  2:                     nm shift  541 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 541 */
  { 104,   0, 542}, /*  1:                     RP shift  542 */
/* State 542 */
  { 107,   0, 803}, /*  1:                   SEMI reduce 248 */
/* State 543 */
  {  30,   0, 548}, /*  1:               DATABASE shift  548 */
  { 149,   0, 544}, /*  2:        database_kw_opt shift  544 */
/* State 544 */
  { 165,   0, 545}, /*  1:                    ids shift  545 */
  {  59,   0, 251}, /*  2:                     ID shift  251 */
  { 113,   2, 252}, /*  3:                 STRING shift  252 */
/* State 545 */
  {   6,   0, 546}, /*  1:                     AS shift  546 */
/* State 546 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 547}, /*  2:                     nm shift  547 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 547 */
  { 107,   0, 838}, /*  1:                   SEMI reduce 283 */
/* State 549 */
  {  30,   0, 548}, /*  1:               DATABASE shift  548 */
  { 149,   0, 550}, /*  2:        database_kw_opt shift  550 */
/* State 550 */
  { 113,   0,  21}, /*  1:                 STRING shift  21 */
  { 181,   1, 551}, /*  2:                     nm shift  551 */
  {  74,   0,  22}, /*  3:                JOIN_KW shift  22 */
  {  59,   0,  20}, /*  4:                     ID shift  20 */
/* State 551 */
  { 107,   0, 841}, /*  1:                   SEMI reduce 286 */
};

/* The state table contains information needed to look up the correct
** action in the action table, given the current state of the parser.
** Information needed includes:
**
**  +  A pointer to the start of the action hash table in yyActionTable.
**
**  +  The number of entries in the action hash table.
**
**  +  The default action.  This is the action to take if no entry for
**     the given look-ahead is found in the action hash table.
*/
struct yyStateEntry {
  const yyActionEntry *hashtbl;  /* Start of the hash table in yyActionTable */
  YYCODETYPE nEntry;             /* Number of entries in action hash table */
  YYACTIONTYPE actionDefault;    /* Default action if look-ahead not found */
};
typedef struct yyStateEntry yyStateEntry;
static const yyStateEntry yyStateTable[] = {
  { &yyActionTable[0],   6, 562 },
  { &yyActionTable[6],   5, 562 },
  { &yyActionTable[11],   0, 557 },
  { &yyActionTable[11],  22, 842 },
  { &yyActionTable[33],   1, 842 },
  { &yyActionTable[34],   0, 558 },
  { &yyActionTable[34],   1, 842 },
  { &yyActionTable[35],   2, 564 },
  { &yyActionTable[37],   3, 842 },
  { &yyActionTable[40],   1, 842 },
  { &yyActionTable[41],   1, 842 },
  { &yyActionTable[42],   6, 842 },
  { &yyActionTable[48],   0, 644 },
  { &yyActionTable[48],   0, 647 },
  { &yyActionTable[48],   0, 648 },
  { &yyActionTable[48],   0, 649 },
  { &yyActionTable[48],   0, 650 },
  { &yyActionTable[48],   0, 651 },
  { &yyActionTable[48],   4, 565 },
  { &yyActionTable[52],   0, 566 },
  { &yyActionTable[52],   0, 583 },
  { &yyActionTable[52],   0, 584 },
  { &yyActionTable[52],   0, 585 },
  { &yyActionTable[52],   3, 842 },
  { &yyActionTable[55],   1, 842 },
  { &yyActionTable[56],   3, 842 },
  { &yyActionTable[59],   1, 842 },
  { &yyActionTable[60],   3, 842 },
  { &yyActionTable[63],   1, 842 },
  { &yyActionTable[64],   3, 842 },
  { &yyActionTable[67],   1, 842 },
  { &yyActionTable[68],   7, 842 },
  { &yyActionTable[75],   3, 842 },
  { &yyActionTable[78],   1, 842 },
  { &yyActionTable[79],   1, 842 },
  { &yyActionTable[80],  13, 842 },
  { &yyActionTable[93],   0, 576 },
  { &yyActionTable[93],   5, 586 },
  { &yyActionTable[98],   1, 596 },
  { &yyActionTable[99],  13, 578 },
  { &yyActionTable[112],   0, 595 },
  { &yyActionTable[112],   4, 842 },
  { &yyActionTable[116],  10, 842 },
  { &yyActionTable[126],   0, 597 },
  { &yyActionTable[126],   2, 643 },
  { &yyActionTable[128],   0, 608 },
  { &yyActionTable[128],   2, 842 },
  { &yyActionTable[130],   2, 643 },
  { &yyActionTable[132],   0, 609 },
  { &yyActionTable[132],   2, 628 },
  { &yyActionTable[134],   0, 626 },
  { &yyActionTable[134],   2, 842 },
  { &yyActionTable[136],   0, 629 },
  { &yyActionTable[136],   0, 630 },
  { &yyActionTable[136],   1, 842 },
  { &yyActionTable[137],   3, 698 },
  { &yyActionTable[140],   2, 643 },
  { &yyActionTable[142],   0, 610 },
  { &yyActionTable[142],   0, 696 },
  { &yyActionTable[142],   0, 697 },
  { &yyActionTable[142],   2, 643 },
  { &yyActionTable[144],   0, 611 },
  { &yyActionTable[144],   1, 842 },
  { &yyActionTable[145],  15, 842 },
  { &yyActionTable[160],   2, 727 },
  { &yyActionTable[162],  18, 785 },
  { &yyActionTable[180],   1, 734 },
  { &yyActionTable[181],   1, 728 },
  { &yyActionTable[182],  18, 842 },
  { &yyActionTable[200],   0, 656 },
  { &yyActionTable[200],   5, 842 },
  { &yyActionTable[205],   2, 842 },
  { &yyActionTable[207],   0, 657 },
  { &yyActionTable[207],   3, 665 },
  { &yyActionTable[210],   2, 667 },
  { &yyActionTable[212],   3, 674 },
  { &yyActionTable[215],   2, 710 },
  { &yyActionTable[217],   2, 701 },
  { &yyActionTable[219],   2, 703 },
  { &yyActionTable[221],   2, 691 },
  { &yyActionTable[223],   2, 705 },
  { &yyActionTable[225],   0, 662 },
  { &yyActionTable[225],   1, 842 },
  { &yyActionTable[226],   2, 706 },
  { &yyActionTable[228],   1, 842 },
  { &yyActionTable[229],   0, 707 },
  { &yyActionTable[229],   1, 842 },
  { &yyActionTable[230],   0, 708 },
  { &yyActionTable[230],   1, 842 },
  { &yyActionTable[231],  17, 842 },
  { &yyActionTable[248],   1, 692 },
  { &yyActionTable[249],  16, 842 },
  { &yyActionTable[265],   2, 699 },
  { &yyActionTable[267],   3, 698 },
  { &yyActionTable[270],   0, 693 },
  { &yyActionTable[270],   2, 842 },
  { &yyActionTable[272],   0, 580 },
  { &yyActionTable[272],   0, 700 },
  { &yyActionTable[272],  28, 695 },
  { &yyActionTable[300],   0, 731 },
  { &yyActionTable[300],  15, 842 },
  { &yyActionTable[315],   0, 726 },
  { &yyActionTable[315],   1, 842 },
  { &yyActionTable[316],   4, 842 },
  { &yyActionTable[320],   1, 729 },
  { &yyActionTable[321],   4, 842 },
  { &yyActionTable[325],   0, 730 },
  { &yyActionTable[325],  26, 737 },
  { &yyActionTable[351],  15, 842 },
  { &yyActionTable[366],  27, 738 },
  { &yyActionTable[393],  15, 842 },
  { &yyActionTable[408],  12, 739 },
  { &yyActionTable[420],  15, 842 },
  { &yyActionTable[435],  12, 740 },
  { &yyActionTable[447],  15, 842 },
  { &yyActionTable[462],  12, 741 },
  { &yyActionTable[474],  15, 842 },
  { &yyActionTable[489],  12, 742 },
  { &yyActionTable[501],  15, 842 },
  { &yyActionTable[516],  16, 743 },
  { &yyActionTable[532],  15, 842 },
  { &yyActionTable[547],  16, 744 },
  { &yyActionTable[563],  15, 842 },
  { &yyActionTable[578],   8, 745 },
  { &yyActionTable[586],  15, 842 },
  { &yyActionTable[601],   8, 746 },
  { &yyActionTable[609],  15, 842 },
  { &yyActionTable[624],   8, 747 },
  { &yyActionTable[632],  15, 842 },
  { &yyActionTable[647],   8, 748 },
  { &yyActionTable[655],  15, 842 },
  { &yyActionTable[670],  16, 749 },
  { &yyActionTable[686],   6, 842 },
  { &yyActionTable[692],  15, 842 },
  { &yyActionTable[707],  16, 750 },
  { &yyActionTable[723],   0, 751 },
  { &yyActionTable[723],   0, 752 },
  { &yyActionTable[723],  15, 842 },
  { &yyActionTable[738],   6, 753 },
  { &yyActionTable[744],  15, 842 },
  { &yyActionTable[759],   6, 754 },
  { &yyActionTable[765],  15, 842 },
  { &yyActionTable[780],   3, 755 },
  { &yyActionTable[783],  15, 842 },
  { &yyActionTable[798],   3, 756 },
  { &yyActionTable[801],  15, 842 },
  { &yyActionTable[816],   3, 757 },
  { &yyActionTable[819],  15, 842 },
  { &yyActionTable[834],   2, 758 },
  { &yyActionTable[836],   0, 759 },
  { &yyActionTable[836],   2, 842 },
  { &yyActionTable[838],   0, 760 },
  { &yyActionTable[838],   1, 842 },
  { &yyActionTable[839],   0, 763 },
  { &yyActionTable[839],   0, 761 },
  { &yyActionTable[839],  15, 842 },
  { &yyActionTable[854],  28, 842 },
  { &yyActionTable[882],  15, 842 },
  { &yyActionTable[897],  16, 769 },
  { &yyActionTable[913],   1, 842 },
  { &yyActionTable[914],  20, 785 },
  { &yyActionTable[934],   5, 842 },
  { &yyActionTable[939],   2, 842 },
  { &yyActionTable[941],   1, 842 },
  { &yyActionTable[942],   1, 842 },
  { &yyActionTable[943],   1, 842 },
  { &yyActionTable[944],   0, 772 },
  { &yyActionTable[944],  28, 784 },
  { &yyActionTable[972],   0, 732 },
  { &yyActionTable[972],   0, 733 },
  { &yyActionTable[972],  15, 842 },
  { &yyActionTable[987],  26, 764 },
  { &yyActionTable[1013],  15, 842 },
  { &yyActionTable[1028],   2, 765 },
  { &yyActionTable[1030],  15, 842 },
  { &yyActionTable[1045],   2, 766 },
  { &yyActionTable[1047],  15, 842 },
  { &yyActionTable[1062],   2, 767 },
  { &yyActionTable[1064],  17, 842 },
  { &yyActionTable[1081],  29, 842 },
  { &yyActionTable[1110],   2, 842 },
  { &yyActionTable[1112],   4, 842 },
  { &yyActionTable[1116],   1, 842 },
  { &yyActionTable[1117],   0, 775 },
  { &yyActionTable[1117],  15, 842 },
  { &yyActionTable[1132],  29, 842 },
  { &yyActionTable[1161],  15, 842 },
  { &yyActionTable[1176],  28, 776 },
  { &yyActionTable[1204],   1, 842 },
  { &yyActionTable[1205],   4, 842 },
  { &yyActionTable[1209],   1, 842 },
  { &yyActionTable[1210],   0, 833 },
  { &yyActionTable[1210],   1, 842 },
  { &yyActionTable[1211],   4, 842 },
  { &yyActionTable[1215],   1, 842 },
  { &yyActionTable[1216],   0, 834 },
  { &yyActionTable[1216],   1, 842 },
  { &yyActionTable[1217],   4, 842 },
  { &yyActionTable[1221],   1, 842 },
  { &yyActionTable[1222],   0, 835 },
  { &yyActionTable[1222],   1, 842 },
  { &yyActionTable[1223],   4, 842 },
  { &yyActionTable[1227],   1, 842 },
  { &yyActionTable[1228],   0, 836 },
  { &yyActionTable[1228],  15, 842 },
  { &yyActionTable[1243],  29, 842 },
  { &yyActionTable[1272],  15, 842 },
  { &yyActionTable[1287],  29, 842 },
  { &yyActionTable[1316],  15, 842 },
  { &yyActionTable[1331],  28, 777 },
  { &yyActionTable[1359],   2, 842 },
  { &yyActionTable[1361],   0, 771 },
  { &yyActionTable[1361],  16, 785 },
  { &yyActionTable[1377],   0, 782 },
  { &yyActionTable[1377],   0, 783 },
  { &yyActionTable[1377],   0, 762 },
  { &yyActionTable[1377],  15, 842 },
  { &yyActionTable[1392],  28, 842 },
  { &yyActionTable[1420],  15, 842 },
  { &yyActionTable[1435],  26, 770 },
  { &yyActionTable[1461],   1, 842 },
  { &yyActionTable[1462],  20, 785 },
  { &yyActionTable[1482],   5, 842 },
  { &yyActionTable[1487],   0, 774 },
  { &yyActionTable[1487],   2, 842 },
  { &yyActionTable[1489],   0, 773 },
  { &yyActionTable[1489],   2, 699 },
  { &yyActionTable[1491],   3, 698 },
  { &yyActionTable[1494],   0, 694 },
  { &yyActionTable[1494],  15, 842 },
  { &yyActionTable[1509],  28, 704 },
  { &yyActionTable[1537],   1, 842 },
  { &yyActionTable[1538],  17, 785 },
  { &yyActionTable[1555],   1, 702 },
  { &yyActionTable[1556],  15, 842 },
  { &yyActionTable[1571],  28, 711 },
  { &yyActionTable[1599],   0, 666 },
  { &yyActionTable[1599],   2, 677 },
  { &yyActionTable[1601],   4, 675 },
  { &yyActionTable[1605],   0, 676 },
  { &yyActionTable[1605],   0, 682 },
  { &yyActionTable[1605],   0, 683 },
  { &yyActionTable[1605],   5, 842 },
  { &yyActionTable[1610],   0, 684 },
  { &yyActionTable[1610],   5, 842 },
  { &yyActionTable[1615],   0, 685 },
  { &yyActionTable[1615],   1, 842 },
  { &yyActionTable[1616],   0, 686 },
  { &yyActionTable[1616],   5, 842 },
  { &yyActionTable[1621],   2, 680 },
  { &yyActionTable[1623],   5, 673 },
  { &yyActionTable[1628],   0, 581 },
  { &yyActionTable[1628],   0, 582 },
  { &yyActionTable[1628],   4, 842 },
  { &yyActionTable[1632],   0, 671 },
  { &yyActionTable[1632],   0, 672 },
  { &yyActionTable[1632],   2, 688 },
  { &yyActionTable[1634],   2, 690 },
  { &yyActionTable[1636],   0, 678 },
  { &yyActionTable[1636],   1, 842 },
  { &yyActionTable[1637],   6, 842 },
  { &yyActionTable[1643],   2, 842 },
  { &yyActionTable[1645],   0, 689 },
  { &yyActionTable[1645],   5, 842 },
  { &yyActionTable[1650],   0, 791 },
  { &yyActionTable[1650],   3, 698 },
  { &yyActionTable[1653],   0, 793 },
  { &yyActionTable[1653],   0, 792 },
  { &yyActionTable[1653],  15, 842 },
  { &yyActionTable[1668],  28, 687 },
  { &yyActionTable[1696],   4, 842 },
  { &yyActionTable[1700],   0, 681 },
  { &yyActionTable[1700],   3, 842 },
  { &yyActionTable[1703],   5, 842 },
  { &yyActionTable[1708],   5, 673 },
  { &yyActionTable[1713],   2, 688 },
  { &yyActionTable[1715],   2, 690 },
  { &yyActionTable[1717],   0, 679 },
  { &yyActionTable[1717],  16, 842 },
  { &yyActionTable[1733],  33, 673 },
  { &yyActionTable[1766],   0, 668 },
  { &yyActionTable[1766],   0, 669 },
  { &yyActionTable[1766],   1, 842 },
  { &yyActionTable[1767],   5, 842 },
  { &yyActionTable[1772],   0, 670 },
  { &yyActionTable[1772],   0, 663 },
  { &yyActionTable[1772],   0, 664 },
  { &yyActionTable[1772],   0, 768 },
  { &yyActionTable[1772],  29, 842 },
  { &yyActionTable[1801],   0, 725 },
  { &yyActionTable[1801],   2, 842 },
  { &yyActionTable[1803],   0, 735 },
  { &yyActionTable[1803],   1, 842 },
  { &yyActionTable[1804],   0, 736 },
  { &yyActionTable[1804],  29, 842 },
  { &yyActionTable[1833],   2, 643 },
  { &yyActionTable[1835],   0, 612 },
  { &yyActionTable[1835],   4, 842 },
  { &yyActionTable[1839],   2, 789 },
  { &yyActionTable[1841],   1, 616 },
  { &yyActionTable[1842],   3, 613 },
  { &yyActionTable[1845],   0, 617 },
  { &yyActionTable[1845],   4, 842 },
  { &yyActionTable[1849],   0, 618 },
  { &yyActionTable[1849],   3, 842 },
  { &yyActionTable[1852],   4, 842 },
  { &yyActionTable[1856],   0, 619 },
  { &yyActionTable[1856],   2, 842 },
  { &yyActionTable[1858],   0, 622 },
  { &yyActionTable[1858],   0, 623 },
  { &yyActionTable[1858],   0, 624 },
  { &yyActionTable[1858],   0, 625 },
  { &yyActionTable[1858],   4, 842 },
  { &yyActionTable[1862],   0, 620 },
  { &yyActionTable[1862],   4, 842 },
  { &yyActionTable[1866],   0, 621 },
  { &yyActionTable[1866],   6, 842 },
  { &yyActionTable[1872],   2, 842 },
  { &yyActionTable[1874],   0, 790 },
  { &yyActionTable[1874],   0, 614 },
  { &yyActionTable[1874],   2, 842 },
  { &yyActionTable[1876],   0, 615 },
  { &yyActionTable[1876],   2, 628 },
  { &yyActionTable[1878],   0, 627 },
  { &yyActionTable[1878],   0, 598 },
  { &yyActionTable[1878],   7, 842 },
  { &yyActionTable[1885],   0, 599 },
  { &yyActionTable[1885],   0, 600 },
  { &yyActionTable[1885],   0, 601 },
  { &yyActionTable[1885],   2, 842 },
  { &yyActionTable[1887],   0, 602 },
  { &yyActionTable[1887],   0, 605 },
  { &yyActionTable[1887],   2, 842 },
  { &yyActionTable[1889],   0, 603 },
  { &yyActionTable[1889],   0, 606 },
  { &yyActionTable[1889],   0, 604 },
  { &yyActionTable[1889],   0, 607 },
  { &yyActionTable[1889],   4, 587 },
  { &yyActionTable[1893],   4, 842 },
  { &yyActionTable[1897],   2, 842 },
  { &yyActionTable[1899],   0, 588 },
  { &yyActionTable[1899],   4, 842 },
  { &yyActionTable[1903],   1, 842 },
  { &yyActionTable[1904],   0, 589 },
  { &yyActionTable[1904],   0, 592 },
  { &yyActionTable[1904],   1, 842 },
  { &yyActionTable[1905],   0, 593 },
  { &yyActionTable[1905],   1, 842 },
  { &yyActionTable[1906],   0, 594 },
  { &yyActionTable[1906],   0, 591 },
  { &yyActionTable[1906],   0, 590 },
  { &yyActionTable[1906],   0, 579 },
  { &yyActionTable[1906],   8, 842 },
  { &yyActionTable[1914],   6, 842 },
  { &yyActionTable[1920],   0, 633 },
  { &yyActionTable[1920],   4, 842 },
  { &yyActionTable[1924],   0, 636 },
  { &yyActionTable[1924],   1, 842 },
  { &yyActionTable[1925],   1, 842 },
  { &yyActionTable[1926],   6, 842 },
  { &yyActionTable[1932],   2, 842 },
  { &yyActionTable[1934],   2, 643 },
  { &yyActionTable[1936],   0, 637 },
  { &yyActionTable[1936],   1, 842 },
  { &yyActionTable[1937],   6, 842 },
  { &yyActionTable[1943],   2, 842 },
  { &yyActionTable[1945],   2, 643 },
  { &yyActionTable[1947],   0, 638 },
  { &yyActionTable[1947],  15, 842 },
  { &yyActionTable[1962],  30, 643 },
  { &yyActionTable[1992],   0, 639 },
  { &yyActionTable[1992],   1, 842 },
  { &yyActionTable[1993],   1, 842 },
  { &yyActionTable[1994],   6, 842 },
  { &yyActionTable[2000],   2, 842 },
  { &yyActionTable[2002],   1, 842 },
  { &yyActionTable[2003],   4, 842 },
  { &yyActionTable[2007],   2, 789 },
  { &yyActionTable[2009],   1, 616 },
  { &yyActionTable[2010],   7, 641 },
  { &yyActionTable[2017],   1, 842 },
  { &yyActionTable[2018],   0, 640 },
  { &yyActionTable[2018],   0, 642 },
  { &yyActionTable[2018],   0, 634 },
  { &yyActionTable[2018],   0, 635 },
  { &yyActionTable[2018],   0, 577 },
  { &yyActionTable[2018],   3, 842 },
  { &yyActionTable[2021],   5, 842 },
  { &yyActionTable[2026],   3, 573 },
  { &yyActionTable[2029],   6, 842 },
  { &yyActionTable[2035],   4, 842 },
  { &yyActionTable[2039],   0, 571 },
  { &yyActionTable[2039],   4, 842 },
  { &yyActionTable[2043],   1, 842 },
  { &yyActionTable[2044],   3, 842 },
  { &yyActionTable[2047],   5, 842 },
  { &yyActionTable[2052],   1, 842 },
  { &yyActionTable[2053],   4, 842 },
  { &yyActionTable[2057],   1, 842 },
  { &yyActionTable[2058],   4, 842 },
  { &yyActionTable[2062],   3, 842 },
  { &yyActionTable[2065],   1, 842 },
  { &yyActionTable[2066],   6, 842 },
  { &yyActionTable[2072],   2, 842 },
  { &yyActionTable[2074],   3, 842 },
  { &yyActionTable[2077],   1, 842 },
  { &yyActionTable[2078],   1, 842 },
  { &yyActionTable[2079],   4, 842 },
  { &yyActionTable[2083],   4, 816 },
  { &yyActionTable[2087],   4, 842 },
  { &yyActionTable[2091],   1, 842 },
  { &yyActionTable[2092],   4, 842 },
  { &yyActionTable[2096],   2, 680 },
  { &yyActionTable[2098],   2, 821 },
  { &yyActionTable[2100],   3, 842 },
  { &yyActionTable[2103],   1, 842 },
  { &yyActionTable[2104],  15, 842 },
  { &yyActionTable[2119],  29, 842 },
  { &yyActionTable[2148],   1, 842 },
  { &yyActionTable[2149],   2, 842 },
  { &yyActionTable[2151],   0, 822 },
  { &yyActionTable[2151],   0, 823 },
  { &yyActionTable[2151],   1, 842 },
  { &yyActionTable[2152],   1, 842 },
  { &yyActionTable[2153],   2, 842 },
  { &yyActionTable[2155],   5, 842 },
  { &yyActionTable[2160],   2, 842 },
  { &yyActionTable[2162],   4, 842 },
  { &yyActionTable[2166],   0, 723 },
  { &yyActionTable[2166],   0, 724 },
  { &yyActionTable[2166],   0, 813 },
  { &yyActionTable[2166],   0, 814 },
  { &yyActionTable[2166],   1, 842 },
  { &yyActionTable[2167],   0, 815 },
  { &yyActionTable[2167],   0, 572 },
  { &yyActionTable[2167],   1, 842 },
  { &yyActionTable[2168],   9, 842 },
  { &yyActionTable[2177],   5, 842 },
  { &yyActionTable[2182],   1, 842 },
  { &yyActionTable[2183],   1, 842 },
  { &yyActionTable[2184],   1, 842 },
  { &yyActionTable[2185],   9, 842 },
  { &yyActionTable[2194],   1, 842 },
  { &yyActionTable[2195],   2, 645 },
  { &yyActionTable[2197],   6, 842 },
  { &yyActionTable[2203],   0, 646 },
  { &yyActionTable[2203],   4, 842 },
  { &yyActionTable[2207],   1, 842 },
  { &yyActionTable[2208],   5, 842 },
  { &yyActionTable[2213],   4, 842 },
  { &yyActionTable[2217],   4, 842 },
  { &yyActionTable[2221],   1, 842 },
  { &yyActionTable[2222],  15, 842 },
  { &yyActionTable[2237],  28, 713 },
  { &yyActionTable[2265],   1, 842 },
  { &yyActionTable[2266],   1, 842 },
  { &yyActionTable[2267],  15, 842 },
  { &yyActionTable[2282],  28, 714 },
  { &yyActionTable[2310],   3, 842 },
  { &yyActionTable[2313],   1, 842 },
  { &yyActionTable[2314],   4, 842 },
  { &yyActionTable[2318],   2, 721 },
  { &yyActionTable[2320],   5, 842 },
  { &yyActionTable[2325],   2, 842 },
  { &yyActionTable[2327],   0, 722 },
  { &yyActionTable[2327],   4, 842 },
  { &yyActionTable[2331],   5, 842 },
  { &yyActionTable[2336],   1, 842 },
  { &yyActionTable[2337],  16, 842 },
  { &yyActionTable[2353],   2, 842 },
  { &yyActionTable[2355],  15, 842 },
  { &yyActionTable[2370],  28, 719 },
  { &yyActionTable[2398],   1, 842 },
  { &yyActionTable[2399],  28, 720 },
  { &yyActionTable[2427],   1, 842 },
  { &yyActionTable[2428],   4, 842 },
  { &yyActionTable[2432],   3, 842 },
  { &yyActionTable[2435],   1, 842 },
  { &yyActionTable[2436],   4, 842 },
  { &yyActionTable[2440],   4, 842 },
  { &yyActionTable[2444],   1, 842 },
  { &yyActionTable[2445],   4, 842 },
  { &yyActionTable[2449],   1, 842 },
  { &yyActionTable[2450],   4, 842 },
  { &yyActionTable[2454],   3, 842 },
  { &yyActionTable[2457],   1, 842 },
  { &yyActionTable[2458],   4, 842 },
  { &yyActionTable[2462],   3, 842 },
  { &yyActionTable[2465],   1, 842 },
  { &yyActionTable[2466],   5, 842 },
  { &yyActionTable[2471],   1, 842 },
  { &yyActionTable[2472],   4, 842 },
  { &yyActionTable[2476],   2, 680 },
  { &yyActionTable[2478],   3, 842 },
  { &yyActionTable[2481],   1, 842 },
  { &yyActionTable[2482],   2, 645 },
  { &yyActionTable[2484],   4, 842 },
  { &yyActionTable[2488],   3, 842 },
  { &yyActionTable[2491],   1, 842 },
  { &yyActionTable[2492],   5, 842 },
  { &yyActionTable[2497],   4, 842 },
  { &yyActionTable[2501],   1, 842 },
  { &yyActionTable[2502],   1, 842 },
  { &yyActionTable[2503],   4, 842 },
  { &yyActionTable[2507],   2, 680 },
  { &yyActionTable[2509],   2, 721 },
  { &yyActionTable[2511],   4, 842 },
  { &yyActionTable[2515],   5, 842 },
  { &yyActionTable[2520],   1, 842 },
  { &yyActionTable[2521],  16, 842 },
  { &yyActionTable[2537],   2, 842 },
  { &yyActionTable[2539],   1, 842 },
  { &yyActionTable[2540],   3, 842 },
  { &yyActionTable[2543],   1, 842 },
  { &yyActionTable[2544],   1, 842 },
  { &yyActionTable[2545],   2, 645 },
  { &yyActionTable[2547],   4, 842 },
  { &yyActionTable[2551],   3, 842 },
  { &yyActionTable[2554],   1, 842 },
  { &yyActionTable[2555],   4, 842 },
  { &yyActionTable[2559],   2, 842 },
  { &yyActionTable[2561],   1, 842 },
  { &yyActionTable[2562],   1, 842 },
  { &yyActionTable[2563],   1, 842 },
  { &yyActionTable[2564],   5, 842 },
  { &yyActionTable[2569],   1, 842 },
  { &yyActionTable[2570],   3, 842 },
  { &yyActionTable[2573],   3, 842 },
  { &yyActionTable[2576],  10, 810 },
  { &yyActionTable[2586],   1, 842 },
  { &yyActionTable[2587],   1, 842 },
  { &yyActionTable[2588],   1, 842 },
  { &yyActionTable[2589],   1, 842 },
  { &yyActionTable[2590],   3, 842 },
  { &yyActionTable[2593],   1, 842 },
  { &yyActionTable[2594],   1, 842 },
  { &yyActionTable[2595],   1, 842 },
  { &yyActionTable[2596],   3, 842 },
  { &yyActionTable[2599],   1, 842 },
  { &yyActionTable[2600],   0, 809 },
  { &yyActionTable[2600],   4, 842 },
  { &yyActionTable[2604],   1, 842 },
  { &yyActionTable[2605],   1, 842 },
  { &yyActionTable[2606],   2, 840 },
  { &yyActionTable[2608],   3, 842 },
  { &yyActionTable[2611],   1, 842 },
  { &yyActionTable[2612],   4, 842 },
  { &yyActionTable[2616],   1, 842 },
  { &yyActionTable[2617],   0, 839 },
  { &yyActionTable[2617],   2, 840 },
  { &yyActionTable[2619],   4, 842 },
  { &yyActionTable[2623],   1, 842 },
  { &yyActionTable[2624],   0, 559 },
  { &yyActionTable[2624],   0, 561 },
  { &yyActionTable[2624],   0, 556 },
};

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammer, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
    0,  /*          $ => nothing */
   59,  /*      ABORT => ID */
   59,  /*      AFTER => ID */
    0,  /* AGG_FUNCTION => nothing */
    0,  /*        ALL => nothing */
    0,  /*        AND => nothing */
    0,  /*         AS => nothing */
   59,  /*        ASC => ID */
   59,  /*     ATTACH => ID */
   59,  /*     BEFORE => ID */
   59,  /*      BEGIN => ID */
    0,  /*    BETWEEN => nothing */
    0,  /*     BITAND => nothing */
    0,  /*     BITNOT => nothing */
    0,  /*      BITOR => nothing */
    0,  /*         BY => nothing */
   59,  /*    CASCADE => ID */
    0,  /*       CASE => nothing */
    0,  /*      CHECK => nothing */
   59,  /*    CLUSTER => ID */
    0,  /*    COLLATE => nothing */
    0,  /*     COLUMN => nothing */
    0,  /*      COMMA => nothing */
    0,  /*    COMMENT => nothing */
    0,  /*     COMMIT => nothing */
    0,  /*     CONCAT => nothing */
   59,  /*   CONFLICT => ID */
    0,  /* CONSTRAINT => nothing */
   59,  /*       COPY => ID */
    0,  /*     CREATE => nothing */
   59,  /*   DATABASE => ID */
    0,  /*    DEFAULT => nothing */
    0,  /* DEFERRABLE => nothing */
   59,  /*   DEFERRED => ID */
    0,  /*     DELETE => nothing */
   59,  /* DELIMITERS => ID */
   59,  /*       DESC => ID */
   59,  /*     DETACH => ID */
    0,  /*   DISTINCT => nothing */
    0,  /*        DOT => nothing */
    0,  /*       DROP => nothing */
   59,  /*       EACH => ID */
    0,  /*       ELSE => nothing */
   59,  /*        END => ID */
    0,  /* END_OF_FILE => nothing */
    0,  /*         EQ => nothing */
    0,  /*     EXCEPT => nothing */
   59,  /*    EXPLAIN => ID */
   59,  /*       FAIL => ID */
    0,  /*      FLOAT => nothing */
   59,  /*        FOR => ID */
    0,  /*    FOREIGN => nothing */
    0,  /*       FROM => nothing */
    0,  /*   FUNCTION => nothing */
    0,  /*         GE => nothing */
    0,  /*       GLOB => nothing */
    0,  /*      GROUP => nothing */
    0,  /*         GT => nothing */
    0,  /*     HAVING => nothing */
    0,  /*         ID => nothing */
   59,  /*     IGNORE => ID */
    0,  /*    ILLEGAL => nothing */
   59,  /*  IMMEDIATE => ID */
    0,  /*         IN => nothing */
    0,  /*      INDEX => nothing */
   59,  /*  INITIALLY => ID */
    0,  /*     INSERT => nothing */
   59,  /*    INSTEAD => ID */
    0,  /*    INTEGER => nothing */
    0,  /*  INTERSECT => nothing */
    0,  /*       INTO => nothing */
    0,  /*         IS => nothing */
    0,  /*     ISNULL => nothing */
    0,  /*       JOIN => nothing */
    0,  /*    JOIN_KW => nothing */
   59,  /*        KEY => ID */
    0,  /*         LE => nothing */
    0,  /*       LIKE => nothing */
    0,  /*      LIMIT => nothing */
    0,  /*         LP => nothing */
    0,  /*     LSHIFT => nothing */
    0,  /*         LT => nothing */
   59,  /*      MATCH => ID */
    0,  /*      MINUS => nothing */
    0,  /*         NE => nothing */
    0,  /*        NOT => nothing */
    0,  /*    NOTNULL => nothing */
    0,  /*       NULL => nothing */
   59,  /*         OF => ID */
   59,  /*     OFFSET => ID */
    0,  /*         ON => nothing */
    0,  /*         OR => nothing */
    0,  /* ORACLE_OUTER_JOIN => nothing */
    0,  /*      ORDER => nothing */
    0,  /*       PLUS => nothing */
   59,  /*     PRAGMA => ID */
    0,  /*    PRIMARY => nothing */
   59,  /*      RAISE => ID */
    0,  /* REFERENCES => nothing */
    0,  /*        REM => nothing */
   59,  /*    REPLACE => ID */
   59,  /*   RESTRICT => ID */
    0,  /*   ROLLBACK => nothing */
   59,  /*        ROW => ID */
    0,  /*         RP => nothing */
    0,  /*     RSHIFT => nothing */
    0,  /*     SELECT => nothing */
    0,  /*       SEMI => nothing */
    0,  /*        SET => nothing */
    0,  /*      SLASH => nothing */
    0,  /*      SPACE => nothing */
    0,  /*       STAR => nothing */
   59,  /*  STATEMENT => ID */
    0,  /*     STRING => nothing */
    0,  /*      TABLE => nothing */
   59,  /*       TEMP => ID */
    0,  /*       THEN => nothing */
    0,  /* TRANSACTION => nothing */
   59,  /*    TRIGGER => ID */
    0,  /*     UMINUS => nothing */
    0,  /* UNCLOSED_STRING => nothing */
    0,  /*      UNION => nothing */
    0,  /*     UNIQUE => nothing */
    0,  /*     UPDATE => nothing */
    0,  /*      UPLUS => nothing */
    0,  /*      USING => nothing */
   59,  /*     VACUUM => ID */
    0,  /*     VALUES => nothing */
   59,  /*       VIEW => ID */
    0,  /*       WHEN => nothing */
    0,  /*      WHERE => nothing */
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
  int stateno;       /* The state-number */
  int major;         /* The major token value.  This is the code
                     ** number for the token at this stack level */
  YYMINORTYPE minor; /* The user-supplied minor token value.  This
                     ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
  int yyerrcnt;                 /* Shifts left before out of the error */
  yyStackEntry *yytop;          /* Pointer to the top stack element */
  sqliteParserARG_SDECL                /* A place to hold %extra_argument */
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
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
static const char *yyTokenName[] = { 
  "$",             "ABORT",         "AFTER",         "AGG_FUNCTION",
  "ALL",           "AND",           "AS",            "ASC",         
  "ATTACH",        "BEFORE",        "BEGIN",         "BETWEEN",     
  "BITAND",        "BITNOT",        "BITOR",         "BY",          
  "CASCADE",       "CASE",          "CHECK",         "CLUSTER",     
  "COLLATE",       "COLUMN",        "COMMA",         "COMMENT",     
  "COMMIT",        "CONCAT",        "CONFLICT",      "CONSTRAINT",  
  "COPY",          "CREATE",        "DATABASE",      "DEFAULT",     
  "DEFERRABLE",    "DEFERRED",      "DELETE",        "DELIMITERS",  
  "DESC",          "DETACH",        "DISTINCT",      "DOT",         
  "DROP",          "EACH",          "ELSE",          "END",         
  "END_OF_FILE",   "EQ",            "EXCEPT",        "EXPLAIN",     
  "FAIL",          "FLOAT",         "FOR",           "FOREIGN",     
  "FROM",          "FUNCTION",      "GE",            "GLOB",        
  "GROUP",         "GT",            "HAVING",        "ID",          
  "IGNORE",        "ILLEGAL",       "IMMEDIATE",     "IN",          
  "INDEX",         "INITIALLY",     "INSERT",        "INSTEAD",     
  "INTEGER",       "INTERSECT",     "INTO",          "IS",          
  "ISNULL",        "JOIN",          "JOIN_KW",       "KEY",         
  "LE",            "LIKE",          "LIMIT",         "LP",          
  "LSHIFT",        "LT",            "MATCH",         "MINUS",       
  "NE",            "NOT",           "NOTNULL",       "NULL",        
  "OF",            "OFFSET",        "ON",            "OR",          
  "ORACLE_OUTER_JOIN",  "ORDER",         "PLUS",          "PRAGMA",      
  "PRIMARY",       "RAISE",         "REFERENCES",    "REM",         
  "REPLACE",       "RESTRICT",      "ROLLBACK",      "ROW",         
  "RP",            "RSHIFT",        "SELECT",        "SEMI",        
  "SET",           "SLASH",         "SPACE",         "STAR",        
  "STATEMENT",     "STRING",        "TABLE",         "TEMP",        
  "THEN",          "TRANSACTION",   "TRIGGER",       "UMINUS",      
  "UNCLOSED_STRING",  "UNION",         "UNIQUE",        "UPDATE",      
  "UPLUS",         "USING",         "VACUUM",        "VALUES",      
  "VIEW",          "WHEN",          "WHERE",         "as",          
  "carg",          "carglist",      "case_else",     "case_exprlist",
  "case_operand",  "ccons",         "cmd",           "cmdlist",     
  "cmdx",          "collate",       "column",        "columnid",    
  "columnlist",    "conslist",      "conslist_opt",  "create_table",
  "create_table_args",  "database_kw_opt",  "dbnm",          "defer_subclause",
  "defer_subclause_opt",  "distinct",      "ecmd",          "error",       
  "explain",       "expr",          "expritem",      "exprlist",    
  "foreach_clause",  "from",          "groupby_opt",   "having_opt",  
  "id",            "ids",           "idxitem",       "idxlist",     
  "idxlist_opt",   "init_deferred_pred_opt",  "input",         "inscollist",  
  "inscollist_opt",  "insert_cmd",    "itemlist",      "joinop",      
  "joinop2",       "likeop",        "limit_opt",     "minus_num",   
  "multiselect_op",  "nm",            "number",        "on_opt",      
  "onconf",        "oneselect",     "orconf",        "orderby_opt", 
  "plus_num",      "plus_opt",      "refact",        "refarg",      
  "refargs",       "resolvetype",   "sclp",          "selcollist",  
  "select",        "seltablist",    "setlist",       "signed",      
  "sortitem",      "sortlist",      "sortorder",     "stl_prefix",  
  "tcons",         "temp",          "trans_opt",     "trigger_cmd", 
  "trigger_cmd_list",  "trigger_decl",  "trigger_event",  "trigger_time",
  "type",          "typename",      "uniqueflag",    "using_opt",   
  "when_clause",   "where_opt",   
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *yyRuleName[] = {
 /*   0 */ "input ::= cmdlist",
 /*   1 */ "cmdlist ::= ecmd",
 /*   2 */ "cmdlist ::= cmdlist ecmd",
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
 /* 124 */ "seltablist ::= stl_prefix LP select RP as on_opt using_opt",
 /* 125 */ "dbnm ::=",
 /* 126 */ "dbnm ::= DOT nm",
 /* 127 */ "joinop ::= COMMA",
 /* 128 */ "joinop ::= JOIN",
 /* 129 */ "joinop ::= JOIN_KW JOIN",
 /* 130 */ "joinop ::= JOIN_KW nm JOIN",
 /* 131 */ "joinop ::= JOIN_KW nm nm JOIN",
 /* 132 */ "on_opt ::= ON expr",
 /* 133 */ "on_opt ::=",
 /* 134 */ "using_opt ::= USING LP idxlist RP",
 /* 135 */ "using_opt ::=",
 /* 136 */ "orderby_opt ::=",
 /* 137 */ "orderby_opt ::= ORDER BY sortlist",
 /* 138 */ "sortlist ::= sortlist COMMA sortitem collate sortorder",
 /* 139 */ "sortlist ::= sortitem collate sortorder",
 /* 140 */ "sortitem ::= expr",
 /* 141 */ "sortorder ::= ASC",
 /* 142 */ "sortorder ::= DESC",
 /* 143 */ "sortorder ::=",
 /* 144 */ "collate ::=",
 /* 145 */ "collate ::= COLLATE id",
 /* 146 */ "groupby_opt ::=",
 /* 147 */ "groupby_opt ::= GROUP BY exprlist",
 /* 148 */ "having_opt ::=",
 /* 149 */ "having_opt ::= HAVING expr",
 /* 150 */ "limit_opt ::=",
 /* 151 */ "limit_opt ::= LIMIT INTEGER",
 /* 152 */ "limit_opt ::= LIMIT INTEGER OFFSET INTEGER",
 /* 153 */ "limit_opt ::= LIMIT INTEGER COMMA INTEGER",
 /* 154 */ "cmd ::= DELETE FROM nm dbnm where_opt",
 /* 155 */ "where_opt ::=",
 /* 156 */ "where_opt ::= WHERE expr",
 /* 157 */ "cmd ::= UPDATE orconf nm dbnm SET setlist where_opt",
 /* 158 */ "setlist ::= setlist COMMA nm EQ expr",
 /* 159 */ "setlist ::= nm EQ expr",
 /* 160 */ "cmd ::= insert_cmd INTO nm dbnm inscollist_opt VALUES LP itemlist RP",
 /* 161 */ "cmd ::= insert_cmd INTO nm dbnm inscollist_opt select",
 /* 162 */ "insert_cmd ::= INSERT orconf",
 /* 163 */ "insert_cmd ::= REPLACE",
 /* 164 */ "itemlist ::= itemlist COMMA expr",
 /* 165 */ "itemlist ::= expr",
 /* 166 */ "inscollist_opt ::=",
 /* 167 */ "inscollist_opt ::= LP inscollist RP",
 /* 168 */ "inscollist ::= inscollist COMMA nm",
 /* 169 */ "inscollist ::= nm",
 /* 170 */ "expr ::= LP expr RP",
 /* 171 */ "expr ::= NULL",
 /* 172 */ "expr ::= ID",
 /* 173 */ "expr ::= JOIN_KW",
 /* 174 */ "expr ::= nm DOT nm",
 /* 175 */ "expr ::= nm DOT nm DOT nm",
 /* 176 */ "expr ::= expr ORACLE_OUTER_JOIN",
 /* 177 */ "expr ::= INTEGER",
 /* 178 */ "expr ::= FLOAT",
 /* 179 */ "expr ::= STRING",
 /* 180 */ "expr ::= ID LP exprlist RP",
 /* 181 */ "expr ::= ID LP STAR RP",
 /* 182 */ "expr ::= expr AND expr",
 /* 183 */ "expr ::= expr OR expr",
 /* 184 */ "expr ::= expr LT expr",
 /* 185 */ "expr ::= expr GT expr",
 /* 186 */ "expr ::= expr LE expr",
 /* 187 */ "expr ::= expr GE expr",
 /* 188 */ "expr ::= expr NE expr",
 /* 189 */ "expr ::= expr EQ expr",
 /* 190 */ "expr ::= expr BITAND expr",
 /* 191 */ "expr ::= expr BITOR expr",
 /* 192 */ "expr ::= expr LSHIFT expr",
 /* 193 */ "expr ::= expr RSHIFT expr",
 /* 194 */ "expr ::= expr likeop expr",
 /* 195 */ "expr ::= expr NOT likeop expr",
 /* 196 */ "likeop ::= LIKE",
 /* 197 */ "likeop ::= GLOB",
 /* 198 */ "expr ::= expr PLUS expr",
 /* 199 */ "expr ::= expr MINUS expr",
 /* 200 */ "expr ::= expr STAR expr",
 /* 201 */ "expr ::= expr SLASH expr",
 /* 202 */ "expr ::= expr REM expr",
 /* 203 */ "expr ::= expr CONCAT expr",
 /* 204 */ "expr ::= expr ISNULL",
 /* 205 */ "expr ::= expr IS NULL",
 /* 206 */ "expr ::= expr NOTNULL",
 /* 207 */ "expr ::= expr NOT NULL",
 /* 208 */ "expr ::= expr IS NOT NULL",
 /* 209 */ "expr ::= NOT expr",
 /* 210 */ "expr ::= BITNOT expr",
 /* 211 */ "expr ::= MINUS expr",
 /* 212 */ "expr ::= PLUS expr",
 /* 213 */ "expr ::= LP select RP",
 /* 214 */ "expr ::= expr BETWEEN expr AND expr",
 /* 215 */ "expr ::= expr NOT BETWEEN expr AND expr",
 /* 216 */ "expr ::= expr IN LP exprlist RP",
 /* 217 */ "expr ::= expr IN LP select RP",
 /* 218 */ "expr ::= expr NOT IN LP exprlist RP",
 /* 219 */ "expr ::= expr NOT IN LP select RP",
 /* 220 */ "expr ::= CASE case_operand case_exprlist case_else END",
 /* 221 */ "case_exprlist ::= case_exprlist WHEN expr THEN expr",
 /* 222 */ "case_exprlist ::= WHEN expr THEN expr",
 /* 223 */ "case_else ::= ELSE expr",
 /* 224 */ "case_else ::=",
 /* 225 */ "case_operand ::= expr",
 /* 226 */ "case_operand ::=",
 /* 227 */ "exprlist ::= exprlist COMMA expritem",
 /* 228 */ "exprlist ::= expritem",
 /* 229 */ "expritem ::= expr",
 /* 230 */ "expritem ::=",
 /* 231 */ "cmd ::= CREATE temp uniqueflag INDEX nm ON nm dbnm LP idxlist RP onconf",
 /* 232 */ "uniqueflag ::= UNIQUE",
 /* 233 */ "uniqueflag ::=",
 /* 234 */ "idxlist_opt ::=",
 /* 235 */ "idxlist_opt ::= LP idxlist RP",
 /* 236 */ "idxlist ::= idxlist COMMA idxitem",
 /* 237 */ "idxlist ::= idxitem",
 /* 238 */ "idxitem ::= nm sortorder",
 /* 239 */ "cmd ::= DROP INDEX nm dbnm",
 /* 240 */ "cmd ::= COPY orconf nm dbnm FROM nm USING DELIMITERS STRING",
 /* 241 */ "cmd ::= COPY orconf nm dbnm FROM nm",
 /* 242 */ "cmd ::= VACUUM",
 /* 243 */ "cmd ::= VACUUM nm",
 /* 244 */ "cmd ::= PRAGMA ids EQ nm",
 /* 245 */ "cmd ::= PRAGMA ids EQ ON",
 /* 246 */ "cmd ::= PRAGMA ids EQ plus_num",
 /* 247 */ "cmd ::= PRAGMA ids EQ minus_num",
 /* 248 */ "cmd ::= PRAGMA ids LP nm RP",
 /* 249 */ "cmd ::= PRAGMA ids",
 /* 250 */ "plus_num ::= plus_opt number",
 /* 251 */ "minus_num ::= MINUS number",
 /* 252 */ "number ::= INTEGER",
 /* 253 */ "number ::= FLOAT",
 /* 254 */ "plus_opt ::= PLUS",
 /* 255 */ "plus_opt ::=",
 /* 256 */ "cmd ::= CREATE trigger_decl BEGIN trigger_cmd_list END",
 /* 257 */ "trigger_decl ::= temp TRIGGER nm trigger_time trigger_event ON nm dbnm foreach_clause when_clause",
 /* 258 */ "trigger_time ::= BEFORE",
 /* 259 */ "trigger_time ::= AFTER",
 /* 260 */ "trigger_time ::= INSTEAD OF",
 /* 261 */ "trigger_time ::=",
 /* 262 */ "trigger_event ::= DELETE",
 /* 263 */ "trigger_event ::= INSERT",
 /* 264 */ "trigger_event ::= UPDATE",
 /* 265 */ "trigger_event ::= UPDATE OF inscollist",
 /* 266 */ "foreach_clause ::=",
 /* 267 */ "foreach_clause ::= FOR EACH ROW",
 /* 268 */ "foreach_clause ::= FOR EACH STATEMENT",
 /* 269 */ "when_clause ::=",
 /* 270 */ "when_clause ::= WHEN expr",
 /* 271 */ "trigger_cmd_list ::= trigger_cmd SEMI trigger_cmd_list",
 /* 272 */ "trigger_cmd_list ::=",
 /* 273 */ "trigger_cmd ::= UPDATE orconf nm SET setlist where_opt",
 /* 274 */ "trigger_cmd ::= INSERT orconf INTO nm inscollist_opt VALUES LP itemlist RP",
 /* 275 */ "trigger_cmd ::= INSERT orconf INTO nm inscollist_opt select",
 /* 276 */ "trigger_cmd ::= DELETE FROM nm where_opt",
 /* 277 */ "trigger_cmd ::= select",
 /* 278 */ "expr ::= RAISE LP IGNORE RP",
 /* 279 */ "expr ::= RAISE LP ROLLBACK COMMA nm RP",
 /* 280 */ "expr ::= RAISE LP ABORT COMMA nm RP",
 /* 281 */ "expr ::= RAISE LP FAIL COMMA nm RP",
 /* 282 */ "cmd ::= DROP TRIGGER nm dbnm",
 /* 283 */ "cmd ::= ATTACH database_kw_opt ids AS nm",
 /* 284 */ "database_kw_opt ::= DATABASE",
 /* 285 */ "database_kw_opt ::=",
 /* 286 */ "cmd ::= DETACH database_kw_opt nm",
};
#endif /* NDEBUG */

/*
** This function returns the symbolic name associated with a token
** value.
*/
const char *sqliteParserTokenName(int tokenType){
#ifndef NDEBUG
  if( tokenType>0 && tokenType<(sizeof(yyTokenName)/sizeof(yyTokenName[0])) ){
    return yyTokenName[tokenType];
  }else{
    return "Unknown";
  }
#else
  return "";
#endif
}

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
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(YYCODETYPE yymajor, YYMINORTYPE *yypminor){
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
    case 135:
#line 673 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy62));}
#line 4380 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 157:
#line 520 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy334));}
#line 4385 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 158:
#line 692 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy334));}
#line 4390 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 159:
#line 690 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy62));}
#line 4395 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 161:
#line 341 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteSrcListDelete((yypminor->yy335));}
#line 4400 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 162:
#line 435 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy62));}
#line 4405 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 163:
#line 440 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy334));}
#line 4410 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 167:
#line 715 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy92));}
#line 4415 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 168:
#line 717 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy92));}
#line 4420 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 171:
#line 498 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy92));}
#line 4425 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 172:
#line 496 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy92));}
#line 4430 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 174:
#line 490 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy62));}
#line 4435 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 183:
#line 396 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy334));}
#line 4440 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 185:
#line 276 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteSelectDelete((yypminor->yy11));}
#line 4445 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 187:
#line 407 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy62));}
#line 4450 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 194:
#line 312 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy62));}
#line 4455 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 195:
#line 310 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy62));}
#line 4460 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 196:
#line 274 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteSelectDelete((yypminor->yy11));}
#line 4465 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 197:
#line 337 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteSrcListDelete((yypminor->yy335));}
#line 4470 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 198:
#line 465 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy62));}
#line 4475 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 200:
#line 411 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy334));}
#line 4480 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 201:
#line 409 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy62));}
#line 4485 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 203:
#line 339 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteSrcListDelete((yypminor->yy335));}
#line 4490 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 207:
#line 807 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteDeleteTriggerStep((yypminor->yy347));}
#line 4495 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 208:
#line 799 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteDeleteTriggerStep((yypminor->yy347));}
#line 4500 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 210:
#line 783 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy234).b);}
#line 4505 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 215:
#line 401 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy92));}
#line 4510 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
      break;
    case 217:
#line 459 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy334));}
#line 4515 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
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

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[pParser->yytop->major]);
  }
#endif
  yymajor = pParser->yytop->major;
  yy_destructor( yymajor, &pParser->yytop->minor);
  pParser->yyidx--;
  pParser->yytop--;
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
  (*freeProc)((void*)pParser);
}

/*
** Find the appropriate action for a parser given the look-ahead token.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_parser_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead             /* The look-ahead token */
){
  const yyStateEntry *pState;   /* Appropriate entry in the state table */
  const yyActionEntry *pAction; /* Action appropriate for the look-ahead */
  int iFallback;                /* Fallback token */
 
  /* if( pParser->yyidx<0 ) return YY_NO_ACTION;  */
  pState = &yyStateTable[pParser->yytop->stateno];
  if( pState->nEntry==0 ){
    return pState->actionDefault;
  }else if( iLookAhead!=YYNOCODE ){
    pAction = &pState->hashtbl[iLookAhead % pState->nEntry];
    while( 1 ){
      if( pAction->lookahead==iLookAhead ) return pAction->action;
      if( pAction->next==0 ) break;
      pAction = &pState->hashtbl[pAction->next-1];
    }
#ifdef YYFALLBACK
    if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
           && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
           yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
      }
#endif
      return yy_find_parser_action(pParser, iFallback);
    }
#endif
  }else if( pState->hashtbl->lookahead!=YYNOCODE ){
    return YY_NO_ACTION;
  }
  return pState->actionDefault;
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer ot the minor token to shift in */
){
  yypParser->yyidx++;
  yypParser->yytop++;
  if( yypParser->yyidx>=YYSTACKDEPTH ){
     sqliteParserARG_FETCH;
     yypParser->yyidx--;
     yypParser->yytop--;
#ifndef NDEBUG
     if( yyTraceFILE ){
       fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
     }
#endif
     while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
     /* Here code is inserted which will execute if the parser
     ** stack every overflows */
     sqliteParserARG_STORE; /* Suppress warning about unused %extra_argument var */
     return;
  }
  yypParser->yytop->stateno = yyNewState;
  yypParser->yytop->major = yyMajor;
  yypParser->yytop->minor = *yypMinor;
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
static struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 170, 1 },
  { 139, 1 },
  { 139, 2 },
  { 154, 3 },
  { 154, 1 },
  { 140, 1 },
  { 156, 1 },
  { 156, 0 },
  { 138, 3 },
  { 206, 0 },
  { 206, 1 },
  { 206, 2 },
  { 138, 2 },
  { 138, 2 },
  { 138, 2 },
  { 138, 2 },
  { 147, 4 },
  { 205, 1 },
  { 205, 0 },
  { 148, 4 },
  { 148, 2 },
  { 144, 3 },
  { 144, 1 },
  { 142, 3 },
  { 143, 1 },
  { 164, 1 },
  { 165, 1 },
  { 165, 1 },
  { 181, 1 },
  { 181, 1 },
  { 181, 1 },
  { 212, 0 },
  { 212, 1 },
  { 212, 4 },
  { 212, 6 },
  { 213, 1 },
  { 213, 2 },
  { 199, 1 },
  { 199, 2 },
  { 199, 2 },
  { 133, 2 },
  { 133, 0 },
  { 132, 3 },
  { 132, 1 },
  { 132, 2 },
  { 132, 2 },
  { 132, 2 },
  { 132, 3 },
  { 132, 3 },
  { 132, 2 },
  { 132, 3 },
  { 132, 3 },
  { 132, 2 },
  { 137, 2 },
  { 137, 3 },
  { 137, 4 },
  { 137, 2 },
  { 137, 5 },
  { 137, 4 },
  { 137, 1 },
  { 137, 2 },
  { 192, 0 },
  { 192, 2 },
  { 191, 2 },
  { 191, 3 },
  { 191, 3 },
  { 191, 3 },
  { 190, 2 },
  { 190, 2 },
  { 190, 1 },
  { 190, 1 },
  { 151, 3 },
  { 151, 2 },
  { 169, 0 },
  { 169, 2 },
  { 169, 2 },
  { 146, 0 },
  { 146, 2 },
  { 145, 3 },
  { 145, 2 },
  { 145, 1 },
  { 204, 2 },
  { 204, 6 },
  { 204, 5 },
  { 204, 3 },
  { 204, 10 },
  { 152, 0 },
  { 152, 1 },
  { 184, 0 },
  { 184, 3 },
  { 186, 0 },
  { 186, 2 },
  { 193, 1 },
  { 193, 1 },
  { 193, 1 },
  { 193, 1 },
  { 193, 1 },
  { 138, 3 },
  { 138, 6 },
  { 138, 3 },
  { 138, 1 },
  { 196, 1 },
  { 196, 3 },
  { 180, 1 },
  { 180, 2 },
  { 180, 1 },
  { 180, 1 },
  { 185, 9 },
  { 153, 1 },
  { 153, 1 },
  { 153, 0 },
  { 194, 2 },
  { 194, 0 },
  { 195, 3 },
  { 195, 2 },
  { 195, 4 },
  { 131, 2 },
  { 131, 1 },
  { 131, 0 },
  { 161, 0 },
  { 161, 2 },
  { 203, 2 },
  { 203, 0 },
  { 197, 6 },
  { 197, 7 },
  { 150, 0 },
  { 150, 2 },
  { 175, 1 },
  { 175, 1 },
  { 175, 2 },
  { 175, 3 },
  { 175, 4 },
  { 183, 2 },
  { 183, 0 },
  { 215, 4 },
  { 215, 0 },
  { 187, 0 },
  { 187, 3 },
  { 201, 5 },
  { 201, 3 },
  { 200, 1 },
  { 202, 1 },
  { 202, 1 },
  { 202, 0 },
  { 141, 0 },
  { 141, 2 },
  { 162, 0 },
  { 162, 3 },
  { 163, 0 },
  { 163, 2 },
  { 178, 0 },
  { 178, 2 },
  { 178, 4 },
  { 178, 4 },
  { 138, 5 },
  { 217, 0 },
  { 217, 2 },
  { 138, 7 },
  { 198, 5 },
  { 198, 3 },
  { 138, 9 },
  { 138, 6 },
  { 173, 2 },
  { 173, 1 },
  { 174, 3 },
  { 174, 1 },
  { 172, 0 },
  { 172, 3 },
  { 171, 3 },
  { 171, 1 },
  { 157, 3 },
  { 157, 1 },
  { 157, 1 },
  { 157, 1 },
  { 157, 3 },
  { 157, 5 },
  { 157, 2 },
  { 157, 1 },
  { 157, 1 },
  { 157, 1 },
  { 157, 4 },
  { 157, 4 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 4 },
  { 177, 1 },
  { 177, 1 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 3 },
  { 157, 2 },
  { 157, 3 },
  { 157, 2 },
  { 157, 3 },
  { 157, 4 },
  { 157, 2 },
  { 157, 2 },
  { 157, 2 },
  { 157, 2 },
  { 157, 3 },
  { 157, 5 },
  { 157, 6 },
  { 157, 5 },
  { 157, 5 },
  { 157, 6 },
  { 157, 6 },
  { 157, 5 },
  { 135, 5 },
  { 135, 4 },
  { 134, 2 },
  { 134, 0 },
  { 136, 1 },
  { 136, 0 },
  { 159, 3 },
  { 159, 1 },
  { 158, 1 },
  { 158, 0 },
  { 138, 12 },
  { 214, 1 },
  { 214, 0 },
  { 168, 0 },
  { 168, 3 },
  { 167, 3 },
  { 167, 1 },
  { 166, 2 },
  { 138, 4 },
  { 138, 9 },
  { 138, 6 },
  { 138, 1 },
  { 138, 2 },
  { 138, 4 },
  { 138, 4 },
  { 138, 4 },
  { 138, 4 },
  { 138, 5 },
  { 138, 2 },
  { 188, 2 },
  { 179, 2 },
  { 182, 1 },
  { 182, 1 },
  { 189, 1 },
  { 189, 0 },
  { 138, 5 },
  { 209, 10 },
  { 211, 1 },
  { 211, 1 },
  { 211, 2 },
  { 211, 0 },
  { 210, 1 },
  { 210, 1 },
  { 210, 1 },
  { 210, 3 },
  { 160, 0 },
  { 160, 3 },
  { 160, 3 },
  { 216, 0 },
  { 216, 2 },
  { 208, 3 },
  { 208, 0 },
  { 207, 6 },
  { 207, 9 },
  { 207, 6 },
  { 207, 4 },
  { 207, 1 },
  { 157, 4 },
  { 157, 6 },
  { 157, 6 },
  { 157, 6 },
  { 138, 4 },
  { 138, 5 },
  { 149, 1 },
  { 149, 0 },
  { 138, 3 },
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
  yymsp = yypParser->yytop;
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<sizeof(yyRuleName)/sizeof(yyRuleName[0]) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0:
        /* No destructor defined for cmdlist */
        break;
      case 1:
        /* No destructor defined for ecmd */
        break;
      case 2:
        /* No destructor defined for cmdlist */
        /* No destructor defined for ecmd */
        break;
      case 3:
        /* No destructor defined for explain */
        /* No destructor defined for cmdx */
        /* No destructor defined for SEMI */
        break;
      case 4:
        /* No destructor defined for SEMI */
        break;
      case 5:
#line 77 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ sqliteExec(pParse); }
#line 5005 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for cmd */
        break;
      case 6:
#line 78 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ sqliteBeginParse(pParse, 1); }
#line 5011 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for EXPLAIN */
        break;
      case 7:
#line 79 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ sqliteBeginParse(pParse, 0); }
#line 5017 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 8:
#line 84 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteBeginTransaction(pParse,yymsp[0].minor.yy52);}
#line 5022 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for BEGIN */
        /* No destructor defined for trans_opt */
        break;
      case 9:
        break;
      case 10:
        /* No destructor defined for TRANSACTION */
        break;
      case 11:
        /* No destructor defined for TRANSACTION */
        /* No destructor defined for nm */
        break;
      case 12:
#line 88 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteCommitTransaction(pParse);}
#line 5038 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMIT */
        /* No destructor defined for trans_opt */
        break;
      case 13:
#line 89 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteCommitTransaction(pParse);}
#line 5045 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for END */
        /* No destructor defined for trans_opt */
        break;
      case 14:
#line 90 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteRollbackTransaction(pParse);}
#line 5052 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ROLLBACK */
        /* No destructor defined for trans_opt */
        break;
      case 15:
        /* No destructor defined for create_table */
        /* No destructor defined for create_table_args */
        break;
      case 16:
#line 95 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
   sqliteStartTable(pParse,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy210,yymsp[-2].minor.yy52,0);
}
#line 5065 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for TABLE */
        break;
      case 17:
#line 99 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = 1;}
#line 5071 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for TEMP */
        break;
      case 18:
#line 100 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = 0;}
#line 5077 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 19:
#line 101 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteEndTable(pParse,&yymsp[0].minor.yy0,0);
}
#line 5084 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for columnlist */
        /* No destructor defined for conslist_opt */
        break;
      case 20:
#line 104 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteEndTable(pParse,0,yymsp[0].minor.yy11);
  sqliteSelectDelete(yymsp[0].minor.yy11);
}
#line 5095 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for AS */
        break;
      case 21:
        /* No destructor defined for columnlist */
        /* No destructor defined for COMMA */
        /* No destructor defined for column */
        break;
      case 22:
        /* No destructor defined for column */
        break;
      case 23:
        /* No destructor defined for columnid */
        /* No destructor defined for type */
        /* No destructor defined for carglist */
        break;
      case 24:
#line 116 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumn(pParse,&yymsp[0].minor.yy210);}
#line 5114 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 25:
#line 122 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy0;}
#line 5119 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 26:
#line 138 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy0;}
#line 5124 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 27:
#line 139 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy0;}
#line 5129 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 28:
#line 144 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy0;}
#line 5134 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 29:
#line 145 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy0;}
#line 5139 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 30:
#line 146 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy0;}
#line 5144 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 31:
        break;
      case 32:
#line 149 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[0].minor.yy210,&yymsp[0].minor.yy210);}
#line 5151 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 33:
#line 150 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[-3].minor.yy210,&yymsp[0].minor.yy0);}
#line 5156 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for signed */
        break;
      case 34:
#line 152 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[-5].minor.yy210,&yymsp[0].minor.yy0);}
#line 5163 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for signed */
        /* No destructor defined for COMMA */
        /* No destructor defined for signed */
        break;
      case 35:
#line 154 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy210;}
#line 5172 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 36:
#line 155 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[-1].minor.yy210;}
#line 5177 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ids */
        break;
      case 37:
        /* No destructor defined for INTEGER */
        break;
      case 38:
        /* No destructor defined for PLUS */
        /* No destructor defined for INTEGER */
        break;
      case 39:
        /* No destructor defined for MINUS */
        /* No destructor defined for INTEGER */
        break;
      case 40:
        /* No destructor defined for carglist */
        /* No destructor defined for carg */
        break;
      case 41:
        break;
      case 42:
        /* No destructor defined for CONSTRAINT */
        /* No destructor defined for nm */
        /* No destructor defined for ccons */
        break;
      case 43:
        /* No destructor defined for ccons */
        break;
      case 44:
#line 163 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5208 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        break;
      case 45:
#line 164 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5214 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        break;
      case 46:
#line 165 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5220 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        break;
      case 47:
#line 166 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5226 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        /* No destructor defined for PLUS */
        break;
      case 48:
#line 167 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,1);}
#line 5233 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        /* No destructor defined for MINUS */
        break;
      case 49:
#line 168 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5240 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        break;
      case 50:
#line 169 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5246 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        /* No destructor defined for PLUS */
        break;
      case 51:
#line 170 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,1);}
#line 5253 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        /* No destructor defined for MINUS */
        break;
      case 52:
        /* No destructor defined for DEFAULT */
        /* No destructor defined for NULL */
        break;
      case 53:
        /* No destructor defined for NULL */
        /* No destructor defined for onconf */
        break;
      case 54:
#line 177 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddNotNull(pParse, yymsp[0].minor.yy52);}
#line 5268 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        /* No destructor defined for NULL */
        break;
      case 55:
#line 178 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddPrimaryKey(pParse,0,yymsp[0].minor.yy52);}
#line 5275 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRIMARY */
        /* No destructor defined for KEY */
        /* No destructor defined for sortorder */
        break;
      case 56:
#line 179 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteCreateIndex(pParse,0,0,0,yymsp[0].minor.yy52,0,0,0);}
#line 5283 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UNIQUE */
        break;
      case 57:
        /* No destructor defined for CHECK */
        /* No destructor defined for LP */
  yy_destructor(157,&yymsp[-2].minor);
        /* No destructor defined for RP */
        /* No destructor defined for onconf */
        break;
      case 58:
#line 182 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteCreateForeignKey(pParse,0,&yymsp[-2].minor.yy210,yymsp[-1].minor.yy92,yymsp[0].minor.yy52);}
#line 5296 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for REFERENCES */
        break;
      case 59:
#line 183 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteDeferForeignKey(pParse,yymsp[0].minor.yy52);}
#line 5302 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 60:
#line 184 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
   sqliteAddCollateType(pParse, sqliteCollateType(yymsp[0].minor.yy210.z, yymsp[0].minor.yy210.n));
}
#line 5309 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COLLATE */
        break;
      case 61:
#line 194 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Restrict * 0x010101; }
#line 5315 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 62:
#line 195 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = (yymsp[-1].minor.yy52 & yymsp[0].minor.yy279.mask) | yymsp[0].minor.yy279.value; }
#line 5320 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 63:
#line 197 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy279.value = 0;     yygotominor.yy279.mask = 0x000000; }
#line 5325 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for MATCH */
        /* No destructor defined for nm */
        break;
      case 64:
#line 198 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy279.value = yymsp[0].minor.yy52;     yygotominor.yy279.mask = 0x0000ff; }
#line 5332 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ON */
        /* No destructor defined for DELETE */
        break;
      case 65:
#line 199 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy279.value = yymsp[0].minor.yy52<<8;  yygotominor.yy279.mask = 0x00ff00; }
#line 5339 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ON */
        /* No destructor defined for UPDATE */
        break;
      case 66:
#line 200 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy279.value = yymsp[0].minor.yy52<<16; yygotominor.yy279.mask = 0xff0000; }
#line 5346 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ON */
        /* No destructor defined for INSERT */
        break;
      case 67:
#line 202 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_SetNull; }
#line 5353 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for SET */
        /* No destructor defined for NULL */
        break;
      case 68:
#line 203 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_SetDflt; }
#line 5360 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for SET */
        /* No destructor defined for DEFAULT */
        break;
      case 69:
#line 204 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Cascade; }
#line 5367 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for CASCADE */
        break;
      case 70:
#line 205 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Restrict; }
#line 5373 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for RESTRICT */
        break;
      case 71:
#line 207 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = yymsp[0].minor.yy52;}
#line 5379 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        /* No destructor defined for DEFERRABLE */
        break;
      case 72:
#line 208 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = yymsp[0].minor.yy52;}
#line 5386 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFERRABLE */
        break;
      case 73:
#line 210 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = 0;}
#line 5392 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 74:
#line 211 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = 1;}
#line 5397 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INITIALLY */
        /* No destructor defined for DEFERRED */
        break;
      case 75:
#line 212 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = 0;}
#line 5404 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INITIALLY */
        /* No destructor defined for IMMEDIATE */
        break;
      case 76:
        break;
      case 77:
        /* No destructor defined for COMMA */
        /* No destructor defined for conslist */
        break;
      case 78:
        /* No destructor defined for conslist */
        /* No destructor defined for COMMA */
        /* No destructor defined for tcons */
        break;
      case 79:
        /* No destructor defined for conslist */
        /* No destructor defined for tcons */
        break;
      case 80:
        /* No destructor defined for tcons */
        break;
      case 81:
        /* No destructor defined for CONSTRAINT */
        /* No destructor defined for nm */
        break;
      case 82:
#line 224 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteAddPrimaryKey(pParse,yymsp[-2].minor.yy92,yymsp[0].minor.yy52);}
#line 5433 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRIMARY */
        /* No destructor defined for KEY */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 83:
#line 226 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteCreateIndex(pParse,0,0,yymsp[-2].minor.yy92,yymsp[0].minor.yy52,0,0,0);}
#line 5442 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UNIQUE */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 84:
        /* No destructor defined for CHECK */
  yy_destructor(157,&yymsp[-1].minor);
        /* No destructor defined for onconf */
        break;
      case 85:
#line 229 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
    sqliteCreateForeignKey(pParse, yymsp[-6].minor.yy92, &yymsp[-3].minor.yy210, yymsp[-2].minor.yy92, yymsp[-1].minor.yy52);
    sqliteDeferForeignKey(pParse, yymsp[0].minor.yy52);
}
#line 5458 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for FOREIGN */
        /* No destructor defined for KEY */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        /* No destructor defined for REFERENCES */
        break;
      case 86:
#line 234 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = 0;}
#line 5468 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 87:
#line 235 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = yymsp[0].minor.yy52;}
#line 5473 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 88:
#line 243 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Default; }
#line 5478 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 89:
#line 244 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = yymsp[0].minor.yy52; }
#line 5483 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ON */
        /* No destructor defined for CONFLICT */
        break;
      case 90:
#line 245 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Default; }
#line 5490 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 91:
#line 246 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = yymsp[0].minor.yy52; }
#line 5495 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for OR */
        break;
      case 92:
#line 247 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Rollback; }
#line 5501 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ROLLBACK */
        break;
      case 93:
#line 248 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Abort; }
#line 5507 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ABORT */
        break;
      case 94:
#line 249 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Fail; }
#line 5513 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for FAIL */
        break;
      case 95:
#line 250 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Ignore; }
#line 5519 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for IGNORE */
        break;
      case 96:
#line 251 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Replace; }
#line 5525 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for REPLACE */
        break;
      case 97:
#line 255 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteDropTable(pParse,&yymsp[0].minor.yy210,0);}
#line 5531 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DROP */
        /* No destructor defined for TABLE */
        break;
      case 98:
#line 259 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteCreateView(pParse, &yymsp[-5].minor.yy0, &yymsp[-2].minor.yy210, yymsp[0].minor.yy11, yymsp[-4].minor.yy52);
}
#line 5540 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for VIEW */
        /* No destructor defined for AS */
        break;
      case 99:
#line 262 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDropTable(pParse, &yymsp[0].minor.yy210, 1);
}
#line 5549 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DROP */
        /* No destructor defined for VIEW */
        break;
      case 100:
#line 268 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteSelect(pParse, yymsp[0].minor.yy11, SRT_Callback, 0, 0, 0, 0);
  sqliteSelectDelete(yymsp[0].minor.yy11);
}
#line 5559 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 101:
#line 278 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy11 = yymsp[0].minor.yy11;}
#line 5564 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 102:
#line 279 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  if( yymsp[0].minor.yy11 ){
    yymsp[0].minor.yy11->op = yymsp[-1].minor.yy52;
    yymsp[0].minor.yy11->pPrior = yymsp[-2].minor.yy11;
  }
  yygotominor.yy11 = yymsp[0].minor.yy11;
}
#line 5575 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 103:
#line 287 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = TK_UNION;}
#line 5580 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UNION */
        break;
      case 104:
#line 288 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = TK_ALL;}
#line 5586 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UNION */
        /* No destructor defined for ALL */
        break;
      case 105:
#line 289 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = TK_INTERSECT;}
#line 5593 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INTERSECT */
        break;
      case 106:
#line 290 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = TK_EXCEPT;}
#line 5599 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for EXCEPT */
        break;
      case 107:
#line 292 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy11 = sqliteSelectNew(yymsp[-6].minor.yy62,yymsp[-5].minor.yy335,yymsp[-4].minor.yy334,yymsp[-3].minor.yy62,yymsp[-2].minor.yy334,yymsp[-1].minor.yy62,yymsp[-7].minor.yy52,yymsp[0].minor.yy280.limit,yymsp[0].minor.yy280.offset);
}
#line 5607 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for SELECT */
        break;
      case 108:
#line 300 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = 1;}
#line 5613 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DISTINCT */
        break;
      case 109:
#line 301 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = 0;}
#line 5619 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ALL */
        break;
      case 110:
#line 302 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = 0;}
#line 5625 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 111:
#line 313 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = yymsp[-1].minor.yy62;}
#line 5630 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 112:
#line 314 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = 0;}
#line 5636 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 113:
#line 315 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
   yygotominor.yy62 = sqliteExprListAppend(yymsp[-2].minor.yy62,yymsp[-1].minor.yy334,yymsp[0].minor.yy210.n?&yymsp[0].minor.yy210:0);
}
#line 5643 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 114:
#line 318 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy62 = sqliteExprListAppend(yymsp[-1].minor.yy62, sqliteExpr(TK_ALL, 0, 0, 0), 0);
}
#line 5650 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for STAR */
        break;
      case 115:
#line 321 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  Expr *pRight = sqliteExpr(TK_ALL, 0, 0, 0);
  Expr *pLeft = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy210);
  yygotominor.yy62 = sqliteExprListAppend(yymsp[-3].minor.yy62, sqliteExpr(TK_DOT, pLeft, pRight, 0), 0);
}
#line 5660 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DOT */
        /* No destructor defined for STAR */
        break;
      case 116:
#line 331 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy210 = yymsp[0].minor.yy210; }
#line 5667 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for AS */
        break;
      case 117:
#line 332 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy210 = yymsp[0].minor.yy210; }
#line 5673 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 118:
#line 333 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy210.n = 0; }
#line 5678 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 119:
#line 345 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy335 = sqliteMalloc(sizeof(*yygotominor.yy335));}
#line 5683 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 120:
#line 346 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy335 = yymsp[0].minor.yy335;}
#line 5688 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for FROM */
        break;
      case 121:
#line 351 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
   yygotominor.yy335 = yymsp[-1].minor.yy335;
   if( yygotominor.yy335 && yygotominor.yy335->nSrc>0 ) yygotominor.yy335->a[yygotominor.yy335->nSrc-1].jointype = yymsp[0].minor.yy52;
}
#line 5697 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 122:
#line 355 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy335 = 0;}
#line 5702 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 123:
#line 356 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy335 = sqliteSrcListAppend(yymsp[-5].minor.yy335,&yymsp[-4].minor.yy210,&yymsp[-3].minor.yy210);
  if( yymsp[-2].minor.yy210.n ) sqliteSrcListAddAlias(yygotominor.yy335,&yymsp[-2].minor.yy210);
  if( yymsp[-1].minor.yy334 ){
    if( yygotominor.yy335 && yygotominor.yy335->nSrc>1 ){ yygotominor.yy335->a[yygotominor.yy335->nSrc-2].pOn = yymsp[-1].minor.yy334; }
    else { sqliteExprDelete(yymsp[-1].minor.yy334); }
  }
  if( yymsp[0].minor.yy92 ){
    if( yygotominor.yy335 && yygotominor.yy335->nSrc>1 ){ yygotominor.yy335->a[yygotominor.yy335->nSrc-2].pUsing = yymsp[0].minor.yy92; }
    else { sqliteIdListDelete(yymsp[0].minor.yy92); }
  }
}
#line 5718 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 124:
#line 368 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy335 = sqliteSrcListAppend(yymsp[-6].minor.yy335,0,0);
  yygotominor.yy335->a[yygotominor.yy335->nSrc-1].pSelect = yymsp[-4].minor.yy11;
  if( yymsp[-2].minor.yy210.n ) sqliteSrcListAddAlias(yygotominor.yy335,&yymsp[-2].minor.yy210);
  if( yymsp[-1].minor.yy334 ){
    if( yygotominor.yy335 && yygotominor.yy335->nSrc>1 ){ yygotominor.yy335->a[yygotominor.yy335->nSrc-2].pOn = yymsp[-1].minor.yy334; }
    else { sqliteExprDelete(yymsp[-1].minor.yy334); }
  }
  if( yymsp[0].minor.yy92 ){
    if( yygotominor.yy335 && yygotominor.yy335->nSrc>1 ){ yygotominor.yy335->a[yygotominor.yy335->nSrc-2].pUsing = yymsp[0].minor.yy92; }
    else { sqliteIdListDelete(yymsp[0].minor.yy92); }
  }
}
#line 5735 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 125:
#line 383 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210.z=0; yygotominor.yy210.n=0;}
#line 5742 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 126:
#line 384 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy210;}
#line 5747 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DOT */
        break;
      case 127:
#line 388 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = JT_INNER; }
#line 5753 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 128:
#line 389 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = JT_INNER; }
#line 5759 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for JOIN */
        break;
      case 129:
#line 390 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = sqliteJoinType(pParse,&yymsp[-1].minor.yy0,0,0); }
#line 5765 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for JOIN */
        break;
      case 130:
#line 391 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = sqliteJoinType(pParse,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy210,0); }
#line 5771 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for JOIN */
        break;
      case 131:
#line 393 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = sqliteJoinType(pParse,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy210,&yymsp[-1].minor.yy210); }
#line 5777 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for JOIN */
        break;
      case 132:
#line 397 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = yymsp[0].minor.yy334;}
#line 5783 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ON */
        break;
      case 133:
#line 398 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = 0;}
#line 5789 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 134:
#line 402 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy92 = yymsp[-1].minor.yy92;}
#line 5794 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for USING */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 135:
#line 403 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy92 = 0;}
#line 5802 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 136:
#line 413 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = 0;}
#line 5807 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 137:
#line 414 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = yymsp[0].minor.yy62;}
#line 5812 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ORDER */
        /* No destructor defined for BY */
        break;
      case 138:
#line 415 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy62 = sqliteExprListAppend(yymsp[-4].minor.yy62,yymsp[-2].minor.yy334,0);
  if( yygotominor.yy62 ) yygotominor.yy62->a[yygotominor.yy62->nExpr-1].sortOrder = yymsp[-1].minor.yy52+yymsp[0].minor.yy52;
}
#line 5822 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 139:
#line 419 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy62 = sqliteExprListAppend(0,yymsp[-2].minor.yy334,0);
  if( yygotominor.yy62 ) yygotominor.yy62->a[0].sortOrder = yymsp[-1].minor.yy52+yymsp[0].minor.yy52;
}
#line 5831 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 140:
#line 423 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = yymsp[0].minor.yy334;}
#line 5836 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 141:
#line 428 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = SQLITE_SO_ASC;}
#line 5841 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ASC */
        break;
      case 142:
#line 429 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = SQLITE_SO_DESC;}
#line 5847 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DESC */
        break;
      case 143:
#line 430 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = SQLITE_SO_ASC;}
#line 5853 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 144:
#line 431 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = SQLITE_SO_UNK;}
#line 5858 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 145:
#line 432 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = sqliteCollateType(yymsp[0].minor.yy210.z, yymsp[0].minor.yy210.n);}
#line 5863 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COLLATE */
        break;
      case 146:
#line 436 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = 0;}
#line 5869 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 147:
#line 437 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = yymsp[0].minor.yy62;}
#line 5874 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for GROUP */
        /* No destructor defined for BY */
        break;
      case 148:
#line 441 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = 0;}
#line 5881 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 149:
#line 442 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = yymsp[0].minor.yy334;}
#line 5886 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for HAVING */
        break;
      case 150:
#line 445 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy280.limit = -1; yygotominor.yy280.offset = 0;}
#line 5892 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 151:
#line 446 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy280.limit = atoi(yymsp[0].minor.yy0.z); yygotominor.yy280.offset = 0;}
#line 5897 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LIMIT */
        break;
      case 152:
#line 448 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy280.limit = atoi(yymsp[-2].minor.yy0.z); yygotominor.yy280.offset = atoi(yymsp[0].minor.yy0.z);}
#line 5903 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LIMIT */
        /* No destructor defined for OFFSET */
        break;
      case 153:
#line 450 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy280.limit = atoi(yymsp[0].minor.yy0.z); yygotominor.yy280.offset = atoi(yymsp[-2].minor.yy0.z);}
#line 5910 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LIMIT */
        /* No destructor defined for COMMA */
        break;
      case 154:
#line 454 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
   sqliteDeleteFrom(pParse, sqliteSrcListAppend(0,&yymsp[-2].minor.yy210,&yymsp[-1].minor.yy210), yymsp[0].minor.yy334);
}
#line 5919 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DELETE */
        /* No destructor defined for FROM */
        break;
      case 155:
#line 461 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = 0;}
#line 5926 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 156:
#line 462 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = yymsp[0].minor.yy334;}
#line 5931 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for WHERE */
        break;
      case 157:
#line 470 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteUpdate(pParse,sqliteSrcListAppend(0,&yymsp[-4].minor.yy210,&yymsp[-3].minor.yy210),yymsp[-1].minor.yy62,yymsp[0].minor.yy334,yymsp[-5].minor.yy52);}
#line 5937 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UPDATE */
        /* No destructor defined for SET */
        break;
      case 158:
#line 473 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = sqliteExprListAppend(yymsp[-4].minor.yy62,yymsp[0].minor.yy334,&yymsp[-2].minor.yy210);}
#line 5944 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        /* No destructor defined for EQ */
        break;
      case 159:
#line 474 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = sqliteExprListAppend(0,yymsp[0].minor.yy334,&yymsp[-2].minor.yy210);}
#line 5951 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for EQ */
        break;
      case 160:
#line 480 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteInsert(pParse, sqliteSrcListAppend(0,&yymsp[-6].minor.yy210,&yymsp[-5].minor.yy210), yymsp[-1].minor.yy62, 0, yymsp[-4].minor.yy92, yymsp[-8].minor.yy52);}
#line 5957 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INTO */
        /* No destructor defined for VALUES */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 161:
#line 482 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteInsert(pParse, sqliteSrcListAppend(0,&yymsp[-3].minor.yy210,&yymsp[-2].minor.yy210), 0, yymsp[0].minor.yy11, yymsp[-1].minor.yy92, yymsp[-5].minor.yy52);}
#line 5966 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INTO */
        break;
      case 162:
#line 485 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = yymsp[0].minor.yy52;}
#line 5972 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INSERT */
        break;
      case 163:
#line 486 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = OE_Replace;}
#line 5978 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for REPLACE */
        break;
      case 164:
#line 492 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = sqliteExprListAppend(yymsp[-2].minor.yy62,yymsp[0].minor.yy334,0);}
#line 5984 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 165:
#line 493 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = sqliteExprListAppend(0,yymsp[0].minor.yy334,0);}
#line 5990 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 166:
#line 500 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy92 = 0;}
#line 5995 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 167:
#line 501 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy92 = yymsp[-1].minor.yy92;}
#line 6000 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 168:
#line 502 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy92 = sqliteIdListAppend(yymsp[-2].minor.yy92,&yymsp[0].minor.yy210);}
#line 6007 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 169:
#line 503 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy92 = sqliteIdListAppend(0,&yymsp[0].minor.yy210);}
#line 6013 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 170:
#line 522 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = yymsp[-1].minor.yy334; sqliteExprSpan(yygotominor.yy334,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0); }
#line 6018 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 171:
#line 523 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_NULL, 0, 0, &yymsp[0].minor.yy0);}
#line 6023 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 172:
#line 524 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy0);}
#line 6028 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 173:
#line 525 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy0);}
#line 6033 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 174:
#line 526 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  Expr *temp1 = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy210);
  Expr *temp2 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy210);
  yygotominor.yy334 = sqliteExpr(TK_DOT, temp1, temp2, 0);
}
#line 6042 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DOT */
        break;
      case 175:
#line 531 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  Expr *temp1 = sqliteExpr(TK_ID, 0, 0, &yymsp[-4].minor.yy210);
  Expr *temp2 = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy210);
  Expr *temp3 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy210);
  Expr *temp4 = sqliteExpr(TK_DOT, temp2, temp3, 0);
  yygotominor.yy334 = sqliteExpr(TK_DOT, temp1, temp4, 0);
}
#line 6054 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DOT */
        /* No destructor defined for DOT */
        break;
      case 176:
#line 539 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = yymsp[-1].minor.yy334; ExprSetProperty(yygotominor.yy334,EP_Oracle8Join);}
#line 6061 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ORACLE_OUTER_JOIN */
        break;
      case 177:
#line 540 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_INTEGER, 0, 0, &yymsp[0].minor.yy0);}
#line 6067 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 178:
#line 541 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_FLOAT, 0, 0, &yymsp[0].minor.yy0);}
#line 6072 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 179:
#line 542 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_STRING, 0, 0, &yymsp[0].minor.yy0);}
#line 6077 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 180:
#line 543 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExprFunction(yymsp[-1].minor.yy62, &yymsp[-3].minor.yy0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);
}
#line 6085 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        break;
      case 181:
#line 547 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExprFunction(0, &yymsp[-3].minor.yy0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);
}
#line 6094 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for STAR */
        break;
      case 182:
#line 551 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_AND, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6101 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for AND */
        break;
      case 183:
#line 552 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_OR, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6107 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for OR */
        break;
      case 184:
#line 553 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_LT, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6113 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LT */
        break;
      case 185:
#line 554 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_GT, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6119 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for GT */
        break;
      case 186:
#line 555 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_LE, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6125 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LE */
        break;
      case 187:
#line 556 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_GE, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6131 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for GE */
        break;
      case 188:
#line 557 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_NE, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6137 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NE */
        break;
      case 189:
#line 558 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_EQ, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6143 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for EQ */
        break;
      case 190:
#line 559 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_BITAND, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6149 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for BITAND */
        break;
      case 191:
#line 560 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_BITOR, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6155 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for BITOR */
        break;
      case 192:
#line 561 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_LSHIFT, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6161 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LSHIFT */
        break;
      case 193:
#line 562 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_RSHIFT, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6167 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for RSHIFT */
        break;
      case 194:
#line 563 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[0].minor.yy334, 0);
  pList = sqliteExprListAppend(pList, yymsp[-2].minor.yy334, 0);
  yygotominor.yy334 = sqliteExprFunction(pList, 0);
  if( yygotominor.yy334 ) yygotominor.yy334->op = yymsp[-1].minor.yy52;
  sqliteExprSpan(yygotominor.yy334, &yymsp[-2].minor.yy334->span, &yymsp[0].minor.yy334->span);
}
#line 6179 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 195:
#line 570 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[0].minor.yy334, 0);
  pList = sqliteExprListAppend(pList, yymsp[-3].minor.yy334, 0);
  yygotominor.yy334 = sqliteExprFunction(pList, 0);
  if( yygotominor.yy334 ) yygotominor.yy334->op = yymsp[-1].minor.yy52;
  yygotominor.yy334 = sqliteExpr(TK_NOT, yygotominor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-3].minor.yy334->span,&yymsp[0].minor.yy334->span);
}
#line 6191 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        break;
      case 196:
#line 579 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = TK_LIKE;}
#line 6197 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LIKE */
        break;
      case 197:
#line 580 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy52 = TK_GLOB;}
#line 6203 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for GLOB */
        break;
      case 198:
#line 581 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_PLUS, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6209 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PLUS */
        break;
      case 199:
#line 582 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_MINUS, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6215 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for MINUS */
        break;
      case 200:
#line 583 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_STAR, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6221 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for STAR */
        break;
      case 201:
#line 584 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_SLASH, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6227 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for SLASH */
        break;
      case 202:
#line 585 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_REM, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6233 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for REM */
        break;
      case 203:
#line 586 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = sqliteExpr(TK_CONCAT, yymsp[-2].minor.yy334, yymsp[0].minor.yy334, 0);}
#line 6239 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for CONCAT */
        break;
      case 204:
#line 587 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_ISNULL, yymsp[-1].minor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-1].minor.yy334->span,&yymsp[0].minor.yy0);
}
#line 6248 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 205:
#line 591 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_ISNULL, yymsp[-2].minor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-2].minor.yy334->span,&yymsp[0].minor.yy0);
}
#line 6256 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for IS */
        break;
      case 206:
#line 595 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_NOTNULL, yymsp[-1].minor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-1].minor.yy334->span,&yymsp[0].minor.yy0);
}
#line 6265 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 207:
#line 599 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_NOTNULL, yymsp[-2].minor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-2].minor.yy334->span,&yymsp[0].minor.yy0);
}
#line 6273 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        break;
      case 208:
#line 603 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_NOTNULL, yymsp[-3].minor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-3].minor.yy334->span,&yymsp[0].minor.yy0);
}
#line 6282 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for IS */
        /* No destructor defined for NOT */
        break;
      case 209:
#line 607 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_NOT, yymsp[0].minor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy334->span);
}
#line 6292 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 210:
#line 611 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_BITNOT, yymsp[0].minor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy334->span);
}
#line 6300 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 211:
#line 615 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_UMINUS, yymsp[0].minor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy334->span);
}
#line 6308 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 212:
#line 619 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_UPLUS, yymsp[0].minor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy334->span);
}
#line 6316 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 213:
#line 623 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_SELECT, 0, 0, 0);
  if( yygotominor.yy334 ) yygotominor.yy334->pSelect = yymsp[-1].minor.yy11;
  sqliteExprSpan(yygotominor.yy334,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0);
}
#line 6325 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 214:
#line 628 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[-2].minor.yy334, 0);
  pList = sqliteExprListAppend(pList, yymsp[0].minor.yy334, 0);
  yygotominor.yy334 = sqliteExpr(TK_BETWEEN, yymsp[-4].minor.yy334, 0, 0);
  if( yygotominor.yy334 ) yygotominor.yy334->pList = pList;
  sqliteExprSpan(yygotominor.yy334,&yymsp[-4].minor.yy334->span,&yymsp[0].minor.yy334->span);
}
#line 6336 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for BETWEEN */
        /* No destructor defined for AND */
        break;
      case 215:
#line 635 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[-2].minor.yy334, 0);
  pList = sqliteExprListAppend(pList, yymsp[0].minor.yy334, 0);
  yygotominor.yy334 = sqliteExpr(TK_BETWEEN, yymsp[-5].minor.yy334, 0, 0);
  if( yygotominor.yy334 ) yygotominor.yy334->pList = pList;
  yygotominor.yy334 = sqliteExpr(TK_NOT, yygotominor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-5].minor.yy334->span,&yymsp[0].minor.yy334->span);
}
#line 6350 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        /* No destructor defined for BETWEEN */
        /* No destructor defined for AND */
        break;
      case 216:
#line 643 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_IN, yymsp[-4].minor.yy334, 0, 0);
  if( yygotominor.yy334 ) yygotominor.yy334->pList = yymsp[-1].minor.yy62;
  sqliteExprSpan(yygotominor.yy334,&yymsp[-4].minor.yy334->span,&yymsp[0].minor.yy0);
}
#line 6362 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for IN */
        /* No destructor defined for LP */
        break;
      case 217:
#line 648 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_IN, yymsp[-4].minor.yy334, 0, 0);
  if( yygotominor.yy334 ) yygotominor.yy334->pSelect = yymsp[-1].minor.yy11;
  sqliteExprSpan(yygotominor.yy334,&yymsp[-4].minor.yy334->span,&yymsp[0].minor.yy0);
}
#line 6373 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for IN */
        /* No destructor defined for LP */
        break;
      case 218:
#line 653 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_IN, yymsp[-5].minor.yy334, 0, 0);
  if( yygotominor.yy334 ) yygotominor.yy334->pList = yymsp[-1].minor.yy62;
  yygotominor.yy334 = sqliteExpr(TK_NOT, yygotominor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-5].minor.yy334->span,&yymsp[0].minor.yy0);
}
#line 6385 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        /* No destructor defined for IN */
        /* No destructor defined for LP */
        break;
      case 219:
#line 659 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_IN, yymsp[-5].minor.yy334, 0, 0);
  if( yygotominor.yy334 ) yygotominor.yy334->pSelect = yymsp[-1].minor.yy11;
  yygotominor.yy334 = sqliteExpr(TK_NOT, yygotominor.yy334, 0, 0);
  sqliteExprSpan(yygotominor.yy334,&yymsp[-5].minor.yy334->span,&yymsp[0].minor.yy0);
}
#line 6398 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        /* No destructor defined for IN */
        /* No destructor defined for LP */
        break;
      case 220:
#line 667 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_CASE, yymsp[-3].minor.yy334, yymsp[-1].minor.yy334, 0);
  if( yygotominor.yy334 ) yygotominor.yy334->pList = yymsp[-2].minor.yy62;
  sqliteExprSpan(yygotominor.yy334, &yymsp[-4].minor.yy0, &yymsp[0].minor.yy0);
}
#line 6410 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 221:
#line 674 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy62 = sqliteExprListAppend(yymsp[-4].minor.yy62, yymsp[-2].minor.yy334, 0);
  yygotominor.yy62 = sqliteExprListAppend(yygotominor.yy62, yymsp[0].minor.yy334, 0);
}
#line 6418 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for WHEN */
        /* No destructor defined for THEN */
        break;
      case 222:
#line 678 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy62 = sqliteExprListAppend(0, yymsp[-2].minor.yy334, 0);
  yygotominor.yy62 = sqliteExprListAppend(yygotominor.yy62, yymsp[0].minor.yy334, 0);
}
#line 6428 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for WHEN */
        /* No destructor defined for THEN */
        break;
      case 223:
#line 683 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = yymsp[0].minor.yy334;}
#line 6435 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ELSE */
        break;
      case 224:
#line 684 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = 0;}
#line 6441 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 225:
#line 686 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = yymsp[0].minor.yy334;}
#line 6446 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 226:
#line 687 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = 0;}
#line 6451 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 227:
#line 695 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = sqliteExprListAppend(yymsp[-2].minor.yy62,yymsp[0].minor.yy334,0);}
#line 6456 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 228:
#line 696 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy62 = sqliteExprListAppend(0,yymsp[0].minor.yy334,0);}
#line 6462 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 229:
#line 697 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = yymsp[0].minor.yy334;}
#line 6467 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 230:
#line 698 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy334 = 0;}
#line 6472 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 231:
#line 703 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  SrcList *pSrc = sqliteSrcListAppend(0, &yymsp[-5].minor.yy210, &yymsp[-4].minor.yy210);
  if( yymsp[-9].minor.yy52!=OE_None ) yymsp[-9].minor.yy52 = yymsp[0].minor.yy52;
  if( yymsp[-9].minor.yy52==OE_Default) yymsp[-9].minor.yy52 = OE_Abort;
  sqliteCreateIndex(pParse, &yymsp[-7].minor.yy210, pSrc, yymsp[-2].minor.yy92, yymsp[-9].minor.yy52, yymsp[-10].minor.yy52, &yymsp[-11].minor.yy0, &yymsp[-1].minor.yy0);
}
#line 6482 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INDEX */
        /* No destructor defined for ON */
        /* No destructor defined for LP */
        break;
      case 232:
#line 711 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_Abort; }
#line 6490 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UNIQUE */
        break;
      case 233:
#line 712 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = OE_None; }
#line 6496 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 234:
#line 720 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy92 = 0;}
#line 6501 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 235:
#line 721 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy92 = yymsp[-1].minor.yy92;}
#line 6506 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 236:
#line 722 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy92 = sqliteIdListAppend(yymsp[-2].minor.yy92,&yymsp[0].minor.yy210);}
#line 6513 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 237:
#line 723 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy92 = sqliteIdListAppend(0,&yymsp[0].minor.yy210);}
#line 6519 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 238:
#line 724 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[-1].minor.yy210;}
#line 6524 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for sortorder */
        break;
      case 239:
#line 729 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDropIndex(pParse, sqliteSrcListAppend(0,&yymsp[-1].minor.yy210,&yymsp[0].minor.yy210));
}
#line 6532 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DROP */
        /* No destructor defined for INDEX */
        break;
      case 240:
#line 737 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteCopy(pParse,sqliteSrcListAppend(0,&yymsp[-6].minor.yy210,&yymsp[-5].minor.yy210),&yymsp[-3].minor.yy210,&yymsp[0].minor.yy0,yymsp[-7].minor.yy52);}
#line 6539 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COPY */
        /* No destructor defined for FROM */
        /* No destructor defined for USING */
        /* No destructor defined for DELIMITERS */
        break;
      case 241:
#line 739 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteCopy(pParse,sqliteSrcListAppend(0,&yymsp[-3].minor.yy210,&yymsp[-2].minor.yy210),&yymsp[0].minor.yy210,0,yymsp[-4].minor.yy52);}
#line 6548 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COPY */
        /* No destructor defined for FROM */
        break;
      case 242:
#line 743 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteVacuum(pParse,0);}
#line 6555 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for VACUUM */
        break;
      case 243:
#line 744 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqliteVacuum(pParse,&yymsp[0].minor.yy210);}
#line 6561 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for VACUUM */
        break;
      case 244:
#line 748 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy210,&yymsp[0].minor.yy210,0);}
#line 6567 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        /* No destructor defined for EQ */
        break;
      case 245:
#line 749 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy210,&yymsp[0].minor.yy0,0);}
#line 6574 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        /* No destructor defined for EQ */
        break;
      case 246:
#line 750 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy210,&yymsp[0].minor.yy210,0);}
#line 6581 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        /* No destructor defined for EQ */
        break;
      case 247:
#line 751 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy210,&yymsp[0].minor.yy210,1);}
#line 6588 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        /* No destructor defined for EQ */
        break;
      case 248:
#line 752 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-3].minor.yy210,&yymsp[-1].minor.yy210,0);}
#line 6595 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 249:
#line 753 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[0].minor.yy210,&yymsp[0].minor.yy210,0);}
#line 6603 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        break;
      case 250:
#line 754 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy210;}
#line 6609 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for plus_opt */
        break;
      case 251:
#line 755 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy210;}
#line 6615 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for MINUS */
        break;
      case 252:
#line 756 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy0;}
#line 6621 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 253:
#line 757 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy210 = yymsp[0].minor.yy0;}
#line 6626 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 254:
        /* No destructor defined for PLUS */
        break;
      case 255:
        break;
      case 256:
#line 763 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  Token all;
  all.z = yymsp[-4].minor.yy0.z;
  all.n = (yymsp[0].minor.yy0.z - yymsp[-4].minor.yy0.z) + yymsp[0].minor.yy0.n;
  sqliteFinishTrigger(pParse, yymsp[-1].minor.yy347, &all);
}
#line 6641 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for trigger_decl */
        /* No destructor defined for BEGIN */
        break;
      case 257:
#line 771 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  SrcList *pTab = sqliteSrcListAppend(0, &yymsp[-3].minor.yy210, &yymsp[-2].minor.yy210);
  sqliteBeginTrigger(pParse, &yymsp[-7].minor.yy210, yymsp[-6].minor.yy52, yymsp[-5].minor.yy234.a, yymsp[-5].minor.yy234.b, pTab, yymsp[-1].minor.yy52, yymsp[0].minor.yy270, yymsp[-9].minor.yy52);
}
#line 6651 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for TRIGGER */
        /* No destructor defined for ON */
        break;
      case 258:
#line 777 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = TK_BEFORE; }
#line 6658 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for BEFORE */
        break;
      case 259:
#line 778 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = TK_AFTER;  }
#line 6664 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for AFTER */
        break;
      case 260:
#line 779 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = TK_INSTEAD;}
#line 6670 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INSTEAD */
        /* No destructor defined for OF */
        break;
      case 261:
#line 780 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = TK_BEFORE; }
#line 6677 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 262:
#line 784 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy234.a = TK_DELETE; yygotominor.yy234.b = 0; }
#line 6682 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DELETE */
        break;
      case 263:
#line 785 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy234.a = TK_INSERT; yygotominor.yy234.b = 0; }
#line 6688 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INSERT */
        break;
      case 264:
#line 786 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy234.a = TK_UPDATE; yygotominor.yy234.b = 0;}
#line 6694 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UPDATE */
        break;
      case 265:
#line 787 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy234.a = TK_UPDATE; yygotominor.yy234.b = yymsp[0].minor.yy92; }
#line 6700 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UPDATE */
        /* No destructor defined for OF */
        break;
      case 266:
#line 790 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = TK_ROW; }
#line 6707 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 267:
#line 791 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = TK_ROW; }
#line 6712 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for FOR */
        /* No destructor defined for EACH */
        /* No destructor defined for ROW */
        break;
      case 268:
#line 792 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy52 = TK_STATEMENT; }
#line 6720 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for FOR */
        /* No destructor defined for EACH */
        /* No destructor defined for STATEMENT */
        break;
      case 269:
#line 795 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy270 = 0; }
#line 6728 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 270:
#line 796 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy270 = yymsp[0].minor.yy334; }
#line 6733 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for WHEN */
        break;
      case 271:
#line 800 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yymsp[-2].minor.yy347->pNext = yymsp[0].minor.yy347;
  yygotominor.yy347 = yymsp[-2].minor.yy347;
}
#line 6742 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for SEMI */
        break;
      case 272:
#line 804 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy347 = 0; }
#line 6748 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 273:
#line 810 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy347 = sqliteTriggerUpdateStep(&yymsp[-3].minor.yy210, yymsp[-1].minor.yy62, yymsp[0].minor.yy334, yymsp[-4].minor.yy52); }
#line 6753 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UPDATE */
        /* No destructor defined for SET */
        break;
      case 274:
#line 815 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy347 = sqliteTriggerInsertStep(&yymsp[-5].minor.yy210, yymsp[-4].minor.yy92, yymsp[-1].minor.yy62, 0, yymsp[-7].minor.yy52);}
#line 6760 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INSERT */
        /* No destructor defined for INTO */
        /* No destructor defined for VALUES */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 275:
#line 818 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy347 = sqliteTriggerInsertStep(&yymsp[-2].minor.yy210, yymsp[-1].minor.yy92, 0, yymsp[0].minor.yy11, yymsp[-4].minor.yy52);}
#line 6770 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INSERT */
        /* No destructor defined for INTO */
        break;
      case 276:
#line 822 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy347 = sqliteTriggerDeleteStep(&yymsp[-1].minor.yy210, yymsp[0].minor.yy334);}
#line 6777 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DELETE */
        /* No destructor defined for FROM */
        break;
      case 277:
#line 825 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{yygotominor.yy347 = sqliteTriggerSelectStep(yymsp[0].minor.yy11); }
#line 6784 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        break;
      case 278:
#line 828 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_RAISE, 0, 0, 0); 
  yygotominor.yy334->iColumn = OE_Ignore;
  sqliteExprSpan(yygotominor.yy334, &yymsp[-3].minor.yy0, &yymsp[0].minor.yy0);
}
#line 6793 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for IGNORE */
        break;
      case 279:
#line 833 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy210); 
  yygotominor.yy334->iColumn = OE_Rollback;
  sqliteExprSpan(yygotominor.yy334, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 6804 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for ROLLBACK */
        /* No destructor defined for COMMA */
        break;
      case 280:
#line 838 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy210); 
  yygotominor.yy334->iColumn = OE_Abort;
  sqliteExprSpan(yygotominor.yy334, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 6816 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for ABORT */
        /* No destructor defined for COMMA */
        break;
      case 281:
#line 843 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy334 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy210); 
  yygotominor.yy334->iColumn = OE_Fail;
  sqliteExprSpan(yygotominor.yy334, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 6828 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for FAIL */
        /* No destructor defined for COMMA */
        break;
      case 282:
#line 850 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDropTrigger(pParse,sqliteSrcListAppend(0,&yymsp[-1].minor.yy210,&yymsp[0].minor.yy210));
}
#line 6838 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DROP */
        /* No destructor defined for TRIGGER */
        break;
      case 283:
#line 855 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteAttach(pParse, &yymsp[-2].minor.yy210, &yymsp[0].minor.yy210);
}
#line 6847 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ATTACH */
        /* No destructor defined for database_kw_opt */
        /* No destructor defined for AS */
        break;
      case 284:
        /* No destructor defined for DATABASE */
        break;
      case 285:
        break;
      case 286:
#line 863 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"
{
  sqliteDetach(pParse, &yymsp[0].minor.yy210);
}
#line 6862 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DETACH */
        /* No destructor defined for database_kw_opt */
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yypParser->yytop -= yysize;
  yyact = yy_find_parser_action(yypParser,yygoto);
  if( yyact < YYNSTATE ){
    yy_shift(yypParser,yyact,yygoto,&yygotominor);
  }else if( yyact == YYNSTATE + YYNRULE + 1 ){
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
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
#line 23 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.y"

  if( pParse->zErrMsg==0 ){
    if( TOKEN.z[0] ){
      sqliteSetNString(&pParse->zErrMsg, 
          "near \"", -1, TOKEN.z, TOKEN.n, "\": syntax error", -1, 0);
    }else{
      sqliteSetString(&pParse->zErrMsg, "incomplete SQL statement", 0);
    }
  }
  pParse->nErr++;

#line 6919 "/home/wez/src/php/php4.3-cvs/ext/sqlite/libsqlite/src/parse.c"
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
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
    if( yymajor==0 ) return;
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yytop = &yypParser->yystack[0];
    yypParser->yytop->stateno = 0;
    yypParser->yytop->major = 0;
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
    yyact = yy_find_parser_action(yypParser,yymajor);
    if( yyact<YYNSTATE ){
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      if( yyendofinput && yypParser->yyidx>=0 ){
        yymajor = 0;
      }else{
        yymajor = YYNOCODE;
      }
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else if( yyact == YY_ERROR_ACTION ){
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
      if( yypParser->yytop->major==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yypParser->yytop->major != YYERRORSYMBOL &&
          (yyact = yy_find_parser_action(yypParser,YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yypParser->yytop->major!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
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
      yy_destructor(yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }else{
      yy_accept(yypParser);
      yymajor = YYNOCODE;
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
