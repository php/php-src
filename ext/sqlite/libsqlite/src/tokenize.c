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
  int len;                 /* Number of characters in the keyword */
  int tokenType;           /* The token value for this keyword */
  Keyword *pNext;          /* Next keyword with the same hash */
};

/*
** These are the keywords
*/
static Keyword aKeywordTable[] = {
  { "ABORT",             0, TK_ABORT,            0 },
  { "AFTER",             0, TK_AFTER,            0 },
  { "ALL",               0, TK_ALL,              0 },
  { "AND",               0, TK_AND,              0 },
  { "AS",                0, TK_AS,               0 },
  { "ASC",               0, TK_ASC,              0 },
  { "BEFORE",            0, TK_BEFORE,           0 },
  { "BEGIN",             0, TK_BEGIN,            0 },
  { "BETWEEN",           0, TK_BETWEEN,          0 },
  { "BY",                0, TK_BY,               0 },
  { "CASCADE",           0, TK_CASCADE,          0 },
  { "CASE",              0, TK_CASE,             0 },
  { "CHECK",             0, TK_CHECK,            0 },
  { "CLUSTER",           0, TK_CLUSTER,          0 },
  { "COLLATE",           0, TK_COLLATE,          0 },
  { "COMMIT",            0, TK_COMMIT,           0 },
  { "CONFLICT",          0, TK_CONFLICT,         0 },
  { "CONSTRAINT",        0, TK_CONSTRAINT,       0 },
  { "COPY",              0, TK_COPY,             0 },
  { "CREATE",            0, TK_CREATE,           0 },
  { "CROSS",             0, TK_JOIN_KW,          0 },
  { "DEFAULT",           0, TK_DEFAULT,          0 },
  { "DEFERRED",          0, TK_DEFERRED,         0 },
  { "DEFERRABLE",        0, TK_DEFERRABLE,       0 },
  { "DELETE",            0, TK_DELETE,           0 },
  { "DELIMITERS",        0, TK_DELIMITERS,       0 },
  { "DESC",              0, TK_DESC,             0 },
  { "DISTINCT",          0, TK_DISTINCT,         0 },
  { "DROP",              0, TK_DROP,             0 },
  { "END",               0, TK_END,              0 },
  { "EACH",              0, TK_EACH,             0 },
  { "ELSE",              0, TK_ELSE,             0 },
  { "EXCEPT",            0, TK_EXCEPT,           0 },
  { "EXPLAIN",           0, TK_EXPLAIN,          0 },
  { "FAIL",              0, TK_FAIL,             0 },
  { "FOR",               0, TK_FOR,              0 },
  { "FOREIGN",           0, TK_FOREIGN,          0 },
  { "FROM",              0, TK_FROM,             0 },
  { "FULL",              0, TK_JOIN_KW,          0 },
  { "GLOB",              0, TK_GLOB,             0 },
  { "GROUP",             0, TK_GROUP,            0 },
  { "HAVING",            0, TK_HAVING,           0 },
  { "IGNORE",            0, TK_IGNORE,           0 },
  { "IMMEDIATE",         0, TK_IMMEDIATE,        0 },
  { "IN",                0, TK_IN,               0 },
  { "INDEX",             0, TK_INDEX,            0 },
  { "INITIALLY",         0, TK_INITIALLY,        0 },
  { "INNER",             0, TK_JOIN_KW,          0 },
  { "INSERT",            0, TK_INSERT,           0 },
  { "INSTEAD",           0, TK_INSTEAD,          0 },
  { "INTERSECT",         0, TK_INTERSECT,        0 },
  { "INTO",              0, TK_INTO,             0 },
  { "IS",                0, TK_IS,               0 },
  { "ISNULL",            0, TK_ISNULL,           0 },
  { "JOIN",              0, TK_JOIN,             0 },
  { "KEY",               0, TK_KEY,              0 },
  { "LEFT",              0, TK_JOIN_KW,          0 },
  { "LIKE",              0, TK_LIKE,             0 },
  { "LIMIT",             0, TK_LIMIT,            0 },
  { "MATCH",             0, TK_MATCH,            0 },
  { "NATURAL",           0, TK_JOIN_KW,          0 },
  { "NOT",               0, TK_NOT,              0 },
  { "NOTNULL",           0, TK_NOTNULL,          0 },
  { "NULL",              0, TK_NULL,             0 },
  { "OF",                0, TK_OF,               0 },
  { "OFFSET",            0, TK_OFFSET,           0 },
  { "ON",                0, TK_ON,               0 },
  { "OR",                0, TK_OR,               0 },
  { "ORDER",             0, TK_ORDER,            0 },
  { "OUTER",             0, TK_JOIN_KW,          0 },
  { "PRAGMA",            0, TK_PRAGMA,           0 },
  { "PRIMARY",           0, TK_PRIMARY,          0 },
  { "RAISE",             0, TK_RAISE,            0 },
  { "REFERENCES",        0, TK_REFERENCES,       0 },
  { "REPLACE",           0, TK_REPLACE,          0 },
  { "RESTRICT",          0, TK_RESTRICT,         0 },
  { "RIGHT",             0, TK_JOIN_KW,          0 },
  { "ROLLBACK",          0, TK_ROLLBACK,         0 },
  { "ROW",               0, TK_ROW,              0 },
  { "SELECT",            0, TK_SELECT,           0 },
  { "SET",               0, TK_SET,              0 },
  { "STATEMENT",         0, TK_STATEMENT,        0 },
  { "TABLE",             0, TK_TABLE,            0 },
  { "TEMP",              0, TK_TEMP,             0 },
  { "TEMPORARY",         0, TK_TEMP,             0 },
  { "THEN",              0, TK_THEN,             0 },
  { "TRANSACTION",       0, TK_TRANSACTION,      0 },
  { "TRIGGER",           0, TK_TRIGGER,          0 },
  { "UNION",             0, TK_UNION,            0 },
  { "UNIQUE",            0, TK_UNIQUE,           0 },
  { "UPDATE",            0, TK_UPDATE,           0 },
  { "USING",             0, TK_USING,            0 },
  { "VACUUM",            0, TK_VACUUM,           0 },
  { "VALUES",            0, TK_VALUES,           0 },
  { "VIEW",              0, TK_VIEW,             0 },
  { "WHEN",              0, TK_WHEN,             0 },
  { "WHERE",             0, TK_WHERE,            0 },
};

/*
** This is the hash table
*/
#define KEY_HASH_SIZE 71
static Keyword *apHashTable[KEY_HASH_SIZE];


/*
** This function looks up an identifier to determine if it is a
** keyword.  If it is a keyword, the token code of that keyword is 
** returned.  If the input is not a keyword, TK_ID is returned.
*/
int sqliteKeywordCode(const char *z, int n){
  int h;
  Keyword *p;
  if( aKeywordTable[0].len==0 ){
    /* Initialize the keyword hash table */
    sqliteOsEnterMutex();
    if( aKeywordTable[0].len==0 ){
      int i;
      int n;
      n = sizeof(aKeywordTable)/sizeof(aKeywordTable[0]);
      for(i=0; i<n; i++){
        aKeywordTable[i].len = strlen(aKeywordTable[i].zName);
        h = sqliteHashNoCase(aKeywordTable[i].zName, aKeywordTable[i].len);
        h %= KEY_HASH_SIZE;
        aKeywordTable[i].pNext = apHashTable[h];
        apHashTable[h] = &aKeywordTable[i];
      }
    }
    sqliteOsLeaveMutex();
  }
  h = sqliteHashNoCase(z, n) % KEY_HASH_SIZE;
  for(p=apHashTable[h]; p; p=p->pNext){
    if( p->len==n && sqliteStrNICmp(p->zName, z, n)==0 ){
      return p->tokenType;
    }
  }
  return TK_ID;
}


/*
** If X is a character that can be used in an identifier then
** isIdChar[X] will be 1.  Otherwise isIdChar[X] will be 0.
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
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 8x */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 9x */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* Ax */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* Bx */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* Cx */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* Dx */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* Ex */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* Fx */
};


/*
** Return the length of the token that begins at z[0].  Return
** -1 if the token is (or might be) incomplete.  Store the token
** type in *tokenType before returning.
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
      if( z[1]==0 ) return -1;
      if( z[1]=='-' ){
        for(i=2; z[i] && z[i]!='\n'; i++){}
        *tokenType = TK_COMMENT;
        return i;
      }
      *tokenType = TK_MINUS;
      return 1;
    }
    case '(': {
      if( z[1]=='+' && z[2]==')' ){
        *tokenType = TK_ORACLE_OUTER_JOIN;
        return 3;
      }else{
        *tokenType = TK_LP;
        return 1;
      }
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
      if( !isdigit(z[1]) ){
        *tokenType = TK_DOT;
        return 1;
      }
      /* Fall thru into the next case */
    }
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
      *tokenType = TK_INTEGER;
      for(i=1; isdigit(z[i]); i++){}
      if( z[i]=='.' ){
        i++;
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
      }else if( z[0]=='.' ){
        *tokenType = TK_FLOAT;
      }
      return i;
    }
    case '[': {
      for(i=1; z[i] && z[i-1]!=']'; i++){}
      *tokenType = TK_ID;
      return i;
    }
    default: {
      if( !isIdChar[*z] ){
        break;
      }
      for(i=1; isIdChar[z[i]]; i++){}
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
    sqliteSetString(pzErrMsg, "out of memory", 0);
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
          sqliteSetString(pzErrMsg, "interrupt", 0);
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
    sqliteSetString(&pParse->zErrMsg, sqlite_error_string(pParse->rc), 0);
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
  if( pParse->pVdbe && (pParse->useCallback || pParse->nErr>0) ){
    sqliteVdbeDelete(pParse->pVdbe);
    pParse->pVdbe = 0;
  }
  if( pParse->pNewTable ){
    sqliteDeleteTable(pParse->db, pParse->pNewTable);
    pParse->pNewTable = 0;
  }
  if( nErr>0 && (pParse->rc==SQLITE_OK || pParse->rc==SQLITE_DONE) ){
    pParse->rc = SQLITE_ERROR;
  }
  return nErr;
}
