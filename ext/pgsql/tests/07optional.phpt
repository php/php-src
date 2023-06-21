--TEST--
PostgreSQL optional functions
--EXTENSIONS--
pgsql
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
    pg_set_error_verbosity($db, PGSQL_ERRORS_TERSE);
    pg_set_error_verbosity($db, PGSQL_ERRORS_DEFAULT);
    pg_set_error_verbosity($db, PGSQL_ERRORS_VERBOSE);
    pg_set_error_verbosity($db, PGSQL_ERRORS_SQLSTATE);
}
if (function_exists('pg_set_error_context_visibility')) {
    pg_set_error_context_visibility($db, PGSQL_SHOW_CONTEXT_NEVER);
    pg_set_error_context_visibility($db, PGSQL_SHOW_CONTEXT_ERRORS);
    pg_set_error_context_visibility($db, PGSQL_SHOW_CONTEXT_ALWAYS);
}
echo "OK";
?>
--EXPECT--
OK
