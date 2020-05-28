--TEST--
socket_import_stream: Bad arguments
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
	die('SKIP sockets extension not available.');
}
--FILE--
<?php

var_dump(socket_import_stream());
var_dump(socket_import_stream(1, 2));
var_dump(socket_import_stream(1));
var_dump(socket_import_stream(new stdclass));
var_dump(socket_import_stream(fopen(__FILE__, "rb")));
var_dump(socket_import_stream(socket_create(AF_INET, SOCK_DGRAM, SOL_UDP)));
$s = stream_socket_server("udp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND);
var_dump($s);
var_dump(fclose($s));
var_dump(socket_import_stream($s));


echo "Done.";
--EXPECTF--
Warning: socket_import_stream() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: socket_import_stream() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: socket_import_stream() expects parameter 1 to be resource, int given in %s on line %d
NULL

Warning: socket_import_stream() expects parameter 1 to be resource, object given in %s on line %d
NULL

Warning: socket_import_stream(): cannot represent a stream of type STDIO as a Socket Descriptor in %s on line %d
bool(false)

Warning: socket_import_stream(): supplied resource is not a valid stream resource in %s on line %d
bool(false)
resource(%d) of type (stream)
bool(true)

Warning: socket_import_stream(): supplied resource is not a valid stream resource in %s on line %d
bool(false)
Done.
