/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_define_by_name, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_TYPE_INFO(0, column_name, IS_STRING, 0)
	ZEND_ARG_INFO(0, variable)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_bind_by_name, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_TYPE_INFO(0, column_name, IS_STRING, 0)
	ZEND_ARG_INFO(0, variable)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maximum_length, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_bind_array_by_name, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_TYPE_INFO(0, column_name, IS_STRING, 0)
	ZEND_ARG_INFO(0, variable)
	ZEND_ARG_TYPE_INFO(0, maximum_array_length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maximum_item_length, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "SQLT_AFC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_free_descriptor, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_save, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_import, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_load, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_read, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_oci_lob_eof arginfo_oci_free_descriptor

#define arginfo_oci_lob_tell arginfo_oci_lob_load

#define arginfo_oci_lob_rewind arginfo_oci_free_descriptor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_seek, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, whence, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_oci_lob_size arginfo_oci_lob_load

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_write, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_append, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor_to, OCI_Lob, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor_from, OCI_Lob, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_truncate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_lob_erase, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_flush, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flag, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ocisetbufferinglob, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
	ZEND_ARG_TYPE_INFO(0, mode, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocigetbufferinglob arginfo_oci_free_descriptor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_copy, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor_to, OCI_Lob, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor_from, OCI_Lob, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_is_equal, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor_first, OCI_Lob, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor_second, OCI_Lob, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_export, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor, OCI_Lob, 0)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_oci_new_descriptor, 0, 1, OCI_Lob, 0)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "OCI_DTYPE_LOB")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_rollback, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection_resource)
ZEND_END_ARG_INFO()

#define arginfo_oci_commit arginfo_oci_rollback

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_field_name, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_number_or_name)
ZEND_END_ARG_INFO()

#define arginfo_oci_field_size arginfo_oci_field_name

#define arginfo_oci_field_scale arginfo_oci_field_name

#define arginfo_oci_field_precision arginfo_oci_field_name

#define arginfo_oci_field_type arginfo_oci_field_name

#define arginfo_oci_field_type_raw arginfo_oci_field_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_field_is_null, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, column_number_or_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_internal_debug, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, mode, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_execute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "OCI_COMMIT_ON_SUCCESS")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_cancel, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

#define arginfo_oci_fetch arginfo_oci_cancel

ZEND_BEGIN_ARG_INFO_EX(arginfo_ocifetchinto, 0, 0, 2)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_fetch_all, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_INFO(0, output)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, skip, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maximum_rows, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_fetch_object, 0, 1, IS_OBJECT, 0)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_fetch_row, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

#define arginfo_oci_fetch_assoc arginfo_oci_fetch_row

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_fetch_array, 0, 0, 1)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_oci_free_statement arginfo_oci_cancel

#define arginfo_oci_close arginfo_oci_rollback

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_new_connect, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, connection_string, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, character_set, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, session_mode, IS_LONG, 0, "OCI_DEFAULT")
ZEND_END_ARG_INFO()

#define arginfo_oci_connect arginfo_oci_new_connect

#define arginfo_oci_pconnect arginfo_oci_new_connect

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_error, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, connection_or_statement_resource, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_num_fields, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, statement_resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_parse, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_TYPE_INFO(0, sql_text, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_oci_get_implicit_resultset arginfo_oci_fetch_row

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_prefetch, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement_resource)
	ZEND_ARG_TYPE_INFO(0, number_of_rows, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_client_identifier, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_TYPE_INFO(0, client_identifier, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_edition, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, edition_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_module_name, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_TYPE_INFO(0, module_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_action, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_TYPE_INFO(0, action, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_client_info, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_TYPE_INFO(0, client_information, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_oci_set_db_operation arginfo_oci_set_action

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_call_timeout, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_TYPE_INFO(0, call_timeout, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_password_change, 0, 0, 4)
	ZEND_ARG_INFO(0, connection_resource_or_connection_string)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, old_password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, new_password, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_new_cursor, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_resource)
ZEND_END_ARG_INFO()

#define arginfo_oci_result arginfo_oci_field_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_client_version, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_oci_server_version arginfo_oci_new_cursor

#define arginfo_oci_statement_type arginfo_oci_fetch_row

#define arginfo_oci_num_rows arginfo_oci_fetch_row

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_free_collection, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, collection, OCI_Collection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_collection_append, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, collection, OCI_Collection, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_element_get, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, collection, OCI_Collection, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_collection_assign, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, collection_to, OCI_Collection, 0)
	ZEND_ARG_OBJ_INFO(0, collection_from, OCI_Collection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_collection_element_assign, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, collection, OCI_Collection, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_collection_size, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, collection, OCI_Collection, 0)
ZEND_END_ARG_INFO()

#define arginfo_oci_collection_max arginfo_oci_collection_size

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_collection_trim, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, collection, OCI_Collection, 0)
	ZEND_ARG_TYPE_INFO(0, number, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_new_collection, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_TYPE_INFO(0, type_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, schema_name, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_register_taf_callback, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection_resource)
	ZEND_ARG_INFO(0, function_name)
ZEND_END_ARG_INFO()

#define arginfo_oci_unregister_taf_callback arginfo_oci_rollback

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Lob_save, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Lob_import, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

#define arginfo_class_OCI_Lob_savefile arginfo_class_OCI_Lob_import

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OCI_Lob_load, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OCI_Lob_read, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Lob_eof, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OCI_Lob_tell arginfo_class_OCI_Lob_load

#define arginfo_class_OCI_Lob_rewind arginfo_class_OCI_Lob_eof

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Lob_seek, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, whence, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_OCI_Lob_size arginfo_class_OCI_Lob_load

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OCI_Lob_write, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Lob_append, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob_descriptor_from, OCI_Lob, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Lob_truncate, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OCI_Lob_erase, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Lob_flush, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flag, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Lob_setbuffering, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, mode, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OCI_Lob_getbuffering arginfo_class_OCI_Lob_eof

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Lob_writetofile, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_OCI_Lob_export arginfo_class_OCI_Lob_writetofile

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Lob_writetemporary, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "OCI_TEMP_CLOB")
ZEND_END_ARG_INFO()

#define arginfo_class_OCI_Lob_close arginfo_class_OCI_Lob_eof

#define arginfo_class_OCI_Lob_free arginfo_class_OCI_Lob_eof

#define arginfo_class_OCI_Collection_free arginfo_class_OCI_Lob_eof

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Collection_append, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OCI_Collection_getElem, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Collection_assign, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, collection_from, OCI_Collection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Collection_assignelem, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OCI_Collection_size arginfo_class_OCI_Lob_load

#define arginfo_class_OCI_Collection_max arginfo_class_OCI_Lob_load

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OCI_Collection_trim, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, number, IS_LONG, 0)
ZEND_END_ARG_INFO()
