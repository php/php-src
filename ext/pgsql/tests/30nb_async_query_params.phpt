--TEST--
PostgreSQL non-blocking async query params
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
if (!function_exists('pg_send_query_params')) die('skip function pg_send_query_params() does not exist');
?>
--FILE--
<?php

include('inc/config.inc');
include('inc/nonblocking.inc');
$table_name = "table_30nb_async_query_params";

$db = pg_connect($conn_str);
pg_query($db, "create table {$table_name} (num int, str text, bin bytea)");
pg_query($db, "insert into {$table_name} (num) values(1000)");

$db_socket = pg_socket($db);
stream_set_blocking($db_socket, false);

$sent = pg_send_query_params($db, "SELECT * FROM ".$table_name." WHERE num > \$1;", array(100));
if ($sent === FALSE) {
    echo "pg_send_query_params() error\n";
} elseif ($sent === 0) {
    nb_flush($db, $db_socket);
}

nb_consume($db, $db_socket);

if (!($result = pg_get_result($db))) {
    echo "pg_get_result() error\n";
}
if (!($rows = pg_num_rows($result))) {
    echo "pg_num_rows() error\n";
}
for ($i=0; $i < $rows; $i++) {
    pg_fetch_array($result, $i, PGSQL_NUM);
}
for ($i=0; $i < $rows; $i++) {
    pg_fetch_object($result);
}
for ($i=0; $i < $rows; $i++) {
    pg_fetch_row($result, $i);
}
for ($i=0; $i < $rows; $i++) {
    pg_fetch_result($result, $i, 0);
}

pg_num_rows(pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num > \$1;", array(100)));
pg_num_fields(pg_query_params($db, "SELECT * FROM ".$table_name." WHERE num > \$1;", array(100)));
pg_field_name($result, 0);
pg_field_num($result, "num");
pg_field_size($result, 0);
pg_field_type($result, 0);
pg_field_prtlen($result, 0);
pg_field_is_null($result, 0);

$sent = pg_send_query_params($db, "INSERT INTO ".$table_name." VALUES (\$1, \$2);", array(9999, "A'BC"));

if ($sent === FALSE) {
    echo "pg_send_query_params() error\n";
} elseif ($sent === 0) {
    nb_flush($db, $db_socket);
}

pg_last_oid($result);
pg_free_result($result);

pg_close($db);

echo "OK";
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_30nb_async_query_params";

$db = pg_connect($conn_str);
pg_query($db, "drop table {$table_name}");
?>
--EXPECT--
OK
