/*
** 2003 Feb 4
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** $Id$
**
** This file implements an in-core database using Red-Black balanced
** binary trees.
** 
** It was contributed to SQLite by anonymous on 2003-Feb-04 23:24:49 UTC.
*/
#include "btree.h"
#include "sqliteInt.h"
#include <assert.h>

/*
** Omit this whole file if the SQLITE_OMIT_INMEMORYDB macro is
** defined.  This allows a lot of code to be omitted for installations
** that do not need it.
*/
#ifndef SQLITE_OMIT_INMEMORYDB


typedef struct BtRbTree BtRbTree;
typedef struct BtRbNode BtRbNode;
typedef struct BtRollbackOp BtRollbackOp;
typedef struct Rbtree Rbtree;
typedef struct RbtCursor RbtCursor;

/* Forward declarations */
static BtOps sqliteRbtreeOps;
static BtCursorOps sqliteRbtreeCursorOps;

/*
 * During each transaction (or checkpoint), a linked-list of
 * "rollback-operations" is accumulated. If the transaction is rolled back,
 * then the list of operations must be executed (to restore the database to
 * it's state before the transaction started). If the transaction is to be
 * committed, just delete the list.
 *
 * Each operation is represented as follows, depending on the value of eOp:
 *
 * ROLLBACK_INSERT  ->  Need to insert (pKey, pData) into table iTab.
 * ROLLBACK_DELETE  ->  Need to delete the record (pKey) into table iTab.
 * ROLLBACK_CREATE  ->  Need to create table iTab.
 * ROLLBACK_DROP    ->  Need to drop table iTab.
 */
struct BtRollbackOp {
  u8 eOp;
  int iTab;
  int nKey; 
  void *pKey;
  int nData;
  void *pData;
  BtRollbackOp *pNext;
};

/*
** Legal values for BtRollbackOp.eOp:
*/
#define ROLLBACK_INSERT 1 /* Insert a record */
#define ROLLBACK_DELETE 2 /* Delete a record */
#define ROLLBACK_CREATE 3 /* Create a table */
#define ROLLBACK_DROP   4 /* Drop a table */

struct Rbtree {
  BtOps *pOps;    /* Function table */
  int aMetaData[SQLITE_N_BTREE_META];

  int next_idx;   /* next available table index */
  Hash tblHash;   /* All created tables, by index */
  u8 isAnonymous; /* True if this Rbtree is to be deleted when closed */
  u8 eTransState; /* State of this Rbtree wrt transactions */

  BtRollbackOp *pTransRollback; 
  BtRollbackOp *pCheckRollback;
  BtRollbackOp *pCheckRollbackTail;
};

/*
** Legal values for Rbtree.eTransState.
*/
#define TRANS_NONE           0  /* No transaction is in progress */
#define TRANS_INTRANSACTION  1  /* A transaction is in progress */
#define TRANS_INCHECKPOINT   2  /* A checkpoint is in progress  */
#define TRANS_ROLLBACK       3  /* We are currently rolling back a checkpoint or
                                 * transaction. */

struct RbtCursor {
  BtCursorOps *pOps;        /* Function table */
  Rbtree    *pRbtree;
  BtRbTree *pTree;
  int       iTree;          /* Index of pTree in pRbtree */
  BtRbNode *pNode;
  RbtCursor *pShared;       /* List of all cursors on the same Rbtree */
  u8 eSkip;                 /* Determines if next step operation is a no-op */
  u8 wrFlag;                /* True if this cursor is open for writing */
};

/*
** Legal values for RbtCursor.eSkip.
*/
#define SKIP_NONE     0   /* Always step the cursor */
#define SKIP_NEXT     1   /* The next sqliteRbtreeNext() is a no-op */
#define SKIP_PREV     2   /* The next sqliteRbtreePrevious() is a no-op */
#define SKIP_INVALID  3   /* Calls to Next() and Previous() are invalid */

struct BtRbTree {
  RbtCursor *pCursors;     /* All cursors pointing to this tree */
  BtRbNode *pHead;         /* Head of the tree, or NULL */
};

struct BtRbNode {
  int nKey;
  void *pKey;
  int nData;
  void *pData;
  u8 isBlack;        /* true for a black node, 0 for a red node */
  BtRbNode *pParent; /* Nodes parent node, NULL for the tree head */
  BtRbNode *pLeft;   /* Nodes left child, or NULL */
  BtRbNode *pRight;  /* Nodes right child, or NULL */

  int nBlackHeight;  /* Only used during the red-black integrity check */
};

/* Forward declarations */
static int memRbtreeMoveto(
  RbtCursor* pCur,
  const void *pKey,
  int nKey,
  int *pRes
);
static int memRbtreeClearTable(Rbtree* tree, int n);
static int memRbtreeNext(RbtCursor* pCur, int *pRes);
static int memRbtreeLast(RbtCursor* pCur, int *pRes);
static int memRbtreePrevious(RbtCursor* pCur, int *pRes);


/*
** This routine checks all cursors that point to the same table
** as pCur points to.  If any of those cursors were opened with
** wrFlag==0 then this routine returns SQLITE_LOCKED.  If all
** cursors point to the same table were opened with wrFlag==1
** then this routine returns SQLITE_OK.
**
** In addition to checking for read-locks (where a read-lock 
** means a cursor opened with wrFlag==0) this routine also NULLs
** out the pNode field of all other cursors.
** This is necessary because an insert 
** or delete might change erase the node out from under
** another cursor.
*/
static int checkReadLocks(RbtCursor *pCur){
  RbtCursor *p;
  assert( pCur->wrFlag );
  for(p=pCur->pTree->pCursors; p; p=p->pShared){
    if( p!=pCur ){
      if( p->wrFlag==0 ) return SQLITE_LOCKED;
      p->pNode = 0;
    }
  }
  return SQLITE_OK;
}

/*
 * The key-compare function for the red-black trees. Returns as follows:
 *
 * (key1 < key2)             -1
 * (key1 == key2)             0 
 * (key1 > key2)              1
 *
 * Keys are compared using memcmp(). If one key is an exact prefix of the
 * other, then the shorter key is less than the longer key.
 */
static int key_compare(void const*pKey1, int nKey1, void const*pKey2, int nKey2)
{
  int mcmp = memcmp(pKey1, pKey2, (nKey1 <= nKey2)?nKey1:nKey2);
  if( mcmp == 0){
    if( nKey1 == nKey2 ) return 0;
    return ((nKey1 < nKey2)?-1:1);
  }
  return ((mcmp>0)?1:-1);
}

/*
 * Perform the LEFT-rotate transformation on node X of tree pTree. This
 * transform is part of the red-black balancing code.
 *
 *        |                   |
 *        X                   Y
 *       / \                 / \
 *      a   Y               X   c
 *         / \             / \
 *        b   c           a   b
 *
 *      BEFORE              AFTER
 */
static void leftRotate(BtRbTree *pTree, BtRbNode *pX)
{
  BtRbNode *pY;
  BtRbNode *pb;
  pY = pX->pRight;
  pb = pY->pLeft;

  pY->pParent = pX->pParent;
  if( pX->pParent ){
    if( pX->pParent->pLeft == pX ) pX->pParent->pLeft = pY;
    else pX->pParent->pRight = pY;
  }
  pY->pLeft = pX;
  pX->pParent = pY;
  pX->pRight = pb;
  if( pb ) pb->pParent = pX;
  if( pTree->pHead == pX ) pTree->pHead = pY;
}

/*
 * Perform the RIGHT-rotate transformation on node X of tree pTree. This
 * transform is part of the red-black balancing code.
 *
 *        |                   |
 *        X                   Y
 *       / \                 / \
 *      Y   c               a   X
 *     / \                     / \
 *    a   b                   b   c
 *
 *      BEFORE              AFTER
 */
static void rightRotate(BtRbTree *pTree, BtRbNode *pX)
{
  BtRbNode *pY;
  BtRbNode *pb;
  pY = pX->pLeft;
  pb = pY->pRight;

  pY->pParent = pX->pParent;
  if( pX->pParent ){
    if( pX->pParent->pLeft == pX ) pX->pParent->pLeft = pY;
    else pX->pParent->pRight = pY;
  }
  pY->pRight = pX;
  pX->pParent = pY;
  pX->pLeft = pb;
  if( pb ) pb->pParent = pX;
  if( pTree->pHead == pX ) pTree->pHead = pY;
}

/*
 * A string-manipulation helper function for check_redblack_tree(). If (orig ==
 * NULL) a copy of val is returned. If (orig != NULL) then a copy of the *
 * concatenation of orig and val is returned. The original orig is deleted
 * (using sqliteFree()).
 */
static char *append_val(char * orig, char const * val)
{
  if( !orig ){
    return sqliteStrDup( val );
  } else{
    char * ret = 0;
    sqliteSetString(&ret, orig, val, (char*)0);
    sqliteFree( orig );
    return ret;
  }
  assert(0);
}

/*
 * Append a string representation of the entire node to orig and return it.
 * This is used to produce debugging information if check_redblack_tree() finds
 * a problem with a red-black binary tree.
 */
static char *append_node(char * orig, BtRbNode *pNode, int indent)
{
  char buf[128];
  int i;

  for( i=0; i<indent; i++ ){
      orig = append_val(orig, " ");
  }

  sprintf(buf, "%p", pNode);
  orig = append_val(orig, buf);

  if( pNode ){
    indent += 3;
    if( pNode->isBlack ){
      orig = append_val(orig, " B \n");
    }else{
      orig = append_val(orig, " R \n");
    }
    orig = append_node( orig, pNode->pLeft, indent );
    orig = append_node( orig, pNode->pRight, indent );
  }else{
    orig = append_val(orig, "\n");
  }
  return orig;
}

/*
 * Print a representation of a node to stdout. This function is only included
 * so you can call it from within a debugger if things get really bad.  It
 * is not called from anyplace in the code.
 */
static void print_node(BtRbNode *pNode)
{
    char * str = append_node(0, pNode, 0);
    printf("%s", str);

    /* Suppress a warning message about print_node() being unused */
    (void)print_node;
}

/* 
 * Check the following properties of the red-black tree:
 * (1) - If a node is red, both of it's children are black
 * (2) - Each path from a given node to a leaf (NULL) node passes thru the
 *       same number of black nodes 
 *
 * If there is a problem, append a description (using append_val() ) to *msg.
 */
static void check_redblack_tree(BtRbTree * tree, char ** msg)
{
  BtRbNode *pNode;

  /* 0 -> came from parent 
   * 1 -> came from left
   * 2 -> came from right */
  int prev_step = 0;

  pNode = tree->pHead;
  while( pNode ){
    switch( prev_step ){
      case 0:
        if( pNode->pLeft ){
          pNode = pNode->pLeft;
        }else{ 
          prev_step = 1;
        }
        break;
      case 1:
        if( pNode->pRight ){
          pNode = pNode->pRight;
          prev_step = 0;
        }else{
          prev_step = 2;
        }
        break;
      case 2:
        /* Check red-black property (1) */
        if( !pNode->isBlack &&
            ( (pNode->pLeft && !pNode->pLeft->isBlack) ||
              (pNode->pRight && !pNode->pRight->isBlack) )
          ){
          char buf[128];
          sprintf(buf, "Red node with red child at %p\n", pNode);
          *msg = append_val(*msg, buf);
          *msg = append_node(*msg, tree->pHead, 0);
          *msg = append_val(*msg, "\n");
        }

        /* Check red-black property (2) */
        { 
          int leftHeight = 0;
          int rightHeight = 0;
          if( pNode->pLeft ){
            leftHeight += pNode->pLeft->nBlackHeight;
            leftHeight += (pNode->pLeft->isBlack?1:0);
          }
          if( pNode->pRight ){
            rightHeight += pNode->pRight->nBlackHeight;
            rightHeight += (pNode->pRight->isBlack?1:0);
          }
          if( leftHeight != rightHeight ){
            char buf[128];
            sprintf(buf, "Different black-heights at %p\n", pNode);
            *msg = append_val(*msg, buf);
            *msg = append_node(*msg, tree->pHead, 0);
            *msg = append_val(*msg, "\n");
          }
          pNode->nBlackHeight = leftHeight;
        }

        if( pNode->pParent ){
          if( pNode == pNode->pParent->pLeft ) prev_step = 1;
          else prev_step = 2;
        }
        pNode = pNode->pParent;
        break;
      default: assert(0);
    }
  }
} 

/*
 * Node pX has just been inserted into pTree (by code in sqliteRbtreeInsert()).
 * It is possible that pX is a red node with a red parent, which is a violation
 * of the red-black tree properties. This function performs rotations and 
 * color changes to rebalance the tree
 */
static void do_insert_balancing(BtRbTree *pTree, BtRbNode *pX)
{
  /* In the first iteration of this loop, pX points to the red node just
   * inserted in the tree. If the parent of pX exists (pX is not the root
   * node) and is red, then the properties of the red-black tree are
   * violated.
   *
   * At the start of any subsequent iterations, pX points to a red node
   * with a red parent. In all other respects the tree is a legal red-black
   * binary tree. */
  while( pX != pTree->pHead && !pX->pParent->isBlack ){
    BtRbNode *pUncle;
    BtRbNode *pGrandparent;

    /* Grandparent of pX must exist and must be black. */
    pGrandparent = pX->pParent->pParent;
    assert( pGrandparent );
    assert( pGrandparent->isBlack );

    /* Uncle of pX may or may not exist. */
    if( pX->pParent == pGrandparent->pLeft ) 
      pUncle = pGrandparent->pRight;
    else 
      pUncle = pGrandparent->pLeft;

    /* If the uncle of pX exists and is red, we do the following:
     *       |                 |
     *      G(b)              G(r)
     *      /  \              /  \        
     *   U(r)   P(r)       U(b)  P(b)
     *            \                \
     *           X(r)              X(r)
     *
     *     BEFORE             AFTER
     * pX is then set to G. If the parent of G is red, then the while loop
     * will run again.  */
    if( pUncle && !pUncle->isBlack ){
      pGrandparent->isBlack = 0;
      pUncle->isBlack = 1;
      pX->pParent->isBlack = 1;
      pX = pGrandparent;
    }else{

      if( pX->pParent == pGrandparent->pLeft ){
        if( pX == pX->pParent->pRight ){
          /* If pX is a right-child, do the following transform, essentially
           * to change pX into a left-child: 
           *       |                  | 
           *      G(b)               G(b)
           *      /  \               /  \        
           *   P(r)   U(b)        X(r)  U(b)
           *      \                /
           *     X(r)            P(r) <-- new X
           *
           *     BEFORE             AFTER
           */
          pX = pX->pParent;
          leftRotate(pTree, pX);
        }

        /* Do the following transform, which balances the tree :) 
         *       |                  | 
         *      G(b)               P(b)
         *      /  \               /  \        
         *   P(r)   U(b)        X(r)  G(r)
         *    /                         \
         *  X(r)                        U(b)
         *
         *     BEFORE             AFTER
         */
        assert( pGrandparent == pX->pParent->pParent );
        pGrandparent->isBlack = 0;
        pX->pParent->isBlack = 1;
        rightRotate( pTree, pGrandparent );

      }else{
        /* This code is symetric to the illustrated case above. */
        if( pX == pX->pParent->pLeft ){
          pX = pX->pParent;
          rightRotate(pTree, pX);
        }
        assert( pGrandparent == pX->pParent->pParent );
        pGrandparent->isBlack = 0;
        pX->pParent->isBlack = 1;
        leftRotate( pTree, pGrandparent );
      }
    }
  }
  pTree->pHead->isBlack = 1;
}

/*
 * A child of pParent, which in turn had child pX, has just been removed from 
 * pTree (the figure below depicts the operation, Z is being removed). pParent
 * or pX, or both may be NULL.  
 *                |           |
 *                P           P
 *               / \         / \
 *              Z           X
 *             / \
 *            X  nil
 *
 * This function is only called if Z was black. In this case the red-black tree
 * properties have been violated, and pX has an "extra black". This function 
 * performs rotations and color-changes to re-balance the tree.
 */
static 
void do_delete_balancing(BtRbTree *pTree, BtRbNode *pX, BtRbNode *pParent)
{
  BtRbNode *pSib; 

  /* TODO: Comment this code! */
  while( pX != pTree->pHead && (!pX || pX->isBlack) ){
    if( pX == pParent->pLeft ){
      pSib = pParent->pRight;
      if( pSib && !(pSib->isBlack) ){
        pSib->isBlack = 1;
        pParent->isBlack = 0;
        leftRotate(pTree, pParent);
        pSib = pParent->pRight;
      }
      if( !pSib ){
        pX = pParent;
      }else if( 
          (!pSib->pLeft  || pSib->pLeft->isBlack) &&
          (!pSib->pRight || pSib->pRight->isBlack) ) {
        pSib->isBlack = 0;
        pX = pParent;
      }else{
        if( (!pSib->pRight || pSib->pRight->isBlack) ){
          if( pSib->pLeft ) pSib->pLeft->isBlack = 1;
          pSib->isBlack = 0;
          rightRotate( pTree, pSib );
          pSib = pParent->pRight;
        }
        pSib->isBlack = pParent->isBlack;
        pParent->isBlack = 1;
        if( pSib->pRight ) pSib->pRight->isBlack = 1;
        leftRotate(pTree, pParent);
        pX = pTree->pHead;
      }
    }else{
      pSib = pParent->pLeft;
      if( pSib && !(pSib->isBlack) ){
        pSib->isBlack = 1;
        pParent->isBlack = 0;
        rightRotate(pTree, pParent);
        pSib = pParent->pLeft;
      }
      if( !pSib ){
        pX = pParent;
      }else if( 
          (!pSib->pLeft  || pSib->pLeft->isBlack) &&
          (!pSib->pRight || pSib->pRight->isBlack) ){
        pSib->isBlack = 0;
        pX = pParent;
      }else{
        if( (!pSib->pLeft || pSib->pLeft->isBlack) ){
          if( pSib->pRight ) pSib->pRight->isBlack = 1;
          pSib->isBlack = 0;
          leftRotate( pTree, pSib );
          pSib = pParent->pLeft;
        }
        pSib->isBlack = pParent->isBlack;
        pParent->isBlack = 1;
        if( pSib->pLeft ) pSib->pLeft->isBlack = 1;
        rightRotate(pTree, pParent);
        pX = pTree->pHead;
      }
    }
    pParent = pX->pParent;
  }
  if( pX ) pX->isBlack = 1;
}

/*
 * Create table n in tree pRbtree. Table n must not exist.
 */
static void btreeCreateTable(Rbtree* pRbtree, int n)
{
  BtRbTree *pNewTbl = sqliteMalloc(sizeof(BtRbTree));
  sqliteHashInsert(&pRbtree->tblHash, 0, n, pNewTbl);
}

/*
 * Log a single "rollback-op" for the given Rbtree. See comments for struct
 * BtRollbackOp.
 */
static void btreeLogRollbackOp(Rbtree* pRbtree, BtRollbackOp *pRollbackOp)
{
  assert( pRbtree->eTransState == TRANS_INCHECKPOINT ||
      pRbtree->eTransState == TRANS_INTRANSACTION );
  if( pRbtree->eTransState == TRANS_INTRANSACTION ){
    pRollbackOp->pNext = pRbtree->pTransRollback;
    pRbtree->pTransRollback = pRollbackOp;
  }
  if( pRbtree->eTransState == TRANS_INCHECKPOINT ){
    if( !pRbtree->pCheckRollback ){
      pRbtree->pCheckRollbackTail = pRollbackOp;
    }
    pRollbackOp->pNext = pRbtree->pCheckRollback;
    pRbtree->pCheckRollback = pRollbackOp;
  }
}

int sqliteRbtreeOpen(
  const char *zFilename,
  int mode,
  int nPg,
  Btree **ppBtree
){
  Rbtree **ppRbtree = (Rbtree**)ppBtree;
  *ppRbtree = (Rbtree *)sqliteMalloc(sizeof(Rbtree));
  if( sqlite_malloc_failed ) goto open_no_mem;
  sqliteHashInit(&(*ppRbtree)->tblHash, SQLITE_HASH_INT, 0);

  /* Create a binary tree for the SQLITE_MASTER table at location 2 */
  btreeCreateTable(*ppRbtree, 2);
  if( sqlite_malloc_failed ) goto open_no_mem;
  (*ppRbtree)->next_idx = 3;
  (*ppRbtree)->pOps = &sqliteRbtreeOps;
  /* Set file type to 4; this is so that "attach ':memory:' as ...."  does not
  ** think that the database in uninitialised and refuse to attach
  */
  (*ppRbtree)->aMetaData[2] = 4;
  
  return SQLITE_OK;

open_no_mem:
  *ppBtree = 0;
  return SQLITE_NOMEM;
}

/*
 * Create a new table in the supplied Rbtree. Set *n to the new table number.
 * Return SQLITE_OK if the operation is a success.
 */
static int memRbtreeCreateTable(Rbtree* tree, int* n)
{
  assert( tree->eTransState != TRANS_NONE );

  *n = tree->next_idx++;
  btreeCreateTable(tree, *n);
  if( sqlite_malloc_failed ) return SQLITE_NOMEM;

  /* Set up the rollback structure (if we are not doing this as part of a
   * rollback) */
  if( tree->eTransState != TRANS_ROLLBACK ){
    BtRollbackOp *pRollbackOp = sqliteMalloc(sizeof(BtRollbackOp));
    if( pRollbackOp==0 ) return SQLITE_NOMEM;
    pRollbackOp->eOp = ROLLBACK_DROP;
    pRollbackOp->iTab = *n;
    btreeLogRollbackOp(tree, pRollbackOp);
  }

  return SQLITE_OK;
}

/*
 * Delete table n from the supplied Rbtree. 
 */
static int memRbtreeDropTable(Rbtree* tree, int n)
{
  BtRbTree *pTree;
  assert( tree->eTransState != TRANS_NONE );

  memRbtreeClearTable(tree, n);
  pTree = sqliteHashInsert(&tree->tblHash, 0, n, 0);
  assert(pTree);
  assert( pTree->pCursors==0 );
  sqliteFree(pTree);

  if( tree->eTransState != TRANS_ROLLBACK ){
    BtRollbackOp *pRollbackOp = sqliteMalloc(sizeof(BtRollbackOp));
    if( pRollbackOp==0 ) return SQLITE_NOMEM;
    pRollbackOp->eOp = ROLLBACK_CREATE;
    pRollbackOp->iTab = n;
    btreeLogRollbackOp(tree, pRollbackOp);
  }

  return SQLITE_OK;
}

static int memRbtreeKeyCompare(RbtCursor* pCur, const void *pKey, int nKey,
                                 int nIgnore, int *pRes)
{
  assert(pCur);

  if( !pCur->pNode ) {
    *pRes = -1;
  } else {
    if( (pCur->pNode->nKey - nIgnore) < 0 ){
      *pRes = -1;
    }else{
      *pRes = key_compare(pCur->pNode->pKey, pCur->pNode->nKey-nIgnore, 
          pKey, nKey);
    }
  }
  return SQLITE_OK;
}

/*
 * Get a new cursor for table iTable of the supplied Rbtree. The wrFlag
 * parameter indicates that the cursor is open for writing.
 *
 * Note that RbtCursor.eSkip and RbtCursor.pNode both initialize to 0.
 */
static int memRbtreeCursor(
  Rbtree* tree,
  int iTable,
  int wrFlag,
  RbtCursor **ppCur
){
  RbtCursor *pCur;
  assert(tree);
  pCur = *ppCur = sqliteMalloc(sizeof(RbtCursor));
  if( sqlite_malloc_failed ) return SQLITE_NOMEM;
  pCur->pTree  = sqliteHashFind(&tree->tblHash, 0, iTable);
  pCur->pRbtree = tree;
  pCur->iTree  = iTable;
  pCur->pOps = &sqliteRbtreeCursorOps;
  pCur->wrFlag = wrFlag;
  pCur->pShared = pCur->pTree->pCursors;
  pCur->pTree->pCursors = pCur;
  

  assert( (*ppCur)->pTree );
  return SQLITE_OK;
}

/*
 * Insert a new record into the Rbtree.  The key is given by (pKey,nKey)
 * and the data is given by (pData,nData).  The cursor is used only to
 * define what database the record should be inserted into.  The cursor
 * is left pointing at the new record.
 *
 * If the key exists already in the tree, just replace the data. 
 */
static int memRbtreeInsert(
  RbtCursor* pCur,
  const void *pKey,
  int nKey,
  const void *pDataInput,
  int nData
){
  void * pData;
  int match;

  /* It is illegal to call sqliteRbtreeInsert() if we are
  ** not in a transaction */
  assert( pCur->pRbtree->eTransState != TRANS_NONE );

  /* Make sure some other cursor isn't trying to read this same table */
  if( checkReadLocks(pCur) ){
    return SQLITE_LOCKED; /* The table pCur points to has a read lock */
  }

  /* Take a copy of the input data now, in case we need it for the 
   * replace case */
  pData = sqliteMallocRaw(nData);
  if( sqlite_malloc_failed ) return SQLITE_NOMEM;
  memcpy(pData, pDataInput, nData);

  /* Move the cursor to a node near the key to be inserted. If the key already
   * exists in the table, then (match == 0). In this case we can just replace
   * the data associated with the entry, we don't need to manipulate the tree.
   * 
   * If there is no exact match, then the cursor points at what would be either
   * the predecessor (match == -1) or successor (match == 1) of the
   * searched-for key, were it to be inserted. The new node becomes a child of
   * this node.
   * 
   * The new node is initially red.
   */
  memRbtreeMoveto( pCur, pKey, nKey, &match);
  if( match ){
    BtRbNode *pNode = sqliteMalloc(sizeof(BtRbNode));
    if( pNode==0 ) return SQLITE_NOMEM;
    pNode->nKey = nKey;
    pNode->pKey = sqliteMallocRaw(nKey);
    if( sqlite_malloc_failed ) return SQLITE_NOMEM;
    memcpy(pNode->pKey, pKey, nKey);
    pNode->nData = nData;
    pNode->pData = pData; 
    if( pCur->pNode ){
      switch( match ){
        case -1:
          assert( !pCur->pNode->pRight );
          pNode->pParent = pCur->pNode;
          pCur->pNode->pRight = pNode;
          break;
        case 1:
          assert( !pCur->pNode->pLeft );
          pNode->pParent = pCur->pNode;
          pCur->pNode->pLeft = pNode;
          break;
        default:
          assert(0);
      }
    }else{
      pCur->pTree->pHead = pNode;
    }

    /* Point the cursor at the node just inserted, as per SQLite requirements */
    pCur->pNode = pNode;

    /* A new node has just been inserted, so run the balancing code */
    do_insert_balancing(pCur->pTree, pNode);

    /* Set up a rollback-op in case we have to roll this operation back */
    if( pCur->pRbtree->eTransState != TRANS_ROLLBACK ){
      BtRollbackOp *pOp = sqliteMalloc( sizeof(BtRollbackOp) );
      if( pOp==0 ) return SQLITE_NOMEM;
      pOp->eOp = ROLLBACK_DELETE;
      pOp->iTab = pCur->iTree;
      pOp->nKey = pNode->nKey;
      pOp->pKey = sqliteMallocRaw( pOp->nKey );
      if( sqlite_malloc_failed ) return SQLITE_NOMEM;
      memcpy( pOp->pKey, pNode->pKey, pOp->nKey );
      btreeLogRollbackOp(pCur->pRbtree, pOp);
    }

  }else{ 
    /* No need to insert a new node in the tree, as the key already exists.
     * Just clobber the current nodes data. */

    /* Set up a rollback-op in case we have to roll this operation back */
    if( pCur->pRbtree->eTransState != TRANS_ROLLBACK ){
      BtRollbackOp *pOp = sqliteMalloc( sizeof(BtRollbackOp) );
      if( pOp==0 ) return SQLITE_NOMEM;
      pOp->iTab = pCur->iTree;
      pOp->nKey = pCur->pNode->nKey;
      pOp->pKey = sqliteMallocRaw( pOp->nKey );
      if( sqlite_malloc_failed ) return SQLITE_NOMEM;
      memcpy( pOp->pKey, pCur->pNode->pKey, pOp->nKey );
      pOp->nData = pCur->pNode->nData;
      pOp->pData = pCur->pNode->pData;
      pOp->eOp = ROLLBACK_INSERT;
      btreeLogRollbackOp(pCur->pRbtree, pOp);
    }else{
      sqliteFree( pCur->pNode->pData );
    }

    /* Actually clobber the nodes data */
    pCur->pNode->pData = pData;
    pCur->pNode->nData = nData;
  }

  return SQLITE_OK;
}

/* Move the cursor so that it points to an entry near pKey.
** Return a success code.
**
**     *pRes<0      The cursor is left pointing at an entry that
**                  is smaller than pKey or if the table is empty
**                  and the cursor is therefore left point to nothing.
**
**     *pRes==0     The cursor is left pointing at an entry that
**                  exactly matches pKey.
**
**     *pRes>0      The cursor is left pointing at an entry that
**                  is larger than pKey.
*/
static int memRbtreeMoveto(
  RbtCursor* pCur,
  const void *pKey,
  int nKey,
  int *pRes
){
  BtRbNode *pTmp = 0;

  pCur->pNode = pCur->pTree->pHead;
  *pRes = -1;
  while( pCur->pNode && *pRes ) {
    *pRes = key_compare(pCur->pNode->pKey, pCur->pNode->nKey, pKey, nKey);
    pTmp = pCur->pNode;
    switch( *pRes ){
      case 1:    /* cursor > key */
        pCur->pNode = pCur->pNode->pLeft;
        break;
      case -1:   /* cursor < key */
        pCur->pNode = pCur->pNode->pRight;
        break;
    }
  } 

  /* If (pCur->pNode == NULL), then we have failed to find a match. Set
   * pCur->pNode to pTmp, which is either NULL (if the tree is empty) or the
   * last node traversed in the search. In either case the relation ship
   * between pTmp and the searched for key is already stored in *pRes. pTmp is
   * either the successor or predecessor of the key we tried to move to. */
  if( !pCur->pNode ) pCur->pNode = pTmp;
  pCur->eSkip = SKIP_NONE;

  return SQLITE_OK;
}


/*
** Delete the entry that the cursor is pointing to.
**
** The cursor is left pointing at either the next or the previous
** entry.  If the cursor is left pointing to the next entry, then 
** the pCur->eSkip flag is set to SKIP_NEXT which forces the next call to 
** sqliteRbtreeNext() to be a no-op.  That way, you can always call
** sqliteRbtreeNext() after a delete and the cursor will be left
** pointing to the first entry after the deleted entry.  Similarly,
** pCur->eSkip is set to SKIP_PREV is the cursor is left pointing to
** the entry prior to the deleted entry so that a subsequent call to
** sqliteRbtreePrevious() will always leave the cursor pointing at the
** entry immediately before the one that was deleted.
*/
static int memRbtreeDelete(RbtCursor* pCur)
{
  BtRbNode *pZ;      /* The one being deleted */
  BtRbNode *pChild;  /* The child of the spliced out node */

  /* It is illegal to call sqliteRbtreeDelete() if we are
  ** not in a transaction */
  assert( pCur->pRbtree->eTransState != TRANS_NONE );

  /* Make sure some other cursor isn't trying to read this same table */
  if( checkReadLocks(pCur) ){
    return SQLITE_LOCKED; /* The table pCur points to has a read lock */
  }

  pZ = pCur->pNode;
  if( !pZ ){
    return SQLITE_OK;
  }

  /* If we are not currently doing a rollback, set up a rollback op for this 
   * deletion */
  if( pCur->pRbtree->eTransState != TRANS_ROLLBACK ){
    BtRollbackOp *pOp = sqliteMalloc( sizeof(BtRollbackOp) );
    if( pOp==0 ) return SQLITE_NOMEM;
    pOp->iTab = pCur->iTree;
    pOp->nKey = pZ->nKey;
    pOp->pKey = pZ->pKey;
    pOp->nData = pZ->nData;
    pOp->pData = pZ->pData;
    pOp->eOp = ROLLBACK_INSERT;
    btreeLogRollbackOp(pCur->pRbtree, pOp);
  }

  /* First do a standard binary-tree delete (node pZ is to be deleted). How
   * to do this depends on how many children pZ has:
   *
   * If pZ has no children or one child, then splice out pZ.  If pZ has two
   * children, splice out the successor of pZ and replace the key and data of
   * pZ with the key and data of the spliced out successor.  */
  if( pZ->pLeft && pZ->pRight ){
    BtRbNode *pTmp;
    int dummy;
    pCur->eSkip = SKIP_NONE;
    memRbtreeNext(pCur, &dummy);
    assert( dummy == 0 );
    if( pCur->pRbtree->eTransState == TRANS_ROLLBACK ){
      sqliteFree(pZ->pKey);
      sqliteFree(pZ->pData);
    }
    pZ->pData = pCur->pNode->pData;
    pZ->nData = pCur->pNode->nData;
    pZ->pKey = pCur->pNode->pKey;
    pZ->nKey = pCur->pNode->nKey;
    pTmp = pZ;
    pZ = pCur->pNode;
    pCur->pNode = pTmp;
    pCur->eSkip = SKIP_NEXT;
  }else{
    int res;
    pCur->eSkip = SKIP_NONE;
    memRbtreeNext(pCur, &res);
    pCur->eSkip = SKIP_NEXT;
    if( res ){
      memRbtreeLast(pCur, &res);
      memRbtreePrevious(pCur, &res);
      pCur->eSkip = SKIP_PREV;
    }
    if( pCur->pRbtree->eTransState == TRANS_ROLLBACK ){
        sqliteFree(pZ->pKey);
        sqliteFree(pZ->pData);
    }
  }

  /* pZ now points at the node to be spliced out. This block does the 
   * splicing. */
  {
    BtRbNode **ppParentSlot = 0;
    assert( !pZ->pLeft || !pZ->pRight ); /* pZ has at most one child */
    pChild = ((pZ->pLeft)?pZ->pLeft:pZ->pRight);
    if( pZ->pParent ){
      assert( pZ == pZ->pParent->pLeft || pZ == pZ->pParent->pRight );
      ppParentSlot = ((pZ == pZ->pParent->pLeft)
          ?&pZ->pParent->pLeft:&pZ->pParent->pRight);
      *ppParentSlot = pChild;
    }else{
      pCur->pTree->pHead = pChild;
    }
    if( pChild ) pChild->pParent = pZ->pParent;
  }

  /* pZ now points at the spliced out node. pChild is the only child of pZ, or
   * NULL if pZ has no children. If pZ is black, and not the tree root, then we
   * will have violated the "same number of black nodes in every path to a
   * leaf" property of the red-black tree. The code in do_delete_balancing()
   * repairs this. */
  if( pZ->isBlack ){ 
    do_delete_balancing(pCur->pTree, pChild, pZ->pParent);
  }

  sqliteFree(pZ);
  return SQLITE_OK;
}

/*
 * Empty table n of the Rbtree.
 */
static int memRbtreeClearTable(Rbtree* tree, int n)
{
  BtRbTree *pTree;
  BtRbNode *pNode;

  pTree = sqliteHashFind(&tree->tblHash, 0, n);
  assert(pTree);

  pNode = pTree->pHead;
  while( pNode ){
    if( pNode->pLeft ){
      pNode = pNode->pLeft;
    }
    else if( pNode->pRight ){
      pNode = pNode->pRight;
    }
    else {
      BtRbNode *pTmp = pNode->pParent;
      if( tree->eTransState == TRANS_ROLLBACK ){
        sqliteFree( pNode->pKey );
        sqliteFree( pNode->pData );
      }else{
        BtRollbackOp *pRollbackOp = sqliteMallocRaw(sizeof(BtRollbackOp));
        if( pRollbackOp==0 ) return SQLITE_NOMEM;
        pRollbackOp->eOp = ROLLBACK_INSERT;
        pRollbackOp->iTab = n;
        pRollbackOp->nKey = pNode->nKey;
        pRollbackOp->pKey = pNode->pKey;
        pRollbackOp->nData = pNode->nData;
        pRollbackOp->pData = pNode->pData;
        btreeLogRollbackOp(tree, pRollbackOp);
      }
      sqliteFree( pNode );
      if( pTmp ){
        if( pTmp->pLeft == pNode ) pTmp->pLeft = 0;
        else if( pTmp->pRight == pNode ) pTmp->pRight = 0;
      }
      pNode = pTmp;
    }
  }

  pTree->pHead = 0;
  return SQLITE_OK;
}

static int memRbtreeFirst(RbtCursor* pCur, int *pRes)
{
  if( pCur->pTree->pHead ){
    pCur->pNode = pCur->pTree->pHead;
    while( pCur->pNode->pLeft ){
      pCur->pNode = pCur->pNode->pLeft;
    }
  }
  if( pCur->pNode ){
    *pRes = 0;
  }else{
    *pRes = 1;
  }
  pCur->eSkip = SKIP_NONE;
  return SQLITE_OK;
}

static int memRbtreeLast(RbtCursor* pCur, int *pRes)
{
  if( pCur->pTree->pHead ){
    pCur->pNode = pCur->pTree->pHead;
    while( pCur->pNode->pRight ){
      pCur->pNode = pCur->pNode->pRight;
    }
  }
  if( pCur->pNode ){
    *pRes = 0;
  }else{
    *pRes = 1;
  }
  pCur->eSkip = SKIP_NONE;
  return SQLITE_OK;
}

/*
** Advance the cursor to the next entry in the database.  If
** successful then set *pRes=0.  If the cursor
** was already pointing to the last entry in the database before
** this routine was called, then set *pRes=1.
*/
static int memRbtreeNext(RbtCursor* pCur, int *pRes)
{
  if( pCur->pNode && pCur->eSkip != SKIP_NEXT ){
    if( pCur->pNode->pRight ){
      pCur->pNode = pCur->pNode->pRight;
      while( pCur->pNode->pLeft )
        pCur->pNode = pCur->pNode->pLeft;
    }else{
      BtRbNode * pX = pCur->pNode;
      pCur->pNode = pX->pParent;
      while( pCur->pNode && (pCur->pNode->pRight == pX) ){
        pX = pCur->pNode;
        pCur->pNode = pX->pParent;
      }
    }
  }
  pCur->eSkip = SKIP_NONE;

  if( !pCur->pNode ){
    *pRes = 1;
  }else{
    *pRes = 0;
  }

  return SQLITE_OK;
}

static int memRbtreePrevious(RbtCursor* pCur, int *pRes)
{
  if( pCur->pNode && pCur->eSkip != SKIP_PREV ){
    if( pCur->pNode->pLeft ){
      pCur->pNode = pCur->pNode->pLeft;
      while( pCur->pNode->pRight )
        pCur->pNode = pCur->pNode->pRight;
    }else{
      BtRbNode * pX = pCur->pNode;
      pCur->pNode = pX->pParent;
      while( pCur->pNode && (pCur->pNode->pLeft == pX) ){
        pX = pCur->pNode;
        pCur->pNode = pX->pParent;
      }
    }
  }
  pCur->eSkip = SKIP_NONE;

  if( !pCur->pNode ){
    *pRes = 1;
  }else{
    *pRes = 0;
  }

  return SQLITE_OK;
}

static int memRbtreeKeySize(RbtCursor* pCur, int *pSize)
{
  if( pCur->pNode ){
    *pSize = pCur->pNode->nKey;
  }else{
    *pSize = 0;
  }
  return SQLITE_OK;
}

static int memRbtreeKey(RbtCursor* pCur, int offset, int amt, char *zBuf)
{
  if( !pCur->pNode ) return 0;
  if( !pCur->pNode->pKey || ((amt + offset) <= pCur->pNode->nKey) ){
    memcpy(zBuf, ((char*)pCur->pNode->pKey)+offset, amt);
    return amt;
  }else{
    memcpy(zBuf, ((char*)pCur->pNode->pKey)+offset, pCur->pNode->nKey-offset);
    return pCur->pNode->nKey-offset;
  }
  assert(0);
}

static int memRbtreeDataSize(RbtCursor* pCur, int *pSize)
{
  if( pCur->pNode ){
    *pSize = pCur->pNode->nData;
  }else{
    *pSize = 0;
  }
  return SQLITE_OK;
}

static int memRbtreeData(RbtCursor *pCur, int offset, int amt, char *zBuf)
{
  if( !pCur->pNode ) return 0;
  if( (amt + offset) <= pCur->pNode->nData ){
    memcpy(zBuf, ((char*)pCur->pNode->pData)+offset, amt);
    return amt;
  }else{
    memcpy(zBuf, ((char*)pCur->pNode->pData)+offset ,pCur->pNode->nData-offset);
    return pCur->pNode->nData-offset;
  }
  assert(0);
}

static int memRbtreeCloseCursor(RbtCursor* pCur)
{
  if( pCur->pTree->pCursors==pCur ){
    pCur->pTree->pCursors = pCur->pShared;
  }else{
    RbtCursor *p = pCur->pTree->pCursors;
    while( p && p->pShared!=pCur ){ p = p->pShared; }
    assert( p!=0 );
    if( p ){
      p->pShared = pCur->pShared;
    }
  }
  sqliteFree(pCur);
  return SQLITE_OK;
}

static int memRbtreeGetMeta(Rbtree* tree, int* aMeta)
{
  memcpy( aMeta, tree->aMetaData, sizeof(int) * SQLITE_N_BTREE_META );
  return SQLITE_OK;
}

static int memRbtreeUpdateMeta(Rbtree* tree, int* aMeta)
{
  memcpy( tree->aMetaData, aMeta, sizeof(int) * SQLITE_N_BTREE_META );
  return SQLITE_OK;
}

/*
 * Check that each table in the Rbtree meets the requirements for a red-black
 * binary tree. If an error is found, return an explanation of the problem in 
 * memory obtained from sqliteMalloc(). Parameters aRoot and nRoot are ignored. 
 */
static char *memRbtreeIntegrityCheck(Rbtree* tree, int* aRoot, int nRoot)
{
  char * msg = 0;
  HashElem *p;

  for(p=sqliteHashFirst(&tree->tblHash); p; p=sqliteHashNext(p)){
    BtRbTree *pTree = sqliteHashData(p);
    check_redblack_tree(pTree, &msg);
  }

  return msg;
}

static int memRbtreeSetCacheSize(Rbtree* tree, int sz)
{
  return SQLITE_OK;
}

static int memRbtreeSetSafetyLevel(Rbtree *pBt, int level){
  return SQLITE_OK;
}

static int memRbtreeBeginTrans(Rbtree* tree)
{
  if( tree->eTransState != TRANS_NONE ) 
    return SQLITE_ERROR;

  assert( tree->pTransRollback == 0 );
  tree->eTransState = TRANS_INTRANSACTION;
  return SQLITE_OK;
}

/*
** Delete a linked list of BtRollbackOp structures.
*/
static void deleteRollbackList(BtRollbackOp *pOp){
  while( pOp ){
    BtRollbackOp *pTmp = pOp->pNext;
    sqliteFree(pOp->pData);
    sqliteFree(pOp->pKey);
    sqliteFree(pOp);
    pOp = pTmp;
  }
}

static int memRbtreeCommit(Rbtree* tree){
  /* Just delete pTransRollback and pCheckRollback */
  deleteRollbackList(tree->pCheckRollback);
  deleteRollbackList(tree->pTransRollback);
  tree->pTransRollback = 0;
  tree->pCheckRollback = 0;
  tree->pCheckRollbackTail = 0;
  tree->eTransState = TRANS_NONE;
  return SQLITE_OK;
}

/*
 * Close the supplied Rbtree. Delete everything associated with it.
 */
static int memRbtreeClose(Rbtree* tree)
{
  HashElem *p;
  memRbtreeCommit(tree);
  while( (p=sqliteHashFirst(&tree->tblHash))!=0 ){
    tree->eTransState = TRANS_ROLLBACK;
    memRbtreeDropTable(tree, sqliteHashKeysize(p));
  }
  sqliteHashClear(&tree->tblHash);
  sqliteFree(tree);
  return SQLITE_OK;
}

/*
 * Execute and delete the supplied rollback-list on pRbtree.
 */
static void execute_rollback_list(Rbtree *pRbtree, BtRollbackOp *pList)
{
  BtRollbackOp *pTmp;
  RbtCursor cur;
  int res;

  cur.pRbtree = pRbtree;
  cur.wrFlag = 1;
  while( pList ){
    switch( pList->eOp ){
      case ROLLBACK_INSERT:
        cur.pTree  = sqliteHashFind( &pRbtree->tblHash, 0, pList->iTab );
        assert(cur.pTree);
        cur.iTree  = pList->iTab;
        cur.eSkip  = SKIP_NONE;
        memRbtreeInsert( &cur, pList->pKey,
            pList->nKey, pList->pData, pList->nData );
        break;
      case ROLLBACK_DELETE:
        cur.pTree  = sqliteHashFind( &pRbtree->tblHash, 0, pList->iTab );
        assert(cur.pTree);
        cur.iTree  = pList->iTab;
        cur.eSkip  = SKIP_NONE;
        memRbtreeMoveto(&cur, pList->pKey, pList->nKey, &res);
        assert(res == 0);
        memRbtreeDelete( &cur );
        break;
      case ROLLBACK_CREATE:
        btreeCreateTable(pRbtree, pList->iTab);
        break;
      case ROLLBACK_DROP:
        memRbtreeDropTable(pRbtree, pList->iTab);
        break;
      default:
        assert(0);
    }
    sqliteFree(pList->pKey);
    sqliteFree(pList->pData);
    pTmp = pList->pNext;
    sqliteFree(pList);
    pList = pTmp;
  }
}

static int memRbtreeRollback(Rbtree* tree)
{
  tree->eTransState = TRANS_ROLLBACK;
  execute_rollback_list(tree, tree->pCheckRollback);
  execute_rollback_list(tree, tree->pTransRollback);
  tree->pTransRollback = 0;
  tree->pCheckRollback = 0;
  tree->pCheckRollbackTail = 0;
  tree->eTransState = TRANS_NONE;
  return SQLITE_OK;
}

static int memRbtreeBeginCkpt(Rbtree* tree)
{
  if( tree->eTransState != TRANS_INTRANSACTION ) 
    return SQLITE_ERROR;

  assert( tree->pCheckRollback == 0 );
  assert( tree->pCheckRollbackTail == 0 );
  tree->eTransState = TRANS_INCHECKPOINT;
  return SQLITE_OK;
}

static int memRbtreeCommitCkpt(Rbtree* tree)
{
  if( tree->eTransState == TRANS_INCHECKPOINT ){ 
    if( tree->pCheckRollback ){
      tree->pCheckRollbackTail->pNext = tree->pTransRollback;
      tree->pTransRollback = tree->pCheckRollback;
      tree->pCheckRollback = 0;
      tree->pCheckRollbackTail = 0;
    }
    tree->eTransState = TRANS_INTRANSACTION;
  }
  return SQLITE_OK;
}

static int memRbtreeRollbackCkpt(Rbtree* tree)
{
  if( tree->eTransState != TRANS_INCHECKPOINT ) return SQLITE_OK;
  tree->eTransState = TRANS_ROLLBACK;
  execute_rollback_list(tree, tree->pCheckRollback);
  tree->pCheckRollback = 0;
  tree->pCheckRollbackTail = 0;
  tree->eTransState = TRANS_INTRANSACTION;
  return SQLITE_OK;
}

#ifdef SQLITE_TEST
static int memRbtreePageDump(Rbtree* tree, int pgno, int rec)
{
  assert(!"Cannot call sqliteRbtreePageDump");
  return SQLITE_OK;
}

static int memRbtreeCursorDump(RbtCursor* pCur, int* aRes)
{
  assert(!"Cannot call sqliteRbtreeCursorDump");
  return SQLITE_OK;
}

static struct Pager *memRbtreePager(Rbtree* tree)
{
  assert(!"Cannot call sqliteRbtreePager");
  return SQLITE_OK;
}
#endif

/*
** Return the full pathname of the underlying database file.
*/
static const char *memRbtreeGetFilename(Rbtree *pBt){
  return 0;  /* A NULL return indicates there is no underlying file */
}

/*
** The copy file function is not implemented for the in-memory database
*/
static int memRbtreeCopyFile(Rbtree *pBt, Rbtree *pBt2){
  return SQLITE_INTERNAL;  /* Not implemented */
}

static BtOps sqliteRbtreeOps = {
    (int(*)(Btree*)) memRbtreeClose,
    (int(*)(Btree*,int)) memRbtreeSetCacheSize,
    (int(*)(Btree*,int)) memRbtreeSetSafetyLevel,
    (int(*)(Btree*)) memRbtreeBeginTrans,
    (int(*)(Btree*)) memRbtreeCommit,
    (int(*)(Btree*)) memRbtreeRollback,
    (int(*)(Btree*)) memRbtreeBeginCkpt,
    (int(*)(Btree*)) memRbtreeCommitCkpt,
    (int(*)(Btree*)) memRbtreeRollbackCkpt,
    (int(*)(Btree*,int*)) memRbtreeCreateTable,
    (int(*)(Btree*,int*)) memRbtreeCreateTable,
    (int(*)(Btree*,int)) memRbtreeDropTable,
    (int(*)(Btree*,int)) memRbtreeClearTable,
    (int(*)(Btree*,int,int,BtCursor**)) memRbtreeCursor,
    (int(*)(Btree*,int*)) memRbtreeGetMeta,
    (int(*)(Btree*,int*)) memRbtreeUpdateMeta,
    (char*(*)(Btree*,int*,int)) memRbtreeIntegrityCheck,
    (const char*(*)(Btree*)) memRbtreeGetFilename,
    (int(*)(Btree*,Btree*)) memRbtreeCopyFile,

#ifdef SQLITE_TEST
    (int(*)(Btree*,int,int)) memRbtreePageDump,
    (struct Pager*(*)(Btree*)) memRbtreePager
#endif
};

static BtCursorOps sqliteRbtreeCursorOps = {
    (int(*)(BtCursor*,const void*,int,int*)) memRbtreeMoveto,
    (int(*)(BtCursor*)) memRbtreeDelete,
    (int(*)(BtCursor*,const void*,int,const void*,int)) memRbtreeInsert,
    (int(*)(BtCursor*,int*)) memRbtreeFirst,
    (int(*)(BtCursor*,int*)) memRbtreeLast,
    (int(*)(BtCursor*,int*)) memRbtreeNext,
    (int(*)(BtCursor*,int*)) memRbtreePrevious,
    (int(*)(BtCursor*,int*)) memRbtreeKeySize,
    (int(*)(BtCursor*,int,int,char*)) memRbtreeKey,
    (int(*)(BtCursor*,const void*,int,int,int*)) memRbtreeKeyCompare,
    (int(*)(BtCursor*,int*)) memRbtreeDataSize,
    (int(*)(BtCursor*,int,int,char*)) memRbtreeData,
    (int(*)(BtCursor*)) memRbtreeCloseCursor,
#ifdef SQLITE_TEST
    (int(*)(BtCursor*,int*)) memRbtreeCursorDump,
#endif

};

#endif /* SQLITE_OMIT_INMEMORYDB */
