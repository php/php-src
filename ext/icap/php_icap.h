/* $Id$ */

#ifndef PHP_ICAP_H
#define PHP_ICAP_H

#if HAVE_ICAP

#ifndef PHP_WIN32
#include "build-defs.h"
#endif

extern PHP_MINIT_FUNCTION(icap);
PHP_MINFO_FUNCTION(icap);

/* Functions accessable to PHP */
extern zend_module_entry php_icap_module_entry;
#define php_icap_module_ptr &php_icap_module_entry
#define phpext_icap_ptr php_icap_module_ptr

extern int icap_init_request(INIT_FUNC_ARGS);
extern int icap_end_request(void);
PHP_FUNCTION(icap_open);
PHP_FUNCTION(icap_popen);
PHP_FUNCTION(icap_reopen);
PHP_FUNCTION(icap_close);
PHP_FUNCTION(icap_fetch_event);
PHP_FUNCTION(icap_list_events);
PHP_FUNCTION(icap_create_calendar);
PHP_FUNCTION(icap_rename_calendar);
PHP_FUNCTION(icap_delete_calendar);
PHP_FUNCTION(icap_store_event);
PHP_FUNCTION(icap_delete_event);
PHP_FUNCTION(icap_snooze);
PHP_FUNCTION(icap_list_alarms);


#else
#define php_icap_module_ptr NULL
#endif /* HAVE_ICAP */


#endif
