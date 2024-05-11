<?php

// These vars are used to connect db.
// values can be set to meet your environment with the
// environment var PGSQL_TEST_CONNSTR

// "test" database must exist. i.e. "createdb test" before testing
$conn_str = getenv('PGSQL_TEST_CONNSTR') ?: "host=localhost dbname=test port=5432 user=postgres password=postgres";    // connection string
?>
