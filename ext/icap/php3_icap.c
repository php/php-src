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

#define ICAP1

#ifdef ERROR
#undef ERROR
#endif

#include "php.h"

#if COMPILE_DL
#include "dl/phpdl.h"
#endif

#if HAVE_ICAP

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#include "icap.h"
#include "icapclient.h"
#include "php3_icap.h"
#include "modules.h"
#if (WIN32|WINNT)
#include "winsock.h"
#endif
event_t global_event;
CALSTREAM *cal_open();
CALSTREAM *cal_close_it ();
CALSTREAM *cal_close_full ();


typedef struct php3_icap_le_struct {
	CALSTREAM *icap_stream;
	long flags;
} pils;


typedef struct cal_list
{
u_int32_t uid;
struct cal_list *next;
} cal_list_t;

static cal_list_t *g_cal_list=NULL;
static cal_list_t *g_cal_list_end=NULL;
static u_int32_t guid;
/* 
 * this array should be set up as:
 * {"PHPScriptFunctionName",dllFunctionName,1} 
 */

function_entry icap_functions[] = {
	{"icap_open", php3_icap_open, NULL},
	{"icap_popen", php3_icap_popen, NULL},
	{"icap_reopen", php3_icap_reopen, NULL},
	{"icap_fetch_event", php3_icap_fetch_event, NULL},
	{"icap_list_events", php3_icap_list_events, NULL},
	{"icap_list_alarms", php3_icap_list_alarms, NULL},
	{"icap_create_calendar", php3_icap_create_calendar, NULL},
	{"icap_rename_calendar", php3_icap_rename_calendar, NULL},
	{"icap_delete_calendar", php3_icap_delete_calendar, NULL},
	{"icap_delete_event", php3_icap_delete_event, NULL},
	{"icap_store_event", php3_icap_store_event, NULL},
	{"icap_snooze", php3_icap_snooze, NULL},
	{NULL, NULL, NULL}
};


php3_module_entry php3_icap_module_entry = {
	ICAPVER, icap_functions, icap_init, NULL, NULL, NULL, icap_info, 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &php3_icap_module_entry; }
#endif

/* 
   I believe since this global is used ONLY within this module,
   and nothing will link to this module, we can use the simple 
   thread local_ storage
*/
int le_icap;
#ifdef OP_RELOGIN
/* AJS: persistent connection type */
int le_picap;
#endif
char icap_user[80]="";
char icap_password[80]="";

CALSTREAM *cal_close_it (pils *icap_le_struct)
{
	CALSTREAM *ret;
	ret = cal_logout (icap_le_struct->icap_stream,icap_le_struct->flags);
	event_destroy(&global_event);
	efree(icap_le_struct);
	return ret;
}


void icap_info(void)
{
	php3_printf("Icap Support enabled<br>");
	php3_printf("<table>");
	php3_printf("<tr><td>Icap Version:</td>");
	php3_printf("<td>%s</td>",ICAPVER);
	php3_printf("</tr></table>");
}

int icap_init(INIT_FUNC_ARGS)
{

    le_icap = register_list_destructors(cal_close_it,NULL);
	REGISTER_MAIN_LONG_CONSTANT("CLASS_PRIVATE", CLASS_PRIVATE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("CLASS_PUBLIC", CLASS_PUBLIC, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("CLASS_CONFIDENTIAL", CLASS_CONFIDENTIAL, CONST_PERSISTENT | CONST_CS);

    event_init(&global_event);
	return SUCCESS;
}

void php3_icap_do_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	pval *calendar;
	pval *user;
	pval *passwd;
	pval *options;
	CALSTREAM *icap_stream;
	pils *icap_le_struct;
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
	strcpy(icap_user,user->value.str.val);
	strcpy(icap_password,passwd->value.str.val);
	if(myargc ==4) {
		convert_to_long(options);
		flags=options->value.lval;
	}
		icap_stream = cal_connect(calendar->value.str.val);
	if (!icap_stream) {
		php3_error(E_WARNING,"Couldn't open stream %s\n",calendar->value.str.val);
		RETURN_FALSE;
	}
	cal_login(icap_stream,user->value.str.val,passwd->value.str.val);
	cal_select(icap_stream,"INBOX");
	icap_le_struct = emalloc(sizeof(pils));
	icap_le_struct->icap_stream = icap_stream;
	icap_le_struct->flags = 0;	
	ind = php3_list_insert(icap_le_struct, le_icap);	
	RETURN_LONG(ind);
}




/* {{{ proto int icap_close(int stream_id [, int options])
   Close an ICAP stream */
void php3_icap_close(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *options, *streamind;
        int ind, ind_type;
        pils *icap_le_struct=NULL; 
        int myargcount=ARG_COUNT(ht);
        long flags = 0;

        if (myargcount < 1 || myargcount > 2 || getParameters(ht, myargcount, &streamind, &options) == FAILURE) {
                WRONG_PARAM_COUNT;
        }
        convert_to_long(streamind);
        ind = streamind->value.lval;
        icap_le_struct = (pils *)php3_list_find(ind, &ind_type);
        if (!icap_le_struct ) {
	  php3_error(E_WARNING, "Unable to find stream pointer");
                RETURN_FALSE;
	}
        if(myargcount==2) {
                convert_to_long(options);
                flags = options->value.lval;
                icap_le_struct->flags = flags;
        }
        php3_list_delete(ind);
        RETURN_TRUE;
}
/* }}} */








/* {{{ proto int icap_open(string calendar, string user, string password [, int options])
   Open an ICAP stream to a calendar */
void php3_icap_open(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_icap_do_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ proto int icap_reopen(int stream_id, string calendar [, int options])
   Reopen ICAP stream to new calendar */
void php3_icap_reopen(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	pval *calendar;
	pval *options;
	CALSTREAM *icap_stream;
	pils *icap_le_struct; 
	int ind, ind_type;
	long flags=0;
	long cl_flags=0;
	int myargc=ARG_COUNT(ht);

	if (myargc<2 || myargc>3 || getParameters(ht,myargc,&streamind, &calendar, &options) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

	convert_to_long(streamind);
	ind = streamind->value.lval;
	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	convert_to_string(calendar);
	if(myargc == 3) {
		convert_to_long(options);
		flags = options->value.lval;
		icap_le_struct->flags = cl_flags;	
	}
	icap_stream = cal_connect(calendar->value.str.val);
	//	cal_login(icap_stream, calendar->value.str.val);
	if (icap_stream == NULL) {
		php3_error(E_WARNING,"Couldn't re-open stream\n");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int icap_expunge(int stream_id)
   Delete all messages marked for deletion */
void php3_icap_expunge(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	int ind, ind_type;
	pils *icap_le_struct; 

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	cal_expunge (icap_le_struct->icap_stream);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int icap_fetch_event(int stream_id,int eventid, [int options])
   Fetch an event*/
void php3_icap_fetch_event(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*eventid,*options=NULL;
	int ind, ind_type;
	pils *icap_le_struct=NULL; 
	event_t *myevent=NULL;
	int myargcount=ARG_COUNT(ht);
	
	if (myargcount < 1 || myargcount > 3 || getParameters(ht, myargcount, &streamind, &eventid,&options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	convert_to_long(eventid);
	ind = streamind->value.lval;
	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
    }
	if(myargcount==3) {
		convert_to_long(options);
	}
	cal_fetch(icap_le_struct->icap_stream,eventid->value.lval);

	object_init(return_value);
	add_property_long(return_value,"uid",global_event.uid);
	add_property_long(return_value,"year",global_event.date.year);
	add_property_long(return_value,"month",global_event.date.month);
	add_property_long(return_value,"day",global_event.date.mday);
	add_property_long(return_value,"starthour",global_event.start.hour);
	add_property_long(return_value,"startminute",global_event.start.minute);
	add_property_long(return_value,"endhour",global_event.end.hour);
	add_property_long(return_value,"endminute",global_event.end.minute);
	add_property_string(return_value,"category",global_event.category,1);
	add_property_string(return_value,"title",global_event.title,1);
	add_property_string(return_value,"description",global_event.description,1);
	add_property_long(return_value,"class",global_event.ical_class);
	add_property_long(return_value,"alarm_year",global_event.alarm_last.year);
	add_property_long(return_value,"alarm_month",global_event.alarm_last.month);
	add_property_long(return_value,"alarm_day",global_event.alarm_last.mday);
	add_property_long(return_value,"alarm",global_event.alarm);
}
/* }}} */

/* {{{ proto array icap_list_events(int stream_id,int begindate, [int enddate])
   Returns list of UIDs for that day or range of days */
void php3_icap_list_events(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*begindate,*enddate;
        pval **pvalue;
	int ind, ind_type;
	unsigned long i;
	char *t;
	int icap_folders=0;
	unsigned int msgno;
	pils *icap_le_struct; 
	cal_list_t *my_cal_list;
	caldate_t begincal,endcal;
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc <2 || myargc > 3 || getParameters(ht,myargc,&streamind,&begindate,&enddate) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_array(begindate);
	if(myargc == 3) convert_to_array(enddate);
	ind = streamind->value.lval;

	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

       if(_php3_hash_find(begindate->value.ht,"year",sizeof("year"),(void **) &pvalue)== SUCCESS){
	 SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          begincal.year=(*pvalue)->value.lval;
       }
       if(_php3_hash_find(begindate->value.ht,"month",sizeof("month"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          begincal.month=(*pvalue)->value.lval;
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
      endcal.month=(*pvalue)->value.lval;
    }
    if(_php3_hash_find(enddate->value.ht,"day",sizeof("day"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
      convert_to_long(*pvalue);
      endcal.mday=(*pvalue)->value.lval;
    }
  }
 
 
 cal_search(icap_le_struct->icap_stream,&begincal,(myargc == 3) ? NULL : &endcal);
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


/* {{{ proto string icap_create_calendar(int stream_id, string calendar)
   Create a new calendar*/

void php3_icap_create_calendar(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *calendar;
	int ind, ind_type;
	pils *icap_le_struct; 
	int myargc=ARG_COUNT(ht);
	if (myargc <1 || myargc > 2 || getParameters(ht,myargc,&streamind,&calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(calendar);
	ind = streamind->value.lval;

	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (icap_create(icap_le_struct->icap_stream,calendar->value.str.val)) 
	  {
	    RETURN_TRUE;
	  }
	else 
	  {
	    RETURN_FALSE;
	  }
}
/* }}} */


/* {{{ proto string icap_rename(int stream_id, string src_calendar, string dest_calendar)
   Rename a calendar*/
void php3_icap_rename_calendar(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *src_calendar,*dest_calendar;
	int ind, ind_type;
	pils *icap_le_struct; 
	int myargc=ARG_COUNT(ht);
	if (myargc <2 || myargc > 3 || getParameters(ht,myargc,&streamind,&src_calendar,&dest_calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(src_calendar);
	convert_to_string(dest_calendar);
	ind = streamind->value.lval;

	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if(icap_rename(icap_le_struct->icap_stream,src_calendar->value.str.val,dest_calendar->value.str.val)) {RETURN_TRUE;}
	else {RETURN_FALSE; }
}
/* }}} */




/* {{{ proto int icap_reopen(int stream_id, array date, array time)
   list alarms for a given time */
void php3_icap_list_alarms(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *date,*time;
        pval **pvalue;
	caldate_t mydate;
	caltime_t mytime;
	int ind, ind_type;
	pils *icap_le_struct; 
        int icap_folders=0;
        unsigned int msgno;
        cal_list_t *my_cal_list;
        caldate_t begincal,endcal;

	int myargc=ARG_COUNT(ht);
	if (myargc != 3 || getParameters(ht,myargc,&streamind,&date,&time) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_array(date);
	convert_to_array(time);
	ind = streamind->value.lval;

	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

       if(_php3_hash_find(date->value.ht,"year",sizeof("year"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.year=(*pvalue)->value.lval;
       }
       if(_php3_hash_find(date->value.ht,"month",sizeof("month"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.month=(*pvalue)->value.lval;
       }
       if(_php3_hash_find(date->value.ht,"day",sizeof("day"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.mday=(*pvalue)->value.lval;
       }

       if(_php3_hash_find(time->value.ht,"hour",sizeof("hour"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mytime.hour=(*pvalue)->value.lval;
       }
       if(_php3_hash_find(time->value.ht,"minute",sizeof("minute"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mytime.minute=(*pvalue)->value.lval;
       }

       cal_search_alarming(icap_le_struct->icap_stream,&mydate,&mytime);
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


/* {{{ proto string icap_delete_calendar(int stream_id, string calendar)
   Delete calendar*/
void php3_icap_delete_calendar(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *calendar;
	int ind, ind_type;
	pils *icap_le_struct; 
	int myargc=ARG_COUNT(ht);
	if (myargc <1 || myargc > 2 || getParameters(ht,myargc,&streamind,&calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(calendar);
	ind = streamind->value.lval;

	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (icap_delete_calendar(icap_le_struct->icap_stream,calendar->value.str.val)) 
	  {
	    RETURN_TRUE;
	  }
	else 
	  {
	    RETURN_FALSE;
	  }
}
/* }}} */


/* {{{ proto string icap_delete_event(int stream_id, int uid)
   Delete event*/
void php3_icap_delete_event(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *uid;
	int ind, ind_type;
	pils *icap_le_struct; 
	int myargc=ARG_COUNT(ht);
	if (myargc <1 || myargc > 2 || getParameters(ht,myargc,&streamind,&uid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(uid);
	ind = streamind->value.lval;

	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (cal_remove(icap_le_struct->icap_stream,uid->value.lval)) 
	  {
	    RETURN_TRUE;
	  }
	else 
	  {
	    RETURN_FALSE;
	  }
}
/* }}} */

/* {{{ proto string icap_delete_calendar(int stream_id, int uid)
   Delete event*/
icap_delete_calendar(){
  return 1;
}
/* }}} */

void php3_icap_popen(INTERNAL_FUNCTION_PARAMETERS){
}


/* {{{ proto string icap_store_event(int stream_id, object event)
   Store an  event*/
void php3_icap_store_event(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*storeobject;
	int ind, ind_type;
	unsigned long i;
	char *t;
	int icap_folders=0;
	unsigned int msgno;
	pils *icap_le_struct; 
	pval **pvalue;
	cal_list_t *my_cal_list;
	int myargc;
	int uid;
	event_t myevent;
	myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&streamind,&storeobject) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_array(storeobject);

	ind = streamind->value.lval;

	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	event_init(&myevent);
	if(_php3_hash_find(storeobject->value.ht,"uid",sizeof("uid"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.uid=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"year",sizeof("year"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.date.year=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"month",sizeof("month"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.date.month=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"day",sizeof("day"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.date.mday=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"starthour",sizeof("starthour"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.start.hour=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"startminute",sizeof("startminute"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.start.minute=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"endhour",sizeof("endhour"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.end.hour=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"endminute",sizeof("endminute"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.end.minute=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"category",sizeof("category"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent.category=strdup((*pvalue)->value.str.val);
	}
	if(_php3_hash_find(storeobject->value.ht,"title",sizeof("title"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent.title=strdup((*pvalue)->value.str.val);
	}
	if(_php3_hash_find(storeobject->value.ht,"description",sizeof("description"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent.description=strdup((*pvalue)->value.str.val);
	}

	if(_php3_hash_find(storeobject->value.ht,"alarm",sizeof("alarm"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.alarm=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"alarm_year",sizeof("alarm_year"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.alarm_last.year=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"alarm_month",sizeof("alarm_month"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent.alarm_last.month=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"alarm_mday",sizeof("alarm_day"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent.alarm_last.mday=(*pvalue)->value.lval;
	}
	if(_php3_hash_find(storeobject->value.ht,"class",sizeof("class"),(void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent.ical_class=(*pvalue)->value.lval;
	}



	cal_append(icap_le_struct->icap_stream,"INBOX",&myevent);
	event_destroy(&myevent);
	RETURN_LONG(guid);
}
/* }}} */


/* {{{ proto string icap_snooze(int stream_id, int uid)
   Snooze an alarm*/
void php3_icap_snooze(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*uid;
	int ind, ind_type;
	pils *icap_le_struct; 
	pval **pvalue;
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&streamind,&uid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(uid);

	ind = streamind->value.lval;

	icap_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!icap_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	if(cal_snooze(icap_le_struct->icap_stream,uid->value.lval))
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


void cc_searched (u_int32_t cal_uid)
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
guid=uid;
}


void cc_fetched(const event_t *event)
{
event_copy(&global_event,event);

}

void cc_log(const char *fmt, ...)
{

}

void cc_dlog(const char *fmt, ...)
{

}

#endif

/*
 * Local_ variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */


