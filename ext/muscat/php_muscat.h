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


#ifndef PHP_MUSCAT_H
#define PHP_MUSCAT_H

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_MUSCAT
#include <muscat.h>

#define MUSCAT_HANDLE_RESOURCE "muscat handle"

extern zend_module_entry muscat_module_entry;
#define phpext_muscat_ptr &muscat_module_entry

#ifdef PHP_WIN32
#define PHP_MUSCAT_API __declspec(dllexport)
#else
#define PHP_MUSCAT_API
#endif

PHP_MINIT_FUNCTION(muscat);
PHP_MSHUTDOWN_FUNCTION(muscat);
PHP_RINIT_FUNCTION(muscat);
PHP_RSHUTDOWN_FUNCTION(muscat);
PHP_MINFO_FUNCTION(muscat);

PHP_FUNCTION(muscat_setup);
PHP_FUNCTION(muscat_setup_net);
PHP_FUNCTION(muscat_give);
PHP_FUNCTION(muscat_get);
PHP_FUNCTION(muscat_close);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     
*/
ZEND_BEGIN_MODULE_GLOBALS(muscat)
ZEND_END_MODULE_GLOBALS(muscat)

/* In every function that needs to use variables in php_muscat_globals,
   do call MUSCATLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as MUSCATG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MUSCATG(v) (muscat_globals->v)
#define MUSCATLS_FETCH() php_muscat_globals *muscat_globals = ts_resource(muscat_globals_id)
#else
#define MUSCATG(v) (muscat_globals.v)
#define MUSCATLS_FETCH()
#endif

#else

#define phpext_muscat_ptr NULL

#endif

#endif	/* PHP_MUSCAT_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
