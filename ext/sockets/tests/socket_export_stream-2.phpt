--TEST--
socket_export_stream: Bad arguments
--EXTENSIONS--
sockets
--FILE--
<?php

try {
    socket_export_stream(fopen(__FILE__, "rb"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    socket_export_stream(stream_socket_server("udp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
$s = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
var_dump($s);
socket_close($s);

try {
    var_dump(socket_export_stream($s));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "Done.";
?>
--EXPECTF--
TypeError: socket_export_stream(): Argument #1 ($socket) must be of type Socket, resource given
TypeError: socket_export_stream(): Argument #1 ($socket) must be of type Socket, resource given
object(Socket)#%d (0) {
}
Error: socket_export_stream(): Argument #1 ($socket) has already been closed
Done.
