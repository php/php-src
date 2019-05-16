/*
 * Date last modified: Jan 2001
 * Modifications by Dan Libby (dan@libby.com), including:
 *  - various fixes, null checks, etc
 *  - addition of Q_Iter funcs, macros
 */


/*-**************************************************************
 *
 *  File : q.c
 *
 *  Author: Peter Yard [1993.01.02] -- 02 Jan 1993
 *
 *  Disclaimer: This code is released to the public domain.
 *
 *  Description:
 *      Generic double ended queue (Deque pronounced DEK) for handling
 *      any data types, with sorting.
 *
 *      By use of various functions in this module the caller
 *      can create stacks, queues, lists, doubly linked lists,
 *      sorted lists, indexed lists.  All lists are dynamic.
 *
 *      It is the responsibility of the caller to malloc and free
 *      memory for insertion into the queue. A pointer to the object
 *      is used so that not only can any data be used but various kinds
 *      of data can be pushed on the same queue if one so wished e.g.
 *      various length string literals mixed with pointers to structures
 *      or integers etc.
 *
 *  Enhancements:
 *      A future improvement would be the option of multiple "cursors"
 *      so that multiple locations could occur in the one queue to allow
 *      placemarkers and additional flexibility.  Perhaps even use queue
 *      itself to have a list of cursors.
 *
 * Usage:
 *
 *          /x init queue x/
 *          queue  q;
 *          Q_Init(&q);
 *
 *      To create a stack :
 *
 *          Q_PushHead(&q, &mydata1); /x push x/
 *          Q_PushHead(&q, &mydata2);
 *          .....
 *          data_ptr = Q_PopHead(&q); /x pop x/
 *          .....
 *          data_ptr = Q_Head(&q);   /x top of stack x/
 *
 *      To create a FIFO:
 *
 *          Q_PushHead(&q, &mydata1);
 *          .....
 *          data_ptr = Q_PopTail(&q);
 *
 *      To create a double list:
 *
 *          data_ptr = Q_Head(&q);
 *          ....
 *          data_ptr = Q_Next(&q);
 *          data_ptr = Q_Tail(&q);
 *          if (Q_IsEmpty(&q)) ....
 *          .....
 *          data_ptr = Q_Previous(&q);
 *
 *      To create a sorted list:
 *
 *          Q_PushHead(&q, &mydata1); /x push x/
 *          Q_PushHead(&q, &mydata2);
 *          .....
 *          if (!Q_Sort(&q, MyFunction))
 *              .. error ..
 *
 *          /x fill in key field of mydata1.
 *           * NB: Q_Find does linear search
 *           x/
 *
 *          if (Q_Find(&q, &mydata1, MyFunction))
 *          {
 *              /x found it, queue cursor now at correct record x/
 *              /x can retrieve with x/
 *              data_ptr = Q_Get(&q);
 *
 *              /x alter data , write back with x/
 *              Q_Put(&q, data_ptr);
 *          }
 *
 *          /x Search with binary search x/
 *          if (Q_Seek(&q, &mydata, MyFunction))
 *              /x etc x/
 *
 *
 ****************************************************************/

#include <stdlib.h>
#include <php.h>
#include "queue.h"

static void QuickSort(void *list[], int low, int high,
                      int (*Comp)(const void *, const void *));
static int  Q_BSearch(queue *q, void *key,
                      int (*Comp)(const void *, const void *));

/* The index: a pointer to pointers */

static  void        **queue_index;
static  datanode    **queue_posn_index;


/***
 *
 ** function    : Q_Init
 *
 ** purpose     : Initialise queue object and pointers.
 *
 ** parameters  : 'queue' pointer.
 *
 ** returns     : True_ if init successful else False_
 *
 ** comments    :
 ***/

int Q_Init(queue  *q)
{
   if(q) {
      q->head = q->tail = NULL;
      q->cursor = q->head;
      q->size = 0;
      q->sorted = False_;
   }

   return True_;
}

/***
 *
 ** function    : Q_AtHead
 *
 ** purpose     : tests if cursor is at head of queue
 *
 ** parameters  : 'queue' pointer.
 *
 ** returns     : boolean - True_ is at head else False_
 *
 ** comments    :
 *
 ***/

int Q_AtHead(queue *q)
{
   return(q && q->cursor == q->head);
}


/***
 *
 ** function    : Q_AtTail
 *
 ** purpose     : boolean test if cursor at tail of queue
 *
 ** parameters  : 'queue' pointer to test.
 *
 ** returns     : True_ or False_
 *
 ** comments    :
 *
 ***/

int Q_AtTail(queue *q)
{
   return(q && q->cursor == q->tail);
}


/***
 *
 ** function    : Q_IsEmpty
 *
 ** purpose     : test if queue has nothing in it.
 *
 ** parameters  : 'queue' pointer
 *
 ** returns     : True_ if IsEmpty queue, else False_
 *
 ** comments    :
 *
 ***/

inline int Q_IsEmpty(queue *q)
{
   return(!q || q->size == 0);
}

/***
 *
 ** function    : Q_Size
 *
 ** purpose     : return the number of elements in the queue
 *
 ** parameters  : queue pointer
 *
 ** returns     : number of elements
 *
 ** comments    :
 *
 ***/

int Q_Size(queue *q)
{
   return q ? q->size : 0;
}


/***
 *
 ** function    : Q_Head
 *
 ** purpose     : position queue cursor to first element (head) of queue.
 *
 ** parameters  : 'queue' pointer
 *
 ** returns     : pointer to data at head. If queue is IsEmpty returns NULL
 *
 ** comments    :
 *
 ***/

void *Q_Head(queue *q)
{
   if(Q_IsEmpty(q))
      return NULL;

   q->cursor = q->head;

   return  q->cursor->data;
}


/***
 *
 ** function    : Q_Tail
 *
 ** purpose     : locate cursor at tail of queue.
 *
 ** parameters  : 'queue' pointer
 *
 ** returns     : pointer to data at tail , if queue IsEmpty returns NULL
 *
 ** comments    :
 *
 ***/

void *Q_Tail(queue *q)
{
   if(Q_IsEmpty(q))
      return NULL;

   q->cursor = q->tail;

   return  q->cursor->data;
}


/***
 *
 ** function    : Q_PushHead
 *
 ** purpose     : put a data pointer at the head of the queue
 *
 ** parameters  : 'queue' pointer, void pointer to the data.
 *
 ** returns     : True_ if success else False_ if unable to push data.
 *
 ** comments    :
 *
 ***/

int Q_PushHead(queue *q, void *d)
{
   if(q && d) {
      node    *n;
      datanode *p;

      p = emalloc(sizeof(datanode));
      if(p == NULL)
         return False_;

      n = q->head;

      q->head = (node*)p;
      q->head->prev = NULL;

      if(q->size == 0) {
         q->head->next = NULL;
         q->tail = q->head;
      }
      else {
         q->head->next = (datanode*)n;
         n->prev = q->head;
      }

      q->head->data = d;
      q->size++;

      q->cursor = q->head;

      q->sorted = False_;

      return True_;
   }
   return False_;
}



/***
 *
 ** function    : Q_PushTail
 *
 ** purpose     : put a data element pointer at the tail of the queue
 *
 ** parameters  : queue pointer, pointer to the data
 *
 ** returns     : True_ if data pushed, False_ if data not inserted.
 *
 ** comments    :
 *
 ***/

int Q_PushTail(queue *q, void *d)
{
   if(q && d) {
      node        *p;
      datanode    *n;

      n = emalloc(sizeof(datanode));
      if(n == NULL)
         return False_;

      p = q->tail;
      q->tail = (node *)n;

      if(q->size == 0) {
         q->tail->prev = NULL;
         q->head = q->tail;
      }
      else {
         q->tail->prev = (datanode *)p;
         p->next = q->tail;
      }

      q->tail->next = NULL;

      q->tail->data =  d;
      q->cursor = q->tail;
      q->size++;

      q->sorted = False_;

      return True_;
   }
   return False_;
}



/***
 *
 ** function    : Q_PopHead
 *
 ** purpose     : remove and return the top element at the head of the
 *                queue.
 *
 ** parameters  : queue pointer
 *
 ** returns     : pointer to data element or NULL if queue is IsEmpty.
 *
 ** comments    :
 *
 ***/

void *Q_PopHead(queue *q)
{
   datanode    *n;
   void        *d;

   if(Q_IsEmpty(q))
      return NULL;

   d = q->head->data;
   n = q->head->next;
   efree(q->head);

   q->size--;

   if(q->size == 0)
      q->head = q->tail = q->cursor = NULL;
   else {
      q->head = (node *)n;
      q->head->prev = NULL;
      q->cursor = q->head;
   }

   q->sorted = False_;

   return d;
}


/***
 *
 ** function    : Q_PopTail
 *
 ** purpose     : remove element from tail of queue and return data.
 *
 ** parameters  : queue pointer
 *
 ** returns     : pointer to data element that was at tail. NULL if queue
 *                IsEmpty.
 *
 ** comments    :
 *
 ***/

void *Q_PopTail(queue *q)
{
   datanode    *p;
   void        *d;

   if(Q_IsEmpty(q))
      return NULL;

   d = q->tail->data;
   p = q->tail->prev;
   efree(q->tail);
   q->size--;

   if(q->size == 0)
      q->head = q->tail = q->cursor = NULL;
   else {
      q->tail = (node *)p;
      q->tail->next = NULL;
      q->cursor = q->tail;
   }

   q->sorted = False_;

   return d;
}



/***
 *
 ** function    : Q_Next
 *
 ** purpose     : Move to the next element in the queue without popping
 *
 ** parameters  : queue pointer.
 *
 ** returns     : pointer to data element of new element or NULL if end
 *                of the queue.
 *
 ** comments    : This uses the cursor for the current position. Q_Next
 *                only moves in the direction from the head of the queue
 *                to the tail.
 ***/

void *Q_Next(queue *q)
{
   if(!q)
      return NULL;

   if(!q->cursor || q->cursor->next == NULL)
      return NULL;

   q->cursor = (node *)q->cursor->next;

   return  q->cursor->data ;
}



/***
 *
 ** function    : Q_Previous
 *
 ** purpose     : Opposite of Q_Next. Move to next element closer to the
 *                head of the queue.
 *
 ** parameters  : pointer to queue
 *
 ** returns     : pointer to data of new element else NULL if queue IsEmpty
 *
 ** comments    : Makes cursor move towards the head of the queue.
 *
 ***/

void *Q_Previous(queue *q)
{
   if(!q)
      return NULL;

   if(q->cursor->prev == NULL)
      return NULL;

   q->cursor = (node *)q->cursor->prev;

   return q->cursor->data;
}


void *Q_Iter_Del(queue *q, q_iter iter)
{
   void        *d;
   datanode    *n, *p;

   if(!q)
      return NULL;

   if(iter == NULL)
      return NULL;

   if(iter == (q_iter)q->head)
      return Q_PopHead(q);

   if(iter == (q_iter)q->tail)
      return Q_PopTail(q);

   n = ((node*)iter)->next;
   p = ((node*)iter)->prev;
   d = ((node*)iter)->data;

   efree(iter);

   if(p) {
      p->next = n;
   }
   if (q->cursor == (node*)iter) {
      if (p) {
         q->cursor = p;
      } else {
         q->cursor = n;
      }
   }


   if (n != NULL) {
	n->prev = p;
   }

   q->size--;

   q->sorted = False_;

   return d;
}



/***
 *
 ** function    : Q_DelCur
 *
 ** purpose     : Delete the current queue element as pointed to by
 *                the cursor.
 *
 ** parameters  : queue pointer
 *
 ** returns     : pointer to data element.
 *
 ** comments    : WARNING! It is the responsibility of the caller to
 *                free any memory. Queue cannot distinguish between
 *                pointers to literals and malloced memory.
 *
 ***/

void *Q_DelCur(queue* q) {
   if(q) {
      return Q_Iter_Del(q, (q_iter)q->cursor);
   }
   return 0;
}


/***
 *
 ** function    : Q_Destroy
 *
 ** purpose     : Free all queue resources
 *
 ** parameters  : queue pointer
 *
 ** returns     : null.
 *
 ** comments    : WARNING! It is the responsibility of the caller to
 *                free any memory. Queue cannot distinguish between
 *                pointers to literals and malloced memory.
 *
 ***/

void Q_Destroy(queue *q)
{
   while(!Q_IsEmpty(q)) {
      Q_PopHead(q);
   }
}


/***
 *
 ** function    : Q_Get
 *
 ** purpose     : get the pointer to the data at the cursor location
 *
 ** parameters  : queue pointer
 *
 ** returns     : data element pointer
 *
 ** comments    :
 *
 ***/

void *Q_Get(queue *q)
{
   if(!q)
      return NULL;

   if(q->cursor == NULL)
      return NULL;
   return q->cursor->data;
}



/***
 *
 ** function    : Q_Put
 *
 ** purpose     : replace pointer to data with new pointer to data.
 *
 ** parameters  : queue pointer, data pointer
 *
 ** returns     : boolean- True_ if successful, False_ if cursor at NULL
 *
 ** comments    :
 *
 ***/

int Q_Put(queue *q, void *data)
{
   if(q && data) {
      if(q->cursor == NULL)
         return False_;

      q->cursor->data = data;
      return True_;
   }
   return False_;
}


/***
 *
 ** function    : Q_Find
 *
 ** purpose     : Linear search of queue for match with key in *data
 *
 ** parameters  : queue pointer q, data pointer with data containing key
 *                comparison function here called Comp.
 *
 ** returns     : True_ if found , False_ if not in queue.
 *
 ** comments    : Useful for small queues that are constantly changing
 *                and would otherwise need constant sorting with the
 *                Q_Seek function.
 *                For description of Comp see Q_Sort.
 *                Queue cursor left on position found item else at end.
 *
 ***/

int Q_Find(queue *q, void *data,
           int (*Comp)(const void *, const void *))
{
   void *d;

   if (q == NULL) {
	return False_;
   }

   d = Q_Head(q);
   do {
      if(Comp(d, data) == 0)
         return True_;
      d = Q_Next(q);
   } while(!Q_AtTail(q));

   if(Comp(d, data) == 0)
      return True_;

   return False_;
}

/*========  Sorted Queue and Index functions   ========= */


static void QuickSort(void *list[], int low, int high,
                      int (*Comp)(const void *, const void *))
{
   int     flag = 1, i, j;
   void    *key, *temp;

   if(low < high) {
      i = low;
      j = high + 1;

      key = list[ low ];

      while(flag) {
         i++;
         while(Comp(list[i], key) < 0)
            i++;

         j--;
         while(Comp(list[j], key) > 0)
            j--;

         if(i < j) {
            temp = list[i];
            list[i] = list[j];
            list[j] = temp;
         }
         else  flag = 0;
      }

      temp = list[low];
      list[low] = list[j];
      list[j] = temp;

      QuickSort(list, low, j-1, Comp);
      QuickSort(list, j+1, high, Comp);
   }
}


/***
 *
 ** function    : Q_Sort
 *
 ** purpose     : sort the queue and allow index style access.
 *
 ** parameters  : queue pointer, comparison function compatible with
 *                with 'qsort'.
 *
 ** returns     : True_ if sort succeeded. False_ if error occurred.
 *
 ** comments    : Comp function supplied by caller must return
 *                  -1 if data1  < data2
 *                   0 if data1 == data2
 *                  +1 if data1  > data2
 *
 *                    for Comp(data1, data2)
 *
 *                If queue is already sorted it frees the memory of the
 *                old index and starts again.
 *
 ***/

int Q_Sort(queue *q, int (*Comp)(const void *, const void *))
{
   int         i;
   void        *d;
   datanode    *dn;

   /* if already sorted free memory for tag array */

   if(q->sorted) {
      efree(queue_index);
      efree(queue_posn_index);
      q->sorted = False_;
   }

   /* Now allocate memory of array, array of pointers */

   queue_index = emalloc(q->size * sizeof(q->cursor->data));
   if(queue_index == NULL)
      return False_;

   queue_posn_index = emalloc(q->size * sizeof(q->cursor));
   if(queue_posn_index == NULL) {
      efree(queue_index);
      return False_;
   }

   /* Walk queue putting pointers into array */

   d = Q_Head(q);
   for(i=0; i < q->size; i++) {
      queue_index[i] = d;
      queue_posn_index[i] = q->cursor;
      d = Q_Next(q);
   }

   /* Now sort the index */

   QuickSort(queue_index, 0, q->size - 1, Comp);

   /* Rearrange the actual queue into correct order */

   dn = q->head;
   i = 0;
   while(dn != NULL) {
      dn->data = queue_index[i++];
      dn = dn->next;
   }

   /* Re-position to original element */

   if(d != NULL)
      Q_Find(q, d, Comp);
   else  Q_Head(q);

   q->sorted = True_;

   return True_;
}


/***
 *
 ** function    : Q_BSearch
 *
 ** purpose     : binary search of queue index for node containing key
 *
 ** parameters  : queue pointer 'q', data pointer of key 'key',
 *                  Comp comparison function.
 *
 ** returns     : integer index into array of node pointers,
 *                or -1 if not found.
 *
 ** comments    : see Q_Sort for description of 'Comp' function.
 *
 ***/

static int Q_BSearch( queue *q, void *key,
                      int (*Comp)(const void *, const void*))
{
   int low, mid, hi, val;

   low = 0;
   hi = q->size - 1;

   while(low <= hi) {
      mid = (low + hi) / 2;
      val = Comp(key, queue_index[ mid ]);

      if(val < 0)
         hi = mid - 1;

      else if(val > 0)
         low = mid + 1;

      else /* Success */
         return mid;
   }

   /* Not Found */

   return -1;
}


/***
 *
 ** function    : Q_Seek
 *
 ** purpose     : use index to locate data according to key in 'data'
 *
 ** parameters  : queue pointer 'q', data pointer 'data', Comp comparison
 *                function.
 *
 ** returns     : pointer to data or NULL if could not find it or could
 *                not sort queue.
 *
 ** comments    : see Q_Sort for description of 'Comp' function.
 *
 ***/

void *Q_Seek(queue *q, void *data, int (*Comp)(const void *, const void *))
{
   int idx;

   if (q == NULL) {
	return NULL;
   }

   if(!q->sorted) {
      if(!Q_Sort(q, Comp))
         return NULL;
   }

   idx = Q_BSearch(q, data, Comp);

   if(idx < 0)
      return NULL;

   q->cursor = queue_posn_index[idx];

   return queue_index[idx];
}



/***
 *
 ** function    : Q_Insert
 *
 ** purpose     : Insert an element into an indexed queue
 *
 ** parameters  : queue pointer 'q', data pointer 'data', Comp comparison
 *                function.
 *
 ** returns     : pointer to data or NULL if could not find it or could
 *                not sort queue.
 *
 ** comments    : see Q_Sort for description of 'Comp' function.
 *                WARNING! This code can be very slow since each new
 *                element means a new Q_Sort.  Should only be used for
 *                the insertion of the odd element ,not the piecemeal
 *                building of an entire queue.
 ***/

int Q_Insert(queue *q, void *data, int (*Comp)(const void *, const void *))
{
   if (q == NULL) {
	return False_;
   }

   Q_PushHead(q, data);

   if(!Q_Sort(q, Comp))
      return False_;

   return True_;
}

/* read only funcs for iterating through queue. above funcs modify queue */
q_iter Q_Iter_Head(queue *q) {
   return q ? (q_iter)q->head : NULL;
}

q_iter Q_Iter_Tail(queue *q) {
   return q ? (q_iter)q->tail : NULL;
}

q_iter Q_Iter_Next(q_iter qi) {
   return qi ? (q_iter)((node*)qi)->next : NULL;
}

q_iter Q_Iter_Prev(q_iter qi) {
   return qi ? (q_iter)((node*)qi)->prev : NULL;
}

void * Q_Iter_Get(q_iter qi) {
   return qi ? ((node*)qi)->data : NULL;
}

int Q_Iter_Put(q_iter qi, void* data) {
   if(qi) {
      ((node*)qi)->data = data;
      return True_;
   }
   return False_;
}
