/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_01.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Brad Atkins <brad@youreshop.com>                            |
   +----------------------------------------------------------------------+
*/


/* $Id$ */

extern zend_module_entry notes_module_entry;

#define notes_module_ptr &notes_module_entry

extern PHP_MINIT_FUNCTION(notes);
extern PHP_MSHUTDOWN_FUNCTION(notes);
extern PHP_RINIT_FUNCTION(notes);
extern PHP_RSHUTDOWN_FUNCTION(notes);
PHP_MINFO_FUNCTION(notes);

PHP_FUNCTION(notes_create_db);
PHP_FUNCTION(notes_drop_db);
PHP_FUNCTION(notes_version);
PHP_FUNCTION(notes_create_note);
PHP_FUNCTION(notes_unread);
PHP_FUNCTION(notes_mark_read);
PHP_FUNCTION(notes_mark_unread);
PHP_FUNCTION(notes_header_info);
PHP_FUNCTION(notes_body);
PHP_FUNCTION(notes_find_note);
PHP_FUNCTION(notes_nav_create);
PHP_FUNCTION(notes_search);
PHP_FUNCTION(notes_copy_db);
PHP_FUNCTION(notes_list_msgs);

#define phpext_notes_ptr notes_module_ptr
