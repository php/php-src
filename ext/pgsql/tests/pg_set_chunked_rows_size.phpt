--TEST--
PostgreSQL pg_set_chunked_rows_size
--EXTENSIONS--
pgsql
--SKIPIF--
<?php 
include("inc/skipif.inc");
if (!function_exists("pg_set_chunked_rows_size")) die("skip pg_set_chunked_rows_size unsupported");
?>
--FILE--
<?php

include('inc/config.inc');
$table_name = "test_chunked_rows_size";

$conn = pg_connect($conn_str);
pg_query($conn, "CREATE TABLE {$table_name} (num int, str text)");

for ($i = 0; $i < 10; $i ++)
	pg_query($conn, "INSERT INTO {$table_name} DEFAULT VALUES");

var_dump(pg_send_query($conn, "SELECT * FROM ".$table_name.";"));
try {
	pg_set_chunked_rows_size($conn, -1);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

var_dump(pg_set_chunked_rows_size($conn, 1));
$result = pg_get_result($conn);
var_dump(pg_result_status($result) === PGSQL_TUPLES_CHUNK);
var_dump(pg_num_rows($result));
var_dump(pg_set_chunked_rows_size($conn, 10));
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "test_chunked_rows_size";

$conn = pg_connect($conn_str);
pg_query($conn, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECTF--
bool(true)
pg_set_chunked_rows_size(): Argument #2 ($size) must be between 1 and %d
bool(true)
bool(true)
int(1)
bool(false)
