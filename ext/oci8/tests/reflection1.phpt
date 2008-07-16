--TEST--
Test OCI8 Reflection
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

/* ALL PHP_FE or PHP_ALIAS user callable functions should appear here */

reflection::export(new reflectionfunction('oci_define_by_name'));
reflection::export(new reflectionfunction('oci_bind_by_name'));
reflection::export(new reflectionfunction('oci_bind_array_by_name'));
reflection::export(new reflectionfunction('oci_field_is_null'));
reflection::export(new reflectionfunction('oci_field_name'));
reflection::export(new reflectionfunction('oci_field_size'));
reflection::export(new reflectionfunction('oci_field_scale'));
reflection::export(new reflectionfunction('oci_field_precision'));
reflection::export(new reflectionfunction('oci_field_type'));
reflection::export(new reflectionfunction('oci_field_type_raw'));
reflection::export(new reflectionfunction('oci_execute'));
reflection::export(new reflectionfunction('oci_cancel'));
reflection::export(new reflectionfunction('oci_fetch'));
reflection::export(new reflectionfunction('oci_fetch_object'));
reflection::export(new reflectionfunction('oci_fetch_row'));
reflection::export(new reflectionfunction('oci_fetch_assoc'));
reflection::export(new reflectionfunction('oci_fetch_array'));
reflection::export(new reflectionfunction('ocifetchinto'));
reflection::export(new reflectionfunction('oci_fetch_all'));
reflection::export(new reflectionfunction('oci_free_statement'));
reflection::export(new reflectionfunction('oci_internal_debug'));
reflection::export(new reflectionfunction('oci_num_fields'));
reflection::export(new reflectionfunction('oci_parse'));
reflection::export(new reflectionfunction('oci_new_cursor'));
reflection::export(new reflectionfunction('oci_result'));
reflection::export(new reflectionfunction('oci_server_version'));
reflection::export(new reflectionfunction('oci_statement_type'));
reflection::export(new reflectionfunction('oci_num_rows'));
reflection::export(new reflectionfunction('oci_close'));
reflection::export(new reflectionfunction('oci_connect'));
reflection::export(new reflectionfunction('oci_new_connect'));
reflection::export(new reflectionfunction('oci_pconnect'));
reflection::export(new reflectionfunction('oci_error'));
reflection::export(new reflectionfunction('oci_free_descriptor'));
reflection::export(new reflectionfunction('oci_lob_save'));
reflection::export(new reflectionfunction('oci_lob_import'));
reflection::export(new reflectionfunction('oci_lob_size'));
reflection::export(new reflectionfunction('oci_lob_load'));
reflection::export(new reflectionfunction('oci_lob_read'));
reflection::export(new reflectionfunction('oci_lob_eof'));
reflection::export(new reflectionfunction('oci_lob_tell'));
reflection::export(new reflectionfunction('oci_lob_truncate'));
reflection::export(new reflectionfunction('oci_lob_erase'));
reflection::export(new reflectionfunction('oci_lob_flush'));
reflection::export(new reflectionfunction('ocisetbufferinglob'));
reflection::export(new reflectionfunction('ocigetbufferinglob'));
reflection::export(new reflectionfunction('oci_lob_is_equal'));
reflection::export(new reflectionfunction('oci_lob_rewind'));
reflection::export(new reflectionfunction('oci_lob_write'));
reflection::export(new reflectionfunction('oci_lob_append'));
reflection::export(new reflectionfunction('oci_lob_copy'));
reflection::export(new reflectionfunction('oci_lob_export'));
reflection::export(new reflectionfunction('oci_lob_seek'));
reflection::export(new reflectionfunction('oci_commit'));
reflection::export(new reflectionfunction('oci_rollback'));
reflection::export(new reflectionfunction('oci_new_descriptor'));
reflection::export(new reflectionfunction('oci_set_prefetch'));
reflection::export(new reflectionfunction('oci_password_change'));
reflection::export(new reflectionfunction('oci_free_collection'));
reflection::export(new reflectionfunction('oci_collection_append'));
reflection::export(new reflectionfunction('oci_collection_element_get'));
reflection::export(new reflectionfunction('oci_collection_element_assign'));
reflection::export(new reflectionfunction('oci_collection_assign'));
reflection::export(new reflectionfunction('oci_collection_size'));
reflection::export(new reflectionfunction('oci_collection_max'));
reflection::export(new reflectionfunction('oci_collection_trim'));
reflection::export(new reflectionfunction('oci_new_collection'));
reflection::export(new reflectionfunction('oci_free_cursor'));
reflection::export(new reflectionfunction('ocifreecursor'));
reflection::export(new reflectionfunction('ocibindbyname'));
reflection::export(new reflectionfunction('ocidefinebyname'));
reflection::export(new reflectionfunction('ocicolumnisnull'));
reflection::export(new reflectionfunction('ocicolumnname'));
reflection::export(new reflectionfunction('ocicolumnsize'));
reflection::export(new reflectionfunction('ocicolumnscale'));
reflection::export(new reflectionfunction('ocicolumnprecision'));
reflection::export(new reflectionfunction('ocicolumntype'));
reflection::export(new reflectionfunction('ocicolumntyperaw'));
reflection::export(new reflectionfunction('ociexecute'));
reflection::export(new reflectionfunction('ocicancel'));
reflection::export(new reflectionfunction('ocifetch'));
reflection::export(new reflectionfunction('ocifetchstatement'));
reflection::export(new reflectionfunction('ocifreestatement'));
reflection::export(new reflectionfunction('ociinternaldebug'));
reflection::export(new reflectionfunction('ocinumcols'));
reflection::export(new reflectionfunction('ociparse'));
reflection::export(new reflectionfunction('ocinewcursor'));
reflection::export(new reflectionfunction('ociresult'));
reflection::export(new reflectionfunction('ociserverversion'));
reflection::export(new reflectionfunction('ocistatementtype'));
reflection::export(new reflectionfunction('ocirowcount'));
reflection::export(new reflectionfunction('ocilogoff'));
reflection::export(new reflectionfunction('ocilogon'));
reflection::export(new reflectionfunction('ocinlogon'));
reflection::export(new reflectionfunction('ociplogon'));
reflection::export(new reflectionfunction('ocierror'));
reflection::export(new reflectionfunction('ocifreedesc'));
reflection::export(new reflectionfunction('ocisavelob'));
reflection::export(new reflectionfunction('ocisavelobfile'));
reflection::export(new reflectionfunction('ociwritelobtofile'));
reflection::export(new reflectionfunction('ociloadlob'));
reflection::export(new reflectionfunction('ocicommit'));
reflection::export(new reflectionfunction('ocirollback'));
reflection::export(new reflectionfunction('ocinewdescriptor'));
reflection::export(new reflectionfunction('ocisetprefetch'));
reflection::export(new reflectionfunction('ocipasswordchange'));
reflection::export(new reflectionfunction('ocifreecollection'));
reflection::export(new reflectionfunction('ocinewcollection'));
reflection::export(new reflectionfunction('ocicollappend'));
reflection::export(new reflectionfunction('ocicollgetelem'));
reflection::export(new reflectionfunction('ocicollassignelem'));
reflection::export(new reflectionfunction('ocicollsize'));
reflection::export(new reflectionfunction('ocicollmax'));
reflection::export(new reflectionfunction('ocicolltrim'));

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Function [ <internal:oci8> function oci_define_by_name ] {

  - Parameters [4] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_name ]
    Parameter #2 [ <required> &$variable ]
    Parameter #3 [ <optional> $type ]
  }
}

Function [ <internal:oci8> function oci_bind_by_name ] {

  - Parameters [5] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_name ]
    Parameter #2 [ <required> &$variable ]
    Parameter #3 [ <optional> $maximum_length ]
    Parameter #4 [ <optional> $type ]
  }
}

Function [ <internal:oci8> function oci_bind_array_by_name ] {

  - Parameters [6] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_name ]
    Parameter #2 [ <required> &$variable ]
    Parameter #3 [ <required> $maximum_array_length ]
    Parameter #4 [ <optional> $maximum_item_length ]
    Parameter #5 [ <optional> $type ]
  }
}

Function [ <internal:oci8> function oci_field_is_null ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal:oci8> function oci_field_name ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number ]
  }
}

Function [ <internal:oci8> function oci_field_size ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal:oci8> function oci_field_scale ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number ]
  }
}

Function [ <internal:oci8> function oci_field_precision ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number ]
  }
}

Function [ <internal:oci8> function oci_field_type ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number ]
  }
}

Function [ <internal:oci8> function oci_field_type_raw ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number ]
  }
}

Function [ <internal:oci8> function oci_execute ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <optional> $mode ]
  }
}

Function [ <internal:oci8> function oci_cancel ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function oci_fetch ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function oci_fetch_object ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function oci_fetch_row ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function oci_fetch_assoc ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function oci_fetch_array ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <optional> $mode ]
  }
}

Function [ <internal:oci8> function ocifetchinto ] {

  - Parameters [3] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> &$result ]
    Parameter #2 [ <optional> $mode ]
  }
}

Function [ <internal:oci8> function oci_fetch_all ] {

  - Parameters [5] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> &$output ]
    Parameter #2 [ <optional> $skip ]
    Parameter #3 [ <optional> $maximum_rows ]
    Parameter #4 [ <optional> $flags ]
  }
}

Function [ <internal:oci8> function oci_free_statement ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function oci_internal_debug ] {

  - Parameters [1] {
    Parameter #0 [ <required> $mode ]
  }
}

Function [ <internal:oci8> function oci_num_fields ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function oci_parse ] {

  - Parameters [2] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <required> $sql_text ]
  }
}

Function [ <internal:oci8> function oci_new_cursor ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal:oci8> function oci_result ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal:oci8> function oci_server_version ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal:oci8> function oci_statement_type ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function oci_num_rows ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function oci_close ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal:oci8> function oci_connect ] {

  - Parameters [5] {
    Parameter #0 [ <required> $username ]
    Parameter #1 [ <required> $password ]
    Parameter #2 [ <optional> $connection_string ]
    Parameter #3 [ <optional> $character_set ]
    Parameter #4 [ <optional> $session_mode ]
  }
}

Function [ <internal:oci8> function oci_new_connect ] {

  - Parameters [5] {
    Parameter #0 [ <required> $username ]
    Parameter #1 [ <required> $password ]
    Parameter #2 [ <optional> $connection_string ]
    Parameter #3 [ <optional> $character_set ]
    Parameter #4 [ <optional> $session_mode ]
  }
}

Function [ <internal:oci8> function oci_pconnect ] {

  - Parameters [5] {
    Parameter #0 [ <required> $username ]
    Parameter #1 [ <required> $password ]
    Parameter #2 [ <optional> $connection_string ]
    Parameter #3 [ <optional> $character_set ]
    Parameter #4 [ <optional> $session_mode ]
  }
}

Function [ <internal:oci8> function oci_error ] {

  - Parameters [1] {
    Parameter #0 [ <optional> $connection_or_statement_resource ]
  }
}

Function [ <internal:oci8> function oci_free_descriptor ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal:oci8> function oci_lob_save ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $data ]
    Parameter #2 [ <optional> $offset ]
  }
}

Function [ <internal:oci8> function oci_lob_import ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $filename ]
  }
}

Function [ <internal:oci8> function oci_lob_size ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal:oci8> function oci_lob_load ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal:oci8> function oci_lob_read ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $length ]
  }
}

Function [ <internal:oci8> function oci_lob_eof ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal:oci8> function oci_lob_tell ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal:oci8> function oci_lob_truncate ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <optional> $length ]
  }
}

Function [ <internal:oci8> function oci_lob_erase ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <optional> $offset ]
    Parameter #2 [ <optional> $length ]
  }
}

Function [ <internal:oci8> function oci_lob_flush ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <optional> $flag ]
  }
}

Function [ <internal:oci8> function ocisetbufferinglob ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $flag ]
  }
}

Function [ <internal:oci8> function ocigetbufferinglob ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal:oci8> function oci_lob_is_equal ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $lob_descriptor ]
  }
}

Function [ <internal:oci8> function oci_lob_rewind ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal:oci8> function oci_lob_write ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $string ]
    Parameter #2 [ <optional> $length ]
  }
}

Function [ <internal:oci8> function oci_lob_append ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor_to ]
    Parameter #1 [ <required> $lob_descriptor_from ]
  }
}

Function [ <internal:oci8> function oci_lob_copy ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor_to ]
    Parameter #1 [ <required> $lob_descriptor_from ]
    Parameter #2 [ <optional> $length ]
  }
}

Function [ <internal:oci8> function oci_lob_export ] {

  - Parameters [4] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $filename ]
    Parameter #2 [ <optional> $start ]
    Parameter #3 [ <optional> $length ]
  }
}

Function [ <internal:oci8> function oci_lob_seek ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $offset ]
    Parameter #2 [ <optional> $whence ]
  }
}

Function [ <internal:oci8> function oci_commit ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal:oci8> function oci_rollback ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal:oci8> function oci_new_descriptor ] {

  - Parameters [2] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <optional> $type ]
  }
}

Function [ <internal:oci8> function oci_set_prefetch ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $number_of_rows ]
  }
}

Function [ <internal:oci8> function oci_password_change ] {

  - Parameters [4] {
    Parameter #0 [ <required> $connection_resource_or_connection_string ]
    Parameter #1 [ <required> $username ]
    Parameter #2 [ <required> $old_password ]
    Parameter #3 [ <required> $new_password ]
  }
}

Function [ <internal:oci8> function oci_free_collection ] {

  - Parameters [1] {
    Parameter #0 [ <required> $collection ]
  }
}

Function [ <internal:oci8> function oci_collection_append ] {

  - Parameters [2] {
    Parameter #0 [ <optional> $collection ]
    Parameter #1 [ <optional> $value ]
  }
}

Function [ <internal:oci8> function oci_collection_element_get ] {

  - Parameters [2] {
    Parameter #0 [ <required> $collection ]
    Parameter #1 [ <required> $index ]
  }
}

Function [ <internal:oci8> function oci_collection_element_assign ] {

  - Parameters [3] {
    Parameter #0 [ <required> $collection ]
    Parameter #1 [ <required> $index ]
    Parameter #2 [ <required> $value ]
  }
}

Function [ <internal:oci8> function oci_collection_assign ] {

  - Parameters [2] {
    Parameter #0 [ <required> $collection_to ]
    Parameter #1 [ <required> $collection_from ]
  }
}

Function [ <internal:oci8> function oci_collection_size ] {

  - Parameters [1] {
    Parameter #0 [ <required> $collection ]
  }
}

Function [ <internal:oci8> function oci_collection_max ] {

  - Parameters [1] {
    Parameter #0 [ <required> $collection ]
  }
}

Function [ <internal:oci8> function oci_collection_trim ] {

  - Parameters [2] {
    Parameter #0 [ <required> $collection ]
    Parameter #1 [ <required> $num ]
  }
}

Function [ <internal:oci8> function oci_new_collection ] {

  - Parameters [3] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <required> $type_name ]
    Parameter #2 [ <optional> $schema_name ]
  }
}

Function [ <internal:oci8> function oci_free_cursor ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function ocifreecursor ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function ocibindbyname ] {

  - Parameters [5] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_name ]
    Parameter #2 [ <required> &$variable ]
    Parameter #3 [ <optional> $maximum_length ]
    Parameter #4 [ <optional> $type ]
  }
}

Function [ <internal:oci8> function ocidefinebyname ] {

  - Parameters [4] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_name ]
    Parameter #2 [ <required> &$variable ]
    Parameter #3 [ <optional> $type ]
  }
}

Function [ <internal:oci8> function ocicolumnisnull ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal:oci8> function ocicolumnname ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number ]
  }
}

Function [ <internal:oci8> function ocicolumnsize ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal:oci8> function ocicolumnscale ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number ]
  }
}

Function [ <internal:oci8> function ocicolumnprecision ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number ]
  }
}

Function [ <internal:oci8> function ocicolumntype ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number ]
  }
}

Function [ <internal:oci8> function ocicolumntyperaw ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number ]
  }
}

Function [ <internal:oci8> function ociexecute ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <optional> $mode ]
  }
}

Function [ <internal:oci8> function ocicancel ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function ocifetch ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function ocifetchstatement ] {

  - Parameters [5] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> &$output ]
    Parameter #2 [ <optional> $skip ]
    Parameter #3 [ <optional> $maximum_rows ]
    Parameter #4 [ <optional> $flags ]
  }
}

Function [ <internal:oci8> function ocifreestatement ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function ociinternaldebug ] {

  - Parameters [1] {
    Parameter #0 [ <required> $mode ]
  }
}

Function [ <internal:oci8> function ocinumcols ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function ociparse ] {

  - Parameters [2] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <required> $sql_text ]
  }
}

Function [ <internal:oci8> function ocinewcursor ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal:oci8> function ociresult ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal:oci8> function ociserverversion ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal:oci8> function ocistatementtype ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function ocirowcount ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal:oci8> function ocilogoff ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal:oci8> function ocilogon ] {

  - Parameters [5] {
    Parameter #0 [ <required> $username ]
    Parameter #1 [ <required> $password ]
    Parameter #2 [ <optional> $connection_string ]
    Parameter #3 [ <optional> $character_set ]
    Parameter #4 [ <optional> $session_mode ]
  }
}

Function [ <internal:oci8> function ocinlogon ] {

  - Parameters [5] {
    Parameter #0 [ <required> $username ]
    Parameter #1 [ <required> $password ]
    Parameter #2 [ <optional> $connection_string ]
    Parameter #3 [ <optional> $character_set ]
    Parameter #4 [ <optional> $session_mode ]
  }
}

Function [ <internal:oci8> function ociplogon ] {

  - Parameters [5] {
    Parameter #0 [ <required> $username ]
    Parameter #1 [ <required> $password ]
    Parameter #2 [ <optional> $connection_string ]
    Parameter #3 [ <optional> $character_set ]
    Parameter #4 [ <optional> $session_mode ]
  }
}

Function [ <internal:oci8> function ocierror ] {

  - Parameters [1] {
    Parameter #0 [ <optional> $connection_or_statement_resource ]
  }
}

Function [ <internal:oci8> function ocifreedesc ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal:oci8> function ocisavelob ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $data ]
    Parameter #2 [ <optional> $offset ]
  }
}

Function [ <internal:oci8> function ocisavelobfile ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $filename ]
  }
}

Function [ <internal:oci8> function ociwritelobtofile ] {

  - Parameters [4] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $filename ]
    Parameter #2 [ <optional> $start ]
    Parameter #3 [ <optional> $length ]
  }
}

Function [ <internal:oci8> function ociloadlob ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal:oci8> function ocicommit ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal:oci8> function ocirollback ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal:oci8> function ocinewdescriptor ] {

  - Parameters [2] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <optional> $type ]
  }
}

Function [ <internal:oci8> function ocisetprefetch ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $number_of_rows ]
  }
}

Function [ <internal:oci8> function ocipasswordchange ] {

  - Parameters [4] {
    Parameter #0 [ <required> $connection_resource_or_connection_string ]
    Parameter #1 [ <required> $username ]
    Parameter #2 [ <required> $old_password ]
    Parameter #3 [ <required> $new_password ]
  }
}

Function [ <internal:oci8> function ocifreecollection ] {

  - Parameters [1] {
    Parameter #0 [ <required> $collection ]
  }
}

Function [ <internal:oci8> function ocinewcollection ] {

  - Parameters [3] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <required> $type_name ]
    Parameter #2 [ <optional> $schema_name ]
  }
}

Function [ <internal:oci8> function ocicollappend ] {

  - Parameters [2] {
    Parameter #0 [ <optional> $collection ]
    Parameter #1 [ <optional> $value ]
  }
}

Function [ <internal:oci8> function ocicollgetelem ] {

  - Parameters [2] {
    Parameter #0 [ <required> $collection ]
    Parameter #1 [ <required> $index ]
  }
}

Function [ <internal:oci8> function ocicollassignelem ] {

  - Parameters [3] {
    Parameter #0 [ <required> $collection ]
    Parameter #1 [ <required> $index ]
    Parameter #2 [ <required> $value ]
  }
}

Function [ <internal:oci8> function ocicollsize ] {

  - Parameters [1] {
    Parameter #0 [ <required> $collection ]
  }
}

Function [ <internal:oci8> function ocicollmax ] {

  - Parameters [1] {
    Parameter #0 [ <required> $collection ]
  }
}

Function [ <internal:oci8> function ocicolltrim ] {

  - Parameters [2] {
    Parameter #0 [ <required> $collection ]
    Parameter #1 [ <required> $num ]
  }
}

===DONE===
