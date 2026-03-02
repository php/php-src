--TEST--
stream context tcp_nodelay
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
?>
--FILE--
<?php
$ctxt = stream_context_create([
    "socket" => [
        "tcp_nodelay" => true
    ]
]);

$stream = stream_socket_client(
    "tcp://www.php.net:80", $errno, $errstr, 10, STREAM_CLIENT_CONNECT, $ctxt);

$socket =
    socket_import_stream($stream);

var_dump(socket_get_option($socket, SOL_TCP, TCP_NODELAY) > 0);
?>
--EXPECT--
bool(true)
