--TEST--
PostgreSQL pipeline mode
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
if (!defined('PGSQL_PIPELINE_SYNC') || !function_exists('pg_send_query_params')) {
    die('skip pipeline mode not available');
}
?>
--FILE--
<?php

include('inc/config.inc');
include('inc/nonblocking.inc');

$db = false;

function failure($method) {
    $backtrace = debug_backtrace();
    $line = $backtrace[0]['line'] + 12;
    global $db;
    $error = $db ? pg_last_error($db) : 'unknown error';
    die("{$method} failed on line {$line}: {$error}");
}

if (!$db = pg_connect($conn_str, PGSQL_CONNECT_ASYNC)) {
    failure('pg_connect');
} elseif (pg_connection_status($db) === PGSQL_CONNECTION_BAD) {
    failure('pg_connect');
} elseif ($db_socket = pg_socket($db)) {
    stream_set_blocking($db_socket, FALSE);
} else {
    failure('pg_socket');
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
            failure('async connection failed');
        case PGSQL_POLLING_OK:
            break 2;
        default:
            failure("unknown poll status {$status}");
    }
}

if (!pg_enter_pipeline_mode($db)) {
    failure('pg_enter_pipeline_mode');
}

if (!pg_send_query_params($db, "select $1 as index, now() + ($1||' day')::interval as time", array(1))) {
    failure('pg_send_query_params');
}

if (!pg_flush($db)) {
    failure('pg_flush');
}

for ($i = 2; $i < 50; ++$i) {
    if (!pg_send_query_params($db, "select $1 as index, now() + ($1||' day')::interval as time", array($i))) {
        failure('pg_send_query_params');
    }
}

if (!pg_send_flush_request($db)) {
    failure('pg_send_flush_request');
}

for ($i = 50; $i < 99; ++$i) {
    if (!pg_send_query_params($db, "select $1 as index, now() + ($1||' day')::interval as time", array($i))) {
        failure('pg_send_query_params');
    }
}

if (!pg_pipeline_sync($db)) {
    failure('pg_pipeline_sync');
}

if (pg_pipeline_status($db) !== PGSQL_PIPELINE_ON) {
    failure('pg_pipeline_status');
}

if (!($stream = pg_socket($db))) {
    failure('pg_socket');
}

if (pg_connection_busy($db)) {
    $read = [$stream]; $write = $ex = [];
    while (!stream_select($read, $write, $ex, null, null)) { }
}

for ($i = 1; $i < 99; ++$i) {
    if (!($result = pg_get_result($db))) {
        failure('pg_get_result');
    }

    if (pg_result_status($result) !== PGSQL_TUPLES_OK) {
        failure('pg_result_status');
    }

    if (pg_num_rows($result) == -1) {
        failure('pg_num_rows');
    }

    if (!($row = pg_fetch_row($result, null))) {
        failure('pg_fetch_row');
    }

    pg_free_result($result);

    if (pg_get_result($db) !== false) {
        failure('pg_get_result');
    }
}

if (($result = pg_get_result($db)) !== false) {
	if (pg_result_status($result) !== PGSQL_PIPELINE_SYNC) {
		failure('pg_result_status');
	}
}

for ($i = 99; $i < 199; ++$i) {
    if (!pg_send_query_params($db, "select $1 as index, now() + ($1||' day')::interval as time, pg_sleep(0.001)", array($i))) {
        failure('pg_send_query_params');
    }
}

if (!pg_pipeline_sync($db)) {
    failure('pg_pipeline_sync');
}

usleep(10000);

pg_cancel_query($db);

if (pg_pipeline_status($db) !== PGSQL_PIPELINE_ON) {
    failure('pg_pipeline_status');
}

if (pg_connection_busy($db)) {
    $read = [$stream]; $write = $ex = [];
    while (!stream_select($read, $write, $ex, null, null)) { }
}

$canceled_count = 0;
for ($i = 99; $i < 199; ++$i) {
    if (!($result = pg_get_result($db))) {
        failure('pg_get_result');
    }

    $result_status = pg_result_status($result);
    if ($result_status === PGSQL_FATAL_ERROR) {
        if (pg_connection_status($db) !== PGSQL_CONNECTION_OK) {
            failure('pg_cancel_query');
        }
        if (strpos(pg_last_error($db), 'canceling statement') === false) {
            failure('pg_cancel_query');
        }
        pg_free_result($result);
        if ($result = pg_get_result($db)) {
            failure('pg_get_result');
        }
        continue;
    }
    if ($result_status === 11/*PGSQL_STATUS_PIPELINE_ABORTED*/) {
        ++$canceled_count;
        pg_free_result($result);
        if ($result = pg_get_result($db)) {
            failure('pg_get_result');
        }
        continue;
    }
    if ($result_status !== PGSQL_TUPLES_OK) {
        failure('pg_result_status');
    }

    if (pg_num_rows($result) == -1) {
        failure('pg_num_rows');
    }

    if (!($row = pg_fetch_row($result, null))) {
        failure('pg_fetch_row');
    }

    pg_free_result($result);

    if (pg_get_result($db) !== false) {
        failure('pg_get_result');
    }
}

if ($canceled_count < 1) {
    failure('pg_cancel_query');
}

if (($result = pg_get_result($db)) !== false) {
    if (pg_result_status($result) !== PGSQL_PIPELINE_SYNC) {
        failure('pg_result_status');
    }
}

if (!pg_exit_pipeline_mode($db)) {
    failure('pg_exit_pipeline_mode');
}

if (!pg_enter_pipeline_mode($db)) {
    failure('pg_exit_pipeline_mode');
}

if (!pg_send_query_params($db, "create table if not exists __test__pg_pipeline_sync__test__(f1 integer, f2 character varying)", array())) {
    failure('pg_send_query_params');
}

if (pg_exit_pipeline_mode($db)) {
    failure('pg_exit_pipeline_mode');
}

pg_close($db);

if (!$db = pg_connect($conn_str)) {
    failure('pg_connect');
}

if (!pg_send_query_params($db, "select * from __test__pg_pipeline_sync__test__", array())) {
    failure('pg_send_query_params');
}

if (!($stream = pg_socket($db))) {
    failure('pg_socket');
}

if (pg_connection_busy($db)) {
    $read = [$stream]; $write = $ex = [];
    while (!stream_select($read, $write, $ex, null, null)) { }
}

if (!($result = pg_get_result($db))) {
    failure('pg_get_result');
}

if (pg_result_status($result) !== PGSQL_FATAL_ERROR) {
    failure('pg_result_status');
}

pg_free_result($result);

if (pg_get_result($db) !== false) {
    failure('pg_get_result');
}

pg_close($db);

if (!$db = pg_connect($conn_str)) {
    failure('pg_connect');
}

if (!pg_enter_pipeline_mode($db)) {
    failure('pg_enter_pipeline_mode');
}

if (!pg_send_query_params($db, "select $1 as index, now() + ($1||' day')::interval as time", array(199))) {
    failure('pg_send_query_params');
}

@pg_query_params($db, "select $1 as index, now() + ($1||' day')::interval as time", array(200));
if (strpos(pg_last_error($db), 'synchronous command execution functions are not allowed in pipeline mode') === false) {
    failure('pg_query_params');
}

pg_close($db);

echo "OK";

?>
--EXPECT--
OK
