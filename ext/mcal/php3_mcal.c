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
#ifndef ZEND_VERSION
#include "internal_functions.h"
#endif

#if COMPILE_DL
#include "dl/phpdl.h"
#endif

#if HAVE_MCAL

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#ifndef ZEND_VERSION
#include "php3_list.h"
#endif
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
  long public;
  CALEVENT *event;
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
PHP_FE(mcal_open,NULL)
PHP_FE(mcal_close,NULL)
PHP_FE(mcal_popen,NULL)
PHP_FE(mcal_reopen,NULL)
PHP_FE(mcal_fetch_event,NULL)
PHP_FE(mcal_list_events,NULL)
PHP_FE(mcal_list_alarms,NULL)
PHP_FE(mcal_create_calendar,NULL)
PHP_FE(mcal_rename_calendar,NULL)
PHP_FE(mcal_delete_calendar,NULL)
PHP_FE(mcal_delete_event,NULL)
PHP_FE(mcal_store_event,NULL)
PHP_FE(mcal_snooze,NULL)
PHP_FE(mcal_event_set_category,NULL)
PHP_FE(mcal_event_set_title,NULL)
PHP_FE(mcal_event_set_description,NULL)
PHP_FE(mcal_event_set_start,NULL)
PHP_FE(mcal_event_set_end,NULL)
PHP_FE(mcal_event_set_alarm,NULL)
PHP_FE(mcal_event_set_class,NULL)
PHP_FE(mcal_is_leap_year,NULL)
PHP_FE(mcal_days_in_month,NULL)
PHP_FE(mcal_date_valid,NULL)
PHP_FE(mcal_time_valid,NULL)
PHP_FE(mcal_day_of_week,NULL)
PHP_FE(mcal_day_of_year,NULL)
PHP_FE(mcal_date_compare,NULL)
PHP_FE(mcal_event_init,NULL)
PHP_FE(mcal_next_recurrence,NULL)
PHP_FE(mcal_event_set_recur_daily,NULL)
PHP_FE(mcal_event_set_recur_weekly,NULL)
PHP_FE(mcal_event_set_recur_monthly_mday,NULL)
PHP_FE(mcal_event_set_recur_monthly_wday,NULL)
PHP_FE(mcal_event_set_recur_yearly,NULL)
PHP_FE(mcal_fetch_current_stream_event,NULL)
	{NULL, NULL, NULL}
};

#ifdef ZEND_VERSION
php3_module_entry php3_mcal_module_entry = {
	CALVER, mcal_functions, PHP_MINIT(mcal), NULL, NULL, NULL, PHP_MINFO(mcal), 0, 0, 0, NULL
};
#else
php3_module_entry php3_mcal_module_entry = {"mcal", mcal_functions, PHP_MINIT_FUNCTION, NULL, NULL, NULL, PHP_MINFO_FUNCTION, 0, 0, 0, NULL};
#endif

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &php3_mcal_module_entry; }
#endif

/* 
   I believe since this global is used ONLY within this module,
   and nothing will link to this module, we can use the simple 
   thread local_ storage
*/
int le_mcal;
char mcal_user[80]="";
char mcal_password[80]="";

CALSTREAM *cal_close_it (pils *mcal_le_struct)
{
	CALSTREAM *ret;
	ret = cal_close (mcal_le_struct->mcal_stream,0);
	efree(mcal_le_struct);
	return ret;
}


#ifdef ZEND_VERSION
PHP_MINFO_FUNCTION(mcal)
#else
void PHP_MINFO_FUNCTION(void)
#endif
{
	php3_printf("Mcal Support enabled<br>");
	php3_printf("<table>");
	php3_printf("<tr><td>Mcal Version:</td>");
	php3_printf("<td>%s</td>",CALVER);
	php3_printf("</tr></table>");
}

#ifdef ZEND_VERSION
PHP_MINIT_FUNCTION(mcal)
#else
int PHP_MINIT_FUNCTION(INIT_FUNC_ARGS)
#endif
{

    le_mcal = register_list_destructors(cal_close_it,NULL);

    REGISTER_MAIN_LONG_CONSTANT("MCAL_SUNDAY",SUNDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_MONDAY",MONDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_TUESDAY",TUESDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_WEDNESDAY",WEDNESDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_THURSDAY",THURSDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_FRIDAY",FRIDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_SATURDAY",SATURDAY, CONST_PERSISTENT | CONST_CS);

    REGISTER_MAIN_LONG_CONSTANT("MCAL_JANUARY",JANUARY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_FEBRUARY",FEBRUARY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_MARCH",MARCH, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_APRIL",APRIL, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_MAY",MAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_JUNE",JUNE, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_JULY",JULY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_AUGUST",AUGUST, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_SEPTEMBER",SEPTEMBER, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_OCTOBER",OCTOBER, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_NOVEMBER",NOVEMBER, CONST_PERSISTENT | CONST_CS);


    REGISTER_MAIN_LONG_CONSTANT("MCAL_RECUR_NONE",RECUR_NONE, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_RECUR_DAILY",RECUR_DAILY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_RECUR_WEEKLY",RECUR_WEEKLY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_RECUR_MONTHLY_MDAY",RECUR_MONTHLY_MDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_RECUR_MONTHLY_WDAY",RECUR_MONTHLY_WDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_RECUR_YEARLY",RECUR_YEARLY, CONST_PERSISTENT | CONST_CS);


    REGISTER_MAIN_LONG_CONSTANT("MCAL_M_SUNDAY",M_SUNDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_M_MONDAY",M_MONDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_M_TUESDAY",M_TUESDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_M_WEDNESDAY",M_WEDNESDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_M_THURSDAY",M_THURSDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_M_FRIDAY",M_FRIDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_M_SATURDAY",M_SATURDAY, CONST_PERSISTENT | CONST_CS);

    REGISTER_MAIN_LONG_CONSTANT("MCAL_M_WEEKDAYS",M_WEEKDAYS, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_M_WEEKEND",M_WEEKEND, CONST_PERSISTENT | CONST_CS);
    REGISTER_MAIN_LONG_CONSTANT("MCAL_M_ALLDAYS",M_ALLDAYS, CONST_PERSISTENT | CONST_CS);


    return SUCCESS;
}


static int add_assoc_object(pval *arg, char *key, pval tmp)
{
#ifdef ZEND_VERSION
        HashTable *symtable;
        
        if (arg->type == IS_OBJECT) {
                symtable = arg->value.obj.properties;
        } else {
                symtable = arg->value.ht;
        }
        return zend_hash_update(symtable, key, strlen(key)+1, (void *) &tmp, sizeof(pval *), NULL);
#else
	return _php3_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), NULL);
#endif
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
		php3_printf("option is: %d\n",options->value.lval);
	}
		mcal_stream = cal_open(NULL,calendar->value.str.val,0);
	if (!mcal_stream) {
		php3_error(E_WARNING,"Couldn't open stream %s\n",calendar->value.str.val);
		RETURN_FALSE;
	}

	mcal_le_struct = emalloc(sizeof(pils));
	mcal_le_struct->mcal_stream = mcal_stream;
	mcal_le_struct->event=calevent_new();

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
	CALSTREAM *mcal_stream=NULL;
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
	pval *streamind,*eventid,*options=NULL;
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
	if(myevent == NULL)
	  {
	    RETURN_FALSE;
	  }
	calevent_free(mcal_le_struct->event);
	mcal_le_struct->event=myevent;
	make_event_object(return_value,mcal_le_struct->event);

}
/* }}} */

/* {{{ proto object mcal_fetch_current_stream_event(int stream_id)
   Fetch the current event stored in the stream's event structure*/
void php3_mcal_fetch_current_stream_event(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	int ind, ind_type;
	pils *mcal_le_struct=NULL; 
	int myargcount=ARG_COUNT(ht);
	
	if (myargcount != 1  || getParameters(ht, myargcount, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	ind = streamind->value.lval;
	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
    }
	make_event_object(return_value,mcal_le_struct->event);

}
/* }}} */



void make_event_object(pval *mypvalue,CALEVENT *event)
{
  pval start,end,recurend;
  object_init(mypvalue);
  add_property_long(mypvalue,"id",event->id);
  add_property_long(mypvalue,"public",event->public);
  object_init(&start); 
  
  if(event->start.has_date)
    {
      add_property_long(&start,"year",event->start.year);
      add_property_long(&start,"month",event->start.mon);
      add_property_long(&start,"mday",event->start.mday);
    }
  if(event->start.has_time)
    {
      add_property_long(&start,"hour",event->start.hour);
      add_property_long(&start,"min",event->start.min);
      add_property_long(&start,"sec",event->start.sec);
    }
  
  
  add_assoc_object(mypvalue, "start",start);
  
  object_init(&end);
  if(event->end.has_date)
    {
      add_property_long(&end,"year",event->end.year);
      add_property_long(&end,"month",event->end.mon);
      add_property_long(&end,"mday",event->end.mday);
    }
  if(event->end.has_time)
    {
      add_property_long(&end,"hour",event->end.hour);
      add_property_long(&end,"min",event->end.min);
      add_property_long(&end,"sec",event->end.sec);
    }
  add_assoc_object(mypvalue, "end",end);
  
	
	add_property_string(mypvalue,"category",event->category,1);
	add_property_string(mypvalue,"title",event->title,1);
	add_property_string(mypvalue,"description",event->description,1);
	add_property_long(mypvalue,"alarm",event->alarm);
	add_property_long(mypvalue,"recur_type",event->recur_type);
	add_property_long(mypvalue,"recur_interval",event->recur_interval);
	object_init(&recurend);
  if(event->recur_enddate.has_date)
    {
      add_property_long(&recurend,"year",event->recur_enddate.year);
      add_property_long(&recurend,"month",event->recur_enddate.mon);
      add_property_long(&recurend,"mday",event->recur_enddate.mday);
    }
  if(event->recur_enddate.has_time)
    {
      add_property_long(&recurend,"hour",event->recur_enddate.hour);
      add_property_long(&recurend,"min",event->recur_enddate.min);
      add_property_long(&recurend,"sec",event->recur_enddate.sec);
    }
  add_assoc_object(mypvalue, "recur_enddate",recurend);
  add_property_long(mypvalue,"recur_data",event->recur_data.weekly_wday);	
}



/* {{{ proto array mcal_list_events(int stream_id,object begindate, [object enddate])
   Returns list of UIDs for that day or range of days */
void php3_mcal_list_events(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*startyear,*startmonth,*startday;
	pval *endyear,*endmonth,*endday;
#ifdef ZEND_VERSION
        pval **pvalue;
#else
        pval *pvalue;
#endif
	int ind, ind_type;
	pils *mcal_le_struct; 
	cal_list_t *my_cal_list;
	int myargc;
	datetime_t startdate=DT_INIT;
	datetime_t enddate=DT_INIT;
	myargc=ARG_COUNT(ht);
	if (myargc !=1 || getParameters(ht,myargc,&streamind,&startyear,&startmonth,&startday,&endyear,&endmonth,&endday) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
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
 
	
	if(myargc ==7)
	  {
	    convert_to_long(startyear);
	    convert_to_long(startmonth);
	    convert_to_long(startday);
	    convert_to_long(endyear);
	    convert_to_long(endmonth);
	    convert_to_long(endday);
	    dt_setdate(&startdate,startyear->value.lval,startmonth->value.lval,startday->value.lval);
	    dt_setdate(&enddate,endyear->value.lval,endmonth->value.lval,endday->value.lval);
	  }
	else
	  {
	    startdate=mcal_le_struct->event->start;
	    enddate=mcal_le_struct->event->end;
	  }

	g_cal_list=NULL;
	cal_search_range(mcal_le_struct->mcal_stream,&startdate,&enddate);
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
  pval *streamind, *year,*month,*day,*hour,*min,*sec;
#ifdef ZEND_VERSION
  pval **pvalue;
#else
  pval *pvalue;
#endif
  datetime_t mydate=DT_INIT;
  int ind, ind_type;
  pils *mcal_le_struct; 
  cal_list_t *my_cal_list;
  int myargc=ARG_COUNT(ht);
  if (myargc != 1 || myargc !=7 || getParameters(ht,myargc,&streamind,&year,&month,&day,&hour,&min,&sec) == FAILURE) {
    WRONG_PARAM_COUNT;
	}
  
  convert_to_long(streamind);
  ind = streamind->value.lval;
  
	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);
	if (!mcal_le_struct ) {
	  php3_error(E_WARNING, "Unable to find stream pointer");
	  RETURN_FALSE;
	}
	
        if (array_init(return_value) == FAILURE) {
	  RETURN_FALSE;
        }
	
	if(myargc ==7)
	  {
	    convert_to_long(year);
	    convert_to_long(month);
	    convert_to_long(day);
	    convert_to_long(hour);
	    convert_to_long(min);
	    convert_to_long(sec);
	    dt_setdate(&mydate,year->value.lval,month->value.lval,day->value.lval);
	    dt_setdate(&mydate,hour->value.lval,min->value.lval,sec->value.lval);
	  }
	else
	  {
	    mydate=mcal_le_struct->event->start;
	  }
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
	
	/*	if (mcal_delete_calendar(mcal_le_struct->mcal_stream,calendar->value.str.val)) 
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


void php3_mcal_popen(INTERNAL_FUNCTION_PARAMETERS){
}


/* {{{ proto string mcal_store_event(int stream_id, object event)
   Store an  event*/
void php3_mcal_store_event(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*storeobject;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	unsigned long uid;
	CALEVENT *myevent;
	myargc=ARG_COUNT(ht);
	if (myargc !=1 || getParameters(ht,myargc,&streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);


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
	myevent=mcal_le_struct->event;
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


/* {{{ proto string mcal_event_set_category(int stream_id, string category)
   attach a category to an event*/
void php3_mcal_event_set_category(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*category;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&streamind,&category) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(category);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mcal_le_struct->event->category=strdup(category->value.str.val);
}
	/* }}} */

/* {{{ proto string mcal_event_set_title(int stream_id, string title)
   attach a title to an event*/
void php3_mcal_event_set_title(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*title;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&streamind,&title) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(title);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mcal_le_struct->event->title=strdup(title->value.str.val);
}
	/* }}} */

/* {{{ proto string mcal_event_set_description(int stream_id, string description)
   attach a description to an event*/
void php3_mcal_event_set_description(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*description;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&streamind,&description) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(description);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mcal_le_struct->event->description=strdup(description->value.str.val);
}
	/* }}} */

/* {{{ proto string mcal_event_set_start(int stream_id, int year,int month, int day, [[[int hour],int min],int sec])
   attach a start datetime to an event*/
void php3_mcal_event_set_start(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*year,*month,*date,*hour,*min,*sec;
	int ind, ind_type;
	int myhour=0,mymin=0,mysec=0;
	pils *mcal_le_struct; 
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc <4 || getParameters(ht,myargc,&streamind,&year,&month,&date,&hour,&min,&sec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(year);
	convert_to_long(month);
	convert_to_long(date);
	if(myargc > 4) convert_to_long(hour);
	if(myargc > 5) convert_to_long(min);
	if(myargc > 6) convert_to_long(sec);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}


	dt_setdate(&(mcal_le_struct->event->start),year->value.lval,month->value.lval,date->value.lval);

	if(myargc > 4) myhour=hour->value.lval;
	if(myargc > 5) mymin=min->value.lval;
	if(myargc > 6) mysec=sec->value.lval;
	if(myargc >4) dt_settime(&(mcal_le_struct->event->start),myhour,mymin,mysec);

}
	/* }}} */

/* {{{ proto string mcal_event_set_end(int stream_id, int year,int month, int day, [[[int hour],int min],int sec])
   attach an end datetime to an event*/
void php3_mcal_event_set_end(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*year,*month,*date,*hour,*min,*sec;
	int ind, ind_type;
	int myhour,mymin,mysec;
	pils *mcal_le_struct; 
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc <4 || getParameters(ht,myargc,&streamind,&year,&month,&date,&hour,&min,&sec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(year);
	convert_to_long(month);
	convert_to_long(date);
	if(myargc > 4) convert_to_long(hour);
	if(myargc > 5) convert_to_long(min);
	if(myargc > 6) convert_to_long(sec);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}


	dt_setdate(&(mcal_le_struct->event->end),year->value.lval,month->value.lval,date->value.lval);

	if(myargc > 4) myhour=hour->value.lval;
	if(myargc > 5) mymin=min->value.lval;
	if(myargc > 6) mysec=sec->value.lval;
	if(myargc >4) dt_settime(&(mcal_le_struct->event->end),myhour,mymin,mysec);
}
	/* }}} */

/* {{{ proto int mcal_event_set_alarm(int stream_id, int alarm)
   add an alarm to the streams global event */
void php3_mcal_event_set_alarm(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*alarm;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&streamind,&alarm) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(alarm);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mcal_le_struct->event->alarm=alarm->value.lval;
}
	/* }}} */

/* {{{ proto int mcal_event_init(int stream_id)
   initialize a streams global event */
void php3_mcal_event_init(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=1 || getParameters(ht,myargc,&streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	php3_event_init(mcal_le_struct);
}
	/* }}} */

void php3_event_init(struct php3_mcal_le_struct *mystruct)
{
  calevent_free(mystruct->event);
  mystruct->event=calevent_new();
}


/* {{{ proto int mcal_event_set_class(int stream_id, int class)
   add an class to the streams global event */
void php3_mcal_event_set_class(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*class;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&streamind,&class) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(class);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mcal_le_struct->event->public=class->value.lval;
}
	/* }}} */



/* {{{ proto bool mcal_is_leap_year(int year)
   returns true if year is a leap year, false if not */
void php3_mcal_is_leap_year(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *year;
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=1 || getParameters(ht,myargc,&year) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(year);
	if(isleapyear(year->value.lval)) 
	  {
	    RETURN_TRUE;
	  }
	else 
	  {
	    RETURN_FALSE;
	  }
}
/* }}} */

/* {{{ proto int mcal_days_in_month(int month,bool leap_year)
   returns the number of days in the given month, needs to know if the year is a leap year or not */
void php3_mcal_days_in_month(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *month,*leap;
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=2 || getParameters(ht,myargc,&month,&leap) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(month);
	convert_to_long(leap);
	convert_to_boolean(leap);
	RETURN_LONG(daysinmonth(month->value.lval,leap->value.lval));
}
/* }}} */


/* {{{ proto bool mcal_date_valid(int year,int month,int day)
   returns true if the date is a valid date */
void php3_mcal_date_valid(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *year,*month,*day;
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=3 || getParameters(ht,myargc,&year,&month,&day) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(year);
	convert_to_long(month);
	convert_to_long(day);
	if(datevalid(year->value.lval,month->value.lval,day->value.lval))
	  {
	    RETURN_TRUE;
	  }
	else
	  {
	    RETURN_FALSE;
	  }
}

/* }}} */


/* {{{ proto bool mcal_time_valid(int hour,int min,int sec)
   returns true if the time is a valid time */
void php3_mcal_time_valid(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *hour,*min,*sec;
	int myargc;
	myargc=ARG_COUNT(ht);
	if (myargc !=3 || getParameters(ht,myargc,&hour,&min,&sec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(hour);
	convert_to_long(min);
	convert_to_long(sec);
	if(timevalid(hour->value.lval,min->value.lval,sec->value.lval))
	  {
	    RETURN_TRUE;
	  }
	else
	  {
	    RETURN_FALSE;
	  }
}
/* }}} */

/* {{{ proto int mcal_day_of_week(int year,int month,int day)
   returns the day of the week of the given date */
void php3_mcal_day_of_week(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *year,*month,*day;
	int myargc;
	datetime_t mydate;
	myargc=ARG_COUNT(ht);
	if (myargc !=3 || getParameters(ht,myargc,&year,&month,&day) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(year);
	convert_to_long(month);
	convert_to_long(day);
       dt_init(&mydate);
       dt_setdate(&mydate,year->value.lval,month->value.lval, day->value.lval);

       RETURN_LONG(dt_dayofweek(&mydate));
}
/* }}} */

/* {{{ proto int mcal_day_of_year(int year,int month,int day)
   returns the day of the year of the given date */
void php3_mcal_day_of_year(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *year,*month,*day;
	int myargc;
	datetime_t mydate;
	myargc=ARG_COUNT(ht);
	if (myargc !=3 || getParameters(ht,myargc,&year,&month,&day) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(year);
	convert_to_long(month);
	convert_to_long(day);
       dt_init(&mydate);
       dt_setdate(&mydate,year->value.lval,month->value.lval, day->value.lval);

       RETURN_LONG(dt_dayofyear(&mydate));
}




/* {{{ proto int mcal_day_of_week(int ayear,int amonth,int aday,int byear,int bmonth,int bday)
 returns <0, 0, >0 if a<b, a==b, a>b respectively  */
void php3_mcal_date_compare(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *ayear,*amonth,*aday;
	pval *byear,*bmonth,*bday;
	int myargc;
	datetime_t myadate,mybdate;
	myargc=ARG_COUNT(ht);
	if (myargc !=6 || getParameters(ht,myargc,&ayear,&amonth,&aday,&byear,&bmonth,&bday) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(ayear);
	convert_to_long(amonth);
	convert_to_long(aday);
	convert_to_long(byear);
	convert_to_long(bmonth);
	convert_to_long(bday);
       dt_init(&myadate);
       dt_init(&mybdate);
       dt_setdate(&myadate,ayear->value.lval,amonth->value.lval, aday->value.lval);
       dt_setdate(&mybdate,byear->value.lval,bmonth->value.lval, bday->value.lval);

       RETURN_LONG(dt_compare(&myadate,&mybdate));
}


/* }}} */


/* {{{ proto object mcal_next_recurrence(int stream_id, int weekstart,array next)
   returns an object filled with the next date the event occurs, on or
   after the supplied date.  Returns empty date field if event does not
  occur or something is invalid.
 */
void php3_mcal_next_recurrence(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*weekstart,*next,*pvalue;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t mydate;
	CALEVENT *myevent=NULL;
	myargc=ARG_COUNT(ht);
	if (myargc !=3 || getParameters(ht,myargc,&streamind,&weekstart,&next) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(weekstart);
	convert_to_array(next);
	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	
	if(_php3_hash_find(next->value.ht,"year",sizeof("year"),(void **) &pvalue)== SUCCESS){

	  convert_to_long(pvalue);
	  mydate.year=(pvalue)->value.lval;
	}
	if(_php3_hash_find(next->value.ht,"month",sizeof("month"),(void **) &pvalue)== SUCCESS){

	  convert_to_long(pvalue);
	  mydate.mon=(pvalue)->value.lval;
	}
	if(_php3_hash_find(next->value.ht,"mday",sizeof("mday"),(void **) &pvalue)== SUCCESS){

	  convert_to_long(pvalue);
	  mydate.mday=(pvalue)->value.lval;
	}
	if(_php3_hash_find(next->value.ht,"hour",sizeof("hour"),(void **) &pvalue)== SUCCESS){

	  convert_to_long(pvalue);
	  mydate.hour=(pvalue)->value.lval;
	}
	if(_php3_hash_find(next->value.ht,"min",sizeof("min"),(void **) &pvalue)== SUCCESS){

	  convert_to_long(pvalue);
	  mydate.min=(pvalue)->value.lval;
	}
	if(_php3_hash_find(next->value.ht,"sec",sizeof("sec"),(void **) &pvalue)== SUCCESS){

	  convert_to_long(pvalue);
	  mydate.sec=(pvalue)->value.lval;

	}

	calevent_next_recurrence(mcal_le_struct->event,&mydate,weekstart->value.lval);
	object_init(return_value);
	if(mydate.has_date)
	  {
	    add_property_long(return_value,"year",mydate.year);
	    add_property_long(return_value,"month",mydate.mon);
	    add_property_long(return_value,"mday",mydate.mday);
	  }
	if(mydate.has_time)
	  {
	    add_property_long(return_value,"hour",mydate.hour);
	    add_property_long(return_value,"min",mydate.min);
	    add_property_long(return_value,"sec",mydate.sec);
	  }	
}
	/* }}} */



/* {{{ proto string mcal_event_set_recur_daily(int stream_id,int year,int month,int day,int hour,int min,int sec,int interval
   create a daily recurrence */
void php3_mcal_event_set_recur_daily(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*year,*month,*day,*hour,*min,*sec,*interval;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t endtime=DT_INIT;
	myargc=ARG_COUNT(ht);
	if (myargc !=8 || getParameters(ht,myargc,&streamind,&year,&month,&day,&hour,&min,&sec,&interval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(year);
	convert_to_long(month);
	convert_to_long(day);
	convert_to_long(hour);
	convert_to_long(min);
	convert_to_long(sec);
	convert_to_long(interval);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	dt_setdate(&endtime,year->value.lval,month->value.lval,day->value.lval);
	dt_settime(&endtime,hour->value.lval,min->value.lval,sec->value.lval);
	calevent_recur_daily(mcal_le_struct->event, &endtime,
                                        interval->value.lval);

}
	/* }}} */

/* {{{ proto string mcal_event_set_recur_weekly(int stream_id,int year,int month,int day,int hour,int min,int sec,int interval, int weekdays)
create a weekly recurrence */
void php3_mcal_event_set_recur_weekly(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*year,*month,*day,*hour,*min,*sec,*interval,*weekdays;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t endtime=DT_INIT;
	myargc=ARG_COUNT(ht);
	if (myargc !=4 || getParameters(ht,myargc,&streamind,&year,&month,&day,&hour,&min,&sec,&interval,&weekdays) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(year);
	convert_to_long(month);
	convert_to_long(day);
	convert_to_long(hour);
	convert_to_long(min);
	convert_to_long(sec);
	convert_to_long(interval);
	convert_to_long(weekdays);
	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	dt_setdate(&endtime,year->value.lval,month->value.lval,day->value.lval);
	dt_settime(&endtime,hour->value.lval,min->value.lval,sec->value.lval);
	calevent_recur_weekly(mcal_le_struct->event, &endtime,
                                        interval->value.lval,weekdays->value.lval);

}
	/* }}} */

/* {{{ proto string mcal_event_set_recur_monthly_mday(int stream_id,int year,int month,int day,int hour,int min,int sec,int interval)
 create a monthly by day recurrence */
void php3_mcal_event_set_recur_monthly_mday(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*year,*month,*day,*hour,*min,*sec,*interval;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t endtime=DT_INIT;
	myargc=ARG_COUNT(ht);
	if (myargc !=8 || getParameters(ht,myargc,&streamind,&year,&month,&day,&hour,&min,&sec,&interval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(year);
	convert_to_long(month);
	convert_to_long(day);
	convert_to_long(hour);
	convert_to_long(min);
	convert_to_long(sec);
	convert_to_long(interval);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	dt_setdate(&endtime,year->value.lval,month->value.lval,day->value.lval);
	dt_settime(&endtime,hour->value.lval,min->value.lval,sec->value.lval);
	calevent_recur_monthly_mday(mcal_le_struct->event, &endtime,
                                        interval->value.lval);

}
	/* }}} */

/* {{{ proto string mcal_event_set_recur_monthly_wday(int stream_id,int year,int month,int day,int hour,int min,int sec,int interval)
   create a monthy by week recurrence */
void php3_mcal_event_set_recur_monthly_wday(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*year,*month,*day,*hour,*min,*sec,*interval;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t endtime=DT_INIT;
	myargc=ARG_COUNT(ht);
	if (myargc !=8 || getParameters(ht,myargc,&streamind,&year,&month,&day,&hour,&min,&sec,&interval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(year);
	convert_to_long(month);
	convert_to_long(day);
	convert_to_long(hour);
	convert_to_long(min);
	convert_to_long(sec);
	convert_to_long(interval);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	dt_setdate(&endtime,year->value.lval,month->value.lval,day->value.lval);
	dt_settime(&endtime,hour->value.lval,min->value.lval,sec->value.lval);
	calevent_recur_monthly_wday(mcal_le_struct->event, &endtime,
                                        interval->value.lval);

}
	/* }}} */

/* {{{ proto string mcal_event_set_recur_yearly(int stream_id,int year,int month,int day,int hour,int min,int sec,int interval)
   create a yearly recurrence */
void php3_mcal_event_set_recur_yearly(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*year,*month,*day,*hour,*min,*sec,*interval;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t endtime=DT_INIT;
	myargc=ARG_COUNT(ht);
	if (myargc !=8 || getParameters(ht,myargc,&streamind,&year,&month,&day,&hour,&min,&sec,&interval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(year);
	convert_to_long(month);
	convert_to_long(day);
	convert_to_long(hour);
	convert_to_long(min);
	convert_to_long(sec);
	convert_to_long(interval);

	ind = streamind->value.lval;

	mcal_le_struct = (pils *)php3_list_find(ind, &ind_type);

	if (!mcal_le_struct ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	dt_setdate(&endtime,year->value.lval,month->value.lval,day->value.lval);
	dt_settime(&endtime,hour->value.lval,min->value.lval,sec->value.lval);
	calevent_recur_yearly(mcal_le_struct->event, &endtime,
                                        interval->value.lval);

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


