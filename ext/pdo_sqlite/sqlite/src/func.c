/*
** 2002 February 23
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains the C functions that implement various SQL
** functions of SQLite.  
**
** There is only one exported symbol in this file - the function
** sqliteRegisterBuildinFunctions() found at the bottom of the file.
** All other code has file scope.
**
** $Id$
*/
#include "sqliteInt.h"
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "vdbeInt.h"
#include "os.h"

/*
** Return the collating function associated with a function.
*/
static CollSeq *sqlite3GetFuncCollSeq(sqlite3_context *context){
  return context->pColl;
}

/*
** Implementation of the non-aggregate min() and max() functions
*/
static void minmaxFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  int i;
  int mask;    /* 0 for min() or 0xffffffff for max() */
  int iBest;
  CollSeq *pColl;

  if( argc==0 ) return;
  mask = sqlite3_user_data(context)==0 ? 0 : -1;
  pColl = sqlite3GetFuncCollSeq(context);
  assert( pColl );
  assert( mask==-1 || mask==0 );
  iBest = 0;
  if( sqlite3_value_type(argv[0])==SQLITE_NULL ) return;
  for(i=1; i<argc; i++){
    if( sqlite3_value_type(argv[i])==SQLITE_NULL ) return;
    if( (sqlite3MemCompare(argv[iBest], argv[i], pColl)^mask)>=0 ){
      iBest = i;
    }
  }
  sqlite3_result_value(context, argv[iBest]);
}

/*
** Return the type of the argument.
*/
static void typeofFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  const char *z = 0;
  switch( sqlite3_value_type(argv[0]) ){
    case SQLITE_NULL:    z = "null";    break;
    case SQLITE_INTEGER: z = "integer"; break;
    case SQLITE_TEXT:    z = "text";    break;
    case SQLITE_FLOAT:   z = "real";    break;
    case SQLITE_BLOB:    z = "blob";    break;
  }
  sqlite3_result_text(context, z, -1, SQLITE_STATIC);
}


/*
** Implementation of the length() function
*/
static void lengthFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  int len;

  assert( argc==1 );
  switch( sqlite3_value_type(argv[0]) ){
    case SQLITE_BLOB:
    case SQLITE_INTEGER:
    case SQLITE_FLOAT: {
      sqlite3_result_int(context, sqlite3_value_bytes(argv[0]));
      break;
    }
    case SQLITE_TEXT: {
      const char *z = sqlite3_value_text(argv[0]);
      for(len=0; *z; z++){ if( (0xc0&*z)!=0x80 ) len++; }
      sqlite3_result_int(context, len);
      break;
    }
    default: {
      sqlite3_result_null(context);
      break;
    }
  }
}

/*
** Implementation of the abs() function
*/
static void absFunc(sqlite3_context *context, int argc, sqlite3_value **argv){
  assert( argc==1 );
  switch( sqlite3_value_type(argv[0]) ){
    case SQLITE_INTEGER: {
      i64 iVal = sqlite3_value_int64(argv[0]);
      if( iVal<0 ) iVal = iVal * -1;
      sqlite3_result_int64(context, iVal);
      break;
    }
    case SQLITE_NULL: {
      sqlite3_result_null(context);
      break;
    }
    default: {
      double rVal = sqlite3_value_double(argv[0]);
      if( rVal<0 ) rVal = rVal * -1.0;
      sqlite3_result_double(context, rVal);
      break;
    }
  }
}

/*
** Implementation of the substr() function
*/
static void substrFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  const char *z;
  const char *z2;
  int i;
  int p1, p2, len;

  assert( argc==3 );
  z = sqlite3_value_text(argv[0]);
  if( z==0 ) return;
  p1 = sqlite3_value_int(argv[1]);
  p2 = sqlite3_value_int(argv[2]);
  for(len=0, z2=z; *z2; z2++){ if( (0xc0&*z2)!=0x80 ) len++; }
  if( p1<0 ){
    p1 += len;
    if( p1<0 ){
      p2 += p1;
      p1 = 0;
    }
  }else if( p1>0 ){
    p1--;
  }
  if( p1+p2>len ){
    p2 = len-p1;
  }
  for(i=0; i<p1 && z[i]; i++){
    if( (z[i]&0xc0)==0x80 ) p1++;
  }
  while( z[i] && (z[i]&0xc0)==0x80 ){ i++; p1++; }
  for(; i<p1+p2 && z[i]; i++){
    if( (z[i]&0xc0)==0x80 ) p2++;
  }
  while( z[i] && (z[i]&0xc0)==0x80 ){ i++; p2++; }
  if( p2<0 ) p2 = 0;
  sqlite3_result_text(context, &z[p1], p2, SQLITE_TRANSIENT);
}

/*
** Implementation of the round() function
*/
static void roundFunc(sqlite3_context *context, int argc, sqlite3_value **argv){
  int n = 0;
  double r;
  char zBuf[500];  /* larger than the %f representation of the largest double */
  assert( argc==1 || argc==2 );
  if( argc==2 ){
    if( SQLITE_NULL==sqlite3_value_type(argv[1]) ) return;
    n = sqlite3_value_int(argv[1]);
    if( n>30 ) n = 30;
    if( n<0 ) n = 0;
  }
  if( SQLITE_NULL==sqlite3_value_type(argv[0]) ) return;
  r = sqlite3_value_double(argv[0]);
  sqlite3_snprintf(sizeof(zBuf),zBuf,"%.*f",n,r);
  sqlite3_result_text(context, zBuf, -1, SQLITE_TRANSIENT);
}

/*
** Implementation of the upper() and lower() SQL functions.
*/
static void upperFunc(sqlite3_context *context, int argc, sqlite3_value **argv){
  unsigned char *z;
  int i;
  if( argc<1 || SQLITE_NULL==sqlite3_value_type(argv[0]) ) return;
  z = sqliteMalloc(sqlite3_value_bytes(argv[0])+1);
  if( z==0 ) return;
  strcpy(z, sqlite3_value_text(argv[0]));
  for(i=0; z[i]; i++){
    z[i] = toupper(z[i]);
  }
  sqlite3_result_text(context, z, -1, SQLITE_TRANSIENT);
  sqliteFree(z);
}
static void lowerFunc(sqlite3_context *context, int argc, sqlite3_value **argv){
  unsigned char *z;
  int i;
  if( argc<1 || SQLITE_NULL==sqlite3_value_type(argv[0]) ) return;
  z = sqliteMalloc(sqlite3_value_bytes(argv[0])+1);
  if( z==0 ) return;
  strcpy(z, sqlite3_value_text(argv[0]));
  for(i=0; z[i]; i++){
    z[i] = tolower(z[i]);
  }
  sqlite3_result_text(context, z, -1, SQLITE_TRANSIENT);
  sqliteFree(z);
}

/*
** Implementation of the IFNULL(), NVL(), and COALESCE() functions.  
** All three do the same thing.  They return the first non-NULL
** argument.
*/
static void ifnullFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  int i;
  for(i=0; i<argc; i++){
    if( SQLITE_NULL!=sqlite3_value_type(argv[i]) ){
      sqlite3_result_value(context, argv[i]);
      break;
    }
  }
}

/*
** Implementation of random().  Return a random integer.  
*/
static void randomFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  int r;
  sqlite3Randomness(sizeof(r), &r);
  sqlite3_result_int(context, r);
}

/*
** Implementation of the last_insert_rowid() SQL function.  The return
** value is the same as the sqlite3_last_insert_rowid() API function.
*/
static void last_insert_rowid(
  sqlite3_context *context, 
  int arg, 
  sqlite3_value **argv
){
  sqlite3 *db = sqlite3_user_data(context);
  sqlite3_result_int64(context, sqlite3_last_insert_rowid(db));
}

/*
** Implementation of the changes() SQL function.  The return value is the
** same as the sqlite3_changes() API function.
*/
static void changes(
  sqlite3_context *context,
  int arg,
  sqlite3_value **argv
){
  sqlite3 *db = sqlite3_user_data(context);
  sqlite3_result_int(context, sqlite3_changes(db));
}

/*
** Implementation of the total_changes() SQL function.  The return value is
** the same as the sqlite3_total_changes() API function.
*/
static void total_changes(
  sqlite3_context *context,
  int arg,
  sqlite3_value **argv
){
  sqlite3 *db = sqlite3_user_data(context);
  sqlite3_result_int(context, sqlite3_total_changes(db));
}

/*
** A structure defining how to do GLOB-style comparisons.
*/
struct compareInfo {
  u8 matchAll;
  u8 matchOne;
  u8 matchSet;
  u8 noCase;
};

static const struct compareInfo globInfo = { '*', '?', '[', 0 };
/* The correct SQL-92 behavior is for the LIKE operator to ignore
** case.  Thus  'a' LIKE 'A' would be true. */
static const struct compareInfo likeInfoNorm = { '%', '_',   0, 1 };
/* If SQLITE_CASE_SENSITIVE_LIKE is defined, then the LIKE operator
** is case sensitive causing 'a' LIKE 'A' to be false */
static const struct compareInfo likeInfoAlt = { '%', '_',   0, 0 };

/*
** X is a pointer to the first byte of a UTF-8 character.  Increment
** X so that it points to the next character.  This only works right
** if X points to a well-formed UTF-8 string.
*/
#define sqliteNextChar(X)  while( (0xc0&*++(X))==0x80 ){}
#define sqliteCharVal(X)   sqlite3ReadUtf8(X)


/*
** Compare two UTF-8 strings for equality where the first string can
** potentially be a "glob" expression.  Return true (1) if they
** are the same and false (0) if they are different.
**
** Globbing rules:
**
**      '*'       Matches any sequence of zero or more characters.
**
**      '?'       Matches exactly one character.
**
**     [...]      Matches one character from the enclosed list of
**                characters.
**
**     [^...]     Matches one character not in the enclosed list.
**
** With the [...] and [^...] matching, a ']' character can be included
** in the list by making it the first character after '[' or '^'.  A
** range of characters can be specified using '-'.  Example:
** "[a-z]" matches any single lower-case letter.  To match a '-', make
** it the last character in the list.
**
** This routine is usually quick, but can be N**2 in the worst case.
**
** Hints: to match '*' or '?', put them in "[]".  Like this:
**
**         abc[*]xyz        Matches "abc*xyz" only
*/
static int patternCompare(
  const u8 *zPattern,              /* The glob pattern */
  const u8 *zString,               /* The string to compare against the glob */
  const struct compareInfo *pInfo, /* Information about how to do the compare */
  const int esc                    /* The escape character */
){
  register int c;
  int invert;
  int seen;
  int c2;
  u8 matchOne = pInfo->matchOne;
  u8 matchAll = pInfo->matchAll;
  u8 matchSet = pInfo->matchSet;
  u8 noCase = pInfo->noCase; 
  int prevEscape = 0;     /* True if the previous character was 'escape' */

  while( (c = *zPattern)!=0 ){
    if( !prevEscape && c==matchAll ){
      while( (c=zPattern[1]) == matchAll || c == matchOne ){
        if( c==matchOne ){
          if( *zString==0 ) return 0;
          sqliteNextChar(zString);
        }
        zPattern++;
      }
      if( c && esc && sqlite3ReadUtf8(&zPattern[1])==esc ){
        u8 const *zTemp = &zPattern[1];
        sqliteNextChar(zTemp);
        c = *zTemp;
      }
      if( c==0 ) return 1;
      if( c==matchSet ){
        assert( esc==0 );   /* This is GLOB, not LIKE */
        while( *zString && patternCompare(&zPattern[1],zString,pInfo,esc)==0 ){
          sqliteNextChar(zString);
        }
        return *zString!=0;
      }else{
        while( (c2 = *zString)!=0 ){
          if( noCase ){
            c2 = sqlite3UpperToLower[c2];
            c = sqlite3UpperToLower[c];
            while( c2 != 0 && c2 != c ){ c2 = sqlite3UpperToLower[*++zString]; }
          }else{
            while( c2 != 0 && c2 != c ){ c2 = *++zString; }
          }
          if( c2==0 ) return 0;
          if( patternCompare(&zPattern[1],zString,pInfo,esc) ) return 1;
          sqliteNextChar(zString);
        }
        return 0;
      }
    }else if( !prevEscape && c==matchOne ){
      if( *zString==0 ) return 0;
      sqliteNextChar(zString);
      zPattern++;
    }else if( c==matchSet ){
      int prior_c = 0;
      assert( esc==0 );    /* This only occurs for GLOB, not LIKE */
      seen = 0;
      invert = 0;
      c = sqliteCharVal(zString);
      if( c==0 ) return 0;
      c2 = *++zPattern;
      if( c2=='^' ){ invert = 1; c2 = *++zPattern; }
      if( c2==']' ){
        if( c==']' ) seen = 1;
        c2 = *++zPattern;
      }
      while( (c2 = sqliteCharVal(zPattern))!=0 && c2!=']' ){
        if( c2=='-' && zPattern[1]!=']' && zPattern[1]!=0 && prior_c>0 ){
          zPattern++;
          c2 = sqliteCharVal(zPattern);
          if( c>=prior_c && c<=c2 ) seen = 1;
          prior_c = 0;
        }else if( c==c2 ){
          seen = 1;
          prior_c = c2;
        }else{
          prior_c = c2;
        }
        sqliteNextChar(zPattern);
      }
      if( c2==0 || (seen ^ invert)==0 ) return 0;
      sqliteNextChar(zString);
      zPattern++;
    }else if( esc && !prevEscape && sqlite3ReadUtf8(zPattern)==esc){
      prevEscape = 1;
      sqliteNextChar(zPattern);
    }else{
      if( noCase ){
        if( sqlite3UpperToLower[c] != sqlite3UpperToLower[*zString] ) return 0;
      }else{
        if( c != *zString ) return 0;
      }
      zPattern++;
      zString++;
      prevEscape = 0;
    }
  }
  return *zString==0;
}

/*
** Count the number of times that the LIKE operator (or GLOB which is
** just a variation of LIKE) gets called.  This is used for testing
** only.
*/
#ifdef SQLITE_TEST
int sqlite3_like_count = 0;
#endif


/*
** Implementation of the like() SQL function.  This function implements
** the build-in LIKE operator.  The first argument to the function is the
** pattern and the second argument is the string.  So, the SQL statements:
**
**       A LIKE B
**
** is implemented as like(B,A).
**
** This same function (with a different compareInfo structure) computes
** the GLOB operator.
*/
static void likeFunc(
  sqlite3_context *context, 
  int argc, 
  sqlite3_value **argv
){
  const unsigned char *zA = sqlite3_value_text(argv[0]);
  const unsigned char *zB = sqlite3_value_text(argv[1]);
  int escape = 0;
  if( argc==3 ){
    /* The escape character string must consist of a single UTF-8 character.
    ** Otherwise, return an error.
    */
    const unsigned char *zEsc = sqlite3_value_text(argv[2]);
    if( sqlite3utf8CharLen(zEsc, -1)!=1 ){
      sqlite3_result_error(context, 
          "ESCAPE expression must be a single character", -1);
      return;
    }
    escape = sqlite3ReadUtf8(zEsc);
  }
  if( zA && zB ){
    struct compareInfo *pInfo = sqlite3_user_data(context);
#ifdef SQLITE_TEST
    sqlite3_like_count++;
#endif
    sqlite3_result_int(context, patternCompare(zA, zB, pInfo, escape));
  }
}

/*
** Implementation of the NULLIF(x,y) function.  The result is the first
** argument if the arguments are different.  The result is NULL if the
** arguments are equal to each other.
*/
static void nullifFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  CollSeq *pColl = sqlite3GetFuncCollSeq(context);
  if( sqlite3MemCompare(argv[0], argv[1], pColl)!=0 ){
    sqlite3_result_value(context, argv[0]);
  }
}

/*
** Implementation of the VERSION(*) function.  The result is the version
** of the SQLite library that is running.
*/
static void versionFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  sqlite3_result_text(context, sqlite3_version, -1, SQLITE_STATIC);
}


/*
** EXPERIMENTAL - This is not an official function.  The interface may
** change.  This function may disappear.  Do not write code that depends
** on this function.
**
** Implementation of the QUOTE() function.  This function takes a single
** argument.  If the argument is numeric, the return value is the same as
** the argument.  If the argument is NULL, the return value is the string
** "NULL".  Otherwise, the argument is enclosed in single quotes with
** single-quote escapes.
*/
static void quoteFunc(sqlite3_context *context, int argc, sqlite3_value **argv){
  if( argc<1 ) return;
  switch( sqlite3_value_type(argv[0]) ){
    case SQLITE_NULL: {
      sqlite3_result_text(context, "NULL", 4, SQLITE_STATIC);
      break;
    }
    case SQLITE_INTEGER:
    case SQLITE_FLOAT: {
      sqlite3_result_value(context, argv[0]);
      break;
    }
    case SQLITE_BLOB: {
      static const char hexdigits[] = { 
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' 
      };
      char *zText = 0;
      int nBlob = sqlite3_value_bytes(argv[0]);
      char const *zBlob = sqlite3_value_blob(argv[0]);

      zText = (char *)sqliteMalloc((2*nBlob)+4); 
      if( !zText ){
        sqlite3_result_error(context, "out of memory", -1);
      }else{
        int i;
        for(i=0; i<nBlob; i++){
          zText[(i*2)+2] = hexdigits[(zBlob[i]>>4)&0x0F];
          zText[(i*2)+3] = hexdigits[(zBlob[i])&0x0F];
        }
        zText[(nBlob*2)+2] = '\'';
        zText[(nBlob*2)+3] = '\0';
        zText[0] = 'X';
        zText[1] = '\'';
        sqlite3_result_text(context, zText, -1, SQLITE_TRANSIENT);
        sqliteFree(zText);
      }
      break;
    }
    case SQLITE_TEXT: {
      int i,j,n;
      const char *zArg = sqlite3_value_text(argv[0]);
      char *z;

      for(i=n=0; zArg[i]; i++){ if( zArg[i]=='\'' ) n++; }
      z = sqliteMalloc( i+n+3 );
      if( z==0 ) return;
      z[0] = '\'';
      for(i=0, j=1; zArg[i]; i++){
        z[j++] = zArg[i];
        if( zArg[i]=='\'' ){
          z[j++] = '\'';
        }
      }
      z[j++] = '\'';
      z[j] = 0;
      sqlite3_result_text(context, z, j, SQLITE_TRANSIENT);
      sqliteFree(z);
    }
  }
}

#ifdef SQLITE_SOUNDEX
/*
** Compute the soundex encoding of a word.
*/
static void soundexFunc(sqlite3_context *context, int argc, sqlite3_value **argv){
  char zResult[8];
  const u8 *zIn;
  int i, j;
  static const unsigned char iCode[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 0, 1, 2, 0, 0, 2, 2, 4, 5, 5, 0,
    1, 2, 6, 2, 3, 0, 1, 0, 2, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 0, 1, 2, 0, 0, 2, 2, 4, 5, 5, 0,
    1, 2, 6, 2, 3, 0, 1, 0, 2, 0, 2, 0, 0, 0, 0, 0,
  };
  assert( argc==1 );
  zIn = (u8*)sqlite3_value_text(argv[0]);
  for(i=0; zIn[i] && !isalpha(zIn[i]); i++){}
  if( zIn[i] ){
    zResult[0] = toupper(zIn[i]);
    for(j=1; j<4 && zIn[i]; i++){
      int code = iCode[zIn[i]&0x7f];
      if( code>0 ){
        zResult[j++] = code + '0';
      }
    }
    while( j<4 ){
      zResult[j++] = '0';
    }
    zResult[j] = 0;
    sqlite3_result_text(context, zResult, 4, SQLITE_TRANSIENT);
  }else{
    sqlite3_result_text(context, "?000", 4, SQLITE_STATIC);
  }
}
#endif

#ifdef SQLITE_TEST
/*
** This function generates a string of random characters.  Used for
** generating test data.
*/
static void randStr(sqlite3_context *context, int argc, sqlite3_value **argv){
  static const unsigned char zSrc[] = 
     "abcdefghijklmnopqrstuvwxyz"
     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
     "0123456789"
     ".-!,:*^+=_|?/<> ";
  int iMin, iMax, n, r, i;
  unsigned char zBuf[1000];
  if( argc>=1 ){
    iMin = sqlite3_value_int(argv[0]);
    if( iMin<0 ) iMin = 0;
    if( iMin>=sizeof(zBuf) ) iMin = sizeof(zBuf)-1;
  }else{
    iMin = 1;
  }
  if( argc>=2 ){
    iMax = sqlite3_value_int(argv[1]);
    if( iMax<iMin ) iMax = iMin;
    if( iMax>=sizeof(zBuf) ) iMax = sizeof(zBuf)-1;
  }else{
    iMax = 50;
  }
  n = iMin;
  if( iMax>iMin ){
    sqlite3Randomness(sizeof(r), &r);
    r &= 0x7fffffff;
    n += r%(iMax + 1 - iMin);
  }
  assert( n<sizeof(zBuf) );
  sqlite3Randomness(n, zBuf);
  for(i=0; i<n; i++){
    zBuf[i] = zSrc[zBuf[i]%(sizeof(zSrc)-1)];
  }
  zBuf[n] = 0;
  sqlite3_result_text(context, zBuf, n, SQLITE_TRANSIENT);
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
/*
** The following two SQL functions are used to test returning a text
** result with a destructor. Function 'test_destructor' takes one argument
** and returns the same argument interpreted as TEXT. A destructor is
** passed with the sqlite3_result_text() call.
**
** SQL function 'test_destructor_count' returns the number of outstanding 
** allocations made by 'test_destructor';
**
** WARNING: Not threadsafe.
*/
static int test_destructor_count_var = 0;
static void destructor(void *p){
  char *zVal = (char *)p;
  assert(zVal);
  zVal--;
  sqliteFree(zVal);
  test_destructor_count_var--;
}
static void test_destructor(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **argv
){
  char *zVal;
  int len;
  sqlite3 *db = sqlite3_user_data(pCtx);
 
  test_destructor_count_var++;
  assert( nArg==1 );
  if( sqlite3_value_type(argv[0])==SQLITE_NULL ) return;
  len = sqlite3ValueBytes(argv[0], db->enc); 
  zVal = sqliteMalloc(len+3);
  zVal[len] = 0;
  zVal[len-1] = 0;
  assert( zVal );
  zVal++;
  memcpy(zVal, sqlite3ValueText(argv[0], db->enc), len);
  if( db->enc==SQLITE_UTF8 ){
    sqlite3_result_text(pCtx, zVal, -1, destructor);
#ifndef SQLITE_OMIT_UTF16
  }else if( db->enc==SQLITE_UTF16LE ){
    sqlite3_result_text16le(pCtx, zVal, -1, destructor);
  }else{
    sqlite3_result_text16be(pCtx, zVal, -1, destructor);
#endif /* SQLITE_OMIT_UTF16 */
  }
}
static void test_destructor_count(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **argv
){
  sqlite3_result_int(pCtx, test_destructor_count_var);
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
/*
** Routines for testing the sqlite3_get_auxdata() and sqlite3_set_auxdata()
** interface.
**
** The test_auxdata() SQL function attempts to register each of its arguments
** as auxiliary data.  If there are no prior registrations of aux data for
** that argument (meaning the argument is not a constant or this is its first
** call) then the result for that argument is 0.  If there is a prior
** registration, the result for that argument is 1.  The overall result
** is the individual argument results separated by spaces.
*/
static void free_test_auxdata(void *p) {sqliteFree(p);}
static void test_auxdata(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **argv
){
  int i;
  char *zRet = sqliteMalloc(nArg*2);
  if( !zRet ) return;
  for(i=0; i<nArg; i++){
    char const *z = sqlite3_value_text(argv[i]);
    if( z ){
      char *zAux = sqlite3_get_auxdata(pCtx, i);
      if( zAux ){
        zRet[i*2] = '1';
        if( strcmp(zAux, z) ){
          sqlite3_result_error(pCtx, "Auxilary data corruption", -1);
          return;
        }
      }else{
        zRet[i*2] = '0';
        zAux = sqliteStrDup(z);
        sqlite3_set_auxdata(pCtx, i, zAux, free_test_auxdata);
      }
      zRet[i*2+1] = ' ';
    }
  }
  sqlite3_result_text(pCtx, zRet, 2*nArg-1, free_test_auxdata);
}
#endif /* SQLITE_TEST */

#ifdef SQLITE_TEST
/*
** A function to test error reporting from user functions. This function
** returns a copy of it's first argument as an error.
*/
static void test_error(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **argv
){
  sqlite3_result_error(pCtx, sqlite3_value_text(argv[0]), 0);
}
#endif /* SQLITE_TEST */

/*
** An instance of the following structure holds the context of a
** sum() or avg() aggregate computation.
*/
typedef struct SumCtx SumCtx;
struct SumCtx {
  double sum;     /* Sum of terms */
  int cnt;        /* Number of elements summed */
};

/*
** Routines used to compute the sum or average.
*/
static void sumStep(sqlite3_context *context, int argc, sqlite3_value **argv){
  SumCtx *p;
  if( argc<1 ) return;
  p = sqlite3_aggregate_context(context, sizeof(*p));
  if( p && SQLITE_NULL!=sqlite3_value_type(argv[0]) ){
    p->sum += sqlite3_value_double(argv[0]);
    p->cnt++;
  }
}
static void sumFinalize(sqlite3_context *context){
  SumCtx *p;
  p = sqlite3_aggregate_context(context, sizeof(*p));
  sqlite3_result_double(context, p ? p->sum : 0.0);
}
static void avgFinalize(sqlite3_context *context){
  SumCtx *p;
  p = sqlite3_aggregate_context(context, sizeof(*p));
  if( p && p->cnt>0 ){
    sqlite3_result_double(context, p->sum/(double)p->cnt);
  }
}

/*
** An instance of the following structure holds the context of a
** variance or standard deviation computation.
*/
typedef struct StdDevCtx StdDevCtx;
struct StdDevCtx {
  double sum;     /* Sum of terms */
  double sum2;    /* Sum of the squares of terms */
  int cnt;        /* Number of terms counted */
};

/*
** The following structure keeps track of state information for the
** count() aggregate function.
*/
typedef struct CountCtx CountCtx;
struct CountCtx {
  int n;
};

/*
** Routines to implement the count() aggregate function.
*/
static void countStep(sqlite3_context *context, int argc, sqlite3_value **argv){
  CountCtx *p;
  p = sqlite3_aggregate_context(context, sizeof(*p));
  if( (argc==0 || SQLITE_NULL!=sqlite3_value_type(argv[0])) && p ){
    p->n++;
  }
}   
static void countFinalize(sqlite3_context *context){
  CountCtx *p;
  p = sqlite3_aggregate_context(context, sizeof(*p));
  sqlite3_result_int(context, p ? p->n : 0);
}

/*
** Routines to implement min() and max() aggregate functions.
*/
static void minmaxStep(sqlite3_context *context, int argc, sqlite3_value **argv){
  Mem *pArg  = (Mem *)argv[0];
  Mem *pBest;

  if( sqlite3_value_type(argv[0])==SQLITE_NULL ) return;
  pBest = (Mem *)sqlite3_aggregate_context(context, sizeof(*pBest));
  if( !pBest ) return;

  if( pBest->flags ){
    int max;
    int cmp;
    CollSeq *pColl = sqlite3GetFuncCollSeq(context);
    /* This step function is used for both the min() and max() aggregates,
    ** the only difference between the two being that the sense of the
    ** comparison is inverted. For the max() aggregate, the
    ** sqlite3_user_data() function returns (void *)-1. For min() it
    ** returns (void *)db, where db is the sqlite3* database pointer.
    ** Therefore the next statement sets variable 'max' to 1 for the max()
    ** aggregate, or 0 for min().
    */
    max = ((sqlite3_user_data(context)==(void *)-1)?1:0);
    cmp = sqlite3MemCompare(pBest, pArg, pColl);
    if( (max && cmp<0) || (!max && cmp>0) ){
      sqlite3VdbeMemCopy(pBest, pArg);
    }
  }else{
    sqlite3VdbeMemCopy(pBest, pArg);
  }
}
static void minMaxFinalize(sqlite3_context *context){
  sqlite3_value *pRes;
  pRes = (sqlite3_value *)sqlite3_aggregate_context(context, sizeof(Mem));
  if( pRes->flags ){
    sqlite3_result_value(context, pRes);
  }
  sqlite3VdbeMemRelease(pRes);
}


/*
** This function registered all of the above C functions as SQL
** functions.  This should be the only routine in this file with
** external linkage.
*/
void sqlite3RegisterBuiltinFunctions(sqlite3 *db){
  static const struct {
     char *zName;
     signed char nArg;
     u8 argType;           /* 0: none.  1: db  2: (-1) */
     u8 eTextRep;          /* 1: UTF-16.  0: UTF-8 */
     u8 needCollSeq;
     void (*xFunc)(sqlite3_context*,int,sqlite3_value **);
  } aFuncs[] = {
    { "min",               -1, 0, SQLITE_UTF8,    1, minmaxFunc },
    { "min",                0, 0, SQLITE_UTF8,    1, 0          },
    { "max",               -1, 2, SQLITE_UTF8,    1, minmaxFunc },
    { "max",                0, 2, SQLITE_UTF8,    1, 0          },
    { "typeof",             1, 0, SQLITE_UTF8,    0, typeofFunc },
    { "length",             1, 0, SQLITE_UTF8,    0, lengthFunc },
    { "substr",             3, 0, SQLITE_UTF8,    0, substrFunc },
#ifndef SQLITE_OMIT_UTF16
    { "substr",             3, 0, SQLITE_UTF16LE, 0, sqlite3utf16Substr },
#endif
    { "abs",                1, 0, SQLITE_UTF8,    0, absFunc    },
    { "round",              1, 0, SQLITE_UTF8,    0, roundFunc  },
    { "round",              2, 0, SQLITE_UTF8,    0, roundFunc  },
    { "upper",              1, 0, SQLITE_UTF8,    0, upperFunc  },
    { "lower",              1, 0, SQLITE_UTF8,    0, lowerFunc  },
    { "coalesce",          -1, 0, SQLITE_UTF8,    0, ifnullFunc },
    { "coalesce",           0, 0, SQLITE_UTF8,    0, 0          },
    { "coalesce",           1, 0, SQLITE_UTF8,    0, 0          },
    { "ifnull",             2, 0, SQLITE_UTF8,    1, ifnullFunc },
    { "random",            -1, 0, SQLITE_UTF8,    0, randomFunc },
    { "nullif",             2, 0, SQLITE_UTF8,    1, nullifFunc },
    { "sqlite_version",     0, 0, SQLITE_UTF8,    0, versionFunc},
    { "quote",              1, 0, SQLITE_UTF8,    0, quoteFunc  },
    { "last_insert_rowid",  0, 1, SQLITE_UTF8,    0, last_insert_rowid },
    { "changes",            0, 1, SQLITE_UTF8,    0, changes    },
    { "total_changes",      0, 1, SQLITE_UTF8,    0, total_changes },
#ifdef SQLITE_SOUNDEX
    { "soundex",            1, 0, SQLITE_UTF8, 0, soundexFunc},
#endif
#ifdef SQLITE_TEST
    { "randstr",               2, 0, SQLITE_UTF8, 0, randStr    },
    { "test_destructor",       1, 1, SQLITE_UTF8, 0, test_destructor},
    { "test_destructor_count", 0, 0, SQLITE_UTF8, 0, test_destructor_count},
    { "test_auxdata",         -1, 0, SQLITE_UTF8, 0, test_auxdata},
    { "test_error",            1, 0, SQLITE_UTF8, 0, test_error},
#endif
  };
  static const struct {
    char *zName;
    signed char nArg;
    u8 argType;
    u8 needCollSeq;
    void (*xStep)(sqlite3_context*,int,sqlite3_value**);
    void (*xFinalize)(sqlite3_context*);
  } aAggs[] = {
    { "min",    1, 0, 1, minmaxStep,   minMaxFinalize },
    { "max",    1, 2, 1, minmaxStep,   minMaxFinalize },
    { "sum",    1, 0, 0, sumStep,      sumFinalize    },
    { "avg",    1, 0, 0, sumStep,      avgFinalize    },
    { "count",  0, 0, 0, countStep,    countFinalize  },
    { "count",  1, 0, 0, countStep,    countFinalize  },
  };
  int i;

  for(i=0; i<sizeof(aFuncs)/sizeof(aFuncs[0]); i++){
    void *pArg = 0;
    switch( aFuncs[i].argType ){
      case 1: pArg = db; break;
      case 2: pArg = (void *)(-1); break;
    }
    sqlite3_create_function(db, aFuncs[i].zName, aFuncs[i].nArg,
        aFuncs[i].eTextRep, pArg, aFuncs[i].xFunc, 0, 0);
    if( aFuncs[i].needCollSeq ){
      FuncDef *pFunc = sqlite3FindFunction(db, aFuncs[i].zName, 
          strlen(aFuncs[i].zName), aFuncs[i].nArg, aFuncs[i].eTextRep, 0);
      if( pFunc && aFuncs[i].needCollSeq ){
        pFunc->needCollSeq = 1;
      }
    }
  }
#ifndef SQLITE_OMIT_ALTERTABLE
  sqlite3AlterFunctions(db);
#endif
  for(i=0; i<sizeof(aAggs)/sizeof(aAggs[0]); i++){
    void *pArg = 0;
    switch( aAggs[i].argType ){
      case 1: pArg = db; break;
      case 2: pArg = (void *)(-1); break;
    }
    sqlite3_create_function(db, aAggs[i].zName, aAggs[i].nArg, SQLITE_UTF8, 
        pArg, 0, aAggs[i].xStep, aAggs[i].xFinalize);
    if( aAggs[i].needCollSeq ){
      FuncDef *pFunc = sqlite3FindFunction( db, aAggs[i].zName,
          strlen(aAggs[i].zName), aAggs[i].nArg, SQLITE_UTF8, 0);
      if( pFunc && aAggs[i].needCollSeq ){
        pFunc->needCollSeq = 1;
      }
    }
  }
  sqlite3RegisterDateTimeFunctions(db);
#ifdef SQLITE_SSE
  sqlite3SseFunctions(db);
#endif
#ifdef SQLITE_CASE_SENSITIVE_LIKE
  sqlite3RegisterLikeFunctions(db, 1);
#else
  sqlite3RegisterLikeFunctions(db, 0);
#endif
}

/*
** Set the LIKEOPT flag on the 2-argument function with the given name.
*/
static void setLikeOptFlag(sqlite3 *db, const char *zName){
  FuncDef *pDef;
  pDef = sqlite3FindFunction(db, zName, strlen(zName), 2, SQLITE_UTF8, 0);
  if( pDef ){
    pDef->flags = SQLITE_FUNC_LIKEOPT;
  }
}

/*
** Register the built-in LIKE and GLOB functions.  The caseSensitive
** parameter determines whether or not the LIKE operator is case
** sensitive.  GLOB is always case sensitive.
*/
void sqlite3RegisterLikeFunctions(sqlite3 *db, int caseSensitive){
  struct compareInfo *pInfo;
  if( caseSensitive ){
    pInfo = (struct compareInfo*)&likeInfoAlt;
  }else{
    pInfo = (struct compareInfo*)&likeInfoNorm;
  }
  sqlite3_create_function(db, "like", 2, SQLITE_UTF8, pInfo, likeFunc, 0, 0);
  sqlite3_create_function(db, "like", 3, SQLITE_UTF8, pInfo, likeFunc, 0, 0);
  sqlite3_create_function(db, "glob", 2, SQLITE_UTF8, 
      (struct compareInfo*)&globInfo, likeFunc, 0,0);
  setLikeOptFlag(db, "glob");
  if( caseSensitive ){
    setLikeOptFlag(db, "like");
  }
}

/*
** pExpr points to an expression which implements a function.  If
** it is appropriate to apply the LIKE optimization to that function
** then set aWc[0] through aWc[2] to the wildcard characters and
** return TRUE.  If the function is not a LIKE-style function then
** return FALSE.
*/
int sqlite3IsLikeFunction(sqlite3 *db, Expr *pExpr, char *aWc){
  FuncDef *pDef;
  if( pExpr->op!=TK_FUNCTION ){
    return 0;
  }
  if( pExpr->pList->nExpr!=2 ){
    return 0;
  }
  pDef = sqlite3FindFunction(db, pExpr->token.z, pExpr->token.n, 2,
                             SQLITE_UTF8, 0);
  if( pDef==0 || (pDef->flags & SQLITE_FUNC_LIKEOPT)==0 ){
    return 0;
  }

  /* The memcpy() statement assumes that the wildcard characters are
  ** the first three statements in the compareInfo structure.  The
  ** asserts() that follow verify that assumption
  */
  memcpy(aWc, pDef->pUserData, 3);
  assert( (char*)&likeInfoAlt == (char*)&likeInfoAlt.matchAll );
  assert( &((char*)&likeInfoAlt)[1] == (char*)&likeInfoAlt.matchOne );
  assert( &((char*)&likeInfoAlt)[2] == (char*)&likeInfoAlt.matchSet );

  return 1;
}
