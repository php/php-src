/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Mark Musone <musone@chek.com>                               |
   |          Chuck Hagenbuch <chuck@horde.org>                           |
   +----------------------------------------------------------------------+
*/

#define MCAL1

#ifdef ERROR
#undef ERROR
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_MCAL

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#include "mcal.h"
#include "php_mcal.h"
#include "zend_modules.h"
#include "ext/standard/info.h"
#include "ext/standard/basic_functions.h"
#ifdef PHP_WIN32
#include "winsock.h"
#endif

static void cal_close_it(zend_rsrc_list_entry *rsrc TSRMLS_DC);

typedef struct _php_mcal_le_struct {
	CALSTREAM *mcal_stream;
	long public;
	CALEVENT *event;
	long flags;
} pils;


typedef struct cal_list {
	php_uint32 uid;
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
	PHP_FE(mcal_append_event,NULL)
	PHP_FE(mcal_store_event,NULL)
	PHP_FE(mcal_snooze,NULL)
	PHP_FE(mcal_event_set_category,NULL)
	PHP_FE(mcal_event_set_title,NULL)
	PHP_FE(mcal_event_set_description,NULL)
	PHP_FE(mcal_event_set_start,NULL)
	PHP_FE(mcal_event_set_end,NULL)
	PHP_FE(mcal_event_set_alarm,NULL)
	PHP_FE(mcal_event_set_class,NULL)
	PHP_FE(mcal_event_add_attribute,NULL)
	PHP_FE(mcal_is_leap_year,NULL)
	PHP_FE(mcal_days_in_month,NULL)
	PHP_FE(mcal_date_valid,NULL)
	PHP_FE(mcal_time_valid,NULL)
	PHP_FE(mcal_day_of_week,NULL)
	PHP_FE(mcal_day_of_year,NULL)
	PHP_FE(mcal_week_of_year,NULL)
	PHP_FE(mcal_date_compare,NULL)
	PHP_FE(mcal_event_init,NULL)
	PHP_FE(mcal_next_recurrence,NULL)
	PHP_FE(mcal_event_set_recur_none,NULL)
	PHP_FE(mcal_event_set_recur_daily,NULL)
	PHP_FE(mcal_event_set_recur_weekly,NULL)
	PHP_FE(mcal_event_set_recur_monthly_mday,NULL)
	PHP_FE(mcal_event_set_recur_monthly_wday,NULL)
	PHP_FE(mcal_event_set_recur_yearly,NULL)
	PHP_FE(mcal_fetch_current_stream_event,NULL)
	{NULL, NULL, NULL}
};

zend_module_entry php_mcal_module_entry = {
	STANDARD_MODULE_HEADER,
	"mcal",
        mcal_functions,
        PHP_MINIT(mcal),
        NULL,
        NULL,
        NULL,
        PHP_MINFO(mcal),
        NO_VERSION_YET,
        STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MCAL
ZEND_GET_MODULE(php_mcal)
#endif

/* 
   I believe since this global is used ONLY within this module,
   and nothing will link to this module, we can use the simple 
   thread local_ storage
*/
static int le_mcal;
char *mcal_user;
char *mcal_password;


static void cal_close_it (zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	pils *mcal_le_struct = (pils *)rsrc->ptr;

	cal_close(mcal_le_struct->mcal_stream,0);
	efree(mcal_le_struct);
}


PHP_MINFO_FUNCTION(mcal)
{
	char tmp[128];

	php_info_print_table_start();
	php_info_print_table_row(2, "MCAL Support", "enabled" );
#ifdef MCALVER
	snprintf(tmp, 128, "%s - %d", CALVER, MCALVER);
#else
	snprintf(tmp, 128, "%s", CALVER );
#endif
        php_info_print_table_row(2, "MCAL Version", tmp );
        php_info_print_table_end();
}

PHP_MINIT_FUNCTION(mcal)
{
    le_mcal = zend_register_list_destructors_ex(cal_close_it, NULL, "mcal", module_number);

    REGISTER_LONG_CONSTANT("MCAL_SUNDAY", SUNDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_MONDAY", MONDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_TUESDAY", TUESDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_WEDNESDAY", WEDNESDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_THURSDAY", THURSDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_FRIDAY", FRIDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_SATURDAY", SATURDAY, CONST_PERSISTENT | CONST_CS);

    REGISTER_LONG_CONSTANT("MCAL_JANUARY", JANUARY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_FEBRUARY", FEBRUARY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_MARCH", MARCH, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_APRIL", APRIL, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_MAY", MAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_JUNE", JUNE, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_JULY", JULY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_AUGUST", AUGUST, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_SEPTEMBER", SEPTEMBER, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_OCTOBER", OCTOBER, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_NOVEMBER", NOVEMBER, CONST_PERSISTENT | CONST_CS);

    REGISTER_LONG_CONSTANT("MCAL_RECUR_NONE", RECUR_NONE, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_RECUR_DAILY", RECUR_DAILY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_RECUR_WEEKLY", RECUR_WEEKLY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_RECUR_MONTHLY_MDAY", RECUR_MONTHLY_MDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_RECUR_MONTHLY_WDAY", RECUR_MONTHLY_WDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_RECUR_YEARLY", RECUR_YEARLY, CONST_PERSISTENT | CONST_CS);

    REGISTER_LONG_CONSTANT("MCAL_M_SUNDAY", M_SUNDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_M_MONDAY", M_MONDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_M_TUESDAY", M_TUESDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_M_WEDNESDAY", M_WEDNESDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_M_THURSDAY", M_THURSDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_M_FRIDAY", M_FRIDAY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_M_SATURDAY", M_SATURDAY, CONST_PERSISTENT | CONST_CS);

    REGISTER_LONG_CONSTANT("MCAL_M_WEEKDAYS", M_WEEKDAYS, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_M_WEEKEND", M_WEEKEND, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("MCAL_M_ALLDAYS", M_ALLDAYS, CONST_PERSISTENT | CONST_CS);

    return SUCCESS;
}

static int add_assoc_object(zval *arg, char *key, zval *tmp)
{
	HashTable *symtable;
	
	if (Z_TYPE_P(arg) == IS_OBJECT) {
		symtable = Z_OBJPROP_P(arg);
	} else {
		symtable = Z_ARRVAL_P(arg);
	}
	return zend_hash_update(symtable, key, strlen(key)+1, (void *)&tmp, sizeof(zval *), NULL);
}

static void php_mcal_do_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	zval **calendar, **user, **passwd, **options;
	CALSTREAM *mcal_stream;
	pils *mcal_le_struct;
	long flags=0;
	int ind;
	
	int myargc = ZEND_NUM_ARGS();
	if (myargc < 3 || myargc > 4 || zend_get_parameters_ex(myargc, &calendar, &user, &passwd, &options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(calendar);
	convert_to_string_ex(user);
	convert_to_string_ex(passwd);
	mcal_user = estrndup(Z_STRVAL_PP(user), Z_STRLEN_PP(user));
	mcal_password = estrndup(Z_STRVAL_PP(passwd), Z_STRLEN_PP(passwd));
	if (myargc == 4) {
		convert_to_long_ex(options);
		flags = Z_LVAL_PP(options);
	}
	mcal_stream = cal_open(NULL, Z_STRVAL_PP(calendar), 0);
	efree(mcal_user);
	efree(mcal_password);
	
	if (!mcal_stream) {
		php_error(E_WARNING, "Couldn't open stream %s\n", Z_STRVAL_PP(calendar));
		RETURN_FALSE;
	}
	
	mcal_le_struct = emalloc(sizeof(pils));
	mcal_le_struct->mcal_stream = mcal_stream;
	mcal_le_struct->event=calevent_new();
	
	ind = zend_list_insert(mcal_le_struct, le_mcal);	
	RETURN_LONG(ind);
}

static void php_mcal_event_init(struct _php_mcal_le_struct *mystruct)
{
	calevent_free(mystruct->event);
	mystruct->event=calevent_new();
}

static void _php_make_event_object(zval *myzvalue, CALEVENT *event TSRMLS_DC)
{
	zval *start, *end, *recurend, *attrlist;
	CALATTR *attr;
	
	object_init(myzvalue);
	add_property_long(myzvalue,"id",event->id);
	add_property_long(myzvalue,"public",event->public);
	
	MAKE_STD_ZVAL(start);
	object_init(start); 
	if (event->start.has_date) {
		add_property_long(start,"year",event->start.year);
		add_property_long(start,"month",event->start.mon);
		add_property_long(start,"mday",event->start.mday);
	}
	if (event->start.has_time) {
		add_property_long(start,"hour",event->start.hour);
		add_property_long(start,"min",event->start.min);
		add_property_long(start,"sec",event->start.sec);
	}
	add_assoc_object(myzvalue, "start", start);
	
	MAKE_STD_ZVAL(end);
	object_init(end);
	if (event->end.has_date) {
		add_property_long(end,"year",event->end.year);
		add_property_long(end,"month",event->end.mon);
		add_property_long(end,"mday",event->end.mday);
	}
	if (event->end.has_time) {
		add_property_long(end,"hour",event->end.hour);
		add_property_long(end,"min",event->end.min);
		add_property_long(end,"sec",event->end.sec);
	}
	add_assoc_object(myzvalue, "end", end);
	
	if (event->category)
		add_property_string(myzvalue,"category",event->category,1);
	if (event->title)
		add_property_string(myzvalue,"title",event->title,1);
	if (event->description)
		add_property_string(myzvalue,"description",event->description,1);
	add_property_long(myzvalue,"alarm",event->alarm);
	add_property_long(myzvalue,"recur_type",event->recur_type);
	add_property_long(myzvalue,"recur_interval",event->recur_interval);
	
	MAKE_STD_ZVAL(recurend);
	object_init(recurend);
	if (event->recur_enddate.has_date) {
		add_property_long(recurend,"year",event->recur_enddate.year);
		add_property_long(recurend,"month",event->recur_enddate.mon);
		add_property_long(recurend,"mday",event->recur_enddate.mday);
	}
	if (event->recur_enddate.has_time) {
		add_property_long(recurend,"hour",event->recur_enddate.hour);
		add_property_long(recurend,"min",event->recur_enddate.min);
		add_property_long(recurend,"sec",event->recur_enddate.sec);
	}
	add_assoc_object(myzvalue, "recur_enddate", recurend);
	
	add_property_long(myzvalue,"recur_data",event->recur_data.weekly_wday);	
	
	if (event->attrlist) {
		MAKE_STD_ZVAL(attrlist);
		object_init(attrlist);
		array_init(attrlist);
		for (attr = event->attrlist; attr; attr = attr->next) {
			add_assoc_string(attrlist, attr->name, attr->value, 1);
		}
		add_assoc_object(myzvalue, "attrlist", attrlist);
	}
}

/* {{{ proto int mcal_close(int stream_id [, int options])
   Close an MCAL stream */
PHP_FUNCTION(mcal_close)
{
	zval **options, **streamind;
	int ind, ind_type;
	pils *mcal_le_struct=NULL; 
	int myargcount=ZEND_NUM_ARGS();
	long flags = 0;
	
	if (myargcount < 1 || myargcount > 2 || zend_get_parameters_ex(myargcount, &streamind, &options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (myargcount==2) {
		convert_to_long_ex(options);
		flags = Z_LVAL_PP(options);
		mcal_le_struct->flags = flags;
	}
	zend_list_delete(ind);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mcal_open(string calendar, string user, string password [, int options])
   Open an MCAL stream to a calendar */
PHP_FUNCTION(mcal_open)
{
	php_mcal_do_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string mcal_popen(string calendar, string user, string password [, int options])
   Open a persistent MCAL stream to a calendar */
PHP_FUNCTION(mcal_popen)
{
	php_mcal_do_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

/* {{{ proto int mcal_reopen(int stream_id, string calendar [, int options])
   Reopen MCAL stream to a new calendar */
PHP_FUNCTION(mcal_reopen)
{
	zval **streamind, **calendar, **options;
	CALSTREAM *mcal_stream=NULL;
	pils *mcal_le_struct; 
	int ind, ind_type;
	long flags=0;
	long cl_flags=0;
	int myargc=ZEND_NUM_ARGS();
	
	if (myargc < 2 || myargc > 3 || zend_get_parameters_ex(myargc, &streamind, &calendar, &options) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
	
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	convert_to_string_ex(calendar);
	if (myargc == 3) {
		convert_to_long_ex(options);
		flags = Z_LVAL_PP(options);
		mcal_le_struct->flags = cl_flags;	
	}
	if (mcal_stream == NULL) {
		php_error(E_WARNING,"Couldn't re-open stream\n");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mcal_expunge(int stream_id)
   Delete all events marked for deletion */
PHP_FUNCTION(mcal_expunge)
{
	zval **streamind;
	int ind, ind_type;
	pils *mcal_le_struct; 
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	/*	cal_expunge (mcal_le_struct->mcal_stream);
	 */
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mcal_fetch_event(int stream_id, int eventid [, int options])
   Fetch an event */
PHP_FUNCTION(mcal_fetch_event)
{
	zval **streamind, **eventid, **options=NULL;
	int ind, ind_type;
	pils *mcal_le_struct=NULL; 
	CALEVENT *myevent;
	int myargcount=ZEND_NUM_ARGS();
	
	if (myargcount < 1 || myargcount > 3 || zend_get_parameters_ex(myargcount, &streamind, &eventid, &options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(streamind);
	convert_to_long_ex(eventid);
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if (myargcount == 3) {
		convert_to_long_ex(options);
	}
	cal_fetch(mcal_le_struct->mcal_stream, Z_LVAL_PP(eventid), &myevent);
	if (myevent == NULL) {
	    RETURN_FALSE;
	}
	calevent_free(mcal_le_struct->event);
	mcal_le_struct->event = myevent;
	_php_make_event_object(return_value, mcal_le_struct->event TSRMLS_CC);
}
/* }}} */

/* {{{ proto object mcal_fetch_current_stream_event(int stream_id)
   Fetch the current event stored in the stream's event structure */
PHP_FUNCTION(mcal_fetch_current_stream_event)
{
	zval **streamind;
	int ind, ind_type;
	pils *mcal_le_struct=NULL; 
	int myargcount=ZEND_NUM_ARGS();
	
	if (myargcount != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
    }
	_php_make_event_object(return_value, mcal_le_struct->event TSRMLS_CC);
}
/* }}} */

/* {{{ proto array mcal_list_events(int stream_id, object begindate [, object enddate])
   Returns list of UIDs for that day or range of days */
PHP_FUNCTION(mcal_list_events)
{
	zval **streamind, **startyear, **startmonth, **startday;
	zval **endyear, **endmonth, **endday;
	int ind, ind_type;
	pils *mcal_le_struct; 
	cal_list_t *my_cal_list;
	int myargc;
	datetime_t startdate=DT_INIT;
	datetime_t enddate=DT_INIT;
	
	myargc = ZEND_NUM_ARGS();
	if ((myargc != 1 && myargc != 7) || zend_get_parameters_ex(myargc, &streamind, &startyear, &startmonth, &startday, &endyear, &endmonth, &endday) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	/* Initialize return array */
	array_init(return_value);
	
	if (myargc == 7) {
		convert_to_long_ex(startyear);
		convert_to_long_ex(startmonth);
		convert_to_long_ex(startday);
		convert_to_long_ex(endyear);
		convert_to_long_ex(endmonth);
		convert_to_long_ex(endday);
		dt_setdate(&startdate, Z_LVAL_PP(startyear), Z_LVAL_PP(startmonth), Z_LVAL_PP(startday));
		dt_setdate(&enddate, Z_LVAL_PP(endyear), Z_LVAL_PP(endmonth), Z_LVAL_PP(endday));
	}
	else {
		startdate = mcal_le_struct->event->start;
		enddate = mcal_le_struct->event->end;
	}
	
	g_cal_list = NULL;
	cal_search_range(mcal_le_struct->mcal_stream, &startdate, &enddate);
	my_cal_list = g_cal_list;
	while (my_cal_list != NULL) {
		add_next_index_long(return_value, my_cal_list->uid);
		my_cal_list = my_cal_list->next;
		free(g_cal_list);
		g_cal_list = my_cal_list;
	}
}
/* }}} */

/* {{{ proto string mcal_create_calendar(int stream_id, string calendar)
   Create a new calendar */
PHP_FUNCTION(mcal_create_calendar)
{
	zval **streamind, **calendar;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc = ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(2, &streamind, &calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(calendar);
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	/*
	  if (mcal_create(mcal_le_struct->mcal_stream,Z_STRVAL_P(calendar))) 
	  {
	  RETURN_TRUE;
	  }
	  else 
	  {
	  RETURN_FALSE;
	  }
	*/
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mcal_rename_calendar(int stream_id, string src_calendar, string dest_calendar)
   Rename a calendar */
PHP_FUNCTION(mcal_rename_calendar)
{
	zval **streamind, **src_calendar, **dest_calendar;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc=ZEND_NUM_ARGS();
	if (myargc != 3 || zend_get_parameters_ex(3, &streamind, &src_calendar, &dest_calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(src_calendar);
	convert_to_string_ex(dest_calendar);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	/*
	  if(mcal_rename(mcal_le_struct->mcal_stream,Z_STRVAL_P(src_calendar),Z_STRVAL_P(dest_calendar))) {RETURN_TRUE;}
	  else {RETURN_FALSE; }
	*/
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mcal_list_alarms(int stream_id, int year, int month, int day, int hour, int min, int sec)
   List alarms for a given time */
PHP_FUNCTION(mcal_list_alarms)
{
	zval **streamind, **year, **month, **day, **hour, **min, **sec;
	datetime_t mydate=DT_INIT;
	int ind, ind_type;
	pils *mcal_le_struct; 
	cal_list_t *my_cal_list;
	int myargc=ZEND_NUM_ARGS();
	if ((myargc != 1 && myargc != 7) || zend_get_parameters_ex(myargc, &streamind, &year, &month, &day, &hour, &min, &sec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	array_init(return_value);
	
	if (myargc == 7) {
	    convert_to_long_ex(year);
	    convert_to_long_ex(month);
	    convert_to_long_ex(day);
	    convert_to_long_ex(hour);
	    convert_to_long_ex(min);
	    convert_to_long_ex(sec);
	    dt_setdate(&mydate, Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));
	    dt_settime(&mydate, Z_LVAL_PP(hour), Z_LVAL_PP(min), Z_LVAL_PP(sec));
	}
	else {
		mydate = mcal_le_struct->event->start;
	}
	g_cal_list = NULL;
	cal_search_alarm(mcal_le_struct->mcal_stream, &mydate);
	my_cal_list = g_cal_list;
	while (my_cal_list != NULL) {
		add_next_index_long(return_value, my_cal_list->uid);
		my_cal_list = my_cal_list->next;
		free(g_cal_list);
		g_cal_list = my_cal_list;
	}
}
/* }}} */

/* {{{ proto string mcal_delete_calendar(int stream_id, string calendar)
   Delete calendar */
PHP_FUNCTION(mcal_delete_calendar)
{
	zval **streamind, **calendar;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc=ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(2, &streamind, &calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(calendar);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	/*	if (mcal_delete_calendar(mcal_le_struct->mcal_stream,Z_STRVAL_P(calendar))) 
		{
	    RETURN_TRUE;
		}
		else 
		{
	    RETURN_FALSE;
		}
	*/
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mcal_delete_event(int stream_id, int event_id)
   Delete an event */
PHP_FUNCTION(mcal_delete_event)
{
	zval **streamind, **event_id;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc=ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(2, &streamind, &event_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(event_id);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	if (cal_remove(mcal_le_struct->mcal_stream, Z_LVAL_PP(event_id))) {
	    RETURN_TRUE;
	}
	else {
	    RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string mcal_append_event(int stream_id)
   Append a new event to the calendar stream */
PHP_FUNCTION(mcal_append_event)
{
	zval **streamind;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	unsigned long event_id;
	CALEVENT *myevent=NULL;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	myevent = mcal_le_struct->event;
	cal_append(mcal_le_struct->mcal_stream, "INBOX", &event_id, myevent);
	calevent_free(myevent);
	RETURN_LONG(event_id);
}
/* }}} */

/* {{{ proto string mcal_store_event(int stream_id)
   Store changes to an event */
PHP_FUNCTION(mcal_store_event)
{
	zval **streamind;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	CALEVENT *myevent=NULL;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	myevent = mcal_le_struct->event;
	cal_store(mcal_le_struct->mcal_stream, myevent);
	RETURN_LONG(myevent->id);
}
/* }}} */

/* {{{ proto string mcal_snooze(int stream_id, int uid)
   Snooze an alarm */
PHP_FUNCTION(mcal_snooze)
{
	zval **streamind, **uid;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	myargc = ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(2, &streamind, &uid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(uid);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	if (cal_snooze(mcal_le_struct->mcal_stream, Z_LVAL_PP(uid))) {
	    RETURN_TRUE;
	}
	else {
	    RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string mcal_event_set_category(int stream_id, string category)
   Attach a category to an event */
PHP_FUNCTION(mcal_event_set_category)
{
	zval **streamind, **category;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(2, &streamind, &category) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(category);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mcal_le_struct->event->category = strdup(Z_STRVAL_PP(category));
}
/* }}} */

/* {{{ proto string mcal_event_set_title(int stream_id, string title)
   Attach a title to an event */
PHP_FUNCTION(mcal_event_set_title)
{
	zval **streamind, **title;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(2, &streamind, &title) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(title);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
	RETURN_FALSE;
	}
	mcal_le_struct->event->title = strdup(Z_STRVAL_PP(title));
}
/* }}} */

/* {{{ proto string mcal_event_set_description(int stream_id, string description)
   Attach a description to an event */
PHP_FUNCTION(mcal_event_set_description)
{
	zval **streamind, **description;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(2, &streamind, &description) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(description);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	mcal_le_struct->event->description = strdup(Z_STRVAL_PP(description));
}
/* }}} */

/* {{{ proto string mcal_event_set_start(int stream_id, int year,int month, int day [[[, int hour], int min], int sec])
   Attach a start datetime to an event */
PHP_FUNCTION(mcal_event_set_start)
{
	zval **streamind, **year, **month, **date, **hour, **min, **sec;
	int ind, ind_type;
	int myhour=0, mymin=0, mysec=0;
	pils *mcal_le_struct; 
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc < 4 || myargc > 7 || zend_get_parameters_ex(myargc, &streamind, &year, &month, &date, &hour, &min, &sec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(year);
	convert_to_long_ex(month);
	convert_to_long_ex(date);
	if (myargc > 4) convert_to_long_ex(hour);
	if (myargc > 5) convert_to_long_ex(min);
	if (myargc > 6) convert_to_long_ex(sec);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	dt_setdate(&(mcal_le_struct->event->start), Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(date));
	
	if (myargc > 4) myhour = Z_LVAL_PP(hour);
	if (myargc > 5) mymin  = Z_LVAL_PP(min);
	if (myargc > 6) mysec  = Z_LVAL_PP(sec);
	if (myargc > 4) dt_settime(&(mcal_le_struct->event->start), myhour, mymin, mysec);
}
/* }}} */

/* {{{ proto string mcal_event_set_end(int stream_id, int year,int month, int day [[[, int hour], int min], int sec])
   Attach an end datetime to an event */
PHP_FUNCTION(mcal_event_set_end)
{
	zval **streamind, **year, **month, **date, **hour, **min, **sec;
	int ind, ind_type;
	/* initialize these to zero to make sure we don't use them
	   uninitialized (and to avoid the gcc warning) */
	int myhour = 0; int mymin = 0; int mysec = 0;
	pils *mcal_le_struct; 
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc < 4 || myargc > 7 || zend_get_parameters_ex(myargc, &streamind, &year, &month, &date, &hour, &min, &sec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(year);
	convert_to_long_ex(month);
	convert_to_long_ex(date);
	if (myargc > 4) convert_to_long_ex(hour);
	if (myargc > 5) convert_to_long_ex(min);
	if (myargc > 6) convert_to_long_ex(sec);

	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	dt_setdate(&(mcal_le_struct->event->end), Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(date));
	
	if (myargc > 4) myhour = Z_LVAL_PP(hour);
	if (myargc > 5) mymin  = Z_LVAL_PP(min);
	if (myargc > 6) mysec  = Z_LVAL_PP(sec);
	if (myargc > 4) dt_settime(&(mcal_le_struct->event->end), myhour, mymin, mysec);
}
/* }}} */

/* {{{ proto int mcal_event_set_alarm(int stream_id, int alarm)
   Add an alarm to the streams global event */
PHP_FUNCTION(mcal_event_set_alarm)
{
	zval **streamind, **alarm;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	myargc=ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(2, &streamind, &alarm) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(alarm);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	mcal_le_struct->event->alarm = Z_LVAL_PP(alarm);
}
/* }}} */

/* {{{ proto int mcal_event_init(int stream_id)
   Initialize a streams global event */
PHP_FUNCTION(mcal_event_init)
{
	zval **streamind;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	php_mcal_event_init(mcal_le_struct);
}
/* }}} */

/* {{{ proto int mcal_event_set_class(int stream_id, int class)
   Add an class to the streams global event */
PHP_FUNCTION(mcal_event_set_class)
{
	zval **streamind, **class;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(2, &streamind, &class) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(class);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	mcal_le_struct->event->public = Z_LVAL_PP(class);
}
/* }}} */

/* {{{ proto string mcal_event_add_attribute(int stream_id, string attribute, string value)
   Add an attribute and value to an event */
PHP_FUNCTION(mcal_event_add_attribute)
{
	zval **streamind, **attribute, **val;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 3 || zend_get_parameters_ex(3, &streamind, &attribute, &val) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_string_ex(attribute);
	convert_to_string_ex(val);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
#if MCALVER >= 20000121
	if (calevent_setattr(mcal_le_struct->event, Z_STRVAL_PP(attribute), Z_STRVAL_PP(val))) {
		RETURN_TRUE;
	}
	else
#endif
		{
			RETURN_FALSE;
		}
}
/* }}} */

/* {{{ proto bool mcal_is_leap_year(int year)
   Returns true if year is a leap year, false if not */
PHP_FUNCTION(mcal_is_leap_year)
{
	zval **year;
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 1 || zend_get_parameters_ex(1, &year) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(year);
	
	if (isleapyear(Z_LVAL_PP(year))) {
	    RETURN_TRUE;
	}
	else {
	    RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int mcal_days_in_month(int month, bool leap_year)
   Returns the number of days in the given month, needs to know if the year is a leap year or not */
PHP_FUNCTION(mcal_days_in_month)
{
	zval **month, **leap;
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 2 || zend_get_parameters_ex(2, &month, &leap) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(month);
	convert_to_long_ex(leap);
	convert_to_boolean_ex(leap);
	
	RETURN_LONG(daysinmonth(Z_LVAL_PP(month), Z_LVAL_PP(leap)));
}
/* }}} */

/* {{{ proto bool mcal_date_valid(int year, int month, int day)
   Returns true if the date is a valid date */
PHP_FUNCTION(mcal_date_valid)
{
	zval **year, **month, **day;
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 3 || zend_get_parameters_ex(3, &year, &month, &day) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(year);
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	
	if (datevalid(Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day))) {
	    RETURN_TRUE;
	}
	else {
	    RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool mcal_time_valid(int hour, int min, int sec)
   Returns true if the time is a valid time */
PHP_FUNCTION(mcal_time_valid)
{
	zval **hour, **min, **sec;
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 3 || zend_get_parameters_ex(3, &hour, &min, &sec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(hour);
	convert_to_long_ex(min);
	convert_to_long_ex(sec);
	
	if (timevalid(Z_LVAL_PP(hour), Z_LVAL_PP(min), Z_LVAL_PP(sec))) {
	    RETURN_TRUE;
	}
	else {
	    RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int mcal_day_of_week(int year, int month, int day)
   Returns the day of the week of the given date */
PHP_FUNCTION(mcal_day_of_week)
{
	zval **year, **month, **day;
	int myargc;
	datetime_t mydate;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 3 || zend_get_parameters_ex(3, &year, &month, &day) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(year);
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	
	dt_init(&mydate);
	dt_setdate(&mydate, Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));
	
	RETURN_LONG(dt_dayofweek(&mydate));
}
/* }}} */

/* {{{ proto int mcal_day_of_year(int year, int month, int day)
   Returns the day of the year of the given date */
PHP_FUNCTION(mcal_day_of_year)
{
	zval **year, **month, **day;
	int myargc;
	datetime_t mydate;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 3 || zend_get_parameters_ex(3, &year, &month, &day) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(year);
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	
	dt_init(&mydate);
	dt_setdate(&mydate, Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));
	
	RETURN_LONG(dt_dayofyear(&mydate));
}
/* }}} */

/* {{{ proto int mcal_week_of_year(int day, int month, int year)
   Returns the week number of the given date */
PHP_FUNCTION(mcal_week_of_year)
{
	zval **year, **month, **day;
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 3 || zend_get_parameters_ex(3, &day, &month, &year) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
 	convert_to_long_ex(year);
 	convert_to_long_ex(month);
 	convert_to_long_ex(day);
	
 	if (datevalid(Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day))) { 
		RETURN_LONG(dt_weekofyear(Z_LVAL_PP(day), Z_LVAL_PP(month), Z_LVAL_PP(year)));
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int mcal_date_compare(int ayear, int amonth, int aday, int byear, int bmonth, int bday)
   Returns <0, 0, >0 if a<b, a==b, a>b respectively */
PHP_FUNCTION(mcal_date_compare)
{
	zval **ayear, **amonth, **aday;
	zval **byear, **bmonth, **bday;
	int myargc;
	datetime_t myadate, mybdate;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 6 || zend_get_parameters_ex(6, &ayear, &amonth, &aday, &byear, &bmonth, &bday) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(ayear);
	convert_to_long_ex(amonth);
	convert_to_long_ex(aday);
	convert_to_long_ex(byear);
	convert_to_long_ex(bmonth);
	convert_to_long_ex(bday);
	
	dt_init(&myadate);
	dt_init(&mybdate);
	dt_setdate(&myadate, Z_LVAL_PP(ayear), Z_LVAL_PP(amonth), Z_LVAL_PP(aday));
	dt_setdate(&mybdate, Z_LVAL_PP(byear), Z_LVAL_PP(bmonth), Z_LVAL_PP(bday));
	
	RETURN_LONG(dt_compare(&myadate, &mybdate));
}
/* }}} */

/* {{{ proto object mcal_next_recurrence(int stream_id, int weekstart, array next)
   Returns an object filled with the next date the event occurs, on or after the supplied date.  Returns empty date field if event does not occur or something is invalid. */
PHP_FUNCTION(mcal_next_recurrence)
{
	zval **streamind, **weekstart, **next, **zvalue;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t mydate;
	
	myargc=ZEND_NUM_ARGS();
	if (myargc != 3 || zend_get_parameters_ex(3, &streamind, &weekstart, &next) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(weekstart);
	convert_to_array_ex(next);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	if (zend_hash_find(Z_ARRVAL_PP(next), "year", sizeof("year"), (void **) &zvalue) == SUCCESS) {
		SEPARATE_ZVAL(zvalue);
		convert_to_long_ex(zvalue);
		mydate.year = Z_LVAL_PP(zvalue);
	}
	if (zend_hash_find(Z_ARRVAL_PP(next), "month", sizeof("month"), (void **) &zvalue) == SUCCESS) {
		SEPARATE_ZVAL(zvalue);
		convert_to_long_ex(zvalue);
		mydate.mon = Z_LVAL_PP(zvalue);
	}
	if (zend_hash_find(Z_ARRVAL_PP(next), "mday", sizeof("mday"), (void **) &zvalue) == SUCCESS) {
		SEPARATE_ZVAL(zvalue);
		convert_to_long_ex(zvalue);
		mydate.mday = Z_LVAL_PP(zvalue);
	}
	if (zend_hash_find(Z_ARRVAL_PP(next), "hour", sizeof("hour"), (void **) &zvalue) == SUCCESS) {
		SEPARATE_ZVAL(zvalue);
		convert_to_long_ex(zvalue);
		mydate.hour = Z_LVAL_PP(zvalue);
	}
	if (zend_hash_find(Z_ARRVAL_PP(next), "min", sizeof("min"), (void **) &zvalue) == SUCCESS) {
		SEPARATE_ZVAL(zvalue);
		convert_to_long_ex(zvalue);
		mydate.min = Z_LVAL_PP(zvalue);
	}
	if (zend_hash_find(Z_ARRVAL_PP(next), "sec", sizeof("sec"), (void **) &zvalue) == SUCCESS) {
		SEPARATE_ZVAL(zvalue);
		convert_to_long_ex(zvalue);
		mydate.sec = Z_LVAL_PP(zvalue);
	}
	
	calevent_next_recurrence(mcal_le_struct->event, &mydate, Z_LVAL_PP(weekstart));
	
	if (object_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	if (mydate.has_date) {
	    add_property_long(return_value, "year", mydate.year);
	    add_property_long(return_value, "month", mydate.mon);
	    add_property_long(return_value, "mday", mydate.mday);
	}
	if (mydate.has_time) {
		add_property_long(return_value, "hour", mydate.hour);
		add_property_long(return_value, "min", mydate.min);
		add_property_long(return_value, "sec", mydate.sec);
	}
}
/* }}} */

/* {{{ proto string mcal_event_set_recur_none(int stream_id)
   Create a daily recurrence */
PHP_FUNCTION(mcal_event_set_recur_none)
{
	zval **streamind;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 1 || zend_get_parameters_ex(1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	calevent_recur_none(mcal_le_struct->event);
}
/* }}} */

/* {{{ proto string mcal_event_set_recur_daily(int stream_id, int year, int month, int day, int interval)
   Create a daily recurrence */
PHP_FUNCTION(mcal_event_set_recur_daily)
{
	zval **streamind, **year, **month, **day, **interval;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t endtime = DT_INIT;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 5 || zend_get_parameters_ex(5, &streamind, &year, &month, &day, &interval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(year);
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	convert_to_long_ex(interval);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	dt_setdate(&endtime, Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));
	calevent_recur_daily(mcal_le_struct->event, &endtime, Z_LVAL_PP(interval));
}
/* }}} */

/* {{{ proto string mcal_event_set_recur_weekly(int stream_id, int year, int month, int day, int interval, int weekdays)
   Create a weekly recurrence */
PHP_FUNCTION(mcal_event_set_recur_weekly)
{
	zval **streamind, **year, **month, **day, **interval, **weekdays;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t endtime=DT_INIT;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 6 || zend_get_parameters_ex(6, &streamind, &year, &month, &day, &interval, &weekdays) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(year);
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	convert_to_long_ex(interval);
	convert_to_long_ex(weekdays);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	dt_setdate(&endtime, Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));
	calevent_recur_weekly(mcal_le_struct->event, &endtime, Z_LVAL_PP(interval), Z_LVAL_PP(weekdays));
}
/* }}} */

/* {{{ proto string mcal_event_set_recur_monthly_mday(int stream_id, int year, int month, int day, int interval)
   Create a monthly by day recurrence */
PHP_FUNCTION(mcal_event_set_recur_monthly_mday)
{
	zval **streamind, **year, **month, **day, **interval;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t endtime=DT_INIT;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 5 || zend_get_parameters_ex(5, &streamind, &year, &month, &day, &interval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(year);
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	convert_to_long_ex(interval);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	dt_setdate(&endtime, Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));
	calevent_recur_monthly_mday(mcal_le_struct->event, &endtime, Z_LVAL_PP(interval));
}
/* }}} */

/* {{{ proto string mcal_event_set_recur_monthly_wday(int stream_id, int year, int month, int day, int interval)
   Create a monthly by week recurrence */
PHP_FUNCTION(mcal_event_set_recur_monthly_wday)
{
	zval **streamind, **year, **month, **day, **interval;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t endtime=DT_INIT;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 5 || zend_get_parameters_ex(5, &streamind, &year, &month, &day, &interval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(year);
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	convert_to_long_ex(interval);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	dt_setdate(&endtime, Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));
	calevent_recur_monthly_wday(mcal_le_struct->event, &endtime, Z_LVAL_PP(interval));
}
/* }}} */

/* {{{ proto string mcal_event_set_recur_yearly(int stream_id, int year, int month, int day, int interval)
   Create a yearly recurrence */
PHP_FUNCTION(mcal_event_set_recur_yearly)
{
	zval **streamind, **year, **month, **day, **interval;
	int ind, ind_type;
	pils *mcal_le_struct; 
	int myargc;
	datetime_t endtime=DT_INIT;
	
	myargc = ZEND_NUM_ARGS();
	if (myargc != 5 || zend_get_parameters_ex(5, &streamind, &year, &month, &day, &interval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(streamind);
	convert_to_long_ex(year);
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	convert_to_long_ex(interval);
	
	ind = Z_LVAL_PP(streamind);
	mcal_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!mcal_le_struct) {
		php_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	
	dt_setdate(&endtime, Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));
	calevent_recur_yearly(mcal_le_struct->event, &endtime, Z_LVAL_PP(interval));
}
/* }}} */


/* Interfaces to callbacks */
void cc_searched (unsigned long cal_uid)
{
	if (g_cal_list == NULL) {
		g_cal_list = malloc(sizeof(struct cal_list));
		g_cal_list->uid = cal_uid;
		g_cal_list->next = NULL;
		g_cal_list_end = g_cal_list;
    }
	else {
		g_cal_list_end->next = malloc(sizeof(struct cal_list));
		g_cal_list_end = g_cal_list_end->next;
		g_cal_list_end->uid = cal_uid;
		g_cal_list_end->next = NULL;
	}
}

void cc_appended(php_uint32 uid)
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
