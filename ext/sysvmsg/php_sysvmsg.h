/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SYSVMSG_H
#define PHP_SYSVMSG_H

#if HAVE_SYSVMSG

extern zend_module_entry sysvmsg_module_entry;
#define phpext_sysvmsg_ptr &sysvmsg_module_entry

#ifndef __USE_GNU
/* we want to use mtype instead of __mtype */
#define __USE_GNU
#endif

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(sysvmsg);
PHP_MSHUTDOWN_FUNCTION(sysvmsg);
PHP_RINIT_FUNCTION(sysvmsg);
PHP_RSHUTDOWN_FUNCTION(sysvmsg);
PHP_MINFO_FUNCTION(sysvmsg);

PHP_FUNCTION(msg_get_queue);
PHP_FUNCTION(msg_remove_queue);
PHP_FUNCTION(msg_stat_queue);
PHP_FUNCTION(msg_set_queue);
PHP_FUNCTION(msg_send);
PHP_FUNCTION(msg_receive);

typedef struct {
	key_t key;
	long id;
} sysvmsg_queue_t;

struct php_msgbuf {
	long mtype;
	char mtext[1];
};

#endif /* HAVE_SYSVMSG */

#endif	/* PHP_SYSVMSG_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
