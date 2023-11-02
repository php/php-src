--TEST--
PostgreSQL async query
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = "table_04async_query";

$db = pg_connect($conn_str);
pg_query($db, "create table {$table_name} (num int, str text, bin bytea)");
pg_query($db, "insert into {$table_name} default values");

if (!pg_send_query($db, "SELECT * FROM ".$table_name.";")) {
    echo "pg_send_query() error\n";
}
while(pg_connection_busy($db));  // busy wait: intended
if (pg_connection_status($db) === PGSQL_CONNECTION_BAD) {
    echo "pg_connection_status() error\n";
}
if (!($result = pg_get_result($db)))
{
    echo "pg_get_result() error\n";
}

if (!($rows = pg_num_rows($result))) {
    echo "pg_num_rows() error\n";
}
for ($i=0; $i < $rows; $i++)
{
    pg_fetch_array($result, $i, PGSQL_NUM);
}
for ($i=0; $i < $rows; $i++)
{
    pg_fetch_object($result);
}
for ($i=0; $i < $rows; $i++)
{
    pg_fetch_row($result, $i);
}
for ($i=0; $i < $rows; $i++)
{
    pg_fetch_result($result, $i, 0);
}

pg_num_rows(pg_query($db, "SELECT * FROM ".$table_name.";"));
pg_num_fields(pg_query($db, "SELECT * FROM ".$table_name.";"));
pg_field_name($result, 0);
pg_field_num($result, $field_name);
pg_field_size($result, 0);
pg_field_type($result, 0);
pg_field_prtlen($result, 0);
pg_field_is_null($result, 0);

if (!pg_send_query($db, "INSERT INTO ".$table_name." VALUES (8888, 'GGG');"))
{
    echo "pg_send_query() error\n";
}

pg_last_oid($result);
pg_free_result($result);


echo "OK";
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_04async_query";

$db = pg_connect($conn_str);
pg_query($db, "drop table {$table_name}");
?>
--EXPECT--
OK
