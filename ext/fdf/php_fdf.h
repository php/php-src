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
   | Authors: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_FDF_H
#define PHP_FDF_H

#if HAVE_FDFLIB
#ifdef PHP_WIN32
#else
#define UNIX_DEV
#endif

#if HAVE_FDFTK_H_LOWER
# include <fdftk.h>
#else
# include <FdfTk.h>
#endif

ZEND_BEGIN_MODULE_GLOBALS(fdf)
	FDFErc  error;
	zval   *enum_callback;
	FDFDoc enum_fdf;
ZEND_END_MODULE_GLOBALS(fdf)

#ifdef ZTS
#define FDF_G(v) TSRMG(fdf_globals_id, zend_fdf_globals *, v)
#else
#define FDF_G(v) (fdf_globals.v)
#endif


extern zend_module_entry fdf_module_entry;
#define fdf_module_ptr &fdf_module_entry

PHP_MINIT_FUNCTION(fdf);
PHP_MSHUTDOWN_FUNCTION(fdf);
PHP_RINIT_FUNCTION(fdf);
PHP_MINFO_FUNCTION(fdf);

PHP_FUNCTION(fdf_open);
PHP_FUNCTION(fdf_open_string);
PHP_FUNCTION(fdf_close);
PHP_FUNCTION(fdf_create);
PHP_FUNCTION(fdf_save);
PHP_FUNCTION(fdf_save_string);
PHP_FUNCTION(fdf_get_value);
PHP_FUNCTION(fdf_set_value);
PHP_FUNCTION(fdf_next_field_name);
PHP_FUNCTION(fdf_set_ap);
PHP_FUNCTION(fdf_get_ap);
PHP_FUNCTION(fdf_get_status);
PHP_FUNCTION(fdf_set_status);
PHP_FUNCTION(fdf_set_file);
PHP_FUNCTION(fdf_get_file);
PHP_FUNCTION(fdf_add_template);
PHP_FUNCTION(fdf_set_flags);
PHP_FUNCTION(fdf_get_flags);
PHP_FUNCTION(fdf_set_opt);
PHP_FUNCTION(fdf_get_opt);
PHP_FUNCTION(fdf_set_submit_form_action);
PHP_FUNCTION(fdf_set_javascript_action);
PHP_FUNCTION(fdf_add_doc_javascript);
PHP_FUNCTION(fdf_set_on_import_javascript);
PHP_FUNCTION(fdf_set_encoding);
PHP_FUNCTION(fdf_get_encoding);
PHP_FUNCTION(fdf_set_version);
PHP_FUNCTION(fdf_get_version);
PHP_FUNCTION(fdf_set_target_frame);
PHP_FUNCTION(fdf_errno);
PHP_FUNCTION(fdf_error);
PHP_FUNCTION(fdf_remove_item);
PHP_FUNCTION(fdf_get_attachment);
PHP_FUNCTION(fdf_enum_values);
PHP_FUNCTION(fdf_header);
#else
#define fdf_module_ptr NULL
#endif
#define phpext_fdf_ptr fdf_module_ptr
#endif /* PHP_FDF_H */
