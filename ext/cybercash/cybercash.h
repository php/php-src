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
   | Authors: Evan Klinger <evan715@sirius.com>                           |
   +----------------------------------------------------------------------+
 */

#ifndef CYBERCASH_H
#define CYBERCASH_H


#if HAVE_MCK

extern zend_module_entry cybercash_module_entry;
#define cybercash_module_ptr &cybercash_module_entry

PHP_MINFO_FUNCTION(cybercash);
PHP_FUNCTION(cybercash_encr);
PHP_FUNCTION(cybercash_decr);
PHP_FUNCTION(cybercash_base64_encode);
PHP_FUNCTION(cybercash_base64_decode);

#else
#define cybercash_module_ptr NULL
#endif

#define phpext_cybercash_ptr cybercash_module_ptr

#endif
