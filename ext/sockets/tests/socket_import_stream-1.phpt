--TEST--
socket_import_stream: Basic test
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
	die('SKIP sockets extension not available.');
}
--FILE--
<?php

$domain = (strtoupper(substr(PHP_OS, 0, 3) == 'WIN') ? STREAM_PF_INET : STREAM_PF_UNIX);
$s = stream_socket_pair($domain, STREAM_SOCK_STREAM, 0);

$s0 = reset($s);
$s1 = next($s);

$sock = socket_import_stream($s0);
var_dump($sock);
socket_write($sock, "test message");
socket_close($sock);

var_dump(stream_get_contents($s1));
--EXPECTF--
resource(%d) of type (Socket)
string(12) "test message"
