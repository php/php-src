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
    var_dump(socket_export_stream(fopen(__FILE__, "rb")));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(socket_export_stream(stream_socket_server("udp://127.0.0.1:58392", $errno, $errstr, STREAM_SERVER_BIND)));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
$s = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
var_dump($s);
socket_close($s);
try {
    var_dump(socket_export_stream($s));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}


echo "Done.";
?>
--EXPECTF--
socket_export_stream(): supplied resource is not a valid Socket resource
socket_export_stream(): supplied resource is not a valid Socket resource
resource(%d) of type (Socket)
socket_export_stream(): supplied resource is not a valid Socket resource
Done.
