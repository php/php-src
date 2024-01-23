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
    die('pg_send_query_params failed');
}

if (!pg_flush($db)) {
    die('pg_flush failed');
}

for ($i = 2; $i < 50; ++$i) {
    if (!pg_send_query_params($db, "select $1 as index, now() + ($1||' day')::interval as time", array($i))) {
        die('pg_send_query_params failed');
    }
}

if (!pg_send_flush_request($db)) {
    die('pg_send_flush_request failed');
}

for ($i = 50; $i < 99; ++$i) {
    if (!pg_send_query_params($db, "select $1 as index, now() + ($1||' day')::interval as time", array($i))) {
        die('pg_send_query_params failed');
    }
}

if (!pg_pipeline_sync($db)) {
    die('pg_pipeline_sync failed');
}

if (pg_pipeline_status($db) !== PGSQL_PIPELINE_ON) {
    die('pg_pipeline_status failed');
}

if (!($stream = pg_socket($db))) {
    die('pg_socket');
}

if (pg_connection_busy($db)) {
    $read = [$stream]; $write = $ex = [];
    while (!stream_select($read, $write, $ex, null, null)) { }
}

for ($i = 1; $i < 99; ++$i) {
    if (!($result = pg_get_result($db))) {
        die('pg_get_result');
    }

    if (pg_result_status($result) !== PGSQL_TUPLES_OK) {
        die('pg_result_status failed');
    }

    if (pg_num_rows($result) == -1) {
        die('pg_num_rows failed');
    }

    if (!($row = pg_fetch_row($result, null))) {
        die('pg_fetch_row failed');
    }

    pg_free_result($result);

    if (pg_get_result($db) !== false) {
        die('pg_get_result failed');
    }
}

if (($result = pg_get_result($db)) !== false) {
	if (pg_result_status($result) !== PGSQL_PIPELINE_SYNC) {
		die('pg_result_status failed');
	}
}

if (!pg_exit_pipeline_mode($db)) {
    die('pg_exit_pipeline_mode failed');
}

echo "OK";

pg_close($db);

?>
--EXPECT--
OK
