/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SOCKETS_H
#define PHP_SOCKETS_H

/* $Id$ */

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_SOCKETS

extern zend_module_entry sockets_module_entry;
#define phpext_sockets_ptr &sockets_module_entry

#ifdef PHP_WIN32
#define PHP_SOCKETS_API __declspec(dllexport)
#else
#define PHP_SOCKETS_API
#endif

PHP_MINIT_FUNCTION(sockets);
PHP_MSHUTDOWN_FUNCTION(sockets);
PHP_RINIT_FUNCTION(sockets);
PHP_RSHUTDOWN_FUNCTION(sockets);
PHP_MINFO_FUNCTION(sockets);

PHP_FUNCTION(confirm_sockets_compiled);	/* For testing, remove later. */
PHP_FUNCTION(confirm_sockets_compiled);
PHP_FUNCTION(fd_alloc);
PHP_FUNCTION(fd_dealloc);
PHP_FUNCTION(fd_set);
PHP_FUNCTION(fd_isset);
PHP_FUNCTION(fd_clear);
PHP_FUNCTION(fd_zero);
PHP_FUNCTION(select);
PHP_FUNCTION(open_listen_sok);
PHP_FUNCTION(accept_connect);
PHP_FUNCTION(set_nonblock);
PHP_FUNCTION(listen);
PHP_FUNCTION(close);
PHP_FUNCTION(write);
PHP_FUNCTION(read);
PHP_FUNCTION(signal);
PHP_FUNCTION(getsockname);
PHP_FUNCTION(getpeername);
PHP_FUNCTION(socket);
PHP_FUNCTION(connect);
PHP_FUNCTION(strerror);
PHP_FUNCTION(bind);

/* Fill in this structure and use entries in it
   for thread safety instead of using true globals.
*/
typedef struct {
	/* You can use the next one as type if your module registers any
	   resources. Oh, you can of course rename it to something more
	   suitable, add list entry types or remove it if it not needed.
	   It's just an example.
	*/
	int le_sockets;
} php_sockets_globals;

/* In every function that needs to use variables in php_sockets_globals,
   do call SOCKETSLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as SOCKETSG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define SOCKETSG(v) (sockets_globals->v)
#define SOCKETSLS_FETCH() php_sockets_globals *sockets_globals = ts_resource(gd_sockets_id)
#else
#define SOCKETSG(v) (sockets_globals.v)
#define SOCKETSLS_FETCH()
#endif

#else

#define phpext_sockets_ptr NULL

#endif

#endif	/* PHP_SOCKETS_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
