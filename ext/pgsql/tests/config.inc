<?php
// These vars are used to connect db and create test table.
// values can be set to meet your environment

// "test" database must be existed. i.e. "createdb test" before testing
// PostgreSQL uses login name as username, user must have access to "test" database.
$conn_str = "host=localhost dbname=test port=5432";    // connection string
$table_name = "php_pgsql_test";  // test table that will be created
$num_test_record = 1000;         // Number of records to create

$table_def = "CREATE TABLE php_pgsql_test (num int, str text, bin bytea);"; // Test table 
$field_name = "num";             // For pg_field_num()

?>