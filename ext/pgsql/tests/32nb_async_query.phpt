--TEST--
PostgreSQL non-blocking async queries
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("skipif.inc");
if (!function_exists('pg_send_prepare')) die('skip function pg_send_prepare() does not exist');
?>
--FILE--
<?php

include('config.inc');
include('nonblocking.inc');

$db = pg_connect($conn_str);
$db_socket = pg_socket($db);
stream_set_blocking($db_socket, false);

$nb_send =  pg_send_query($db, "SELECT * FROM ".$table_name.";");
if ($nb_send === FALSE) {
    echo "pg_send_query() error\n";
} elseif ($nb_send === 0) {
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
for ($i=0; $i < $rows; $i++)  {
    pg_fetch_result($result, $i, 0);
}

pg_num_rows(pg_query($db, "SELECT * FROM ".$table_name.";"));
pg_num_fields(pg_query($db, "SELECT * FROM ".$table_name.";"));
pg_field_name($result, 0);
pg_field_num($result, $field_name);
pg_field_size($result, 0);
pg_field_type($result, 0);
pg_field_prtlen($result, null, 0);
pg_field_is_null($result, null, 0);

$nb_send = pg_send_query($db, "INSERT INTO ".$table_name." VALUES (8888, 'GGG');");
if ($nb_send === FALSE) {
    echo "pg_send_query() error\n";
} elseif ($nb_send === 0) {
    nb_flush($db, $db_socket);
}

nb_consume($db, $db_socket);

if (!($result = pg_get_result($db))) {
    echo "pg_get_result() error\n";
}

pg_last_oid($result);
pg_free_result($result);
pg_close($db);

echo "OK";
?>
--EXPECT--
OK
