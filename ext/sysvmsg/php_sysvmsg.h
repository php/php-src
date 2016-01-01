/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

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
PHP_MINFO_FUNCTION(sysvmsg);

PHP_FUNCTION(msg_get_queue);
PHP_FUNCTION(msg_remove_queue);
PHP_FUNCTION(msg_stat_queue);
PHP_FUNCTION(msg_set_queue);
PHP_FUNCTION(msg_send);
PHP_FUNCTION(msg_receive);
PHP_FUNCTION(msg_queue_exists);

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
