--TEST--
socket_export_stream: Bad arguments
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP sockets extension not available.');
}
--FILE--
<?php

try {
    socket_export_stream(fopen(__FILE__, "rb"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    socket_export_stream(stream_socket_server("udp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
$s = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
var_dump($s);
socket_close($s);

try {
    var_dump(socket_export_stream($s));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "Done.";
?>
--EXPECTF--
socket_export_stream(): Argument #1 ($socket) must be of type Socket, resource given
socket_export_stream(): Argument #1 ($socket) must be of type Socket, resource given
object(Socket)#%d (0) {
}
socket_export_stream(): Argument #1 ($socket) has already been closed
Done.
