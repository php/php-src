<?php

// These vars are used to connect db and create test table.
// values can be set to meet your environment with the
// environment var PGSQL_TEST_CONNSTR

// "test" database must exist. i.e. "createdb test" before testing
$conn_str = getenv('PGSQL_TEST_CONNSTR') ?: "host=localhost dbname=test port=5432";    // connection string

$table_name      = "php_pgsql_test";          // test table that will be created
$table_name_92   = "php_pgsql_test_92";       // test table that will be created
$num_test_record = 1000;                      // Number of records to create

// Test table
$table_def    = "CREATE TABLE ${table_name} (num int, str text, bin bytea);";
$table_def_92 = "CREATE TABLE ${table_name_92} (textary text[],  jsn json);";
$field_name = "num";             // For pg_field_num()

?>
