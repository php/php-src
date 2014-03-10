--TEST--
ReflectionExtension basics to check API
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
	$r = new ReflectionExtension("mysql");

	printf("Name: %s\n", $r->name);
	printf("Version: %s\n", $r->getVersion());
	$classes = $r->getClasses();
	if (!empty($classes)) {
		printf("[002] Expecting no class\n");
		asort($classes);
		var_dump($classes);
	}

	$ignore = array();

	$functions = $r->getFunctions();
	asort($functions);
	printf("Functions:\n");
	foreach ($functions as $func) {
		if (isset($ignore[$func->name])) {
			unset($ignore[$func->name]);
		} else {
			printf("  %s\n", $func->name);
		}
	}
	if (!empty($ignore)) {
		printf("Dumping version dependent and missing functions\n");
		var_dump($ignore);
	}


	print "done!";
?>
--EXPECTF--
Name: mysql
Version: 1.0
Functions:
  mysql
  mysql_affected_rows
  mysql_client_encoding
  mysql_close
  mysql_connect
  mysql_data_seek
  mysql_db_name
  mysql_db_query
  mysql_dbname
  mysql_errno
  mysql_error
  mysql_escape_string
  mysql_fetch_array
  mysql_fetch_assoc
  mysql_fetch_field
  mysql_fetch_lengths
  mysql_fetch_object
  mysql_fetch_row
  mysql_field_flags
  mysql_field_len
  mysql_field_name
  mysql_field_seek
  mysql_field_table
  mysql_field_type
  mysql_fieldflags
  mysql_fieldlen
  mysql_fieldname
  mysql_fieldtable
  mysql_fieldtype
  mysql_free_result
  mysql_freeresult
  mysql_get_client_info
  mysql_get_host_info
  mysql_get_proto_info
  mysql_get_server_info
  mysql_info
  mysql_insert_id
  mysql_list_dbs
  mysql_list_fields
  mysql_list_processes
  mysql_list_tables
  mysql_listdbs
  mysql_listfields
  mysql_listtables
  mysql_num_fields
  mysql_num_rows
  mysql_numfields
  mysql_numrows
  mysql_pconnect
  mysql_ping
  mysql_query
  mysql_real_escape_string
  mysql_result
  mysql_select_db
  mysql_selectdb
  mysql_set_charset
  mysql_stat
  mysql_table_name
  mysql_tablename
  mysql_thread_id
  mysql_unbuffered_query
done!