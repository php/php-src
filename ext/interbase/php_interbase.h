/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jouni Ahto <jouni.ahto@exdec.fi>                            |
   |          Andrew Avdeev <andy@simgts.mv.ru>                           |
   |          Ard Biesheuvel <a.k.biesheuvel@its.tudelft.nl>              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_INTERBASE_H
#define PHP_INTERBASE_H

extern zend_module_entry ibase_module_entry;
#define phpext_interbase_ptr &ibase_module_entry

PHP_MINIT_FUNCTION(ibase);
PHP_RINIT_FUNCTION(ibase);
PHP_MSHUTDOWN_FUNCTION(ibase);
PHP_RSHUTDOWN_FUNCTION(ibase);
PHP_MINFO_FUNCTION(ibase);

PHP_FUNCTION(ibase_connect);
PHP_FUNCTION(ibase_pconnect);
PHP_FUNCTION(ibase_close);
PHP_FUNCTION(ibase_drop_db);
PHP_FUNCTION(ibase_query);
PHP_FUNCTION(ibase_fetch_row);
PHP_FUNCTION(ibase_fetch_assoc);
PHP_FUNCTION(ibase_fetch_object);
PHP_FUNCTION(ibase_free_result);
PHP_FUNCTION(ibase_name_result);
PHP_FUNCTION(ibase_prepare);
PHP_FUNCTION(ibase_execute);
PHP_FUNCTION(ibase_free_query);

PHP_FUNCTION(ibase_timefmt);

PHP_FUNCTION(ibase_gen_id);
PHP_FUNCTION(ibase_num_fields);
PHP_FUNCTION(ibase_num_params);
#if abies_0
PHP_FUNCTION(ibase_num_rows);
#endif
PHP_FUNCTION(ibase_affected_rows);
PHP_FUNCTION(ibase_field_info);
PHP_FUNCTION(ibase_param_info);

PHP_FUNCTION(ibase_trans);
PHP_FUNCTION(ibase_commit);
PHP_FUNCTION(ibase_rollback);
PHP_FUNCTION(ibase_commit_ret);
PHP_FUNCTION(ibase_rollback_ret);

PHP_FUNCTION(ibase_blob_create);
PHP_FUNCTION(ibase_blob_add);
PHP_FUNCTION(ibase_blob_cancel);
PHP_FUNCTION(ibase_blob_open);
PHP_FUNCTION(ibase_blob_get);
PHP_FUNCTION(ibase_blob_close);
PHP_FUNCTION(ibase_blob_echo);
PHP_FUNCTION(ibase_blob_info);
PHP_FUNCTION(ibase_blob_import);

PHP_FUNCTION(ibase_add_user);
PHP_FUNCTION(ibase_modify_user);
PHP_FUNCTION(ibase_delete_user);

PHP_FUNCTION(ibase_service_attach);
PHP_FUNCTION(ibase_service_detach);
PHP_FUNCTION(ibase_backup);
PHP_FUNCTION(ibase_restore);
PHP_FUNCTION(ibase_maintain_db);
PHP_FUNCTION(ibase_db_info);
PHP_FUNCTION(ibase_server_info);

PHP_FUNCTION(ibase_errmsg);
PHP_FUNCTION(ibase_errcode);

PHP_FUNCTION(ibase_wait_event);
PHP_FUNCTION(ibase_set_event_handler);
PHP_FUNCTION(ibase_free_event_handler);

#else

#define phpext_interbase_ptr NULL

#endif /* PHP_INTERBASE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
