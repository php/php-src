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

extern PHP_MINIT_FUNCTION(mcal);
PHP_MINFO_FUNCTION(mcal);

/* Functions accessable to PHP */
extern php3_module_entry php3_mcal_module_entry;
#define php3_mcal_module_ptr &php3_mcal_module_entry
#define phpext_mcal_ptr php3_mcal_module_ptr

extern int mcal_init_request(INIT_FUNC_ARGS);
extern int mcal_end_request(void);
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





#else
#define php3_mcal_module_ptr NULL
#endif /* HAVE_MCAL */


#endif




