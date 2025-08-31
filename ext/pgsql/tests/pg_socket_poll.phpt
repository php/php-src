--TEST--
PostgreSQL poll on connection's socket
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
?>
--FILE--
<?php

include('inc/config.inc');
include('inc/nonblocking.inc');

if (!($db = pg_connect($conn_str, PGSQL_CONNECT_ASYNC))) die("pg_connect failed");
$socket = pg_socket($db);
$fp = fopen(__DIR__ . "/inc/config.inc", "r");
try {
	pg_socket_poll($fp, 0, 0);
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}

if (($topoll = pg_socket_poll($socket, 1, 1, 1)) === -1) die("pg_socket_poll failed");
stream_set_blocking($socket, false);

while (1) {
    switch ($status = pg_connect_poll($db)) {
        case PGSQL_POLLING_READING:
            nb_is_writable($socket);
            break;
        case PGSQL_POLLING_WRITING:
            nb_is_writable($socket);
            break;
        case PGSQL_POLLING_OK:
            break 2;
        default:
            die("poll failed");
    }
}
assert(pg_connection_status($db) === PGSQL_CONNECTION_OK);
echo "OK";

pg_close($db);

?>
--EXPECT--
pg_socket_poll(): Argument #1 ($socket) invalid resource socket
OK
