--TEST--
PostgreSQL pipeline mode
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("skipif.inc");
if (!defined('PGSQL_PIPELINE_SYNC') || !function_exists('pg_send_query_params')) {
    die('skip pipeline mode not available');
}
?>
--FILE--
<?php

include('config.inc');
include('nonblocking.inc');

if (!$db = pg_connect($conn_str, PGSQL_CONNECT_ASYNC)) {
    die("pg_connect() error");
} elseif (pg_connection_status($db) === PGSQL_CONNECTION_BAD) {
    die("pg_connect() error");
} elseif ($db_socket = pg_socket($db)) {
    stream_set_blocking($db_socket, FALSE);
} else {
    die("pg_socket() error");
}

while (true) {
    switch ($status = pg_connect_poll($db)) {
        case PGSQL_POLLING_READING:
            nb_is_readable($db_socket);
            break;
        case PGSQL_POLLING_WRITING:
            nb_is_writable($db_socket);
            break;
        case PGSQL_POLLING_FAILED:
            die("async connection failed");
        case PGSQL_POLLING_OK:
            break 2;
        default:
            die("unknown poll status");
    }
}

if (!pg_enter_pipeline_mode($db)) {
    die('pg_enter_pipeline_mode{}');
}

if (!pg_send_query_params($db, "select $1 as index, now() + ($1||' day')::interval as time", array(1))) {
    die('pg_send_query_params{}');
}

if (!pg_pipeline_sync($db)) {
    die('pg_pipeline_sync{}');
}

if (pg_pipeline_status($db) !== PGSQL_PIPELINE_ON) {
    die('pg_pipeline_status{}');
}

if (!($result = pg_get_result($db))) {
    die('pg_get_result()');
}

if (pg_num_rows($result) == -1) {
    die('pg_num_rows()');
}

if (!pg_fetch_row($result, null)) {
    die('pg_fetch_row()');
}

pg_free_result($result);
while (pg_connection_busy($db)) {
    nb_flush($db, $db_socket);
    nb_consume($db, $db_socket);
}

if (!pg_exit_pipeline_mode($db)) {
    die('pg_exit_pipeline_mode{}');
}

echo "OK";

pg_close($db);

?>
--EXPECT--
OK
