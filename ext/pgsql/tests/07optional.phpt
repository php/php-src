--TEST--
PostgreSQL optional functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// optional functions

include('config.inc');

$db = pg_connect($conn_str);
$enc = pg_client_encoding($db);

pg_set_client_encoding($db, $enc);

if (function_exists('pg_set_error_verbosity')) {
	pg_set_error_verbosity(PGSQL_ERRORS_TERSE);
	pg_set_error_verbosity(PGSQL_ERRORS_DEFAULT);
	pg_set_error_verbosity(PGSQL_ERRORS_VERBOSE);
}
echo "OK";
?>
--EXPECT--
OK
