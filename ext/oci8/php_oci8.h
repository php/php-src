/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |                                                                      |
   | Collection support by Andy Sautins <asautins@veripost.net>           |
   | Temporary LOB support by David Benson <dbenson@mancala.com>          |
   | ZTS per process OCIPLogon by Harald Radi <harald.radi@nme.at>        |
   |                                                                      |
   | Redesigned by: Antony Dovgal <antony@zend.com>                       |
   |                Andi Gutmans <andi@php.net>                           |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/

#if HAVE_OCI8
# ifndef PHP_OCI8_H
#  define PHP_OCI8_H

#ifdef ZTS
# include "TSRM.h"
#endif


/*
 * The version of the OCI8 extension.
 */
#ifdef PHP_OCI8_VERSION
/* The definition of PHP_OCI8_VERSION changed in PHP 5.3 and building
 * this code with PHP 5.2 (e.g. when using OCI8 from PECL) will conflict.
 */
#undef PHP_OCI8_VERSION
#endif
#define PHP_OCI8_VERSION "2.2.0"

extern zend_module_entry oci8_module_entry;
#define phpext_oci8_ptr &oci8_module_entry
#define phpext_oci8_11g_ptr &oci8_module_entry
#define phpext_oci8_12c_ptr &oci8_module_entry


PHP_MINIT_FUNCTION(oci);
PHP_RINIT_FUNCTION(oci);
PHP_MSHUTDOWN_FUNCTION(oci);
PHP_RSHUTDOWN_FUNCTION(oci);
PHP_MINFO_FUNCTION(oci);

# endif /* !PHP_OCI8_H */
#else /* !HAVE_OCI8 */

# define oci8_module_ptr NULL

#endif /* HAVE_OCI8 */
