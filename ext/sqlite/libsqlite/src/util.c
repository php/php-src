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
** Utility functions used throughout sqlite.
**
** This file contains functions for allocating memory, comparing
** strings, and stuff like that.
**
** $Id$
*/
#include "sqliteInt.h"
#include <stdarg.h>
#include <ctype.h>

/*
** If malloc() ever fails, this global variable gets set to 1.
** This causes the library to abort and never again function.
*/
int sqlite_malloc_failed = 0;

/*
** If MEMORY_DEBUG is defined, then use versions of malloc() and
** free() that track memory usage and check for buffer overruns.
*/
#ifdef MEMORY_DEBUG

/*
** For keeping track of the number of mallocs and frees.   This
** is used to check for memory leaks.
*/
int sqlite_nMalloc;         /* Number of sqliteMalloc() calls */
int sqlite_nFree;           /* Number of sqliteFree() calls */
int sqlite_iMallocFail;     /* Fail sqliteMalloc() after this many calls */
#if MEMORY_DEBUG>1
static int memcnt = 0;
#endif


/*
** Allocate new memory and set it to zero.  Return NULL if
** no memory is available.
*/
void *sqliteMalloc_(int n, int bZero, char *zFile, int line){
  void *p;
  int *pi;
  int k;
  if( sqlite_iMallocFail>=0 ){
    sqlite_iMallocFail--;
    if( sqlite_iMallocFail==0 ){
      sqlite_malloc_failed++;
#if MEMORY_DEBUG>1
      fprintf(stderr,"**** failed to allocate %d bytes at %s:%d\n",
              n, zFile,line);
#endif
      sqlite_iMallocFail--;
      return 0;
    }
  }
  if( n==0 ) return 0;
  k = (n+sizeof(int)-1)/sizeof(int);
  pi = malloc( (3+k)*sizeof(int));
  if( pi==0 ){
    sqlite_malloc_failed++;
    return 0;
  }
  sqlite_nMalloc++;
  pi[0] = 0xdead1122;
  pi[1] = n;
  pi[k+2] = 0xdead3344;
  p = &pi[2];
  memset(p, bZero==0, n);
#if MEMORY_DEBUG>1
  fprintf(stderr,"%06d malloc %d bytes at 0x%x from %s:%d\n",
      ++memcnt, n, (int)p, zFile,line);
#endif
  return p;
}

/*
** Check to see if the given pointer was obtained from sqliteMalloc()
** and is able to hold at least N bytes.  Raise an exception if this
** is not the case.
**
** This routine is used for testing purposes only.
*/
void sqliteCheckMemory(void *p, int N){
  int *pi = p;
  int n, k;
  pi -= 2;
  assert( pi[0]==0xdead1122 );
  n = pi[1];
  assert( N>=0 && N<n );
  k = (n+sizeof(int)-1)/sizeof(int);
  assert( pi[k+2]==0xdead3344 );
}

/*
** Free memory previously obtained from sqliteMalloc()
*/
void sqliteFree_(void *p, char *zFile, int line){
  if( p ){
    int *pi, k, n;
    pi = p;
    pi -= 2;
    sqlite_nFree++;
    if( pi[0]!=0xdead1122 ){
      fprintf(stderr,"Low-end memory corruption at 0x%x\n", (int)p);
      return;
    }
    n = pi[1];
    k = (n+sizeof(int)-1)/sizeof(int);
    if( pi[k+2]!=0xdead3344 ){
      fprintf(stderr,"High-end memory corruption at 0x%x\n", (int)p);
      return;
    }
    memset(pi, 0xff, (k+3)*sizeof(int));
#if MEMORY_DEBUG>1
    fprintf(stderr,"%06d free %d bytes at 0x%x from %s:%d\n",
         ++memcnt, n, (int)p, zFile,line);
#endif
    free(pi);
  }
}

/*
** Resize a prior allocation.  If p==0, then this routine
** works just like sqliteMalloc().  If n==0, then this routine
** works just like sqliteFree().
*/
void *sqliteRealloc_(void *oldP, int n, char *zFile, int line){
  int *oldPi, *pi, k, oldN, oldK;
  void *p;
  if( oldP==0 ){
    return sqliteMalloc_(n,1,zFile,line);
  }
  if( n==0 ){
    sqliteFree_(oldP,zFile,line);
    return 0;
  }
  oldPi = oldP;
  oldPi -= 2;
  if( oldPi[0]!=0xdead1122 ){
    fprintf(stderr,"Low-end memory corruption in realloc at 0x%x\n", (int)p);
    return 0;
  }
  oldN = oldPi[1];
  oldK = (oldN+sizeof(int)-1)/sizeof(int);
  if( oldPi[oldK+2]!=0xdead3344 ){
    fprintf(stderr,"High-end memory corruption in realloc at 0x%x\n", (int)p);
    return 0;
  }
  k = (n + sizeof(int) - 1)/sizeof(int);
  pi = malloc( (k+3)*sizeof(int) );
  if( pi==0 ){
    sqlite_malloc_failed++;
    return 0;
  }
  pi[0] = 0xdead1122;
  pi[1] = n;
  pi[k+2] = 0xdead3344;
  p = &pi[2];
  memcpy(p, oldP, n>oldN ? oldN : n);
  if( n>oldN ){
    memset(&((char*)p)[oldN], 0, n-oldN);
  }
  memset(oldPi, 0xab, (oldK+3)*sizeof(int));
  free(oldPi);
#if MEMORY_DEBUG>1
  fprintf(stderr,"%06d realloc %d to %d bytes at 0x%x to 0x%x at %s:%d\n",
    ++memcnt, oldN, n, (int)oldP, (int)p, zFile, line);
#endif
  return p;
}

/*
** Make a duplicate of a string into memory obtained from malloc()
** Free the original string using sqliteFree().
**
** This routine is called on all strings that are passed outside of
** the SQLite library.  That way clients can free the string using free()
** rather than having to call sqliteFree().
*/
void sqliteStrRealloc(char **pz){
  char *zNew;
  if( pz==0 || *pz==0 ) return;
  zNew = malloc( strlen(*pz) + 1 );
  if( zNew==0 ){
    sqlite_malloc_failed++;
    sqliteFree(*pz);
    *pz = 0;
  }
  strcpy(zNew, *pz);
  sqliteFree(*pz);
  *pz = zNew;
}

/*
** Make a copy of a string in memory obtained from sqliteMalloc()
*/
char *sqliteStrDup_(const char *z, char *zFile, int line){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqliteMalloc_(strlen(z)+1, 0, zFile, line);
  if( zNew ) strcpy(zNew, z);
  return zNew;
}
char *sqliteStrNDup_(const char *z, int n, char *zFile, int line){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqliteMalloc_(n+1, 0, zFile, line);
  if( zNew ){
    memcpy(zNew, z, n);
    zNew[n] = 0;
  }
  return zNew;
}
#endif /* MEMORY_DEBUG */

/*
** The following versions of malloc() and free() are for use in a
** normal build.
*/
#if !defined(MEMORY_DEBUG)

/*
** Allocate new memory and set it to zero.  Return NULL if
** no memory is available.  See also sqliteMallocRaw().
*/
void *sqliteMalloc(int n){
  void *p;
  if( n==0 ) return 0;
  p = malloc(n);
  if( p==0 ){
    sqlite_malloc_failed++;
    return 0;
  }
  memset(p, 0, n);
  return p;
}

/*
** Allocate new memory but do not set it to zero.  Return NULL if
** no memory is available.  See also sqliteMalloc().
*/
void *sqliteMallocRaw(int n){
  void *p;
  if( n==0 ) return 0;
  p = malloc(n);
  if( p==0 ){
    sqlite_malloc_failed++;
    return 0;
  }
  return p;
}

/*
** Free memory previously obtained from sqliteMalloc()
*/
void sqliteFree(void *p){
  if( p ){
    free(p);
  }
}

/*
** Resize a prior allocation.  If p==0, then this routine
** works just like sqliteMalloc().  If n==0, then this routine
** works just like sqliteFree().
*/
void *sqliteRealloc(void *p, int n){
  void *p2;
  if( p==0 ){
    return sqliteMalloc(n);
  }
  if( n==0 ){
    sqliteFree(p);
    return 0;
  }
  p2 = realloc(p, n);
  if( p2==0 ){
    sqlite_malloc_failed++;
  }
  return p2;
}

/*
** Make a copy of a string in memory obtained from sqliteMalloc()
*/
char *sqliteStrDup(const char *z){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqliteMallocRaw(strlen(z)+1);
  if( zNew ) strcpy(zNew, z);
  return zNew;
}
char *sqliteStrNDup(const char *z, int n){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqliteMallocRaw(n+1);
  if( zNew ){
    memcpy(zNew, z, n);
    zNew[n] = 0;
  }
  return zNew;
}
#endif /* !defined(MEMORY_DEBUG) */

/*
** Create a string from the 2nd and subsequent arguments (up to the
** first NULL argument), store the string in memory obtained from
** sqliteMalloc() and make the pointer indicated by the 1st argument
** point to that string.
*/
void sqliteSetString(char **pz, const char *zFirst, ...){
  va_list ap;
  int nByte;
  const char *z;
  char *zResult;

  if( pz==0 ) return;
  nByte = strlen(zFirst) + 1;
  va_start(ap, zFirst);
  while( (z = va_arg(ap, const char*))!=0 ){
    nByte += strlen(z);
  }
  va_end(ap);
  sqliteFree(*pz);
  *pz = zResult = sqliteMallocRaw( nByte );
  if( zResult==0 ){
    return;
  }
  strcpy(zResult, zFirst);
  zResult += strlen(zResult);
  va_start(ap, zFirst);
  while( (z = va_arg(ap, const char*))!=0 ){
    strcpy(zResult, z);
    zResult += strlen(zResult);
  }
  va_end(ap);
#ifdef MEMORY_DEBUG
#if MEMORY_DEBUG>1
  fprintf(stderr,"string at 0x%x is %s\n", (int)*pz, *pz);
#endif
#endif
}

/*
** Works like sqliteSetString, but each string is now followed by
** a length integer which specifies how much of the source string 
** to copy (in bytes).  -1 means use the whole string.
*/
void sqliteSetNString(char **pz, ...){
  va_list ap;
  int nByte;
  const char *z;
  char *zResult;
  int n;

  if( pz==0 ) return;
  nByte = 0;
  va_start(ap, pz);
  while( (z = va_arg(ap, const char*))!=0 ){
    n = va_arg(ap, int);
    if( n<=0 ) n = strlen(z);
    nByte += n;
  }
  va_end(ap);
  sqliteFree(*pz);
  *pz = zResult = sqliteMallocRaw( nByte + 1 );
  if( zResult==0 ) return;
  va_start(ap, pz);
  while( (z = va_arg(ap, const char*))!=0 ){
    n = va_arg(ap, int);
    if( n<=0 ) n = strlen(z);
    strncpy(zResult, z, n);
    zResult += n;
  }
  *zResult = 0;
#ifdef MEMORY_DEBUG
#if MEMORY_DEBUG>1
  fprintf(stderr,"string at 0x%x is %s\n", (int)*pz, *pz);
#endif
#endif
  va_end(ap);
}

/*
** Convert an SQL-style quoted string into a normal string by removing
** the quote characters.  The conversion is done in-place.  If the
** input does not begin with a quote character, then this routine
** is a no-op.
**
** 2002-Feb-14: This routine is extended to remove MS-Access style
** brackets from around identifers.  For example:  "[a-b-c]" becomes
** "a-b-c".
*/
void sqliteDequote(char *z){
  int quote;
  int i, j;
  if( z==0 ) return;
  quote = z[0];
  switch( quote ){
    case '\'':  break;
    case '"':   break;
    case '[':   quote = ']';  break;
    default:    return;
  }
  for(i=1, j=0; z[i]; i++){
    if( z[i]==quote ){
      if( z[i+1]==quote ){
        z[j++] = quote;
        i++;
      }else{
        z[j++] = 0;
        break;
      }
    }else{
      z[j++] = z[i];
    }
  }
}

/* An array to map all upper-case characters into their corresponding
** lower-case character. 
*/
static unsigned char UpperToLower[] = {
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
     18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
     36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
     54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99,100,101,102,103,
    104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
    122, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,
    108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,
    126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
    144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,
    162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
    180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,
    198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,
    216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,
    234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,
    252,253,254,255
};

/*
** This function computes a hash on the name of a keyword.
** Case is not significant.
*/
int sqliteHashNoCase(const char *z, int n){
  int h = 0;
  if( n<=0 ) n = strlen(z);
  while( n > 0  ){
    h = (h<<3) ^ h ^ UpperToLower[(unsigned char)*z++];
    n--;
  }
  if( h<0 ) h = -h;
  return h;
}

/*
** Some systems have stricmp().  Others have strcasecmp().  Because
** there is no consistency, we will define our own.
*/
int sqliteStrICmp(const char *zLeft, const char *zRight){
  register unsigned char *a, *b;
  a = (unsigned char *)zLeft;
  b = (unsigned char *)zRight;
  while( *a!=0 && UpperToLower[*a]==UpperToLower[*b]){ a++; b++; }
  return *a - *b;
}
int sqliteStrNICmp(const char *zLeft, const char *zRight, int N){
  register unsigned char *a, *b;
  a = (unsigned char *)zLeft;
  b = (unsigned char *)zRight;
  while( N-- > 0 && *a!=0 && UpperToLower[*a]==UpperToLower[*b]){ a++; b++; }
  return N<0 ? 0 : *a - *b;
}

#if 0  /* NOT USED */
/* 
** The sortStrCmp() function below is used to order elements according
** to the ORDER BY clause of a SELECT.  The sort order is a little different
** from what one might expect.  This note attempts to describe what is
** going on.
**
** We want the main string comparision function used for sorting to
** sort both numbers and alphanumeric words into the correct sequence.
** The same routine should do both without prior knowledge of which
** type of text the input represents.  It should even work for strings
** which are a mixture of text and numbers.  (It does not work for
** numeric substrings in exponential notation, however.)
**
** To accomplish this, we keep track of a state number while scanning
** the two strings.  The states are as follows:
**
**    1      Beginning of word
**    2      Arbitrary text
**    3      Integer
**    4      Negative integer
**    5      Real number
**    6      Negative real
**
** The scan begins in state 1, beginning of word.  Transitions to other
** states are determined by characters seen, as shown in the following
** chart:
**
**      Current State         Character Seen  New State
**      --------------------  --------------  -------------------
**      0 Beginning of word   "-"             3 Negative integer
**                            digit           2 Integer
**                            space           0 Beginning of word
**                            otherwise       1 Arbitrary text
**
**      1 Arbitrary text      space           0 Beginning of word
**                            digit           2 Integer
**                            otherwise       1 Arbitrary text
**
**      2 Integer             space           0 Beginning of word
**                            "."             4 Real number
**                            digit           2 Integer
**                            otherwise       1 Arbitrary text
**
**      3 Negative integer    space           0 Beginning of word
**                            "."             5 Negative Real num
**                            digit           3 Negative integer
**                            otherwise       1 Arbitrary text
**
**      4 Real number         space           0 Beginning of word
**                            digit           4 Real number
**                            otherwise       1 Arbitrary text
**
**      5 Negative real num   space           0 Beginning of word
**                            digit           5 Negative real num
**                            otherwise       1 Arbitrary text
**
** To implement this state machine, we first classify each character
** into on of the following categories:
**
**      0  Text
**      1  Space
**      2  Digit
**      3  "-"
**      4  "."
**
** Given an arbitrary character, the array charClass[] maps that character
** into one of the atove categories.
*/
static const unsigned char charClass[] = {
        /* x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF */
/* 0x */   0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0,
/* 1x */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 2x */   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0,
/* 3x */   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,
/* 4x */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 5x */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 6x */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 7x */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 8x */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 9x */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* Ax */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* Bx */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* Cx */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* Dx */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* Ex */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* Fx */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
#define N_CHAR_CLASS 5

/*
** Given the current state number (0 thru 5), this array figures
** the new state number given the character class.
*/
static const unsigned char stateMachine[] = {
 /* Text,  Space, Digit, "-", "." */
      1,      0,    2,    3,   1,      /* State 0: Beginning of word */
      1,      0,    2,    1,   1,      /* State 1: Arbitrary text */
      1,      0,    2,    1,   4,      /* State 2: Integer */
      1,      0,    3,    1,   5,      /* State 3: Negative integer */
      1,      0,    4,    1,   1,      /* State 4: Real number */
      1,      0,    5,    1,   1,      /* State 5: Negative real num */
};

/* This routine does a comparison of two strings.  Case is used only
** if useCase!=0.  Numeric substrings compare in numerical order for the
** most part but this routine does not understand exponential notation.
*/
static int sortStrCmp(const char *atext, const char *btext, int useCase){
  register unsigned char *a, *b, *map, ca, cb;
  int result;
  register int cclass = 0;

  a = (unsigned char *)atext;
  b = (unsigned char *)btext;
  if( useCase ){
    do{
      if( (ca= *a++)!=(cb= *b++) ) break;
      cclass = stateMachine[cclass*N_CHAR_CLASS + charClass[ca]];
    }while( ca!=0 );
  }else{
    map = UpperToLower;
    do{
      if( (ca=map[*a++])!=(cb=map[*b++]) ) break;
      cclass = stateMachine[cclass*N_CHAR_CLASS + charClass[ca]];
    }while( ca!=0 );
    if( ca>='[' && ca<='`' ) cb = b[-1];
    if( cb>='[' && cb<='`' ) ca = a[-1];
  }
  switch( cclass ){
    case 0:
    case 1: {
      if( isdigit(ca) && isdigit(cb) ){
        cclass = 2;
      }
      break;
    }
    default: {
      break;
    }
  }
  switch( cclass ){
    case 2:
    case 3: {
      if( isdigit(ca) ){
        if( isdigit(cb) ){
          int acnt, bcnt;
          acnt = bcnt = 0;
          while( isdigit(*a++) ) acnt++;
          while( isdigit(*b++) ) bcnt++;
          result = acnt - bcnt;
          if( result==0 ) result = ca-cb;
        }else{
          result = 1;
        }
      }else if( isdigit(cb) ){
        result = -1;
      }else if( ca=='.' ){
        result = 1;
      }else if( cb=='.' ){
        result = -1;
      }else{
        result = ca - cb;
        cclass = 2;
      }
      if( cclass==3 ) result = -result;
      break;
    }
    case 0:
    case 1:
    case 4: {
      result = ca - cb;
      break;
    }
    case 5: {
      result = cb - ca;
    };
  }
  return result;
}
#endif /* NOT USED */

/*
** Return TRUE if z is a pure numeric string.  Return FALSE if the
** string contains any character which is not part of a number.
**
** Am empty string is considered numeric.
*/
static int sqliteIsNumber(const char *z){
  if( *z=='-' || *z=='+' ) z++;
  if( !isdigit(*z) ){
    return *z==0;
  }
  z++;
  while( isdigit(*z) ){ z++; }
  if( *z=='.' ){
    z++;
    if( !isdigit(*z) ) return 0;
    while( isdigit(*z) ){ z++; }
    if( *z=='e' || *z=='E' ){
      z++;
      if( *z=='+' || *z=='-' ) z++;
      if( !isdigit(*z) ) return 0;
      while( isdigit(*z) ){ z++; }
    }
  }
  return *z==0;
}

/* This comparison routine is what we use for comparison operations
** between numeric values in an SQL expression.  "Numeric" is a little
** bit misleading here.  What we mean is that the strings have a
** type of "numeric" from the point of view of SQL.  The strings
** do not necessarily contain numbers.  They could contain text.
**
** If the input strings both look like actual numbers then they
** compare in numerical order.  Numerical strings are always less 
** than non-numeric strings so if one input string looks like a
** number and the other does not, then the one that looks like
** a number is the smaller.  Non-numeric strings compare in 
** lexigraphical order (the same order as strcmp()).
*/
int sqliteCompare(const char *atext, const char *btext){
  int result;
  int isNumA, isNumB;
  if( atext==0 ){
    return -1;
  }else if( btext==0 ){
    return 1;
  }
  isNumA = sqliteIsNumber(atext);
  isNumB = sqliteIsNumber(btext);
  if( isNumA ){
    if( !isNumB ){
      result = -1;
    }else{
      double rA, rB;
      rA = atof(atext);
      rB = atof(btext);
      if( rA<rB ){
        result = -1;
      }else if( rA>rB ){
        result = +1;
      }else{
        result = 0;
      }
    }
  }else if( isNumB ){
    result = +1;
  }else {
    result = strcmp(atext, btext);
  }
  return result; 
}

/*
** This routine is used for sorting.  Each key is a list of one or more
** null-terminated elements.  The list is terminated by two nulls in
** a row.  For example, the following text is a key with three elements
**
**            Aone\000Dtwo\000Athree\000\000
**
** All elements begin with one of the characters "+-AD" and end with "\000"
** with zero or more text elements in between.  Except, NULL elements
** consist of the special two-character sequence "N\000".
**
** Both arguments will have the same number of elements.  This routine
** returns negative, zero, or positive if the first argument is less
** than, equal to, or greater than the first.  (Result is a-b).
**
** Each element begins with one of the characters "+", "-", "A", "D".
** This character determines the sort order and collating sequence:
**
**     +      Sort numerically in ascending order
**     -      Sort numerically in descending order
**     A      Sort as strings in ascending order
**     D      Sort as strings in descending order.
**
** For the "+" and "-" sorting, pure numeric strings (strings for which the
** isNum() function above returns TRUE) always compare less than strings
** that are not pure numerics.  Non-numeric strings compare in memcmp()
** order.  This is the same sort order as the sqliteCompare() function
** above generates.
**
** The last point is a change from version 2.6.3 to version 2.7.0.  In
** version 2.6.3 and earlier, substrings of digits compare in numerical 
** and case was used only to break a tie.
**
** Elements that begin with 'A' or 'D' compare in memcmp() order regardless
** of whether or not they look like a number.
**
** Note that the sort order imposed by the rules above is the same
** from the ordering defined by the "<", "<=", ">", and ">=" operators
** of expressions and for indices.  This was not the case for version
** 2.6.3 and earlier.
*/
int sqliteSortCompare(const char *a, const char *b){
  int len;
  int res = 0;
  int isNumA, isNumB;
  int dir = 0;

  while( res==0 && *a && *b ){
    if( a[0]=='N' || b[0]=='N' ){
      if( a[0]==b[0] ){
        a += 2;
        b += 2;
        continue;
      }
      if( a[0]=='N' ){
        dir = b[0];
        res = -1;
      }else{
        dir = a[0];
        res = +1;
      }
      break;
    }
    assert( a[0]==b[0] );
    if( (dir=a[0])=='A' || a[0]=='D' ){
      res = strcmp(&a[1],&b[1]);
      if( res ) break;
    }else{
      isNumA = sqliteIsNumber(&a[1]);
      isNumB = sqliteIsNumber(&b[1]);
      if( isNumA ){
        double rA, rB;
        if( !isNumB ){
          res = -1;
          break;
        }
        rA = atof(&a[1]);
        rB = atof(&b[1]);
        if( rA<rB ){
          res = -1;
          break;
        }
        if( rA>rB ){
          res = +1;
          break;
        }
      }else if( isNumB ){
        res = +1;
        break;
      }else{
        res = strcmp(&a[1],&b[1]);
        if( res ) break;
      }
    }
    len = strlen(&a[1]) + 2;
    a += len;
    b += len;
  }
  if( dir=='-' || dir=='D' ) res = -res;
  return res;
}

/*
** Some powers of 64.  These constants are needed in the
** sqliteRealToSortable() routine below.
*/
#define _64e3  (64.0 * 64.0 * 64.0)
#define _64e4  (64.0 * 64.0 * 64.0 * 64.0)
#define _64e15 (_64e3 * _64e4 * _64e4 * _64e4)
#define _64e16 (_64e4 * _64e4 * _64e4 * _64e4)
#define _64e63 (_64e15 * _64e16 * _64e16 * _64e16)
#define _64e64 (_64e16 * _64e16 * _64e16 * _64e16)

/*
** The following procedure converts a double-precision floating point
** number into a string.  The resulting string has the property that
** two such strings comparied using strcmp() or memcmp() will give the
** same results as a numeric comparison of the original floating point
** numbers.
**
** This routine is used to generate database keys from floating point
** numbers such that the keys sort in the same order as the original
** floating point numbers even though the keys are compared using
** memcmp().
**
** The calling function should have allocated at least 14 characters
** of space for the buffer z[].
*/
void sqliteRealToSortable(double r, char *z){
  int neg;
  int exp;
  int cnt = 0;

  /* This array maps integers between 0 and 63 into base-64 digits.
  ** The digits must be chosen such at their ASCII codes are increasing.
  ** This means we can not use the traditional base-64 digit set. */
  static const char zDigit[] = 
     "0123456789"
     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
     "abcdefghijklmnopqrstuvwxyz"
     "|~";
  if( r<0.0 ){
    neg = 1;
    r = -r;
    *z++ = '-';
  } else {
    neg = 0;
    *z++ = '0';
  }
  exp = 0;

  if( r==0.0 ){
    exp = -1024;
  }else if( r<(0.5/64.0) ){
    while( r < 0.5/_64e64 && exp > -961  ){ r *= _64e64;  exp -= 64; }
    while( r < 0.5/_64e16 && exp > -1009 ){ r *= _64e16;  exp -= 16; }
    while( r < 0.5/_64e4  && exp > -1021 ){ r *= _64e4;   exp -= 4; }
    while( r < 0.5/64.0   && exp > -1024 ){ r *= 64.0;    exp -= 1; }
  }else if( r>=0.5 ){
    while( r >= 0.5*_64e63 && exp < 960  ){ r *= 1.0/_64e64; exp += 64; }
    while( r >= 0.5*_64e15 && exp < 1008 ){ r *= 1.0/_64e16; exp += 16; }
    while( r >= 0.5*_64e3  && exp < 1020 ){ r *= 1.0/_64e4;  exp += 4; }
    while( r >= 0.5        && exp < 1023 ){ r *= 1.0/64.0;   exp += 1; }
  }
  if( neg ){
    exp = -exp;
    r = -r;
  }
  exp += 1024;
  r += 0.5;
  if( exp<0 ) return;
  if( exp>=2048 || r>=1.0 ){
    strcpy(z, "~~~~~~~~~~~~");
    return;
  }
  *z++ = zDigit[(exp>>6)&0x3f];
  *z++ = zDigit[exp & 0x3f];
  while( r>0.0 && cnt<10 ){
    int digit;
    r *= 64.0;
    digit = (int)r;
    assert( digit>=0 && digit<64 );
    *z++ = zDigit[digit & 0x3f];
    r -= digit;
    cnt++;
  }
  *z = 0;
}

#ifdef SQLITE_UTF8
/*
** X is a pointer to the first byte of a UTF-8 character.  Increment
** X so that it points to the next character.  This only works right
** if X points to a well-formed UTF-8 string.
*/
#define sqliteNextChar(X)  while( (0xc0&*++(X))==0x80 ){}
#define sqliteCharVal(X)   sqlite_utf8_to_int(X)

#else /* !defined(SQLITE_UTF8) */
/*
** For iso8859 encoding, the next character is just the next byte.
*/
#define sqliteNextChar(X)  (++(X));
#define sqliteCharVal(X)   ((int)*(X))

#endif /* defined(SQLITE_UTF8) */


#ifdef SQLITE_UTF8
/*
** Convert the UTF-8 character to which z points into a 31-bit
** UCS character.  This only works right if z points to a well-formed
** UTF-8 string.
*/
static int sqlite_utf8_to_int(const unsigned char *z){
  int c;
  static const int initVal[] = {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
     30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
     45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
     60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,
     75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
     90,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,   0,   1,   2,
      3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,
     18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,   0,
      1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
      0,   1,   2,   3,   4,   5,   6,   7,   0,   1,   2,   3,   0,   1, 254,
    255,
  };
  c = initVal[*(z++)];
  while( (0xc0&*z)==0x80 ){
    c = (c<<6) | (0x3f&*(z++));
  }
  return c;
}
#endif

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
int 
sqliteGlobCompare(const unsigned char *zPattern, const unsigned char *zString){
  register int c;
  int invert;
  int seen;
  int c2;

  while( (c = *zPattern)!=0 ){
    switch( c ){
      case '*':
        while( (c=zPattern[1]) == '*' || c == '?' ){
          if( c=='?' ){
            if( *zString==0 ) return 0;
            sqliteNextChar(zString);
          }
          zPattern++;
        }
        if( c==0 ) return 1;
        if( c=='[' ){
          while( *zString && sqliteGlobCompare(&zPattern[1],zString)==0 ){
            sqliteNextChar(zString);
          }
          return *zString!=0;
        }else{
          while( (c2 = *zString)!=0 ){
            while( c2 != 0 && c2 != c ){ c2 = *++zString; }
            if( c2==0 ) return 0;
            if( sqliteGlobCompare(&zPattern[1],zString) ) return 1;
            sqliteNextChar(zString);
          }
          return 0;
        }
      case '?': {
        if( *zString==0 ) return 0;
        sqliteNextChar(zString);
        zPattern++;
        break;
      }
      case '[': {
        int prior_c = 0;
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
        break;
      }
      default: {
        if( c != *zString ) return 0;
        zPattern++;
        zString++;
        break;
      }
    }
  }
  return *zString==0;
}

/*
** Compare two UTF-8 strings for equality using the "LIKE" operator of
** SQL.  The '%' character matches any sequence of 0 or more
** characters and '_' matches any single character.  Case is
** not significant.
**
** This routine is just an adaptation of the sqliteGlobCompare()
** routine above.
*/
int 
sqliteLikeCompare(const unsigned char *zPattern, const unsigned char *zString){
  register int c;
  int c2;

  while( (c = UpperToLower[*zPattern])!=0 ){
    switch( c ){
      case '%': {
        while( (c=zPattern[1]) == '%' || c == '_' ){
          if( c=='_' ){
            if( *zString==0 ) return 0;
            sqliteNextChar(zString);
          }
          zPattern++;
        }
        if( c==0 ) return 1;
        c = UpperToLower[c];
        while( (c2=UpperToLower[*zString])!=0 ){
          while( c2 != 0 && c2 != c ){ c2 = UpperToLower[*++zString]; }
          if( c2==0 ) return 0;
          if( sqliteLikeCompare(&zPattern[1],zString) ) return 1;
          sqliteNextChar(zString);
        }
        return 0;
      }
      case '_': {
        if( *zString==0 ) return 0;
        sqliteNextChar(zString);
        zPattern++;
        break;
      }
      default: {
        if( c != UpperToLower[*zString] ) return 0;
        zPattern++;
        zString++;
        break;
      }
    }
  }
  return *zString==0;
}

/*
** Change the sqlite.magic from SQLITE_MAGIC_OPEN to SQLITE_MAGIC_BUSY.
** Return an error (non-zero) if the magic was not SQLITE_MAGIC_OPEN
** when this routine is called.
**
** This routine is a attempt to detect if two threads use the
** same sqlite* pointer at the same time.  There is a race 
** condition so it is possible that the error is not detected.
** But usually the problem will be seen.  The result will be an
** error which can be used to debug the application that is
** using SQLite incorrectly.
**
** Ticket #202:  If db->magic is not a valid open value, take care not
** to modify the db structure at all.  It could be that db is a stale
** pointer.  In other words, it could be that there has been a prior
** call to sqlite_close(db) and db has been deallocated.  And we do
** not want to write into deallocated memory.
*/
int sqliteSafetyOn(sqlite *db){
  if( db->magic==SQLITE_MAGIC_OPEN ){
    db->magic = SQLITE_MAGIC_BUSY;
    return 0;
  }else if( db->magic==SQLITE_MAGIC_BUSY || db->magic==SQLITE_MAGIC_ERROR
             || db->want_to_close ){
    db->magic = SQLITE_MAGIC_ERROR;
    db->flags |= SQLITE_Interrupt;
  }
  return 1;
}

/*
** Change the magic from SQLITE_MAGIC_BUSY to SQLITE_MAGIC_OPEN.
** Return an error (non-zero) if the magic was not SQLITE_MAGIC_BUSY
** when this routine is called.
*/
int sqliteSafetyOff(sqlite *db){
  if( db->magic==SQLITE_MAGIC_BUSY ){
    db->magic = SQLITE_MAGIC_OPEN;
    return 0;
  }else if( db->magic==SQLITE_MAGIC_OPEN || db->magic==SQLITE_MAGIC_ERROR
             || db->want_to_close ){
    db->magic = SQLITE_MAGIC_ERROR;
    db->flags |= SQLITE_Interrupt;
  }
  return 1;
}

/*
** Check to make sure we are not currently executing an sqlite_exec().
** If we are currently in an sqlite_exec(), return true and set
** sqlite.magic to SQLITE_MAGIC_ERROR.  This will cause a complete
** shutdown of the database.
**
** This routine is used to try to detect when API routines are called
** at the wrong time or in the wrong sequence.
*/
int sqliteSafetyCheck(sqlite *db){
  if( db->pVdbe!=0 ){
    db->magic = SQLITE_MAGIC_ERROR;
    return 1;
  }
  return 0;
}
