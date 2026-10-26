/*
   +----------------------------------------------------------------------+
   | Thread Safe Resource Manager                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1999-2011, Andi Gutmans, Sascha Schumann, Zeev Suraski |
   | This source file is subject to the TSRM license, that is bundled     |
   | with this package in the file LICENSE                                |
   +----------------------------------------------------------------------+
   | Authors:  Marc Henderkes <m@pyc.ac>                                  |
   +----------------------------------------------------------------------+
*/

#ifndef TSRM_ATEXIT_H
#define TSRM_ATEXIT_H

#include "TSRM.h"

#ifdef ZTS

typedef struct _tsrm_tls_entry tsrm_tls_entry;

void tsrm_atexit_init(void);
void tsrm_atexit_set_data(tsrm_tls_entry *thread_resources);
void tsrm_atexit_disarm(void);
void tsrm_free_thread_at_exit(tsrm_tls_entry *thread_resources);

#endif /* ZTS */

#endif /* TSRM_ATEXIT_H */
