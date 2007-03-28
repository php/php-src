/*
** 2001 September 22
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This is the header file for the generic hash-table implemenation
** used in SQLite.
**
** $Id$
*/
#ifndef _SQLITE_HASH_H_
#define _SQLITE_HASH_H_

/* Forward declarations of structures. */
typedef struct Hash Hash;
typedef struct HashElem HashElem;

/* A complete hash table is an instance of the following structure.
** The internals of this structure are intended to be opaque -- client
** code should not attempt to access or modify the fields of this structure
** directly.  Change this structure only by using the routines below.
** However, many of the "procedures" and "functions" for modifying and
** accessing this structure are really macros, so we can't really make
** this structure opaque.
*/
struct Hash {
  char keyClass;          /* SQLITE_HASH_INT, _POINTER, _STRING, _BINARY */
  char copyKey;           /* True if copy of key made on insert */
  int count;              /* Number of entries in this table */
  HashElem *first;        /* The first element of the array */
  int htsize;             /* Number of buckets in the hash table */
  struct _ht {            /* the hash table */
    int count;               /* Number of entries with this hash */
    HashElem *chain;         /* Pointer to first entry with this hash */
  } *ht;
};

/* Each element in the hash table is an instance of the following 
** structure.  All elements are stored on a single doubly-linked list.
**
** Again, this structure is intended to be opaque, but it can't really
** be opaque because it is used by macros.
*/
struct HashElem {
  HashElem *next, *prev;   /* Next and previous elements in the table */
  void *data;              /* Data associated with this element */
  void *pKey; int nKey;    /* Key associated with this element */
};

/*
** There are 4 different modes of operation for a hash table:
**
**   SQLITE_HASH_INT         nKey is used as the key and pKey is ignored.
**
**   SQLITE_HASH_POINTER     pKey is used as the key and nKey is ignored.
**
**   SQLITE_HASH_STRING      pKey points to a string that is nKey bytes long
**                           (including the null-terminator, if any).  Case
**                           is ignored in comparisons.
**
**   SQLITE_HASH_BINARY      pKey points to binary data nKey bytes long. 
**                           memcmp() is used to compare keys.
**
** A copy of the key is made for SQLITE_HASH_STRING and SQLITE_HASH_BINARY
** if the copyKey parameter to HashInit is 1.  
*/
/* #define SQLITE_HASH_INT       1 // NOT USED */
/* #define SQLITE_HASH_POINTER   2 // NOT USED */
#define SQLITE_HASH_STRING    3
#define SQLITE_HASH_BINARY    4

/*
** Access routines.  To delete, insert a NULL pointer.
*/
void sqlite3HashInit(Hash*, int keytype, int copyKey);
void *sqlite3HashInsert(Hash*, const void *pKey, int nKey, void *pData);
void *sqlite3HashFind(const Hash*, const void *pKey, int nKey);
void sqlite3HashClear(Hash*);

/*
** Macros for looping over all elements of a hash table.  The idiom is
** like this:
**
**   Hash h;
**   HashElem *p;
**   ...
**   for(p=sqliteHashFirst(&h); p; p=sqliteHashNext(p)){
**     SomeStructure *pData = sqliteHashData(p);
**     // do something with pData
**   }
*/
#define sqliteHashFirst(H)  ((H)->first)
#define sqliteHashNext(E)   ((E)->next)
#define sqliteHashData(E)   ((E)->data)
#define sqliteHashKey(E)    ((E)->pKey)
#define sqliteHashKeysize(E) ((E)->nKey)

/*
** Number of entries in a hash table
*/
#define sqliteHashCount(H)  ((H)->count)

#endif /* _SQLITE_HASH_H_ */
