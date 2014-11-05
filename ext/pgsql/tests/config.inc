<?php

// These vars are used to connect db and create test table.
// values can be set to meet your environment with the
// environment var PGSQL_TEST_CONNSTR

$conn_str = getenv('PGSQL_TEST_CONNSTR') ?: "host=localhost dbname=test port=5432";    // connection string

$table_name = "php_pgsql_test";  // test table that should be exist
$num_test_record = 1000;         // Number of records to create

$table_def = "CREATE TABLE php_pgsql_test (num int, str text, bin bytea);"; // Test table 
$field_name = "num";             // For pg_field_num()

?>
