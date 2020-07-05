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
reflection::export(new reflectionfunction('oci_fetch_all'));
reflection::export(new reflectionfunction('oci_free_statement'));
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
reflection::export(new reflectionfunction('oci_set_edition'));
reflection::export(new reflectionfunction('oci_set_module_name'));
reflection::export(new reflectionfunction('oci_set_action'));
reflection::export(new reflectionfunction('oci_set_client_info'));
reflection::export(new reflectionfunction('oci_set_client_identifier'));
reflection::export(new reflectionfunction('oci_get_implicit_resultset'));

?>
--EXPECTF--
Function [ <internal%s> function oci_define_by_name ] {

  - Parameters [4] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_name ]
    Parameter #2 [ <required> &$variable ]
    Parameter #3 [ <optional> $type ]
  }
}

Function [ <internal%s> function oci_bind_by_name ] {

  - Parameters [5] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_name ]
    Parameter #2 [ <required> &$variable ]
    Parameter #3 [ <optional> $maximum_length ]
    Parameter #4 [ <optional> $type ]
  }
}

Function [ <internal%s> function oci_bind_array_by_name ] {

  - Parameters [6] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_name ]
    Parameter #2 [ <required> &$variable ]
    Parameter #3 [ <required> $maximum_array_length ]
    Parameter #4 [ <optional> $maximum_item_length ]
    Parameter #5 [ <optional> $type ]
  }
}

Function [ <internal%s> function oci_field_is_null ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal%s> function oci_field_name ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal%s> function oci_field_size ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal%s> function oci_field_scale ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal%s> function oci_field_precision ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal%s> function oci_field_type ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal%s> function oci_field_type_raw ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal%s> function oci_execute ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <optional> $mode ]
  }
}

Function [ <internal%s> function oci_cancel ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal%s> function oci_fetch ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal%s> function oci_fetch_object ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal%s> function oci_fetch_row ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal%s> function oci_fetch_assoc ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal%s> function oci_fetch_array ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <optional> $mode ]
  }
}

Function [ <internal%s> function oci_fetch_all ] {

  - Parameters [5] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> &$output ]
    Parameter #2 [ <optional> $skip ]
    Parameter #3 [ <optional> $maximum_rows ]
    Parameter #4 [ <optional> $flags ]
  }
}

Function [ <internal%s> function oci_free_statement ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal%s> function oci_num_fields ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal%s> function oci_parse ] {

  - Parameters [2] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <required> $sql_text ]
  }
}

Function [ <internal%s> function oci_new_cursor ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal%s> function oci_result ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $column_number_or_name ]
  }
}

Function [ <internal%s> function oci_server_version ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal%s> function oci_statement_type ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal%s> function oci_num_rows ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal%s> function oci_close ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal%s> function oci_connect ] {

  - Parameters [5] {
    Parameter #0 [ <required> $username ]
    Parameter #1 [ <required> $password ]
    Parameter #2 [ <optional> $connection_string ]
    Parameter #3 [ <optional> $character_set ]
    Parameter #4 [ <optional> $session_mode ]
  }
}

Function [ <internal%s> function oci_new_connect ] {

  - Parameters [5] {
    Parameter #0 [ <required> $username ]
    Parameter #1 [ <required> $password ]
    Parameter #2 [ <optional> $connection_string ]
    Parameter #3 [ <optional> $character_set ]
    Parameter #4 [ <optional> $session_mode ]
  }
}

Function [ <internal%s> function oci_pconnect ] {

  - Parameters [5] {
    Parameter #0 [ <required> $username ]
    Parameter #1 [ <required> $password ]
    Parameter #2 [ <optional> $connection_string ]
    Parameter #3 [ <optional> $character_set ]
    Parameter #4 [ <optional> $session_mode ]
  }
}

Function [ <internal%s> function oci_error ] {

  - Parameters [1] {
    Parameter #0 [ <optional> $connection_or_statement_resource ]
  }
}

Function [ <internal%s> function oci_free_descriptor ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal%s> function oci_lob_save ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $data ]
    Parameter #2 [ <optional> $offset ]
  }
}

Function [ <internal%s> function oci_lob_import ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $filename ]
  }
}

Function [ <internal%s> function oci_lob_size ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal%s> function oci_lob_load ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal%s> function oci_lob_read ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $length ]
  }
}

Function [ <internal%s> function oci_lob_eof ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal%s> function oci_lob_tell ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal%s> function oci_lob_truncate ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <optional> $length ]
  }
}

Function [ <internal%s> function oci_lob_erase ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <optional> $offset ]
    Parameter #2 [ <optional> $length ]
  }
}

Function [ <internal%s> function oci_lob_flush ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <optional> $flag ]
  }
}

Function [ <internal%s> function ocisetbufferinglob ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $mode ]
  }
}

Function [ <internal%s> function ocigetbufferinglob ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal%s> function oci_lob_is_equal ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $lob_descriptor ]
  }
}

Function [ <internal%s> function oci_lob_rewind ] {

  - Parameters [1] {
    Parameter #0 [ <required> $lob_descriptor ]
  }
}

Function [ <internal%s> function oci_lob_write ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $string ]
    Parameter #2 [ <optional> $length ]
  }
}

Function [ <internal%s> function oci_lob_append ] {

  - Parameters [2] {
    Parameter #0 [ <required> $lob_descriptor_to ]
    Parameter #1 [ <required> $lob_descriptor_from ]
  }
}

Function [ <internal%s> function oci_lob_copy ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor_to ]
    Parameter #1 [ <required> $lob_descriptor_from ]
    Parameter #2 [ <optional> $length ]
  }
}

Function [ <internal%s> function oci_lob_export ] {

  - Parameters [4] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $filename ]
    Parameter #2 [ <optional> $start ]
    Parameter #3 [ <optional> $length ]
  }
}

Function [ <internal%s> function oci_lob_seek ] {

  - Parameters [3] {
    Parameter #0 [ <required> $lob_descriptor ]
    Parameter #1 [ <required> $offset ]
    Parameter #2 [ <optional> $whence ]
  }
}

Function [ <internal%s> function oci_commit ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal%s> function oci_rollback ] {

  - Parameters [1] {
    Parameter #0 [ <required> $connection_resource ]
  }
}

Function [ <internal%s> function oci_new_descriptor ] {

  - Parameters [2] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <optional> $type ]
  }
}

Function [ <internal%s> function oci_set_prefetch ] {

  - Parameters [2] {
    Parameter #0 [ <required> $statement_resource ]
    Parameter #1 [ <required> $number_of_rows ]
  }
}

Function [ <internal%s> function oci_password_change ] {

  - Parameters [4] {
    Parameter #0 [ <required> $connection_resource_or_connection_string ]
    Parameter #1 [ <required> $username ]
    Parameter #2 [ <required> $old_password ]
    Parameter #3 [ <required> $new_password ]
  }
}

Function [ <internal%s> function oci_free_collection ] {

  - Parameters [1] {
    Parameter #0 [ <required> $collection ]
  }
}

Function [ <internal%s> function oci_collection_append ] {

  - Parameters [2] {
    Parameter #0 [ <required> $collection ]
    Parameter #1 [ <required> $value ]
  }
}

Function [ <internal%s> function oci_collection_element_get ] {

  - Parameters [2] {
    Parameter #0 [ <required> $collection ]
    Parameter #1 [ <required> $index ]
  }
}

Function [ <internal%s> function oci_collection_element_assign ] {

  - Parameters [3] {
    Parameter #0 [ <required> $collection ]
    Parameter #1 [ <required> $index ]
    Parameter #2 [ <required> $value ]
  }
}

Function [ <internal%s> function oci_collection_assign ] {

  - Parameters [2] {
    Parameter #0 [ <required> $collection_to ]
    Parameter #1 [ <required> $collection_from ]
  }
}

Function [ <internal%s> function oci_collection_size ] {

  - Parameters [1] {
    Parameter #0 [ <required> $collection ]
  }
}

Function [ <internal%s> function oci_collection_max ] {

  - Parameters [1] {
    Parameter #0 [ <required> $collection ]
  }
}

Function [ <internal%s> function oci_collection_trim ] {

  - Parameters [2] {
    Parameter #0 [ <required> $collection ]
    Parameter #1 [ <required> $number ]
  }
}

Function [ <internal%s> function oci_new_collection ] {

  - Parameters [3] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <required> $type_name ]
    Parameter #2 [ <optional> $schema_name ]
  }
}

Function [ <internal%s> function oci_free_cursor ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}

Function [ <internal%s> function oci_set_edition ] {

  - Parameters [1] {
    Parameter #0 [ <required> $edition_name ]
  }
}

Function [ <internal%s> function oci_set_module_name ] {

  - Parameters [2] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <required> $module_name ]
  }
}

Function [ <internal%s> function oci_set_action ] {

  - Parameters [2] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <required> $action ]
  }
}

Function [ <internal%s> function oci_set_client_info ] {

  - Parameters [2] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <required> $client_information ]
  }
}

Function [ <internal%s> function oci_set_client_identifier ] {

  - Parameters [2] {
    Parameter #0 [ <required> $connection_resource ]
    Parameter #1 [ <required> $client_identifier ]
  }
}

Function [ <internal%s> function oci_get_implicit_resultset ] {

  - Parameters [1] {
    Parameter #0 [ <required> $statement_resource ]
  }
}