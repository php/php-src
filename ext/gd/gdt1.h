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
   | Author: Jouni Ahto <jah@mork.net>                                    |
   +----------------------------------------------------------------------+
 */

/* 	$Id$	 */

#if HAVE_LIBT1

#include <t1lib.h>

extern void php_free_ps_font(zend_rsrc_list_entry *rsrc TSRMLS_DC);
extern void php_free_ps_enc(zend_rsrc_list_entry *rsrc TSRMLS_DC);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
