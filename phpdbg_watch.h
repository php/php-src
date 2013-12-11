#ifndef PHPDBG_WATCH_H
#define PHPDBG_WATCH_H

#include "TSRM.h"
#include "phpdbg_cmd.h"

#define PHPDBG_WATCH(name) PHPDBG_COMMAND(watch_##name)

/**
* Printer Forward Declarations
*/
/*PHPDBG_WATCH();*/


/* Watchpoint functions/typedefs */

typedef enum {
	WATCH_ON_ZVAL,
	WATCH_ON_HASHTABLE,
	WATCH_ON_PTR
} phpdbg_watchtype;

typedef struct _phpdbg_watchpoint_t phpdbg_watchpoint_t;

struct _phpdbg_watchpoint_t {
	phpdbg_watchpoint_t *parent;
	char *str;
	union {
		zval *zv;
		HashTable *ht;
		void *ptr;
	} addr;
	int size;
	phpdbg_watchtype type;
};

void phpdbg_setup_watchpoints();

int phpdbg_watchpoint_segfault_handler(siginfo_t *info, void *context TSRMLS_DC);

int phpdbg_create_var_watchpoint(char *name, size_t len TSRMLS_DC);

#endif
