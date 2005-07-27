/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
#line 35 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"

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


#line 34 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
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
#define YYNOCODE 214
#define YYACTIONTYPE unsigned short int
#define sqliteParserTOKENTYPE Token
typedef union {
  sqliteParserTOKENTYPE yy0;
  struct TrigEvent yy72;
  struct {int value; int mask;} yy83;
  int yy136;
  ExprList* yy168;
  Expr * yy176;
  Select* yy207;
  TriggerStep * yy209;
  IdList* yy268;
  Expr* yy272;
  SrcList* yy289;
  Token yy324;
  struct LimitVal yy336;
  int yy427;
} YYMINORTYPE;
#define YYSTACKDEPTH 100
#define sqliteParserARG_SDECL Parse *pParse;
#define sqliteParserARG_PDECL ,Parse *pParse
#define sqliteParserARG_FETCH Parse *pParse = yypParser->pParse
#define sqliteParserARG_STORE yypParser->pParse = pParse
#define YYNSTATE 531
#define YYNRULE 280
#define YYERRORSYMBOL 150
#define YYERRSYMDT yy427
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
  {  44,   0, 529}, /*  1:                EXPLAIN shift  529 */
  { 151,   0,   3}, /*  2:                explain shift  3 */
  { 104,   1, 528}, /*  3:                   SEMI shift  528 */
  { 165,   0, 812}, /*  4:                  input accept */
  { 136,   0,   1}, /*  5:                cmdlist shift  1 */
  { 149,   0, 530}, /*  6:                   ecmd shift  530 */
/* State 1 */
  {   0,   0, 531}, /*  1:                      $ reduce 0 */
  { 151,   0,   3}, /*  2:                explain shift  3 */
  { 104,   4, 528}, /*  3:                   SEMI shift  528 */
  {  44,   0, 529}, /*  4:                EXPLAIN shift  529 */
  { 149,   3,   2}, /*  5:                   ecmd shift  2 */
/* State 3 */
  { 120,   3, 483}, /*  1:                 UPDATE shift  483 */
  { 181,   0,  69}, /*  2:              oneselect shift  69 */
  {  40,   0,  25}, /*  3:                    END shift  25 */
  { 123,   6, 509}, /*  4:                 VACUUM shift  509 */
  { 144,   0,  29}, /*  5:           create_table shift  29 */
  { 103,   7,  73}, /*  6:                 SELECT shift  73 */
  {  63,  11, 498}, /*  7:                 INSERT shift  498 */
  {  27,   0, 501}, /*  8:                   COPY shift  501 */
  { 168,  12, 489}, /*  9:             insert_cmd shift  489 */
  {   9,   0,   7}, /* 10:                  BEGIN shift  7 */
  {  23,   0,  23}, /* 11:                 COMMIT shift  23 */
  {  28,   0, 382}, /* 12:                 CREATE shift  382 */
  { 192,  14, 478}, /* 13:                 select shift  478 */
  {  92,  15, 511}, /* 14:                 PRAGMA shift  511 */
  {  32,   0, 479}, /* 15:                 DELETE shift  479 */
  { 135,   0,   6}, /* 16:                    cmd shift  6 */
  {  97,  19, 500}, /* 17:                REPLACE shift  500 */
  { 137,  17,   4}, /* 18:                   cmdx shift  4 */
  {  37,   0, 469}, /* 19:                   DROP shift  469 */
  {  99,   0,  27}, /* 20:               ROLLBACK shift  27 */
/* State 4 */
  { 104,   0,   5}, /*  1:                   SEMI shift  5 */
/* State 6 */
  { 104,   0, 536}, /*  1:                   SEMI reduce 5 */
/* State 7 */
  { 202,   2,   8}, /*  1:              trans_opt shift  8 */
  { 114,   0,  18}, /*  2:            TRANSACTION shift  18 */
/* State 8 */
  { 180,   2,   9}, /*  1:                 onconf shift  9 */
  {  87,   0,  10}, /*  2:                     ON shift  10 */
  { 104,   0, 619}, /*  3:                   SEMI reduce 88 */
/* State 9 */
  { 104,   0, 539}, /*  1:                   SEMI reduce 8 */
/* State 10 */
  {  25,   0,  11}, /*  1:               CONFLICT shift  11 */
/* State 11 */
  {   1,   0,  14}, /*  1:                  ABORT shift  14 */
  {  97,   1,  17}, /*  2:                REPLACE shift  17 */
  {  99,   5,  13}, /*  3:               ROLLBACK shift  13 */
  { 189,   3,  12}, /*  4:            resolvetype shift  12 */
  {  57,   6,  16}, /*  5:                 IGNORE shift  16 */
  {  45,   0,  15}, /*  6:                   FAIL shift  15 */
/* State 18 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0,  19}, /*  2:                     nm shift  19 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 23 */
  { 114,   0,  18}, /*  1:            TRANSACTION shift  18 */
  { 202,   0,  24}, /*  2:              trans_opt shift  24 */
  { 104,   0, 540}, /*  3:                   SEMI reduce 9 */
/* State 24 */
  { 104,   0, 543}, /*  1:                   SEMI reduce 12 */
/* State 25 */
  { 114,   0,  18}, /*  1:            TRANSACTION shift  18 */
  { 202,   0,  26}, /*  2:              trans_opt shift  26 */
  { 104,   0, 540}, /*  3:                   SEMI reduce 9 */
/* State 26 */
  { 104,   0, 544}, /*  1:                   SEMI reduce 13 */
/* State 27 */
  { 114,   0,  18}, /*  1:            TRANSACTION shift  18 */
  { 202,   0,  28}, /*  2:              trans_opt shift  28 */
  { 104,   0, 540}, /*  3:                   SEMI reduce 9 */
/* State 28 */
  { 104,   0, 545}, /*  1:                   SEMI reduce 14 */
/* State 29 */
  {   6,   0, 380}, /*  1:                     AS shift  380 */
  { 145,   3,  30}, /*  2:      create_table_args shift  30 */
  {  76,   0,  31}, /*  3:                     LP shift  31 */
/* State 30 */
  { 104,   0, 546}, /*  1:                   SEMI reduce 15 */
/* State 31 */
  { 140,   4,  37}, /*  1:               columnid shift  37 */
  { 141,   5,  32}, /*  2:             columnlist shift  32 */
  { 177,   0, 345}, /*  3:                     nm shift  345 */
  {  56,   0,  20}, /*  4:                     ID shift  20 */
  {  71,   0,  22}, /*  5:                JOIN_KW shift  22 */
  { 110,   0,  21}, /*  6:                 STRING shift  21 */
  { 139,   0, 379}, /*  7:                 column shift  379 */
/* State 32 */
  {  21,   0,  35}, /*  1:                  COMMA shift  35 */
  { 101,   0, 607}, /*  2:                     RP reduce 76 */
  { 143,   2,  33}, /*  3:           conslist_opt shift  33 */
/* State 33 */
  { 101,   0,  34}, /*  1:                     RP shift  34 */
/* State 34 */
  { 104,   0, 550}, /*  1:                   SEMI reduce 19 */
/* State 35 */
  {  26,   0, 349}, /*  1:             CONSTRAINT shift  349 */
  {  93,   0, 351}, /*  2:                PRIMARY shift  351 */
  { 119,   2, 357}, /*  3:                 UNIQUE shift  357 */
  {  17,   0, 362}, /*  4:                  CHECK shift  362 */
  {  56,   4,  20}, /*  5:                     ID shift  20 */
  { 200,   0, 378}, /*  6:                  tcons shift  378 */
  { 110,   8,  21}, /*  7:                 STRING shift  21 */
  {  71,   0,  22}, /*  8:                JOIN_KW shift  22 */
  { 177,   0, 345}, /*  9:                     nm shift  345 */
  { 139,  12,  36}, /* 10:                 column shift  36 */
  { 140,   0,  37}, /* 11:               columnid shift  37 */
  {  48,   0, 365}, /* 12:                FOREIGN shift  365 */
  { 142,   0, 346}, /* 13:               conslist shift  346 */
/* State 37 */
  { 160,   5, 344}, /*  1:                    ids shift  344 */
  {  56,   0, 248}, /*  2:                     ID shift  248 */
  { 207,   0,  38}, /*  3:                   type shift  38 */
  { 208,   0, 331}, /*  4:               typename shift  331 */
  { 110,   0, 249}, /*  5:                 STRING shift  249 */
/* State 38 */
  { 130,   0,  39}, /*  1:               carglist shift  39 */
/* State 39 */
  {  26,   0,  41}, /*  1:             CONSTRAINT shift  41 */
  {  93,   0,  54}, /*  2:                PRIMARY shift  54 */
  { 119,   2,  60}, /*  3:                 UNIQUE shift  60 */
  { 146,   6, 313}, /*  4:        defer_subclause shift  313 */
  { 134,   8, 318}, /*  5:                  ccons shift  318 */
  {  29,   0, 319}, /*  6:                DEFAULT shift  319 */
  {  84,   9,  44}, /*  7:                   NULL shift  44 */
  {  95,  10, 291}, /*  8:             REFERENCES shift  291 */
  {  19,   0, 314}, /*  9:                COLLATE shift  314 */
  {  82,  11,  46}, /* 10:                    NOT shift  46 */
  {  30,  12, 316}, /* 11:             DEFERRABLE shift  316 */
  {  17,   0,  62}, /* 12:                  CHECK shift  62 */
  { 129,   0,  40}, /* 13:                   carg shift  40 */
/* State 41 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0,  42}, /*  2:                     nm shift  42 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 42 */
  {  30,   0, 316}, /*  1:             DEFERRABLE shift  316 */
  {  84,   0,  44}, /*  2:                   NULL shift  44 */
  {  82,   0,  46}, /*  3:                    NOT shift  46 */
  {  93,   0,  54}, /*  4:                PRIMARY shift  54 */
  { 134,   2,  43}, /*  5:                  ccons shift  43 */
  {  95,   0, 291}, /*  6:             REFERENCES shift  291 */
  { 146,   0, 313}, /*  7:        defer_subclause shift  313 */
  {  17,   0,  62}, /*  8:                  CHECK shift  62 */
  {  19,   0, 314}, /*  9:                COLLATE shift  314 */
  { 119,   9,  60}, /* 10:                 UNIQUE shift  60 */
/* State 44 */
  { 180,   0,  45}, /*  1:                 onconf shift  45 */
  {  87,   0,  10}, /*  2:                     ON shift  10 */
/* State 46 */
  {  84,   2,  47}, /*  1:                   NULL shift  47 */
  {  30,   0,  49}, /*  2:             DEFERRABLE shift  49 */
/* State 47 */
  { 180,   0,  48}, /*  1:                 onconf shift  48 */
  {  87,   0,  10}, /*  2:                     ON shift  10 */
/* State 49 */
  { 164,   2,  50}, /*  1: init_deferred_pred_opt shift  50 */
  {  62,   0,  51}, /*  2:              INITIALLY shift  51 */
/* State 51 */
  {  31,   0,  52}, /*  1:               DEFERRED shift  52 */
  {  59,   1,  53}, /*  2:              IMMEDIATE shift  53 */
/* State 54 */
  {  72,   0,  55}, /*  1:                    KEY shift  55 */
/* State 55 */
  { 198,   0,  56}, /*  1:              sortorder shift  56 */
  {  34,   3,  59}, /*  2:                   DESC shift  59 */
  {   7,   0,  58}, /*  3:                    ASC shift  58 */
/* State 56 */
  { 180,   0,  57}, /*  1:                 onconf shift  57 */
  {  87,   0,  10}, /*  2:                     ON shift  10 */
/* State 60 */
  { 180,   0,  61}, /*  1:                 onconf shift  61 */
  {  87,   0,  10}, /*  2:                     ON shift  10 */
/* State 62 */
  {  76,   0,  63}, /*  1:                     LP shift  63 */
/* State 63 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 288}, /*  3:                   expr shift  288 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 64 */
  {  76,   2,  65}, /*  1:                     LP shift  65 */
  {  36,   0, 559}, /*  2:                    DOT reduce 28 */
/* State 65 */
  { 108,   0, 286}, /*  1:                   STAR shift  286 */
  {  91,   0, 174}, /*  2:                   PLUS shift  174 */
  { 110,   4,  66}, /*  3:                 STRING shift  66 */
  {  56,   0,  64}, /*  4:                     ID shift  64 */
  {  94,   6, 186}, /*  5:                  RAISE shift  186 */
  {  76,   0,  68}, /*  6:                     LP shift  68 */
  {  80,   0, 172}, /*  7:                  MINUS shift  172 */
  {  82,  14, 168}, /*  8:                    NOT shift  168 */
  { 152,   7, 165}, /*  9:                   expr shift  165 */
  { 153,   0, 212}, /* 10:               expritem shift  212 */
  { 154,   8, 284}, /* 11:               exprlist shift  284 */
  {  65,   0, 166}, /* 12:                INTEGER shift  166 */
  {  84,  15, 101}, /* 13:                   NULL shift  101 */
  {  46,   0, 167}, /* 14:                  FLOAT shift  167 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
  { 177,   0, 102}, /* 16:                     nm shift  102 */
  {  16,   0, 176}, /* 17:                   CASE shift  176 */
  {  71,   0,  67}, /* 18:                JOIN_KW shift  67 */
/* State 66 */
  {  36,   0, 560}, /*  1:                    DOT reduce 29 */
/* State 67 */
  {  36,   0, 561}, /*  1:                    DOT reduce 30 */
/* State 68 */
  {  91,   0, 174}, /*  1:                   PLUS shift  174 */
  { 181,   1,  69}, /*  2:              oneselect shift  69 */
  { 110,   4,  66}, /*  3:                 STRING shift  66 */
  {  56,   0,  64}, /*  4:                     ID shift  64 */
  {  94,   6, 186}, /*  5:                  RAISE shift  186 */
  {  76,   0,  68}, /*  6:                     LP shift  68 */
  {  80,   0, 172}, /*  7:                  MINUS shift  172 */
  {  46,   0, 167}, /*  8:                  FLOAT shift  167 */
  { 152,   7, 282}, /*  9:                   expr shift  282 */
  {  84,  15, 101}, /* 10:                   NULL shift  101 */
  {  82,   8, 168}, /* 11:                    NOT shift  168 */
  {  65,   0, 166}, /* 12:                INTEGER shift  166 */
  { 192,  10,  70}, /* 13:                 select shift  70 */
  { 103,   0,  73}, /* 14:                 SELECT shift  73 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
  { 177,   0, 102}, /* 16:                     nm shift  102 */
  {  16,   0, 176}, /* 17:                   CASE shift  176 */
  {  71,   0,  67}, /* 18:                JOIN_KW shift  67 */
/* State 70 */
  { 101,   3, 281}, /*  1:                     RP shift  281 */
  { 176,   1,  71}, /*  2:         multiselect_op shift  71 */
  {  66,   0, 162}, /*  3:              INTERSECT shift  162 */
  { 118,   5, 160}, /*  4:                  UNION shift  160 */
  {  43,   0, 163}, /*  5:                 EXCEPT shift  163 */
/* State 71 */
  { 103,   0,  73}, /*  1:                 SELECT shift  73 */
  { 181,   1,  72}, /*  2:              oneselect shift  72 */
/* State 73 */
  {   4,   0, 280}, /*  1:                    ALL shift  280 */
  { 148,   1,  74}, /*  2:               distinct shift  74 */
  {  35,   0, 279}, /*  3:               DISTINCT shift  279 */
/* State 74 */
  { 190,   0, 272}, /*  1:                   sclp shift  272 */
  { 191,   0,  75}, /*  2:             selcollist shift  75 */
/* State 75 */
  { 156,   3,  76}, /*  1:                   from shift  76 */
  {  49,   0, 235}, /*  2:                   FROM shift  235 */
  {  21,   0, 234}, /*  3:                  COMMA shift  234 */
/* State 76 */
  { 212,   0,  77}, /*  1:              where_opt shift  77 */
  { 127,   0, 232}, /*  2:                  WHERE shift  232 */
/* State 77 */
  {  53,   0, 229}, /*  1:                  GROUP shift  229 */
  { 157,   1,  78}, /*  2:            groupby_opt shift  78 */
/* State 78 */
  { 158,   0,  79}, /*  1:             having_opt shift  79 */
  {  55,   0, 227}, /*  2:                 HAVING shift  227 */
/* State 79 */
  {  90,   0,  88}, /*  1:                  ORDER shift  88 */
  { 183,   0,  80}, /*  2:            orderby_opt shift  80 */
/* State 80 */
  {  75,   0,  82}, /*  1:                  LIMIT shift  82 */
  { 173,   1,  81}, /*  2:              limit_opt shift  81 */
/* State 82 */
  {  65,   0,  83}, /*  1:                INTEGER shift  83 */
/* State 83 */
  { 174,   2,  84}, /*  1:              limit_sep shift  84 */
  {  21,   0,  87}, /*  2:                  COMMA shift  87 */
  {  86,   0,  86}, /*  3:                 OFFSET shift  86 */
/* State 84 */
  {  65,   0,  85}, /*  1:                INTEGER shift  85 */
/* State 86 */
  {  65,   0, 682}, /*  1:                INTEGER reduce 151 */
/* State 87 */
  {  65,   0, 683}, /*  1:                INTEGER reduce 152 */
/* State 88 */
  {  14,   0,  89}, /*  1:                     BY shift  89 */
/* State 89 */
  {  76,   0,  68}, /*  1:                     LP shift  68 */
  {  94,   0, 186}, /*  2:                  RAISE shift  186 */
  {  46,   5, 167}, /*  3:                  FLOAT shift  167 */
  {  71,   0,  67}, /*  4:                JOIN_KW shift  67 */
  {  12,   0, 170}, /*  5:                 BITNOT shift  170 */
  {  56,   0,  64}, /*  6:                     ID shift  64 */
  {  91,   0, 174}, /*  7:                   PLUS shift  174 */
  { 177,   0, 102}, /*  8:                     nm shift  102 */
  { 110,   1,  66}, /*  9:                 STRING shift  66 */
  { 196,   2, 224}, /* 10:               sortitem shift  224 */
  { 197,   0,  90}, /* 11:               sortlist shift  90 */
  {  65,   0, 166}, /* 12:                INTEGER shift  166 */
  {  80,   3, 172}, /* 13:                  MINUS shift  172 */
  {  84,  16, 101}, /* 14:                   NULL shift  101 */
  {  82,  12, 168}, /* 15:                    NOT shift  168 */
  {  16,   0, 176}, /* 16:                   CASE shift  176 */
  { 152,  14,  98}, /* 17:                   expr shift  98 */
/* State 90 */
  {  21,   0,  91}, /*  1:                  COMMA shift  91 */
/* State 91 */
  {  80,   4, 172}, /*  1:                  MINUS shift  172 */
  { 177,   6, 102}, /*  2:                     nm shift  102 */
  {  82,   0, 168}, /*  3:                    NOT shift  168 */
  {  16,   0, 176}, /*  4:                   CASE shift  176 */
  { 196,   7,  92}, /*  5:               sortitem shift  92 */
  {  65,   0, 166}, /*  6:                INTEGER shift  166 */
  {  84,   0, 101}, /*  7:                   NULL shift  101 */
  {  71,   0,  67}, /*  8:                JOIN_KW shift  67 */
  { 152,  10,  98}, /*  9:                   expr shift  98 */
  {  56,   0,  64}, /* 10:                     ID shift  64 */
  {  12,   0, 170}, /* 11:                 BITNOT shift  170 */
  {  91,   0, 174}, /* 12:                   PLUS shift  174 */
  {  76,  11,  68}, /* 13:                     LP shift  68 */
  {  94,  16, 186}, /* 14:                  RAISE shift  186 */
  { 110,  14,  66}, /* 15:                 STRING shift  66 */
  {  46,   0, 167}, /* 16:                  FLOAT shift  167 */
/* State 92 */
  { 138,   0,  93}, /*  1:                collate shift  93 */
  {  19,   0,  95}, /*  2:                COLLATE shift  95 */
/* State 93 */
  { 198,   0,  94}, /*  1:              sortorder shift  94 */
  {  34,   3,  59}, /*  2:                   DESC shift  59 */
  {   7,   0,  58}, /*  3:                    ASC shift  58 */
/* State 95 */
  {  56,   0,  96}, /*  1:                     ID shift  96 */
  { 159,   0,  97}, /*  2:                     id shift  97 */
/* State 98 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 100 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 105}, /*  3:                   expr shift  105 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 102 */
  {  36,   0, 103}, /*  1:                    DOT shift  103 */
/* State 103 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 104}, /*  2:                     nm shift  104 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 105 */
  {  78,   2, 108}, /*  1:                     LT shift  108 */
  {  52,   0, 134}, /*  2:                   GLOB shift  134 */
  { 106,   7, 141}, /*  3:                  SLASH shift  141 */
  {  81,   0, 116}, /*  4:                     NE shift  116 */
  { 108,   8, 139}, /*  5:                   STAR shift  139 */
  {  83,   0, 152}, /*  6:                NOTNULL shift  152 */
  {  80,  10, 137}, /*  7:                  MINUS shift  137 */
  {  82,   0, 130}, /*  8:                    NOT shift  130 */
  {  60,   0, 157}, /*  9:                     IN shift  157 */
  {  54,   0, 110}, /* 10:                     GT shift  110 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  89,  13,  99}, /* 12:      ORACLE_OUTER_JOIN shift  99 */
  {  11,   0, 120}, /* 13:                 BITAND shift  120 */
  {  91,  15, 135}, /* 14:                   PLUS shift  135 */
  {  13,   0, 122}, /* 15:                  BITOR shift  122 */
  {  68,  20, 148}, /* 16:                     IS shift  148 */
  { 172,  16, 128}, /* 17:                 likeop shift  128 */
  {  69,   0, 147}, /* 18:                 ISNULL shift  147 */
  {  96,   0, 143}, /* 19:                    REM shift  143 */
  {  42,   0, 118}, /* 20:                     EQ shift  118 */
  {  24,   0, 145}, /* 21:                 CONCAT shift  145 */
  {  73,   0, 112}, /* 22:                     LE shift  112 */
  {  74,   0, 133}, /* 23:                   LIKE shift  133 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 102,  21, 126}, /* 25:                 RSHIFT shift  126 */
  {  77,  24, 124}, /* 26:                 LSHIFT shift  124 */
/* State 106 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 107}, /*  3:                   expr shift  107 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 107 */
  { 108,   4, 139}, /*  1:                   STAR shift  139 */
  {  82,   0, 130}, /*  2:                    NOT shift  130 */
  {  83,   0, 152}, /*  3:                NOTNULL shift  152 */
  {  81,   5, 116}, /*  4:                     NE shift  116 */
  {  54,   0, 110}, /*  5:                     GT shift  110 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  60,   0, 157}, /*  7:                     IN shift  157 */
  {  91,  10, 135}, /*  8:                   PLUS shift  135 */
  {  89,   0,  99}, /*  9:      ORACLE_OUTER_JOIN shift  99 */
  {  10,   0, 153}, /* 10:                BETWEEN shift  153 */
  { 172,   8, 128}, /* 11:                 likeop shift  128 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  69,  17, 147}, /* 13:                 ISNULL shift  147 */
  {  13,   0, 122}, /* 14:                  BITOR shift  122 */
  {  68,   0, 148}, /* 15:                     IS shift  148 */
  {  96,  13, 143}, /* 16:                    REM shift  143 */
  {  42,   0, 118}, /* 17:                     EQ shift  118 */
  {  51,  19, 114}, /* 18:                     GE shift  114 */
  {  24,   0, 145}, /* 19:                 CONCAT shift  145 */
  {  73,   0, 112}, /* 20:                     LE shift  112 */
  {  74,   0, 133}, /* 21:                   LIKE shift  133 */
  { 102,   0, 126}, /* 22:                 RSHIFT shift  126 */
  {  52,   0, 134}, /* 23:                   GLOB shift  134 */
  {  77,   0, 124}, /* 24:                 LSHIFT shift  124 */
  {  78,  18, 108}, /* 25:                     LT shift  108 */
  { 106,  23, 141}, /* 26:                  SLASH shift  141 */
  {  80,   0, 137}, /* 27:                  MINUS shift  137 */
/* State 108 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 109}, /*  3:                   expr shift  109 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 109 */
  { 108,   3, 139}, /*  1:                   STAR shift  139 */
  {  13,   0, 122}, /*  2:                  BITOR shift  122 */
  {  96,   4, 143}, /*  3:                    REM shift  143 */
  {  24,   0, 145}, /*  4:                 CONCAT shift  145 */
  { 172,   0, 128}, /*  5:                 likeop shift  128 */
  {  89,  10,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  { 102,   0, 126}, /*  7:                 RSHIFT shift  126 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  80,   0, 137}, /*  9:                  MINUS shift  137 */
  {  77,   0, 124}, /* 10:                 LSHIFT shift  124 */
  { 106,   0, 141}, /* 11:                  SLASH shift  141 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
/* State 110 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 111}, /*  3:                   expr shift  111 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 111 */
  { 108,   3, 139}, /*  1:                   STAR shift  139 */
  {  13,   0, 122}, /*  2:                  BITOR shift  122 */
  {  96,   4, 143}, /*  3:                    REM shift  143 */
  {  24,   0, 145}, /*  4:                 CONCAT shift  145 */
  { 172,   0, 128}, /*  5:                 likeop shift  128 */
  {  89,  10,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  { 102,   0, 126}, /*  7:                 RSHIFT shift  126 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  80,   0, 137}, /*  9:                  MINUS shift  137 */
  {  77,   0, 124}, /* 10:                 LSHIFT shift  124 */
  { 106,   0, 141}, /* 11:                  SLASH shift  141 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
/* State 112 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 113}, /*  3:                   expr shift  113 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 113 */
  { 108,   3, 139}, /*  1:                   STAR shift  139 */
  {  13,   0, 122}, /*  2:                  BITOR shift  122 */
  {  96,   4, 143}, /*  3:                    REM shift  143 */
  {  24,   0, 145}, /*  4:                 CONCAT shift  145 */
  { 172,   0, 128}, /*  5:                 likeop shift  128 */
  {  89,  10,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  { 102,   0, 126}, /*  7:                 RSHIFT shift  126 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  80,   0, 137}, /*  9:                  MINUS shift  137 */
  {  77,   0, 124}, /* 10:                 LSHIFT shift  124 */
  { 106,   0, 141}, /* 11:                  SLASH shift  141 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
/* State 114 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 115}, /*  3:                   expr shift  115 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 115 */
  { 108,   3, 139}, /*  1:                   STAR shift  139 */
  {  13,   0, 122}, /*  2:                  BITOR shift  122 */
  {  96,   4, 143}, /*  3:                    REM shift  143 */
  {  24,   0, 145}, /*  4:                 CONCAT shift  145 */
  { 172,   0, 128}, /*  5:                 likeop shift  128 */
  {  89,  10,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  { 102,   0, 126}, /*  7:                 RSHIFT shift  126 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  80,   0, 137}, /*  9:                  MINUS shift  137 */
  {  77,   0, 124}, /* 10:                 LSHIFT shift  124 */
  { 106,   0, 141}, /* 11:                  SLASH shift  141 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
/* State 116 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 117}, /*  3:                   expr shift  117 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 117 */
  {  96,   2, 143}, /*  1:                    REM shift  143 */
  {  80,   0, 137}, /*  2:                  MINUS shift  137 */
  {  54,   0, 110}, /*  3:                     GT shift  110 */
  {  51,   0, 114}, /*  4:                     GE shift  114 */
  {  73,   0, 112}, /*  5:                     LE shift  112 */
  {  11,   0, 120}, /*  6:                 BITAND shift  120 */
  { 102,   3, 126}, /*  7:                 RSHIFT shift  126 */
  { 108,   0, 139}, /*  8:                   STAR shift  139 */
  {  24,   0, 145}, /*  9:                 CONCAT shift  145 */
  {  89,   5,  99}, /* 10:      ORACLE_OUTER_JOIN shift  99 */
  { 106,   0, 141}, /* 11:                  SLASH shift  141 */
  {  91,   6, 135}, /* 12:                   PLUS shift  135 */
  { 172,   8, 128}, /* 13:                 likeop shift  128 */
  {  77,  16, 124}, /* 14:                 LSHIFT shift  124 */
  {  78,   0, 108}, /* 15:                     LT shift  108 */
  {  13,   0, 122}, /* 16:                  BITOR shift  122 */
/* State 118 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 119}, /*  3:                   expr shift  119 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 119 */
  {  96,   2, 143}, /*  1:                    REM shift  143 */
  {  80,   0, 137}, /*  2:                  MINUS shift  137 */
  {  54,   0, 110}, /*  3:                     GT shift  110 */
  {  51,   0, 114}, /*  4:                     GE shift  114 */
  {  73,   0, 112}, /*  5:                     LE shift  112 */
  {  11,   0, 120}, /*  6:                 BITAND shift  120 */
  { 102,   3, 126}, /*  7:                 RSHIFT shift  126 */
  { 108,   0, 139}, /*  8:                   STAR shift  139 */
  {  24,   0, 145}, /*  9:                 CONCAT shift  145 */
  {  89,   5,  99}, /* 10:      ORACLE_OUTER_JOIN shift  99 */
  { 106,   0, 141}, /* 11:                  SLASH shift  141 */
  {  91,   6, 135}, /* 12:                   PLUS shift  135 */
  { 172,   8, 128}, /* 13:                 likeop shift  128 */
  {  77,  16, 124}, /* 14:                 LSHIFT shift  124 */
  {  78,   0, 108}, /* 15:                     LT shift  108 */
  {  13,   0, 122}, /* 16:                  BITOR shift  122 */
/* State 120 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 121}, /*  3:                   expr shift  121 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 121 */
  {  96,   6, 143}, /*  1:                    REM shift  143 */
  {  89,   0,  99}, /*  2:      ORACLE_OUTER_JOIN shift  99 */
  { 106,   0, 141}, /*  3:                  SLASH shift  141 */
  {  91,   0, 135}, /*  4:                   PLUS shift  135 */
  { 172,   7, 128}, /*  5:                 likeop shift  128 */
  {  80,   8, 137}, /*  6:                  MINUS shift  137 */
  { 108,   0, 139}, /*  7:                   STAR shift  139 */
  {  24,   0, 145}, /*  8:                 CONCAT shift  145 */
/* State 122 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 123}, /*  3:                   expr shift  123 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 123 */
  {  96,   6, 143}, /*  1:                    REM shift  143 */
  {  89,   0,  99}, /*  2:      ORACLE_OUTER_JOIN shift  99 */
  { 106,   0, 141}, /*  3:                  SLASH shift  141 */
  {  91,   0, 135}, /*  4:                   PLUS shift  135 */
  { 172,   7, 128}, /*  5:                 likeop shift  128 */
  {  80,   8, 137}, /*  6:                  MINUS shift  137 */
  { 108,   0, 139}, /*  7:                   STAR shift  139 */
  {  24,   0, 145}, /*  8:                 CONCAT shift  145 */
/* State 124 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 125}, /*  3:                   expr shift  125 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 125 */
  {  96,   6, 143}, /*  1:                    REM shift  143 */
  {  89,   0,  99}, /*  2:      ORACLE_OUTER_JOIN shift  99 */
  { 106,   0, 141}, /*  3:                  SLASH shift  141 */
  {  91,   0, 135}, /*  4:                   PLUS shift  135 */
  { 172,   7, 128}, /*  5:                 likeop shift  128 */
  {  80,   8, 137}, /*  6:                  MINUS shift  137 */
  { 108,   0, 139}, /*  7:                   STAR shift  139 */
  {  24,   0, 145}, /*  8:                 CONCAT shift  145 */
/* State 126 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 127}, /*  3:                   expr shift  127 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 127 */
  {  96,   6, 143}, /*  1:                    REM shift  143 */
  {  89,   0,  99}, /*  2:      ORACLE_OUTER_JOIN shift  99 */
  { 106,   0, 141}, /*  3:                  SLASH shift  141 */
  {  91,   0, 135}, /*  4:                   PLUS shift  135 */
  { 172,   7, 128}, /*  5:                 likeop shift  128 */
  {  80,   8, 137}, /*  6:                  MINUS shift  137 */
  { 108,   0, 139}, /*  7:                   STAR shift  139 */
  {  24,   0, 145}, /*  8:                 CONCAT shift  145 */
/* State 128 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 129}, /*  3:                   expr shift  129 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 129 */
  {  96,   2, 143}, /*  1:                    REM shift  143 */
  {  80,   0, 137}, /*  2:                  MINUS shift  137 */
  {  54,   0, 110}, /*  3:                     GT shift  110 */
  {  51,   0, 114}, /*  4:                     GE shift  114 */
  {  73,   0, 112}, /*  5:                     LE shift  112 */
  {  11,   0, 120}, /*  6:                 BITAND shift  120 */
  { 102,   3, 126}, /*  7:                 RSHIFT shift  126 */
  { 108,   0, 139}, /*  8:                   STAR shift  139 */
  {  24,   0, 145}, /*  9:                 CONCAT shift  145 */
  {  89,   5,  99}, /* 10:      ORACLE_OUTER_JOIN shift  99 */
  { 106,   0, 141}, /* 11:                  SLASH shift  141 */
  {  91,   6, 135}, /* 12:                   PLUS shift  135 */
  { 172,   8, 128}, /* 13:                 likeop shift  128 */
  {  77,  16, 124}, /* 14:                 LSHIFT shift  124 */
  {  78,   0, 108}, /* 15:                     LT shift  108 */
  {  13,   0, 122}, /* 16:                  BITOR shift  122 */
/* State 130 */
  {  84,   2, 213}, /*  1:                   NULL shift  213 */
  {  60,   0, 218}, /*  2:                     IN shift  218 */
  {  74,   0, 133}, /*  3:                   LIKE shift  133 */
  {  52,   6, 134}, /*  4:                   GLOB shift  134 */
  { 172,   4, 131}, /*  5:                 likeop shift  131 */
  {  10,   0, 214}, /*  6:                BETWEEN shift  214 */
/* State 131 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 132}, /*  3:                   expr shift  132 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 132 */
  {  96,   2, 143}, /*  1:                    REM shift  143 */
  {  80,   0, 137}, /*  2:                  MINUS shift  137 */
  {  54,   0, 110}, /*  3:                     GT shift  110 */
  {  51,   0, 114}, /*  4:                     GE shift  114 */
  {  73,   0, 112}, /*  5:                     LE shift  112 */
  {  11,   0, 120}, /*  6:                 BITAND shift  120 */
  { 102,   3, 126}, /*  7:                 RSHIFT shift  126 */
  { 108,   0, 139}, /*  8:                   STAR shift  139 */
  {  24,   0, 145}, /*  9:                 CONCAT shift  145 */
  {  89,   5,  99}, /* 10:      ORACLE_OUTER_JOIN shift  99 */
  { 106,   0, 141}, /* 11:                  SLASH shift  141 */
  {  91,   6, 135}, /* 12:                   PLUS shift  135 */
  { 172,   8, 128}, /* 13:                 likeop shift  128 */
  {  77,  16, 124}, /* 14:                 LSHIFT shift  124 */
  {  78,   0, 108}, /* 15:                     LT shift  108 */
  {  13,   0, 122}, /* 16:                  BITOR shift  122 */
/* State 135 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 136}, /*  3:                   expr shift  136 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 136 */
  { 108,   2, 139}, /*  1:                   STAR shift  139 */
  {  96,   3, 143}, /*  2:                    REM shift  143 */
  {  24,   0, 145}, /*  3:                 CONCAT shift  145 */
  { 106,   0, 141}, /*  4:                  SLASH shift  141 */
  { 172,   4, 128}, /*  5:                 likeop shift  128 */
  {  89,   0,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
/* State 137 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 138}, /*  3:                   expr shift  138 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 138 */
  { 108,   2, 139}, /*  1:                   STAR shift  139 */
  {  96,   3, 143}, /*  2:                    REM shift  143 */
  {  24,   0, 145}, /*  3:                 CONCAT shift  145 */
  { 106,   0, 141}, /*  4:                  SLASH shift  141 */
  { 172,   4, 128}, /*  5:                 likeop shift  128 */
  {  89,   0,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
/* State 139 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 140}, /*  3:                   expr shift  140 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 140 */
  {  24,   0, 145}, /*  1:                 CONCAT shift  145 */
  { 172,   0, 128}, /*  2:                 likeop shift  128 */
  {  89,   0,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
/* State 141 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 142}, /*  3:                   expr shift  142 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 142 */
  {  24,   0, 145}, /*  1:                 CONCAT shift  145 */
  { 172,   0, 128}, /*  2:                 likeop shift  128 */
  {  89,   0,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
/* State 143 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 144}, /*  3:                   expr shift  144 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 144 */
  {  24,   0, 145}, /*  1:                 CONCAT shift  145 */
  { 172,   0, 128}, /*  2:                 likeop shift  128 */
  {  89,   0,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
/* State 145 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 146}, /*  3:                   expr shift  146 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 146 */
  { 172,   0, 128}, /*  1:                 likeop shift  128 */
  {  89,   0,  99}, /*  2:      ORACLE_OUTER_JOIN shift  99 */
/* State 148 */
  {  84,   2, 149}, /*  1:                   NULL shift  149 */
  {  82,   0, 150}, /*  2:                    NOT shift  150 */
/* State 150 */
  {  84,   0, 151}, /*  1:                   NULL shift  151 */
/* State 153 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 154}, /*  3:                   expr shift  154 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 154 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 155}, /*  3:                    AND shift  155 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 155 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 156}, /*  3:                   expr shift  156 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 156 */
  {  96,   2, 143}, /*  1:                    REM shift  143 */
  {  80,   0, 137}, /*  2:                  MINUS shift  137 */
  {  54,   0, 110}, /*  3:                     GT shift  110 */
  {  51,   0, 114}, /*  4:                     GE shift  114 */
  {  73,   0, 112}, /*  5:                     LE shift  112 */
  {  11,   0, 120}, /*  6:                 BITAND shift  120 */
  { 102,   3, 126}, /*  7:                 RSHIFT shift  126 */
  { 108,   0, 139}, /*  8:                   STAR shift  139 */
  {  24,   0, 145}, /*  9:                 CONCAT shift  145 */
  {  89,   5,  99}, /* 10:      ORACLE_OUTER_JOIN shift  99 */
  { 106,   0, 141}, /* 11:                  SLASH shift  141 */
  {  91,   6, 135}, /* 12:                   PLUS shift  135 */
  { 172,   8, 128}, /* 13:                 likeop shift  128 */
  {  77,  16, 124}, /* 14:                 LSHIFT shift  124 */
  {  78,   0, 108}, /* 15:                     LT shift  108 */
  {  13,   0, 122}, /* 16:                  BITOR shift  122 */
/* State 157 */
  {  76,   0, 158}, /*  1:                     LP shift  158 */
/* State 158 */
  {  80,   0, 172}, /*  1:                  MINUS shift  172 */
  { 181,   0,  69}, /*  2:              oneselect shift  69 */
  {  82,   0, 168}, /*  3:                    NOT shift  168 */
  { 103,   0,  73}, /*  4:                 SELECT shift  73 */
  {  84,   0, 101}, /*  5:                   NULL shift  101 */
  {  65,   0, 166}, /*  6:                INTEGER shift  166 */
  {  46,   0, 167}, /*  7:                  FLOAT shift  167 */
  {  71,   0,  67}, /*  8:                JOIN_KW shift  67 */
  { 152,  10, 165}, /*  9:                   expr shift  165 */
  {  12,   0, 170}, /* 10:                 BITNOT shift  170 */
  { 110,   0,  66}, /* 11:                 STRING shift  66 */
  {  91,   8, 174}, /* 12:                   PLUS shift  174 */
  { 192,   9, 159}, /* 13:                 select shift  159 */
  { 153,   0, 212}, /* 14:               expritem shift  212 */
  { 154,  16, 208}, /* 15:               exprlist shift  208 */
  {  94,   0, 186}, /* 16:                  RAISE shift  186 */
  {  76,  19,  68}, /* 17:                     LP shift  68 */
  { 177,   0, 102}, /* 18:                     nm shift  102 */
  {  56,  20,  64}, /* 19:                     ID shift  64 */
  {  16,   0, 176}, /* 20:                   CASE shift  176 */
/* State 159 */
  { 101,   3, 164}, /*  1:                     RP shift  164 */
  { 176,   1,  71}, /*  2:         multiselect_op shift  71 */
  {  66,   0, 162}, /*  3:              INTERSECT shift  162 */
  { 118,   5, 160}, /*  4:                  UNION shift  160 */
  {  43,   0, 163}, /*  5:                 EXCEPT shift  163 */
/* State 160 */
  {   4,   0, 161}, /*  1:                    ALL shift  161 */
  { 103,   0, 634}, /*  2:                 SELECT reduce 103 */
/* State 161 */
  { 103,   0, 635}, /*  1:                 SELECT reduce 104 */
/* State 162 */
  { 103,   0, 636}, /*  1:                 SELECT reduce 105 */
/* State 163 */
  { 103,   0, 637}, /*  1:                 SELECT reduce 106 */
/* State 165 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 168 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 169}, /*  3:                   expr shift  169 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 169 */
  {  78,   2, 108}, /*  1:                     LT shift  108 */
  {  52,   0, 134}, /*  2:                   GLOB shift  134 */
  { 106,   7, 141}, /*  3:                  SLASH shift  141 */
  {  81,   0, 116}, /*  4:                     NE shift  116 */
  { 108,   8, 139}, /*  5:                   STAR shift  139 */
  {  83,   0, 152}, /*  6:                NOTNULL shift  152 */
  {  80,  10, 137}, /*  7:                  MINUS shift  137 */
  {  82,   0, 130}, /*  8:                    NOT shift  130 */
  {  60,   0, 157}, /*  9:                     IN shift  157 */
  {  54,   0, 110}, /* 10:                     GT shift  110 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  89,  13,  99}, /* 12:      ORACLE_OUTER_JOIN shift  99 */
  {  11,   0, 120}, /* 13:                 BITAND shift  120 */
  {  91,  15, 135}, /* 14:                   PLUS shift  135 */
  {  13,   0, 122}, /* 15:                  BITOR shift  122 */
  {  68,  20, 148}, /* 16:                     IS shift  148 */
  { 172,  16, 128}, /* 17:                 likeop shift  128 */
  {  69,   0, 147}, /* 18:                 ISNULL shift  147 */
  {  96,   0, 143}, /* 19:                    REM shift  143 */
  {  42,   0, 118}, /* 20:                     EQ shift  118 */
  {  24,   0, 145}, /* 21:                 CONCAT shift  145 */
  {  73,   0, 112}, /* 22:                     LE shift  112 */
  {  74,   0, 133}, /* 23:                   LIKE shift  133 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 102,  21, 126}, /* 25:                 RSHIFT shift  126 */
  {  77,  24, 124}, /* 26:                 LSHIFT shift  124 */
/* State 170 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 171}, /*  3:                   expr shift  171 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 171 */
  { 172,   0, 128}, /*  1:                 likeop shift  128 */
  {  89,   0,  99}, /*  2:      ORACLE_OUTER_JOIN shift  99 */
/* State 172 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 173}, /*  3:                   expr shift  173 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 173 */
  { 172,   0, 128}, /*  1:                 likeop shift  128 */
  {  89,   0,  99}, /*  2:      ORACLE_OUTER_JOIN shift  99 */
/* State 174 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 175}, /*  3:                   expr shift  175 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 175 */
  { 172,   0, 128}, /*  1:                 likeop shift  128 */
  {  89,   0,  99}, /*  2:      ORACLE_OUTER_JOIN shift  99 */
/* State 176 */
  { 126,   0, 755}, /*  1:                   WHEN reduce 224 */
  {  76,   0,  68}, /*  2:                     LP shift  68 */
  {  46,   5, 167}, /*  3:                  FLOAT shift  167 */
  {  71,   0,  67}, /*  4:                JOIN_KW shift  67 */
  {  12,   0, 170}, /*  5:                 BITNOT shift  170 */
  {  56,   0,  64}, /*  6:                     ID shift  64 */
  {  91,   0, 174}, /*  7:                   PLUS shift  174 */
  { 177,   1, 102}, /*  8:                     nm shift  102 */
  { 110,   2,  66}, /*  9:                 STRING shift  66 */
  {  94,   0, 186}, /* 10:                  RAISE shift  186 */
  {  82,  12, 168}, /* 11:                    NOT shift  168 */
  {  65,   0, 166}, /* 12:                INTEGER shift  166 */
  {  80,   3, 172}, /* 13:                  MINUS shift  172 */
  {  84,  16, 101}, /* 14:                   NULL shift  101 */
  { 133,  11, 178}, /* 15:           case_operand shift  178 */
  {  16,   0, 176}, /* 16:                   CASE shift  176 */
  { 152,  14, 177}, /* 17:                   expr shift  177 */
/* State 177 */
  {  60,   0, 157}, /*  1:                     IN shift  157 */
  {  88,   0, 106}, /*  2:                     OR shift  106 */
  {  89,   1,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
  {  68,   7, 148}, /*  4:                     IS shift  148 */
  {  91,   0, 135}, /*  5:                   PLUS shift  135 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  10,   0, 153}, /*  7:                BETWEEN shift  153 */
  {  11,   0, 120}, /*  8:                 BITAND shift  120 */
  {  13,   0, 122}, /*  9:                  BITOR shift  122 */
  {  96,   0, 143}, /* 10:                    REM shift  143 */
  { 126,   4, 754}, /* 11:                   WHEN reduce 223 */
  {  69,   8, 147}, /* 12:                 ISNULL shift  147 */
  {  73,   0, 112}, /* 13:                     LE shift  112 */
  {  42,   9, 118}, /* 14:                     EQ shift  118 */
  {  77,   0, 124}, /* 15:                 LSHIFT shift  124 */
  { 102,  13, 126}, /* 16:                 RSHIFT shift  126 */
  {  74,   0, 133}, /* 17:                   LIKE shift  133 */
  {  51,   0, 114}, /* 18:                     GE shift  114 */
  {  52,   0, 134}, /* 19:                   GLOB shift  134 */
  { 106,  15, 141}, /* 20:                  SLASH shift  141 */
  {  78,   0, 108}, /* 21:                     LT shift  108 */
  { 108,   0, 139}, /* 22:                   STAR shift  139 */
  {  80,  18, 137}, /* 23:                  MINUS shift  137 */
  {  81,  19, 116}, /* 24:                     NE shift  116 */
  {  82,  27, 130}, /* 25:                    NOT shift  130 */
  {  83,  29, 152}, /* 26:                NOTNULL shift  152 */
  {  24,   0, 145}, /* 27:                 CONCAT shift  145 */
  { 172,   0, 128}, /* 28:                 likeop shift  128 */
  {  54,   0, 110}, /* 29:                     GT shift  110 */
/* State 178 */
  { 132,   2, 179}, /*  1:          case_exprlist shift  179 */
  { 126,   0, 204}, /*  2:                   WHEN shift  204 */
/* State 179 */
  {  40,   0, 753}, /*  1:                    END reduce 222 */
  {  39,   0, 202}, /*  2:                   ELSE shift  202 */
  { 126,   0, 182}, /*  3:                   WHEN shift  182 */
  { 131,   2, 180}, /*  4:              case_else shift  180 */
/* State 180 */
  {  40,   0, 181}, /*  1:                    END shift  181 */
/* State 182 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 183}, /*  3:                   expr shift  183 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 183 */
  {  60,   0, 157}, /*  1:                     IN shift  157 */
  {  88,   0, 106}, /*  2:                     OR shift  106 */
  {  89,   1,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
  {  10,   0, 153}, /*  4:                BETWEEN shift  153 */
  {  91,   0, 135}, /*  5:                   PLUS shift  135 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  11,   0, 120}, /*  7:                 BITAND shift  120 */
  {  13,   0, 122}, /*  8:                  BITOR shift  122 */
  {  73,   0, 112}, /*  9:                     LE shift  112 */
  {  96,   0, 143}, /* 10:                    REM shift  143 */
  {  68,   4, 148}, /* 11:                     IS shift  148 */
  {  69,   7, 147}, /* 12:                 ISNULL shift  147 */
  {  77,   0, 124}, /* 13:                 LSHIFT shift  124 */
  {  42,   8, 118}, /* 14:                     EQ shift  118 */
  {  51,   0, 114}, /* 15:                     GE shift  114 */
  { 102,   9, 126}, /* 16:                 RSHIFT shift  126 */
  {  74,   0, 133}, /* 17:                   LIKE shift  133 */
  {  52,   0, 134}, /* 18:                   GLOB shift  134 */
  {  24,   0, 145}, /* 19:                 CONCAT shift  145 */
  { 106,  13, 141}, /* 20:                  SLASH shift  141 */
  {  78,   0, 108}, /* 21:                     LT shift  108 */
  { 108,   0, 139}, /* 22:                   STAR shift  139 */
  {  80,  15, 137}, /* 23:                  MINUS shift  137 */
  {  81,  18, 116}, /* 24:                     NE shift  116 */
  {  82,  19, 130}, /* 25:                    NOT shift  130 */
  {  83,  29, 152}, /* 26:                NOTNULL shift  152 */
  { 113,   0, 184}, /* 27:                   THEN shift  184 */
  { 172,   0, 128}, /* 28:                 likeop shift  128 */
  {  54,   0, 110}, /* 29:                     GT shift  110 */
/* State 184 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 185}, /*  3:                   expr shift  185 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 185 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 186 */
  {  76,   0, 187}, /*  1:                     LP shift  187 */
/* State 187 */
  {  45,   3, 198}, /*  1:                   FAIL shift  198 */
  {  57,   1, 188}, /*  2:                 IGNORE shift  188 */
  {   1,   0, 194}, /*  3:                  ABORT shift  194 */
  {  99,   0, 190}, /*  4:               ROLLBACK shift  190 */
/* State 188 */
  { 101,   0, 189}, /*  1:                     RP shift  189 */
/* State 190 */
  {  21,   0, 191}, /*  1:                  COMMA shift  191 */
/* State 191 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 192}, /*  2:                     nm shift  192 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 192 */
  { 101,   0, 193}, /*  1:                     RP shift  193 */
/* State 194 */
  {  21,   0, 195}, /*  1:                  COMMA shift  195 */
/* State 195 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 196}, /*  2:                     nm shift  196 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 196 */
  { 101,   0, 197}, /*  1:                     RP shift  197 */
/* State 198 */
  {  21,   0, 199}, /*  1:                  COMMA shift  199 */
/* State 199 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 200}, /*  2:                     nm shift  200 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 200 */
  { 101,   0, 201}, /*  1:                     RP shift  201 */
/* State 202 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 203}, /*  3:                   expr shift  203 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 203 */
  {  60,   0, 157}, /*  1:                     IN shift  157 */
  {  88,   0, 106}, /*  2:                     OR shift  106 */
  {  89,   1,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
  {  10,   0, 153}, /*  4:                BETWEEN shift  153 */
  {  91,   0, 135}, /*  5:                   PLUS shift  135 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  40,   8, 752}, /*  7:                    END reduce 221 */
  {  11,   0, 120}, /*  8:                 BITAND shift  120 */
  {  13,   0, 122}, /*  9:                  BITOR shift  122 */
  {  96,   0, 143}, /* 10:                    REM shift  143 */
  {  68,   4, 148}, /* 11:                     IS shift  148 */
  {  69,   7, 147}, /* 12:                 ISNULL shift  147 */
  {  73,   0, 112}, /* 13:                     LE shift  112 */
  {  42,   9, 118}, /* 14:                     EQ shift  118 */
  {  77,   0, 124}, /* 15:                 LSHIFT shift  124 */
  { 102,  13, 126}, /* 16:                 RSHIFT shift  126 */
  {  74,   0, 133}, /* 17:                   LIKE shift  133 */
  {  51,   0, 114}, /* 18:                     GE shift  114 */
  {  52,   0, 134}, /* 19:                   GLOB shift  134 */
  { 106,  15, 141}, /* 20:                  SLASH shift  141 */
  {  78,   0, 108}, /* 21:                     LT shift  108 */
  { 108,   0, 139}, /* 22:                   STAR shift  139 */
  {  80,  18, 137}, /* 23:                  MINUS shift  137 */
  {  81,  19, 116}, /* 24:                     NE shift  116 */
  {  82,  27, 130}, /* 25:                    NOT shift  130 */
  {  83,  29, 152}, /* 26:                NOTNULL shift  152 */
  {  24,   0, 145}, /* 27:                 CONCAT shift  145 */
  { 172,   0, 128}, /* 28:                 likeop shift  128 */
  {  54,   0, 110}, /* 29:                     GT shift  110 */
/* State 204 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 205}, /*  3:                   expr shift  205 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 205 */
  {  60,   0, 157}, /*  1:                     IN shift  157 */
  {  88,   0, 106}, /*  2:                     OR shift  106 */
  {  89,   1,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
  {  10,   0, 153}, /*  4:                BETWEEN shift  153 */
  {  91,   0, 135}, /*  5:                   PLUS shift  135 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  11,   0, 120}, /*  7:                 BITAND shift  120 */
  {  13,   0, 122}, /*  8:                  BITOR shift  122 */
  {  73,   0, 112}, /*  9:                     LE shift  112 */
  {  96,   0, 143}, /* 10:                    REM shift  143 */
  {  68,   4, 148}, /* 11:                     IS shift  148 */
  {  69,   7, 147}, /* 12:                 ISNULL shift  147 */
  {  77,   0, 124}, /* 13:                 LSHIFT shift  124 */
  {  42,   8, 118}, /* 14:                     EQ shift  118 */
  {  51,   0, 114}, /* 15:                     GE shift  114 */
  { 102,   9, 126}, /* 16:                 RSHIFT shift  126 */
  {  74,   0, 133}, /* 17:                   LIKE shift  133 */
  {  52,   0, 134}, /* 18:                   GLOB shift  134 */
  {  24,   0, 145}, /* 19:                 CONCAT shift  145 */
  { 106,  13, 141}, /* 20:                  SLASH shift  141 */
  {  78,   0, 108}, /* 21:                     LT shift  108 */
  { 108,   0, 139}, /* 22:                   STAR shift  139 */
  {  80,  15, 137}, /* 23:                  MINUS shift  137 */
  {  81,  18, 116}, /* 24:                     NE shift  116 */
  {  82,  19, 130}, /* 25:                    NOT shift  130 */
  {  83,  29, 152}, /* 26:                NOTNULL shift  152 */
  { 113,   0, 206}, /* 27:                   THEN shift  206 */
  { 172,   0, 128}, /* 28:                 likeop shift  128 */
  {  54,   0, 110}, /* 29:                     GT shift  110 */
/* State 206 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 207}, /*  3:                   expr shift  207 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 207 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 208 */
  {  21,   0, 210}, /*  1:                  COMMA shift  210 */
  { 101,   1, 209}, /*  2:                     RP shift  209 */
/* State 210 */
  {  80,   4, 172}, /*  1:                  MINUS shift  172 */
  { 177,   6, 102}, /*  2:                     nm shift  102 */
  {  82,   0, 168}, /*  3:                    NOT shift  168 */
  {  16,   0, 176}, /*  4:                   CASE shift  176 */
  {  84,   0, 101}, /*  5:                   NULL shift  101 */
  {  65,   0, 166}, /*  6:                INTEGER shift  166 */
  {  56,   0,  64}, /*  7:                     ID shift  64 */
  {  71,   0,  67}, /*  8:                JOIN_KW shift  67 */
  { 152,   7, 165}, /*  9:                   expr shift  165 */
  { 153,   0, 211}, /* 10:               expritem shift  211 */
  {  12,   0, 170}, /* 11:                 BITNOT shift  170 */
  {  91,   0, 174}, /* 12:                   PLUS shift  174 */
  {  76,  11,  68}, /* 13:                     LP shift  68 */
  {  94,  16, 186}, /* 14:                  RAISE shift  186 */
  { 110,  14,  66}, /* 15:                 STRING shift  66 */
  {  46,   0, 167}, /* 16:                  FLOAT shift  167 */
/* State 214 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 215}, /*  3:                   expr shift  215 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 215 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 216}, /*  3:                    AND shift  216 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 216 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 217}, /*  3:                   expr shift  217 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 217 */
  {  78,   2, 108}, /*  1:                     LT shift  108 */
  {  52,   0, 134}, /*  2:                   GLOB shift  134 */
  { 106,   7, 141}, /*  3:                  SLASH shift  141 */
  {  81,   0, 116}, /*  4:                     NE shift  116 */
  { 108,   8, 139}, /*  5:                   STAR shift  139 */
  {  83,   0, 152}, /*  6:                NOTNULL shift  152 */
  {  80,  10, 137}, /*  7:                  MINUS shift  137 */
  {  82,   0, 130}, /*  8:                    NOT shift  130 */
  {  60,   0, 157}, /*  9:                     IN shift  157 */
  {  54,   0, 110}, /* 10:                     GT shift  110 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  89,  13,  99}, /* 12:      ORACLE_OUTER_JOIN shift  99 */
  {  11,   0, 120}, /* 13:                 BITAND shift  120 */
  {  91,  15, 135}, /* 14:                   PLUS shift  135 */
  {  13,   0, 122}, /* 15:                  BITOR shift  122 */
  {  68,  20, 148}, /* 16:                     IS shift  148 */
  { 172,  16, 128}, /* 17:                 likeop shift  128 */
  {  69,   0, 147}, /* 18:                 ISNULL shift  147 */
  {  96,   0, 143}, /* 19:                    REM shift  143 */
  {  42,   0, 118}, /* 20:                     EQ shift  118 */
  {  24,   0, 145}, /* 21:                 CONCAT shift  145 */
  {  73,   0, 112}, /* 22:                     LE shift  112 */
  {  74,   0, 133}, /* 23:                   LIKE shift  133 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 102,  21, 126}, /* 25:                 RSHIFT shift  126 */
  {  77,  24, 124}, /* 26:                 LSHIFT shift  124 */
/* State 218 */
  {  76,   0, 219}, /*  1:                     LP shift  219 */
/* State 219 */
  {  80,   0, 172}, /*  1:                  MINUS shift  172 */
  { 181,   0,  69}, /*  2:              oneselect shift  69 */
  {  82,   0, 168}, /*  3:                    NOT shift  168 */
  { 103,   0,  73}, /*  4:                 SELECT shift  73 */
  {  84,   0, 101}, /*  5:                   NULL shift  101 */
  {  65,   0, 166}, /*  6:                INTEGER shift  166 */
  {  46,   0, 167}, /*  7:                  FLOAT shift  167 */
  {  71,   0,  67}, /*  8:                JOIN_KW shift  67 */
  { 152,  10, 165}, /*  9:                   expr shift  165 */
  {  12,   0, 170}, /* 10:                 BITNOT shift  170 */
  { 110,   0,  66}, /* 11:                 STRING shift  66 */
  {  91,   8, 174}, /* 12:                   PLUS shift  174 */
  { 192,   9, 220}, /* 13:                 select shift  220 */
  { 153,   0, 212}, /* 14:               expritem shift  212 */
  { 154,  16, 222}, /* 15:               exprlist shift  222 */
  {  94,   0, 186}, /* 16:                  RAISE shift  186 */
  {  76,  19,  68}, /* 17:                     LP shift  68 */
  { 177,   0, 102}, /* 18:                     nm shift  102 */
  {  56,  20,  64}, /* 19:                     ID shift  64 */
  {  16,   0, 176}, /* 20:                   CASE shift  176 */
/* State 220 */
  { 101,   3, 221}, /*  1:                     RP shift  221 */
  { 176,   1,  71}, /*  2:         multiselect_op shift  71 */
  {  66,   0, 162}, /*  3:              INTERSECT shift  162 */
  { 118,   5, 160}, /*  4:                  UNION shift  160 */
  {  43,   0, 163}, /*  5:                 EXCEPT shift  163 */
/* State 222 */
  {  21,   0, 210}, /*  1:                  COMMA shift  210 */
  { 101,   1, 223}, /*  2:                     RP shift  223 */
/* State 224 */
  { 138,   0, 225}, /*  1:                collate shift  225 */
  {  19,   0,  95}, /*  2:                COLLATE shift  95 */
/* State 225 */
  { 198,   0, 226}, /*  1:              sortorder shift  226 */
  {  34,   3,  59}, /*  2:                   DESC shift  59 */
  {   7,   0,  58}, /*  3:                    ASC shift  58 */
/* State 227 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 228}, /*  3:                   expr shift  228 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 228 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 229 */
  {  14,   0, 230}, /*  1:                     BY shift  230 */
/* State 230 */
  { 153,   0, 212}, /*  1:               expritem shift  212 */
  { 154,   0, 231}, /*  2:               exprlist shift  231 */
  {  76,   0,  68}, /*  3:                     LP shift  68 */
  {  71,   0,  67}, /*  4:                JOIN_KW shift  67 */
  {  46,  11, 167}, /*  5:                  FLOAT shift  167 */
  {  56,   0,  64}, /*  6:                     ID shift  64 */
  {  91,   0, 174}, /*  7:                   PLUS shift  174 */
  { 177,   0, 102}, /*  8:                     nm shift  102 */
  { 110,   3,  66}, /*  9:                 STRING shift  66 */
  {  94,   0, 186}, /* 10:                  RAISE shift  186 */
  {  12,   0, 170}, /* 11:                 BITNOT shift  170 */
  {  65,   0, 166}, /* 12:                INTEGER shift  166 */
  {  80,   5, 172}, /* 13:                  MINUS shift  172 */
  {  84,  16, 101}, /* 14:                   NULL shift  101 */
  {  82,  12, 168}, /* 15:                    NOT shift  168 */
  {  16,   0, 176}, /* 16:                   CASE shift  176 */
  { 152,  14, 165}, /* 17:                   expr shift  165 */
/* State 231 */
  {  21,   0, 210}, /*  1:                  COMMA shift  210 */
/* State 232 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 233}, /*  3:                   expr shift  233 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 233 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 235 */
  { 193,   0, 236}, /*  1:             seltablist shift  236 */
  { 199,   1, 246}, /*  2:             stl_prefix shift  246 */
/* State 236 */
  {  70,   0, 239}, /*  1:                   JOIN shift  239 */
  {  21,   0, 238}, /*  2:                  COMMA shift  238 */
  { 170,   1, 237}, /*  3:                 joinop shift  237 */
  {  71,   0, 240}, /*  4:                JOIN_KW shift  240 */
/* State 240 */
  { 110,   4,  21}, /*  1:                 STRING shift  21 */
  {  71,   5,  22}, /*  2:                JOIN_KW shift  22 */
  { 177,   0, 242}, /*  3:                     nm shift  242 */
  {  70,   0, 241}, /*  4:                   JOIN shift  241 */
  {  56,   0,  20}, /*  5:                     ID shift  20 */
/* State 242 */
  { 110,   4,  21}, /*  1:                 STRING shift  21 */
  {  71,   5,  22}, /*  2:                JOIN_KW shift  22 */
  { 177,   0, 244}, /*  3:                     nm shift  244 */
  {  70,   0, 243}, /*  4:                   JOIN shift  243 */
  {  56,   0,  20}, /*  5:                     ID shift  20 */
/* State 244 */
  {  70,   0, 245}, /*  1:                   JOIN shift  245 */
/* State 246 */
  { 110,   0,  21}, /*  1:                 STRING shift  21 */
  {  76,   4, 266}, /*  2:                     LP shift  266 */
  { 177,   0, 247}, /*  3:                     nm shift  247 */
  {  71,   5,  22}, /*  4:                JOIN_KW shift  22 */
  {  56,   0,  20}, /*  5:                     ID shift  20 */
/* State 247 */
  { 160,   3, 252}, /*  1:                    ids shift  252 */
  {  56,   5, 248}, /*  2:                     ID shift  248 */
  { 110,   0, 249}, /*  3:                 STRING shift  249 */
  { 128,   0, 253}, /*  4:                     as shift  253 */
  {   6,   0, 250}, /*  5:                     AS shift  250 */
/* State 250 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 251}, /*  2:                     nm shift  251 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 253 */
  {  87,   0, 264}, /*  1:                     ON shift  264 */
  { 179,   1, 254}, /*  2:                 on_opt shift  254 */
/* State 254 */
  { 210,   2, 255}, /*  1:              using_opt shift  255 */
  { 122,   0, 256}, /*  2:                  USING shift  256 */
/* State 256 */
  {  76,   0, 257}, /*  1:                     LP shift  257 */
/* State 257 */
  { 162,   0, 258}, /*  1:                idxlist shift  258 */
  {  56,   0,  20}, /*  2:                     ID shift  20 */
  { 110,   2,  21}, /*  3:                 STRING shift  21 */
  { 177,   0, 262}, /*  4:                     nm shift  262 */
  {  71,   0,  22}, /*  5:                JOIN_KW shift  22 */
  { 161,   5, 263}, /*  6:                idxitem shift  263 */
/* State 258 */
  {  21,   0, 260}, /*  1:                  COMMA shift  260 */
  { 101,   1, 259}, /*  2:                     RP shift  259 */
/* State 260 */
  { 110,   0,  21}, /*  1:                 STRING shift  21 */
  { 161,   4, 261}, /*  2:                idxitem shift  261 */
  { 177,   0, 262}, /*  3:                     nm shift  262 */
  {  71,   5,  22}, /*  4:                JOIN_KW shift  22 */
  {  56,   0,  20}, /*  5:                     ID shift  20 */
/* State 264 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 265}, /*  3:                   expr shift  265 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 265 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 266 */
  { 192,   0, 267}, /*  1:                 select shift  267 */
  { 181,   3,  69}, /*  2:              oneselect shift  69 */
  { 103,   0,  73}, /*  3:                 SELECT shift  73 */
/* State 267 */
  { 101,   3, 268}, /*  1:                     RP shift  268 */
  { 176,   1,  71}, /*  2:         multiselect_op shift  71 */
  {  66,   0, 162}, /*  3:              INTERSECT shift  162 */
  { 118,   5, 160}, /*  4:                  UNION shift  160 */
  {  43,   0, 163}, /*  5:                 EXCEPT shift  163 */
/* State 268 */
  { 160,   3, 252}, /*  1:                    ids shift  252 */
  {  56,   5, 248}, /*  2:                     ID shift  248 */
  { 110,   0, 249}, /*  3:                 STRING shift  249 */
  { 128,   0, 269}, /*  4:                     as shift  269 */
  {   6,   0, 250}, /*  5:                     AS shift  250 */
/* State 269 */
  {  87,   0, 264}, /*  1:                     ON shift  264 */
  { 179,   1, 270}, /*  2:                 on_opt shift  270 */
/* State 270 */
  { 210,   2, 271}, /*  1:              using_opt shift  271 */
  { 122,   0, 256}, /*  2:                  USING shift  256 */
/* State 272 */
  {  80,   4, 172}, /*  1:                  MINUS shift  172 */
  { 177,   6, 276}, /*  2:                     nm shift  276 */
  {  82,   0, 168}, /*  3:                    NOT shift  168 */
  {  16,   0, 176}, /*  4:                   CASE shift  176 */
  {  84,   0, 101}, /*  5:                   NULL shift  101 */
  {  65,   0, 166}, /*  6:                INTEGER shift  166 */
  {  56,   0,  64}, /*  7:                     ID shift  64 */
  {  71,   0,  67}, /*  8:                JOIN_KW shift  67 */
  { 152,   7, 273}, /*  9:                   expr shift  273 */
  {  76,  11,  68}, /* 10:                     LP shift  68 */
  {  12,   0, 170}, /* 11:                 BITNOT shift  170 */
  {  91,   0, 174}, /* 12:                   PLUS shift  174 */
  { 108,  10, 275}, /* 13:                   STAR shift  275 */
  {  94,  16, 186}, /* 14:                  RAISE shift  186 */
  { 110,  14,  66}, /* 15:                 STRING shift  66 */
  {  46,   0, 167}, /* 16:                  FLOAT shift  167 */
/* State 273 */
  {  69,   0, 147}, /*  1:                 ISNULL shift  147 */
  { 106,   5, 141}, /*  2:                  SLASH shift  141 */
  {  68,   0, 148}, /*  3:                     IS shift  148 */
  { 102,   1, 126}, /*  4:                 RSHIFT shift  126 */
  {  73,   0, 112}, /*  5:                     LE shift  112 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {   6,   0, 250}, /*  7:                     AS shift  250 */
  { 172,   2, 128}, /*  8:                 likeop shift  128 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  { 108,  21, 139}, /* 10:                   STAR shift  139 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  { 110,  27, 249}, /* 12:                 STRING shift  249 */
  {  78,   0, 108}, /* 13:                     LT shift  108 */
  {  13,   0, 122}, /* 14:                  BITOR shift  122 */
  {  80,   0, 137}, /* 15:                  MINUS shift  137 */
  {  81,   0, 116}, /* 16:                     NE shift  116 */
  {  82,   0, 130}, /* 17:                    NOT shift  130 */
  {  83,   0, 152}, /* 18:                NOTNULL shift  152 */
  {  51,   0, 114}, /* 19:                     GE shift  114 */
  {  52,   0, 134}, /* 20:                   GLOB shift  134 */
  {  42,   0, 118}, /* 21:                     EQ shift  118 */
  {  54,   0, 110}, /* 22:                     GT shift  110 */
  {  88,   0, 106}, /* 23:                     OR shift  106 */
  {  89,  32,  99}, /* 24:      ORACLE_OUTER_JOIN shift  99 */
  {  24,   0, 145}, /* 25:                 CONCAT shift  145 */
  {  91,   0, 135}, /* 26:                   PLUS shift  135 */
  {  77,  33, 124}, /* 27:                 LSHIFT shift  124 */
  {  60,   0, 157}, /* 28:                     IN shift  157 */
  { 160,   0, 252}, /* 29:                    ids shift  252 */
  { 128,   0, 274}, /* 30:                     as shift  274 */
  {  96,   0, 143}, /* 31:                    REM shift  143 */
  {  56,   0, 248}, /* 32:                     ID shift  248 */
  {  11,   0, 120}, /* 33:                 BITAND shift  120 */
/* State 276 */
  {  36,   0, 277}, /*  1:                    DOT shift  277 */
/* State 277 */
  { 110,   0,  21}, /*  1:                 STRING shift  21 */
  {  71,   5,  22}, /*  2:                JOIN_KW shift  22 */
  { 177,   0, 104}, /*  3:                     nm shift  104 */
  { 108,   0, 278}, /*  4:                   STAR shift  278 */
  {  56,   0,  20}, /*  5:                     ID shift  20 */
/* State 282 */
  {  60,   0, 157}, /*  1:                     IN shift  157 */
  {  88,   0, 106}, /*  2:                     OR shift  106 */
  {  89,   1,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
  {  10,   0, 153}, /*  4:                BETWEEN shift  153 */
  {  91,   0, 135}, /*  5:                   PLUS shift  135 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  11,   0, 120}, /*  7:                 BITAND shift  120 */
  {  13,   0, 122}, /*  8:                  BITOR shift  122 */
  {  73,   0, 112}, /*  9:                     LE shift  112 */
  {  96,   0, 143}, /* 10:                    REM shift  143 */
  {  68,   4, 148}, /* 11:                     IS shift  148 */
  {  69,   7, 147}, /* 12:                 ISNULL shift  147 */
  {  77,   0, 124}, /* 13:                 LSHIFT shift  124 */
  {  42,   8, 118}, /* 14:                     EQ shift  118 */
  { 101,   0, 283}, /* 15:                     RP shift  283 */
  { 102,   9, 126}, /* 16:                 RSHIFT shift  126 */
  {  74,   0, 133}, /* 17:                   LIKE shift  133 */
  {  51,   0, 114}, /* 18:                     GE shift  114 */
  {  52,   0, 134}, /* 19:                   GLOB shift  134 */
  { 106,  13, 141}, /* 20:                  SLASH shift  141 */
  {  78,   0, 108}, /* 21:                     LT shift  108 */
  { 108,   0, 139}, /* 22:                   STAR shift  139 */
  {  80,  18, 137}, /* 23:                  MINUS shift  137 */
  {  81,  19, 116}, /* 24:                     NE shift  116 */
  {  82,  27, 130}, /* 25:                    NOT shift  130 */
  {  83,  29, 152}, /* 26:                NOTNULL shift  152 */
  {  24,   0, 145}, /* 27:                 CONCAT shift  145 */
  { 172,   0, 128}, /* 28:                 likeop shift  128 */
  {  54,   0, 110}, /* 29:                     GT shift  110 */
/* State 284 */
  {  21,   0, 210}, /*  1:                  COMMA shift  210 */
  { 101,   1, 285}, /*  2:                     RP shift  285 */
/* State 286 */
  { 101,   0, 287}, /*  1:                     RP shift  287 */
/* State 288 */
  {  60,   0, 157}, /*  1:                     IN shift  157 */
  {  88,   0, 106}, /*  2:                     OR shift  106 */
  {  89,   1,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
  {  10,   0, 153}, /*  4:                BETWEEN shift  153 */
  {  91,   0, 135}, /*  5:                   PLUS shift  135 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  11,   0, 120}, /*  7:                 BITAND shift  120 */
  {  13,   0, 122}, /*  8:                  BITOR shift  122 */
  {  73,   0, 112}, /*  9:                     LE shift  112 */
  {  96,   0, 143}, /* 10:                    REM shift  143 */
  {  68,   4, 148}, /* 11:                     IS shift  148 */
  {  69,   7, 147}, /* 12:                 ISNULL shift  147 */
  {  77,   0, 124}, /* 13:                 LSHIFT shift  124 */
  {  42,   8, 118}, /* 14:                     EQ shift  118 */
  { 101,   0, 289}, /* 15:                     RP shift  289 */
  { 102,   9, 126}, /* 16:                 RSHIFT shift  126 */
  {  74,   0, 133}, /* 17:                   LIKE shift  133 */
  {  51,   0, 114}, /* 18:                     GE shift  114 */
  {  52,   0, 134}, /* 19:                   GLOB shift  134 */
  { 106,  13, 141}, /* 20:                  SLASH shift  141 */
  {  78,   0, 108}, /* 21:                     LT shift  108 */
  { 108,   0, 139}, /* 22:                   STAR shift  139 */
  {  80,  18, 137}, /* 23:                  MINUS shift  137 */
  {  81,  19, 116}, /* 24:                     NE shift  116 */
  {  82,  27, 130}, /* 25:                    NOT shift  130 */
  {  83,  29, 152}, /* 26:                NOTNULL shift  152 */
  {  24,   0, 145}, /* 27:                 CONCAT shift  145 */
  { 172,   0, 128}, /* 28:                 likeop shift  128 */
  {  54,   0, 110}, /* 29:                     GT shift  110 */
/* State 289 */
  { 180,   0, 290}, /*  1:                 onconf shift  290 */
  {  87,   0,  10}, /*  2:                     ON shift  10 */
/* State 291 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 292}, /*  2:                     nm shift  292 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 292 */
  {  76,   0, 310}, /*  1:                     LP shift  310 */
  { 163,   0, 293}, /*  2:            idxlist_opt shift  293 */
/* State 293 */
  { 188,   0, 294}, /*  1:                refargs shift  294 */
/* State 294 */
  {  87,   0, 298}, /*  1:                     ON shift  298 */
  { 187,   3, 295}, /*  2:                 refarg shift  295 */
  {  79,   0, 296}, /*  3:                  MATCH shift  296 */
/* State 296 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 297}, /*  2:                     nm shift  297 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 298 */
  { 120,   2, 306}, /*  1:                 UPDATE shift  306 */
  {  63,   0, 308}, /*  2:                 INSERT shift  308 */
  {  32,   0, 299}, /*  3:                 DELETE shift  299 */
/* State 299 */
  {  98,   0, 305}, /*  1:               RESTRICT shift  305 */
  { 105,   0, 301}, /*  2:                    SET shift  301 */
  { 186,   1, 300}, /*  3:                 refact shift  300 */
  {  15,   0, 304}, /*  4:                CASCADE shift  304 */
/* State 301 */
  {  84,   0, 302}, /*  1:                   NULL shift  302 */
  {  29,   0, 303}, /*  2:                DEFAULT shift  303 */
/* State 306 */
  {  98,   0, 305}, /*  1:               RESTRICT shift  305 */
  { 105,   0, 301}, /*  2:                    SET shift  301 */
  { 186,   1, 307}, /*  3:                 refact shift  307 */
  {  15,   0, 304}, /*  4:                CASCADE shift  304 */
/* State 308 */
  {  98,   0, 305}, /*  1:               RESTRICT shift  305 */
  { 105,   0, 301}, /*  2:                    SET shift  301 */
  { 186,   1, 309}, /*  3:                 refact shift  309 */
  {  15,   0, 304}, /*  4:                CASCADE shift  304 */
/* State 310 */
  { 162,   0, 311}, /*  1:                idxlist shift  311 */
  {  56,   0,  20}, /*  2:                     ID shift  20 */
  { 110,   2,  21}, /*  3:                 STRING shift  21 */
  { 177,   0, 262}, /*  4:                     nm shift  262 */
  {  71,   0,  22}, /*  5:                JOIN_KW shift  22 */
  { 161,   5, 263}, /*  6:                idxitem shift  263 */
/* State 311 */
  {  21,   0, 260}, /*  1:                  COMMA shift  260 */
  { 101,   1, 312}, /*  2:                     RP shift  312 */
/* State 314 */
  {  56,   0,  96}, /*  1:                     ID shift  96 */
  { 159,   0, 315}, /*  2:                     id shift  315 */
/* State 316 */
  { 164,   2, 317}, /*  1: init_deferred_pred_opt shift  317 */
  {  62,   0,  51}, /*  2:              INITIALLY shift  51 */
/* State 319 */
  {  91,   2, 323}, /*  1:                   PLUS shift  323 */
  {  84,   7, 330}, /*  2:                   NULL shift  330 */
  {  65,   0, 322}, /*  3:                INTEGER shift  322 */
  {  80,   0, 326}, /*  4:                  MINUS shift  326 */
  {  46,   0, 329}, /*  5:                  FLOAT shift  329 */
  { 110,   0, 320}, /*  6:                 STRING shift  320 */
  {  56,   0, 321}, /*  7:                     ID shift  321 */
/* State 323 */
  {  46,   0, 325}, /*  1:                  FLOAT shift  325 */
  {  65,   0, 324}, /*  2:                INTEGER shift  324 */
/* State 326 */
  {  46,   0, 328}, /*  1:                  FLOAT shift  328 */
  {  65,   0, 327}, /*  2:                INTEGER shift  327 */
/* State 331 */
  { 160,   2, 343}, /*  1:                    ids shift  343 */
  {  76,   4, 332}, /*  2:                     LP shift  332 */
  { 110,   0, 249}, /*  3:                 STRING shift  249 */
  {  56,   0, 248}, /*  4:                     ID shift  248 */
/* State 332 */
  {  80,   0, 341}, /*  1:                  MINUS shift  341 */
  {  65,   0, 338}, /*  2:                INTEGER shift  338 */
  {  91,   0, 339}, /*  3:                   PLUS shift  339 */
  { 195,   3, 333}, /*  4:                 signed shift  333 */
/* State 333 */
  {  21,   0, 335}, /*  1:                  COMMA shift  335 */
  { 101,   1, 334}, /*  2:                     RP shift  334 */
/* State 335 */
  {  80,   0, 341}, /*  1:                  MINUS shift  341 */
  {  65,   0, 338}, /*  2:                INTEGER shift  338 */
  {  91,   0, 339}, /*  3:                   PLUS shift  339 */
  { 195,   3, 336}, /*  4:                 signed shift  336 */
/* State 336 */
  { 101,   0, 337}, /*  1:                     RP shift  337 */
/* State 339 */
  {  65,   0, 340}, /*  1:                INTEGER shift  340 */
/* State 341 */
  {  65,   0, 342}, /*  1:                INTEGER shift  342 */
/* State 346 */
  { 200,   4, 377}, /*  1:                  tcons shift  377 */
  {  17,   0, 362}, /*  2:                  CHECK shift  362 */
  {  26,   0, 349}, /*  3:             CONSTRAINT shift  349 */
  {  48,   0, 365}, /*  4:                FOREIGN shift  365 */
  {  93,   7, 351}, /*  5:                PRIMARY shift  351 */
  { 101,   5, 608}, /*  6:                     RP reduce 77 */
  {  21,   0, 347}, /*  7:                  COMMA shift  347 */
  { 119,   0, 357}, /*  8:                 UNIQUE shift  357 */
/* State 347 */
  {  48,   0, 365}, /*  1:                FOREIGN shift  365 */
  {  26,   0, 349}, /*  2:             CONSTRAINT shift  349 */
  { 200,   2, 348}, /*  3:                  tcons shift  348 */
  {  93,   0, 351}, /*  4:                PRIMARY shift  351 */
  {  17,   0, 362}, /*  5:                  CHECK shift  362 */
  { 119,   5, 357}, /*  6:                 UNIQUE shift  357 */
/* State 349 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 350}, /*  2:                     nm shift  350 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 351 */
  {  72,   0, 352}, /*  1:                    KEY shift  352 */
/* State 352 */
  {  76,   0, 353}, /*  1:                     LP shift  353 */
/* State 353 */
  { 162,   0, 354}, /*  1:                idxlist shift  354 */
  {  56,   0,  20}, /*  2:                     ID shift  20 */
  { 110,   2,  21}, /*  3:                 STRING shift  21 */
  { 177,   0, 262}, /*  4:                     nm shift  262 */
  {  71,   0,  22}, /*  5:                JOIN_KW shift  22 */
  { 161,   5, 263}, /*  6:                idxitem shift  263 */
/* State 354 */
  {  21,   0, 260}, /*  1:                  COMMA shift  260 */
  { 101,   1, 355}, /*  2:                     RP shift  355 */
/* State 355 */
  { 180,   0, 356}, /*  1:                 onconf shift  356 */
  {  87,   0,  10}, /*  2:                     ON shift  10 */
/* State 357 */
  {  76,   0, 358}, /*  1:                     LP shift  358 */
/* State 358 */
  { 162,   0, 359}, /*  1:                idxlist shift  359 */
  {  56,   0,  20}, /*  2:                     ID shift  20 */
  { 110,   2,  21}, /*  3:                 STRING shift  21 */
  { 177,   0, 262}, /*  4:                     nm shift  262 */
  {  71,   0,  22}, /*  5:                JOIN_KW shift  22 */
  { 161,   5, 263}, /*  6:                idxitem shift  263 */
/* State 359 */
  {  21,   0, 260}, /*  1:                  COMMA shift  260 */
  { 101,   1, 360}, /*  2:                     RP shift  360 */
/* State 360 */
  { 180,   0, 361}, /*  1:                 onconf shift  361 */
  {  87,   0,  10}, /*  2:                     ON shift  10 */
/* State 362 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 363}, /*  3:                   expr shift  363 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 363 */
  { 180,   3, 364}, /*  1:                 onconf shift  364 */
  {  91,   0, 135}, /*  2:                   PLUS shift  135 */
  {  60,   0, 157}, /*  3:                     IN shift  157 */
  {  42,   0, 118}, /*  4:                     EQ shift  118 */
  {  13,   0, 122}, /*  5:                  BITOR shift  122 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  96,   0, 143}, /*  7:                    REM shift  143 */
  {  78,   0, 108}, /*  8:                     LT shift  108 */
  {  68,   0, 148}, /*  9:                     IS shift  148 */
  {  69,   0, 147}, /* 10:                 ISNULL shift  147 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  { 102,   4, 126}, /* 13:                 RSHIFT shift  126 */
  {  73,   5, 112}, /* 14:                     LE shift  112 */
  {  74,   0, 133}, /* 15:                   LIKE shift  133 */
  {  51,   0, 114}, /* 16:                     GE shift  114 */
  { 106,   0, 141}, /* 17:                  SLASH shift  141 */
  {  77,   0, 124}, /* 18:                 LSHIFT shift  124 */
  { 108,   8, 139}, /* 19:                   STAR shift  139 */
  {  82,  26, 130}, /* 20:                    NOT shift  130 */
  {  80,   0, 137}, /* 21:                  MINUS shift  137 */
  {  81,  16, 116}, /* 22:                     NE shift  116 */
  { 172,  20, 128}, /* 23:                 likeop shift  128 */
  {  83,   0, 152}, /* 24:                NOTNULL shift  152 */
  {  54,  27, 110}, /* 25:                     GT shift  110 */
  {  52,   0, 134}, /* 26:                   GLOB shift  134 */
  {  24,   0, 145}, /* 27:                 CONCAT shift  145 */
  {  87,   0,  10}, /* 28:                     ON shift  10 */
  {  88,   0, 106}, /* 29:                     OR shift  106 */
  {  89,   0,  99}, /* 30:      ORACLE_OUTER_JOIN shift  99 */
/* State 365 */
  {  72,   0, 366}, /*  1:                    KEY shift  366 */
/* State 366 */
  {  76,   0, 367}, /*  1:                     LP shift  367 */
/* State 367 */
  { 162,   0, 368}, /*  1:                idxlist shift  368 */
  {  56,   0,  20}, /*  2:                     ID shift  20 */
  { 110,   2,  21}, /*  3:                 STRING shift  21 */
  { 177,   0, 262}, /*  4:                     nm shift  262 */
  {  71,   0,  22}, /*  5:                JOIN_KW shift  22 */
  { 161,   5, 263}, /*  6:                idxitem shift  263 */
/* State 368 */
  {  21,   0, 260}, /*  1:                  COMMA shift  260 */
  { 101,   1, 369}, /*  2:                     RP shift  369 */
/* State 369 */
  {  95,   0, 370}, /*  1:             REFERENCES shift  370 */
/* State 370 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 371}, /*  2:                     nm shift  371 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 371 */
  {  76,   0, 310}, /*  1:                     LP shift  310 */
  { 163,   0, 372}, /*  2:            idxlist_opt shift  372 */
/* State 372 */
  { 188,   0, 373}, /*  1:                refargs shift  373 */
/* State 373 */
  { 147,   0, 375}, /*  1:    defer_subclause_opt shift  375 */
  {  30,   0, 316}, /*  2:             DEFERRABLE shift  316 */
  {  79,   2, 296}, /*  3:                  MATCH shift  296 */
  {  87,   0, 298}, /*  4:                     ON shift  298 */
  {  82,   0, 374}, /*  5:                    NOT shift  374 */
  { 187,   5, 295}, /*  6:                 refarg shift  295 */
  { 146,   0, 376}, /*  7:        defer_subclause shift  376 */
/* State 374 */
  {  30,   0,  49}, /*  1:             DEFERRABLE shift  49 */
/* State 380 */
  { 192,   0, 381}, /*  1:                 select shift  381 */
  { 181,   3,  69}, /*  2:              oneselect shift  69 */
  { 103,   0,  73}, /*  3:                 SELECT shift  73 */
/* State 381 */
  {  66,   0, 162}, /*  1:              INTERSECT shift  162 */
  { 176,   1,  71}, /*  2:         multiselect_op shift  71 */
  {  43,   0, 163}, /*  3:                 EXCEPT shift  163 */
  { 118,   3, 160}, /*  4:                  UNION shift  160 */
  { 104,   0, 551}, /*  5:                   SEMI reduce 20 */
/* State 382 */
  {  61,   0, 762}, /*  1:                  INDEX reduce 231 */
  { 115,   1, 401}, /*  2:                TRIGGER shift  401 */
  { 119,   0, 400}, /*  3:                 UNIQUE shift  400 */
  { 201,   0, 383}, /*  4:                   temp shift  383 */
  { 112,   0, 390}, /*  5:                   TEMP shift  390 */
  { 209,   3, 391}, /*  6:             uniqueflag shift  391 */
/* State 383 */
  { 111,   0, 384}, /*  1:                  TABLE shift  384 */
  { 125,   1, 386}, /*  2:                   VIEW shift  386 */
/* State 384 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 385}, /*  2:                     nm shift  385 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 386 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 387}, /*  2:                     nm shift  387 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 387 */
  {   6,   0, 388}, /*  1:                     AS shift  388 */
/* State 388 */
  { 192,   0, 389}, /*  1:                 select shift  389 */
  { 181,   3,  69}, /*  2:              oneselect shift  69 */
  { 103,   0,  73}, /*  3:                 SELECT shift  73 */
/* State 389 */
  {  66,   0, 162}, /*  1:              INTERSECT shift  162 */
  { 176,   1,  71}, /*  2:         multiselect_op shift  71 */
  {  43,   0, 163}, /*  3:                 EXCEPT shift  163 */
  { 118,   3, 160}, /*  4:                  UNION shift  160 */
  { 104,   0, 629}, /*  5:                   SEMI reduce 98 */
/* State 391 */
  {  61,   0, 392}, /*  1:                  INDEX shift  392 */
/* State 392 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 393}, /*  2:                     nm shift  393 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 393 */
  {  87,   0, 394}, /*  1:                     ON shift  394 */
/* State 394 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 395}, /*  2:                     nm shift  395 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 395 */
  {  76,   0, 396}, /*  1:                     LP shift  396 */
/* State 396 */
  { 162,   0, 397}, /*  1:                idxlist shift  397 */
  {  56,   0,  20}, /*  2:                     ID shift  20 */
  { 110,   2,  21}, /*  3:                 STRING shift  21 */
  { 177,   0, 262}, /*  4:                     nm shift  262 */
  {  71,   0,  22}, /*  5:                JOIN_KW shift  22 */
  { 161,   5, 263}, /*  6:                idxitem shift  263 */
/* State 397 */
  {  21,   0, 260}, /*  1:                  COMMA shift  260 */
  { 101,   1, 398}, /*  2:                     RP shift  398 */
/* State 398 */
  { 180,   2, 399}, /*  1:                 onconf shift  399 */
  {  87,   0,  10}, /*  2:                     ON shift  10 */
  { 104,   0, 619}, /*  3:                   SEMI reduce 88 */
/* State 399 */
  { 104,   0, 760}, /*  1:                   SEMI reduce 229 */
/* State 400 */
  {  61,   0, 761}, /*  1:                  INDEX reduce 230 */
/* State 401 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 402}, /*  2:                     nm shift  402 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 402 */
  {  64,   2, 467}, /*  1:                INSTEAD shift  467 */
  {   8,   0, 465}, /*  2:                 BEFORE shift  465 */
  { 206,   4, 403}, /*  3:           trigger_time shift  403 */
  {   2,   0, 466}, /*  4:                  AFTER shift  466 */
/* State 403 */
  { 120,   3, 462}, /*  1:                 UPDATE shift  462 */
  { 205,   0, 404}, /*  2:          trigger_event shift  404 */
  {  32,   0, 460}, /*  3:                 DELETE shift  460 */
  {  63,   0, 461}, /*  4:                 INSERT shift  461 */
/* State 404 */
  {  87,   0, 405}, /*  1:                     ON shift  405 */
/* State 405 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 406}, /*  2:                     nm shift  406 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 406 */
  {  47,   0, 456}, /*  1:                    FOR shift  456 */
  { 155,   1, 407}, /*  2:         foreach_clause shift  407 */
/* State 407 */
  { 126,   3, 454}, /*  1:                   WHEN shift  454 */
  { 211,   0, 408}, /*  2:            when_clause shift  408 */
  {   9,   0, 797}, /*  3:                  BEGIN reduce 266 */
/* State 408 */
  {   9,   0, 409}, /*  1:                  BEGIN shift  409 */
/* State 409 */
  {  63,   0, 431}, /*  1:                 INSERT shift  431 */
  { 181,   0,  69}, /*  2:              oneselect shift  69 */
  { 120,   0, 416}, /*  3:                 UPDATE shift  416 */
  { 192,   3, 410}, /*  4:                 select shift  410 */
  { 103,   8,  73}, /*  5:                 SELECT shift  73 */
  { 203,   9, 413}, /*  6:            trigger_cmd shift  413 */
  { 204,   0, 411}, /*  7:       trigger_cmd_list shift  411 */
  {  40,   0, 800}, /*  8:                    END reduce 269 */
  {  32,   0, 450}, /*  9:                 DELETE shift  450 */
/* State 410 */
  {  66,   0, 162}, /*  1:              INTERSECT shift  162 */
  { 176,   1,  71}, /*  2:         multiselect_op shift  71 */
  {  43,   0, 163}, /*  3:                 EXCEPT shift  163 */
  { 118,   3, 160}, /*  4:                  UNION shift  160 */
  { 104,   0, 805}, /*  5:                   SEMI reduce 274 */
/* State 411 */
  {  40,   0, 412}, /*  1:                    END shift  412 */
/* State 412 */
  { 104,   0, 785}, /*  1:                   SEMI reduce 254 */
/* State 413 */
  { 104,   0, 414}, /*  1:                   SEMI shift  414 */
/* State 414 */
  {  63,   0, 431}, /*  1:                 INSERT shift  431 */
  { 181,   0,  69}, /*  2:              oneselect shift  69 */
  { 120,   0, 416}, /*  3:                 UPDATE shift  416 */
  { 192,   3, 410}, /*  4:                 select shift  410 */
  { 103,   8,  73}, /*  5:                 SELECT shift  73 */
  { 203,   9, 413}, /*  6:            trigger_cmd shift  413 */
  { 204,   0, 415}, /*  7:       trigger_cmd_list shift  415 */
  {  40,   0, 800}, /*  8:                    END reduce 269 */
  {  32,   0, 450}, /*  9:                 DELETE shift  450 */
/* State 415 */
  {  40,   0, 799}, /*  1:                    END reduce 268 */
/* State 416 */
  { 182,   2, 419}, /*  1:                 orconf shift  419 */
  {  88,   0, 417}, /*  2:                     OR shift  417 */
/* State 417 */
  {   1,   0,  14}, /*  1:                  ABORT shift  14 */
  {  97,   1,  17}, /*  2:                REPLACE shift  17 */
  {  99,   5,  13}, /*  3:               ROLLBACK shift  13 */
  { 189,   3, 418}, /*  4:            resolvetype shift  418 */
  {  57,   6,  16}, /*  5:                 IGNORE shift  16 */
  {  45,   0,  15}, /*  6:                   FAIL shift  15 */
/* State 419 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 420}, /*  2:                     nm shift  420 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 420 */
  { 105,   0, 421}, /*  1:                    SET shift  421 */
/* State 421 */
  { 110,   0,  21}, /*  1:                 STRING shift  21 */
  {  71,   4,  22}, /*  2:                JOIN_KW shift  22 */
  { 177,   0, 428}, /*  3:                     nm shift  428 */
  {  56,   0,  20}, /*  4:                     ID shift  20 */
  { 194,   0, 422}, /*  5:                setlist shift  422 */
/* State 422 */
  { 212,   3, 427}, /*  1:              where_opt shift  427 */
  {  21,   0, 423}, /*  2:                  COMMA shift  423 */
  { 104,   0, 685}, /*  3:                   SEMI reduce 154 */
  { 127,   0, 232}, /*  4:                  WHERE shift  232 */
/* State 423 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 424}, /*  2:                     nm shift  424 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 424 */
  {  42,   0, 425}, /*  1:                     EQ shift  425 */
/* State 425 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 426}, /*  3:                   expr shift  426 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 426 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 427 */
  { 104,   0, 801}, /*  1:                   SEMI reduce 270 */
/* State 428 */
  {  42,   0, 429}, /*  1:                     EQ shift  429 */
/* State 429 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 430}, /*  3:                   expr shift  430 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 430 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 431 */
  {  67,   0, 621}, /*  1:                   INTO reduce 90 */
  {  88,   1, 417}, /*  2:                     OR shift  417 */
  { 182,   0, 432}, /*  3:                 orconf shift  432 */
/* State 432 */
  {  67,   0, 433}, /*  1:                   INTO shift  433 */
/* State 433 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 434}, /*  2:                     nm shift  434 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 434 */
  {  76,   0, 435}, /*  1:                     LP shift  435 */
  { 167,   0, 441}, /*  2:         inscollist_opt shift  441 */
/* State 435 */
  { 110,   0,  21}, /*  1:                 STRING shift  21 */
  { 166,   4, 436}, /*  2:             inscollist shift  436 */
  { 177,   0, 440}, /*  3:                     nm shift  440 */
  {  71,   5,  22}, /*  4:                JOIN_KW shift  22 */
  {  56,   0,  20}, /*  5:                     ID shift  20 */
/* State 436 */
  {  21,   0, 438}, /*  1:                  COMMA shift  438 */
  { 101,   1, 437}, /*  2:                     RP shift  437 */
/* State 438 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 439}, /*  2:                     nm shift  439 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 441 */
  { 192,   3, 442}, /*  1:                 select shift  442 */
  { 181,   0,  69}, /*  2:              oneselect shift  69 */
  { 124,   0, 443}, /*  3:                 VALUES shift  443 */
  { 103,   0,  73}, /*  4:                 SELECT shift  73 */
/* State 442 */
  {  66,   0, 162}, /*  1:              INTERSECT shift  162 */
  { 176,   1,  71}, /*  2:         multiselect_op shift  71 */
  {  43,   0, 163}, /*  3:                 EXCEPT shift  163 */
  { 118,   3, 160}, /*  4:                  UNION shift  160 */
  { 104,   0, 803}, /*  5:                   SEMI reduce 272 */
/* State 443 */
  {  76,   0, 444}, /*  1:                     LP shift  444 */
/* State 444 */
  {  80,   4, 172}, /*  1:                  MINUS shift  172 */
  { 177,   6, 102}, /*  2:                     nm shift  102 */
  {  82,   0, 168}, /*  3:                    NOT shift  168 */
  {  16,   0, 176}, /*  4:                   CASE shift  176 */
  {  84,   0, 101}, /*  5:                   NULL shift  101 */
  {  65,   0, 166}, /*  6:                INTEGER shift  166 */
  {  56,   0,  64}, /*  7:                     ID shift  64 */
  {  71,   0,  67}, /*  8:                JOIN_KW shift  67 */
  { 152,   7, 449}, /*  9:                   expr shift  449 */
  { 169,   0, 445}, /* 10:               itemlist shift  445 */
  {  12,   0, 170}, /* 11:                 BITNOT shift  170 */
  {  91,   0, 174}, /* 12:                   PLUS shift  174 */
  {  76,  11,  68}, /* 13:                     LP shift  68 */
  {  94,  16, 186}, /* 14:                  RAISE shift  186 */
  { 110,  14,  66}, /* 15:                 STRING shift  66 */
  {  46,   0, 167}, /* 16:                  FLOAT shift  167 */
/* State 445 */
  {  21,   0, 446}, /*  1:                  COMMA shift  446 */
  { 101,   1, 448}, /*  2:                     RP shift  448 */
/* State 446 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 447}, /*  3:                   expr shift  447 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 447 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 448 */
  { 104,   0, 802}, /*  1:                   SEMI reduce 271 */
/* State 449 */
  {  88,   2, 106}, /*  1:                     OR shift  106 */
  {  60,   0, 157}, /*  2:                     IN shift  157 */
  {   5,   0, 100}, /*  3:                    AND shift  100 */
  {  68,   0, 148}, /*  4:                     IS shift  148 */
  { 172,   1, 128}, /*  5:                 likeop shift  128 */
  {  89,   3,  99}, /*  6:      ORACLE_OUTER_JOIN shift  99 */
  {  13,   0, 122}, /*  7:                  BITOR shift  122 */
  {  91,   0, 135}, /*  8:                   PLUS shift  135 */
  {  74,   0, 133}, /*  9:                   LIKE shift  133 */
  {  78,   0, 108}, /* 10:                     LT shift  108 */
  {  10,   0, 153}, /* 11:                BETWEEN shift  153 */
  {  11,   0, 120}, /* 12:                 BITAND shift  120 */
  {  96,   4, 143}, /* 13:                    REM shift  143 */
  {  69,   7, 147}, /* 14:                 ISNULL shift  147 */
  {  42,   0, 118}, /* 15:                     EQ shift  118 */
  {  80,  17, 137}, /* 16:                  MINUS shift  137 */
  {  52,  20, 134}, /* 17:                   GLOB shift  134 */
  {  73,   0, 112}, /* 18:                     LE shift  112 */
  { 102,   9, 126}, /* 19:                 RSHIFT shift  126 */
  {  24,   0, 145}, /* 20:                 CONCAT shift  145 */
  {  54,   0, 110}, /* 21:                     GT shift  110 */
  {  77,   0, 124}, /* 22:                 LSHIFT shift  124 */
  { 106,  10, 141}, /* 23:                  SLASH shift  141 */
  {  51,   0, 114}, /* 24:                     GE shift  114 */
  { 108,  16, 139}, /* 25:                   STAR shift  139 */
  {  81,   0, 116}, /* 26:                     NE shift  116 */
  {  82,  21, 130}, /* 27:                    NOT shift  130 */
  {  83,   0, 152}, /* 28:                NOTNULL shift  152 */
/* State 450 */
  {  49,   0, 451}, /*  1:                   FROM shift  451 */
/* State 451 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 452}, /*  2:                     nm shift  452 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 452 */
  { 104,   0, 685}, /*  1:                   SEMI reduce 154 */
  { 127,   0, 232}, /*  2:                  WHERE shift  232 */
  { 212,   1, 453}, /*  3:              where_opt shift  453 */
/* State 453 */
  { 104,   0, 804}, /*  1:                   SEMI reduce 273 */
/* State 454 */
  {  76,   4,  68}, /*  1:                     LP shift  68 */
  {  91,   1, 174}, /*  2:                   PLUS shift  174 */
  { 152,   0, 455}, /*  3:                   expr shift  455 */
  {  46,   7, 167}, /*  4:                  FLOAT shift  167 */
  {  94,   0, 186}, /*  5:                  RAISE shift  186 */
  { 110,   9,  66}, /*  6:                 STRING shift  66 */
  {  16,   0, 176}, /*  7:                   CASE shift  176 */
  {  82,   0, 168}, /*  8:                    NOT shift  168 */
  {  80,  11, 172}, /*  9:                  MINUS shift  172 */
  {  84,   0, 101}, /* 10:                   NULL shift  101 */
  {  65,   0, 166}, /* 11:                INTEGER shift  166 */
  {  71,  14,  67}, /* 12:                JOIN_KW shift  67 */
  { 177,  15, 102}, /* 13:                     nm shift  102 */
  {  56,   0,  64}, /* 14:                     ID shift  64 */
  {  12,   0, 170}, /* 15:                 BITNOT shift  170 */
/* State 455 */
  {  60,   0, 157}, /*  1:                     IN shift  157 */
  {  88,   0, 106}, /*  2:                     OR shift  106 */
  {  89,   1,  99}, /*  3:      ORACLE_OUTER_JOIN shift  99 */
  {   9,   0, 798}, /*  4:                  BEGIN reduce 267 */
  {  91,   0, 135}, /*  5:                   PLUS shift  135 */
  {   5,   0, 100}, /*  6:                    AND shift  100 */
  {  10,   0, 153}, /*  7:                BETWEEN shift  153 */
  {  11,   0, 120}, /*  8:                 BITAND shift  120 */
  {  13,   0, 122}, /*  9:                  BITOR shift  122 */
  {  96,   4, 143}, /* 10:                    REM shift  143 */
  {  68,   7, 148}, /* 11:                     IS shift  148 */
  {  69,   8, 147}, /* 12:                 ISNULL shift  147 */
  {  73,   0, 112}, /* 13:                     LE shift  112 */
  {  42,   9, 118}, /* 14:                     EQ shift  118 */
  {  77,   0, 124}, /* 15:                 LSHIFT shift  124 */
  { 102,  13, 126}, /* 16:                 RSHIFT shift  126 */
  {  74,   0, 133}, /* 17:                   LIKE shift  133 */
  {  51,   0, 114}, /* 18:                     GE shift  114 */
  {  52,   0, 134}, /* 19:                   GLOB shift  134 */
  { 106,  15, 141}, /* 20:                  SLASH shift  141 */
  {  78,   0, 108}, /* 21:                     LT shift  108 */
  { 108,   0, 139}, /* 22:                   STAR shift  139 */
  {  80,  18, 137}, /* 23:                  MINUS shift  137 */
  {  81,  19, 116}, /* 24:                     NE shift  116 */
  {  82,  27, 130}, /* 25:                    NOT shift  130 */
  {  83,  29, 152}, /* 26:                NOTNULL shift  152 */
  {  24,   0, 145}, /* 27:                 CONCAT shift  145 */
  { 172,   0, 128}, /* 28:                 likeop shift  128 */
  {  54,   0, 110}, /* 29:                     GT shift  110 */
/* State 456 */
  {  38,   0, 457}, /*  1:                   EACH shift  457 */
/* State 457 */
  { 100,   0, 458}, /*  1:                    ROW shift  458 */
  { 109,   0, 459}, /*  2:              STATEMENT shift  459 */
/* State 460 */
  {  87,   0, 790}, /*  1:                     ON reduce 259 */
/* State 461 */
  {  87,   0, 791}, /*  1:                     ON reduce 260 */
/* State 462 */
  {  85,   0, 463}, /*  1:                     OF shift  463 */
  {  87,   1, 792}, /*  2:                     ON reduce 261 */
/* State 463 */
  { 110,   0,  21}, /*  1:                 STRING shift  21 */
  { 166,   4, 464}, /*  2:             inscollist shift  464 */
  { 177,   0, 440}, /*  3:                     nm shift  440 */
  {  71,   5,  22}, /*  4:                JOIN_KW shift  22 */
  {  56,   0,  20}, /*  5:                     ID shift  20 */
/* State 464 */
  {  21,   0, 438}, /*  1:                  COMMA shift  438 */
  {  87,   1, 793}, /*  2:                     ON reduce 262 */
/* State 467 */
  {  85,   0, 468}, /*  1:                     OF shift  468 */
/* State 469 */
  {  61,   0, 474}, /*  1:                  INDEX shift  474 */
  { 125,   1, 472}, /*  2:                   VIEW shift  472 */
  { 111,   0, 470}, /*  3:                  TABLE shift  470 */
  { 115,   3, 476}, /*  4:                TRIGGER shift  476 */
/* State 470 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 471}, /*  2:                     nm shift  471 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 471 */
  { 104,   0, 628}, /*  1:                   SEMI reduce 97 */
/* State 472 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 473}, /*  2:                     nm shift  473 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 473 */
  { 104,   0, 630}, /*  1:                   SEMI reduce 99 */
/* State 474 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 475}, /*  2:                     nm shift  475 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 475 */
  { 104,   0, 768}, /*  1:                   SEMI reduce 237 */
/* State 476 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 477}, /*  2:                     nm shift  477 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 477 */
  { 104,   0, 810}, /*  1:                   SEMI reduce 279 */
/* State 478 */
  {  66,   0, 162}, /*  1:              INTERSECT shift  162 */
  { 176,   1,  71}, /*  2:         multiselect_op shift  71 */
  {  43,   0, 163}, /*  3:                 EXCEPT shift  163 */
  { 118,   3, 160}, /*  4:                  UNION shift  160 */
  { 104,   0, 631}, /*  5:                   SEMI reduce 100 */
/* State 479 */
  {  49,   0, 480}, /*  1:                   FROM shift  480 */
/* State 480 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 481}, /*  2:                     nm shift  481 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 481 */
  { 104,   0, 685}, /*  1:                   SEMI reduce 154 */
  { 127,   0, 232}, /*  2:                  WHERE shift  232 */
  { 212,   1, 482}, /*  3:              where_opt shift  482 */
/* State 482 */
  { 104,   0, 684}, /*  1:                   SEMI reduce 153 */
/* State 483 */
  { 182,   2, 484}, /*  1:                 orconf shift  484 */
  {  88,   0, 417}, /*  2:                     OR shift  417 */
/* State 484 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 485}, /*  2:                     nm shift  485 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 485 */
  { 105,   0, 486}, /*  1:                    SET shift  486 */
/* State 486 */
  { 110,   0,  21}, /*  1:                 STRING shift  21 */
  {  71,   4,  22}, /*  2:                JOIN_KW shift  22 */
  { 177,   0, 428}, /*  3:                     nm shift  428 */
  {  56,   0,  20}, /*  4:                     ID shift  20 */
  { 194,   0, 487}, /*  5:                setlist shift  487 */
/* State 487 */
  { 212,   3, 488}, /*  1:              where_opt shift  488 */
  {  21,   0, 423}, /*  2:                  COMMA shift  423 */
  { 104,   0, 685}, /*  3:                   SEMI reduce 154 */
  { 127,   0, 232}, /*  4:                  WHERE shift  232 */
/* State 488 */
  { 104,   0, 687}, /*  1:                   SEMI reduce 156 */
/* State 489 */
  {  67,   0, 490}, /*  1:                   INTO shift  490 */
/* State 490 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 491}, /*  2:                     nm shift  491 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 491 */
  {  76,   0, 435}, /*  1:                     LP shift  435 */
  { 167,   0, 492}, /*  2:         inscollist_opt shift  492 */
/* State 492 */
  { 192,   3, 493}, /*  1:                 select shift  493 */
  { 181,   0,  69}, /*  2:              oneselect shift  69 */
  { 124,   0, 494}, /*  3:                 VALUES shift  494 */
  { 103,   0,  73}, /*  4:                 SELECT shift  73 */
/* State 493 */
  {  66,   0, 162}, /*  1:              INTERSECT shift  162 */
  { 176,   1,  71}, /*  2:         multiselect_op shift  71 */
  {  43,   0, 163}, /*  3:                 EXCEPT shift  163 */
  { 118,   3, 160}, /*  4:                  UNION shift  160 */
  { 104,   0, 691}, /*  5:                   SEMI reduce 160 */
/* State 494 */
  {  76,   0, 495}, /*  1:                     LP shift  495 */
/* State 495 */
  {  80,   4, 172}, /*  1:                  MINUS shift  172 */
  { 177,   6, 102}, /*  2:                     nm shift  102 */
  {  82,   0, 168}, /*  3:                    NOT shift  168 */
  {  16,   0, 176}, /*  4:                   CASE shift  176 */
  {  84,   0, 101}, /*  5:                   NULL shift  101 */
  {  65,   0, 166}, /*  6:                INTEGER shift  166 */
  {  56,   0,  64}, /*  7:                     ID shift  64 */
  {  71,   0,  67}, /*  8:                JOIN_KW shift  67 */
  { 152,   7, 449}, /*  9:                   expr shift  449 */
  { 169,   0, 496}, /* 10:               itemlist shift  496 */
  {  12,   0, 170}, /* 11:                 BITNOT shift  170 */
  {  91,   0, 174}, /* 12:                   PLUS shift  174 */
  {  76,  11,  68}, /* 13:                     LP shift  68 */
  {  94,  16, 186}, /* 14:                  RAISE shift  186 */
  { 110,  14,  66}, /* 15:                 STRING shift  66 */
  {  46,   0, 167}, /* 16:                  FLOAT shift  167 */
/* State 496 */
  {  21,   0, 446}, /*  1:                  COMMA shift  446 */
  { 101,   1, 497}, /*  2:                     RP shift  497 */
/* State 497 */
  { 104,   0, 690}, /*  1:                   SEMI reduce 159 */
/* State 498 */
  {  67,   0, 621}, /*  1:                   INTO reduce 90 */
  {  88,   1, 417}, /*  2:                     OR shift  417 */
  { 182,   0, 499}, /*  3:                 orconf shift  499 */
/* State 499 */
  {  67,   0, 692}, /*  1:                   INTO reduce 161 */
/* State 500 */
  {  67,   0, 693}, /*  1:                   INTO reduce 162 */
/* State 501 */
  { 182,   2, 502}, /*  1:                 orconf shift  502 */
  {  88,   0, 417}, /*  2:                     OR shift  417 */
/* State 502 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 503}, /*  2:                     nm shift  503 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 503 */
  {  49,   0, 504}, /*  1:                   FROM shift  504 */
/* State 504 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 505}, /*  2:                     nm shift  505 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 505 */
  { 122,   2, 506}, /*  1:                  USING shift  506 */
  { 104,   0, 770}, /*  2:                   SEMI reduce 239 */
/* State 506 */
  {  33,   0, 507}, /*  1:             DELIMITERS shift  507 */
/* State 507 */
  { 110,   0, 508}, /*  1:                 STRING shift  508 */
/* State 508 */
  { 104,   0, 769}, /*  1:                   SEMI reduce 238 */
/* State 509 */
  { 110,   0,  21}, /*  1:                 STRING shift  21 */
  {  71,   4,  22}, /*  2:                JOIN_KW shift  22 */
  { 177,   0, 510}, /*  3:                     nm shift  510 */
  {  56,   0,  20}, /*  4:                     ID shift  20 */
  { 104,   0, 771}, /*  5:                   SEMI reduce 240 */
/* State 510 */
  { 104,   0, 772}, /*  1:                   SEMI reduce 241 */
/* State 511 */
  {  56,   0, 248}, /*  1:                     ID shift  248 */
  { 160,   0, 512}, /*  2:                    ids shift  512 */
  { 110,   1, 249}, /*  3:                 STRING shift  249 */
/* State 512 */
  {  42,   0, 513}, /*  1:                     EQ shift  513 */
  {  76,   0, 525}, /*  2:                     LP shift  525 */
  { 104,   0, 778}, /*  3:                   SEMI reduce 247 */
/* State 513 */
  { 110,   3,  21}, /*  1:                 STRING shift  21 */
  {  91,   4, 524}, /*  2:                   PLUS shift  524 */
  {  80,   0, 522}, /*  3:                  MINUS shift  522 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
  { 184,   0, 516}, /*  5:               plus_num shift  516 */
  { 185,   9, 518}, /*  6:               plus_opt shift  518 */
  {  56,   0,  20}, /*  7:                     ID shift  20 */
  { 177,  10, 514}, /*  8:                     nm shift  514 */
  { 175,   0, 517}, /*  9:              minus_num shift  517 */
  {  87,   0, 515}, /* 10:                     ON shift  515 */
/* State 514 */
  { 104,   0, 773}, /*  1:                   SEMI reduce 242 */
/* State 515 */
  { 104,   0, 774}, /*  1:                   SEMI reduce 243 */
/* State 516 */
  { 104,   0, 775}, /*  1:                   SEMI reduce 244 */
/* State 517 */
  { 104,   0, 776}, /*  1:                   SEMI reduce 245 */
/* State 518 */
  {  46,   0, 521}, /*  1:                  FLOAT shift  521 */
  { 178,   1, 519}, /*  2:                 number shift  519 */
  {  65,   0, 520}, /*  3:                INTEGER shift  520 */
/* State 519 */
  { 104,   0, 779}, /*  1:                   SEMI reduce 248 */
/* State 520 */
  { 104,   0, 781}, /*  1:                   SEMI reduce 250 */
/* State 521 */
  { 104,   0, 782}, /*  1:                   SEMI reduce 251 */
/* State 522 */
  {  46,   0, 521}, /*  1:                  FLOAT shift  521 */
  { 178,   1, 523}, /*  2:                 number shift  523 */
  {  65,   0, 520}, /*  3:                INTEGER shift  520 */
/* State 523 */
  { 104,   0, 780}, /*  1:                   SEMI reduce 249 */
/* State 525 */
  {  56,   0,  20}, /*  1:                     ID shift  20 */
  { 177,   0, 526}, /*  2:                     nm shift  526 */
  { 110,   0,  21}, /*  3:                 STRING shift  21 */
  {  71,   0,  22}, /*  4:                JOIN_KW shift  22 */
/* State 526 */
  { 101,   0, 527}, /*  1:                     RP shift  527 */
/* State 527 */
  { 104,   0, 777}, /*  1:                   SEMI reduce 246 */
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
  { &yyActionTable[0],   6, 538 },
  { &yyActionTable[6],   5, 538 },
  { &yyActionTable[11],   0, 533 },
  { &yyActionTable[11],  20, 811 },
  { &yyActionTable[31],   1, 811 },
  { &yyActionTable[32],   0, 534 },
  { &yyActionTable[32],   1, 811 },
  { &yyActionTable[33],   2, 540 },
  { &yyActionTable[35],   3, 811 },
  { &yyActionTable[38],   1, 811 },
  { &yyActionTable[39],   1, 811 },
  { &yyActionTable[40],   6, 811 },
  { &yyActionTable[46],   0, 620 },
  { &yyActionTable[46],   0, 623 },
  { &yyActionTable[46],   0, 624 },
  { &yyActionTable[46],   0, 625 },
  { &yyActionTable[46],   0, 626 },
  { &yyActionTable[46],   0, 627 },
  { &yyActionTable[46],   4, 541 },
  { &yyActionTable[50],   0, 542 },
  { &yyActionTable[50],   0, 559 },
  { &yyActionTable[50],   0, 560 },
  { &yyActionTable[50],   0, 561 },
  { &yyActionTable[50],   3, 811 },
  { &yyActionTable[53],   1, 811 },
  { &yyActionTable[54],   3, 811 },
  { &yyActionTable[57],   1, 811 },
  { &yyActionTable[58],   3, 811 },
  { &yyActionTable[61],   1, 811 },
  { &yyActionTable[62],   3, 811 },
  { &yyActionTable[65],   1, 811 },
  { &yyActionTable[66],   7, 811 },
  { &yyActionTable[73],   3, 811 },
  { &yyActionTable[76],   1, 811 },
  { &yyActionTable[77],   1, 811 },
  { &yyActionTable[78],  13, 811 },
  { &yyActionTable[91],   0, 552 },
  { &yyActionTable[91],   5, 562 },
  { &yyActionTable[96],   1, 572 },
  { &yyActionTable[97],  13, 554 },
  { &yyActionTable[110],   0, 571 },
  { &yyActionTable[110],   4, 811 },
  { &yyActionTable[114],  10, 811 },
  { &yyActionTable[124],   0, 573 },
  { &yyActionTable[124],   2, 619 },
  { &yyActionTable[126],   0, 584 },
  { &yyActionTable[126],   2, 811 },
  { &yyActionTable[128],   2, 619 },
  { &yyActionTable[130],   0, 585 },
  { &yyActionTable[130],   2, 604 },
  { &yyActionTable[132],   0, 602 },
  { &yyActionTable[132],   2, 811 },
  { &yyActionTable[134],   0, 605 },
  { &yyActionTable[134],   0, 606 },
  { &yyActionTable[134],   1, 811 },
  { &yyActionTable[135],   3, 672 },
  { &yyActionTable[138],   2, 619 },
  { &yyActionTable[140],   0, 586 },
  { &yyActionTable[140],   0, 670 },
  { &yyActionTable[140],   0, 671 },
  { &yyActionTable[140],   2, 619 },
  { &yyActionTable[142],   0, 587 },
  { &yyActionTable[142],   1, 811 },
  { &yyActionTable[143],  15, 811 },
  { &yyActionTable[158],   2, 702 },
  { &yyActionTable[160],  18, 759 },
  { &yyActionTable[178],   1, 708 },
  { &yyActionTable[179],   1, 703 },
  { &yyActionTable[180],  18, 811 },
  { &yyActionTable[198],   0, 632 },
  { &yyActionTable[198],   5, 811 },
  { &yyActionTable[203],   2, 811 },
  { &yyActionTable[205],   0, 633 },
  { &yyActionTable[205],   3, 641 },
  { &yyActionTable[208],   2, 643 },
  { &yyActionTable[210],   3, 650 },
  { &yyActionTable[213],   2, 685 },
  { &yyActionTable[215],   2, 675 },
  { &yyActionTable[217],   2, 677 },
  { &yyActionTable[219],   2, 665 },
  { &yyActionTable[221],   2, 679 },
  { &yyActionTable[223],   0, 638 },
  { &yyActionTable[223],   1, 811 },
  { &yyActionTable[224],   3, 680 },
  { &yyActionTable[227],   1, 811 },
  { &yyActionTable[228],   0, 681 },
  { &yyActionTable[228],   1, 811 },
  { &yyActionTable[229],   1, 811 },
  { &yyActionTable[230],   1, 811 },
  { &yyActionTable[231],  17, 811 },
  { &yyActionTable[248],   1, 666 },
  { &yyActionTable[249],  16, 811 },
  { &yyActionTable[265],   2, 673 },
  { &yyActionTable[267],   3, 672 },
  { &yyActionTable[270],   0, 667 },
  { &yyActionTable[270],   2, 811 },
  { &yyActionTable[272],   0, 556 },
  { &yyActionTable[272],   0, 674 },
  { &yyActionTable[272],  28, 669 },
  { &yyActionTable[300],   0, 705 },
  { &yyActionTable[300],  15, 811 },
  { &yyActionTable[315],   0, 701 },
  { &yyActionTable[315],   1, 811 },
  { &yyActionTable[316],   4, 811 },
  { &yyActionTable[320],   0, 704 },
  { &yyActionTable[320],  26, 711 },
  { &yyActionTable[346],  15, 811 },
  { &yyActionTable[361],  27, 712 },
  { &yyActionTable[388],  15, 811 },
  { &yyActionTable[403],  12, 713 },
  { &yyActionTable[415],  15, 811 },
  { &yyActionTable[430],  12, 714 },
  { &yyActionTable[442],  15, 811 },
  { &yyActionTable[457],  12, 715 },
  { &yyActionTable[469],  15, 811 },
  { &yyActionTable[484],  12, 716 },
  { &yyActionTable[496],  15, 811 },
  { &yyActionTable[511],  16, 717 },
  { &yyActionTable[527],  15, 811 },
  { &yyActionTable[542],  16, 718 },
  { &yyActionTable[558],  15, 811 },
  { &yyActionTable[573],   8, 719 },
  { &yyActionTable[581],  15, 811 },
  { &yyActionTable[596],   8, 720 },
  { &yyActionTable[604],  15, 811 },
  { &yyActionTable[619],   8, 721 },
  { &yyActionTable[627],  15, 811 },
  { &yyActionTable[642],   8, 722 },
  { &yyActionTable[650],  15, 811 },
  { &yyActionTable[665],  16, 723 },
  { &yyActionTable[681],   6, 811 },
  { &yyActionTable[687],  15, 811 },
  { &yyActionTable[702],  16, 724 },
  { &yyActionTable[718],   0, 725 },
  { &yyActionTable[718],   0, 726 },
  { &yyActionTable[718],  15, 811 },
  { &yyActionTable[733],   6, 727 },
  { &yyActionTable[739],  15, 811 },
  { &yyActionTable[754],   6, 728 },
  { &yyActionTable[760],  15, 811 },
  { &yyActionTable[775],   3, 729 },
  { &yyActionTable[778],  15, 811 },
  { &yyActionTable[793],   3, 730 },
  { &yyActionTable[796],  15, 811 },
  { &yyActionTable[811],   3, 731 },
  { &yyActionTable[814],  15, 811 },
  { &yyActionTable[829],   2, 732 },
  { &yyActionTable[831],   0, 733 },
  { &yyActionTable[831],   2, 811 },
  { &yyActionTable[833],   0, 734 },
  { &yyActionTable[833],   1, 811 },
  { &yyActionTable[834],   0, 737 },
  { &yyActionTable[834],   0, 735 },
  { &yyActionTable[834],  15, 811 },
  { &yyActionTable[849],  28, 811 },
  { &yyActionTable[877],  15, 811 },
  { &yyActionTable[892],  16, 743 },
  { &yyActionTable[908],   1, 811 },
  { &yyActionTable[909],  20, 759 },
  { &yyActionTable[929],   5, 811 },
  { &yyActionTable[934],   2, 811 },
  { &yyActionTable[936],   1, 811 },
  { &yyActionTable[937],   1, 811 },
  { &yyActionTable[938],   1, 811 },
  { &yyActionTable[939],   0, 746 },
  { &yyActionTable[939],  28, 758 },
  { &yyActionTable[967],   0, 706 },
  { &yyActionTable[967],   0, 707 },
  { &yyActionTable[967],  15, 811 },
  { &yyActionTable[982],  26, 738 },
  { &yyActionTable[1008],  15, 811 },
  { &yyActionTable[1023],   2, 739 },
  { &yyActionTable[1025],  15, 811 },
  { &yyActionTable[1040],   2, 740 },
  { &yyActionTable[1042],  15, 811 },
  { &yyActionTable[1057],   2, 741 },
  { &yyActionTable[1059],  17, 811 },
  { &yyActionTable[1076],  29, 811 },
  { &yyActionTable[1105],   2, 811 },
  { &yyActionTable[1107],   4, 811 },
  { &yyActionTable[1111],   1, 811 },
  { &yyActionTable[1112],   0, 749 },
  { &yyActionTable[1112],  15, 811 },
  { &yyActionTable[1127],  29, 811 },
  { &yyActionTable[1156],  15, 811 },
  { &yyActionTable[1171],  28, 750 },
  { &yyActionTable[1199],   1, 811 },
  { &yyActionTable[1200],   4, 811 },
  { &yyActionTable[1204],   1, 811 },
  { &yyActionTable[1205],   0, 806 },
  { &yyActionTable[1205],   1, 811 },
  { &yyActionTable[1206],   4, 811 },
  { &yyActionTable[1210],   1, 811 },
  { &yyActionTable[1211],   0, 807 },
  { &yyActionTable[1211],   1, 811 },
  { &yyActionTable[1212],   4, 811 },
  { &yyActionTable[1216],   1, 811 },
  { &yyActionTable[1217],   0, 808 },
  { &yyActionTable[1217],   1, 811 },
  { &yyActionTable[1218],   4, 811 },
  { &yyActionTable[1222],   1, 811 },
  { &yyActionTable[1223],   0, 809 },
  { &yyActionTable[1223],  15, 811 },
  { &yyActionTable[1238],  29, 811 },
  { &yyActionTable[1267],  15, 811 },
  { &yyActionTable[1282],  29, 811 },
  { &yyActionTable[1311],  15, 811 },
  { &yyActionTable[1326],  28, 751 },
  { &yyActionTable[1354],   2, 811 },
  { &yyActionTable[1356],   0, 745 },
  { &yyActionTable[1356],  16, 759 },
  { &yyActionTable[1372],   0, 756 },
  { &yyActionTable[1372],   0, 757 },
  { &yyActionTable[1372],   0, 736 },
  { &yyActionTable[1372],  15, 811 },
  { &yyActionTable[1387],  28, 811 },
  { &yyActionTable[1415],  15, 811 },
  { &yyActionTable[1430],  26, 744 },
  { &yyActionTable[1456],   1, 811 },
  { &yyActionTable[1457],  20, 759 },
  { &yyActionTable[1477],   5, 811 },
  { &yyActionTable[1482],   0, 748 },
  { &yyActionTable[1482],   2, 811 },
  { &yyActionTable[1484],   0, 747 },
  { &yyActionTable[1484],   2, 673 },
  { &yyActionTable[1486],   3, 672 },
  { &yyActionTable[1489],   0, 668 },
  { &yyActionTable[1489],  15, 811 },
  { &yyActionTable[1504],  28, 678 },
  { &yyActionTable[1532],   1, 811 },
  { &yyActionTable[1533],  17, 759 },
  { &yyActionTable[1550],   1, 676 },
  { &yyActionTable[1551],  15, 811 },
  { &yyActionTable[1566],  28, 686 },
  { &yyActionTable[1594],   0, 642 },
  { &yyActionTable[1594],   2, 653 },
  { &yyActionTable[1596],   4, 651 },
  { &yyActionTable[1600],   0, 652 },
  { &yyActionTable[1600],   0, 656 },
  { &yyActionTable[1600],   0, 657 },
  { &yyActionTable[1600],   5, 811 },
  { &yyActionTable[1605],   0, 658 },
  { &yyActionTable[1605],   5, 811 },
  { &yyActionTable[1610],   0, 659 },
  { &yyActionTable[1610],   1, 811 },
  { &yyActionTable[1611],   0, 660 },
  { &yyActionTable[1611],   5, 811 },
  { &yyActionTable[1616],   5, 649 },
  { &yyActionTable[1621],   0, 557 },
  { &yyActionTable[1621],   0, 558 },
  { &yyActionTable[1621],   4, 811 },
  { &yyActionTable[1625],   0, 647 },
  { &yyActionTable[1625],   0, 648 },
  { &yyActionTable[1625],   2, 662 },
  { &yyActionTable[1627],   2, 664 },
  { &yyActionTable[1629],   0, 654 },
  { &yyActionTable[1629],   1, 811 },
  { &yyActionTable[1630],   6, 811 },
  { &yyActionTable[1636],   2, 811 },
  { &yyActionTable[1638],   0, 663 },
  { &yyActionTable[1638],   5, 811 },
  { &yyActionTable[1643],   0, 765 },
  { &yyActionTable[1643],   0, 767 },
  { &yyActionTable[1643],   0, 766 },
  { &yyActionTable[1643],  15, 811 },
  { &yyActionTable[1658],  28, 661 },
  { &yyActionTable[1686],   3, 811 },
  { &yyActionTable[1689],   5, 811 },
  { &yyActionTable[1694],   5, 649 },
  { &yyActionTable[1699],   2, 662 },
  { &yyActionTable[1701],   2, 664 },
  { &yyActionTable[1703],   0, 655 },
  { &yyActionTable[1703],  16, 811 },
  { &yyActionTable[1719],  33, 649 },
  { &yyActionTable[1752],   0, 644 },
  { &yyActionTable[1752],   0, 645 },
  { &yyActionTable[1752],   1, 811 },
  { &yyActionTable[1753],   5, 811 },
  { &yyActionTable[1758],   0, 646 },
  { &yyActionTable[1758],   0, 639 },
  { &yyActionTable[1758],   0, 640 },
  { &yyActionTable[1758],   0, 742 },
  { &yyActionTable[1758],  29, 811 },
  { &yyActionTable[1787],   0, 700 },
  { &yyActionTable[1787],   2, 811 },
  { &yyActionTable[1789],   0, 709 },
  { &yyActionTable[1789],   1, 811 },
  { &yyActionTable[1790],   0, 710 },
  { &yyActionTable[1790],  29, 811 },
  { &yyActionTable[1819],   2, 619 },
  { &yyActionTable[1821],   0, 588 },
  { &yyActionTable[1821],   4, 811 },
  { &yyActionTable[1825],   2, 763 },
  { &yyActionTable[1827],   1, 592 },
  { &yyActionTable[1828],   3, 589 },
  { &yyActionTable[1831],   0, 593 },
  { &yyActionTable[1831],   4, 811 },
  { &yyActionTable[1835],   0, 594 },
  { &yyActionTable[1835],   3, 811 },
  { &yyActionTable[1838],   4, 811 },
  { &yyActionTable[1842],   0, 595 },
  { &yyActionTable[1842],   2, 811 },
  { &yyActionTable[1844],   0, 598 },
  { &yyActionTable[1844],   0, 599 },
  { &yyActionTable[1844],   0, 600 },
  { &yyActionTable[1844],   0, 601 },
  { &yyActionTable[1844],   4, 811 },
  { &yyActionTable[1848],   0, 596 },
  { &yyActionTable[1848],   4, 811 },
  { &yyActionTable[1852],   0, 597 },
  { &yyActionTable[1852],   6, 811 },
  { &yyActionTable[1858],   2, 811 },
  { &yyActionTable[1860],   0, 764 },
  { &yyActionTable[1860],   0, 590 },
  { &yyActionTable[1860],   2, 811 },
  { &yyActionTable[1862],   0, 591 },
  { &yyActionTable[1862],   2, 604 },
  { &yyActionTable[1864],   0, 603 },
  { &yyActionTable[1864],   0, 574 },
  { &yyActionTable[1864],   7, 811 },
  { &yyActionTable[1871],   0, 575 },
  { &yyActionTable[1871],   0, 576 },
  { &yyActionTable[1871],   0, 577 },
  { &yyActionTable[1871],   2, 811 },
  { &yyActionTable[1873],   0, 578 },
  { &yyActionTable[1873],   0, 581 },
  { &yyActionTable[1873],   2, 811 },
  { &yyActionTable[1875],   0, 579 },
  { &yyActionTable[1875],   0, 582 },
  { &yyActionTable[1875],   0, 580 },
  { &yyActionTable[1875],   0, 583 },
  { &yyActionTable[1875],   4, 563 },
  { &yyActionTable[1879],   4, 811 },
  { &yyActionTable[1883],   2, 811 },
  { &yyActionTable[1885],   0, 564 },
  { &yyActionTable[1885],   4, 811 },
  { &yyActionTable[1889],   1, 811 },
  { &yyActionTable[1890],   0, 565 },
  { &yyActionTable[1890],   0, 568 },
  { &yyActionTable[1890],   1, 811 },
  { &yyActionTable[1891],   0, 569 },
  { &yyActionTable[1891],   1, 811 },
  { &yyActionTable[1892],   0, 570 },
  { &yyActionTable[1892],   0, 567 },
  { &yyActionTable[1892],   0, 566 },
  { &yyActionTable[1892],   0, 555 },
  { &yyActionTable[1892],   8, 811 },
  { &yyActionTable[1900],   6, 811 },
  { &yyActionTable[1906],   0, 609 },
  { &yyActionTable[1906],   4, 811 },
  { &yyActionTable[1910],   0, 612 },
  { &yyActionTable[1910],   1, 811 },
  { &yyActionTable[1911],   1, 811 },
  { &yyActionTable[1912],   6, 811 },
  { &yyActionTable[1918],   2, 811 },
  { &yyActionTable[1920],   2, 619 },
  { &yyActionTable[1922],   0, 613 },
  { &yyActionTable[1922],   1, 811 },
  { &yyActionTable[1923],   6, 811 },
  { &yyActionTable[1929],   2, 811 },
  { &yyActionTable[1931],   2, 619 },
  { &yyActionTable[1933],   0, 614 },
  { &yyActionTable[1933],  15, 811 },
  { &yyActionTable[1948],  30, 619 },
  { &yyActionTable[1978],   0, 615 },
  { &yyActionTable[1978],   1, 811 },
  { &yyActionTable[1979],   1, 811 },
  { &yyActionTable[1980],   6, 811 },
  { &yyActionTable[1986],   2, 811 },
  { &yyActionTable[1988],   1, 811 },
  { &yyActionTable[1989],   4, 811 },
  { &yyActionTable[1993],   2, 763 },
  { &yyActionTable[1995],   1, 592 },
  { &yyActionTable[1996],   7, 617 },
  { &yyActionTable[2003],   1, 811 },
  { &yyActionTable[2004],   0, 616 },
  { &yyActionTable[2004],   0, 618 },
  { &yyActionTable[2004],   0, 610 },
  { &yyActionTable[2004],   0, 611 },
  { &yyActionTable[2004],   0, 553 },
  { &yyActionTable[2004],   3, 811 },
  { &yyActionTable[2007],   5, 811 },
  { &yyActionTable[2012],   6, 549 },
  { &yyActionTable[2018],   2, 811 },
  { &yyActionTable[2020],   4, 811 },
  { &yyActionTable[2024],   0, 547 },
  { &yyActionTable[2024],   4, 811 },
  { &yyActionTable[2028],   1, 811 },
  { &yyActionTable[2029],   3, 811 },
  { &yyActionTable[2032],   5, 811 },
  { &yyActionTable[2037],   0, 548 },
  { &yyActionTable[2037],   1, 811 },
  { &yyActionTable[2038],   4, 811 },
  { &yyActionTable[2042],   1, 811 },
  { &yyActionTable[2043],   4, 811 },
  { &yyActionTable[2047],   1, 811 },
  { &yyActionTable[2048],   6, 811 },
  { &yyActionTable[2054],   2, 811 },
  { &yyActionTable[2056],   3, 811 },
  { &yyActionTable[2059],   1, 811 },
  { &yyActionTable[2060],   1, 811 },
  { &yyActionTable[2061],   4, 811 },
  { &yyActionTable[2065],   4, 789 },
  { &yyActionTable[2069],   4, 811 },
  { &yyActionTable[2073],   1, 811 },
  { &yyActionTable[2074],   4, 811 },
  { &yyActionTable[2078],   2, 794 },
  { &yyActionTable[2080],   3, 811 },
  { &yyActionTable[2083],   1, 811 },
  { &yyActionTable[2084],   9, 811 },
  { &yyActionTable[2093],   5, 811 },
  { &yyActionTable[2098],   1, 811 },
  { &yyActionTable[2099],   1, 811 },
  { &yyActionTable[2100],   1, 811 },
  { &yyActionTable[2101],   9, 811 },
  { &yyActionTable[2110],   1, 811 },
  { &yyActionTable[2111],   2, 621 },
  { &yyActionTable[2113],   6, 811 },
  { &yyActionTable[2119],   0, 622 },
  { &yyActionTable[2119],   4, 811 },
  { &yyActionTable[2123],   1, 811 },
  { &yyActionTable[2124],   5, 811 },
  { &yyActionTable[2129],   4, 811 },
  { &yyActionTable[2133],   4, 811 },
  { &yyActionTable[2137],   1, 811 },
  { &yyActionTable[2138],  15, 811 },
  { &yyActionTable[2153],  28, 688 },
  { &yyActionTable[2181],   1, 811 },
  { &yyActionTable[2182],   1, 811 },
  { &yyActionTable[2183],  15, 811 },
  { &yyActionTable[2198],  28, 689 },
  { &yyActionTable[2226],   3, 811 },
  { &yyActionTable[2229],   1, 811 },
  { &yyActionTable[2230],   4, 811 },
  { &yyActionTable[2234],   2, 696 },
  { &yyActionTable[2236],   5, 811 },
  { &yyActionTable[2241],   2, 811 },
  { &yyActionTable[2243],   0, 697 },
  { &yyActionTable[2243],   4, 811 },
  { &yyActionTable[2247],   0, 698 },
  { &yyActionTable[2247],   0, 699 },
  { &yyActionTable[2247],   4, 811 },
  { &yyActionTable[2251],   5, 811 },
  { &yyActionTable[2256],   1, 811 },
  { &yyActionTable[2257],  16, 811 },
  { &yyActionTable[2273],   2, 811 },
  { &yyActionTable[2275],  15, 811 },
  { &yyActionTable[2290],  28, 694 },
  { &yyActionTable[2318],   1, 811 },
  { &yyActionTable[2319],  28, 695 },
  { &yyActionTable[2347],   1, 811 },
  { &yyActionTable[2348],   4, 811 },
  { &yyActionTable[2352],   3, 811 },
  { &yyActionTable[2355],   1, 811 },
  { &yyActionTable[2356],  15, 811 },
  { &yyActionTable[2371],  29, 811 },
  { &yyActionTable[2400],   1, 811 },
  { &yyActionTable[2401],   2, 811 },
  { &yyActionTable[2403],   0, 795 },
  { &yyActionTable[2403],   0, 796 },
  { &yyActionTable[2403],   1, 811 },
  { &yyActionTable[2404],   1, 811 },
  { &yyActionTable[2405],   2, 811 },
  { &yyActionTable[2407],   5, 811 },
  { &yyActionTable[2412],   2, 811 },
  { &yyActionTable[2414],   0, 786 },
  { &yyActionTable[2414],   0, 787 },
  { &yyActionTable[2414],   1, 811 },
  { &yyActionTable[2415],   0, 788 },
  { &yyActionTable[2415],   4, 811 },
  { &yyActionTable[2419],   4, 811 },
  { &yyActionTable[2423],   1, 811 },
  { &yyActionTable[2424],   4, 811 },
  { &yyActionTable[2428],   1, 811 },
  { &yyActionTable[2429],   4, 811 },
  { &yyActionTable[2433],   1, 811 },
  { &yyActionTable[2434],   4, 811 },
  { &yyActionTable[2438],   1, 811 },
  { &yyActionTable[2439],   5, 811 },
  { &yyActionTable[2444],   1, 811 },
  { &yyActionTable[2445],   4, 811 },
  { &yyActionTable[2449],   3, 811 },
  { &yyActionTable[2452],   1, 811 },
  { &yyActionTable[2453],   2, 621 },
  { &yyActionTable[2455],   4, 811 },
  { &yyActionTable[2459],   1, 811 },
  { &yyActionTable[2460],   5, 811 },
  { &yyActionTable[2465],   4, 811 },
  { &yyActionTable[2469],   1, 811 },
  { &yyActionTable[2470],   1, 811 },
  { &yyActionTable[2471],   4, 811 },
  { &yyActionTable[2475],   2, 696 },
  { &yyActionTable[2477],   4, 811 },
  { &yyActionTable[2481],   5, 811 },
  { &yyActionTable[2486],   1, 811 },
  { &yyActionTable[2487],  16, 811 },
  { &yyActionTable[2503],   2, 811 },
  { &yyActionTable[2505],   1, 811 },
  { &yyActionTable[2506],   3, 811 },
  { &yyActionTable[2509],   1, 811 },
  { &yyActionTable[2510],   1, 811 },
  { &yyActionTable[2511],   2, 621 },
  { &yyActionTable[2513],   4, 811 },
  { &yyActionTable[2517],   1, 811 },
  { &yyActionTable[2518],   4, 811 },
  { &yyActionTable[2522],   2, 811 },
  { &yyActionTable[2524],   1, 811 },
  { &yyActionTable[2525],   1, 811 },
  { &yyActionTable[2526],   1, 811 },
  { &yyActionTable[2527],   5, 811 },
  { &yyActionTable[2532],   1, 811 },
  { &yyActionTable[2533],   3, 811 },
  { &yyActionTable[2536],   3, 811 },
  { &yyActionTable[2539],  10, 784 },
  { &yyActionTable[2549],   1, 811 },
  { &yyActionTable[2550],   1, 811 },
  { &yyActionTable[2551],   1, 811 },
  { &yyActionTable[2552],   1, 811 },
  { &yyActionTable[2553],   3, 811 },
  { &yyActionTable[2556],   1, 811 },
  { &yyActionTable[2557],   1, 811 },
  { &yyActionTable[2558],   1, 811 },
  { &yyActionTable[2559],   3, 811 },
  { &yyActionTable[2562],   1, 811 },
  { &yyActionTable[2563],   0, 783 },
  { &yyActionTable[2563],   4, 811 },
  { &yyActionTable[2567],   1, 811 },
  { &yyActionTable[2568],   1, 811 },
  { &yyActionTable[2569],   0, 535 },
  { &yyActionTable[2569],   0, 537 },
  { &yyActionTable[2569],   0, 532 },
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
   56,  /*      ABORT => ID */
   56,  /*      AFTER => ID */
    0,  /* AGG_FUNCTION => nothing */
    0,  /*        ALL => nothing */
    0,  /*        AND => nothing */
    0,  /*         AS => nothing */
   56,  /*        ASC => ID */
   56,  /*     BEFORE => ID */
   56,  /*      BEGIN => ID */
    0,  /*    BETWEEN => nothing */
    0,  /*     BITAND => nothing */
    0,  /*     BITNOT => nothing */
    0,  /*      BITOR => nothing */
    0,  /*         BY => nothing */
   56,  /*    CASCADE => ID */
    0,  /*       CASE => nothing */
    0,  /*      CHECK => nothing */
   56,  /*    CLUSTER => ID */
    0,  /*    COLLATE => nothing */
    0,  /*     COLUMN => nothing */
    0,  /*      COMMA => nothing */
    0,  /*    COMMENT => nothing */
    0,  /*     COMMIT => nothing */
    0,  /*     CONCAT => nothing */
   56,  /*   CONFLICT => ID */
    0,  /* CONSTRAINT => nothing */
   56,  /*       COPY => ID */
    0,  /*     CREATE => nothing */
    0,  /*    DEFAULT => nothing */
    0,  /* DEFERRABLE => nothing */
   56,  /*   DEFERRED => ID */
    0,  /*     DELETE => nothing */
   56,  /* DELIMITERS => ID */
   56,  /*       DESC => ID */
    0,  /*   DISTINCT => nothing */
    0,  /*        DOT => nothing */
    0,  /*       DROP => nothing */
   56,  /*       EACH => ID */
    0,  /*       ELSE => nothing */
   56,  /*        END => ID */
    0,  /* END_OF_FILE => nothing */
    0,  /*         EQ => nothing */
    0,  /*     EXCEPT => nothing */
   56,  /*    EXPLAIN => ID */
   56,  /*       FAIL => ID */
    0,  /*      FLOAT => nothing */
   56,  /*        FOR => ID */
    0,  /*    FOREIGN => nothing */
    0,  /*       FROM => nothing */
    0,  /*   FUNCTION => nothing */
    0,  /*         GE => nothing */
    0,  /*       GLOB => nothing */
    0,  /*      GROUP => nothing */
    0,  /*         GT => nothing */
    0,  /*     HAVING => nothing */
    0,  /*         ID => nothing */
   56,  /*     IGNORE => ID */
    0,  /*    ILLEGAL => nothing */
   56,  /*  IMMEDIATE => ID */
    0,  /*         IN => nothing */
    0,  /*      INDEX => nothing */
   56,  /*  INITIALLY => ID */
    0,  /*     INSERT => nothing */
   56,  /*    INSTEAD => ID */
    0,  /*    INTEGER => nothing */
    0,  /*  INTERSECT => nothing */
    0,  /*       INTO => nothing */
    0,  /*         IS => nothing */
    0,  /*     ISNULL => nothing */
    0,  /*       JOIN => nothing */
    0,  /*    JOIN_KW => nothing */
   56,  /*        KEY => ID */
    0,  /*         LE => nothing */
    0,  /*       LIKE => nothing */
    0,  /*      LIMIT => nothing */
    0,  /*         LP => nothing */
    0,  /*     LSHIFT => nothing */
    0,  /*         LT => nothing */
   56,  /*      MATCH => ID */
    0,  /*      MINUS => nothing */
    0,  /*         NE => nothing */
    0,  /*        NOT => nothing */
    0,  /*    NOTNULL => nothing */
    0,  /*       NULL => nothing */
   56,  /*         OF => ID */
   56,  /*     OFFSET => ID */
    0,  /*         ON => nothing */
    0,  /*         OR => nothing */
    0,  /* ORACLE_OUTER_JOIN => nothing */
    0,  /*      ORDER => nothing */
    0,  /*       PLUS => nothing */
   56,  /*     PRAGMA => ID */
    0,  /*    PRIMARY => nothing */
   56,  /*      RAISE => ID */
    0,  /* REFERENCES => nothing */
    0,  /*        REM => nothing */
   56,  /*    REPLACE => ID */
   56,  /*   RESTRICT => ID */
    0,  /*   ROLLBACK => nothing */
   56,  /*        ROW => ID */
    0,  /*         RP => nothing */
    0,  /*     RSHIFT => nothing */
    0,  /*     SELECT => nothing */
    0,  /*       SEMI => nothing */
    0,  /*        SET => nothing */
    0,  /*      SLASH => nothing */
    0,  /*      SPACE => nothing */
    0,  /*       STAR => nothing */
   56,  /*  STATEMENT => ID */
    0,  /*     STRING => nothing */
    0,  /*      TABLE => nothing */
   56,  /*       TEMP => ID */
    0,  /*       THEN => nothing */
    0,  /* TRANSACTION => nothing */
   56,  /*    TRIGGER => ID */
    0,  /*     UMINUS => nothing */
    0,  /* UNCLOSED_STRING => nothing */
    0,  /*      UNION => nothing */
    0,  /*     UNIQUE => nothing */
    0,  /*     UPDATE => nothing */
    0,  /*      UPLUS => nothing */
    0,  /*      USING => nothing */
   56,  /*     VACUUM => ID */
    0,  /*     VALUES => nothing */
   56,  /*       VIEW => ID */
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
  "BEFORE",        "BEGIN",         "BETWEEN",       "BITAND",      
  "BITNOT",        "BITOR",         "BY",            "CASCADE",     
  "CASE",          "CHECK",         "CLUSTER",       "COLLATE",     
  "COLUMN",        "COMMA",         "COMMENT",       "COMMIT",      
  "CONCAT",        "CONFLICT",      "CONSTRAINT",    "COPY",        
  "CREATE",        "DEFAULT",       "DEFERRABLE",    "DEFERRED",    
  "DELETE",        "DELIMITERS",    "DESC",          "DISTINCT",    
  "DOT",           "DROP",          "EACH",          "ELSE",        
  "END",           "END_OF_FILE",   "EQ",            "EXCEPT",      
  "EXPLAIN",       "FAIL",          "FLOAT",         "FOR",         
  "FOREIGN",       "FROM",          "FUNCTION",      "GE",          
  "GLOB",          "GROUP",         "GT",            "HAVING",      
  "ID",            "IGNORE",        "ILLEGAL",       "IMMEDIATE",   
  "IN",            "INDEX",         "INITIALLY",     "INSERT",      
  "INSTEAD",       "INTEGER",       "INTERSECT",     "INTO",        
  "IS",            "ISNULL",        "JOIN",          "JOIN_KW",     
  "KEY",           "LE",            "LIKE",          "LIMIT",       
  "LP",            "LSHIFT",        "LT",            "MATCH",       
  "MINUS",         "NE",            "NOT",           "NOTNULL",     
  "NULL",          "OF",            "OFFSET",        "ON",          
  "OR",            "ORACLE_OUTER_JOIN",  "ORDER",         "PLUS",        
  "PRAGMA",        "PRIMARY",       "RAISE",         "REFERENCES",  
  "REM",           "REPLACE",       "RESTRICT",      "ROLLBACK",    
  "ROW",           "RP",            "RSHIFT",        "SELECT",      
  "SEMI",          "SET",           "SLASH",         "SPACE",       
  "STAR",          "STATEMENT",     "STRING",        "TABLE",       
  "TEMP",          "THEN",          "TRANSACTION",   "TRIGGER",     
  "UMINUS",        "UNCLOSED_STRING",  "UNION",         "UNIQUE",      
  "UPDATE",        "UPLUS",         "USING",         "VACUUM",      
  "VALUES",        "VIEW",          "WHEN",          "WHERE",       
  "as",            "carg",          "carglist",      "case_else",   
  "case_exprlist",  "case_operand",  "ccons",         "cmd",         
  "cmdlist",       "cmdx",          "collate",       "column",      
  "columnid",      "columnlist",    "conslist",      "conslist_opt",
  "create_table",  "create_table_args",  "defer_subclause",  "defer_subclause_opt",
  "distinct",      "ecmd",          "error",         "explain",     
  "expr",          "expritem",      "exprlist",      "foreach_clause",
  "from",          "groupby_opt",   "having_opt",    "id",          
  "ids",           "idxitem",       "idxlist",       "idxlist_opt", 
  "init_deferred_pred_opt",  "input",         "inscollist",    "inscollist_opt",
  "insert_cmd",    "itemlist",      "joinop",        "joinop2",     
  "likeop",        "limit_opt",     "limit_sep",     "minus_num",   
  "multiselect_op",  "nm",            "number",        "on_opt",      
  "onconf",        "oneselect",     "orconf",        "orderby_opt", 
  "plus_num",      "plus_opt",      "refact",        "refarg",      
  "refargs",       "resolvetype",   "sclp",          "selcollist",  
  "select",        "seltablist",    "setlist",       "signed",      
  "sortitem",      "sortlist",      "sortorder",     "stl_prefix",  
  "tcons",         "temp",          "trans_opt",     "trigger_cmd", 
  "trigger_cmd_list",  "trigger_event",  "trigger_time",  "type",        
  "typename",      "uniqueflag",    "using_opt",     "when_clause", 
  "where_opt",   
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
 /* 123 */ "seltablist ::= stl_prefix nm as on_opt using_opt",
 /* 124 */ "seltablist ::= stl_prefix LP select RP as on_opt using_opt",
 /* 125 */ "joinop ::= COMMA",
 /* 126 */ "joinop ::= JOIN",
 /* 127 */ "joinop ::= JOIN_KW JOIN",
 /* 128 */ "joinop ::= JOIN_KW nm JOIN",
 /* 129 */ "joinop ::= JOIN_KW nm nm JOIN",
 /* 130 */ "on_opt ::= ON expr",
 /* 131 */ "on_opt ::=",
 /* 132 */ "using_opt ::= USING LP idxlist RP",
 /* 133 */ "using_opt ::=",
 /* 134 */ "orderby_opt ::=",
 /* 135 */ "orderby_opt ::= ORDER BY sortlist",
 /* 136 */ "sortlist ::= sortlist COMMA sortitem collate sortorder",
 /* 137 */ "sortlist ::= sortitem collate sortorder",
 /* 138 */ "sortitem ::= expr",
 /* 139 */ "sortorder ::= ASC",
 /* 140 */ "sortorder ::= DESC",
 /* 141 */ "sortorder ::=",
 /* 142 */ "collate ::=",
 /* 143 */ "collate ::= COLLATE id",
 /* 144 */ "groupby_opt ::=",
 /* 145 */ "groupby_opt ::= GROUP BY exprlist",
 /* 146 */ "having_opt ::=",
 /* 147 */ "having_opt ::= HAVING expr",
 /* 148 */ "limit_opt ::=",
 /* 149 */ "limit_opt ::= LIMIT INTEGER",
 /* 150 */ "limit_opt ::= LIMIT INTEGER limit_sep INTEGER",
 /* 151 */ "limit_sep ::= OFFSET",
 /* 152 */ "limit_sep ::= COMMA",
 /* 153 */ "cmd ::= DELETE FROM nm where_opt",
 /* 154 */ "where_opt ::=",
 /* 155 */ "where_opt ::= WHERE expr",
 /* 156 */ "cmd ::= UPDATE orconf nm SET setlist where_opt",
 /* 157 */ "setlist ::= setlist COMMA nm EQ expr",
 /* 158 */ "setlist ::= nm EQ expr",
 /* 159 */ "cmd ::= insert_cmd INTO nm inscollist_opt VALUES LP itemlist RP",
 /* 160 */ "cmd ::= insert_cmd INTO nm inscollist_opt select",
 /* 161 */ "insert_cmd ::= INSERT orconf",
 /* 162 */ "insert_cmd ::= REPLACE",
 /* 163 */ "itemlist ::= itemlist COMMA expr",
 /* 164 */ "itemlist ::= expr",
 /* 165 */ "inscollist_opt ::=",
 /* 166 */ "inscollist_opt ::= LP inscollist RP",
 /* 167 */ "inscollist ::= inscollist COMMA nm",
 /* 168 */ "inscollist ::= nm",
 /* 169 */ "expr ::= LP expr RP",
 /* 170 */ "expr ::= NULL",
 /* 171 */ "expr ::= ID",
 /* 172 */ "expr ::= JOIN_KW",
 /* 173 */ "expr ::= nm DOT nm",
 /* 174 */ "expr ::= expr ORACLE_OUTER_JOIN",
 /* 175 */ "expr ::= INTEGER",
 /* 176 */ "expr ::= FLOAT",
 /* 177 */ "expr ::= STRING",
 /* 178 */ "expr ::= ID LP exprlist RP",
 /* 179 */ "expr ::= ID LP STAR RP",
 /* 180 */ "expr ::= expr AND expr",
 /* 181 */ "expr ::= expr OR expr",
 /* 182 */ "expr ::= expr LT expr",
 /* 183 */ "expr ::= expr GT expr",
 /* 184 */ "expr ::= expr LE expr",
 /* 185 */ "expr ::= expr GE expr",
 /* 186 */ "expr ::= expr NE expr",
 /* 187 */ "expr ::= expr EQ expr",
 /* 188 */ "expr ::= expr BITAND expr",
 /* 189 */ "expr ::= expr BITOR expr",
 /* 190 */ "expr ::= expr LSHIFT expr",
 /* 191 */ "expr ::= expr RSHIFT expr",
 /* 192 */ "expr ::= expr likeop expr",
 /* 193 */ "expr ::= expr NOT likeop expr",
 /* 194 */ "likeop ::= LIKE",
 /* 195 */ "likeop ::= GLOB",
 /* 196 */ "expr ::= expr PLUS expr",
 /* 197 */ "expr ::= expr MINUS expr",
 /* 198 */ "expr ::= expr STAR expr",
 /* 199 */ "expr ::= expr SLASH expr",
 /* 200 */ "expr ::= expr REM expr",
 /* 201 */ "expr ::= expr CONCAT expr",
 /* 202 */ "expr ::= expr ISNULL",
 /* 203 */ "expr ::= expr IS NULL",
 /* 204 */ "expr ::= expr NOTNULL",
 /* 205 */ "expr ::= expr NOT NULL",
 /* 206 */ "expr ::= expr IS NOT NULL",
 /* 207 */ "expr ::= NOT expr",
 /* 208 */ "expr ::= BITNOT expr",
 /* 209 */ "expr ::= MINUS expr",
 /* 210 */ "expr ::= PLUS expr",
 /* 211 */ "expr ::= LP select RP",
 /* 212 */ "expr ::= expr BETWEEN expr AND expr",
 /* 213 */ "expr ::= expr NOT BETWEEN expr AND expr",
 /* 214 */ "expr ::= expr IN LP exprlist RP",
 /* 215 */ "expr ::= expr IN LP select RP",
 /* 216 */ "expr ::= expr NOT IN LP exprlist RP",
 /* 217 */ "expr ::= expr NOT IN LP select RP",
 /* 218 */ "expr ::= CASE case_operand case_exprlist case_else END",
 /* 219 */ "case_exprlist ::= case_exprlist WHEN expr THEN expr",
 /* 220 */ "case_exprlist ::= WHEN expr THEN expr",
 /* 221 */ "case_else ::= ELSE expr",
 /* 222 */ "case_else ::=",
 /* 223 */ "case_operand ::= expr",
 /* 224 */ "case_operand ::=",
 /* 225 */ "exprlist ::= exprlist COMMA expritem",
 /* 226 */ "exprlist ::= expritem",
 /* 227 */ "expritem ::= expr",
 /* 228 */ "expritem ::=",
 /* 229 */ "cmd ::= CREATE uniqueflag INDEX nm ON nm LP idxlist RP onconf",
 /* 230 */ "uniqueflag ::= UNIQUE",
 /* 231 */ "uniqueflag ::=",
 /* 232 */ "idxlist_opt ::=",
 /* 233 */ "idxlist_opt ::= LP idxlist RP",
 /* 234 */ "idxlist ::= idxlist COMMA idxitem",
 /* 235 */ "idxlist ::= idxitem",
 /* 236 */ "idxitem ::= nm",
 /* 237 */ "cmd ::= DROP INDEX nm",
 /* 238 */ "cmd ::= COPY orconf nm FROM nm USING DELIMITERS STRING",
 /* 239 */ "cmd ::= COPY orconf nm FROM nm",
 /* 240 */ "cmd ::= VACUUM",
 /* 241 */ "cmd ::= VACUUM nm",
 /* 242 */ "cmd ::= PRAGMA ids EQ nm",
 /* 243 */ "cmd ::= PRAGMA ids EQ ON",
 /* 244 */ "cmd ::= PRAGMA ids EQ plus_num",
 /* 245 */ "cmd ::= PRAGMA ids EQ minus_num",
 /* 246 */ "cmd ::= PRAGMA ids LP nm RP",
 /* 247 */ "cmd ::= PRAGMA ids",
 /* 248 */ "plus_num ::= plus_opt number",
 /* 249 */ "minus_num ::= MINUS number",
 /* 250 */ "number ::= INTEGER",
 /* 251 */ "number ::= FLOAT",
 /* 252 */ "plus_opt ::= PLUS",
 /* 253 */ "plus_opt ::=",
 /* 254 */ "cmd ::= CREATE TRIGGER nm trigger_time trigger_event ON nm foreach_clause when_clause BEGIN trigger_cmd_list END",
 /* 255 */ "trigger_time ::= BEFORE",
 /* 256 */ "trigger_time ::= AFTER",
 /* 257 */ "trigger_time ::= INSTEAD OF",
 /* 258 */ "trigger_time ::=",
 /* 259 */ "trigger_event ::= DELETE",
 /* 260 */ "trigger_event ::= INSERT",
 /* 261 */ "trigger_event ::= UPDATE",
 /* 262 */ "trigger_event ::= UPDATE OF inscollist",
 /* 263 */ "foreach_clause ::=",
 /* 264 */ "foreach_clause ::= FOR EACH ROW",
 /* 265 */ "foreach_clause ::= FOR EACH STATEMENT",
 /* 266 */ "when_clause ::=",
 /* 267 */ "when_clause ::= WHEN expr",
 /* 268 */ "trigger_cmd_list ::= trigger_cmd SEMI trigger_cmd_list",
 /* 269 */ "trigger_cmd_list ::=",
 /* 270 */ "trigger_cmd ::= UPDATE orconf nm SET setlist where_opt",
 /* 271 */ "trigger_cmd ::= INSERT orconf INTO nm inscollist_opt VALUES LP itemlist RP",
 /* 272 */ "trigger_cmd ::= INSERT orconf INTO nm inscollist_opt select",
 /* 273 */ "trigger_cmd ::= DELETE FROM nm where_opt",
 /* 274 */ "trigger_cmd ::= select",
 /* 275 */ "expr ::= RAISE LP IGNORE RP",
 /* 276 */ "expr ::= RAISE LP ROLLBACK COMMA nm RP",
 /* 277 */ "expr ::= RAISE LP ABORT COMMA nm RP",
 /* 278 */ "expr ::= RAISE LP FAIL COMMA nm RP",
 /* 279 */ "cmd ::= DROP TRIGGER nm",
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
    case 132:
#line 660 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy168));}
#line 4269 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 152:
#line 514 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy272));}
#line 4274 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 153:
#line 679 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy272));}
#line 4279 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 154:
#line 677 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy168));}
#line 4284 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 156:
#line 341 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteSrcListDelete((yypminor->yy289));}
#line 4289 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 157:
#line 431 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy168));}
#line 4294 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 158:
#line 436 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy272));}
#line 4299 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 162:
#line 701 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy268));}
#line 4304 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 163:
#line 703 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy268));}
#line 4309 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 166:
#line 492 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy268));}
#line 4314 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 167:
#line 490 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy268));}
#line 4319 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 169:
#line 484 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy168));}
#line 4324 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 179:
#line 392 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy272));}
#line 4329 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 181:
#line 276 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteSelectDelete((yypminor->yy207));}
#line 4334 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 183:
#line 403 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy168));}
#line 4339 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 190:
#line 312 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy168));}
#line 4344 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 191:
#line 310 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy168));}
#line 4349 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 192:
#line 274 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteSelectDelete((yypminor->yy207));}
#line 4354 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 193:
#line 337 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteSrcListDelete((yypminor->yy289));}
#line 4359 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 194:
#line 460 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy168));}
#line 4364 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 196:
#line 407 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy272));}
#line 4369 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 197:
#line 405 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprListDelete((yypminor->yy168));}
#line 4374 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 199:
#line 339 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteSrcListDelete((yypminor->yy289));}
#line 4379 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 205:
#line 762 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy72).b);}
#line 4384 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 210:
#line 397 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteIdListDelete((yypminor->yy268));}
#line 4389 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
      break;
    case 212:
#line 454 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteExprDelete((yypminor->yy272));}
#line 4394 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
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
  { 165, 1 },
  { 136, 1 },
  { 136, 2 },
  { 149, 3 },
  { 149, 1 },
  { 137, 1 },
  { 151, 1 },
  { 151, 0 },
  { 135, 3 },
  { 202, 0 },
  { 202, 1 },
  { 202, 2 },
  { 135, 2 },
  { 135, 2 },
  { 135, 2 },
  { 135, 2 },
  { 144, 4 },
  { 201, 1 },
  { 201, 0 },
  { 145, 4 },
  { 145, 2 },
  { 141, 3 },
  { 141, 1 },
  { 139, 3 },
  { 140, 1 },
  { 159, 1 },
  { 160, 1 },
  { 160, 1 },
  { 177, 1 },
  { 177, 1 },
  { 177, 1 },
  { 207, 0 },
  { 207, 1 },
  { 207, 4 },
  { 207, 6 },
  { 208, 1 },
  { 208, 2 },
  { 195, 1 },
  { 195, 2 },
  { 195, 2 },
  { 130, 2 },
  { 130, 0 },
  { 129, 3 },
  { 129, 1 },
  { 129, 2 },
  { 129, 2 },
  { 129, 2 },
  { 129, 3 },
  { 129, 3 },
  { 129, 2 },
  { 129, 3 },
  { 129, 3 },
  { 129, 2 },
  { 134, 2 },
  { 134, 3 },
  { 134, 4 },
  { 134, 2 },
  { 134, 5 },
  { 134, 4 },
  { 134, 1 },
  { 134, 2 },
  { 188, 0 },
  { 188, 2 },
  { 187, 2 },
  { 187, 3 },
  { 187, 3 },
  { 187, 3 },
  { 186, 2 },
  { 186, 2 },
  { 186, 1 },
  { 186, 1 },
  { 146, 3 },
  { 146, 2 },
  { 164, 0 },
  { 164, 2 },
  { 164, 2 },
  { 143, 0 },
  { 143, 2 },
  { 142, 3 },
  { 142, 2 },
  { 142, 1 },
  { 200, 2 },
  { 200, 6 },
  { 200, 5 },
  { 200, 3 },
  { 200, 10 },
  { 147, 0 },
  { 147, 1 },
  { 180, 0 },
  { 180, 3 },
  { 182, 0 },
  { 182, 2 },
  { 189, 1 },
  { 189, 1 },
  { 189, 1 },
  { 189, 1 },
  { 189, 1 },
  { 135, 3 },
  { 135, 6 },
  { 135, 3 },
  { 135, 1 },
  { 192, 1 },
  { 192, 3 },
  { 176, 1 },
  { 176, 2 },
  { 176, 1 },
  { 176, 1 },
  { 181, 9 },
  { 148, 1 },
  { 148, 1 },
  { 148, 0 },
  { 190, 2 },
  { 190, 0 },
  { 191, 3 },
  { 191, 2 },
  { 191, 4 },
  { 128, 2 },
  { 128, 1 },
  { 128, 0 },
  { 156, 0 },
  { 156, 2 },
  { 199, 2 },
  { 199, 0 },
  { 193, 5 },
  { 193, 7 },
  { 170, 1 },
  { 170, 1 },
  { 170, 2 },
  { 170, 3 },
  { 170, 4 },
  { 179, 2 },
  { 179, 0 },
  { 210, 4 },
  { 210, 0 },
  { 183, 0 },
  { 183, 3 },
  { 197, 5 },
  { 197, 3 },
  { 196, 1 },
  { 198, 1 },
  { 198, 1 },
  { 198, 0 },
  { 138, 0 },
  { 138, 2 },
  { 157, 0 },
  { 157, 3 },
  { 158, 0 },
  { 158, 2 },
  { 173, 0 },
  { 173, 2 },
  { 173, 4 },
  { 174, 1 },
  { 174, 1 },
  { 135, 4 },
  { 212, 0 },
  { 212, 2 },
  { 135, 6 },
  { 194, 5 },
  { 194, 3 },
  { 135, 8 },
  { 135, 5 },
  { 168, 2 },
  { 168, 1 },
  { 169, 3 },
  { 169, 1 },
  { 167, 0 },
  { 167, 3 },
  { 166, 3 },
  { 166, 1 },
  { 152, 3 },
  { 152, 1 },
  { 152, 1 },
  { 152, 1 },
  { 152, 3 },
  { 152, 2 },
  { 152, 1 },
  { 152, 1 },
  { 152, 1 },
  { 152, 4 },
  { 152, 4 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 4 },
  { 172, 1 },
  { 172, 1 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 3 },
  { 152, 2 },
  { 152, 3 },
  { 152, 2 },
  { 152, 3 },
  { 152, 4 },
  { 152, 2 },
  { 152, 2 },
  { 152, 2 },
  { 152, 2 },
  { 152, 3 },
  { 152, 5 },
  { 152, 6 },
  { 152, 5 },
  { 152, 5 },
  { 152, 6 },
  { 152, 6 },
  { 152, 5 },
  { 132, 5 },
  { 132, 4 },
  { 131, 2 },
  { 131, 0 },
  { 133, 1 },
  { 133, 0 },
  { 154, 3 },
  { 154, 1 },
  { 153, 1 },
  { 153, 0 },
  { 135, 10 },
  { 209, 1 },
  { 209, 0 },
  { 163, 0 },
  { 163, 3 },
  { 162, 3 },
  { 162, 1 },
  { 161, 1 },
  { 135, 3 },
  { 135, 8 },
  { 135, 5 },
  { 135, 1 },
  { 135, 2 },
  { 135, 4 },
  { 135, 4 },
  { 135, 4 },
  { 135, 4 },
  { 135, 5 },
  { 135, 2 },
  { 184, 2 },
  { 175, 2 },
  { 178, 1 },
  { 178, 1 },
  { 185, 1 },
  { 185, 0 },
  { 135, 12 },
  { 206, 1 },
  { 206, 1 },
  { 206, 2 },
  { 206, 0 },
  { 205, 1 },
  { 205, 1 },
  { 205, 1 },
  { 205, 3 },
  { 155, 0 },
  { 155, 3 },
  { 155, 3 },
  { 211, 0 },
  { 211, 2 },
  { 204, 3 },
  { 204, 0 },
  { 203, 6 },
  { 203, 9 },
  { 203, 6 },
  { 203, 4 },
  { 203, 1 },
  { 152, 4 },
  { 152, 6 },
  { 152, 6 },
  { 152, 6 },
  { 135, 3 },
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
#line 77 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ sqliteExec(pParse); }
#line 4877 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for cmd */
        break;
      case 6:
#line 78 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ sqliteBeginParse(pParse, 1); }
#line 4883 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for EXPLAIN */
        break;
      case 7:
#line 79 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ sqliteBeginParse(pParse, 0); }
#line 4889 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 8:
#line 84 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteBeginTransaction(pParse,yymsp[0].minor.yy136);}
#line 4894 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
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
#line 88 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteCommitTransaction(pParse);}
#line 4910 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMIT */
        /* No destructor defined for trans_opt */
        break;
      case 13:
#line 89 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteCommitTransaction(pParse);}
#line 4917 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for END */
        /* No destructor defined for trans_opt */
        break;
      case 14:
#line 90 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteRollbackTransaction(pParse);}
#line 4924 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ROLLBACK */
        /* No destructor defined for trans_opt */
        break;
      case 15:
        /* No destructor defined for create_table */
        /* No destructor defined for create_table_args */
        break;
      case 16:
#line 95 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
   sqliteStartTable(pParse,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy324,yymsp[-2].minor.yy136,0);
}
#line 4937 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for TABLE */
        break;
      case 17:
#line 99 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = pParse->isTemp || !pParse->initFlag;}
#line 4943 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for TEMP */
        break;
      case 18:
#line 100 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = pParse->isTemp;}
#line 4949 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 19:
#line 101 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  sqliteEndTable(pParse,&yymsp[0].minor.yy0,0);
}
#line 4956 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for columnlist */
        /* No destructor defined for conslist_opt */
        break;
      case 20:
#line 104 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  sqliteEndTable(pParse,0,yymsp[0].minor.yy207);
  sqliteSelectDelete(yymsp[0].minor.yy207);
}
#line 4967 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
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
#line 116 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddColumn(pParse,&yymsp[0].minor.yy324);}
#line 4986 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 25:
#line 122 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy0;}
#line 4991 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 26:
#line 138 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy0;}
#line 4996 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 27:
#line 139 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy0;}
#line 5001 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 28:
#line 144 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy0;}
#line 5006 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 29:
#line 145 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy0;}
#line 5011 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 30:
#line 146 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy0;}
#line 5016 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 31:
        break;
      case 32:
#line 149 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[0].minor.yy324,&yymsp[0].minor.yy324);}
#line 5023 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 33:
#line 150 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[-3].minor.yy324,&yymsp[0].minor.yy0);}
#line 5028 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for signed */
        break;
      case 34:
#line 152 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddColumnType(pParse,&yymsp[-5].minor.yy324,&yymsp[0].minor.yy0);}
#line 5035 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for signed */
        /* No destructor defined for COMMA */
        /* No destructor defined for signed */
        break;
      case 35:
#line 154 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy324;}
#line 5044 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 36:
#line 155 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[-1].minor.yy324;}
#line 5049 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
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
#line 163 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5080 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        break;
      case 45:
#line 164 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5086 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        break;
      case 46:
#line 165 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5092 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        break;
      case 47:
#line 166 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5098 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        /* No destructor defined for PLUS */
        break;
      case 48:
#line 167 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,1);}
#line 5105 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        /* No destructor defined for MINUS */
        break;
      case 49:
#line 168 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5112 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        break;
      case 50:
#line 169 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,0);}
#line 5118 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFAULT */
        /* No destructor defined for PLUS */
        break;
      case 51:
#line 170 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddDefaultValue(pParse,&yymsp[0].minor.yy0,1);}
#line 5125 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
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
#line 177 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddNotNull(pParse, yymsp[0].minor.yy136);}
#line 5140 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        /* No destructor defined for NULL */
        break;
      case 55:
#line 178 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddPrimaryKey(pParse,0,yymsp[0].minor.yy136);}
#line 5147 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRIMARY */
        /* No destructor defined for KEY */
        /* No destructor defined for sortorder */
        break;
      case 56:
#line 179 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteCreateIndex(pParse,0,0,0,yymsp[0].minor.yy136,0,0);}
#line 5155 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UNIQUE */
        break;
      case 57:
        /* No destructor defined for CHECK */
        /* No destructor defined for LP */
  yy_destructor(152,&yymsp[-2].minor);
        /* No destructor defined for RP */
        /* No destructor defined for onconf */
        break;
      case 58:
#line 182 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteCreateForeignKey(pParse,0,&yymsp[-2].minor.yy324,yymsp[-1].minor.yy268,yymsp[0].minor.yy136);}
#line 5168 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for REFERENCES */
        break;
      case 59:
#line 183 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteDeferForeignKey(pParse,yymsp[0].minor.yy136);}
#line 5174 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 60:
#line 184 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
   sqliteAddCollateType(pParse, sqliteCollateType(yymsp[0].minor.yy324.z, yymsp[0].minor.yy324.n));
}
#line 5181 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COLLATE */
        break;
      case 61:
#line 194 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Restrict * 0x010101; }
#line 5187 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 62:
#line 195 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = (yymsp[-1].minor.yy136 & yymsp[0].minor.yy83.mask) | yymsp[0].minor.yy83.value; }
#line 5192 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 63:
#line 197 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy83.value = 0;     yygotominor.yy83.mask = 0x000000; }
#line 5197 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for MATCH */
        /* No destructor defined for nm */
        break;
      case 64:
#line 198 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy83.value = yymsp[0].minor.yy136;     yygotominor.yy83.mask = 0x0000ff; }
#line 5204 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ON */
        /* No destructor defined for DELETE */
        break;
      case 65:
#line 199 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy83.value = yymsp[0].minor.yy136<<8;  yygotominor.yy83.mask = 0x00ff00; }
#line 5211 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ON */
        /* No destructor defined for UPDATE */
        break;
      case 66:
#line 200 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy83.value = yymsp[0].minor.yy136<<16; yygotominor.yy83.mask = 0xff0000; }
#line 5218 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ON */
        /* No destructor defined for INSERT */
        break;
      case 67:
#line 202 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_SetNull; }
#line 5225 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for SET */
        /* No destructor defined for NULL */
        break;
      case 68:
#line 203 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_SetDflt; }
#line 5232 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for SET */
        /* No destructor defined for DEFAULT */
        break;
      case 69:
#line 204 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Cascade; }
#line 5239 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for CASCADE */
        break;
      case 70:
#line 205 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Restrict; }
#line 5245 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for RESTRICT */
        break;
      case 71:
#line 207 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = yymsp[0].minor.yy136;}
#line 5251 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        /* No destructor defined for DEFERRABLE */
        break;
      case 72:
#line 208 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = yymsp[0].minor.yy136;}
#line 5258 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DEFERRABLE */
        break;
      case 73:
#line 210 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = 0;}
#line 5264 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 74:
#line 211 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = 1;}
#line 5269 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INITIALLY */
        /* No destructor defined for DEFERRED */
        break;
      case 75:
#line 212 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = 0;}
#line 5276 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
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
#line 224 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteAddPrimaryKey(pParse,yymsp[-2].minor.yy268,yymsp[0].minor.yy136);}
#line 5305 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRIMARY */
        /* No destructor defined for KEY */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 83:
#line 226 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteCreateIndex(pParse,0,0,yymsp[-2].minor.yy268,yymsp[0].minor.yy136,0,0);}
#line 5314 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UNIQUE */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 84:
        /* No destructor defined for CHECK */
  yy_destructor(152,&yymsp[-1].minor);
        /* No destructor defined for onconf */
        break;
      case 85:
#line 229 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
    sqliteCreateForeignKey(pParse, yymsp[-6].minor.yy268, &yymsp[-3].minor.yy324, yymsp[-2].minor.yy268, yymsp[-1].minor.yy136);
    sqliteDeferForeignKey(pParse, yymsp[0].minor.yy136);
}
#line 5330 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for FOREIGN */
        /* No destructor defined for KEY */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        /* No destructor defined for REFERENCES */
        break;
      case 86:
#line 234 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = 0;}
#line 5340 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 87:
#line 235 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = yymsp[0].minor.yy136;}
#line 5345 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 88:
#line 243 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Default; }
#line 5350 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 89:
#line 244 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = yymsp[0].minor.yy136; }
#line 5355 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ON */
        /* No destructor defined for CONFLICT */
        break;
      case 90:
#line 245 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Default; }
#line 5362 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 91:
#line 246 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = yymsp[0].minor.yy136; }
#line 5367 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for OR */
        break;
      case 92:
#line 247 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Rollback; }
#line 5373 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ROLLBACK */
        break;
      case 93:
#line 248 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Abort; }
#line 5379 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ABORT */
        break;
      case 94:
#line 249 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Fail; }
#line 5385 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for FAIL */
        break;
      case 95:
#line 250 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Ignore; }
#line 5391 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for IGNORE */
        break;
      case 96:
#line 251 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Replace; }
#line 5397 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for REPLACE */
        break;
      case 97:
#line 255 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteDropTable(pParse,&yymsp[0].minor.yy324,0);}
#line 5403 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DROP */
        /* No destructor defined for TABLE */
        break;
      case 98:
#line 259 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  sqliteCreateView(pParse, &yymsp[-5].minor.yy0, &yymsp[-2].minor.yy324, yymsp[0].minor.yy207, yymsp[-4].minor.yy136);
}
#line 5412 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for VIEW */
        /* No destructor defined for AS */
        break;
      case 99:
#line 262 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  sqliteDropTable(pParse, &yymsp[0].minor.yy324, 1);
}
#line 5421 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DROP */
        /* No destructor defined for VIEW */
        break;
      case 100:
#line 268 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  sqliteSelect(pParse, yymsp[0].minor.yy207, SRT_Callback, 0, 0, 0, 0);
  sqliteSelectDelete(yymsp[0].minor.yy207);
}
#line 5431 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 101:
#line 278 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy207 = yymsp[0].minor.yy207;}
#line 5436 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 102:
#line 279 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  if( yymsp[0].minor.yy207 ){
    yymsp[0].minor.yy207->op = yymsp[-1].minor.yy136;
    yymsp[0].minor.yy207->pPrior = yymsp[-2].minor.yy207;
  }
  yygotominor.yy207 = yymsp[0].minor.yy207;
}
#line 5447 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 103:
#line 287 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = TK_UNION;}
#line 5452 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UNION */
        break;
      case 104:
#line 288 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = TK_ALL;}
#line 5458 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UNION */
        /* No destructor defined for ALL */
        break;
      case 105:
#line 289 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = TK_INTERSECT;}
#line 5465 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INTERSECT */
        break;
      case 106:
#line 290 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = TK_EXCEPT;}
#line 5471 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for EXCEPT */
        break;
      case 107:
#line 292 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy207 = sqliteSelectNew(yymsp[-6].minor.yy168,yymsp[-5].minor.yy289,yymsp[-4].minor.yy272,yymsp[-3].minor.yy168,yymsp[-2].minor.yy272,yymsp[-1].minor.yy168,yymsp[-7].minor.yy136,yymsp[0].minor.yy336.limit,yymsp[0].minor.yy336.offset);
}
#line 5479 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for SELECT */
        break;
      case 108:
#line 300 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = 1;}
#line 5485 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DISTINCT */
        break;
      case 109:
#line 301 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = 0;}
#line 5491 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ALL */
        break;
      case 110:
#line 302 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = 0;}
#line 5497 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 111:
#line 313 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = yymsp[-1].minor.yy168;}
#line 5502 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 112:
#line 314 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = 0;}
#line 5508 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 113:
#line 315 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
   yygotominor.yy168 = sqliteExprListAppend(yymsp[-2].minor.yy168,yymsp[-1].minor.yy272,yymsp[0].minor.yy324.n?&yymsp[0].minor.yy324:0);
}
#line 5515 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 114:
#line 318 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy168 = sqliteExprListAppend(yymsp[-1].minor.yy168, sqliteExpr(TK_ALL, 0, 0, 0), 0);
}
#line 5522 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for STAR */
        break;
      case 115:
#line 321 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  Expr *pRight = sqliteExpr(TK_ALL, 0, 0, 0);
  Expr *pLeft = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy324);
  yygotominor.yy168 = sqliteExprListAppend(yymsp[-3].minor.yy168, sqliteExpr(TK_DOT, pLeft, pRight, 0), 0);
}
#line 5532 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DOT */
        /* No destructor defined for STAR */
        break;
      case 116:
#line 331 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy324 = yymsp[0].minor.yy324; }
#line 5539 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for AS */
        break;
      case 117:
#line 332 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy324 = yymsp[0].minor.yy324; }
#line 5545 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 118:
#line 333 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy324.n = 0; }
#line 5550 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 119:
#line 345 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy289 = sqliteMalloc(sizeof(*yygotominor.yy289));}
#line 5555 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 120:
#line 346 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy289 = yymsp[0].minor.yy289;}
#line 5560 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for FROM */
        break;
      case 121:
#line 351 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
   yygotominor.yy289 = yymsp[-1].minor.yy289;
   if( yygotominor.yy289 && yygotominor.yy289->nSrc>0 ) yygotominor.yy289->a[yygotominor.yy289->nSrc-1].jointype = yymsp[0].minor.yy136;
}
#line 5569 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 122:
#line 355 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy289 = 0;}
#line 5574 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 123:
#line 356 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy289 = sqliteSrcListAppend(yymsp[-4].minor.yy289,&yymsp[-3].minor.yy324);
  if( yymsp[-2].minor.yy324.n ) sqliteSrcListAddAlias(yygotominor.yy289,&yymsp[-2].minor.yy324);
  if( yymsp[-1].minor.yy272 ){
    if( yygotominor.yy289 && yygotominor.yy289->nSrc>1 ){ yygotominor.yy289->a[yygotominor.yy289->nSrc-2].pOn = yymsp[-1].minor.yy272; }
    else { sqliteExprDelete(yymsp[-1].minor.yy272); }
  }
  if( yymsp[0].minor.yy268 ){
    if( yygotominor.yy289 && yygotominor.yy289->nSrc>1 ){ yygotominor.yy289->a[yygotominor.yy289->nSrc-2].pUsing = yymsp[0].minor.yy268; }
    else { sqliteIdListDelete(yymsp[0].minor.yy268); }
  }
}
#line 5590 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 124:
#line 368 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy289 = sqliteSrcListAppend(yymsp[-6].minor.yy289,0);
  yygotominor.yy289->a[yygotominor.yy289->nSrc-1].pSelect = yymsp[-4].minor.yy207;
  if( yymsp[-2].minor.yy324.n ) sqliteSrcListAddAlias(yygotominor.yy289,&yymsp[-2].minor.yy324);
  if( yymsp[-1].minor.yy272 ){
    if( yygotominor.yy289 && yygotominor.yy289->nSrc>1 ){ yygotominor.yy289->a[yygotominor.yy289->nSrc-2].pOn = yymsp[-1].minor.yy272; }
    else { sqliteExprDelete(yymsp[-1].minor.yy272); }
  }
  if( yymsp[0].minor.yy268 ){
    if( yygotominor.yy289 && yygotominor.yy289->nSrc>1 ){ yygotominor.yy289->a[yygotominor.yy289->nSrc-2].pUsing = yymsp[0].minor.yy268; }
    else { sqliteIdListDelete(yymsp[0].minor.yy268); }
  }
}
#line 5607 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 125:
#line 384 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = JT_INNER; }
#line 5614 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 126:
#line 385 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = JT_INNER; }
#line 5620 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for JOIN */
        break;
      case 127:
#line 386 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = sqliteJoinType(pParse,&yymsp[-1].minor.yy0,0,0); }
#line 5626 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for JOIN */
        break;
      case 128:
#line 387 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = sqliteJoinType(pParse,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy324,0); }
#line 5632 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for JOIN */
        break;
      case 129:
#line 389 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = sqliteJoinType(pParse,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy324,&yymsp[-1].minor.yy324); }
#line 5638 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for JOIN */
        break;
      case 130:
#line 393 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = yymsp[0].minor.yy272;}
#line 5644 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ON */
        break;
      case 131:
#line 394 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = 0;}
#line 5650 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 132:
#line 398 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy268 = yymsp[-1].minor.yy268;}
#line 5655 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for USING */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 133:
#line 399 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy268 = 0;}
#line 5663 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 134:
#line 409 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = 0;}
#line 5668 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 135:
#line 410 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = yymsp[0].minor.yy168;}
#line 5673 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ORDER */
        /* No destructor defined for BY */
        break;
      case 136:
#line 411 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy168 = sqliteExprListAppend(yymsp[-4].minor.yy168,yymsp[-2].minor.yy272,0);
  if( yygotominor.yy168 ) yygotominor.yy168->a[yygotominor.yy168->nExpr-1].sortOrder = yymsp[-1].minor.yy136+yymsp[0].minor.yy136;
}
#line 5683 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 137:
#line 415 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy168 = sqliteExprListAppend(0,yymsp[-2].minor.yy272,0);
  if( yygotominor.yy168 ) yygotominor.yy168->a[0].sortOrder = yymsp[-1].minor.yy136+yymsp[0].minor.yy136;
}
#line 5692 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 138:
#line 419 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = yymsp[0].minor.yy272;}
#line 5697 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 139:
#line 424 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = SQLITE_SO_ASC;}
#line 5702 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ASC */
        break;
      case 140:
#line 425 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = SQLITE_SO_DESC;}
#line 5708 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DESC */
        break;
      case 141:
#line 426 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = SQLITE_SO_ASC;}
#line 5714 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 142:
#line 427 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = SQLITE_SO_UNK;}
#line 5719 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 143:
#line 428 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = sqliteCollateType(yymsp[0].minor.yy324.z, yymsp[0].minor.yy324.n);}
#line 5724 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COLLATE */
        break;
      case 144:
#line 432 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = 0;}
#line 5730 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 145:
#line 433 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = yymsp[0].minor.yy168;}
#line 5735 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for GROUP */
        /* No destructor defined for BY */
        break;
      case 146:
#line 437 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = 0;}
#line 5742 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 147:
#line 438 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = yymsp[0].minor.yy272;}
#line 5747 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for HAVING */
        break;
      case 148:
#line 441 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy336.limit = -1; yygotominor.yy336.offset = 0;}
#line 5753 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 149:
#line 442 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy336.limit = atoi(yymsp[0].minor.yy0.z); yygotominor.yy336.offset = 0;}
#line 5758 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LIMIT */
        break;
      case 150:
#line 444 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy336.limit = atoi(yymsp[-2].minor.yy0.z); yygotominor.yy336.offset = atoi(yymsp[0].minor.yy0.z);}
#line 5764 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LIMIT */
        /* No destructor defined for limit_sep */
        break;
      case 151:
        /* No destructor defined for OFFSET */
        break;
      case 152:
        /* No destructor defined for COMMA */
        break;
      case 153:
#line 451 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteDeleteFrom(pParse, &yymsp[-1].minor.yy324, yymsp[0].minor.yy272);}
#line 5777 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DELETE */
        /* No destructor defined for FROM */
        break;
      case 154:
#line 456 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = 0;}
#line 5784 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 155:
#line 457 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = yymsp[0].minor.yy272;}
#line 5789 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for WHERE */
        break;
      case 156:
#line 465 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteUpdate(pParse,&yymsp[-3].minor.yy324,yymsp[-1].minor.yy168,yymsp[0].minor.yy272,yymsp[-4].minor.yy136);}
#line 5795 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UPDATE */
        /* No destructor defined for SET */
        break;
      case 157:
#line 468 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = sqliteExprListAppend(yymsp[-4].minor.yy168,yymsp[0].minor.yy272,&yymsp[-2].minor.yy324);}
#line 5802 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        /* No destructor defined for EQ */
        break;
      case 158:
#line 469 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = sqliteExprListAppend(0,yymsp[0].minor.yy272,&yymsp[-2].minor.yy324);}
#line 5809 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for EQ */
        break;
      case 159:
#line 474 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteInsert(pParse, &yymsp[-5].minor.yy324, yymsp[-1].minor.yy168, 0, yymsp[-4].minor.yy268, yymsp[-7].minor.yy136);}
#line 5815 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INTO */
        /* No destructor defined for VALUES */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 160:
#line 476 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteInsert(pParse, &yymsp[-2].minor.yy324, 0, yymsp[0].minor.yy207, yymsp[-1].minor.yy268, yymsp[-4].minor.yy136);}
#line 5824 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INTO */
        break;
      case 161:
#line 479 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = yymsp[0].minor.yy136;}
#line 5830 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INSERT */
        break;
      case 162:
#line 480 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = OE_Replace;}
#line 5836 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for REPLACE */
        break;
      case 163:
#line 486 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = sqliteExprListAppend(yymsp[-2].minor.yy168,yymsp[0].minor.yy272,0);}
#line 5842 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 164:
#line 487 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = sqliteExprListAppend(0,yymsp[0].minor.yy272,0);}
#line 5848 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 165:
#line 494 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy268 = 0;}
#line 5853 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 166:
#line 495 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy268 = yymsp[-1].minor.yy268;}
#line 5858 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 167:
#line 496 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy268 = sqliteIdListAppend(yymsp[-2].minor.yy268,&yymsp[0].minor.yy324);}
#line 5865 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 168:
#line 497 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy268 = sqliteIdListAppend(0,&yymsp[0].minor.yy324);}
#line 5871 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 169:
#line 516 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = yymsp[-1].minor.yy272; sqliteExprSpan(yygotominor.yy272,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0); }
#line 5876 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 170:
#line 517 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_NULL, 0, 0, &yymsp[0].minor.yy0);}
#line 5881 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 171:
#line 518 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy0);}
#line 5886 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 172:
#line 519 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy0);}
#line 5891 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 173:
#line 520 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  Expr *temp1 = sqliteExpr(TK_ID, 0, 0, &yymsp[-2].minor.yy324);
  Expr *temp2 = sqliteExpr(TK_ID, 0, 0, &yymsp[0].minor.yy324);
  yygotominor.yy272 = sqliteExpr(TK_DOT, temp1, temp2, 0);
}
#line 5900 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DOT */
        break;
      case 174:
#line 526 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = yymsp[-1].minor.yy272; ExprSetProperty(yygotominor.yy272,EP_Oracle8Join);}
#line 5906 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ORACLE_OUTER_JOIN */
        break;
      case 175:
#line 527 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_INTEGER, 0, 0, &yymsp[0].minor.yy0);}
#line 5912 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 176:
#line 528 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_FLOAT, 0, 0, &yymsp[0].minor.yy0);}
#line 5917 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 177:
#line 529 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_STRING, 0, 0, &yymsp[0].minor.yy0);}
#line 5922 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 178:
#line 530 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExprFunction(yymsp[-1].minor.yy168, &yymsp[-3].minor.yy0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);
}
#line 5930 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        break;
      case 179:
#line 534 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExprFunction(0, &yymsp[-3].minor.yy0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);
}
#line 5939 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for STAR */
        break;
      case 180:
#line 538 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_AND, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 5946 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for AND */
        break;
      case 181:
#line 539 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_OR, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 5952 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for OR */
        break;
      case 182:
#line 540 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_LT, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 5958 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LT */
        break;
      case 183:
#line 541 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_GT, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 5964 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for GT */
        break;
      case 184:
#line 542 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_LE, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 5970 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LE */
        break;
      case 185:
#line 543 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_GE, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 5976 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for GE */
        break;
      case 186:
#line 544 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_NE, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 5982 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NE */
        break;
      case 187:
#line 545 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_EQ, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 5988 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for EQ */
        break;
      case 188:
#line 546 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_BITAND, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 5994 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for BITAND */
        break;
      case 189:
#line 547 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_BITOR, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 6000 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for BITOR */
        break;
      case 190:
#line 548 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_LSHIFT, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 6006 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LSHIFT */
        break;
      case 191:
#line 549 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_RSHIFT, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 6012 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for RSHIFT */
        break;
      case 192:
#line 550 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[0].minor.yy272, 0);
  pList = sqliteExprListAppend(pList, yymsp[-2].minor.yy272, 0);
  yygotominor.yy272 = sqliteExprFunction(pList, 0);
  if( yygotominor.yy272 ) yygotominor.yy272->op = yymsp[-1].minor.yy136;
  sqliteExprSpan(yygotominor.yy272, &yymsp[-2].minor.yy272->span, &yymsp[0].minor.yy272->span);
}
#line 6024 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 193:
#line 557 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[0].minor.yy272, 0);
  pList = sqliteExprListAppend(pList, yymsp[-3].minor.yy272, 0);
  yygotominor.yy272 = sqliteExprFunction(pList, 0);
  if( yygotominor.yy272 ) yygotominor.yy272->op = yymsp[-1].minor.yy136;
  yygotominor.yy272 = sqliteExpr(TK_NOT, yygotominor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-3].minor.yy272->span,&yymsp[0].minor.yy272->span);
}
#line 6036 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        break;
      case 194:
#line 566 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = TK_LIKE;}
#line 6042 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LIKE */
        break;
      case 195:
#line 567 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy136 = TK_GLOB;}
#line 6048 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for GLOB */
        break;
      case 196:
#line 568 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_PLUS, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 6054 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PLUS */
        break;
      case 197:
#line 569 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_MINUS, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 6060 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for MINUS */
        break;
      case 198:
#line 570 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_STAR, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 6066 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for STAR */
        break;
      case 199:
#line 571 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_SLASH, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 6072 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for SLASH */
        break;
      case 200:
#line 572 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_REM, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 6078 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for REM */
        break;
      case 201:
#line 573 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = sqliteExpr(TK_CONCAT, yymsp[-2].minor.yy272, yymsp[0].minor.yy272, 0);}
#line 6084 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for CONCAT */
        break;
      case 202:
#line 574 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_ISNULL, yymsp[-1].minor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-1].minor.yy272->span,&yymsp[0].minor.yy0);
}
#line 6093 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 203:
#line 578 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_ISNULL, yymsp[-2].minor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-2].minor.yy272->span,&yymsp[0].minor.yy0);
}
#line 6101 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for IS */
        break;
      case 204:
#line 582 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_NOTNULL, yymsp[-1].minor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-1].minor.yy272->span,&yymsp[0].minor.yy0);
}
#line 6110 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 205:
#line 586 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_NOTNULL, yymsp[-2].minor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-2].minor.yy272->span,&yymsp[0].minor.yy0);
}
#line 6118 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        break;
      case 206:
#line 590 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_NOTNULL, yymsp[-3].minor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-3].minor.yy272->span,&yymsp[0].minor.yy0);
}
#line 6127 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for IS */
        /* No destructor defined for NOT */
        break;
      case 207:
#line 594 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_NOT, yymsp[0].minor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy272->span);
}
#line 6137 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 208:
#line 598 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_BITNOT, yymsp[0].minor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy272->span);
}
#line 6145 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 209:
#line 602 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_UMINUS, yymsp[0].minor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy272->span);
}
#line 6153 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 210:
#line 606 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_UPLUS, yymsp[0].minor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy272->span);
}
#line 6161 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 211:
#line 610 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_SELECT, 0, 0, 0);
  if( yygotominor.yy272 ) yygotominor.yy272->pSelect = yymsp[-1].minor.yy207;
  sqliteExprSpan(yygotominor.yy272,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0);
}
#line 6170 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 212:
#line 615 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[-2].minor.yy272, 0);
  pList = sqliteExprListAppend(pList, yymsp[0].minor.yy272, 0);
  yygotominor.yy272 = sqliteExpr(TK_BETWEEN, yymsp[-4].minor.yy272, 0, 0);
  if( yygotominor.yy272 ) yygotominor.yy272->pList = pList;
  sqliteExprSpan(yygotominor.yy272,&yymsp[-4].minor.yy272->span,&yymsp[0].minor.yy272->span);
}
#line 6181 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for BETWEEN */
        /* No destructor defined for AND */
        break;
      case 213:
#line 622 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  ExprList *pList = sqliteExprListAppend(0, yymsp[-2].minor.yy272, 0);
  pList = sqliteExprListAppend(pList, yymsp[0].minor.yy272, 0);
  yygotominor.yy272 = sqliteExpr(TK_BETWEEN, yymsp[-5].minor.yy272, 0, 0);
  if( yygotominor.yy272 ) yygotominor.yy272->pList = pList;
  yygotominor.yy272 = sqliteExpr(TK_NOT, yygotominor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-5].minor.yy272->span,&yymsp[0].minor.yy272->span);
}
#line 6195 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        /* No destructor defined for BETWEEN */
        /* No destructor defined for AND */
        break;
      case 214:
#line 630 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_IN, yymsp[-4].minor.yy272, 0, 0);
  if( yygotominor.yy272 ) yygotominor.yy272->pList = yymsp[-1].minor.yy168;
  sqliteExprSpan(yygotominor.yy272,&yymsp[-4].minor.yy272->span,&yymsp[0].minor.yy0);
}
#line 6207 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for IN */
        /* No destructor defined for LP */
        break;
      case 215:
#line 635 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_IN, yymsp[-4].minor.yy272, 0, 0);
  if( yygotominor.yy272 ) yygotominor.yy272->pSelect = yymsp[-1].minor.yy207;
  sqliteExprSpan(yygotominor.yy272,&yymsp[-4].minor.yy272->span,&yymsp[0].minor.yy0);
}
#line 6218 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for IN */
        /* No destructor defined for LP */
        break;
      case 216:
#line 640 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_IN, yymsp[-5].minor.yy272, 0, 0);
  if( yygotominor.yy272 ) yygotominor.yy272->pList = yymsp[-1].minor.yy168;
  yygotominor.yy272 = sqliteExpr(TK_NOT, yygotominor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-5].minor.yy272->span,&yymsp[0].minor.yy0);
}
#line 6230 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        /* No destructor defined for IN */
        /* No destructor defined for LP */
        break;
      case 217:
#line 646 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_IN, yymsp[-5].minor.yy272, 0, 0);
  if( yygotominor.yy272 ) yygotominor.yy272->pSelect = yymsp[-1].minor.yy207;
  yygotominor.yy272 = sqliteExpr(TK_NOT, yygotominor.yy272, 0, 0);
  sqliteExprSpan(yygotominor.yy272,&yymsp[-5].minor.yy272->span,&yymsp[0].minor.yy0);
}
#line 6243 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for NOT */
        /* No destructor defined for IN */
        /* No destructor defined for LP */
        break;
      case 218:
#line 654 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_CASE, yymsp[-3].minor.yy272, yymsp[-1].minor.yy272, 0);
  if( yygotominor.yy272 ) yygotominor.yy272->pList = yymsp[-2].minor.yy168;
  sqliteExprSpan(yygotominor.yy272, &yymsp[-4].minor.yy0, &yymsp[0].minor.yy0);
}
#line 6255 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 219:
#line 661 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy168 = sqliteExprListAppend(yymsp[-4].minor.yy168, yymsp[-2].minor.yy272, 0);
  yygotominor.yy168 = sqliteExprListAppend(yygotominor.yy168, yymsp[0].minor.yy272, 0);
}
#line 6263 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for WHEN */
        /* No destructor defined for THEN */
        break;
      case 220:
#line 665 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy168 = sqliteExprListAppend(0, yymsp[-2].minor.yy272, 0);
  yygotominor.yy168 = sqliteExprListAppend(yygotominor.yy168, yymsp[0].minor.yy272, 0);
}
#line 6273 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for WHEN */
        /* No destructor defined for THEN */
        break;
      case 221:
#line 670 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = yymsp[0].minor.yy272;}
#line 6280 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for ELSE */
        break;
      case 222:
#line 671 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = 0;}
#line 6286 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 223:
#line 673 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = yymsp[0].minor.yy272;}
#line 6291 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 224:
#line 674 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = 0;}
#line 6296 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 225:
#line 682 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = sqliteExprListAppend(yymsp[-2].minor.yy168,yymsp[0].minor.yy272,0);}
#line 6301 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 226:
#line 683 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy168 = sqliteExprListAppend(0,yymsp[0].minor.yy272,0);}
#line 6307 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 227:
#line 684 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = yymsp[0].minor.yy272;}
#line 6312 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 228:
#line 685 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy272 = 0;}
#line 6317 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 229:
#line 690 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  if( yymsp[-8].minor.yy136!=OE_None ) yymsp[-8].minor.yy136 = yymsp[0].minor.yy136;
  if( yymsp[-8].minor.yy136==OE_Default) yymsp[-8].minor.yy136 = OE_Abort;
  sqliteCreateIndex(pParse, &yymsp[-6].minor.yy324, &yymsp[-4].minor.yy324, yymsp[-2].minor.yy268, yymsp[-8].minor.yy136, &yymsp[-9].minor.yy0, &yymsp[-1].minor.yy0);
}
#line 6326 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INDEX */
        /* No destructor defined for ON */
        /* No destructor defined for LP */
        break;
      case 230:
#line 697 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_Abort; }
#line 6334 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UNIQUE */
        break;
      case 231:
#line 698 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = OE_None; }
#line 6340 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 232:
#line 706 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy268 = 0;}
#line 6345 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 233:
#line 707 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy268 = yymsp[-1].minor.yy268;}
#line 6350 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 234:
#line 708 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy268 = sqliteIdListAppend(yymsp[-2].minor.yy268,&yymsp[0].minor.yy324);}
#line 6357 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COMMA */
        break;
      case 235:
#line 709 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy268 = sqliteIdListAppend(0,&yymsp[0].minor.yy324);}
#line 6363 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 236:
#line 710 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy324;}
#line 6368 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 237:
#line 715 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteDropIndex(pParse, &yymsp[0].minor.yy324);}
#line 6373 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DROP */
        /* No destructor defined for INDEX */
        break;
      case 238:
#line 721 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteCopy(pParse,&yymsp[-5].minor.yy324,&yymsp[-3].minor.yy324,&yymsp[0].minor.yy0,yymsp[-6].minor.yy136);}
#line 6380 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COPY */
        /* No destructor defined for FROM */
        /* No destructor defined for USING */
        /* No destructor defined for DELIMITERS */
        break;
      case 239:
#line 723 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteCopy(pParse,&yymsp[-2].minor.yy324,&yymsp[0].minor.yy324,0,yymsp[-3].minor.yy136);}
#line 6389 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for COPY */
        /* No destructor defined for FROM */
        break;
      case 240:
#line 727 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteVacuum(pParse,0);}
#line 6396 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for VACUUM */
        break;
      case 241:
#line 728 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqliteVacuum(pParse,&yymsp[0].minor.yy324);}
#line 6402 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for VACUUM */
        break;
      case 242:
#line 732 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy324,&yymsp[0].minor.yy324,0);}
#line 6408 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        /* No destructor defined for EQ */
        break;
      case 243:
#line 733 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy324,&yymsp[0].minor.yy0,0);}
#line 6415 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        /* No destructor defined for EQ */
        break;
      case 244:
#line 734 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy324,&yymsp[0].minor.yy324,0);}
#line 6422 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        /* No destructor defined for EQ */
        break;
      case 245:
#line 735 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-2].minor.yy324,&yymsp[0].minor.yy324,1);}
#line 6429 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        /* No destructor defined for EQ */
        break;
      case 246:
#line 736 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[-3].minor.yy324,&yymsp[-1].minor.yy324,0);}
#line 6436 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 247:
#line 737 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{sqlitePragma(pParse,&yymsp[0].minor.yy324,&yymsp[0].minor.yy324,0);}
#line 6444 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for PRAGMA */
        break;
      case 248:
#line 738 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy324;}
#line 6450 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for plus_opt */
        break;
      case 249:
#line 739 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy324;}
#line 6456 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for MINUS */
        break;
      case 250:
#line 740 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy0;}
#line 6462 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 251:
#line 741 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy324 = yymsp[0].minor.yy0;}
#line 6467 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 252:
        /* No destructor defined for PLUS */
        break;
      case 253:
        break;
      case 254:
#line 748 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  Token all;
  all.z = yymsp[-11].minor.yy0.z;
  all.n = (yymsp[0].minor.yy0.z - yymsp[-11].minor.yy0.z) + yymsp[0].minor.yy0.n;
  sqliteCreateTrigger(pParse, &yymsp[-9].minor.yy324, yymsp[-8].minor.yy136, yymsp[-7].minor.yy72.a, yymsp[-7].minor.yy72.b, &yymsp[-5].minor.yy324, yymsp[-4].minor.yy136, yymsp[-3].minor.yy176, yymsp[-1].minor.yy209, &all);
}
#line 6482 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for TRIGGER */
        /* No destructor defined for ON */
        /* No destructor defined for BEGIN */
        break;
      case 255:
#line 756 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = TK_BEFORE; }
#line 6490 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for BEFORE */
        break;
      case 256:
#line 757 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = TK_AFTER;  }
#line 6496 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for AFTER */
        break;
      case 257:
#line 758 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = TK_INSTEAD;}
#line 6502 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INSTEAD */
        /* No destructor defined for OF */
        break;
      case 258:
#line 759 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = TK_BEFORE; }
#line 6509 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 259:
#line 763 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy72.a = TK_DELETE; yygotominor.yy72.b = 0; }
#line 6514 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DELETE */
        break;
      case 260:
#line 764 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy72.a = TK_INSERT; yygotominor.yy72.b = 0; }
#line 6520 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INSERT */
        break;
      case 261:
#line 765 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy72.a = TK_UPDATE; yygotominor.yy72.b = 0;}
#line 6526 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UPDATE */
        break;
      case 262:
#line 766 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy72.a = TK_UPDATE; yygotominor.yy72.b = yymsp[0].minor.yy268; }
#line 6532 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UPDATE */
        /* No destructor defined for OF */
        break;
      case 263:
#line 769 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = TK_ROW; }
#line 6539 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 264:
#line 770 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = TK_ROW; }
#line 6544 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for FOR */
        /* No destructor defined for EACH */
        /* No destructor defined for ROW */
        break;
      case 265:
#line 771 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy136 = TK_STATEMENT; }
#line 6552 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for FOR */
        /* No destructor defined for EACH */
        /* No destructor defined for STATEMENT */
        break;
      case 266:
#line 774 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy176 = 0; }
#line 6560 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 267:
#line 775 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy176 = yymsp[0].minor.yy272; }
#line 6565 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for WHEN */
        break;
      case 268:
#line 778 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yymsp[-2].minor.yy209->pNext = yymsp[0].minor.yy209 ; yygotominor.yy209 = yymsp[-2].minor.yy209; }
#line 6572 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for SEMI */
        break;
      case 269:
#line 780 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy209 = 0; }
#line 6578 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 270:
#line 785 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{ yygotominor.yy209 = sqliteTriggerUpdateStep(&yymsp[-3].minor.yy324, yymsp[-1].minor.yy168, yymsp[0].minor.yy272, yymsp[-4].minor.yy136); }
#line 6583 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for UPDATE */
        /* No destructor defined for SET */
        break;
      case 271:
#line 790 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy209 = sqliteTriggerInsertStep(&yymsp[-5].minor.yy324, yymsp[-4].minor.yy268, yymsp[-1].minor.yy168, 0, yymsp[-7].minor.yy136);}
#line 6590 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INSERT */
        /* No destructor defined for INTO */
        /* No destructor defined for VALUES */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 272:
#line 793 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy209 = sqliteTriggerInsertStep(&yymsp[-2].minor.yy324, yymsp[-1].minor.yy268, 0, yymsp[0].minor.yy207, yymsp[-4].minor.yy136);}
#line 6600 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for INSERT */
        /* No destructor defined for INTO */
        break;
      case 273:
#line 797 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy209 = sqliteTriggerDeleteStep(&yymsp[-1].minor.yy324, yymsp[0].minor.yy272);}
#line 6607 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DELETE */
        /* No destructor defined for FROM */
        break;
      case 274:
#line 800 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{yygotominor.yy209 = sqliteTriggerSelectStep(yymsp[0].minor.yy207); }
#line 6614 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        break;
      case 275:
#line 803 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_RAISE, 0, 0, 0); 
  yygotominor.yy272->iColumn = OE_Ignore;
  sqliteExprSpan(yygotominor.yy272, &yymsp[-3].minor.yy0, &yymsp[0].minor.yy0);
}
#line 6623 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for IGNORE */
        break;
      case 276:
#line 808 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy324); 
  yygotominor.yy272->iColumn = OE_Rollback;
  sqliteExprSpan(yygotominor.yy272, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 6634 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for ROLLBACK */
        /* No destructor defined for COMMA */
        break;
      case 277:
#line 813 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy324); 
  yygotominor.yy272->iColumn = OE_Abort;
  sqliteExprSpan(yygotominor.yy272, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 6646 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for ABORT */
        /* No destructor defined for COMMA */
        break;
      case 278:
#line 818 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
  yygotominor.yy272 = sqliteExpr(TK_RAISE, 0, 0, &yymsp[-1].minor.yy324); 
  yygotominor.yy272->iColumn = OE_Fail;
  sqliteExprSpan(yygotominor.yy272, &yymsp[-5].minor.yy0, &yymsp[0].minor.yy0);
}
#line 6658 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for FAIL */
        /* No destructor defined for COMMA */
        break;
      case 279:
#line 825 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"
{
    sqliteDropTrigger(pParse,&yymsp[0].minor.yy324,0);
}
#line 6668 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
        /* No destructor defined for DROP */
        /* No destructor defined for TRIGGER */
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
#line 23 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.y"

  if( pParse->zErrMsg==0 ){
    if( TOKEN.z[0] ){
      sqliteSetNString(&pParse->zErrMsg, 
          "near \"", -1, TOKEN.z, TOKEN.n, "\": syntax error", -1, 0);
    }else{
      sqliteSetString(&pParse->zErrMsg, "incomplete SQL statement", 0);
    }
  }
  pParse->nErr++;

#line 6725 "/home/wez/src/php/pear/PECL/sqlite/libsqlite/src/parse.c"
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
