/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |          Mark Musone         <musone@chek.com>                  |
   +----------------------------------------------------------------------+
 */

#define MCAL1

#ifdef ERROR
#undef ERROR
#endif

#include "php.h"

#if COMPILE_DL
#include "dl/phpdl.h"
#endif

#if HAVE_MCAL

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#include "mcal.h"
#include "php3_mcal.h"
#include "modules.h"
#if (WIN32|WINNT)
#include "winsock.h"
#endif
CALSTREAM *cal_open();
CALSTREAM *cal_close_it ();
CALSTREAM *cal_close_full ();


typedef struct php3_mcal_le_struct {
	CALSTREAM *mcal_stream;
	long flags;
} pils;


typedef struct cal_list
{
u_int32_t uid;
struct cal_list *next;
} cal_list_t;

static cal_list_t *g_cal_list=NULL;
static cal_list_t *g_cal_list_end=NULL;
/* 
 * this array should be set up as:
 * {"PHPScriptFunctionName",dllFunctionName,1} 
 */

function_entry mcal_functions[] = {
	{"mcal_open", php3_mcal_open, NULL},
	{"mcal_popen", php3_mcal_popen, NULL},
	{"mcal_reopen", php3_mcal_reopen, NULL},
	{"mcal_fetch_event", php3_mcal_fetch_event, NULL},
	{"mcal_list_events", php3_mcal_list_events, NULL},
	{"mcal_list_alarms", php3_mcal_list_alarms, NULL},
	{"mcal_create_calendar", php3_mcal_create_calendar, NULL},
	{"mcal_rename_calendar", php3_mcal_rename_calendar, NULL},
	{"mcal_delete_calendar", php3_mcal_delete_calendar, NULL},
	{"mcal_delete_event", php3_mcal_delete_event, NULL},
	{"mcal_store_event", php3_mcal_store_event, NULL},
	{"mcal_snooze", php3_mcal_snooze, NULL},
	{NULL, NULL, NULL}
};


php3_module_entry php3_mcal_module_entry = {
	CALVER, mcal_functions, PHP_MINIT(mcal), NULL, NULL, NULL, PHP_MINFO(mcal), 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &php3_mcal_module_entry; }
#endif

/* 
   I believe since this global is used ONLY within this module,
   and nothing will link to this module, we can use the simple 
   thread local_ storage
*/
int le_mcal;
#ifdef OP_RELOGIN
/* AJS: persistent connection type */
int le_pmcal;
#endif
char mcal_user[80]="";
char mcal_password[80]="";

CALSTREAM *cal_close_it (pils *mcal_le_struct)
{
	CALSTREAM *ret;
	ret = cal_close (mcal_le_struct->mcal_stream,0);
	efree(mcal_le_struct);
	return ret;
}


PHP_MINFO_FUNCTION(mcal)
{
	php3_printf("Mcal Support enabled<br>");
	php3_printf("<table>");
	php3_printf("<tr><td>Mcal Version:</td>");
	php3_printf("<td>%s</td>",CALVER);
	php3_printf("</tr></table>");
}

PHP_MINIT_FUNCTION(mcal)
{

    le_mcal = register_list_destructors(cal_close_it,NULL);

    return SUCCESS;
}


static int add_assoc_object(pval *arg, char *key, pval *tmp)
{
        HashTable *symtable;
        
        if (arg->type == IS_OBJECT) {
                symtable = arg->value.obj.properties;
        } else {
                symtable = arg->value.ht;
        }
        return zend_hash_update(symtable, key, strlen(key)+1, (void *) &tmp, sizeof(pval *), NULL);
}


void php3_mcal_do_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	pval *calendar;
	pval *user;
	pval *passwd;
	pval *options;
	CALSTREAM *mcal_stream;
	pils *mcal_le_struct;
	long flags=0;
	long cl_flags=0;
	int ind;
        int myargc=ARG_COUNT(ht);

	
	if (myargc <3 || myargc >4 || getParameters(ht, myargc, &calendar,&user,&passwd,&options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(calendar);
	convert_to_string(user);
	convert_to_string(passwd);
	strcpy(mcal_user,user->value.str.val);
	strcpy(mcal_password,passwd->value.str.val);
	if(myargc ==4) {
		convert_to_long(options);
		flags=options->value.lval;
	}
		mcal_stream = cal_open(NULL,calendar->value.str.val,0);
	if (!mcal_stream) {
		php3_error(E_WARNING,"Couldn't open stream %s\n",calendar->value.str.val);
		RETURN_FALSE;
	}

	mcal_le_struct = emalloc(sizeof(pils));
	mcal_le_struct->mcal_stream = mcal_stream;
	mcal_le_struct->flags = 0;	
	ind = php3_list_insert(mcal_le_struct, le_mcal);	
	RETURN_LONG(ind);
}




/* {{{ proto int mcal_close(int stream_id [, int options])
   Close an MCAL stream */
void php3_mcal_close(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *options, *streamind;
        int ind, ind_type;
        pils *mcal_le_struct=NULL; 
        int myargcount=ARG_COUNT(ht);
        long flags = 0;

        if (myargcount < 1 || myargcount > 2 || getParameters(ht, myargcount, &streamind, &options) == FAILURE) {
                WRONG_PARAM_COUNT;
        }
        convert_to_long(streamind);
        ind = streamind->value.lval;
        mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);
        if (!mcal_le_struct ) {
	  php3_error(E_WARNING, "Unable to find stream pointer");
                RETURN_FALSE;
	}
        if(myargcount==2) {
                convert_to_long(options);
                flags = options->value.lval;
                mcal_le_struct->flags = flags;
        }
        php3_list_delete(ind);
        RETURN_TRUE;
}
/* }}} */








/* {{{ proto int mcal_open(string calendar, string user, string password [, int options])
   Open an MCAL stream to a calendar */
void php3_mcal_open(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mcal_do_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ proto int mcal_reopen(int stream_id, string calendar [, int options])
   Reopen MCAL stream to new calendar */
void php3_mcal_reopen(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	pval *calendar;
	pval *options;
	CALSTREAM *mcal_stream;
	pils *mcal_le_struct; 
	int ind, ind_type;
	long flags=0;
	long cl_flags=0;
	int myargc=ARG_COUNT(ht);

	if (myargc<2 || myargc>3 || getParameters(ht,myargc,&streamind, &calendar, &options) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

	convert_to_long(streamind);
	ind = streamind->value.lval;
	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	convert_to_string(calendar);
	if(myargc == 3) {
		convert_to_long(options);
		flags = options->value.lval;
		mcal_le_struct->flags = cl_flags;	
	}
	//	mcal_stream = cal_connect(calendar->value.str.val);
	//	cal_login(mcal_stream, calendar->value.str.val);
	if (mcal_stream == NULL) {
		php3_error(E_WARNING,"Couldn't re-open stream\n");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int mcal_expunge(int stream_id)
   Delete all messages marked for deletion */
void php3_mcal_expunge(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	int ind, ind_type;
	pval *start,*end;
	pils *mcal_le_struct; 

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

/*	cal_expunge (mcal_le_struct->mcal_stream);
*/
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mcal_fetch_event(int stream_id,int eventid, [int options])
   Fetch an event*/
void php3_mcal_fetch_event(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*eventid,*start,*end,*options=NULL;
	int ind, ind_type;
	pils *mcal_le_struct=NULL; 
	CALEVENT *myevent;
	int myargcount=ARG_COUNT(ht);
	
	if (myargcount < 1 || myargcount > 3 || getParameters(ht, myargcount, &streamind, &eventid,&options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	convert_to_long(eventid);
	ind = streamind->value.lval;
	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
    }
	if(myargcount==3) {
		convert_to_long(options);
	}
	cal_fetch(mcal_le_struct->mcal_stream,eventid->value.lval,&myevent);

	object_init(return_value);
	add_property_long(return_value,"id",myevent->id);
	add_property_long(return_value,"public",myevent->public);
	MAKE_STD_ZVAL(start);
	object_init(start);
	if(myevent->start.has_date)
	  {
	    add_property_long(start,"year",myevent->start.year);
	    add_property_long(start,"month",myevent->start.mon);
	    add_property_long(start,"mday",myevent->start.mday);
	  }
	if(myevent->start.has_time)
	  {
	    add_property_long(start,"hour",myevent->start.hour);
	    add_property_long(start,"min",myevent->start.min);
	    add_property_long(start,"sec",myevent->start.sec);
	  }
	add_assoc_object(return_value, "start",start);

	MAKE_STD_ZVAL(end);
	object_init(end);
	if(myevent->end.has_date)
	  {
	    add_property_long(end,"year",myevent->end.year);
	    add_property_long(end,"month",myevent->end.mon);
	    add_property_long(end,"mday",myevent->end.mday);
	  }
	if(myevent->end.has_time)
	  {
	    add_property_long(end,"hour",myevent->end.hour);
	    add_property_long(end,"min",myevent->end.min);
	    add_property_long(end,"sec",myevent->end.sec);
	  }
	add_assoc_object(return_value, "end",end);
	
	add_property_string(return_value,"category",myevent->category,1);
	add_property_string(return_value,"title",myevent->title,1);
	add_property_string(return_value,"description",myevent->description,1);
	add_property_long(return_value,"alarm",myevent->alarm);
}
/* }}} */

/* {{{ proto array mcal_list_events(int stream_id,int begindate, [int enddate])
   Returns list of UIDs for that day or range of days */
void php3_mcal_list_events(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*begindate,*enddate;
        pval **pvalue;
	int ind, ind_type;
	unsigned long i;
	char *t;
	int mcal_folders=0;
	unsigned int msgno;
	pils *mcal_le_struct; 
	cal_list_t *my_cal_list;
	datetime_t begincal,endcal;
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc <2 || myargc > 3 || getParameters(ht,myargc,&streamind,&begindate,&enddate) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_array(begindate);
	if(myargc == 3) convert_to_array(enddate);
	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	begincal.has_time=0;
	endcal.has_time=0;
	if(_php3_hash_find(begindate->value.ht,"year",sizeof("year"),(void **) &pvalue)== SUCCESS){
	 SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          begincal.year=(*pvalue)->value.lval;
       }
       if(_php3_hash_find(begindate->value.ht,"month",sizeof("month"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          begincal.mon=(*pvalue)->value.lval;
       }
       if(_php3_hash_find(begindate->value.ht,"day",sizeof("day"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          begincal.mday=(*pvalue)->value.lval;
       }
if(myargc == 3)
  {
    if(_php3_hash_find(enddate->value.ht,"year",sizeof("year"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
      convert_to_long(*pvalue);
      endcal.year=(*pvalue)->value.lval;
    }
    if(_php3_hash_find(enddate->value.ht,"month",sizeof("month"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
      convert_to_long(*pvalue);
      endcal.mon=(*pvalue)->value.lval;
    }
    if(_php3_hash_find(enddate->value.ht,"day",sizeof("day"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
      convert_to_long(*pvalue);
      endcal.mday=(*pvalue)->value.lval;
    }
  }
 

g_cal_list=NULL;
 cal_search_range(mcal_le_struct->mcal_stream,&begincal,&endcal);
 my_cal_list=g_cal_list;
 while(my_cal_list != NULL)
   {
     add_next_index_long(return_value,my_cal_list->uid);
     my_cal_list=my_cal_list->next;
     free(g_cal_list);
     g_cal_list=my_cal_list;
   }
}
/* }}} */


/* {{{ proto string mcal_create_calendar(int stream_id, string calendar)
   Create a new calendar*/

void php3_mcal_create_calendar(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *calendar;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc=ARG_COUNT(ht);
	if (myargc <1 || myargc > 2 || getParameters(ht,myargc,&streamind,&calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(calendar);
	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
/*
	if (mcal_create(mcal_le_struct->mcal_stream,calendar->value.str.val)) 
	  {
	    RETURN_TRUE;
	  }
	else 
	  {
	    RETURN_FALSE;
	  }
*/
}
/* }}} */


/* {{{ proto string mcal_rename(int stream_id, string src_calendar, string dest_calendar)
   Rename a calendar*/
void php3_mcal_rename_calendar(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *src_calendar,*dest_calendar;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc=ARG_COUNT(ht);
	if (myargc <2 || myargc > 3 || getParameters(ht,myargc,&streamind,&src_calendar,&dest_calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(src_calendar);
	convert_to_string(dest_calendar);
	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
/*
	if(mcal_rename(mcal_le_struct->mcal_stream,src_calendar->value.str.val,dest_calendar->value.str.val)) {RETURN_TRUE;}
	else {RETURN_FALSE; }
*/
}
/* }}} */




/* {{{ proto int mcal_reopen(int stream_id, array date, array time)
   list alarms for a given time */
void php3_mcal_list_alarms(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *date,*time;
        pval **pvalue;
	datetime_t mydate;
	int ind, ind_type;
	pils *mcal_le_struct; 
        int mcal_folders=0;
        unsigned int msgno;
        cal_list_t *my_cal_list;

	int myargc=ARG_COUNT(ht);
	if (myargc != 3 || getParameters(ht,myargc,&streamind,&date,&time) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_array(date);
	convert_to_array(time);
	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

        if (array_init(return_value) == FAILURE) {
                RETURN_FALSE;
        }
	mydate.has_date=1;
	mydate.has_time=1;
       if(_php3_hash_find(date->value.ht,"year",sizeof("year"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.year=(*pvalue)->value.lval;
       }
       if(_php3_hash_find(date->value.ht,"month",sizeof("month"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.mon=(*pvalue)->value.lval;
       }
       if(_php3_hash_find(date->value.ht,"day",sizeof("day"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.mday=(*pvalue)->value.lval;
       }

       if(_php3_hash_find(time->value.ht,"hour",sizeof("hour"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.hour=(*pvalue)->value.lval;
       }
       if(_php3_hash_find(time->value.ht,"minute",sizeof("minute"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.min=(*pvalue)->value.lval;
       }
       mydate.sec=0;
       g_cal_list=NULL;
       cal_search_alarm(mcal_le_struct->mcal_stream,&mydate);
       my_cal_list=g_cal_list;
 while(my_cal_list != NULL)
   {
     add_next_index_long(return_value,my_cal_list->uid);
     my_cal_list=my_cal_list->next;
     free(g_cal_list);
     g_cal_list=my_cal_list;
   }


}
/* }}} */


/* {{{ proto string mcal_delete_calendar(int stream_id, string calendar)
   Delete calendar*/
void php3_mcal_delete_calendar(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *calendar;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc=ARG_COUNT(ht);
	if (myargc <1 || myargc > 2 || getParameters(ht,myargc,&streamind,&calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(calendar);
	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	if (mcal_delete_calendar(mcal_le_struct->mcal_stream,calendar->value.str.val)) 
	  {
	    RETURN_TRUE;
	  }
	else 
	  {
	    RETURN_FALSE;
	  }
	
}
/* }}} */


/* {{{ proto string mcal_delete_event(int stream_id, int uid)
   Delete event*/
void php3_mcal_delete_event(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *uid;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc=ARG_COUNT(ht);
	if (myargc <1 || myargc > 2 || getParameters(ht,myargc,&streamind,&uid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(uid);
	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (cal_remove(mcal_le_struct->mcal_stream,uid->value.lval)) 
	  {
	    RETURN_TRUE;
	  }
	else 
	  {
	    RETURN_FALSE;
	  }
}
/* }}} */

/* {{{ proto string mcal_delete_calendar(int stream_id, int uid)
   Delete event*/
mcal_delete_calendar(){
  return 1;
}
/* }}} */

void php3_mcal_popen(INTERNAL_FUNCTION_PARAMETERS){
}


/* {{{ proto string mcal_store_event(int stream_id, object event)
   Store an  event*/
void php3_mcal_store_event(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*storeobject;
	int ind, ind_type;
	unsigned long i;
	char *t;
	int mcal_folders=0;
	unsigned int msgno;
	pils *mcal_le_struct; 
	pval **pvalue,**temppvalue;
	cal_list_t *my_cal_list;
	int myargc;
	unsigned long uid;
	CALEVENT *myevent;
	myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&streamind,&storeobject) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_array(storeobject);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	myevent=calevent_new();
	if(_php3_hash_find(storeobject->value.ht,"uid",sizeof("uid"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent->id=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"public",sizeof("public"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent->public=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"category",sizeof("category"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent->category=strdup((*pvalue)->value.str.val);
	}
	if(_php3_hash_find(storeobject->value.ht,"title",sizeof("title"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent->title=strdup((*pvalue)->value.str.val);
	}
	if(_php3_hash_find(storeobject->value.ht,"description",sizeof("description"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent->description=strdup((*pvalue)->value.str.val);
	}

	if(_php3_hash_find(storeobject->value.ht,"alarm",sizeof("alarm"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent->alarm=(*pvalue)->value.lval;
	}


       	if(_php3_hash_find(storeobject->value.ht,"start",sizeof("start"),(void **) &temppvalue)== SUCCESS){
          SEPARATE_ZVAL(temppvalue);
	  convert_to_array(*temppvalue);
	  
	  if(_php3_hash_find((*temppvalue)->value.ht,"year",sizeof("year"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.year=(*pvalue)->value.lval;
	  }
	  if(_php3_hash_find((*temppvalue)->value.ht,"month",sizeof("month"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.mon=(*pvalue)->value.lval;
	  }
	  if(_php3_hash_find((*temppvalue)->value.ht,"mday",sizeof("mday"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.mday=(*pvalue)->value.lval;
	  }
	  if(_php3_hash_find((*temppvalue)->value.ht,"hour",sizeof("hour"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.hour=(*pvalue)->value.lval;
	  }
	  if(_php3_hash_find((*temppvalue)->value.ht,"min",sizeof("min"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.min=(*pvalue)->value.lval;
	  }
	  if(_php3_hash_find((*temppvalue)->value.ht,"sec",sizeof("sec"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.sec=(*pvalue)->value.lval;
	  }
	  myevent->start.has_date=true;
	}

       	if(_php3_hash_find(storeobject->value.ht,"end",sizeof("end"),(void **) &temppvalue)== SUCCESS){
          SEPARATE_ZVAL(temppvalue);
	  convert_to_array(*temppvalue);
	  
	  if(_php3_hash_find((*temppvalue)->value.ht,"year",sizeof("year"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.year=(*pvalue)->value.lval;
	  }
	  if(_php3_hash_find((*temppvalue)->value.ht,"month",sizeof("month"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.mon=(*pvalue)->value.lval;
	  }
	  if(_php3_hash_find((*temppvalue)->value.ht,"mday",sizeof("mday"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.mday=(*pvalue)->value.lval;
	  }
	  if(_php3_hash_find((*temppvalue)->value.ht,"hour",sizeof("hour"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.hour=(*pvalue)->value.lval;
	  }
	  if(_php3_hash_find((*temppvalue)->value.ht,"min",sizeof("min"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.min=(*pvalue)->value.lval;
	  }
	  if(_php3_hash_find((*temppvalue)->value.ht,"sec",sizeof("sec"),(void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.sec=(*pvalue)->value.lval;
	  }
	  myevent->end.has_date=true;
	}

	cal_append(mcal_le_struct->mcal_stream,"INBOX",&uid,myevent);
	calevent_free(myevent);
	RETURN_LONG(uid);
}
/* }}} */


/* {{{ proto string mcal_snooze(int stream_id, int uid)
   Snooze an alarm*/
void php3_mcal_snooze(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*uid;
	int ind, ind_type;
	pils *mcal_le_struct; 
	pval **pvalue;
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&streamind,&uid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(uid);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	if(cal_snooze(mcal_le_struct->mcal_stream,uid->value.lval))
	  {
	    RETURN_TRUE;
	  }
	else
	  {
	    RETURN_FALSE;
	  }


}
/* }}} */


/* Interfaces to callbacks */


void cc_searched (unsigned long cal_uid)
{

  if(g_cal_list==NULL)
    {
      g_cal_list=malloc(sizeof(struct cal_list));
      g_cal_list->uid=cal_uid;
      g_cal_list->next=NULL;
      g_cal_list_end=g_cal_list;
    }
  else
    {
      g_cal_list_end->next=malloc(sizeof(struct cal_list));
      g_cal_list_end=g_cal_list_end->next;
      g_cal_list_end->uid=cal_uid;
      g_cal_list_end->next=NULL;
    }
}






void cc_appended(u_int32_t uid)
{

}


void cc_fetched(const CALEVENT *event)
{

}


void cc_login(const char **user, const char **pwd)
{

*user=mcal_user;
*pwd=mcal_password; 
}


void cc_vlog(const char *fmt,va_list ap)
{
}
void cc_vdlog(const char *fmt,va_list ap)
{
}

#endif


/*
 * Local_ variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */


