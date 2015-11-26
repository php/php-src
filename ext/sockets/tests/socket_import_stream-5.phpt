--TEST--
socket_import_stream: effects of leaked handles
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
	die('SKIP sockets extension not available.');
}
if (!function_exists('leak_variable'))
	die('SKIP only for debug builds');
--FILE--
<?php

$stream0 = stream_socket_server("udp://0.0.0.0:58380", $errno, $errstr, STREAM_SERVER_BIND);
$sock0 = socket_import_stream($stream0);
leak_variable($stream0, true);

$stream1 = stream_socket_server("udp://0.0.0.0:58381", $errno, $errstr, STREAM_SERVER_BIND);
$sock1 = socket_import_stream($stream1);
leak_variable($sock1, true);

echo "Done.\n";
--EXPECT--
Done.
