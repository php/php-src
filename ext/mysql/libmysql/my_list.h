/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#ifndef _list_h_
#define _list_h_

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct st_list {
  struct st_list *prev,*next;
  void *data;
} LIST;

typedef int (*list_walk_action)(void *,void *);

extern LIST *list_add(LIST *root,LIST *element);
extern LIST *list_delete(LIST *root,LIST *element);
extern LIST *list_cons(void *data,LIST *root);
extern LIST *list_reverse(LIST *root);
extern void list_free(LIST *root,pbool free_data);
extern uint list_length(LIST *list);
extern int list_walk(LIST *list,list_walk_action action,gptr argument);

#define rest(a) ((a)->next)
#define list_push(a,b) (a)=list_cons((b),(a))
#define list_pop(A) {LIST *old=(A); (A)=list_delete(old,old) ; my_free((gptr) old,MYF(MY_FAE)); }

#ifdef	__cplusplus
}
#endif
#endif
