--TEST--
ReflectionFunction to check API
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
	$r = new ReflectionExtension("mysql");

	$ignore = array();

	$functions = $r->getFunctions();
	asort($functions);
	printf("Functions:\n");
	foreach ($functions as $func) {
		if (isset($ignore[$func->name]))
			continue;

		printf("  %s\n", $func->name);
		$rf = new ReflectionFunction($func->name);
		printf("    Deprecated: %s\n", $rf->isDeprecated() ? "yes" : "no");
		printf("    Accepted parameters: %d\n", $rf->getNumberOfParameters());
		printf("    Required parameters: %d\n", $rf->getNumberOfRequiredParameters());
		foreach( $rf->getParameters() as $param ) {
			printf("      %s\n", $param);
		}
	}

	print "done!";
?>
--EXPECTF--
Functions:
  mysql
    Deprecated: yes
    Accepted parameters: 3
    Required parameters: 2
      Parameter #0 [ <required> $database_name ]
      Parameter #1 [ <required> $query ]
      Parameter #2 [ <optional> $link_identifier ]
  mysql_affected_rows
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_client_encoding
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_close
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_connect
    Deprecated: no
    Accepted parameters: 5
    Required parameters: 0
      Parameter #0 [ <optional> $hostname ]
      Parameter #1 [ <optional> $username ]
      Parameter #2 [ <optional> $password ]
      Parameter #3 [ <optional> $new ]
      Parameter #4 [ <optional> $flags ]
  mysql_data_seek
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $row_number ]
  mysql_db_name
    Deprecated: no
    Accepted parameters: 3
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $row ]
      Parameter #2 [ <optional> $field ]
  mysql_db_query
    Deprecated: yes
    Accepted parameters: 3
    Required parameters: 2
      Parameter #0 [ <required> $database_name ]
      Parameter #1 [ <required> $query ]
      Parameter #2 [ <optional> $link_identifier ]
  mysql_dbname
    Deprecated: yes
    Accepted parameters: 3
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $row ]
      Parameter #2 [ <optional> $field ]
  mysql_errno
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_error
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_escape_string
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $string ]
  mysql_fetch_array
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 1
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <optional> $result_type ]
  mysql_fetch_assoc
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $result ]
  mysql_fetch_field
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 1
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <optional> $field_offset ]
  mysql_fetch_lengths
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $result ]
  mysql_fetch_object
    Deprecated: no
    Accepted parameters: 3
    Required parameters: 1
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <optional> $class_name ]
      Parameter #2 [ <optional> $ctor_params ]
  mysql_fetch_row
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $result ]
  mysql_field_flags
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_offset ]
  mysql_field_len
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_offset ]
  mysql_field_name
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_index ]
  mysql_field_seek
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_offset ]
  mysql_field_table
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_offset ]
  mysql_field_type
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_offset ]
  mysql_fieldflags
    Deprecated: yes
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_offset ]
  mysql_fieldlen
    Deprecated: yes
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_offset ]
  mysql_fieldname
    Deprecated: yes
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_index ]
  mysql_fieldtable
    Deprecated: yes
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_offset ]
  mysql_fieldtype
    Deprecated: yes
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $field_offset ]
  mysql_free_result
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $result ]
  mysql_freeresult
    Deprecated: yes
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $result ]
  mysql_get_client_info
    Deprecated: no
    Accepted parameters: 0
    Required parameters: 0
  mysql_get_host_info
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_get_proto_info
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_get_server_info
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_info
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_insert_id
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_list_dbs
    Deprecated: yes
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_list_fields
    Deprecated: no
    Accepted parameters: 3
    Required parameters: 2
      Parameter #0 [ <required> $database_name ]
      Parameter #1 [ <required> $table_name ]
      Parameter #2 [ <optional> $link_identifier ]
  mysql_list_processes
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_list_tables
    Deprecated: yes
    Accepted parameters: 2
    Required parameters: 1
      Parameter #0 [ <required> $database_name ]
      Parameter #1 [ <optional> $link_identifier ]
  mysql_listdbs
    Deprecated: yes
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_listfields
    Deprecated: yes
    Accepted parameters: 3
    Required parameters: 2
      Parameter #0 [ <required> $database_name ]
      Parameter #1 [ <required> $table_name ]
      Parameter #2 [ <optional> $link_identifier ]
  mysql_listtables
    Deprecated: yes
    Accepted parameters: 2
    Required parameters: 1
      Parameter #0 [ <required> $database_name ]
      Parameter #1 [ <optional> $link_identifier ]
  mysql_num_fields
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $result ]
  mysql_num_rows
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $result ]
  mysql_numfields
    Deprecated: yes
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $result ]
  mysql_numrows
    Deprecated: yes
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $result ]
  mysql_pconnect
    Deprecated: no
    Accepted parameters: 4
    Required parameters: 0
      Parameter #0 [ <optional> $hostname ]
      Parameter #1 [ <optional> $username ]
      Parameter #2 [ <optional> $password ]
      Parameter #3 [ <optional> $flags ]
  mysql_ping
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_query
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 1
      Parameter #0 [ <required> $query ]
      Parameter #1 [ <optional> $link_identifier ]
  mysql_real_escape_string
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 1
      Parameter #0 [ <required> $string ]
      Parameter #1 [ <optional> $link_identifier ]
  mysql_result
    Deprecated: no
    Accepted parameters: 3
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $row ]
      Parameter #2 [ <optional> $field ]
  mysql_select_db
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 1
      Parameter #0 [ <required> $database_name ]
      Parameter #1 [ <optional> $link_identifier ]
  mysql_selectdb
    Deprecated: yes
    Accepted parameters: 2
    Required parameters: 1
      Parameter #0 [ <required> $database_name ]
      Parameter #1 [ <optional> $link_identifier ]
  mysql_set_charset
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 1
      Parameter #0 [ <required> $charset_name ]
      Parameter #1 [ <optional> $link_identifier ]
  mysql_stat
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_table_name
    Deprecated: no
    Accepted parameters: 3
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $row ]
      Parameter #2 [ <optional> $field ]
  mysql_tablename
    Deprecated: no
    Accepted parameters: 3
    Required parameters: 2
      Parameter #0 [ <required> $result ]
      Parameter #1 [ <required> $row ]
      Parameter #2 [ <optional> $field ]
  mysql_thread_id
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 0
      Parameter #0 [ <optional> $link_identifier ]
  mysql_unbuffered_query
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 1
      Parameter #0 [ <required> $query ]
      Parameter #1 [ <optional> $link_identifier ]
done!