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
** Internal interface definitions for SQLite.
**
** @(#) $Id$
*/
#ifndef _SQLITEINT_H_
#define _SQLITEINT_H_

/*
** These #defines should enable >2GB file support on Posix if the
** underlying operating system supports it.  If the OS lacks
** large file support, or if the OS is windows, these should be no-ops.
**
** Large file support can be disabled using the -DSQLITE_DISABLE_LFS switch
** on the compiler command line.  This is necessary if you are compiling
** on a recent machine (ex: RedHat 7.2) but you want your code to work
** on an older machine (ex: RedHat 6.0).  If you compile on RedHat 7.2
** without this option, LFS is enable.  But LFS does not exist in the kernel
** in RedHat 6.0, so the code won't work.  Hence, for maximum binary
** portability you should omit LFS.
**
** Similar is true for MacOS.  LFS is only supported on MacOS 9 and later.
*/
#ifndef SQLITE_DISABLE_LFS
# define _LARGE_FILE       1
# ifndef _FILE_OFFSET_BITS
#   define _FILE_OFFSET_BITS 64
# endif
# define _LARGEFILE_SOURCE 1
#endif

#include "config.h"
#include "sqlite3.h"
#include "hash.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
** The maximum number of in-memory pages to use for the main database
** table and for temporary tables.
*/
#define MAX_PAGES   2000
#define TEMP_PAGES   500

/*
** If the following macro is set to 1, then NULL values are considered
** distinct for the SELECT DISTINCT statement and for UNION or EXCEPT
** compound queries.  No other SQL database engine (among those tested) 
** works this way except for OCELOT.  But the SQL92 spec implies that
** this is how things should work.
**
** If the following macro is set to 0, then NULLs are indistinct for
** SELECT DISTINCT and for UNION.
*/
#define NULL_ALWAYS_DISTINCT 0

/*
** If the following macro is set to 1, then NULL values are considered
** distinct when determining whether or not two entries are the same
** in a UNIQUE index.  This is the way PostgreSQL, Oracle, DB2, MySQL,
** OCELOT, and Firebird all work.  The SQL92 spec explicitly says this
** is the way things are suppose to work.
**
** If the following macro is set to 0, the NULLs are indistinct for
** a UNIQUE index.  In this mode, you can only have a single NULL entry
** for a column declared UNIQUE.  This is the way Informix and SQL Server
** work.
*/
#define NULL_DISTINCT_FOR_UNIQUE 1

/*
** The maximum number of attached databases.  This must be at least 2
** in order to support the main database file (0) and the file used to
** hold temporary tables (1).  And it must be less than 32 because
** we use a bitmask of databases with a u32 in places (for example
** the Parse.cookieMask field).
*/
#define MAX_ATTACHED 10

/*
** The maximum value of a ?nnn wildcard that the parser will accept.
*/
#define SQLITE_MAX_VARIABLE_NUMBER 999

/*
** When building SQLite for embedded systems where memory is scarce,
** you can define one or more of the following macros to omit extra
** features of the library and thus keep the size of the library to
** a minimum.
*/
/* #define SQLITE_OMIT_AUTHORIZATION  1 */
/* #define SQLITE_OMIT_INMEMORYDB     1 */
/* #define SQLITE_OMIT_VACUUM         1 */
/* #define SQLITE_OMIT_DATETIME_FUNCS 1 */
/* #define SQLITE_OMIT_PROGRESS_CALLBACK 1 */

/*
** Integers of known sizes.  These typedefs might change for architectures
** where the sizes very.  Preprocessor macros are available so that the
** types can be conveniently redefined at compile-type.  Like this:
**
**         cc '-DUINTPTR_TYPE=long long int' ...
*/
#ifndef UINT64_TYPE
# if defined(_MSC_VER) || defined(__BORLANDC__)
#   define UINT64_TYPE unsigned __int64
# else
#   define UINT64_TYPE unsigned long long int
# endif
#endif
#ifndef UINT32_TYPE
# define UINT32_TYPE unsigned int
#endif
#ifndef UINT16_TYPE
# define UINT16_TYPE unsigned short int
#endif
#ifndef INT16_TYPE
# define INT16_TYPE short int
#endif
#ifndef UINT8_TYPE
# define UINT8_TYPE unsigned char
#endif
#ifndef INT8_TYPE
# define INT8_TYPE signed char
#endif
#ifndef LONGDOUBLE_TYPE
# define LONGDOUBLE_TYPE long double
#endif
#ifndef INTPTR_TYPE
# if SQLITE_PTR_SZ==4
#   define INTPTR_TYPE int
# else
#   define INTPTR_TYPE sqlite_int64
# endif
#endif
#ifndef UINTPTR_TYPE
# if SQLITE_PTR_SZ==4
#   define UINTPTR_TYPE unsigned int
# else
#   define UINTPTR_TYPE sqlite_uint64
# endif
#endif
typedef sqlite_int64 i64;          /* 8-byte signed integer */
typedef UINT64_TYPE u64;           /* 8-byte unsigned integer */
typedef UINT32_TYPE u32;           /* 4-byte unsigned integer */
typedef UINT16_TYPE u16;           /* 2-byte unsigned integer */
typedef INT16_TYPE i16;            /* 2-byte signed integer */
typedef UINT8_TYPE u8;             /* 1-byte unsigned integer */
typedef UINT8_TYPE i8;             /* 1-byte signed integer */
typedef INTPTR_TYPE ptr;           /* Big enough to hold a pointer */
typedef UINTPTR_TYPE uptr;         /* Big enough to hold a pointer */

/*
** Macros to determine whether the machine is big or little endian,
** evaluated at runtime.
*/
extern const int sqlite3one;
#define SQLITE_BIGENDIAN    (*(char *)(&sqlite3one)==0)
#define SQLITE_LITTLEENDIAN (*(char *)(&sqlite3one)==1)

/*
** An instance of the following structure is used to store the busy-handler
** callback for a given sqlite handle. 
**
** The sqlite.busyHandler member of the sqlite struct contains the busy
** callback for the database handle. Each pager opened via the sqlite
** handle is passed a pointer to sqlite.busyHandler. The busy-handler
** callback is currently invoked only from within pager.c.
*/
typedef struct BusyHandler BusyHandler;
struct BusyHandler {
  int (*xFunc)(void *,int);  /* The busy callback */
  void *pArg;                /* First arg to busy callback */
};

/*
** Defer sourcing vdbe.h and btree.h until after the "u8" and 
** "BusyHandler typedefs.
*/
#include "vdbe.h"
#include "btree.h"

/*
** This macro casts a pointer to an integer.  Useful for doing
** pointer arithmetic.
*/
#define Addr(X)  ((uptr)X)

/*
** If memory allocation problems are found, recompile with
**
**      -DSQLITE_DEBUG=1
**
** to enable some sanity checking on malloc() and free().  To
** check for memory leaks, recompile with
**
**      -DSQLITE_DEBUG=2
**
** and a line of text will be written to standard error for
** each malloc() and free().  This output can be analyzed
** by an AWK script to determine if there are any leaks.
*/
#ifdef SQLITE_DEBUG
# define sqliteMalloc(X)    sqlite3Malloc_(X,1,__FILE__,__LINE__)
# define sqliteMallocRaw(X) sqlite3Malloc_(X,0,__FILE__,__LINE__)
# define sqliteFree(X)      sqlite3Free_(X,__FILE__,__LINE__)
# define sqliteRealloc(X,Y) sqlite3Realloc_(X,Y,__FILE__,__LINE__)
# define sqliteStrDup(X)    sqlite3StrDup_(X,__FILE__,__LINE__)
# define sqliteStrNDup(X,Y) sqlite3StrNDup_(X,Y,__FILE__,__LINE__)
#else
# define sqliteFree          sqlite3FreeX
# define sqliteMalloc        sqlite3Malloc
# define sqliteMallocRaw     sqlite3MallocRaw
# define sqliteRealloc       sqlite3Realloc
# define sqliteStrDup        sqlite3StrDup
# define sqliteStrNDup       sqlite3StrNDup
#endif

/*
** This variable gets set if malloc() ever fails.  After it gets set,
** the SQLite library shuts down permanently.
*/
extern int sqlite3_malloc_failed;

/*
** The following global variables are used for testing and debugging
** only.  They only work if SQLITE_DEBUG is defined.
*/
#ifdef SQLITE_DEBUG
extern int sqlite3_nMalloc;       /* Number of sqliteMalloc() calls */
extern int sqlite3_nFree;         /* Number of sqliteFree() calls */
extern int sqlite3_iMallocFail;   /* Fail sqliteMalloc() after this many calls */
#endif

/*
** Name of the master database table.  The master database table
** is a special table that holds the names and attributes of all
** user tables and indices.
*/
#define MASTER_NAME       "sqlite_master"
#define TEMP_MASTER_NAME  "sqlite_temp_master"

/*
** The root-page of the master database table.
*/
#define MASTER_ROOT       1

/*
** The name of the schema table.
*/
#define SCHEMA_TABLE(x)  (x==1?TEMP_MASTER_NAME:MASTER_NAME)

/*
** A convenience macro that returns the number of elements in
** an array.
*/
#define ArraySize(X)    (sizeof(X)/sizeof(X[0]))

/*
** Forward references to structures
*/
typedef struct Column Column;
typedef struct Table Table;
typedef struct Index Index;
typedef struct Instruction Instruction;
typedef struct Expr Expr;
typedef struct ExprList ExprList;
typedef struct Parse Parse;
typedef struct Token Token;
typedef struct IdList IdList;
typedef struct SrcList SrcList;
typedef struct WhereInfo WhereInfo;
typedef struct WhereLevel WhereLevel;
typedef struct Select Select;
typedef struct AggExpr AggExpr;
typedef struct FuncDef FuncDef;
typedef struct Trigger Trigger;
typedef struct TriggerStep TriggerStep;
typedef struct TriggerStack TriggerStack;
typedef struct FKey FKey;
typedef struct Db Db;
typedef struct AuthContext AuthContext;
typedef struct KeyClass KeyClass;
typedef struct CollSeq CollSeq;
typedef struct KeyInfo KeyInfo;

/*
** Each database file to be accessed by the system is an instance
** of the following structure.  There are normally two of these structures
** in the sqlite.aDb[] array.  aDb[0] is the main database file and
** aDb[1] is the database file used to hold temporary tables.  Additional
** databases may be attached.
*/
struct Db {
  char *zName;         /* Name of this database */
  Btree *pBt;          /* The B*Tree structure for this database file */
  int schema_cookie;   /* Database schema version number for this file */
  Hash tblHash;        /* All tables indexed by name */
  Hash idxHash;        /* All (named) indices indexed by name */
  Hash trigHash;       /* All triggers indexed by name */
  Hash aFKey;          /* Foreign keys indexed by to-table */
  u16 flags;           /* Flags associated with this database */
  u8 inTrans;          /* 0: not writable.  1: Transaction.  2: Checkpoint */
  u8 safety_level;     /* How aggressive at synching data to disk */
  int cache_size;      /* Number of pages to use in the cache */
  void *pAux;          /* Auxiliary data.  Usually NULL */
  void (*xFreeAux)(void*);  /* Routine to free pAux */
};

/*
** These macros can be used to test, set, or clear bits in the 
** Db.flags field.
*/
#define DbHasProperty(D,I,P)     (((D)->aDb[I].flags&(P))==(P))
#define DbHasAnyProperty(D,I,P)  (((D)->aDb[I].flags&(P))!=0)
#define DbSetProperty(D,I,P)     (D)->aDb[I].flags|=(P)
#define DbClearProperty(D,I,P)   (D)->aDb[I].flags&=~(P)

/*
** Allowed values for the DB.flags field.
**
** The DB_SchemaLoaded flag is set after the database schema has been
** read into internal hash tables.
**
** DB_UnresetViews means that one or more views have column names that
** have been filled out.  If the schema changes, these column names might
** changes and so the view will need to be reset.
*/
#define DB_SchemaLoaded    0x0001  /* The schema has been loaded */
#define DB_UnresetViews    0x0002  /* Some views have defined column names */

#define SQLITE_UTF16NATIVE (SQLITE_BIGENDIAN?SQLITE_UTF16BE:SQLITE_UTF16LE)

/*
** Each database is an instance of the following structure.
**
** The sqlite.lastRowid records the last insert rowid generated by an
** insert statement.  Inserts on views do not affect its value.  Each
** trigger has its own context, so that lastRowid can be updated inside
** triggers as usual.  The previous value will be restored once the trigger
** exits.  Upon entering a before or instead of trigger, lastRowid is no
** longer (since after version 2.8.12) reset to -1.
**
** The sqlite.nChange does not count changes within triggers and keeps no
** context.  It is reset at start of sqlite3_exec.
** The sqlite.lsChange represents the number of changes made by the last
** insert, update, or delete statement.  It remains constant throughout the
** length of a statement and is then updated by OP_SetCounts.  It keeps a
** context stack just like lastRowid so that the count of changes
** within a trigger is not seen outside the trigger.  Changes to views do not
** affect the value of lsChange.
** The sqlite.csChange keeps track of the number of current changes (since
** the last statement) and is used to update sqlite_lsChange.
**
** The member variables sqlite.errCode, sqlite.zErrMsg and sqlite.zErrMsg16
** store the most recent error code and, if applicable, string. The
** internal function sqlite3Error() is used to set these variables
** consistently.
*/
struct sqlite3 {
  int nDb;                      /* Number of backends currently in use */
  Db *aDb;                      /* All backends */
  Db aDbStatic[2];              /* Static space for the 2 default backends */
  int flags;                    /* Miscellanous flags. See below */
  u8 file_format;               /* What file format version is this database? */
  u8 temp_store;                /* 1: file 2: memory 0: default */
  int nTable;                   /* Number of tables in the database */
  BusyHandler busyHandler;      /* Busy callback */
  void *pCommitArg;             /* Argument to xCommitCallback() */   
  int (*xCommitCallback)(void*);/* Invoked at every commit. */
  Hash aFunc;                   /* All functions that can be in SQL exprs */
  Hash aCollSeq;                /* All collating sequences */
  CollSeq *pDfltColl;           /* The default collating sequence (BINARY) */
  i64 lastRowid;                /* ROWID of most recent insert (see above) */
  i64 priorNewRowid;            /* Last randomly generated ROWID */
  int magic;                    /* Magic number for detect library misuse */
  int nChange;                  /* Value returned by sqlite3_changes() */
  int nTotalChange;             /* Value returned by sqlite3_total_changes() */
  struct sqlite3InitInfo {      /* Information used during initialization */
    int iDb;                    /* When back is being initialized */
    int newTnum;                /* Rootpage of table being initialized */
    u8 busy;                    /* TRUE if currently initializing */
  } init;
  struct Vdbe *pVdbe;           /* List of active virtual machines */
  int activeVdbeCnt;            /* Number of vdbes currently executing */
  void (*xTrace)(void*,const char*);     /* Trace function */
  void *pTraceArg;                       /* Argument to the trace function */
#ifndef SQLITE_OMIT_AUTHORIZATION
  int (*xAuth)(void*,int,const char*,const char*,const char*,const char*);
                                /* Access authorization function */
  void *pAuthArg;               /* 1st argument to the access auth function */
#endif
#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
  int (*xProgress)(void *);     /* The progress callback */
  void *pProgressArg;           /* Argument to the progress callback */
  int nProgressOps;             /* Number of opcodes for progress callback */
#endif

  int errCode;                  /* Most recent error code (SQLITE_*) */
  u8 enc;                       /* Text encoding for this database. */
  u8 autoCommit;                /* The auto-commit flag. */
  void(*xCollNeeded)(void*,sqlite3*,int eTextRep,const char*);
  void(*xCollNeeded16)(void*,sqlite3*,int eTextRep,const void*);
  void *pCollNeededArg;
  sqlite3_value *pValue;        /* Value used for transient conversions */
  sqlite3_value *pErr;          /* Most recent error message */

  char *zErrMsg;                /* Most recent error message (UTF-8 encoded) */
  char *zErrMsg16;              /* Most recent error message (UTF-8 encoded) */
};

/*
** Possible values for the sqlite.flags and or Db.flags fields.
**
** On sqlite.flags, the SQLITE_InTrans value means that we have
** executed a BEGIN.  On Db.flags, SQLITE_InTrans means a statement
** transaction is active on that particular database file.
*/
#define SQLITE_VdbeTrace      0x00000001  /* True to trace VDBE execution */
#define SQLITE_Initialized    0x00000002  /* True after initialization */
#define SQLITE_Interrupt      0x00000004  /* Cancel current operation */
#define SQLITE_InTrans        0x00000008  /* True if in a transaction */
#define SQLITE_InternChanges  0x00000010  /* Uncommitted Hash table changes */
#define SQLITE_FullColNames   0x00000020  /* Show full column names on SELECT */
#define SQLITE_ShortColNames  0x00000040  /* Show short columns names */
#define SQLITE_CountRows      0x00000080  /* Count rows changed by INSERT, */
                                          /*   DELETE, or UPDATE and return */
                                          /*   the count using a callback. */
#define SQLITE_NullCallback   0x00000100  /* Invoke the callback once if the */
                                          /*   result set is empty */
#define SQLITE_SqlTrace       0x00000200  /* Debug print SQL as it executes */
#define SQLITE_VdbeListing    0x00000400  /* Debug listings of VDBE programs */

/*
** Possible values for the sqlite.magic field.
** The numbers are obtained at random and have no special meaning, other
** than being distinct from one another.
*/
#define SQLITE_MAGIC_OPEN     0xa029a697  /* Database is open */
#define SQLITE_MAGIC_CLOSED   0x9f3c2d33  /* Database is closed */
#define SQLITE_MAGIC_BUSY     0xf03b7906  /* Database currently in use */
#define SQLITE_MAGIC_ERROR    0xb5357930  /* An SQLITE_MISUSE error occurred */

/*
** Each SQL function is defined by an instance of the following
** structure.  A pointer to this structure is stored in the sqlite.aFunc
** hash table.  When multiple functions have the same name, the hash table
** points to a linked list of these structures.
*/
struct FuncDef {
  char *zName;         /* SQL name of the function */
  int nArg;            /* Number of arguments.  -1 means unlimited */
  u8 iPrefEnc;         /* Preferred text encoding (SQLITE_UTF8, 16LE, 16BE) */
  void *pUserData;     /* User data parameter */
  FuncDef *pNext;      /* Next function with same name */
  void (*xFunc)(sqlite3_context*,int,sqlite3_value**); /* Regular function */
  void (*xStep)(sqlite3_context*,int,sqlite3_value**); /* Aggregate step */
  void (*xFinalize)(sqlite3_context*);                /* Aggregate finializer */
  u8 needCollSeq;      /* True if sqlite3GetFuncCollSeq() might be called */
};

/*
** information about each column of an SQL table is held in an instance
** of this structure.
*/
struct Column {
  char *zName;     /* Name of this column */
  char *zDflt;     /* Default value of this column */
  char *zType;     /* Data type for this column */
  CollSeq *pColl;  /* Collating sequence.  If NULL, use the default */
  u8 notNull;      /* True if there is a NOT NULL constraint */
  u8 isPrimKey;    /* True if this column is part of the PRIMARY KEY */
  char affinity;   /* One of the SQLITE_AFF_... values */
};

/*
** A "Collating Sequence" is defined by an instance of the following
** structure. Conceptually, a collating sequence consists of a name and
** a comparison routine that defines the order of that sequence.
**
** There may two seperate implementations of the collation function, one
** that processes text in UTF-8 encoding (CollSeq.xCmp) and another that
** processes text encoded in UTF-16 (CollSeq.xCmp16), using the machine
** native byte order. When a collation sequence is invoked, SQLite selects
** the version that will require the least expensive encoding
** transalations, if any.
**
** The CollSeq.pUser member variable is an extra parameter that passed in
** as the first argument to the UTF-8 comparison function, xCmp.
** CollSeq.pUser16 is the equivalent for the UTF-16 comparison function,
** xCmp16.
**
** If both CollSeq.xCmp and CollSeq.xCmp16 are NULL, it means that the
** collating sequence is undefined.  Indices built on an undefined
** collating sequence may not be read or written.
*/
struct CollSeq {
  char *zName;         /* Name of the collating sequence, UTF-8 encoded */
  u8 enc;              /* Text encoding handled by xCmp() */
  void *pUser;         /* First argument to xCmp() */
  int (*xCmp)(void*,int, const void*, int, const void*);
};

/*
** A sort order can be either ASC or DESC.
*/
#define SQLITE_SO_ASC       0  /* Sort in ascending order */
#define SQLITE_SO_DESC      1  /* Sort in ascending order */

/*
** Column affinity types.
*/
#define SQLITE_AFF_INTEGER  'i'
#define SQLITE_AFF_NUMERIC  'n'
#define SQLITE_AFF_TEXT     't'
#define SQLITE_AFF_NONE     'o'


/*
** Each SQL table is represented in memory by an instance of the
** following structure.
**
** Table.zName is the name of the table.  The case of the original
** CREATE TABLE statement is stored, but case is not significant for
** comparisons.
**
** Table.nCol is the number of columns in this table.  Table.aCol is a
** pointer to an array of Column structures, one for each column.
**
** If the table has an INTEGER PRIMARY KEY, then Table.iPKey is the index of
** the column that is that key.   Otherwise Table.iPKey is negative.  Note
** that the datatype of the PRIMARY KEY must be INTEGER for this field to
** be set.  An INTEGER PRIMARY KEY is used as the rowid for each row of
** the table.  If a table has no INTEGER PRIMARY KEY, then a random rowid
** is generated for each row of the table.  Table.hasPrimKey is true if
** the table has any PRIMARY KEY, INTEGER or otherwise.
**
** Table.tnum is the page number for the root BTree page of the table in the
** database file.  If Table.iDb is the index of the database table backend
** in sqlite.aDb[].  0 is for the main database and 1 is for the file that
** holds temporary tables and indices.  If Table.isTransient
** is true, then the table is stored in a file that is automatically deleted
** when the VDBE cursor to the table is closed.  In this case Table.tnum 
** refers VDBE cursor number that holds the table open, not to the root
** page number.  Transient tables are used to hold the results of a
** sub-query that appears instead of a real table name in the FROM clause 
** of a SELECT statement.
*/
struct Table {
  char *zName;     /* Name of the table */
  int nCol;        /* Number of columns in this table */
  Column *aCol;    /* Information about each column */
  int iPKey;       /* If not less then 0, use aCol[iPKey] as the primary key */
  Index *pIndex;   /* List of SQL indexes on this table. */
  int tnum;        /* Root BTree node for this table (see note above) */
  Select *pSelect; /* NULL for tables.  Points to definition if a view. */
  u8 readOnly;     /* True if this table should not be written by the user */
  u8 iDb;          /* Index into sqlite.aDb[] of the backend for this table */
  u8 isTransient;  /* True if automatically deleted when VDBE finishes */
  u8 hasPrimKey;   /* True if there exists a primary key */
  u8 keyConf;      /* What to do in case of uniqueness conflict on iPKey */
  Trigger *pTrigger; /* List of SQL triggers on this table */
  FKey *pFKey;       /* Linked list of all foreign keys in this table */
  char *zColAff;     /* String defining the affinity of each column */
};

/*
** Each foreign key constraint is an instance of the following structure.
**
** A foreign key is associated with two tables.  The "from" table is
** the table that contains the REFERENCES clause that creates the foreign
** key.  The "to" table is the table that is named in the REFERENCES clause.
** Consider this example:
**
**     CREATE TABLE ex1(
**       a INTEGER PRIMARY KEY,
**       b INTEGER CONSTRAINT fk1 REFERENCES ex2(x)
**     );
**
** For foreign key "fk1", the from-table is "ex1" and the to-table is "ex2".
**
** Each REFERENCES clause generates an instance of the following structure
** which is attached to the from-table.  The to-table need not exist when
** the from-table is created.  The existance of the to-table is not checked
** until an attempt is made to insert data into the from-table.
**
** The sqlite.aFKey hash table stores pointers to this structure
** given the name of a to-table.  For each to-table, all foreign keys
** associated with that table are on a linked list using the FKey.pNextTo
** field.
*/
struct FKey {
  Table *pFrom;     /* The table that constains the REFERENCES clause */
  FKey *pNextFrom;  /* Next foreign key in pFrom */
  char *zTo;        /* Name of table that the key points to */
  FKey *pNextTo;    /* Next foreign key that points to zTo */
  int nCol;         /* Number of columns in this key */
  struct sColMap {  /* Mapping of columns in pFrom to columns in zTo */
    int iFrom;         /* Index of column in pFrom */
    char *zCol;        /* Name of column in zTo.  If 0 use PRIMARY KEY */
  } *aCol;          /* One entry for each of nCol column s */
  u8 isDeferred;    /* True if constraint checking is deferred till COMMIT */
  u8 updateConf;    /* How to resolve conflicts that occur on UPDATE */
  u8 deleteConf;    /* How to resolve conflicts that occur on DELETE */
  u8 insertConf;    /* How to resolve conflicts that occur on INSERT */
};

/*
** SQLite supports many different ways to resolve a contraint
** error.  ROLLBACK processing means that a constraint violation
** causes the operation in process to fail and for the current transaction
** to be rolled back.  ABORT processing means the operation in process
** fails and any prior changes from that one operation are backed out,
** but the transaction is not rolled back.  FAIL processing means that
** the operation in progress stops and returns an error code.  But prior
** changes due to the same operation are not backed out and no rollback
** occurs.  IGNORE means that the particular row that caused the constraint
** error is not inserted or updated.  Processing continues and no error
** is returned.  REPLACE means that preexisting database rows that caused
** a UNIQUE constraint violation are removed so that the new insert or
** update can proceed.  Processing continues and no error is reported.
**
** RESTRICT, SETNULL, and CASCADE actions apply only to foreign keys.
** RESTRICT is the same as ABORT for IMMEDIATE foreign keys and the
** same as ROLLBACK for DEFERRED keys.  SETNULL means that the foreign
** key is set to NULL.  CASCADE means that a DELETE or UPDATE of the
** referenced table row is propagated into the row that holds the
** foreign key.
** 
** The following symbolic values are used to record which type
** of action to take.
*/
#define OE_None     0   /* There is no constraint to check */
#define OE_Rollback 1   /* Fail the operation and rollback the transaction */
#define OE_Abort    2   /* Back out changes but do no rollback transaction */
#define OE_Fail     3   /* Stop the operation but leave all prior changes */
#define OE_Ignore   4   /* Ignore the error. Do not do the INSERT or UPDATE */
#define OE_Replace  5   /* Delete existing record, then do INSERT or UPDATE */

#define OE_Restrict 6   /* OE_Abort for IMMEDIATE, OE_Rollback for DEFERRED */
#define OE_SetNull  7   /* Set the foreign key value to NULL */
#define OE_SetDflt  8   /* Set the foreign key value to its default */
#define OE_Cascade  9   /* Cascade the changes */

#define OE_Default  99  /* Do whatever the default action is */


/*
** An instance of the following structure is passed as the first
** argument to sqlite3VdbeKeyCompare and is used to control the 
** comparison of the two index keys.
**
** If the KeyInfo.incrKey value is true and the comparison would
** otherwise be equal, then return a result as if the second key larger.
*/
struct KeyInfo {
  u8 enc;             /* Text encoding - one of the TEXT_Utf* values */
  u8 incrKey;         /* Increase 2nd key by epsilon before comparison */
  int nField;         /* Number of entries in aColl[] */
  u8 *aSortOrder;     /* If defined an aSortOrder[i] is true, sort DESC */
  CollSeq *aColl[1];  /* Collating sequence for each term of the key */
};

/*
** Each SQL index is represented in memory by an
** instance of the following structure.
**
** The columns of the table that are to be indexed are described
** by the aiColumn[] field of this structure.  For example, suppose
** we have the following table and index:
**
**     CREATE TABLE Ex1(c1 int, c2 int, c3 text);
**     CREATE INDEX Ex2 ON Ex1(c3,c1);
**
** In the Table structure describing Ex1, nCol==3 because there are
** three columns in the table.  In the Index structure describing
** Ex2, nColumn==2 since 2 of the 3 columns of Ex1 are indexed.
** The value of aiColumn is {2, 0}.  aiColumn[0]==2 because the 
** first column to be indexed (c3) has an index of 2 in Ex1.aCol[].
** The second column to be indexed (c1) has an index of 0 in
** Ex1.aCol[], hence Ex2.aiColumn[1]==0.
**
** The Index.onError field determines whether or not the indexed columns
** must be unique and what to do if they are not.  When Index.onError=OE_None,
** it means this is not a unique index.  Otherwise it is a unique index
** and the value of Index.onError indicate the which conflict resolution 
** algorithm to employ whenever an attempt is made to insert a non-unique
** element.
*/
struct Index {
  char *zName;     /* Name of this index */
  int nColumn;     /* Number of columns in the table used by this index */
  int *aiColumn;   /* Which columns are used by this index.  1st is 0 */
  Table *pTable;   /* The SQL table being indexed */
  int tnum;        /* Page containing root of this index in database file */
  u8 onError;      /* OE_Abort, OE_Ignore, OE_Replace, or OE_None */
  u8 autoIndex;    /* True if is automatically created (ex: by UNIQUE) */
  u8 iDb;          /* Index in sqlite.aDb[] of where this index is stored */
  char *zColAff;   /* String defining the affinity of each column */
  Index *pNext;    /* The next index associated with the same table */
  KeyInfo keyInfo; /* Info on how to order keys.  MUST BE LAST */
};

/*
** Each token coming out of the lexer is an instance of
** this structure.  Tokens are also used as part of an expression.
**
** Note if Token.z==0 then Token.dyn and Token.n are undefined and
** may contain random values.  Do not make any assuptions about Token.dyn
** and Token.n when Token.z==0.
*/
struct Token {
  const unsigned char *z; /* Text of the token.  Not NULL-terminated! */
  unsigned dyn  : 1;      /* True for malloced memory, false for static */
  unsigned n    : 31;     /* Number of characters in this token */
};

/*
** Each node of an expression in the parse tree is an instance
** of this structure.
**
** Expr.op is the opcode.  The integer parser token codes are reused
** as opcodes here.  For example, the parser defines TK_GE to be an integer
** code representing the ">=" operator.  This same integer code is reused
** to represent the greater-than-or-equal-to operator in the expression
** tree.
**
** Expr.pRight and Expr.pLeft are subexpressions.  Expr.pList is a list
** of argument if the expression is a function.
**
** Expr.token is the operator token for this node.  For some expressions
** that have subexpressions, Expr.token can be the complete text that gave
** rise to the Expr.  In the latter case, the token is marked as being
** a compound token.
**
** An expression of the form ID or ID.ID refers to a column in a table.
** For such expressions, Expr.op is set to TK_COLUMN and Expr.iTable is
** the integer cursor number of a VDBE cursor pointing to that table and
** Expr.iColumn is the column number for the specific column.  If the
** expression is used as a result in an aggregate SELECT, then the
** value is also stored in the Expr.iAgg column in the aggregate so that
** it can be accessed after all aggregates are computed.
**
** If the expression is a function, the Expr.iTable is an integer code
** representing which function.  If the expression is an unbound variable
** marker (a question mark character '?' in the original SQL) then the
** Expr.iTable holds the index number for that variable.
**
** The Expr.pSelect field points to a SELECT statement.  The SELECT might
** be the right operand of an IN operator.  Or, if a scalar SELECT appears
** in an expression the opcode is TK_SELECT and Expr.pSelect is the only
** operand.
*/
struct Expr {
  u8 op;                 /* Operation performed by this node */
  char affinity;         /* The affinity of the column or 0 if not a column */
  u8 iDb;                /* Database referenced by this expression */
  u8 flags;              /* Various flags.  See below */
  CollSeq *pColl;        /* The collation type of the column or 0 */
  Expr *pLeft, *pRight;  /* Left and right subnodes */
  ExprList *pList;       /* A list of expressions used as function arguments
                         ** or in "<expr> IN (<expr-list)" */
  Token token;           /* An operand token */
  Token span;            /* Complete text of the expression */
  int iTable, iColumn;   /* When op==TK_COLUMN, then this expr node means the
                         ** iColumn-th field of the iTable-th table. */
  int iAgg;              /* When op==TK_COLUMN and pParse->useAgg==TRUE, pull
                         ** result from the iAgg-th element of the aggregator */
  Select *pSelect;       /* When the expression is a sub-select.  Also the
                         ** right side of "<expr> IN (<select>)" */
};

/*
** The following are the meanings of bits in the Expr.flags field.
*/
#define EP_FromJoin     0x0001  /* Originated in ON or USING clause of a join */

/*
** These macros can be used to test, set, or clear bits in the 
** Expr.flags field.
*/
#define ExprHasProperty(E,P)     (((E)->flags&(P))==(P))
#define ExprHasAnyProperty(E,P)  (((E)->flags&(P))!=0)
#define ExprSetProperty(E,P)     (E)->flags|=(P)
#define ExprClearProperty(E,P)   (E)->flags&=~(P)

/*
** A list of expressions.  Each expression may optionally have a
** name.  An expr/name combination can be used in several ways, such
** as the list of "expr AS ID" fields following a "SELECT" or in the
** list of "ID = expr" items in an UPDATE.  A list of expressions can
** also be used as the argument to a function, in which case the a.zName
** field is not used.
*/
struct ExprList {
  int nExpr;             /* Number of expressions on the list */
  int nAlloc;            /* Number of entries allocated below */
  struct ExprList_item {
    Expr *pExpr;           /* The list of expressions */
    char *zName;           /* Token associated with this expression */
    u8 sortOrder;          /* 1 for DESC or 0 for ASC */
    u8 isAgg;              /* True if this is an aggregate like count(*) */
    u8 done;               /* A flag to indicate when processing is finished */
  } *a;                  /* One entry for each expression */
};

/*
** An instance of this structure can hold a simple list of identifiers,
** such as the list "a,b,c" in the following statements:
**
**      INSERT INTO t(a,b,c) VALUES ...;
**      CREATE INDEX idx ON t(a,b,c);
**      CREATE TRIGGER trig BEFORE UPDATE ON t(a,b,c) ...;
**
** The IdList.a.idx field is used when the IdList represents the list of
** column names after a table name in an INSERT statement.  In the statement
**
**     INSERT INTO t(a,b,c) ...
**
** If "a" is the k-th column of table "t", then IdList.a[0].idx==k.
*/
struct IdList {
  int nId;         /* Number of identifiers on the list */
  int nAlloc;      /* Number of entries allocated for a[] below */
  struct IdList_item {
    char *zName;      /* Name of the identifier */
    int idx;          /* Index in some Table.aCol[] of a column named zName */
  } *a;
};

/*
** The following structure describes the FROM clause of a SELECT statement.
** Each table or subquery in the FROM clause is a separate element of
** the SrcList.a[] array.
**
** With the addition of multiple database support, the following structure
** can also be used to describe a particular table such as the table that
** is modified by an INSERT, DELETE, or UPDATE statement.  In standard SQL,
** such a table must be a simple name: ID.  But in SQLite, the table can
** now be identified by a database name, a dot, then the table name: ID.ID.
*/
struct SrcList {
  i16 nSrc;        /* Number of tables or subqueries in the FROM clause */
  i16 nAlloc;      /* Number of entries allocated in a[] below */
  struct SrcList_item {
    char *zDatabase;  /* Name of database holding this table */
    char *zName;      /* Name of the table */
    char *zAlias;     /* The "B" part of a "A AS B" phrase.  zName is the "A" */
    Table *pTab;      /* An SQL table corresponding to zName */
    Select *pSelect;  /* A SELECT statement used in place of a table name */
    int jointype;     /* Type of join between this table and the next */
    int iCursor;      /* The VDBE cursor number used to access this table */
    Expr *pOn;        /* The ON clause of a join */
    IdList *pUsing;   /* The USING clause of a join */
  } a[1];             /* One entry for each identifier on the list */
};

/*
** Permitted values of the SrcList.a.jointype field
*/
#define JT_INNER     0x0001    /* Any kind of inner or cross join */
#define JT_NATURAL   0x0002    /* True for a "natural" join */
#define JT_LEFT      0x0004    /* Left outer join */
#define JT_RIGHT     0x0008    /* Right outer join */
#define JT_OUTER     0x0010    /* The "OUTER" keyword is present */
#define JT_ERROR     0x0020    /* unknown or unsupported join type */

/*
** For each nested loop in a WHERE clause implementation, the WhereInfo
** structure contains a single instance of this structure.  This structure
** is intended to be private the the where.c module and should not be
** access or modified by other modules.
*/
struct WhereLevel {
  int iMem;            /* Memory cell used by this level */
  Index *pIdx;         /* Index used */
  int iCur;            /* Cursor number used for this index */
  int score;           /* How well this indexed scored */
  int brk;             /* Jump here to break out of the loop */
  int cont;            /* Jump here to continue with the next loop cycle */
  int op, p1, p2;      /* Opcode used to terminate the loop */
  int iLeftJoin;       /* Memory cell used to implement LEFT OUTER JOIN */
  int top;             /* First instruction of interior of the loop */
  int inOp, inP1, inP2;/* Opcode used to implement an IN operator */
  int bRev;            /* Do the scan in the reverse direction */
};

/*
** The WHERE clause processing routine has two halves.  The
** first part does the start of the WHERE loop and the second
** half does the tail of the WHERE loop.  An instance of
** this structure is returned by the first half and passed
** into the second half to give some continuity.
*/
struct WhereInfo {
  Parse *pParse;
  SrcList *pTabList;   /* List of tables in the join */
  int iContinue;       /* Jump here to continue with next record */
  int iBreak;          /* Jump here to break out of the loop */
  int nLevel;          /* Number of nested loop */
  WhereLevel a[1];     /* Information about each nest loop in the WHERE */
};

/*
** An instance of the following structure contains all information
** needed to generate code for a single SELECT statement.
**
** The zSelect field is used when the Select structure must be persistent.
** Normally, the expression tree points to tokens in the original input
** string that encodes the select.  But if the Select structure must live
** longer than its input string (for example when it is used to describe
** a VIEW) we have to make a copy of the input string so that the nodes
** of the expression tree will have something to point to.  zSelect is used
** to hold that copy.
**
** nLimit is set to -1 if there is no LIMIT clause.  nOffset is set to 0.
** If there is a LIMIT clause, the parser sets nLimit to the value of the
** limit and nOffset to the value of the offset (or 0 if there is not
** offset).  But later on, nLimit and nOffset become the memory locations
** in the VDBE that record the limit and offset counters.
*/
struct Select {
  ExprList *pEList;      /* The fields of the result */
  u8 op;                 /* One of: TK_UNION TK_ALL TK_INTERSECT TK_EXCEPT */
  u8 isDistinct;         /* True if the DISTINCT keyword is present */
  SrcList *pSrc;         /* The FROM clause */
  Expr *pWhere;          /* The WHERE clause */
  ExprList *pGroupBy;    /* The GROUP BY clause */
  Expr *pHaving;         /* The HAVING clause */
  ExprList *pOrderBy;    /* The ORDER BY clause */
  Select *pPrior;        /* Prior select in a compound select statement */
  int nLimit, nOffset;   /* LIMIT and OFFSET values.  -1 means not used */
  int iLimit, iOffset;   /* Memory registers holding LIMIT & OFFSET counters */
  char *zSelect;         /* Complete text of the SELECT command */
  IdList **ppOpenTemp;   /* OP_OpenTemp addresses used by multi-selects */
};

/*
** The results of a select can be distributed in several ways.
*/
#define SRT_Callback     1  /* Invoke a callback with each row of result */
#define SRT_Mem          2  /* Store result in a memory cell */
#define SRT_Set          3  /* Store result as unique keys in a table */
#define SRT_Union        5  /* Store result as keys in a table */
#define SRT_Except       6  /* Remove result from a UNION table */
#define SRT_Table        7  /* Store result as data with a unique key */
#define SRT_TempTable    8  /* Store result in a trasient table */
#define SRT_Discard      9  /* Do not save the results anywhere */
#define SRT_Sorter      10  /* Store results in the sorter */
#define SRT_Subroutine  11  /* Call a subroutine to handle results */

/*
** When a SELECT uses aggregate functions (like "count(*)" or "avg(f1)")
** we have to do some additional analysis of expressions.  An instance
** of the following structure holds information about a single subexpression
** somewhere in the SELECT statement.  An array of these structures holds
** all the information we need to generate code for aggregate
** expressions.
**
** Note that when analyzing a SELECT containing aggregates, both
** non-aggregate field variables and aggregate functions are stored
** in the AggExpr array of the Parser structure.
**
** The pExpr field points to an expression that is part of either the
** field list, the GROUP BY clause, the HAVING clause or the ORDER BY
** clause.  The expression will be freed when those clauses are cleaned
** up.  Do not try to delete the expression attached to AggExpr.pExpr.
**
** If AggExpr.pExpr==0, that means the expression is "count(*)".
*/
struct AggExpr {
  int isAgg;        /* if TRUE contains an aggregate function */
  Expr *pExpr;      /* The expression */
  FuncDef *pFunc;   /* Information about the aggregate function */
};

/*
** An SQL parser context.  A copy of this structure is passed through
** the parser and down into all the parser action routine in order to
** carry around information that is global to the entire parse.
*/
struct Parse {
  sqlite3 *db;         /* The main database structure */
  int rc;              /* Return code from execution */
  char *zErrMsg;       /* An error message */
  Token sErrToken;     /* The token at which the error occurred */
  Token sNameToken;    /* Token with unqualified schema object name */
  Token sLastToken;    /* The last token parsed */
  const char *zSql;    /* All SQL text */
  const char *zTail;   /* All SQL text past the last semicolon parsed */
  Table *pNewTable;    /* A table being constructed by CREATE TABLE */
  Vdbe *pVdbe;         /* An engine for executing database bytecode */
  u8 colNamesSet;      /* TRUE after OP_ColumnName has been issued to pVdbe */
  u8 explain;          /* True if the EXPLAIN flag is found on the query */
  u8 nameClash;        /* A permanent table name clashes with temp table name */
  u8 useAgg;           /* If true, extract field values from the aggregator
                       ** while generating expressions.  Normally false */
  u8 checkSchema;      /* Causes schema cookie check after an error */
  int nErr;            /* Number of errors seen */
  int nTab;            /* Number of previously allocated VDBE cursors */
  int nMem;            /* Number of memory cells used so far */
  int nSet;            /* Number of sets used so far */
  int nAgg;            /* Number of aggregate expressions */
  int nVar;            /* Number of '?' variables seen in the SQL so far */
  int nVarExpr;        /* Number of used slots in apVarExpr[] */
  int nVarExprAlloc;   /* Number of allocated slots in apVarExpr[] */
  Expr **apVarExpr;    /* Pointers to :aaa and $aaaa wildcard expressions */
  AggExpr *aAgg;       /* An array of aggregate expressions */
  const char *zAuthContext; /* The 6th parameter to db->xAuth callbacks */
  Trigger *pNewTrigger;     /* Trigger under construct by a CREATE TRIGGER */
  TriggerStack *trigStack;  /* Trigger actions being coded */
  u32 cookieMask;      /* Bitmask of schema verified databases */
  int cookieValue[MAX_ATTACHED+2];  /* Values of cookies to verify */
  int cookieGoto;      /* Address of OP_Goto to cookie verifier subroutine */
  u32 writeMask;       /* Start a write transaction on these databases */
};

/*
** An instance of the following structure can be declared on a stack and used
** to save the Parse.zAuthContext value so that it can be restored later.
*/
struct AuthContext {
  const char *zAuthContext;   /* Put saved Parse.zAuthContext here */
  Parse *pParse;              /* The Parse structure */
};

/*
** Bitfield flags for P2 value in OP_PutIntKey and OP_Delete
*/
#define OPFLAG_NCHANGE   1    /* Set to update db->nChange */
#define OPFLAG_LASTROWID 2    /* Set to update db->lastRowid */

/*
 * Each trigger present in the database schema is stored as an instance of
 * struct Trigger. 
 *
 * Pointers to instances of struct Trigger are stored in two ways.
 * 1. In the "trigHash" hash table (part of the sqlite3* that represents the 
 *    database). This allows Trigger structures to be retrieved by name.
 * 2. All triggers associated with a single table form a linked list, using the
 *    pNext member of struct Trigger. A pointer to the first element of the
 *    linked list is stored as the "pTrigger" member of the associated
 *    struct Table.
 *
 * The "step_list" member points to the first element of a linked list
 * containing the SQL statements specified as the trigger program.
 */
struct Trigger {
  char *name;             /* The name of the trigger                        */
  char *table;            /* The table or view to which the trigger applies */
  u8 iDb;                 /* Database containing this trigger               */
  u8 iTabDb;              /* Database containing Trigger.table              */
  u8 op;                  /* One of TK_DELETE, TK_UPDATE, TK_INSERT         */
  u8 tr_tm;               /* One of TK_BEFORE, TK_AFTER */
  Expr *pWhen;            /* The WHEN clause of the expresion (may be NULL) */
  IdList *pColumns;       /* If this is an UPDATE OF <column-list> trigger,
                             the <column-list> is stored here */
  int foreach;            /* One of TK_ROW or TK_STATEMENT */
  Token nameToken;        /* Token containing zName. Use during parsing only */

  TriggerStep *step_list; /* Link list of trigger program steps             */
  Trigger *pNext;         /* Next trigger associated with the table */
};

/*
 * An instance of struct TriggerStep is used to store a single SQL statement
 * that is a part of a trigger-program. 
 *
 * Instances of struct TriggerStep are stored in a singly linked list (linked
 * using the "pNext" member) referenced by the "step_list" member of the 
 * associated struct Trigger instance. The first element of the linked list is
 * the first step of the trigger-program.
 * 
 * The "op" member indicates whether this is a "DELETE", "INSERT", "UPDATE" or
 * "SELECT" statement. The meanings of the other members is determined by the 
 * value of "op" as follows:
 *
 * (op == TK_INSERT)
 * orconf    -> stores the ON CONFLICT algorithm
 * pSelect   -> If this is an INSERT INTO ... SELECT ... statement, then
 *              this stores a pointer to the SELECT statement. Otherwise NULL.
 * target    -> A token holding the name of the table to insert into.
 * pExprList -> If this is an INSERT INTO ... VALUES ... statement, then
 *              this stores values to be inserted. Otherwise NULL.
 * pIdList   -> If this is an INSERT INTO ... (<column-names>) VALUES ... 
 *              statement, then this stores the column-names to be
 *              inserted into.
 *
 * (op == TK_DELETE)
 * target    -> A token holding the name of the table to delete from.
 * pWhere    -> The WHERE clause of the DELETE statement if one is specified.
 *              Otherwise NULL.
 * 
 * (op == TK_UPDATE)
 * target    -> A token holding the name of the table to update rows of.
 * pWhere    -> The WHERE clause of the UPDATE statement if one is specified.
 *              Otherwise NULL.
 * pExprList -> A list of the columns to update and the expressions to update
 *              them to. See sqlite3Update() documentation of "pChanges"
 *              argument.
 * 
 */
struct TriggerStep {
  int op;              /* One of TK_DELETE, TK_UPDATE, TK_INSERT, TK_SELECT */
  int orconf;          /* OE_Rollback etc. */
  Trigger *pTrig;      /* The trigger that this step is a part of */

  Select *pSelect;     /* Valid for SELECT and sometimes 
			  INSERT steps (when pExprList == 0) */
  Token target;        /* Valid for DELETE, UPDATE, INSERT steps */
  Expr *pWhere;        /* Valid for DELETE, UPDATE steps */
  ExprList *pExprList; /* Valid for UPDATE statements and sometimes 
			   INSERT steps (when pSelect == 0)         */
  IdList *pIdList;     /* Valid for INSERT statements only */

  TriggerStep * pNext; /* Next in the link-list */
};

/*
 * An instance of struct TriggerStack stores information required during code
 * generation of a single trigger program. While the trigger program is being
 * coded, its associated TriggerStack instance is pointed to by the
 * "pTriggerStack" member of the Parse structure.
 *
 * The pTab member points to the table that triggers are being coded on. The 
 * newIdx member contains the index of the vdbe cursor that points at the temp
 * table that stores the new.* references. If new.* references are not valid
 * for the trigger being coded (for example an ON DELETE trigger), then newIdx
 * is set to -1. The oldIdx member is analogous to newIdx, for old.* references.
 *
 * The ON CONFLICT policy to be used for the trigger program steps is stored 
 * as the orconf member. If this is OE_Default, then the ON CONFLICT clause 
 * specified for individual triggers steps is used.
 *
 * struct TriggerStack has a "pNext" member, to allow linked lists to be
 * constructed. When coding nested triggers (triggers fired by other triggers)
 * each nested trigger stores its parent trigger's TriggerStack as the "pNext" 
 * pointer. Once the nested trigger has been coded, the pNext value is restored
 * to the pTriggerStack member of the Parse stucture and coding of the parent
 * trigger continues.
 *
 * Before a nested trigger is coded, the linked list pointed to by the 
 * pTriggerStack is scanned to ensure that the trigger is not about to be coded
 * recursively. If this condition is detected, the nested trigger is not coded.
 */
struct TriggerStack {
  Table *pTab;         /* Table that triggers are currently being coded on */
  int newIdx;          /* Index of vdbe cursor to "new" temp table */
  int oldIdx;          /* Index of vdbe cursor to "old" temp table */
  int orconf;          /* Current orconf policy */
  int ignoreJump;      /* where to jump to for a RAISE(IGNORE) */
  Trigger *pTrigger;   /* The trigger currently being coded */
  TriggerStack *pNext; /* Next trigger down on the trigger stack */
};

/*
** The following structure contains information used by the sqliteFix...
** routines as they walk the parse tree to make database references
** explicit.  
*/
typedef struct DbFixer DbFixer;
struct DbFixer {
  Parse *pParse;      /* The parsing context.  Error messages written here */
  const char *zDb;    /* Make sure all objects are contained in this database */
  const char *zType;  /* Type of the container - used for error messages */
  const Token *pName; /* Name of the container - used for error messages */
};

/*
** A pointer to this structure is used to communicate information
** from sqlite3Init and OP_ParseSchema into the sqlite3InitCallback.
*/
typedef struct {
  sqlite3 *db;        /* The database being initialized */
  char **pzErrMsg;    /* Error message stored here */
} InitData;


/*
 * This global flag is set for performance testing of triggers. When it is set
 * SQLite will perform the overhead of building new and old trigger references 
 * even when no triggers exist
 */
extern int sqlite3_always_code_trigger_setup;

/*
** Internal function prototypes
*/
int sqlite3StrICmp(const char *, const char *);
int sqlite3StrNICmp(const char *, const char *, int);
int sqlite3HashNoCase(const char *, int);
int sqlite3IsNumber(const char*, int*, u8);
int sqlite3Compare(const char *, const char *);
int sqlite3SortCompare(const char *, const char *);
void sqlite3RealToSortable(double r, char *);
#ifdef SQLITE_DEBUG
  void *sqlite3Malloc_(int,int,char*,int);
  void sqlite3Free_(void*,char*,int);
  void *sqlite3Realloc_(void*,int,char*,int);
  char *sqlite3StrDup_(const char*,char*,int);
  char *sqlite3StrNDup_(const char*, int,char*,int);
  void sqlite3CheckMemory(void*,int);
#else
  void *sqlite3Malloc(int);
  void *sqlite3MallocRaw(int);
  void sqlite3Free(void*);
  void *sqlite3Realloc(void*,int);
  char *sqlite3StrDup(const char*);
  char *sqlite3StrNDup(const char*, int);
# define sqlite3CheckMemory(a,b)
#endif
void sqlite3FreeX(void*);
char *sqlite3MPrintf(const char*, ...);
char *sqlite3VMPrintf(const char*, va_list);
void sqlite3DebugPrintf(const char*, ...);
void *sqlite3TextToPtr(const char*);
void sqlite3SetString(char **, const char *, ...);
void sqlite3ErrorMsg(Parse*, const char*, ...);
void sqlite3Dequote(char*);
int sqlite3KeywordCode(const char*, int);
int sqlite3RunParser(Parse*, const char*, char **);
void sqlite3FinishCoding(Parse*);
Expr *sqlite3Expr(int, Expr*, Expr*, Token*);
Expr *sqlite3ExprAnd(Expr*, Expr*);
void sqlite3ExprSpan(Expr*,Token*,Token*);
Expr *sqlite3ExprFunction(ExprList*, Token*);
void sqlite3ExprAssignVarNumber(Parse*, Expr*);
void sqlite3ExprDelete(Expr*);
ExprList *sqlite3ExprListAppend(ExprList*,Expr*,Token*);
void sqlite3ExprListDelete(ExprList*);
int sqlite3Init(sqlite3*, char**);
int sqlite3InitCallback(void*, int, char**, char**);
void sqlite3Pragma(Parse*,Token*,Token*,Token*,int);
void sqlite3ResetInternalSchema(sqlite3*, int);
void sqlite3BeginParse(Parse*,int);
void sqlite3RollbackInternalChanges(sqlite3*);
void sqlite3CommitInternalChanges(sqlite3*);
Table *sqlite3ResultSetOfSelect(Parse*,char*,Select*);
void sqlite3OpenMasterTable(Vdbe *v, int);
void sqlite3StartTable(Parse*,Token*,Token*,Token*,int,int);
void sqlite3AddColumn(Parse*,Token*);
void sqlite3AddNotNull(Parse*, int);
void sqlite3AddPrimaryKey(Parse*, ExprList*, int);
void sqlite3AddColumnType(Parse*,Token*,Token*);
void sqlite3AddDefaultValue(Parse*,Token*,int);
void sqlite3AddCollateType(Parse*, const char*, int);
void sqlite3EndTable(Parse*,Token*,Select*);
void sqlite3CreateView(Parse*,Token*,Token*,Token*,Select*,int);
int sqlite3ViewGetColumnNames(Parse*,Table*);
void sqlite3DropTable(Parse*, SrcList*, int);
void sqlite3DeleteTable(sqlite3*, Table*);
void sqlite3Insert(Parse*, SrcList*, ExprList*, Select*, IdList*, int);
IdList *sqlite3IdListAppend(IdList*, Token*);
int sqlite3IdListIndex(IdList*,const char*);
SrcList *sqlite3SrcListAppend(SrcList*, Token*, Token*);
void sqlite3SrcListAddAlias(SrcList*, Token*);
void sqlite3SrcListAssignCursors(Parse*, SrcList*);
void sqlite3IdListDelete(IdList*);
void sqlite3SrcListDelete(SrcList*);
void sqlite3CreateIndex(Parse*,Token*,Token*,SrcList*,ExprList*,int,Token*,
                        Token*);
void sqlite3DropIndex(Parse*, SrcList*);
void sqlite3AddKeyType(Vdbe*, ExprList*);
void sqlite3AddIdxKeyType(Vdbe*, Index*);
int sqlite3Select(Parse*, Select*, int, int, Select*, int, int*, char *aff);
Select *sqlite3SelectNew(ExprList*,SrcList*,Expr*,ExprList*,Expr*,ExprList*,
                        int,int,int);
void sqlite3SelectDelete(Select*);
void sqlite3SelectUnbind(Select*);
Table *sqlite3SrcListLookup(Parse*, SrcList*);
int sqlite3IsReadOnly(Parse*, Table*, int);
void sqlite3OpenTableForReading(Vdbe*, int iCur, Table*);
void sqlite3DeleteFrom(Parse*, SrcList*, Expr*);
void sqlite3Update(Parse*, SrcList*, ExprList*, Expr*, int);
WhereInfo *sqlite3WhereBegin(Parse*, SrcList*, Expr*, int, ExprList**);
void sqlite3WhereEnd(WhereInfo*);
void sqlite3ExprCode(Parse*, Expr*);
int sqlite3ExprCodeExprList(Parse*, ExprList*);
void sqlite3ExprIfTrue(Parse*, Expr*, int, int);
void sqlite3ExprIfFalse(Parse*, Expr*, int, int);
Table *sqlite3FindTable(sqlite3*,const char*, const char*);
Table *sqlite3LocateTable(Parse*,const char*, const char*);
Index *sqlite3FindIndex(sqlite3*,const char*, const char*);
void sqlite3UnlinkAndDeleteTable(sqlite3*,int,const char*);
void sqlite3UnlinkAndDeleteIndex(sqlite3*,int,const char*);
void sqlite3UnlinkAndDeleteTrigger(sqlite3*,int,const char*);
void sqlite3Vacuum(Parse*, Token*);
int sqlite3RunVacuum(char**, sqlite3*);
char *sqlite3NameFromToken(Token*);
int sqlite3ExprCheck(Parse*, Expr*, int, int*);
int sqlite3ExprCompare(Expr*, Expr*);
int sqliteFuncId(Token*);
int sqlite3ExprResolveIds(Parse*, SrcList*, ExprList*, Expr*);
int sqlite3ExprResolveAndCheck(Parse*,SrcList*,ExprList*,Expr*,int,int*);
int sqlite3ExprAnalyzeAggregates(Parse*, Expr*);
Vdbe *sqlite3GetVdbe(Parse*);
void sqlite3Randomness(int, void*);
void sqlite3RollbackAll(sqlite3*);
void sqlite3CodeVerifySchema(Parse*, int);
void sqlite3BeginTransaction(Parse*, int);
void sqlite3CommitTransaction(Parse*);
void sqlite3RollbackTransaction(Parse*);
int sqlite3ExprIsConstant(Expr*);
int sqlite3ExprIsInteger(Expr*, int*);
int sqlite3IsRowid(const char*);
void sqlite3GenerateRowDelete(sqlite3*, Vdbe*, Table*, int, int);
void sqlite3GenerateRowIndexDelete(sqlite3*, Vdbe*, Table*, int, char*);
void sqlite3GenerateIndexKey(Vdbe*, Index*, int);
void sqlite3GenerateConstraintChecks(Parse*,Table*,int,char*,int,int,int,int);
void sqlite3CompleteInsertion(Parse*, Table*, int, char*, int, int, int);
void sqlite3OpenTableAndIndices(Parse*, Table*, int, int);
void sqlite3BeginWriteOperation(Parse*, int, int);
Expr *sqlite3ExprDup(Expr*);
void sqlite3TokenCopy(Token*, Token*);
ExprList *sqlite3ExprListDup(ExprList*);
SrcList *sqlite3SrcListDup(SrcList*);
IdList *sqlite3IdListDup(IdList*);
Select *sqlite3SelectDup(Select*);
FuncDef *sqlite3FindFunction(sqlite3*,const char*,int,int,u8,int);
void sqlite3RegisterBuiltinFunctions(sqlite3*);
void sqlite3RegisterDateTimeFunctions(sqlite3*);
int sqlite3SafetyOn(sqlite3*);
int sqlite3SafetyOff(sqlite3*);
int sqlite3SafetyCheck(sqlite3*);
void sqlite3ChangeCookie(sqlite3*, Vdbe*, int);
void sqlite3BeginTrigger(Parse*, Token*,Token*,int,int,IdList*,SrcList*,
                         int,Expr*,int);
void sqlite3FinishTrigger(Parse*, TriggerStep*, Token*);
void sqlite3DropTrigger(Parse*, SrcList*);
void sqlite3DropTriggerPtr(Parse*, Trigger*, int);
int sqlite3TriggersExist(Parse* , Trigger* , int , int , int, ExprList*);
int sqlite3CodeRowTrigger(Parse*, int, ExprList*, int, Table *, int, int, 
                         int, int);
void sqliteViewTriggers(Parse*, Table*, Expr*, int, ExprList*);
void sqlite3DeleteTriggerStep(TriggerStep*);
TriggerStep *sqlite3TriggerSelectStep(Select*);
TriggerStep *sqlite3TriggerInsertStep(Token*, IdList*, ExprList*, Select*, int);
TriggerStep *sqlite3TriggerUpdateStep(Token*, ExprList*, Expr*, int);
TriggerStep *sqlite3TriggerDeleteStep(Token*, Expr*);
void sqlite3DeleteTrigger(Trigger*);
int sqlite3JoinType(Parse*, Token*, Token*, Token*);
void sqlite3CreateForeignKey(Parse*, ExprList*, Token*, ExprList*, int);
void sqlite3DeferForeignKey(Parse*, int);
#ifndef SQLITE_OMIT_AUTHORIZATION
  void sqlite3AuthRead(Parse*,Expr*,SrcList*);
  int sqlite3AuthCheck(Parse*,int, const char*, const char*, const char*);
  void sqlite3AuthContextPush(Parse*, AuthContext*, const char*);
  void sqlite3AuthContextPop(AuthContext*);
#else
# define sqlite3AuthRead(a,b,c)
# define sqlite3AuthCheck(a,b,c,d,e)    SQLITE_OK
# define sqlite3AuthContextPush(a,b,c)
# define sqlite3AuthContextPop(a)  ((void)(a))
#endif
void sqlite3Attach(Parse*, Token*, Token*, int, Token*);
void sqlite3Detach(Parse*, Token*);
int sqlite3BtreeFactory(const sqlite3 *db, const char *zFilename,
                       int omitJournal, int nCache, Btree **ppBtree);
int sqlite3FixInit(DbFixer*, Parse*, int, const char*, const Token*);
int sqlite3FixSrcList(DbFixer*, SrcList*);
int sqlite3FixSelect(DbFixer*, Select*);
int sqlite3FixExpr(DbFixer*, Expr*);
int sqlite3FixExprList(DbFixer*, ExprList*);
int sqlite3FixTriggerStep(DbFixer*, TriggerStep*);
double sqlite3AtoF(const char *z, const char **);
char *sqlite3_snprintf(int,char*,const char*,...);
int sqlite3GetInt32(const char *, int*);
int sqlite3FitsIn64Bits(const char *);
int sqlite3utf16ByteLen(const void *pData, int nChar);
int sqlite3utf8CharLen(const char *pData, int nByte);
int sqlite3ReadUtf8(const unsigned char *);
int sqlite3PutVarint(unsigned char *, u64);
int sqlite3GetVarint(const unsigned char *, u64 *);
int sqlite3GetVarint32(const unsigned char *, u32 *);
int sqlite3VarintLen(u64 v);
char sqlite3AffinityType(const char *, int);
void sqlite3IndexAffinityStr(Vdbe *, Index *);
void sqlite3TableAffinityStr(Vdbe *, Table *);
char sqlite3CompareAffinity(Expr *pExpr, char aff2);
int sqlite3IndexAffinityOk(Expr *pExpr, char idx_affinity);
char sqlite3ExprAffinity(Expr *pExpr);
int sqlite3atoi64(const char*, i64*);
void sqlite3Error(sqlite3*, int, const char*,...);
void *sqlite3HexToBlob(const char *z);
int sqlite3TwoPartName(Parse *, Token *, Token *, Token **);
const char *sqlite3ErrStr(int);
int sqlite3ReadUniChar(const char *zStr, int *pOffset, u8 *pEnc, int fold);
int sqlite3ReadSchema(Parse *pParse);
CollSeq *sqlite3FindCollSeq(sqlite3*,u8 enc, const char *,int,int);
CollSeq *sqlite3LocateCollSeq(Parse *pParse, const char *zName, int nName);
CollSeq *sqlite3ExprCollSeq(Parse *pParse, Expr *pExpr);
int sqlite3CheckCollSeq(Parse *, CollSeq *);
int sqlite3CheckIndexCollSeq(Parse *, Index *);
int sqlite3CheckObjectName(Parse *, const char *);
void sqlite3VdbeSetChanges(sqlite3 *, int);
void sqlite3utf16Substr(sqlite3_context *,int,sqlite3_value **);

const void *sqlite3ValueText(sqlite3_value*, u8);
int sqlite3ValueBytes(sqlite3_value*, u8);
void sqlite3ValueSetStr(sqlite3_value*, int, const void *,u8, void(*)(void*));
void sqlite3ValueFree(sqlite3_value*);
sqlite3_value *sqlite3ValueNew();
sqlite3_value *sqlite3GetTransientValue(sqlite3*db);
extern const unsigned char sqlite3UpperToLower[];

#endif
