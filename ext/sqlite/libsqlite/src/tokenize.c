/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** An tokenizer for SQL
**
** This file contains C code that splits an SQL input string up into
** individual tokens and sends those tokens one-by-one over to the
** parser for analysis.
**
** $Id$
*/
#include "sqliteInt.h"
#include "os.h"
#include <ctype.h>
#include <stdlib.h>

/*
** All the keywords of the SQL language are stored as in a hash
** table composed of instances of the following structure.
*/
typedef struct Keyword Keyword;
struct Keyword {
  char *zName;             /* The keyword name */
  u8 tokenType;            /* Token value for this keyword */
  u8 len;                  /* Length of this keyword */
  u8 iNext;                /* Index in aKeywordTable[] of next with same hash */
};

/*
** These are the keywords
*/
static Keyword aKeywordTable[] = {
  { "ABORT",             TK_ABORT,        },
  { "AFTER",             TK_AFTER,        },
  { "ALL",               TK_ALL,          },
  { "AND",               TK_AND,          },
  { "AS",                TK_AS,           },
  { "ASC",               TK_ASC,          },
  { "ATTACH",            TK_ATTACH,       },
  { "BEFORE",            TK_BEFORE,       },
  { "BEGIN",             TK_BEGIN,        },
  { "BETWEEN",           TK_BETWEEN,      },
  { "BY",                TK_BY,           },
  { "CASCADE",           TK_CASCADE,      },
  { "CASE",              TK_CASE,         },
  { "CHECK",             TK_CHECK,        },
  { "CLUSTER",           TK_CLUSTER,      },
  { "COLLATE",           TK_COLLATE,      },
  { "COMMIT",            TK_COMMIT,       },
  { "CONFLICT",          TK_CONFLICT,     },
  { "CONSTRAINT",        TK_CONSTRAINT,   },
  { "COPY",              TK_COPY,         },
  { "CREATE",            TK_CREATE,       },
  { "CROSS",             TK_JOIN_KW,      },
  { "DATABASE",          TK_DATABASE,     },
  { "DEFAULT",           TK_DEFAULT,      },
  { "DEFERRED",          TK_DEFERRED,     },
  { "DEFERRABLE",        TK_DEFERRABLE,   },
  { "DELETE",            TK_DELETE,       },
  { "DELIMITERS",        TK_DELIMITERS,   },
  { "DESC",              TK_DESC,         },
  { "DETACH",            TK_DETACH,       },
  { "DISTINCT",          TK_DISTINCT,     },
  { "DROP",              TK_DROP,         },
  { "END",               TK_END,          },
  { "EACH",              TK_EACH,         },
  { "ELSE",              TK_ELSE,         },
  { "EXCEPT",            TK_EXCEPT,       },
  { "EXPLAIN",           TK_EXPLAIN,      },
  { "FAIL",              TK_FAIL,         },
  { "FOR",               TK_FOR,          },
  { "FOREIGN",           TK_FOREIGN,      },
  { "FROM",              TK_FROM,         },
  { "FULL",              TK_JOIN_KW,      },
  { "GLOB",              TK_GLOB,         },
  { "GROUP",             TK_GROUP,        },
  { "HAVING",            TK_HAVING,       },
  { "IGNORE",            TK_IGNORE,       },
  { "IMMEDIATE",         TK_IMMEDIATE,    },
  { "IN",                TK_IN,           },
  { "INDEX",             TK_INDEX,        },
  { "INITIALLY",         TK_INITIALLY,    },
  { "INNER",             TK_JOIN_KW,      },
  { "INSERT",            TK_INSERT,       },
  { "INSTEAD",           TK_INSTEAD,      },
  { "INTERSECT",         TK_INTERSECT,    },
  { "INTO",              TK_INTO,         },
  { "IS",                TK_IS,           },
  { "ISNULL",            TK_ISNULL,       },
  { "JOIN",              TK_JOIN,         },
  { "KEY",               TK_KEY,          },
  { "LEFT",              TK_JOIN_KW,      },
  { "LIKE",              TK_LIKE,         },
  { "LIMIT",             TK_LIMIT,        },
  { "MATCH",             TK_MATCH,        },
  { "NATURAL",           TK_JOIN_KW,      },
  { "NOT",               TK_NOT,          },
  { "NOTNULL",           TK_NOTNULL,      },
  { "NULL",              TK_NULL,         },
  { "OF",                TK_OF,           },
  { "OFFSET",            TK_OFFSET,       },
  { "ON",                TK_ON,           },
  { "OR",                TK_OR,           },
  { "ORDER",             TK_ORDER,        },
  { "OUTER",             TK_JOIN_KW,      },
  { "PRAGMA",            TK_PRAGMA,       },
  { "PRIMARY",           TK_PRIMARY,      },
  { "RAISE",             TK_RAISE,        },
  { "REFERENCES",        TK_REFERENCES,   },
  { "REPLACE",           TK_REPLACE,      },
  { "RESTRICT",          TK_RESTRICT,     },
  { "RIGHT",             TK_JOIN_KW,      },
  { "ROLLBACK",          TK_ROLLBACK,     },
  { "ROW",               TK_ROW,          },
  { "SELECT",            TK_SELECT,       },
  { "SET",               TK_SET,          },
  { "STATEMENT",         TK_STATEMENT,    },
  { "TABLE",             TK_TABLE,        },
  { "TEMP",              TK_TEMP,         },
  { "TEMPORARY",         TK_TEMP,         },
  { "THEN",              TK_THEN,         },
  { "TRANSACTION",       TK_TRANSACTION,  },
  { "TRIGGER",           TK_TRIGGER,      },
  { "UNION",             TK_UNION,        },
  { "UNIQUE",            TK_UNIQUE,       },
  { "UPDATE",            TK_UPDATE,       },
  { "USING",             TK_USING,        },
  { "VACUUM",            TK_VACUUM,       },
  { "VALUES",            TK_VALUES,       },
  { "VIEW",              TK_VIEW,         },
  { "WHEN",              TK_WHEN,         },
  { "WHERE",             TK_WHERE,        },
};

/*
** This is the hash table
*/
#define KEY_HASH_SIZE 101
static u8 aiHashTable[KEY_HASH_SIZE];


/*
** This function looks up an identifier to determine if it is a
** keyword.  If it is a keyword, the token code of that keyword is 
** returned.  If the input is not a keyword, TK_ID is returned.
*/
int sqliteKeywordCode(const char *z, int n){
  int h, i;
  Keyword *p;
  static char needInit = 1;
  if( needInit ){
    /* Initialize the keyword hash table */
    sqliteOsEnterMutex();
    if( needInit ){
      int nk;
      nk = sizeof(aKeywordTable)/sizeof(aKeywordTable[0]);
      for(i=0; i<nk; i++){
        aKeywordTable[i].len = strlen(aKeywordTable[i].zName);
        h = sqliteHashNoCase(aKeywordTable[i].zName, aKeywordTable[i].len);
        h %= KEY_HASH_SIZE;
        aKeywordTable[i].iNext = aiHashTable[h];
        aiHashTable[h] = i+1;
      }
      needInit = 0;
    }
    sqliteOsLeaveMutex();
  }
  h = sqliteHashNoCase(z, n) % KEY_HASH_SIZE;
  for(i=aiHashTable[h]; i; i=p->iNext){
    p = &aKeywordTable[i-1];
    if( p->len==n && sqliteStrNICmp(p->zName, z, n)==0 ){
      return p->tokenType;
    }
  }
  return TK_ID;
}


/*
** If X is a character that can be used in an identifier and
** X&0x80==0 then isIdChar[X] will be 1.  If X&0x80==0x80 then
** X is always an identifier character.  (Hence all UTF-8
** characters can be part of an identifier).  isIdChar[X] will
** be 0 for every character in the lower 128 ASCII characters
** that cannot be used as part of an identifier.
**
** In this implementation, an identifier can be a string of
** alphabetic characters, digits, and "_" plus any character
** with the high-order bit set.  The latter rule means that
** any sequence of UTF-8 characters or characters taken from
** an extended ISO8859 character set can form an identifier.
*/
static const char isIdChar[] = {
/* x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 0x */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 1x */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 2x */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  /* 3x */
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 4x */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,  /* 5x */
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 6x */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  /* 7x */
};


/*
** Return the length of the token that begins at z[0]. 
** Store the token type in *tokenType before returning.
*/
static int sqliteGetToken(const unsigned char *z, int *tokenType){
  int i;
  switch( *z ){
    case ' ': case '\t': case '\n': case '\f': case '\r': {
      for(i=1; isspace(z[i]); i++){}
      *tokenType = TK_SPACE;
      return i;
    }
    case '-': {
      if( z[1]=='-' ){
        for(i=2; z[i] && z[i]!='\n'; i++){}
        *tokenType = TK_COMMENT;
        return i;
      }
      *tokenType = TK_MINUS;
      return 1;
    }
    case '(': {
      *tokenType = TK_LP;
      return 1;
    }
    case ')': {
      *tokenType = TK_RP;
      return 1;
    }
    case ';': {
      *tokenType = TK_SEMI;
      return 1;
    }
    case '+': {
      *tokenType = TK_PLUS;
      return 1;
    }
    case '*': {
      *tokenType = TK_STAR;
      return 1;
    }
    case '/': {
      if( z[1]!='*' || z[2]==0 ){
        *tokenType = TK_SLASH;
        return 1;
      }
      for(i=3; z[i] && (z[i]!='/' || z[i-1]!='*'); i++){}
      if( z[i] ) i++;
      *tokenType = TK_COMMENT;
      return i;
    }
    case '%': {
      *tokenType = TK_REM;
      return 1;
    }
    case '=': {
      *tokenType = TK_EQ;
      return 1 + (z[1]=='=');
    }
    case '<': {
      if( z[1]=='=' ){
        *tokenType = TK_LE;
        return 2;
      }else if( z[1]=='>' ){
        *tokenType = TK_NE;
        return 2;
      }else if( z[1]=='<' ){
        *tokenType = TK_LSHIFT;
        return 2;
      }else{
        *tokenType = TK_LT;
        return 1;
      }
    }
    case '>': {
      if( z[1]=='=' ){
        *tokenType = TK_GE;
        return 2;
      }else if( z[1]=='>' ){
        *tokenType = TK_RSHIFT;
        return 2;
      }else{
        *tokenType = TK_GT;
        return 1;
      }
    }
    case '!': {
      if( z[1]!='=' ){
        *tokenType = TK_ILLEGAL;
        return 2;
      }else{
        *tokenType = TK_NE;
        return 2;
      }
    }
    case '|': {
      if( z[1]!='|' ){
        *tokenType = TK_BITOR;
        return 1;
      }else{
        *tokenType = TK_CONCAT;
        return 2;
      }
    }
    case ',': {
      *tokenType = TK_COMMA;
      return 1;
    }
    case '&': {
      *tokenType = TK_BITAND;
      return 1;
    }
    case '~': {
      *tokenType = TK_BITNOT;
      return 1;
    }
    case '\'': case '"': {
      int delim = z[0];
      for(i=1; z[i]; i++){
        if( z[i]==delim ){
          if( z[i+1]==delim ){
            i++;
          }else{
            break;
          }
        }
      }
      if( z[i] ) i++;
      *tokenType = TK_STRING;
      return i;
    }
    case '.': {
      *tokenType = TK_DOT;
      return 1;
    }
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
      *tokenType = TK_INTEGER;
      for(i=1; isdigit(z[i]); i++){}
      if( z[i]=='.' && isdigit(z[i+1]) ){
        i += 2;
        while( isdigit(z[i]) ){ i++; }
        *tokenType = TK_FLOAT;
      }
      if( (z[i]=='e' || z[i]=='E') &&
           ( isdigit(z[i+1]) 
            || ((z[i+1]=='+' || z[i+1]=='-') && isdigit(z[i+2]))
           )
      ){
        i += 2;
        while( isdigit(z[i]) ){ i++; }
        *tokenType = TK_FLOAT;
      }
      return i;
    }
    case '[': {
      for(i=1; z[i] && z[i-1]!=']'; i++){}
      *tokenType = TK_ID;
      return i;
    }
    case '?': {
      *tokenType = TK_VARIABLE;
      return 1;
    }
    default: {
      if( (*z&0x80)==0 && !isIdChar[*z] ){
        break;
      }
      for(i=1; (z[i]&0x80)!=0 || isIdChar[z[i]]; i++){}
      *tokenType = sqliteKeywordCode((char*)z, i);
      return i;
    }
  }
  *tokenType = TK_ILLEGAL;
  return 1;
}

/*
** Run the parser on the given SQL string.  The parser structure is
** passed in.  An SQLITE_ status code is returned.  If an error occurs
** and pzErrMsg!=NULL then an error message might be written into 
** memory obtained from malloc() and *pzErrMsg made to point to that
** error message.  Or maybe not.
*/
int sqliteRunParser(Parse *pParse, const char *zSql, char **pzErrMsg){
  int nErr = 0;
  int i;
  void *pEngine;
  int tokenType;
  int lastTokenParsed = -1;
  sqlite *db = pParse->db;
  extern void *sqliteParserAlloc(void*(*)(int));
  extern void sqliteParserFree(void*, void(*)(void*));
  extern int sqliteParser(void*, int, Token, Parse*);

  db->flags &= ~SQLITE_Interrupt;
  pParse->rc = SQLITE_OK;
  i = 0;
  pEngine = sqliteParserAlloc((void*(*)(int))malloc);
  if( pEngine==0 ){
    sqliteSetString(pzErrMsg, "out of memory", (char*)0);
    return 1;
  }
  pParse->sLastToken.dyn = 0;
  pParse->zTail = zSql;
  while( sqlite_malloc_failed==0 && zSql[i]!=0 ){
    assert( i>=0 );
    pParse->sLastToken.z = &zSql[i];
    assert( pParse->sLastToken.dyn==0 );
    pParse->sLastToken.n = sqliteGetToken((unsigned char*)&zSql[i], &tokenType);
    i += pParse->sLastToken.n;
    switch( tokenType ){
      case TK_SPACE:
      case TK_COMMENT: {
        if( (db->flags & SQLITE_Interrupt)!=0 ){
          pParse->rc = SQLITE_INTERRUPT;
          sqliteSetString(pzErrMsg, "interrupt", (char*)0);
          goto abort_parse;
        }
        break;
      }
      case TK_ILLEGAL: {
        sqliteSetNString(pzErrMsg, "unrecognized token: \"", -1, 
           pParse->sLastToken.z, pParse->sLastToken.n, "\"", 1, 0);
        nErr++;
        goto abort_parse;
      }
      case TK_SEMI: {
        pParse->zTail = &zSql[i];
        /* Fall thru into the default case */
      }
      default: {
        sqliteParser(pEngine, tokenType, pParse->sLastToken, pParse);
        lastTokenParsed = tokenType;
        if( pParse->rc!=SQLITE_OK ){
          goto abort_parse;
        }
        break;
      }
    }
  }
abort_parse:
  if( zSql[i]==0 && nErr==0 && pParse->rc==SQLITE_OK ){
    if( lastTokenParsed!=TK_SEMI ){
      sqliteParser(pEngine, TK_SEMI, pParse->sLastToken, pParse);
      pParse->zTail = &zSql[i];
    }
    sqliteParser(pEngine, 0, pParse->sLastToken, pParse);
  }
  sqliteParserFree(pEngine, free);
  if( pParse->rc!=SQLITE_OK && pParse->rc!=SQLITE_DONE && pParse->zErrMsg==0 ){
    sqliteSetString(&pParse->zErrMsg, sqlite_error_string(pParse->rc),
                    (char*)0);
  }
  if( pParse->zErrMsg ){
    if( pzErrMsg && *pzErrMsg==0 ){
      *pzErrMsg = pParse->zErrMsg;
    }else{
      sqliteFree(pParse->zErrMsg);
    }
    pParse->zErrMsg = 0;
    if( !nErr ) nErr++;
  }
  if( pParse->pVdbe && pParse->nErr>0 ){
    sqliteVdbeDelete(pParse->pVdbe);
    pParse->pVdbe = 0;
  }
  if( pParse->pNewTable ){
    sqliteDeleteTable(pParse->db, pParse->pNewTable);
    pParse->pNewTable = 0;
  }
  if( pParse->pNewTrigger ){
    sqliteDeleteTrigger(pParse->pNewTrigger);
    pParse->pNewTrigger = 0;
  }
  if( nErr>0 && (pParse->rc==SQLITE_OK || pParse->rc==SQLITE_DONE) ){
    pParse->rc = SQLITE_ERROR;
  }
  return nErr;
}

/*
** Token types used by the sqlite_complete() routine.  See the header
** comments on that procedure for additional information.
*/
#define tkEXPLAIN 0
#define tkCREATE  1
#define tkTEMP    2
#define tkTRIGGER 3
#define tkEND     4
#define tkSEMI    5
#define tkWS      6
#define tkOTHER   7

/*
** Return TRUE if the given SQL string ends in a semicolon.
**
** Special handling is require for CREATE TRIGGER statements.
** Whenever the CREATE TRIGGER keywords are seen, the statement
** must end with ";END;".
**
** This implementation uses a state machine with 7 states:
**
**   (0) START     At the beginning or end of an SQL statement.  This routine
**                 returns 1 if it ends in the START state and 0 if it ends
**                 in any other state.
**
**   (1) EXPLAIN   The keyword EXPLAIN has been seen at the beginning of 
**                 a statement.
**
**   (2) CREATE    The keyword CREATE has been seen at the beginning of a
**                 statement, possibly preceeded by EXPLAIN and/or followed by
**                 TEMP or TEMPORARY
**
**   (3) NORMAL    We are in the middle of statement which ends with a single
**                 semicolon.
**
**   (4) TRIGGER   We are in the middle of a trigger definition that must be
**                 ended by a semicolon, the keyword END, and another semicolon.
**
**   (5) SEMI      We've seen the first semicolon in the ";END;" that occurs at
**                 the end of a trigger definition.
**
**   (6) END       We've seen the ";END" of the ";END;" that occurs at the end
**                 of a trigger difinition.
**
** Transitions between states above are determined by tokens extracted
** from the input.  The following tokens are significant:
**
**   (0) tkEXPLAIN   The "explain" keyword.
**   (1) tkCREATE    The "create" keyword.
**   (2) tkTEMP      The "temp" or "temporary" keyword.
**   (3) tkTRIGGER   The "trigger" keyword.
**   (4) tkEND       The "end" keyword.
**   (5) tkSEMI      A semicolon.
**   (6) tkWS        Whitespace
**   (7) tkOTHER     Any other SQL token.
**
** Whitespace never causes a state transition and is always ignored.
*/
int sqlite_complete(const char *zSql){
  u8 state = 0;   /* Current state, using numbers defined in header comment */
  u8 token;       /* Value of the next token */

  /* The following matrix defines the transition from one state to another
  ** according to what token is seen.  trans[state][token] returns the
  ** next state.
  */
  static const u8 trans[7][8] = {
                     /* Token:                                                */
     /* State:       **  EXPLAIN  CREATE  TEMP  TRIGGER  END  SEMI  WS  OTHER */
     /* 0   START: */ {       1,      2,    3,       3,   3,    0,  0,     3, },
     /* 1 EXPLAIN: */ {       3,      2,    3,       3,   3,    0,  1,     3, },
     /* 2  CREATE: */ {       3,      3,    2,       4,   3,    0,  2,     3, },
     /* 3  NORMAL: */ {       3,      3,    3,       3,   3,    0,  3,     3, },
     /* 4 TRIGGER: */ {       4,      4,    4,       4,   4,    5,  4,     4, },
     /* 5    SEMI: */ {       4,      4,    4,       4,   6,    5,  5,     4, },
     /* 6     END: */ {       4,      4,    4,       4,   4,    0,  6,     4, },
  };

  while( *zSql ){
    switch( *zSql ){
      case ';': {  /* A semicolon */
        token = tkSEMI;
        break;
      }
      case ' ':
      case '\r':
      case '\t':
      case '\n':
      case '\f': {  /* White space is ignored */
        token = tkWS;
        break;
      }
      case '/': {   /* C-style comments */
        if( zSql[1]!='*' ){
          token = tkOTHER;
          break;
        }
        zSql += 2;
        while( zSql[0] && (zSql[0]!='*' || zSql[1]!='/') ){ zSql++; }
        if( zSql[0]==0 ) return 0;
        zSql++;
        token = tkWS;
        break;
      }
      case '-': {   /* SQL-style comments from "--" to end of line */
        if( zSql[1]!='-' ){
          token = tkOTHER;
          break;
        }
        while( *zSql && *zSql!='\n' ){ zSql++; }
        if( *zSql==0 ) return state==0;
        token = tkWS;
        break;
      }
      case '[': {   /* Microsoft-style identifiers in [...] */
        zSql++;
        while( *zSql && *zSql!=']' ){ zSql++; }
        if( *zSql==0 ) return 0;
        token = tkOTHER;
        break;
      }
      case '"':     /* single- and double-quoted strings */
      case '\'': {
        int c = *zSql;
        zSql++;
        while( *zSql && *zSql!=c ){ zSql++; }
        if( *zSql==0 ) return 0;
        token = tkOTHER;
        break;
      }
      default: {
        if( isIdChar[(u8)*zSql] ){
          /* Keywords and unquoted identifiers */
          int nId;
          for(nId=1; isIdChar[(u8)zSql[nId]]; nId++){}
          switch( *zSql ){
            case 'c': case 'C': {
              if( nId==6 && sqliteStrNICmp(zSql, "create", 6)==0 ){
                token = tkCREATE;
              }else{
                token = tkOTHER;
              }
              break;
            }
            case 't': case 'T': {
              if( nId==7 && sqliteStrNICmp(zSql, "trigger", 7)==0 ){
                token = tkTRIGGER;
              }else if( nId==4 && sqliteStrNICmp(zSql, "temp", 4)==0 ){
                token = tkTEMP;
              }else if( nId==9 && sqliteStrNICmp(zSql, "temporary", 9)==0 ){
                token = tkTEMP;
              }else{
                token = tkOTHER;
              }
              break;
            }
            case 'e':  case 'E': {
              if( nId==3 && sqliteStrNICmp(zSql, "end", 3)==0 ){
                token = tkEND;
              }else if( nId==7 && sqliteStrNICmp(zSql, "explain", 7)==0 ){
                token = tkEXPLAIN;
              }else{
                token = tkOTHER;
              }
              break;
            }
            default: {
              token = tkOTHER;
              break;
            }
          }
          zSql += nId-1;
        }else{
          /* Operators and special symbols */
          token = tkOTHER;
        }
        break;
      }
    }
    state = trans[state][token];
    zSql++;
  }
  return state==0;
}
