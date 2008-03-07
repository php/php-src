/*
** 2005 May 23 
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
**
** This file contains functions used to access the internal hash tables
** of user defined functions and collation sequences.
**
** $Id$
*/

#include "sqliteInt.h"

/*
** Invoke the 'collation needed' callback to request a collation sequence
** in the database text encoding of name zName, length nName.
** If the collation sequence
*/
static void callCollNeeded(sqlite3 *db, const char *zName, int nName){
  assert( !db->xCollNeeded || !db->xCollNeeded16 );
  if( nName<0 ) nName = strlen(zName);
  if( db->xCollNeeded ){
    char *zExternal = sqlite3DbStrNDup(db, zName, nName);
    if( !zExternal ) return;
    db->xCollNeeded(db->pCollNeededArg, db, (int)ENC(db), zExternal);
    sqlite3_free(zExternal);
  }
#ifndef SQLITE_OMIT_UTF16
  if( db->xCollNeeded16 ){
    char const *zExternal;
    sqlite3_value *pTmp = sqlite3ValueNew(db);
    sqlite3ValueSetStr(pTmp, nName, zName, SQLITE_UTF8, SQLITE_STATIC);
    zExternal = sqlite3ValueText(pTmp, SQLITE_UTF16NATIVE);
    if( zExternal ){
      db->xCollNeeded16(db->pCollNeededArg, db, (int)ENC(db), zExternal);
    }
    sqlite3ValueFree(pTmp);
  }
#endif
}

/*
** This routine is called if the collation factory fails to deliver a
** collation function in the best encoding but there may be other versions
** of this collation function (for other text encodings) available. Use one
** of these instead if they exist. Avoid a UTF-8 <-> UTF-16 conversion if
** possible.
*/
static int synthCollSeq(sqlite3 *db, CollSeq *pColl){
  CollSeq *pColl2;
  char *z = pColl->zName;
  int n = strlen(z);
  int i;
  static const u8 aEnc[] = { SQLITE_UTF16BE, SQLITE_UTF16LE, SQLITE_UTF8 };
  for(i=0; i<3; i++){
    pColl2 = sqlite3FindCollSeq(db, aEnc[i], z, n, 0);
    if( pColl2->xCmp!=0 ){
      memcpy(pColl, pColl2, sizeof(CollSeq));
      pColl->xDel = 0;         /* Do not copy the destructor */
      return SQLITE_OK;
    }
  }
  return SQLITE_ERROR;
}

/*
** This function is responsible for invoking the collation factory callback
** or substituting a collation sequence of a different encoding when the
** requested collation sequence is not available in the database native
** encoding.
** 
** If it is not NULL, then pColl must point to the database native encoding 
** collation sequence with name zName, length nName.
**
** The return value is either the collation sequence to be used in database
** db for collation type name zName, length nName, or NULL, if no collation
** sequence can be found.
*/
CollSeq *sqlite3GetCollSeq(
  sqlite3* db, 
  CollSeq *pColl, 
  const char *zName, 
  int nName
){
  CollSeq *p;

  p = pColl;
  if( !p ){
    p = sqlite3FindCollSeq(db, ENC(db), zName, nName, 0);
  }
  if( !p || !p->xCmp ){
    /* No collation sequence of this type for this encoding is registered.
    ** Call the collation factory to see if it can supply us with one.
    */
    callCollNeeded(db, zName, nName);
    p = sqlite3FindCollSeq(db, ENC(db), zName, nName, 0);
  }
  if( p && !p->xCmp && synthCollSeq(db, p) ){
    p = 0;
  }
  assert( !p || p->xCmp );
  return p;
}

/*
** This routine is called on a collation sequence before it is used to
** check that it is defined. An undefined collation sequence exists when
** a database is loaded that contains references to collation sequences
** that have not been defined by sqlite3_create_collation() etc.
**
** If required, this routine calls the 'collation needed' callback to
** request a definition of the collating sequence. If this doesn't work, 
** an equivalent collating sequence that uses a text encoding different
** from the main database is substituted, if one is available.
*/
int sqlite3CheckCollSeq(Parse *pParse, CollSeq *pColl){
  if( pColl ){
    const char *zName = pColl->zName;
    CollSeq *p = sqlite3GetCollSeq(pParse->db, pColl, zName, -1);
    if( !p ){
      if( pParse->nErr==0 ){
        sqlite3ErrorMsg(pParse, "no such collation sequence: %s", zName);
      }
      pParse->nErr++;
      return SQLITE_ERROR;
    }
    assert( p==pColl );
  }
  return SQLITE_OK;
}



/*
** Locate and return an entry from the db.aCollSeq hash table. If the entry
** specified by zName and nName is not found and parameter 'create' is
** true, then create a new entry. Otherwise return NULL.
**
** Each pointer stored in the sqlite3.aCollSeq hash table contains an
** array of three CollSeq structures. The first is the collation sequence
** prefferred for UTF-8, the second UTF-16le, and the third UTF-16be.
**
** Stored immediately after the three collation sequences is a copy of
** the collation sequence name. A pointer to this string is stored in
** each collation sequence structure.
*/
static CollSeq *findCollSeqEntry(
  sqlite3 *db,
  const char *zName,
  int nName,
  int create
){
  CollSeq *pColl;
  if( nName<0 ) nName = strlen(zName);
  pColl = sqlite3HashFind(&db->aCollSeq, zName, nName);

  if( 0==pColl && create ){
    pColl = sqlite3DbMallocZero(db, 3*sizeof(*pColl) + nName + 1 );
    if( pColl ){
      CollSeq *pDel = 0;
      pColl[0].zName = (char*)&pColl[3];
      pColl[0].enc = SQLITE_UTF8;
      pColl[1].zName = (char*)&pColl[3];
      pColl[1].enc = SQLITE_UTF16LE;
      pColl[2].zName = (char*)&pColl[3];
      pColl[2].enc = SQLITE_UTF16BE;
      memcpy(pColl[0].zName, zName, nName);
      pColl[0].zName[nName] = 0;
      pDel = sqlite3HashInsert(&db->aCollSeq, pColl[0].zName, nName, pColl);

      /* If a malloc() failure occured in sqlite3HashInsert(), it will 
      ** return the pColl pointer to be deleted (because it wasn't added
      ** to the hash table).
      */
      assert( pDel==0 || pDel==pColl );
      if( pDel!=0 ){
        db->mallocFailed = 1;
        sqlite3_free(pDel);
        pColl = 0;
      }
    }
  }
  return pColl;
}

/*
** Parameter zName points to a UTF-8 encoded string nName bytes long.
** Return the CollSeq* pointer for the collation sequence named zName
** for the encoding 'enc' from the database 'db'.
**
** If the entry specified is not found and 'create' is true, then create a
** new entry.  Otherwise return NULL.
**
** A separate function sqlite3LocateCollSeq() is a wrapper around
** this routine.  sqlite3LocateCollSeq() invokes the collation factory
** if necessary and generates an error message if the collating sequence
** cannot be found.
*/
CollSeq *sqlite3FindCollSeq(
  sqlite3 *db,
  u8 enc,
  const char *zName,
  int nName,
  int create
){
  CollSeq *pColl;
  if( zName ){
    pColl = findCollSeqEntry(db, zName, nName, create);
  }else{
    pColl = db->pDfltColl;
  }
  assert( SQLITE_UTF8==1 && SQLITE_UTF16LE==2 && SQLITE_UTF16BE==3 );
  assert( enc>=SQLITE_UTF8 && enc<=SQLITE_UTF16BE );
  if( pColl ) pColl += enc-1;
  return pColl;
}

/*
** Locate a user function given a name, a number of arguments and a flag
** indicating whether the function prefers UTF-16 over UTF-8.  Return a
** pointer to the FuncDef structure that defines that function, or return
** NULL if the function does not exist.
**
** If the createFlag argument is true, then a new (blank) FuncDef
** structure is created and liked into the "db" structure if a
** no matching function previously existed.  When createFlag is true
** and the nArg parameter is -1, then only a function that accepts
** any number of arguments will be returned.
**
** If createFlag is false and nArg is -1, then the first valid
** function found is returned.  A function is valid if either xFunc
** or xStep is non-zero.
**
** If createFlag is false, then a function with the required name and
** number of arguments may be returned even if the eTextRep flag does not
** match that requested.
*/
FuncDef *sqlite3FindFunction(
  sqlite3 *db,       /* An open database */
  const char *zName, /* Name of the function.  Not null-terminated */
  int nName,         /* Number of characters in the name */
  int nArg,          /* Number of arguments.  -1 means any number */
  u8 enc,            /* Preferred text encoding */
  int createFlag     /* Create new entry if true and does not otherwise exist */
){
  FuncDef *p;         /* Iterator variable */
  FuncDef *pFirst;    /* First function with this name */
  FuncDef *pBest = 0; /* Best match found so far */
  int bestmatch = 0;  


  assert( enc==SQLITE_UTF8 || enc==SQLITE_UTF16LE || enc==SQLITE_UTF16BE );
  if( nArg<-1 ) nArg = -1;

  pFirst = (FuncDef*)sqlite3HashFind(&db->aFunc, zName, nName);
  for(p=pFirst; p; p=p->pNext){
    /* During the search for the best function definition, bestmatch is set
    ** as follows to indicate the quality of the match with the definition
    ** pointed to by pBest:
    **
    ** 0: pBest is NULL. No match has been found.
    ** 1: A variable arguments function that prefers UTF-8 when a UTF-16
    **    encoding is requested, or vice versa.
    ** 2: A variable arguments function that uses UTF-16BE when UTF-16LE is
    **    requested, or vice versa.
    ** 3: A variable arguments function using the same text encoding.
    ** 4: A function with the exact number of arguments requested that
    **    prefers UTF-8 when a UTF-16 encoding is requested, or vice versa.
    ** 5: A function with the exact number of arguments requested that
    **    prefers UTF-16LE when UTF-16BE is requested, or vice versa.
    ** 6: An exact match.
    **
    ** A larger value of 'matchqual' indicates a more desirable match.
    */
    if( p->nArg==-1 || p->nArg==nArg || nArg==-1 ){
      int match = 1;          /* Quality of this match */
      if( p->nArg==nArg || nArg==-1 ){
        match = 4;
      }
      if( enc==p->iPrefEnc ){
        match += 2;
      }
      else if( (enc==SQLITE_UTF16LE && p->iPrefEnc==SQLITE_UTF16BE) ||
               (enc==SQLITE_UTF16BE && p->iPrefEnc==SQLITE_UTF16LE) ){
        match += 1;
      }

      if( match>bestmatch ){
        pBest = p;
        bestmatch = match;
      }
    }
  }

  /* If the createFlag parameter is true, and the seach did not reveal an
  ** exact match for the name, number of arguments and encoding, then add a
  ** new entry to the hash table and return it.
  */
  if( createFlag && bestmatch<6 && 
      (pBest = sqlite3DbMallocZero(db, sizeof(*pBest)+nName))!=0 ){
    pBest->nArg = nArg;
    pBest->pNext = pFirst;
    pBest->iPrefEnc = enc;
    memcpy(pBest->zName, zName, nName);
    pBest->zName[nName] = 0;
    if( pBest==sqlite3HashInsert(&db->aFunc,pBest->zName,nName,(void*)pBest) ){
      db->mallocFailed = 1;
      sqlite3_free(pBest);
      return 0;
    }
  }

  if( pBest && (pBest->xStep || pBest->xFunc || createFlag) ){
    return pBest;
  }
  return 0;
}

/*
** Free all resources held by the schema structure. The void* argument points
** at a Schema struct. This function does not call sqlite3_free() on the 
** pointer itself, it just cleans up subsiduary resources (i.e. the contents
** of the schema hash tables).
*/
void sqlite3SchemaFree(void *p){
  Hash temp1;
  Hash temp2;
  HashElem *pElem;
  Schema *pSchema = (Schema *)p;

  temp1 = pSchema->tblHash;
  temp2 = pSchema->trigHash;
  sqlite3HashInit(&pSchema->trigHash, SQLITE_HASH_STRING, 0);
  sqlite3HashClear(&pSchema->aFKey);
  sqlite3HashClear(&pSchema->idxHash);
  for(pElem=sqliteHashFirst(&temp2); pElem; pElem=sqliteHashNext(pElem)){
    sqlite3DeleteTrigger((Trigger*)sqliteHashData(pElem));
  }
  sqlite3HashClear(&temp2);
  sqlite3HashInit(&pSchema->tblHash, SQLITE_HASH_STRING, 0);
  for(pElem=sqliteHashFirst(&temp1); pElem; pElem=sqliteHashNext(pElem)){
    Table *pTab = sqliteHashData(pElem);
    sqlite3DeleteTable(pTab);
  }
  sqlite3HashClear(&temp1);
  pSchema->pSeqTab = 0;
  pSchema->flags &= ~DB_SchemaLoaded;
}

/*
** Find and return the schema associated with a BTree.  Create
** a new one if necessary.
*/
Schema *sqlite3SchemaGet(sqlite3 *db, Btree *pBt){
  Schema * p;
  if( pBt ){
    p = (Schema *)sqlite3BtreeSchema(pBt, sizeof(Schema), sqlite3SchemaFree);
  }else{
    p = (Schema *)sqlite3MallocZero(sizeof(Schema));
  }
  if( !p ){
    db->mallocFailed = 1;
  }else if ( 0==p->file_format ){
    sqlite3HashInit(&p->tblHash, SQLITE_HASH_STRING, 0);
    sqlite3HashInit(&p->idxHash, SQLITE_HASH_STRING, 0);
    sqlite3HashInit(&p->trigHash, SQLITE_HASH_STRING, 0);
    sqlite3HashInit(&p->aFKey, SQLITE_HASH_STRING, 1);
    p->enc = SQLITE_UTF8;
  }
  return p;
}
