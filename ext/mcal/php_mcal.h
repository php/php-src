/* $Id$ */

#ifndef _INCLUDED_MCAL_H
#define _INCLUDED_MCAL_H

#if COMPILE_DL
#undef HAVE_MCAL
#define HAVE_MCAL 1
#endif

#if HAVE_MCAL

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#ifndef MSVC5
#include "build-defs.h"
#endif

/* Functions accessable to PHP */
//extern zend_module_entry php3_mcal_module_entry;
extern zend_module_entry php3_mcal_module_entry;
#define php3_mcal_module_ptr &php3_mcal_module_entry
#define phpext_mcal_ptr php3_mcal_module_ptr

#ifdef ZEND_VERSION
extern PHP_MINIT_FUNCTION(mcal);
PHP_MINFO_FUNCTION(mcal);
#else
extern int PHP_MINIT_FUNCTION(INIT_FUNC_ARGS);
extern void PHP_MINFO_FUNCTION(void);
#endif


extern int mcal_init_request(INIT_FUNC_ARGS);
extern int mcal_end_request(void);
void make_event_object();
void php3_event_init();

void php3_mcal_open(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_popen(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_reopen(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_close(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_fetch_event(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_list_events(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_create_calendar(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_rename_calendar(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_delete_calendar(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_store_event(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_delete_event(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_snooze(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_list_alarms(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_category(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_title(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_description(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_start(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_end(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_alarm(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_class(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_is_leap_year(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_days_in_month(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_date_valid(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_time_valid(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_day_of_week(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_day_of_year(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_date_compare(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_init(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_next_recurrence(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_recur_daily(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_recur_weekly(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_recur_monthly_mday(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_recur_monthly_wday(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_event_set_recur_yearly(INTERNAL_FUNCTION_PARAMETERS);
void php3_mcal_fetch_current_stream_event(INTERNAL_FUNCTION_PARAMETERS);


#else
#define php3_mcal_module_ptr NULL
#endif /* HAVE_MCAL */


#endif




