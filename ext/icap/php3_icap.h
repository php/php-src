/* $Id$ */

#ifndef _INCLUDED_ICAP_H
#define _INCLUDED_ICAP_H

#if COMPILE_DL
#undef HAVE_ICAP
#define HAVE_ICAP 1
#endif

#if HAVE_ICAP

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#ifndef MSVC5
#include "build-defs.h"
#endif

typedef struct calendar_stream {
int id;
} CALENDARSTREAM;

/* Functions accessable to PHP */
extern php3_module_entry php3_icap_module_entry;
#define php3_icap_module_ptr &php3_icap_module_entry

extern int icap_init(INIT_FUNC_ARGS);
extern int icap_init_request(INIT_FUNC_ARGS);
extern int icap_end_request(void);
extern void icap_info(void);
void php3_icap_open(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_popen(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_reopen(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_close(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_fetch_event(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_list_events(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_create_calendar(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_rename_calendar(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_delete_calendar(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_store_event(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_delete_event(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_snooze(INTERNAL_FUNCTION_PARAMETERS);
void php3_icap_list_alarms(INTERNAL_FUNCTION_PARAMETERS);





#else
#define php3_icap_module_ptr NULL
#endif /* HAVE_ICAP */


#endif



