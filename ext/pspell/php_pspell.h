/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Vlad Krupin <phpdevel@echospace.com>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef _PSPELL_H
#define _PSPELL_H
#if HAVE_PSPELL
extern zend_module_entry pspell_module_entry;
#define pspell_module_ptr &pspell_module_entry

PHP_MINIT_FUNCTION(pspell);
PHP_MINFO_FUNCTION(pspell);
PHP_FUNCTION(pspell_new);
PHP_FUNCTION(pspell_new_personal);
PHP_FUNCTION(pspell_new_config);
PHP_FUNCTION(pspell_check);
PHP_FUNCTION(pspell_suggest);
PHP_FUNCTION(pspell_store_replacement);
PHP_FUNCTION(pspell_add_to_personal);
PHP_FUNCTION(pspell_add_to_session);
PHP_FUNCTION(pspell_clear_session);
PHP_FUNCTION(pspell_save_wordlist);
PHP_FUNCTION(pspell_config_create);
PHP_FUNCTION(pspell_config_runtogether);
PHP_FUNCTION(pspell_config_mode);
PHP_FUNCTION(pspell_config_ignore);
PHP_FUNCTION(pspell_config_personal);
PHP_FUNCTION(pspell_config_repl);
PHP_FUNCTION(pspell_config_save_repl);
#else
#define pspell_module_ptr NULL
#endif

#define phpext_pspell_ptr pspell_module_ptr

#endif /* _PSPELL_H */
