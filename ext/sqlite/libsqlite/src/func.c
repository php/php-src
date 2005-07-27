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
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "sqliteInt.h"

/*
** Implementation of the non-aggregate min() and max() functions
*/
static void minFunc(sqlite_func *context, int argc, const char **argv){
  const char *zBest; 
  int i;

  if( argc==0 ) return;
  zBest = argv[0];
  if( zBest==0 ) return;
  for(i=1; i<argc; i++){
    if( argv[i]==0 ) return;
    if( sqliteCompare(argv[i], zBest)<0 ){
      zBest = argv[i];
    }
  }
  sqlite_set_result_string(context, zBest, -1);
}
static void maxFunc(sqlite_func *context, int argc, const char **argv){
  const char *zBest; 
  int i;

  if( argc==0 ) return;
  zBest = argv[0];
  if( zBest==0 ) return;
  for(i=1; i<argc; i++){
    if( argv[i]==0 ) return;
    if( sqliteCompare(argv[i], zBest)>0 ){
      zBest = argv[i];
    }
  }
  sqlite_set_result_string(context, zBest, -1);
}

/*
** Implementation of the length() function
*/
static void lengthFunc(sqlite_func *context, int argc, const char **argv){
  const char *z;
  int len;

  assert( argc==1 );
  z = argv[0];
  if( z==0 ) return;
#ifdef SQLITE_UTF8
  for(len=0; *z; z++){ if( (0xc0&*z)!=0x80 ) len++; }
#else
  len = strlen(z);
#endif
  sqlite_set_result_int(context, len);
}

/*
** Implementation of the abs() function
*/
static void absFunc(sqlite_func *context, int argc, const char **argv){
  const char *z;
  assert( argc==1 );
  z = argv[0];
  if( z==0 ) return;
  if( z[0]=='-' && isdigit(z[1]) ) z++;
  sqlite_set_result_string(context, z, -1);
}

/*
** Implementation of the substr() function
*/
static void substrFunc(sqlite_func *context, int argc, const char **argv){
  const char *z;
#ifdef SQLITE_UTF8
  const char *z2;
  int i;
#endif
  int p1, p2, len;
  assert( argc==3 );
  z = argv[0];
  if( z==0 ) return;
  p1 = atoi(argv[1]?argv[1]:0);
  p2 = atoi(argv[2]?argv[2]:0);
#ifdef SQLITE_UTF8
  for(len=0, z2=z; *z2; z2++){ if( (0xc0&*z2)!=0x80 ) len++; }
#else
  len = strlen(z);
#endif
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
#ifdef SQLITE_UTF8
  for(i=0; i<p1; i++){
    assert( z[i] );
    if( (z[i]&0xc0)==0x80 ) p1++;
  }
  while( z[i] && (z[i]&0xc0)==0x80 ){ i++; p1++; }
  for(; i<p1+p2; i++){
    assert( z[i] );
    if( (z[i]&0xc0)==0x80 ) p2++;
  }
  while( z[i] && (z[i]&0xc0)==0x80 ){ i++; p2++; }
#endif
  if( p2<0 ) p2 = 0;
  sqlite_set_result_string(context, &z[p1], p2);
}

/*
** Implementation of the round() function
*/
static void roundFunc(sqlite_func *context, int argc, const char **argv){
  int n;
  double r;
  char zBuf[100];
  assert( argc==1 || argc==2 );
  if( argv[0]==0 || (argc==2 && argv[1]==0) ) return;
  n = argc==2 ? atoi(argv[1]) : 0;
  if( n>30 ) n = 30;
  if( n<0 ) n = 0;
  r = atof(argv[0]);
  sprintf(zBuf,"%.*f",n,r);
  sqlite_set_result_string(context, zBuf, -1);
}

/*
** Implementation of the upper() and lower() SQL functions.
*/
static void upperFunc(sqlite_func *context, int argc, const char **argv){
  char *z;
  int i;
  if( argc<1 || argv[0]==0 ) return;
  z = sqlite_set_result_string(context, argv[0], -1);
  if( z==0 ) return;
  for(i=0; z[i]; i++){
    if( islower(z[i]) ) z[i] = toupper(z[i]);
  }
}
static void lowerFunc(sqlite_func *context, int argc, const char **argv){
  char *z;
  int i;
  if( argc<1 || argv[0]==0 ) return;
  z = sqlite_set_result_string(context, argv[0], -1);
  if( z==0 ) return;
  for(i=0; z[i]; i++){
    if( isupper(z[i]) ) z[i] = tolower(z[i]);
  }
}

/*
** Implementation of the IFNULL(), NVL(), and COALESCE() functions.  
** All three do the same thing.  They return the first argument
** non-NULL argument.
*/
static void ifnullFunc(sqlite_func *context, int argc, const char **argv){
  int i;
  for(i=0; i<argc; i++){
    if( argv[i] ){
      sqlite_set_result_string(context, argv[i], -1);
      break;
    }
  }
}

/*
** Implementation of random().  Return a random integer.  
*/
static void randomFunc(sqlite_func *context, int argc, const char **argv){
  sqlite_set_result_int(context, sqliteRandomInteger());
}

/*
** Implementation of the last_insert_rowid() SQL function.  The return
** value is the same as the sqlite_last_insert_rowid() API function.
*/
static void last_insert_rowid(sqlite_func *context, int arg, const char **argv){
  sqlite *db = sqlite_user_data(context);
  sqlite_set_result_int(context, sqlite_last_insert_rowid(db));
}

/*
** Implementation of the like() SQL function.  This function implements
** the build-in LIKE operator.  The first argument to the function is the
** string and the second argument is the pattern.  So, the SQL statements:
**
**       A LIKE B
**
** is implemented as like(A,B).
*/
static void likeFunc(sqlite_func *context, int arg, const char **argv){
  if( argv[0]==0 || argv[1]==0 ) return;
  sqlite_set_result_int(context, sqliteLikeCompare(argv[0], argv[1]));
}

/*
** Implementation of the glob() SQL function.  This function implements
** the build-in GLOB operator.  The first argument to the function is the
** string and the second argument is the pattern.  So, the SQL statements:
**
**       A GLOB B
**
** is implemented as glob(A,B).
*/
static void globFunc(sqlite_func *context, int arg, const char **argv){
  if( argv[0]==0 || argv[1]==0 ) return;
  sqlite_set_result_int(context, sqliteGlobCompare(argv[0], argv[1]));
}

/*
** Implementation of the NULLIF(x,y) function.  The result is the first
** argument if the arguments are different.  The result is NULL if the
** arguments are equal to each other.
*/
static void nullifFunc(sqlite_func *context, int argc, const char **argv){
  if( argv[0]!=0 && sqliteCompare(argv[0],argv[1])!=0 ){
    sqlite_set_result_string(context, argv[0], -1);
  }
}

/*
** Implementation of the VERSION(*) function.  The result is the version
** of the SQLite library that is running.
*/
static void versionFunc(sqlite_func *context, int argc, const char **argv){
  sqlite_set_result_string(context, sqlite_version, -1);
}

#ifdef SQLITE_TEST
/*
** This function generates a string of random characters.  Used for
** generating test data.
*/
static void randStr(sqlite_func *context, int argc, const char **argv){
  static const char zSrc[] = 
     "abcdefghijklmnopqrstuvwxyz"
     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
     "0123456789"
     ".-!,:*^+=_|?/<> ";
  int iMin, iMax, n, r, i;
  char zBuf[1000];
  if( argc>=1 ){
    iMin = atoi(argv[0]);
    if( iMin<0 ) iMin = 0;
    if( iMin>=sizeof(zBuf) ) iMin = sizeof(zBuf)-1;
  }else{
    iMin = 1;
  }
  if( argc>=2 ){
    iMax = atoi(argv[1]);
    if( iMax<iMin ) iMax = iMin;
    if( iMax>=sizeof(zBuf) ) iMax = sizeof(zBuf);
  }else{
    iMax = 50;
  }
  n = iMin;
  if( iMax>iMin ){
    r = sqliteRandomInteger();
    if( r<0 ) r = -r;
    n += r%(iMax + 1 - iMin);
  }
  r = 0;
  for(i=0; i<n; i++){
    r = (r + sqliteRandomByte())% (sizeof(zSrc)-1);
    zBuf[i] = zSrc[r];
  }
  zBuf[n] = 0;
  sqlite_set_result_string(context, zBuf, n);
}
#endif

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
static void sumStep(sqlite_func *context, int argc, const char **argv){
  SumCtx *p;
  if( argc<1 ) return;
  p = sqlite_aggregate_context(context, sizeof(*p));
  if( p && argv[0] ){
    p->sum += atof(argv[0]);
    p->cnt++;
  }
}
static void sumFinalize(sqlite_func *context){
  SumCtx *p;
  p = sqlite_aggregate_context(context, sizeof(*p));
  sqlite_set_result_double(context, p ? p->sum : 0.0);
}
static void avgFinalize(sqlite_func *context){
  SumCtx *p;
  p = sqlite_aggregate_context(context, sizeof(*p));
  if( p && p->cnt>0 ){
    sqlite_set_result_double(context, p->sum/(double)p->cnt);
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

#if 0   /* Omit because math library is required */
/*
** Routines used to compute the standard deviation as an aggregate.
*/
static void stdDevStep(sqlite_func *context, int argc, const char **argv){
  StdDevCtx *p;
  double x;
  if( argc<1 ) return;
  p = sqlite_aggregate_context(context, sizeof(*p));
  if( p && argv[0] ){
    x = atof(argv[0]);
    p->sum += x;
    p->sum2 += x*x;
    p->cnt++;
  }
}
static void stdDevFinalize(sqlite_func *context){
  double rN = sqlite_aggregate_count(context);
  StdDevCtx *p = sqlite_aggregate_context(context, sizeof(*p));
  if( p && p->cnt>1 ){
    double rCnt = cnt;
    sqlite_set_result_double(context, 
       sqrt((p->sum2 - p->sum*p->sum/rCnt)/(rCnt-1.0)));
  }
}
#endif

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
static void countStep(sqlite_func *context, int argc, const char **argv){
  CountCtx *p;
  p = sqlite_aggregate_context(context, sizeof(*p));
  if( (argc==0 || argv[0]) && p ){
    p->n++;
  }
}   
static void countFinalize(sqlite_func *context){
  CountCtx *p;
  p = sqlite_aggregate_context(context, sizeof(*p));
  sqlite_set_result_int(context, p ? p->n : 0);
}

/*
** This function tracks state information for the min() and max()
** aggregate functions.
*/
typedef struct MinMaxCtx MinMaxCtx;
struct MinMaxCtx {
  char *z;         /* The best so far */
  char zBuf[28];   /* Space that can be used for storage */
};

/*
** Routines to implement min() and max() aggregate functions.
*/
static void minStep(sqlite_func *context, int argc, const char **argv){
  MinMaxCtx *p;
  p = sqlite_aggregate_context(context, sizeof(*p));
  if( p==0 || argc<1 || argv[0]==0 ) return;
  if( p->z==0 || sqliteCompare(argv[0],p->z)<0 ){
    int len;
    if( p->z && p->z!=p->zBuf ){
      sqliteFree(p->z);
    }
    len = strlen(argv[0]);
    if( len < sizeof(p->zBuf) ){
      p->z = p->zBuf;
    }else{
      p->z = sqliteMalloc( len+1 );
      if( p->z==0 ) return;
    }
    strcpy(p->z, argv[0]);
  }
}
static void maxStep(sqlite_func *context, int argc, const char **argv){
  MinMaxCtx *p;
  p = sqlite_aggregate_context(context, sizeof(*p));
  if( p==0 || argc<1 || argv[0]==0 ) return;
  if( p->z==0 || sqliteCompare(argv[0],p->z)>0 ){
    int len;
    if( p->z && p->z!=p->zBuf ){
      sqliteFree(p->z);
    }
    len = strlen(argv[0]);
    if( len < sizeof(p->zBuf) ){
      p->z = p->zBuf;
    }else{
      p->z = sqliteMalloc( len+1 );
      if( p->z==0 ) return;
    }
    strcpy(p->z, argv[0]);
  }
}
static void minMaxFinalize(sqlite_func *context){
  MinMaxCtx *p;
  p = sqlite_aggregate_context(context, sizeof(*p));
  if( p && p->z ){
    sqlite_set_result_string(context, p->z, strlen(p->z));
  }
  if( p && p->z && p->z!=p->zBuf ){
    sqliteFree(p->z);
  }
}

/*
** This function registered all of the above C functions as SQL
** functions.  This should be the only routine in this file with
** external linkage.
*/
void sqliteRegisterBuiltinFunctions(sqlite *db){
  static struct {
     char *zName;
     int nArg;
     int dataType;
     void (*xFunc)(sqlite_func*,int,const char**);
  } aFuncs[] = {
    { "min",       -1, SQLITE_ARGS,    minFunc    },
    { "min",        0, 0,              0          },
    { "max",       -1, SQLITE_ARGS,    maxFunc    },
    { "max",        0, 0,              0          },
    { "length",     1, SQLITE_NUMERIC, lengthFunc },
    { "substr",     3, SQLITE_TEXT,    substrFunc },
    { "abs",        1, SQLITE_NUMERIC, absFunc    },
    { "round",      1, SQLITE_NUMERIC, roundFunc  },
    { "round",      2, SQLITE_NUMERIC, roundFunc  },
    { "upper",      1, SQLITE_TEXT,    upperFunc  },
    { "lower",      1, SQLITE_TEXT,    lowerFunc  },
    { "coalesce",  -1, SQLITE_ARGS,    ifnullFunc },
    { "coalesce",   0, 0,              0          },
    { "coalesce",   1, 0,              0          },
    { "ifnull",     2, SQLITE_ARGS,    ifnullFunc },
    { "random",    -1, SQLITE_NUMERIC, randomFunc },
    { "like",       2, SQLITE_NUMERIC, likeFunc   },
    { "glob",       2, SQLITE_NUMERIC, globFunc   },
    { "nullif",     2, SQLITE_ARGS,    nullifFunc },
    { "sqlite_version",0,SQLITE_TEXT,  versionFunc},
#ifdef SQLITE_TEST
    { "randstr",    2, SQLITE_TEXT,    randStr    },
#endif
  };
  static struct {
    char *zName;
    int nArg;
    int dataType;
    void (*xStep)(sqlite_func*,int,const char**);
    void (*xFinalize)(sqlite_func*);
  } aAggs[] = {
    { "min",    1, 0,              minStep,      minMaxFinalize },
    { "max",    1, 0,              maxStep,      minMaxFinalize },
    { "sum",    1, SQLITE_NUMERIC, sumStep,      sumFinalize    },
    { "avg",    1, SQLITE_NUMERIC, sumStep,      avgFinalize    },
    { "count",  0, SQLITE_NUMERIC, countStep,    countFinalize  },
    { "count",  1, SQLITE_NUMERIC, countStep,    countFinalize  },
#if 0
    { "stddev", 1, SQLITE_NUMERIC, stdDevStep,   stdDevFinalize },
#endif
  };
  int i;

  for(i=0; i<sizeof(aFuncs)/sizeof(aFuncs[0]); i++){
    sqlite_create_function(db, aFuncs[i].zName,
           aFuncs[i].nArg, aFuncs[i].xFunc, 0);
    if( aFuncs[i].xFunc ){
      sqlite_function_type(db, aFuncs[i].zName, aFuncs[i].dataType);
    }
  }
  sqlite_create_function(db, "last_insert_rowid", 0, 
           last_insert_rowid, db);
  sqlite_function_type(db, "last_insert_rowid", SQLITE_NUMERIC);
  for(i=0; i<sizeof(aAggs)/sizeof(aAggs[0]); i++){
    sqlite_create_aggregate(db, aAggs[i].zName,
           aAggs[i].nArg, aAggs[i].xStep, aAggs[i].xFinalize, 0);
    sqlite_function_type(db, aAggs[i].zName, aAggs[i].dataType);
  }
}
