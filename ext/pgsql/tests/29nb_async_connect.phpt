--TEST--
PostgreSQL non-blocking async connect
--SKIPIF--
<?php
include("skipif.inc");
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

while (TRUE) {
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
assert(pg_connection_status($db) === PGSQL_CONNECTION_OK);
echo "OK";

pg_close($db);

?>
--EXPECT--
OK
