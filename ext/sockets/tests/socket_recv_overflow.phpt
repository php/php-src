--TEST--
socket_recvfrom overflow on length argument
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (strtolower(substr(PHP_OS, 0, 3)) === 'win') {
    die('skip not valid for Windows.');
}
--FILE--
<?php
$s = socket_create(AF_UNIX, SOCK_DGRAM, 0);
$buf = $end = "";
var_dump(socket_recvfrom($s, $buf, PHP_INT_MAX, 0, $end));
var_dump(socket_recvfrom($s, $buf, -1, 0, $end));
?>
--EXPECT--
bool(false)
bool(false)
