--TEST--
SO_RCVBUF and SO_SNDBUF context options reject invalid values
--FILE--
<?php
foreach (['so_rcvbuf', 'so_sndbuf'] as $option) {
    foreach ([0, -1, 'abc'] as $value) {
        $context = stream_context_create(['socket' => [$option => $value]]);
        var_dump(@stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr,
            STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context));
        echo $errstr, PHP_EOL;
    }
}

$context = stream_context_create(['socket' => ['so_rcvbuf' => 0]]);
var_dump(@stream_socket_client("tcp://127.0.0.1:1", $errno, $errstr, 1,
    STREAM_CLIENT_CONNECT, $context));
echo $errstr, PHP_EOL;

?>
--EXPECT--
bool(false)
so_rcvbuf context option must be between 1 and 2147483647
bool(false)
so_rcvbuf context option must be between 1 and 2147483647
bool(false)
so_rcvbuf context option must be between 1 and 2147483647
bool(false)
so_sndbuf context option must be between 1 and 2147483647
bool(false)
so_sndbuf context option must be between 1 and 2147483647
bool(false)
so_sndbuf context option must be between 1 and 2147483647
bool(false)
so_rcvbuf context option must be between 1 and 2147483647
