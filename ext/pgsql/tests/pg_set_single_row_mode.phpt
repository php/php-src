--TEST--
PostgreSQL pg_set_single_row_mode
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = "test_single_row_mode";

$conn = pg_connect($conn_str);
pg_query($conn, "CREATE TABLE {$table_name} (num int)");

for ($i = 0; $i < 3; $i++) {
    pg_query($conn, "INSERT INTO {$table_name} VALUES ({$i})");
}

var_dump(pg_send_query($conn, "SELECT * FROM {$table_name} ORDER BY num"));
var_dump(pg_set_single_row_mode($conn));

for ($i = 0; $i < 3; $i++) {
    $result = pg_get_result($conn);
    var_dump(pg_result_status($result) === PGSQL_SINGLE_TUPLE);
    var_dump(pg_fetch_result($result, 0, 0));
}

$result = pg_get_result($conn);
var_dump(pg_result_status($result) === PGSQL_TUPLES_OK);
var_dump(pg_num_rows($result));
var_dump(pg_get_result($conn));
var_dump(pg_set_single_row_mode($conn));
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "test_single_row_mode";

$conn = pg_connect($conn_str);
pg_query($conn, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(1) "0"
bool(true)
string(1) "1"
bool(true)
string(1) "2"
bool(true)
int(0)
bool(false)
bool(false)
