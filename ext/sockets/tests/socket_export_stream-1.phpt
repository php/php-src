--TEST--
socket_export_stream: Basic test
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP sockets extension not available.');
}
--FILE--
<?php

$domain = (strtoupper(substr(PHP_OS, 0, 3) == 'WIN') ? AF_INET : AF_UNIX);
socket_create_pair($domain, SOCK_STREAM, 0, $s);

$s0 = reset($s);
$s1 = next($s);

$stream = socket_export_stream($s0);
var_dump($stream);

socket_write($s1, "test message");
socket_close($s1);

var_dump(stream_get_contents($stream));
?>
--EXPECTF--
resource(%d) of type (stream)
string(12) "test message"
