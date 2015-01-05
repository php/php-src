/* 
 * Date last modified: Jan 2001
 * Modifications by Dan Libby (dan@libby.com), including:
 *  - various fixes, null checks, etc
 *  - addition of Q_Iter funcs, macros
 */

/*
 *  File : q.h
 *
 *  Peter Yard  02 Jan 1993.
 *
 *  Disclaimer: This code is released to the public domain.
 */

#ifndef Q__H
#define Q__H

#ifndef False_
   #define False_ 0
#endif

#ifndef True_
   #define True_ 1
#endif

typedef struct nodeptr datanode;

typedef struct nodeptr {
   void        *data ;
   datanode    *prev, *next ;
} node ;

/* For external use with Q_Iter* funcs */
typedef struct nodeptr* q_iter;

typedef struct {
   node        *head, *tail, *cursor;
   int         size, sorted, item_deleted;
} queue;

typedef  struct {
   void        *dataptr;
   node        *loc ;
} index_elt ;


int    Q_Init(queue  *q);
void   Q_Destroy(queue *q);
int    Q_IsEmpty(queue *q);
int    Q_Size(queue *q);
int    Q_AtHead(queue *q);
int    Q_AtTail(queue *q);
int    Q_PushHead(queue *q, void *d);
int    Q_PushTail(queue *q, void *d);
void  *Q_Head(queue *q);
void  *Q_Tail(queue *q);
void  *Q_PopHead(queue *q);
void  *Q_PopTail(queue *q);
void  *Q_Next(queue *q);
void  *Q_Previous(queue *q);
void  *Q_DelCur(queue *q);
void  *Q_Get(queue *q);
int    Q_Put(queue *q, void *data);
int    Q_Sort(queue *q, int (*Comp)(const void *, const void *));
int    Q_Find(queue *q, void *data,
              int (*Comp)(const void *, const void *));
void  *Q_Seek(queue *q, void *data,
              int (*Comp)(const void *, const void *));
int    Q_Insert(queue *q, void *data,
                int (*Comp)(const void *, const void *));

/* read only funcs for iterating through queue. above funcs modify queue */
q_iter Q_Iter_Head(queue *q);
q_iter Q_Iter_Tail(queue *q);
q_iter Q_Iter_Next(q_iter qi);
q_iter Q_Iter_Prev(q_iter qi);
void*  Q_Iter_Get(q_iter qi);
int    Q_Iter_Put(q_iter qi, void* data); /* not read only! here for completeness. */
void*  Q_Iter_Del(queue *q, q_iter iter); /* not read only! here for completeness. */

/* Fast (macro'd) versions of above */
#define Q_Iter_Head_F(q) (q ? (q_iter)((queue*)q)->head : NULL)
#define Q_Iter_Tail_F(q) (q ? (q_iter)((queue*)q)->tail : NULL)
#define Q_Iter_Next_F(qi) (qi ? (q_iter)((node*)qi)->next : NULL)
#define Q_Iter_Prev_F(qi) (qi ? (q_iter)((node*)qi)->prev : NULL)
#define Q_Iter_Get_F(qi) (qi ? ((node*)qi)->data : NULL)

#endif /* Q__H */
