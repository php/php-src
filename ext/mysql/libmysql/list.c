/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*
  Code for handling dubble-linked lists in C
*/

#include "mysys_priv.h"
#include <my_list.h>



	/* Add a element to start of list */

LIST *list_add(LIST *root, LIST *element)
{
  DBUG_ENTER("list_add");
  DBUG_PRINT("enter",("root: %lx  element: %lx", root, element));
  if (root)
  {
    if (root->prev)			/* If add in mid of list */
      root->prev->next= element;
    element->prev=root->prev;
    root->prev=element;
  }
  else
    element->prev=0;
  element->next=root;
  DBUG_RETURN(element);			/* New root */
}


LIST *list_delete(LIST *root, LIST *element)
{
  if (element->prev)
    element->prev->next=element->next;
  else
    root=element->next;
  if (element->next)
    element->next->prev=element->prev;
  return root;
}


void list_free(LIST *root, pbool free_data)
{
  LIST *next;
  while (root)
  {
    next=root->next;
    if (free_data)
      my_free((gptr) root->data,MYF(0));
    my_free((gptr) root,MYF(0));
    root=next;
  }
}


LIST *list_cons(void *data, LIST *list)
{
  LIST *new_charset=(LIST*) my_malloc(sizeof(LIST),MYF(MY_FAE));
  if (!new_charset)
    return 0;
  new_charset->data=data;
  return list_add(list,new_charset);
}


LIST *list_reverse(LIST *root)
{
  LIST *last;

  last=root;
  while (root)
  {
    last=root;
    root=root->next;
    last->next=last->prev;
    last->prev=root;
  }
  return last;
}

uint list_length(LIST *list)
{
  uint count;
  for (count=0 ; list ; list=list->next, count++) ;
  return count;
}


int list_walk(LIST *list, list_walk_action action, gptr argument)
{
  int error=0;
  while (list)
  {
    if ((error = (*action)(list->data,argument)))
      return error;
    list=rest(list);
  }
  return 0;
}
