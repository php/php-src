/*
   +----------------------------------------------------------------------+
   | Muscat/Empower 1.5 Module for PHP version 4.0                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2000 Ananova Ltd                                       |
   | http://www.ananova.com                                               |
   | Ananova Ltd, PO Box 36, Leeds, LS11 5ZA, United Kingdom              |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU General Public License          |
   | as published by the Free Software Foundation; either version 2       |
   | of the License, or (at your option) any later version.               |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 59 Temple Place - Suite 330, Boston,               |
   | MA  02111-1307, USA.                                                 |
   |                                                                      |
   | A copy is availble at http://www.gnu.org/copyleft/gpl.txt            |
   +----------------------------------------------------------------------+
   | Author: Sam Liddicott <sam@ananova.com>                              |
   +----------------------------------------------------------------------+
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_muscat.h"

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_MUSCAT
#include <muscat_handles.h>
#include "../standard/file.h"

/* If you declare any globals in php_muscat.h uncomment this:*/
ZEND_DECLARE_MODULE_GLOBALS(muscat)

/* True global resources - no need for thread safety here */
static int le_muscat;

/* Every user visible function must have an entry in muscat_functions[].
*/
function_entry muscat_functions[] = {
	PHP_FE(muscat_setup,	NULL)
	PHP_FE(muscat_setup_net,	NULL)
	PHP_FE(muscat_give,	NULL)
	PHP_FE(muscat_get,	NULL)
	PHP_FE(muscat_close,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in muscat_functions[] */
};

zend_module_entry muscat_module_entry = {
    STANDARD_MODULE_HEADER,
	"muscat",
	muscat_functions,
	PHP_MINIT(muscat),
	PHP_MSHUTDOWN(muscat),
	PHP_RINIT(muscat),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(muscat),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(muscat),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MUSCAT
ZEND_GET_MODULE(muscat)
#endif

/* {{{ destructor to close and free muscat handles
    */
void _muscat_close(_muscat_handle * handle) {
	if (handle->open) {
	  if (handle->net) _net_muscat_close(&handle->handles.muscatnet_handle);
	  else H_Close_Muscat(&handle->handles.muscat_handle);
	  handle->open=0;
	}
}

/* {{{ destructor to close and free muscat handles
    */
void _muscat_dispose(_muscat_handle * handle) {
	if (handle->open) {
	  _muscat_close(handle);
	}
	efree(handle);
}

/* }}} */

/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
PHP_INI_END()
*/

PHP_MINIT_FUNCTION(muscat)
{
/* Remove comments if you have entries in php.ini
	REGISTER_INI_ENTRIES();
*/
	le_muscat=register_list_destructors(_muscat_dispose, NULL);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(muscat)
{
	return SUCCESS;
}

/* Remove if there's nothing to do at request start */
PHP_RINIT_FUNCTION(muscat)
{
	return SUCCESS;
}

/* Remove if there's nothing to do at request end */
PHP_RSHUTDOWN_FUNCTION(muscat)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(muscat)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "muscat support", "enabled");
	php_info_print_table_row(2, "Muscat/Empower 1.5 Module for PHP version 4.0<br />Copyright (c) 2000 <a href=\"www.ananova.com\">Ananova Ltd<a/>","For information on Muscat/Empower without php see <a href=\"www.smartlogik.com\">www.smartlogik.com</a><br /><small>Muscat and Empower are probably trade marks</small>");
	php_info_print_table_row(2, "muscat path", PHP_MUSCAT_DIR);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}

/* {{{ _local_discard(Muscat_handle * handle) {
	This is not a user function, it throws away all muscat_get output */
void _local_discard(struct Muscat_handle * handle) {
	while(! H_Getfrom_Muscat(handle)) {};
}
/* }}} */

/* {{{ _discard(Muscat_handle * handle) {
	This is not a user function, it throws away all muscat_get output */
void _discard(_muscat_handle * handle) {
	if (handle->net) _net_discard(&handle->handles.muscatnet_handle);
	else _local_discard(&handle->handles.muscat_handle);
}
/* }}} */


/* {{{ proto resource muscat_setup(int size [, string muscat_dir])
   Creates a new muscat session and returns the handle. Size is the ammount of memory in bytes to allocate for muscat muscat_dir is the muscat installation dir e.g. "/usr/local/empower", it defaults to the compile time muscat directory */
PHP_FUNCTION(muscat_setup)
{
	zval **size_arg, **muscat_dir_arg;
	int size;
	int muscat_dir_len;
	char *muscat_dir = NULL;
	int r;

	_muscat_handle *handle=emalloc(sizeof(_muscat_handle));
	handle->net=0;
	handle->open=0;

	switch (ZEND_NUM_ARGS()) {
	  case 1:
	    if (zend_get_parameters_ex(1, &size_arg)==FAILURE) WRONG_PARAM_COUNT;
	    muscat_dir=PHP_MUSCAT_DIR;
	    muscat_dir_len=strlen(muscat_dir);
	    break;
	  case 2:
	    if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &size_arg, &muscat_dir_arg) == FAILURE) WRONG_PARAM_COUNT;
	    convert_to_string_ex(muscat_dir_arg);
	    muscat_dir = Z_STRVAL_PP(muscat_dir_arg);
	    muscat_dir_len = Z_STRLEN_PP(muscat_dir_arg);
	  default:
	    WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(size_arg);
	size = Z_LVAL_PP(size_arg);

	if (r=H_Setup_Muscat(&handle->handles.muscat_handle,size,muscat_dir,"",0)) {
	  handle->open=1;
	  _discard(handle);
	  ZEND_REGISTER_RESOURCE(return_value, handle, le_muscat);
	} else {  // destroy this failure!
	  efree(handle);
	  RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource muscat_setup_net(string muscat_host, int port)
   Creates a new muscat session and returns the handle. muscat_host is the hostname to connect to port is the port number to connect to - actually takes exactly the same args as fsockopen */
PHP_FUNCTION(muscat_setup_net)
{
	zval **socket_arg;
	_muscat_handle * handle;
	void *what;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &socket_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(socket_arg)!=IS_RESOURCE) {
	  zend_error(E_ERROR,"First parameter must be a socket handle");
	  RETURN_FALSE;
	}

	// Must have been successful open
	handle=emalloc(sizeof(_muscat_handle));
	memset(handle,0,sizeof(*handle));
	MAKE_STD_ZVAL(handle->handles.muscatnet_handle.socketr);
	handle->net=1;
	handle->open=1;
	// now need to store this result to keep its reference counted
	*(handle->handles.muscatnet_handle.socketr)=**socket_arg;
	zval_copy_ctor(handle->handles.muscatnet_handle.socketr);

	// but for our convenience extract the FD
	what=zend_fetch_resource(socket_arg TSRMLS_CC,-1,"Socket-Handle",NULL,1,php_file_le_socket());
        ZEND_VERIFY_RESOURCE(what);
	handle->handles.muscatnet_handle.socketd=*(int*)what;
	php_set_sock_blocking(handle->handles.muscatnet_handle.socketd,1);
	_discard(handle);
	ZEND_REGISTER_RESOURCE(return_value, handle, le_muscat);
}

PHP_FUNCTION(muscat_setup_net_)
{
	_muscat_handle * handle;
	void *what;

//	php_fsockopen(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	if ((return_value)->type!=IS_RESOURCE) return;

	// Must have been successful open
	handle=emalloc(sizeof(_muscat_handle));
	memset(handle,0,sizeof(*handle));
	handle->net=1;
	handle->open=1;
	// now need to store this result to keep its reference counted
	*handle->handles.muscatnet_handle.socketr=*return_value;
	zval_copy_ctor(handle->handles.muscatnet_handle.socketr);
	// but for our convenience extract the FD
	what=zend_fetch_resource(&return_value TSRMLS_CC,-1,"File-Handle",NULL,1,php_file_le_socket);
        ZEND_VERIFY_RESOURCE(what);
	handle->handles.muscatnet_handle.socketd=*(int*)what;
	_discard(handle);
	ZEND_REGISTER_RESOURCE(return_value, handle, le_muscat);
}
/* }}} */

/* {{{ proto int muscat_give(resource muscat_handle, string string)
   Sends string to the core muscat api */
PHP_FUNCTION(muscat_give)
{
	zval **Muscat_handle_arg, **string_arg;
	int string_len;
	char *string = NULL;
	_muscat_handle *handle;
	int r;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &Muscat_handle_arg, &string_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(handle, _muscat_handle *,
	  Muscat_handle_arg, -1, MUSCAT_HANDLE_RESOURCE, le_muscat);

	convert_to_string_ex(string_arg);
	string = Z_STRVAL_PP(string_arg);
	string_len = Z_STRLEN_PP(string_arg);
	_discard(handle);
	if (handle->net) r=_net_muscat_give(&handle->handles.muscatnet_handle,string);
	else r=H_Give_Muscat(&handle->handles.muscat_handle,string);
}
/* }}} */

/* {{{ proto string muscat_get(resource muscat_handle)
   Gets a line back from the core muscat api.  Returns a literal FALSE when there is no more to get (as opposed to ""). Use === FALSE or !== FALSE to check for this */
PHP_FUNCTION(muscat_get)
{
	zval **Muscat_handle_arg;
	_muscat_handle *handle;
	int r;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &Muscat_handle_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(handle, _muscat_handle *, 
	  Muscat_handle_arg, -1, MUSCAT_HANDLE_RESOURCE, le_muscat);

	if (handle->net) {  // returns strings for US, no need to dup
	  if (! (r=_net_muscat_get(&handle->handles.muscatnet_handle))) {
	    RETURN_STRINGL(handle->handles.muscatnet_handle.muscat_response.p,
	                   handle->handles.muscatnet_handle.muscat_response.length,0);
	  } else RETURN_FALSE;
	} else { // must dup string
	  if (! (r=H_Getfrom_Muscat(&handle->handles.muscat_handle))) {
	    RETURN_STRINGL(handle->handles.muscat_handle.p,
	                   handle->handles.muscat_handle.length,1);
	  } else RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int muscat_close(resource muscat_handle)
   Shuts down the muscat session and releases any memory back to php. [Not back to the system, note!] */
PHP_FUNCTION(muscat_close)
{
	zval **muscat_handle_arg;
	_muscat_handle *handle;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &muscat_handle_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	if (Z_TYPE_PP(muscat_handle_arg)!=IS_RESOURCE) {
	  zend_error(E_ERROR,"First parameter must be a muscat handle");
	  RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(handle, _muscat_handle *, 
	  muscat_handle_arg, -1, MUSCAT_HANDLE_RESOURCE, le_muscat);

	_muscat_close(handle);
	// Let destructor take care of efree!
	zend_list_delete(Z_LVAL_PP(muscat_handle_arg));
}
/* }}} */


#endif	/* HAVE_MUSCAT */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
