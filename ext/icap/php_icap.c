/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Mark Musone <musone@chek.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define ICAP1

#ifdef ERROR
#undef ERROR
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_ICAP

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#include "cal.h"
#include "php_icap.h"
#include "zend_modules.h"
#include "ext/standard/info.h"
#include "ext/standard/basic_functions.h"
#ifdef PHP_WIN32
#include "winsock.h"
#endif
CALSTREAM *cal_open();
CALSTREAM *cal_close_it ();
CALSTREAM *cal_close_full ();


typedef struct php_icap_le_struct {
	CALSTREAM *icap_stream;
	long flags;
} pils;


typedef struct cal_list
{
php_uint32 uid;
struct cal_list *next;
} cal_list_t;

static cal_list_t *g_cal_list=NULL;
static cal_list_t *g_cal_list_end=NULL;
/* 
 * this array should be set up as:
 * {"PHPScriptFunctionName", dllFunctionName, 1} 
 */

function_entry icap_functions[] = {
	PHP_FE(icap_open,				NULL)
/* for now it's obviously a dummy 
	PHP_FE(icap_popen,				NULL)
*/
	PHP_FE(icap_reopen,				NULL)
	PHP_FE(icap_fetch_event,		NULL)
	PHP_FE(icap_list_events,		NULL)
	PHP_FE(icap_list_alarms,		NULL)
	PHP_FE(icap_create_calendar,	NULL)
	PHP_FE(icap_rename_calendar,	NULL)
	PHP_FE(icap_delete_calendar,	NULL)
	PHP_FE(icap_delete_event,		NULL)
	PHP_FE(icap_store_event,		NULL)
	PHP_FE(icap_snooze,				NULL)
	{NULL, NULL, NULL}
};


zend_module_entry php_icap_module_entry = {
    STANDARD_MODULE_HEADER,
    "icap",
    icap_functions,
    PHP_MINIT(icap),
    NULL,
    NULL,
    NULL,
    PHP_MINFO(icap),
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_ICAP
ZEND_GET_MODULE(php_icap)
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

CALSTREAM *cal_close_it (zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	pils *icap_le_struct = (pils *)rsrc->ptr;
	CALSTREAM *ret;

	ret = cal_close (icap_le_struct->icap_stream, 0);
	efree(icap_le_struct);
	return ret;
}


PHP_MINFO_FUNCTION(icap)
{
	php_info_print_table_start();        
	php_info_print_table_row(2, "ICAP Support", "enabled");        
	php_info_print_table_row(2, "ICAP Version", CALVER);        
	php_info_print_table_end();
}

PHP_MINIT_FUNCTION(icap)
{
    le_icap = zend_register_list_destructors_ex(cal_close_it, NULL, "icap", module_number);
    return SUCCESS;
}


static int add_assoc_object(pval *arg, char *key, pval *tmp)
{
        HashTable *symtable;
        
        if (Z_TYPE_P(arg) == IS_OBJECT) {
                symtable = Z_OBJPROP_P(arg);
        } else {
                symtable = Z_ARRVAL_P(arg);
        }
        return zend_hash_update(symtable, key, strlen(key)+1, (void *) &tmp, sizeof(pval *), NULL);
}


void php_icap_do_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
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
        int myargc=ZEND_NUM_ARGS();

	
	if (myargc <3 || myargc >4 || getParameters(ht, myargc, &calendar, &user, &passwd, &options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(calendar);
	convert_to_string(user);
	convert_to_string(passwd);
	strcpy(icap_user, Z_STRVAL_P(user));
	strcpy(icap_password, Z_STRVAL_P(passwd));
	if(myargc ==4) {
		convert_to_long(options);
		flags=Z_LVAL_P(options);
	}
		icap_stream = cal_open(NULL, Z_STRVAL_P(calendar), 0);
	if (!icap_stream) {
		php_error(E_WARNING, "%s(): Couldn't open stream %s", get_active_function_name(TSRMLS_C), Z_STRVAL_P(calendar));
		RETURN_FALSE;
	}

	icap_le_struct = emalloc(sizeof(pils));
	icap_le_struct->icap_stream = icap_stream;
	icap_le_struct->flags = 0;	
	ind = zend_list_insert(icap_le_struct, le_icap);	
	RETURN_LONG(ind);
}


/* {{{ proto int icap_close(int stream_id [, int options])
   Close an ICAP stream */
PHP_FUNCTION(icap_close)
{
        pval *options, *streamind;
        int ind, ind_type;
        pils *icap_le_struct=NULL; 
        int myargcount=ZEND_NUM_ARGS();
        long flags = 0;

        if (myargcount < 1 || myargcount > 2 || getParameters(ht, myargcount, &streamind, &options) == FAILURE) {
                WRONG_PARAM_COUNT;
        }
        convert_to_long(streamind);
        ind = Z_LVAL_P(streamind);
        icap_le_struct = (pils *)zend_list_find(ind, &ind_type);
        if (!icap_le_struct ) {
	  php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
                RETURN_FALSE;
	}
        if(myargcount==2) {
                convert_to_long(options);
                flags = Z_LVAL_P(options);
                icap_le_struct->flags = flags;
        }
        zend_list_delete(ind);
        RETURN_TRUE;
}
/* }}} */


/* {{{ proto int icap_open(string calendar, string user, string password [, int options])
   Open an ICAP stream to a calendar */
PHP_FUNCTION(icap_open)
{
	php_icap_do_open(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ proto int icap_reopen(int stream_id, string calendar [, int options])
   Reopen ICAP stream to new calendar */
PHP_FUNCTION(icap_reopen)
{
	pval *streamind;
	pval *calendar;
	pval *options;
	CALSTREAM *icap_stream;
	pils *icap_le_struct; 
	int ind, ind_type;
	long flags=0;
	long cl_flags=0;
	int myargc=ZEND_NUM_ARGS();

	if (myargc<2 || myargc>3 || getParameters(ht, myargc, &streamind, &calendar, &options) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

	convert_to_long(streamind);
	ind = Z_LVAL_P(streamind);
	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	convert_to_string(calendar);
	if(myargc == 3) {
		convert_to_long(options);
		flags = Z_LVAL_P(options);
		icap_le_struct->flags = cl_flags;	
	}
	/*
		icap_stream = cal_connect(Z_STRVAL_P(calendar));
		cal_login(icap_stream, Z_STRVAL_P(calendar));
	*/
	if (icap_stream == NULL) {
		php_error(E_WARNING, "%s(): Couldn't re-open stream", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int icap_expunge(int stream_id)
   Delete all messages marked for deletion */
PHP_FUNCTION(icap_expunge)
{
	pval *streamind;
	int ind, ind_type;
	pval *start, *end;
	pils *icap_le_struct; 

	if (ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = Z_LVAL_P(streamind);

	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);

	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

/*	cal_expunge (icap_le_struct->icap_stream);
*/
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int icap_fetch_event(int stream_id, int eventid [, int options])
   Fetch an event */
PHP_FUNCTION(icap_fetch_event)
{
	pval *streamind, *eventid, *start, *end, *options=NULL;
	int ind, ind_type;
	pils *icap_le_struct=NULL; 
	CALEVENT *myevent;
	int myargcount=ZEND_NUM_ARGS();
	
	if (myargcount < 1 || myargcount > 3 || getParameters(ht, myargcount, &streamind, &eventid, &options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	convert_to_long(eventid);
	ind = Z_LVAL_P(streamind);
	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
    }
	if(myargcount==3) {
		convert_to_long(options);
	}
	cal_fetch(icap_le_struct->icap_stream, Z_LVAL_P(eventid), &myevent);

	object_init(return_value);
	add_property_long(return_value, "id", myevent->id);
	add_property_long(return_value, "public", myevent->public);
	MAKE_STD_ZVAL(start);
	object_init(start);
	if(myevent->start.has_date)
	  {
	    add_property_long(start, "year", myevent->start.year);
	    add_property_long(start, "month", myevent->start.mon);
	    add_property_long(start, "mday", myevent->start.mday);
	  }
	if(myevent->start.has_time)
	  {
	    add_property_long(start, "hour", myevent->start.hour);
	    add_property_long(start, "min", myevent->start.min);
	    add_property_long(start, "sec", myevent->start.sec);
	  }
	add_assoc_object(return_value, "start", start);

	MAKE_STD_ZVAL(end);
	object_init(end);
	if(myevent->end.has_date)
	  {
	    add_property_long(end, "year", myevent->end.year);
	    add_property_long(end, "month", myevent->end.mon);
	    add_property_long(end, "mday", myevent->end.mday);
	  }
	if(myevent->end.has_time)
	  {
	    add_property_long(end, "hour", myevent->end.hour);
	    add_property_long(end, "min", myevent->end.min);
	    add_property_long(end, "sec", myevent->end.sec);
	  }
	add_assoc_object(return_value, "end", end);
	
	add_property_string(return_value, "category", myevent->category, 1);
	add_property_string(return_value, "title", myevent->title, 1);
	add_property_string(return_value, "description", myevent->description, 1);
	add_property_long(return_value, "alarm", myevent->alarm);
}
/* }}} */

/* {{{ proto array icap_list_events(int stream_id, int begindate [, int enddate])
   Returns list of UIDs for that day or range of days */
PHP_FUNCTION(icap_list_events)
{
	pval *streamind, *begindate, *enddate;
        pval **pvalue;
	int ind, ind_type;
	unsigned long i;
	char *t;
	int icap_folders=0;
	unsigned int msgno;
	pils *icap_le_struct; 
	cal_list_t *my_cal_list;
	datetime_t begincal, endcal;
	int myargc;
	myargc=ZEND_NUM_ARGS();
	if (myargc <2 || myargc > 3 || getParameters(ht, myargc, &streamind, &begindate, &enddate) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_array(begindate);
	if(myargc == 3) convert_to_array(enddate);
	ind = Z_LVAL_P(streamind);

	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);

	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	begincal.has_time=0;
	endcal.has_time=0;
	if(zend_hash_find(Z_ARRVAL_P(begindate), "year", sizeof("year"), (void **) &pvalue)== SUCCESS){
	 SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          begincal.year=Z_LVAL_PP(pvalue);
       }
       if(zend_hash_find(Z_ARRVAL_P(begindate), "month", sizeof("month"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          begincal.mon=Z_LVAL_PP(pvalue);
       }
       if(zend_hash_find(Z_ARRVAL_P(begindate), "day", sizeof("day"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          begincal.mday=Z_LVAL_PP(pvalue);
       }
if(myargc == 3)
  {
    if(zend_hash_find(Z_ARRVAL_P(enddate), "year", sizeof("year"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
      convert_to_long(*pvalue);
      endcal.year=Z_LVAL_PP(pvalue);
    }
    if(zend_hash_find(Z_ARRVAL_P(enddate), "month", sizeof("month"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
      convert_to_long(*pvalue);
      endcal.mon=Z_LVAL_PP(pvalue);
    }
    if(zend_hash_find(Z_ARRVAL_P(enddate), "day", sizeof("day"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
      convert_to_long(*pvalue);
      endcal.mday=Z_LVAL_PP(pvalue);
    }
  }
 

g_cal_list=NULL;
 cal_search_range(icap_le_struct->icap_stream, &begincal, &endcal);
 my_cal_list=g_cal_list;
 while(my_cal_list != NULL)
   {
     add_next_index_long(return_value, my_cal_list->uid);
     my_cal_list=my_cal_list->next;
     free(g_cal_list);
     g_cal_list=my_cal_list;
   }
}
/* }}} */


/* {{{ proto string icap_create_calendar(int stream_id, string calendar)
   Create a new calendar*/

PHP_FUNCTION(icap_create_calendar)
{
	pval *streamind, *calendar;
	int ind, ind_type;
	pils *icap_le_struct; 
	int myargc=ZEND_NUM_ARGS();
	if (myargc <1 || myargc > 2 || getParameters(ht, myargc, &streamind, &calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(calendar);
	ind = Z_LVAL_P(streamind);

	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
/*
	if (icap_create(icap_le_struct->icap_stream, Z_STRVAL_P(calendar))) 
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


/* {{{ proto string icap_rename_calendar(int stream_id, string src_calendar, string dest_calendar)
   Rename a calendar*/
PHP_FUNCTION(icap_rename_calendar)
{
	pval *streamind, *src_calendar, *dest_calendar;
	int ind, ind_type;
	pils *icap_le_struct; 
	int myargc=ZEND_NUM_ARGS();
	if (myargc <2 || myargc > 3 || getParameters(ht, myargc, &streamind, &src_calendar, &dest_calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(src_calendar);
	convert_to_string(dest_calendar);
	ind = Z_LVAL_P(streamind);

	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
/*
	if(icap_rename(icap_le_struct->icap_stream, Z_STRVAL_P(src_calendar), Z_STRVAL_P(dest_calendar))) {RETURN_TRUE;}
	else {RETURN_FALSE; }
*/
}
/* }}} */




/* {{{ proto int icap_list_alarms(int stream_id, array date, array time)
   List alarms for a given time */
PHP_FUNCTION(icap_list_alarms)
{
	pval *streamind, *date, *time;
        pval **pvalue;
	datetime_t mydate;
	int ind, ind_type;
	pils *icap_le_struct; 
        int icap_folders=0;
        unsigned int msgno;
        cal_list_t *my_cal_list;

	int myargc=ZEND_NUM_ARGS();
	if (myargc != 3 || getParameters(ht, myargc, &streamind, &date, &time) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_array(date);
	convert_to_array(time);
	ind = Z_LVAL_P(streamind);

	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

        if (array_init(return_value) == FAILURE) {
                RETURN_FALSE;
        }
	mydate.has_date=1;
	mydate.has_time=1;
       if(zend_hash_find(Z_ARRVAL_P(date), "year", sizeof("year"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.year=Z_LVAL_PP(pvalue);
       }
       if(zend_hash_find(Z_ARRVAL_P(date), "month", sizeof("month"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.mon=Z_LVAL_PP(pvalue);
       }
       if(zend_hash_find(Z_ARRVAL_P(date), "day", sizeof("day"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.mday=Z_LVAL_PP(pvalue);
       }

       if(zend_hash_find(Z_ARRVAL_P(time), "hour", sizeof("hour"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.hour=Z_LVAL_PP(pvalue);
       }
       if(zend_hash_find(Z_ARRVAL_P(time), "minute", sizeof("minute"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
          convert_to_long(*pvalue);
          mydate.min=Z_LVAL_PP(pvalue);
       }
       mydate.sec=0;
       g_cal_list=NULL;
       cal_search_alarm(icap_le_struct->icap_stream, &mydate);
       my_cal_list=g_cal_list;
 while(my_cal_list != NULL)
   {
     add_next_index_long(return_value, my_cal_list->uid);
     my_cal_list=my_cal_list->next;
     free(g_cal_list);
     g_cal_list=my_cal_list;
   }


}
/* }}} */


/* {{{ proto string icap_delete_calendar(int stream_id, string calendar)
   Delete calendar */
PHP_FUNCTION(icap_delete_calendar)
{
	pval *streamind, *calendar;
	int ind, ind_type;
	pils *icap_le_struct; 
	int myargc=ZEND_NUM_ARGS();
	if (myargc <1 || myargc > 2 || getParameters(ht, myargc, &streamind, &calendar) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(calendar);
	ind = Z_LVAL_P(streamind);

	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	
	if (icap_delete_calendar(icap_le_struct->icap_stream, Z_STRVAL_P(calendar))) 
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
   Delete event */
PHP_FUNCTION(icap_delete_event)
{
	pval *streamind, *uid;
	int ind, ind_type;
	pils *icap_le_struct; 
	int myargc=ZEND_NUM_ARGS();
	if (myargc <1 || myargc > 2 || getParameters(ht, myargc, &streamind, &uid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(uid);
	ind = Z_LVAL_P(streamind);

	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);
	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	if (cal_remove(icap_le_struct->icap_stream, Z_LVAL_P(uid))) 
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

/* {{{ proto void icap_popen(void)
   For now this is obviously a dummy */
PHP_FUNCTION(icap_popen)
{
}
/* }}} */

/* {{{ proto string icap_store_event(int stream_id, object event)
   Store an event */
PHP_FUNCTION(icap_store_event)
{
	pval *streamind, *storeobject;
	int ind, ind_type;
	unsigned long i;
	char *t;
	int icap_folders=0;
	unsigned int msgno;
	pils *icap_le_struct; 
	pval **pvalue, **temppvalue;
	cal_list_t *my_cal_list;
	int myargc;
	unsigned long uid;
	CALEVENT *myevent;
	myargc=ZEND_NUM_ARGS();
	if (myargc !=2 || getParameters(ht, myargc, &streamind, &storeobject) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_array(storeobject);

	ind = Z_LVAL_P(streamind);

	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);

	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	myevent=calevent_new();
	if(zend_hash_find(Z_ARRVAL_P(storeobject), "uid", sizeof("uid"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent->id=Z_LVAL_PP(pvalue);
	}
	if(zend_hash_find(Z_ARRVAL_P(storeobject), "public", sizeof("public"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent->public=Z_LVAL_PP(pvalue);
	}
	if(zend_hash_find(Z_ARRVAL_P(storeobject), "category", sizeof("category"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent->category=strdup(Z_STRVAL_PP(pvalue));
	}
	if(zend_hash_find(Z_ARRVAL_P(storeobject), "title", sizeof("title"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent->title=strdup(Z_STRVAL_PP(pvalue));
	}
	if(zend_hash_find(Z_ARRVAL_P(storeobject), "description", sizeof("description"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_string(*pvalue);
	  myevent->description=strdup(Z_STRVAL_PP(pvalue));
	}

	if(zend_hash_find(Z_ARRVAL_P(storeobject), "alarm", sizeof("alarm"), (void **) &pvalue)== SUCCESS){
          SEPARATE_ZVAL(pvalue);
	  convert_to_long(*pvalue);
	  myevent->alarm=Z_LVAL_PP(pvalue);
	}


       	if(zend_hash_find(Z_ARRVAL_P(storeobject), "start", sizeof("start"), (void **) &temppvalue)== SUCCESS){
          SEPARATE_ZVAL(temppvalue);
	  convert_to_array(*temppvalue);
	  
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "year", sizeof("year"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.year=Z_LVAL_PP(pvalue);
	  }
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "month", sizeof("month"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.mon=Z_LVAL_PP(pvalue);
	  }
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "mday", sizeof("mday"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.mday=Z_LVAL_PP(pvalue);
	  }
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "hour", sizeof("hour"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.hour=Z_LVAL_PP(pvalue);
	  }
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "min", sizeof("min"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.min=Z_LVAL_PP(pvalue);
	  }
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "sec", sizeof("sec"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->start.sec=Z_LVAL_PP(pvalue);
	  }
	  myevent->start.has_date=true;
	}

       	if(zend_hash_find(Z_ARRVAL_P(storeobject), "end", sizeof("end"), (void **) &temppvalue)== SUCCESS){
          SEPARATE_ZVAL(temppvalue);
	  convert_to_array(*temppvalue);
	  
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "year", sizeof("year"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.year=Z_LVAL_PP(pvalue);
	  }
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "month", sizeof("month"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.mon=Z_LVAL_PP(pvalue);
	  }
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "mday", sizeof("mday"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.mday=Z_LVAL_PP(pvalue);
	  }
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "hour", sizeof("hour"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.hour=Z_LVAL_PP(pvalue);
	  }
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "min", sizeof("min"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.min=Z_LVAL_PP(pvalue);
	  }
	  if(zend_hash_find(Z_ARRVAL_PP(temppvalue), "sec", sizeof("sec"), (void **) &pvalue)== SUCCESS){
	    SEPARATE_ZVAL(pvalue);
	    convert_to_long(*pvalue);
	    myevent->end.sec=Z_LVAL_PP(pvalue);
	  }
	  myevent->end.has_date=true;
	}

	cal_append(icap_le_struct->icap_stream, "INBOX", &uid, myevent);
	calevent_free(myevent);
	RETURN_LONG(uid);
}
/* }}} */


/* {{{ proto string icap_snooze(int stream_id, int uid)
   Snooze an alarm */
PHP_FUNCTION(icap_snooze)
{
	pval *streamind, *uid;
	int ind, ind_type;
	pils *icap_le_struct; 
	pval **pvalue;
	int myargc;
	myargc=ZEND_NUM_ARGS();
	if (myargc !=2 || getParameters(ht, myargc, &streamind, &uid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(uid);

	ind = Z_LVAL_P(streamind);

	icap_le_struct = (pils *)zend_list_find(ind, &ind_type);

	if (!icap_le_struct ) {
		php_error(E_WARNING, "%s(): Unable to find stream pointer", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	if(cal_snooze(icap_le_struct->icap_stream, Z_LVAL_P(uid)))
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


void cc_appended(php_uint32 uid)
{

}


void cc_fetched(const CALEVENT *event)
{

}


void cc_login(const char **user, const char **pwd)
{

*user=icap_user;
*pwd=icap_password; 
}


void cc_vlog(const char *fmt, va_list ap)
{
}
void cc_vdlog(const char *fmt, va_list ap)
{
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
