/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 850d4e63296c95a27cdfc1244d63b1ed496acd54 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_define_by_name, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, var, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_ocidefinebyname arginfo_oci_define_by_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_bind_by_name, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO(0, param, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(1, var, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, max_length, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_ocibindbyname arginfo_oci_bind_by_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_bind_array_by_name, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO(0, param, IS_STRING, 0)
	ZEND_ARG_INFO(1, var)
	ZEND_ARG_TYPE_INFO(0, max_array_length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, max_item_length, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "SQLT_AFC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_free_descriptor, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocifreedesc arginfo_oci_free_descriptor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_save, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_ocisavelob arginfo_oci_lob_save

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_import, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocisavelobfile arginfo_oci_lob_import

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_lob_load, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
ZEND_END_ARG_INFO()

#define arginfo_ociloadlob arginfo_oci_lob_load

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_lob_read, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_oci_lob_eof arginfo_oci_free_descriptor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_lob_tell, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
ZEND_END_ARG_INFO()

#define arginfo_oci_lob_rewind arginfo_oci_free_descriptor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_seek, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, whence, IS_LONG, 0, "OCI_SEEK_SET")
ZEND_END_ARG_INFO()

#define arginfo_oci_lob_size arginfo_oci_lob_tell

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_lob_write, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_append, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, to, OCILob, 0)
	ZEND_ARG_OBJ_INFO(0, from, OCILob, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_truncate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_lob_erase, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_flush, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flag, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ocisetbufferinglob, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
	ZEND_ARG_TYPE_INFO(0, mode, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocigetbufferinglob arginfo_oci_free_descriptor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_copy, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, to, OCILob, 0)
	ZEND_ARG_OBJ_INFO(0, from, OCILob, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_is_equal, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob1, OCILob, 0)
	ZEND_ARG_OBJ_INFO(0, lob2, OCILob, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_lob_export, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, OCILob, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_ociwritelobtofile arginfo_oci_lob_export

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_oci_new_descriptor, 0, 1, OCILob, 1)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "OCI_DTYPE_LOB")
ZEND_END_ARG_INFO()

#define arginfo_ocinewdescriptor arginfo_oci_new_descriptor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_rollback, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

#define arginfo_ocirollback arginfo_oci_rollback

#define arginfo_oci_commit arginfo_oci_rollback

#define arginfo_ocicommit arginfo_oci_rollback

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_field_name, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_MASK(0, column, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_ocicolumnname arginfo_oci_field_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_field_size, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_MASK(0, column, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_ocicolumnsize arginfo_oci_field_size

#define arginfo_oci_field_scale arginfo_oci_field_size

#define arginfo_ocicolumnscale arginfo_oci_field_size

#define arginfo_oci_field_precision arginfo_oci_field_size

#define arginfo_ocicolumnprecision arginfo_oci_field_size

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_field_type, 0, 2, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_MASK(0, column, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_ocicolumntype arginfo_oci_field_type

#define arginfo_oci_field_type_raw arginfo_oci_field_size

#define arginfo_ocicolumntyperaw arginfo_oci_field_size

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_field_is_null, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_MASK(0, column, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_ocicolumnisnull arginfo_oci_field_is_null

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_execute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "OCI_COMMIT_ON_SUCCESS")
ZEND_END_ARG_INFO()

#define arginfo_ociexecute arginfo_oci_execute

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_cancel, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
ZEND_END_ARG_INFO()

#define arginfo_ocicancel arginfo_oci_cancel

#define arginfo_oci_fetch arginfo_oci_cancel

#define arginfo_ocifetch arginfo_oci_cancel

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ocifetchinto, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_INFO(1, result)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "OCI_NUM")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_fetch_all, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_INFO(1, output)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "OCI_FETCHSTATEMENT_BY_COLUMN | OCI_ASSOC")
ZEND_END_ARG_INFO()

#define arginfo_ocifetchstatement arginfo_oci_fetch_all

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_oci_fetch_object, 0, 1, stdClass, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "OCI_ASSOC | OCI_RETURN_NULLS")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_fetch_row, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
ZEND_END_ARG_INFO()

#define arginfo_oci_fetch_assoc arginfo_oci_fetch_row

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_fetch_array, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "OCI_BOTH | OCI_RETURN_NULLS")
ZEND_END_ARG_INFO()

#define arginfo_oci_free_statement arginfo_oci_cancel

#define arginfo_ocifreestatement arginfo_oci_cancel

#define arginfo_oci_free_cursor arginfo_oci_cancel

#define arginfo_ocifreecursor arginfo_oci_cancel

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_close, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

#define arginfo_ocilogoff arginfo_oci_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_new_connect, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, connection_string, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, session_mode, IS_LONG, 0, "OCI_DEFAULT")
ZEND_END_ARG_INFO()

#define arginfo_ocinlogon arginfo_oci_new_connect

#define arginfo_oci_connect arginfo_oci_new_connect

#define arginfo_ocilogon arginfo_oci_new_connect

#define arginfo_oci_pconnect arginfo_oci_new_connect

#define arginfo_ociplogon arginfo_oci_new_connect

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_error, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, connection_or_statement, "null")
ZEND_END_ARG_INFO()

#define arginfo_ocierror arginfo_oci_error

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_num_fields, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, statement)
ZEND_END_ARG_INFO()

#define arginfo_ocinumcols arginfo_oci_num_fields

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_parse, 0, 0, 2)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, sql, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ociparse arginfo_oci_parse

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_get_implicit_resultset, 0, 0, 1)
	ZEND_ARG_INFO(0, statement)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_prefetch, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO(0, rows, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocisetprefetch arginfo_oci_set_prefetch

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_prefetch_lob, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO(0, prefetch_lob_size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_client_identifier, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, client_id, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_edition, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, edition, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_module_name, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_action, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, action, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_client_info, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, client_info, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_oci_set_db_operation arginfo_oci_set_action

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_set_call_timeout, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_password_change, 0, 0, 4)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, old_password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, new_password, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocipasswordchange arginfo_oci_password_change

ZEND_BEGIN_ARG_INFO_EX(arginfo_oci_new_cursor, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

#define arginfo_ocinewcursor arginfo_oci_new_cursor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_result, 0, 2, IS_MIXED, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_MASK(0, column, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_ociresult arginfo_oci_result

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_client_version, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_server_version, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

#define arginfo_ociserverversion arginfo_oci_server_version

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_statement_type, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
ZEND_END_ARG_INFO()

#define arginfo_ocistatementtype arginfo_oci_statement_type

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_num_rows, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
ZEND_END_ARG_INFO()

#define arginfo_ocirowcount arginfo_oci_num_rows

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_free_collection, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, collection, OCICollection, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocifreecollection arginfo_oci_free_collection

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_collection_append, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, collection, OCICollection, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocicollappend arginfo_oci_collection_append

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_collection_element_get, 0, 2, MAY_BE_STRING|MAY_BE_DOUBLE|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, collection, OCICollection, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocicollgetelem arginfo_oci_collection_element_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_collection_assign, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, to, OCICollection, 0)
	ZEND_ARG_OBJ_INFO(0, from, OCICollection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_collection_element_assign, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, collection, OCICollection, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocicollassignelem arginfo_oci_collection_element_assign

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_oci_collection_size, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, collection, OCICollection, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocicollsize arginfo_oci_collection_size

#define arginfo_oci_collection_max arginfo_oci_collection_size

#define arginfo_ocicollmax arginfo_oci_collection_size

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_collection_trim, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, collection, OCICollection, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_ocicolltrim arginfo_oci_collection_trim

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_oci_new_collection, 0, 2, OCICollection, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, type_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, schema, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_ocinewcollection arginfo_oci_new_collection

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_oci_register_taf_callback, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 1)
ZEND_END_ARG_INFO()

#define arginfo_oci_unregister_taf_callback arginfo_oci_rollback

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCILob_save, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCILob_import, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OCILob_saveFile arginfo_class_OCILob_import

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_OCILob_load, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_OCILob_read, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCILob_eof, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_OCILob_tell, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_OCILob_rewind arginfo_class_OCILob_eof

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCILob_seek, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, whence, IS_LONG, 0, "OCI_SEEK_SET")
ZEND_END_ARG_INFO()

#define arginfo_class_OCILob_size arginfo_class_OCILob_tell

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_OCILob_write, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCILob_append, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, from, OCILob, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCILob_truncate, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_OCILob_erase, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCILob_flush, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flag, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCILob_setBuffering, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, mode, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OCILob_getBuffering arginfo_class_OCILob_eof

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCILob_writeToFile, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_OCILob_export arginfo_class_OCILob_writeToFile

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCILob_writeTemporary, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "OCI_TEMP_CLOB")
ZEND_END_ARG_INFO()

#define arginfo_class_OCILob_close arginfo_class_OCILob_eof

#define arginfo_class_OCILob_free arginfo_class_OCILob_eof

#define arginfo_class_OCICollection_free arginfo_class_OCILob_eof

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCICollection_append, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_OCICollection_getElem, 0, 1, MAY_BE_STRING|MAY_BE_DOUBLE|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCICollection_assign, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, from, OCICollection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCICollection_assignElem, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OCICollection_size arginfo_class_OCILob_tell

#define arginfo_class_OCICollection_max arginfo_class_OCILob_tell

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_OCICollection_trim, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(oci_define_by_name);
ZEND_FUNCTION(oci_bind_by_name);
ZEND_FUNCTION(oci_bind_array_by_name);
ZEND_FUNCTION(oci_free_descriptor);
ZEND_FUNCTION(oci_lob_save);
ZEND_FUNCTION(oci_lob_import);
ZEND_FUNCTION(oci_lob_load);
ZEND_FUNCTION(oci_lob_read);
ZEND_FUNCTION(oci_lob_eof);
ZEND_FUNCTION(oci_lob_tell);
ZEND_FUNCTION(oci_lob_rewind);
ZEND_FUNCTION(oci_lob_seek);
ZEND_FUNCTION(oci_lob_size);
ZEND_FUNCTION(oci_lob_write);
ZEND_FUNCTION(oci_lob_append);
ZEND_FUNCTION(oci_lob_truncate);
ZEND_FUNCTION(oci_lob_erase);
ZEND_FUNCTION(oci_lob_flush);
ZEND_FUNCTION(ocisetbufferinglob);
ZEND_FUNCTION(ocigetbufferinglob);
ZEND_FUNCTION(oci_lob_copy);
ZEND_FUNCTION(oci_lob_is_equal);
ZEND_FUNCTION(oci_lob_export);
ZEND_FUNCTION(oci_new_descriptor);
ZEND_FUNCTION(oci_rollback);
ZEND_FUNCTION(oci_commit);
ZEND_FUNCTION(oci_field_name);
ZEND_FUNCTION(oci_field_size);
ZEND_FUNCTION(oci_field_scale);
ZEND_FUNCTION(oci_field_precision);
ZEND_FUNCTION(oci_field_type);
ZEND_FUNCTION(oci_field_type_raw);
ZEND_FUNCTION(oci_field_is_null);
ZEND_FUNCTION(oci_execute);
ZEND_FUNCTION(oci_cancel);
ZEND_FUNCTION(oci_fetch);
ZEND_FUNCTION(ocifetchinto);
ZEND_FUNCTION(oci_fetch_all);
ZEND_FUNCTION(oci_fetch_object);
ZEND_FUNCTION(oci_fetch_row);
ZEND_FUNCTION(oci_fetch_assoc);
ZEND_FUNCTION(oci_fetch_array);
ZEND_FUNCTION(oci_free_statement);
ZEND_FUNCTION(oci_close);
ZEND_FUNCTION(oci_new_connect);
ZEND_FUNCTION(oci_connect);
ZEND_FUNCTION(oci_pconnect);
ZEND_FUNCTION(oci_error);
ZEND_FUNCTION(oci_num_fields);
ZEND_FUNCTION(oci_parse);
ZEND_FUNCTION(oci_get_implicit_resultset);
ZEND_FUNCTION(oci_set_prefetch);
ZEND_FUNCTION(oci_set_prefetch_lob);
ZEND_FUNCTION(oci_set_client_identifier);
ZEND_FUNCTION(oci_set_edition);
ZEND_FUNCTION(oci_set_module_name);
ZEND_FUNCTION(oci_set_action);
ZEND_FUNCTION(oci_set_client_info);
ZEND_FUNCTION(oci_set_db_operation);
ZEND_FUNCTION(oci_set_call_timeout);
ZEND_FUNCTION(oci_password_change);
ZEND_FUNCTION(oci_new_cursor);
ZEND_FUNCTION(oci_result);
ZEND_FUNCTION(oci_client_version);
ZEND_FUNCTION(oci_server_version);
ZEND_FUNCTION(oci_statement_type);
ZEND_FUNCTION(oci_num_rows);
ZEND_FUNCTION(oci_free_collection);
ZEND_FUNCTION(oci_collection_append);
ZEND_FUNCTION(oci_collection_element_get);
ZEND_FUNCTION(oci_collection_assign);
ZEND_FUNCTION(oci_collection_element_assign);
ZEND_FUNCTION(oci_collection_size);
ZEND_FUNCTION(oci_collection_max);
ZEND_FUNCTION(oci_collection_trim);
ZEND_FUNCTION(oci_new_collection);
ZEND_FUNCTION(oci_register_taf_callback);
ZEND_FUNCTION(oci_unregister_taf_callback);
ZEND_METHOD(OCILob, writeTemporary);
ZEND_METHOD(OCILob, close);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(oci_define_by_name, arginfo_oci_define_by_name)
	ZEND_DEP_FALIAS(ocidefinebyname, oci_define_by_name, arginfo_ocidefinebyname)
	ZEND_FE(oci_bind_by_name, arginfo_oci_bind_by_name)
	ZEND_DEP_FALIAS(ocibindbyname, oci_bind_by_name, arginfo_ocibindbyname)
	ZEND_FE(oci_bind_array_by_name, arginfo_oci_bind_array_by_name)
	ZEND_FE(oci_free_descriptor, arginfo_oci_free_descriptor)
	ZEND_DEP_FALIAS(ocifreedesc, oci_free_descriptor, arginfo_ocifreedesc)
	ZEND_FE(oci_lob_save, arginfo_oci_lob_save)
	ZEND_DEP_FALIAS(ocisavelob, oci_lob_save, arginfo_ocisavelob)
	ZEND_FE(oci_lob_import, arginfo_oci_lob_import)
	ZEND_DEP_FALIAS(ocisavelobfile, oci_lob_import, arginfo_ocisavelobfile)
	ZEND_FE(oci_lob_load, arginfo_oci_lob_load)
	ZEND_DEP_FALIAS(ociloadlob, oci_lob_load, arginfo_ociloadlob)
	ZEND_FE(oci_lob_read, arginfo_oci_lob_read)
	ZEND_FE(oci_lob_eof, arginfo_oci_lob_eof)
	ZEND_FE(oci_lob_tell, arginfo_oci_lob_tell)
	ZEND_FE(oci_lob_rewind, arginfo_oci_lob_rewind)
	ZEND_FE(oci_lob_seek, arginfo_oci_lob_seek)
	ZEND_FE(oci_lob_size, arginfo_oci_lob_size)
	ZEND_FE(oci_lob_write, arginfo_oci_lob_write)
	ZEND_FE(oci_lob_append, arginfo_oci_lob_append)
	ZEND_FE(oci_lob_truncate, arginfo_oci_lob_truncate)
	ZEND_FE(oci_lob_erase, arginfo_oci_lob_erase)
	ZEND_FE(oci_lob_flush, arginfo_oci_lob_flush)
	ZEND_FE(ocisetbufferinglob, arginfo_ocisetbufferinglob)
	ZEND_FE(ocigetbufferinglob, arginfo_ocigetbufferinglob)
	ZEND_FE(oci_lob_copy, arginfo_oci_lob_copy)
	ZEND_FE(oci_lob_is_equal, arginfo_oci_lob_is_equal)
	ZEND_FE(oci_lob_export, arginfo_oci_lob_export)
	ZEND_DEP_FALIAS(ociwritelobtofile, oci_lob_export, arginfo_ociwritelobtofile)
	ZEND_FE(oci_new_descriptor, arginfo_oci_new_descriptor)
	ZEND_DEP_FALIAS(ocinewdescriptor, oci_new_descriptor, arginfo_ocinewdescriptor)
	ZEND_FE(oci_rollback, arginfo_oci_rollback)
	ZEND_DEP_FALIAS(ocirollback, oci_rollback, arginfo_ocirollback)
	ZEND_FE(oci_commit, arginfo_oci_commit)
	ZEND_DEP_FALIAS(ocicommit, oci_commit, arginfo_ocicommit)
	ZEND_FE(oci_field_name, arginfo_oci_field_name)
	ZEND_DEP_FALIAS(ocicolumnname, oci_field_name, arginfo_ocicolumnname)
	ZEND_FE(oci_field_size, arginfo_oci_field_size)
	ZEND_DEP_FALIAS(ocicolumnsize, oci_field_size, arginfo_ocicolumnsize)
	ZEND_FE(oci_field_scale, arginfo_oci_field_scale)
	ZEND_DEP_FALIAS(ocicolumnscale, oci_field_scale, arginfo_ocicolumnscale)
	ZEND_FE(oci_field_precision, arginfo_oci_field_precision)
	ZEND_DEP_FALIAS(ocicolumnprecision, oci_field_precision, arginfo_ocicolumnprecision)
	ZEND_FE(oci_field_type, arginfo_oci_field_type)
	ZEND_DEP_FALIAS(ocicolumntype, oci_field_type, arginfo_ocicolumntype)
	ZEND_FE(oci_field_type_raw, arginfo_oci_field_type_raw)
	ZEND_DEP_FALIAS(ocicolumntyperaw, oci_field_type_raw, arginfo_ocicolumntyperaw)
	ZEND_FE(oci_field_is_null, arginfo_oci_field_is_null)
	ZEND_DEP_FALIAS(ocicolumnisnull, oci_field_is_null, arginfo_ocicolumnisnull)
	ZEND_FE(oci_execute, arginfo_oci_execute)
	ZEND_DEP_FALIAS(ociexecute, oci_execute, arginfo_ociexecute)
	ZEND_FE(oci_cancel, arginfo_oci_cancel)
	ZEND_DEP_FALIAS(ocicancel, oci_cancel, arginfo_ocicancel)
	ZEND_FE(oci_fetch, arginfo_oci_fetch)
	ZEND_DEP_FALIAS(ocifetch, oci_fetch, arginfo_ocifetch)
	ZEND_DEP_FE(ocifetchinto, arginfo_ocifetchinto)
	ZEND_FE(oci_fetch_all, arginfo_oci_fetch_all)
	ZEND_DEP_FALIAS(ocifetchstatement, oci_fetch_all, arginfo_ocifetchstatement)
	ZEND_FE(oci_fetch_object, arginfo_oci_fetch_object)
	ZEND_FE(oci_fetch_row, arginfo_oci_fetch_row)
	ZEND_FE(oci_fetch_assoc, arginfo_oci_fetch_assoc)
	ZEND_FE(oci_fetch_array, arginfo_oci_fetch_array)
	ZEND_FE(oci_free_statement, arginfo_oci_free_statement)
	ZEND_DEP_FALIAS(ocifreestatement, oci_free_statement, arginfo_ocifreestatement)
	ZEND_FALIAS(oci_free_cursor, oci_free_statement, arginfo_oci_free_cursor)
	ZEND_DEP_FALIAS(ocifreecursor, oci_free_statement, arginfo_ocifreecursor)
	ZEND_FE(oci_close, arginfo_oci_close)
	ZEND_DEP_FALIAS(ocilogoff, oci_close, arginfo_ocilogoff)
	ZEND_FE(oci_new_connect, arginfo_oci_new_connect)
	ZEND_DEP_FALIAS(ocinlogon, oci_new_connect, arginfo_ocinlogon)
	ZEND_FE(oci_connect, arginfo_oci_connect)
	ZEND_DEP_FALIAS(ocilogon, oci_connect, arginfo_ocilogon)
	ZEND_FE(oci_pconnect, arginfo_oci_pconnect)
	ZEND_DEP_FALIAS(ociplogon, oci_pconnect, arginfo_ociplogon)
	ZEND_FE(oci_error, arginfo_oci_error)
	ZEND_DEP_FALIAS(ocierror, oci_error, arginfo_ocierror)
	ZEND_FE(oci_num_fields, arginfo_oci_num_fields)
	ZEND_DEP_FALIAS(ocinumcols, oci_num_fields, arginfo_ocinumcols)
	ZEND_FE(oci_parse, arginfo_oci_parse)
	ZEND_DEP_FALIAS(ociparse, oci_parse, arginfo_ociparse)
	ZEND_FE(oci_get_implicit_resultset, arginfo_oci_get_implicit_resultset)
	ZEND_FE(oci_set_prefetch, arginfo_oci_set_prefetch)
	ZEND_DEP_FALIAS(ocisetprefetch, oci_set_prefetch, arginfo_ocisetprefetch)
	ZEND_FE(oci_set_prefetch_lob, arginfo_oci_set_prefetch_lob)
	ZEND_FE(oci_set_client_identifier, arginfo_oci_set_client_identifier)
	ZEND_FE(oci_set_edition, arginfo_oci_set_edition)
	ZEND_FE(oci_set_module_name, arginfo_oci_set_module_name)
	ZEND_FE(oci_set_action, arginfo_oci_set_action)
	ZEND_FE(oci_set_client_info, arginfo_oci_set_client_info)
	ZEND_FE(oci_set_db_operation, arginfo_oci_set_db_operation)
	ZEND_FE(oci_set_call_timeout, arginfo_oci_set_call_timeout)
	ZEND_FE(oci_password_change, arginfo_oci_password_change)
	ZEND_DEP_FALIAS(ocipasswordchange, oci_password_change, arginfo_ocipasswordchange)
	ZEND_FE(oci_new_cursor, arginfo_oci_new_cursor)
	ZEND_DEP_FALIAS(ocinewcursor, oci_new_cursor, arginfo_ocinewcursor)
	ZEND_FE(oci_result, arginfo_oci_result)
	ZEND_DEP_FALIAS(ociresult, oci_result, arginfo_ociresult)
	ZEND_FE(oci_client_version, arginfo_oci_client_version)
	ZEND_FE(oci_server_version, arginfo_oci_server_version)
	ZEND_DEP_FALIAS(ociserverversion, oci_server_version, arginfo_ociserverversion)
	ZEND_FE(oci_statement_type, arginfo_oci_statement_type)
	ZEND_DEP_FALIAS(ocistatementtype, oci_statement_type, arginfo_ocistatementtype)
	ZEND_FE(oci_num_rows, arginfo_oci_num_rows)
	ZEND_DEP_FALIAS(ocirowcount, oci_num_rows, arginfo_ocirowcount)
	ZEND_FE(oci_free_collection, arginfo_oci_free_collection)
	ZEND_DEP_FALIAS(ocifreecollection, oci_free_collection, arginfo_ocifreecollection)
	ZEND_FE(oci_collection_append, arginfo_oci_collection_append)
	ZEND_DEP_FALIAS(ocicollappend, oci_collection_append, arginfo_ocicollappend)
	ZEND_FE(oci_collection_element_get, arginfo_oci_collection_element_get)
	ZEND_DEP_FALIAS(ocicollgetelem, oci_collection_element_get, arginfo_ocicollgetelem)
	ZEND_FE(oci_collection_assign, arginfo_oci_collection_assign)
	ZEND_FE(oci_collection_element_assign, arginfo_oci_collection_element_assign)
	ZEND_DEP_FALIAS(ocicollassignelem, oci_collection_element_assign, arginfo_ocicollassignelem)
	ZEND_FE(oci_collection_size, arginfo_oci_collection_size)
	ZEND_DEP_FALIAS(ocicollsize, oci_collection_size, arginfo_ocicollsize)
	ZEND_FE(oci_collection_max, arginfo_oci_collection_max)
	ZEND_DEP_FALIAS(ocicollmax, oci_collection_max, arginfo_ocicollmax)
	ZEND_FE(oci_collection_trim, arginfo_oci_collection_trim)
	ZEND_DEP_FALIAS(ocicolltrim, oci_collection_trim, arginfo_ocicolltrim)
	ZEND_FE(oci_new_collection, arginfo_oci_new_collection)
	ZEND_DEP_FALIAS(ocinewcollection, oci_new_collection, arginfo_ocinewcollection)
	ZEND_FE(oci_register_taf_callback, arginfo_oci_register_taf_callback)
	ZEND_FE(oci_unregister_taf_callback, arginfo_oci_unregister_taf_callback)
	ZEND_FE_END
};


static const zend_function_entry class_OCILob_methods[] = {
	ZEND_ME_MAPPING(save, oci_lob_save, arginfo_class_OCILob_save, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(import, oci_lob_import, arginfo_class_OCILob_import, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(saveFile, oci_lob_import, arginfo_class_OCILob_saveFile, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(load, oci_lob_load, arginfo_class_OCILob_load, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(read, oci_lob_read, arginfo_class_OCILob_read, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(eof, oci_lob_eof, arginfo_class_OCILob_eof, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(tell, oci_lob_tell, arginfo_class_OCILob_tell, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(rewind, oci_lob_rewind, arginfo_class_OCILob_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(seek, oci_lob_seek, arginfo_class_OCILob_seek, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(size, oci_lob_size, arginfo_class_OCILob_size, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(write, oci_lob_write, arginfo_class_OCILob_write, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(append, oci_lob_append, arginfo_class_OCILob_append, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(truncate, oci_lob_truncate, arginfo_class_OCILob_truncate, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(erase, oci_lob_erase, arginfo_class_OCILob_erase, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(flush, oci_lob_flush, arginfo_class_OCILob_flush, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setBuffering, ocisetbufferinglob, arginfo_class_OCILob_setBuffering, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getBuffering, ocigetbufferinglob, arginfo_class_OCILob_getBuffering, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeToFile, oci_lob_export, arginfo_class_OCILob_writeToFile, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(export, oci_lob_export, arginfo_class_OCILob_export, ZEND_ACC_PUBLIC)
	ZEND_ME(OCILob, writeTemporary, arginfo_class_OCILob_writeTemporary, ZEND_ACC_PUBLIC)
	ZEND_ME(OCILob, close, arginfo_class_OCILob_close, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(free, oci_free_descriptor, arginfo_class_OCILob_free, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_OCICollection_methods[] = {
	ZEND_ME_MAPPING(free, oci_free_collection, arginfo_class_OCICollection_free, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(append, oci_collection_append, arginfo_class_OCICollection_append, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getElem, oci_collection_element_get, arginfo_class_OCICollection_getElem, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(assign, oci_collection_assign, arginfo_class_OCICollection_assign, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(assignElem, oci_collection_element_assign, arginfo_class_OCICollection_assignElem, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(size, oci_collection_size, arginfo_class_OCICollection_size, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(max, oci_collection_max, arginfo_class_OCICollection_max, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(trim, oci_collection_trim, arginfo_class_OCICollection_trim, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static void register_oci8_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("OCI_DEFAULT", OCI_DEFAULT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SYSOPER", OCI_SYSOPER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SYSDBA", OCI_SYSDBA, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_CRED_EXT", PHP_OCI_CRED_EXT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DESCRIBE_ONLY", OCI_DESCRIBE_ONLY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_COMMIT_ON_SUCCESS", OCI_COMMIT_ON_SUCCESS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_NO_AUTO_COMMIT", OCI_DEFAULT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_EXACT_FETCH", OCI_EXACT_FETCH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SEEK_SET", PHP_OCI_SEEK_SET, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SEEK_CUR", PHP_OCI_SEEK_CUR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_SEEK_END", PHP_OCI_SEEK_END, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_LOB_BUFFER_FREE", OCI_LOB_BUFFER_FREE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BFILEE", SQLT_BFILEE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_CFILEE", SQLT_CFILEE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_CLOB", SQLT_CLOB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BLOB", SQLT_BLOB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_RDD", SQLT_RDD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_INT", SQLT_INT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_NUM", SQLT_NUM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_RSET", SQLT_RSET, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_AFC", SQLT_AFC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_CHR", SQLT_CHR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_VCS", SQLT_VCS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_AVC", SQLT_AVC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_STR", SQLT_STR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_LVC", SQLT_LVC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_FLT", SQLT_FLT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_UIN", SQLT_UIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_LNG", SQLT_LNG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_LBI", SQLT_LBI, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BIN", SQLT_BIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_ODT", SQLT_ODT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BDOUBLE", SQLT_BDOUBLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_BFLOAT", SQLT_BFLOAT, CONST_PERSISTENT);
#if OCI_MAJOR_VERSION >= 12
	REGISTER_LONG_CONSTANT("SQLT_BOL", SQLT_BOL, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OCI_B_NTY", SQLT_NTY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLT_NTY", SQLT_NTY, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("OCI_SYSDATE", "SYSDATE", CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BFILE", SQLT_BFILEE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CFILEE", SQLT_CFILEE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CLOB", SQLT_CLOB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BLOB", SQLT_BLOB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_ROWID", SQLT_RDD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_CURSOR", SQLT_RSET, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_BIN", SQLT_BIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_INT", SQLT_INT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_B_NUM", SQLT_NUM, CONST_PERSISTENT);
#if OCI_MAJOR_VERSION >= 12
	REGISTER_LONG_CONSTANT("OCI_B_BOL", SQLT_BOL, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OCI_FETCHSTATEMENT_BY_COLUMN", PHP_OCI_FETCHSTATEMENT_BY_COLUMN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FETCHSTATEMENT_BY_ROW", PHP_OCI_FETCHSTATEMENT_BY_ROW, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_ASSOC", PHP_OCI_ASSOC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_NUM", PHP_OCI_NUM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_BOTH", PHP_OCI_BOTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_RETURN_NULLS", PHP_OCI_RETURN_NULLS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_RETURN_LOBS", PHP_OCI_RETURN_LOBS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DTYPE_FILE", OCI_DTYPE_FILE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DTYPE_LOB", OCI_DTYPE_LOB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_DTYPE_ROWID", OCI_DTYPE_ROWID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_D_FILE", OCI_DTYPE_FILE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_D_LOB", OCI_DTYPE_LOB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_D_ROWID", OCI_DTYPE_ROWID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_TEMP_CLOB", OCI_TEMP_CLOB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_TEMP_BLOB", OCI_TEMP_BLOB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FO_END", OCI_FO_END, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FO_ABORT", OCI_FO_ABORT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FO_REAUTH", OCI_FO_REAUTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FO_BEGIN", OCI_FO_BEGIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FO_ERROR", OCI_FO_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FO_NONE", OCI_FO_NONE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FO_SESSION", OCI_FO_SESSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FO_SELECT", OCI_FO_SELECT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FO_TXNAL", OCI_FO_TXNAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OCI_FO_RETRY", OCI_FO_RETRY, CONST_PERSISTENT);


	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "oci_new_connect", sizeof("oci_new_connect") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "ocinlogon", sizeof("ocinlogon") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "oci_connect", sizeof("oci_connect") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "ocilogon", sizeof("ocilogon") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "oci_pconnect", sizeof("oci_pconnect") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "ociplogon", sizeof("ociplogon") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
}

static zend_class_entry *register_class_OCILob(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OCILob", class_OCILob_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

	zend_string *attribute_name_AllowDynamicProperties_class_OCILob_0 = zend_string_init_interned("AllowDynamicProperties", sizeof("AllowDynamicProperties") - 1, 1);
	zend_add_class_attribute(class_entry, attribute_name_AllowDynamicProperties_class_OCILob_0, 0);
	zend_string_release(attribute_name_AllowDynamicProperties_class_OCILob_0);

	return class_entry;
}

static zend_class_entry *register_class_OCICollection(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OCICollection", class_OCICollection_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

	zend_string *attribute_name_AllowDynamicProperties_class_OCICollection_0 = zend_string_init_interned("AllowDynamicProperties", sizeof("AllowDynamicProperties") - 1, 1);
	zend_add_class_attribute(class_entry, attribute_name_AllowDynamicProperties_class_OCICollection_0, 0);
	zend_string_release(attribute_name_AllowDynamicProperties_class_OCICollection_0);

	return class_entry;
}
