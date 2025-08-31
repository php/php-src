--TEST--
socket_import_stream: Bad arguments
--EXTENSIONS--
sockets
--FILE--
<?php

var_dump(socket_import_stream(fopen(__FILE__, "rb")));
try {
    socket_import_stream(socket_create(AF_INET, SOCK_DGRAM, SOL_UDP));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
$s = stream_socket_server("udp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND);
var_dump($s);
var_dump(fclose($s));
try {
    socket_import_stream($s);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done.";
?>
--EXPECTF--
Warning: socket_import_stream(): Cannot represent a stream of type STDIO as a Socket Descriptor in %s on line %d
bool(false)
socket_import_stream(): Argument #1 ($stream) must be of type resource, Socket given
resource(%d) of type (stream)
bool(true)
socket_import_stream(): supplied resource is not a valid stream resource
Done.
