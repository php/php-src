/****************************************************************************
*
*					Copyright (C) 1991 Kendall Bennett.
*							All rights reserved.
*
* Filename:		$RCSfile$
* Version:		$Revision$
*
* Language:		ANSI C
* Environment:	any
*
* Description:	Module to implement doubly linked lists. Includes a routine
*				to peform a mergesort on the doubly linked list.
*
* $Id$
*
* Revision History:
* -----------------
*
* $Log$
* Revision 1.2  1999/05/11 00:01:42  zeev
* * Get Apache to work.  POST doesn't work yet.
* * There are now -I directives for the absolute path of php4, php4/libzend and the builddir for
*   the Apache module, so we can #include any php/Zend header.
* * Rename config.h to php_config.h
*
* Revision 1.1  1999/04/21 23:11:20  ssb
* moved apache, com and hyperwave into ext/
*
* Revision 1.1.1.1  1999/04/07 21:03:31  zeev
* PHP 4.0
*
* Revision 1.1.1.1  1999/03/17 04:29:11  andi
* PHP4
*
* Revision 1.1.1.1  1998/12/21 07:56:22  andi
* Trying to start the zend CVS tree
*
* Revision 1.1  1998/08/12 09:29:16  steinm
* First version of Hyperwave module.
*
* Revision 1.5  91/12/31  19:39:49  kjb
* Modified include file directories.
* 
* Revision 1.4  91/10/28  03:16:39  kjb
* Ported to the Iris.
* 
* Revision 1.3  91/09/27  03:09:18  kjb
* Cosmetic change.
* 
* Revision 1.2  91/09/03  18:27:42  ROOT_DOS
* Ported to UNIX.
* 
* Revision 1.1  91/09/01  18:35:23  ROOT_DOS
* Initial revision
* 
****************************************************************************/

#ifndef MSVC5
#include "php_config.h"
#endif

#include <stdio.h>
#include <malloc.h>
#include <signal.h>
#include "dlist.h"

#define PUBLIC
#define PRIVATE static

PUBLIC void *dlst_newnode(int size)
/****************************************************************************
*
* Function:		dlst_newnode
* Parameters:	size	- Amount of memory to allocate for node
* Returns:      Pointer to the allocated node's user space.
*
* Description:	Allocates the memory required for a node, adding a small
*				header at the start of the node. We return a reference to
*				the user space of the node, as if it had been allocated via
*				malloc().
*
****************************************************************************/
{
	PHP_DLST_BUCKET	*node;

	if ( !(node = (PHP_DLST_BUCKET*)malloc(size + sizeof(PHP_DLST_BUCKET))) ) {
		fprintf(stderr,"Not enough memory to allocate list node.\n");
/*		raise(SIGABRT);*/
		return NULL;
		}

	return PHP_DLST_USERSPACE(node);		/* Return pointer to user space */
}

PUBLIC void dlst_freenode(void *node)
/****************************************************************************
*
* Function:		dlst_freenode
* Parameters:	node	- Node to free.
*
* Description:  Frees a node previously allocated with lst_newnode().
*
****************************************************************************/
{
	free(PHP_DLST_HEADER(node));
}

PUBLIC DLIST *dlst_init(void)
/****************************************************************************
*
* Function:		dlst_init
* Returns:      Pointer to a newly created list.
*
* Description:	Initialises a list and returns a pointer to it.
*
****************************************************************************/
{
	DLIST	*l;

	if ((l = (DLIST*)malloc(sizeof(DLIST))) != NULL) {
		l->count = 0;
		l->head = &(l->hz[0]);
		l->z = &(l->hz[1]);
		l->head->next = l->z->next = l->z;
		l->z->prev = l->head->prev = l->head;
		}
	else {
		fprintf(stderr,"Insufficient memory to allocate list\n");
		/*raise(SIGABRT);*/
		return NULL;
		}

	return l;
}

PUBLIC void dlst_kill(DLIST *l,void (*freeNode)(void *node))
/****************************************************************************
*
* Function:		dlst_kill
* Parameters:	l			- List to kill
*				freeNode	- Pointer to user routine to free a node
*
* Description:	Kills the list l, by deleting all of the elements contained
*				within the list one by one and then deleting the list
*				itself. Note that we call the user supplied routine
*				(*freeNode)() to free each list node. This allows the user
*				program to perform any extra processing needed to kill each
*				node (if each node contains pointers to other items on the
*				heap for example). If no extra processing is required, just
*				pass the address of dlst_freenode(), ie:
*
*					dlst_kill(myList,dlst_freenode);
*
****************************************************************************/
{
	PHP_DLST_BUCKET	*n,*p;

	n = l->head->next;
	while (n != l->z) {			/* Free all nodes in list				*/
		p = n;
		n = n->next;
		(*freeNode)(PHP_DLST_USERSPACE(p));
		}
	free(l);					/* Free the list itself					*/
}

PUBLIC void dlst_insertafter(DLIST *l,void *node,void *after)
/****************************************************************************
*
* Function:		lst_insertafter
* Parameters:	l		- List to insert node into
*				node	- Pointer to user space of node to insert
*				after	- Pointer to user space of node to insert node after
*
* Description:	Inserts a new node into the list after the node 'after'. To
*				insert a new node at the beginning of the list, user the
*				macro PHP_DLST_HEAD in place of 'after'. ie:
*
*					dlst_insertafter(mylist,node,PHP_DLST_HEAD(mylist));
*
****************************************************************************/
{
	PHP_DLST_BUCKET	*n = PHP_DLST_HEADER(node),*a = PHP_DLST_HEADER(after);

	n->next = a->next;
	a->next = n;
	n->prev = a;
	n->next->prev = n;
	l->count++;
}

PUBLIC void *dlst_deletenext(DLIST *l,void *node)
/****************************************************************************
*
* Function:		lst_deletenext
* Parameters:	l		- List to delete node from.
*				node	- Node to delete the next node from
* Returns:		Pointer to the deleted node's userspace.
*
* Description:	Removes the node AFTER 'node' from the list l.
*
****************************************************************************/
{
	PHP_DLST_BUCKET	*n = PHP_DLST_HEADER(node);

	node = PHP_DLST_USERSPACE(n->next);
	n->next->next->prev = n;
	n->next = n->next->next;
	l->count--;
	return node;
}

PUBLIC void *dlst_first(DLIST *l)
/****************************************************************************
*
* Function:		dlst_first
* Parameters:	l		- List to obtain first node from
* Returns:		Pointer to first node in list, NULL if list is empty.
*
* Description:	Returns a pointer to the user space of the first node in
*				the list. If the list is empty, we return NULL.
*
****************************************************************************/
{
	PHP_DLST_BUCKET	*n;

	n = l->head->next;
	return (n == l->z ? NULL : PHP_DLST_USERSPACE(n));
}

PUBLIC void *dlst_last(DLIST *l)
/****************************************************************************
*
* Function:		dlst_last
* Parameters:	l	- List to obtain last node from
* Returns:		Pointer to last node in list, NULL if list is empty.
*
* Description:	Returns a pointer to the user space of the last node in
*				the list. If the list is empty, we return NULL.
*
****************************************************************************/
{
	PHP_DLST_BUCKET	*n;

	n = l->z->prev;
	return (n == l->head ? NULL : PHP_DLST_USERSPACE(n));
}

PUBLIC void *dlst_next(void *prev)
/****************************************************************************
*
* Function:		dlst_next
* Parameters:	prev	- Previous node in list to obtain next node from
* Returns:		Pointer to the next node in the list, NULL at end of list.
*
* Description:	Returns a pointer to the user space of the next node in the
*				list given a pointer to the user space of the previous node.
*				If we have reached the end of the list, we return NULL. The
*				end of the list is detected when the next pointer of a node
*				points back to itself, as does the dummy last node's next
*				pointer. This enables us to detect the end of the list
*				without needed access to the list data structure itself.
*
*				NOTE:	We do no checking to ensure that 'prev' is NOT a
*						NULL pointer.
*
****************************************************************************/
{
	PHP_DLST_BUCKET	*n = PHP_DLST_HEADER(prev);

	n = n->next;
	return (n == n->next ? NULL : PHP_DLST_USERSPACE(n));
}

PUBLIC void *dlst_prev(void *next)
/****************************************************************************
*
* Function:		dlst_prev
* Parameters:	next	- Next node in list to obtain previous node from
* Returns:		Pointer to the previous node in the list, NULL at start list.
*
* Description:	Returns a pointer to the user space of the prev node in the
*				list given a pointer to the user space of the next node.
*				If we have reached the start of the list, we return NULL. The
*				start of the list is detected when the prev pointer of a node
*				points back to itself, as does the dummy head node's prev
*				pointer. This enables us to detect the start of the list
*				without needed access to the list data structure itself.
*
*				NOTE:	We do no checking to ensure that 'next' is NOT a
*						NULL pointer.
*
****************************************************************************/
{
	PHP_DLST_BUCKET	*n = PHP_DLST_HEADER(next);

	n = n->prev;
	return (n == n->prev ? NULL : PHP_DLST_USERSPACE(n));
}

/* Static globals required by merge()	*/

static PHP_DLST_BUCKET	*z;
static int 			(*cmp)(void*,void*);

PRIVATE PHP_DLST_BUCKET *merge(PHP_DLST_BUCKET *a,PHP_DLST_BUCKET *b,PHP_DLST_BUCKET **end)
/****************************************************************************
*
* Function:		merge
* Parameters:	a,b		- Sublist's to merge
* Returns:		Pointer to the merged sublists.
*
* Description:	Merges two sorted lists of nodes together into a single
*				sorted list.
*
****************************************************************************/
{
	PHP_DLST_BUCKET	*c;

	/* Go through the lists, merging them together in sorted order	*/

	c = z;
	while (a != z && b != z) {
		if ((*cmp)(PHP_DLST_USERSPACE(a),PHP_DLST_USERSPACE(b)) <= 0) {
			c->next = a; c = a; a = a->next;
			}
		else {
			c->next = b; c = b; b = b->next;
			}
		};

	/* If one of the lists is not exhausted, then re-attach it to the end
	 * of the newly merged list
	 */

	if (a != z) c->next = a;
	if (b != z) c->next = b;

	/* Set *end to point to the end of the newly merged list	*/

	while (c->next != z) c = c->next;
	*end = c;

	/* Determine the start of the merged lists, and reset z to point to
	 * itself
	 */

	c = z->next; z->next = z;
	return c;
}

PUBLIC void dlst_mergesort(DLIST *l,int (*cmp_func)(void*,void*))
/****************************************************************************
*
* Function:		dlst_mergesort
* Parameters:	l			- List to merge sort
*				cmp_func	- Function to compare two user spaces
*
* Description:	Mergesort's all the nodes in the list. 'cmp' must point to
*				a comparison function that can compare the user spaces of
*				two different nodes. 'cmp' should work the same as
*				strcmp(), in terms of the values it returns. Rather than
*				waste processing time keeping the previous pointers up to
*				date while performing the mergesort, we simply run through
*				the list at the end of the sort to fix the previous pointers.
*
****************************************************************************/
{
	int			i,N;
	PHP_DLST_BUCKET	*a,*b;		/* Pointers to sublists to merge			*/
	PHP_DLST_BUCKET	*c;			/* Pointer to end of sorted sublists		*/
	PHP_DLST_BUCKET	*head;		/* Pointer to dummy head node for list		*/
	PHP_DLST_BUCKET	*todo;		/* Pointer to sublists yet to be sorted		*/
	PHP_DLST_BUCKET	*t;			/* Temporary								*/

	/* Set up globals required by merge() and pointer to head	*/

	z = l->z; cmp = cmp_func; head = l->head;

	for (N = 1,a = z; a != head->next; N = N + N) {
		todo = head->next; c = head;
		while (todo != z) {

			/* Build first sublist to be merged, and splice from main list
			 */

			a = t = todo;
			for (i = 1; i < N; i++) t = t->next;
			b = t->next; t->next = z; t = b;

			/* Build second sublist to be merged and splice from main list
			 */

			for (i = 1; i < N; i++) t = t->next;
			todo = t->next; t->next = z;

			/* Merge the two sublists created, and set 'c' to point to the
			 * end of the newly merged sublists.
			 */

			c->next = merge(a,b,&t); c = t;
			}
		}

	/* Fix the previous pointers for the list	*/

	a = b = l->head;
	b = b->next;
	while (1) {
		b->prev = a;
		if (b == z)
			break;
		a = a->next;
		b = b->next;
		}
}

