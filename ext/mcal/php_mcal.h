/* $Id$ */

#ifndef PHP_MCAL_H
#define PHP_MCAL_H

#if HAVE_MCAL

/* Functions accessable to PHP */
extern zend_module_entry php_mcal_module_entry;
#define php_mcal_module_ptr &php_mcal_module_entry

extern int mcal_init_request(INIT_FUNC_ARGS);
extern int mcal_end_request(void);

PHP_MINIT_FUNCTION(mcal);
PHP_MINFO_FUNCTION(mcal);

PHP_FUNCTION(mcal_open);
PHP_FUNCTION(mcal_popen);
PHP_FUNCTION(mcal_reopen);
PHP_FUNCTION(mcal_close);
PHP_FUNCTION(mcal_fetch_event);
PHP_FUNCTION(mcal_list_events);
PHP_FUNCTION(mcal_create_calendar);
PHP_FUNCTION(mcal_rename_calendar);
PHP_FUNCTION(mcal_delete_calendar);
PHP_FUNCTION(mcal_append_event);
PHP_FUNCTION(mcal_store_event);
PHP_FUNCTION(mcal_delete_event);
PHP_FUNCTION(mcal_snooze);
PHP_FUNCTION(mcal_list_alarms);
PHP_FUNCTION(mcal_event_set_category);
PHP_FUNCTION(mcal_event_set_title);
PHP_FUNCTION(mcal_event_set_description);
PHP_FUNCTION(mcal_event_set_start);
PHP_FUNCTION(mcal_event_set_end);
PHP_FUNCTION(mcal_event_set_alarm);
PHP_FUNCTION(mcal_event_set_class);
PHP_FUNCTION(mcal_event_add_attribute);
PHP_FUNCTION(mcal_is_leap_year);
PHP_FUNCTION(mcal_days_in_month);
PHP_FUNCTION(mcal_date_valid);
PHP_FUNCTION(mcal_time_valid);
PHP_FUNCTION(mcal_day_of_week);
PHP_FUNCTION(mcal_day_of_year);
PHP_FUNCTION(mcal_week_of_year);
PHP_FUNCTION(mcal_date_compare);
PHP_FUNCTION(mcal_event_init);
PHP_FUNCTION(mcal_next_recurrence);
PHP_FUNCTION(mcal_event_set_recur_none);
PHP_FUNCTION(mcal_event_set_recur_daily);
PHP_FUNCTION(mcal_event_set_recur_weekly);
PHP_FUNCTION(mcal_event_set_recur_monthly_mday);
PHP_FUNCTION(mcal_event_set_recur_monthly_wday);
PHP_FUNCTION(mcal_event_set_recur_yearly);
PHP_FUNCTION(mcal_fetch_current_stream_event);

#else
#define php_mcal_module_ptr NULL
#endif /* HAVE_MCAL */

#define phpext_mcal_ptr php_mcal_module_ptr

#endif
