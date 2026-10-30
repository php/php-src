--TEST--
Test for socket_create_pair() with SOCK_CLOEXEC/SOCK_NONBLOCK
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined('SOCK_CLOEXEC')) die("skip SOCK_CLOEXEC");
if (!defined('SOCK_NONBLOCK')) die("skip SOCK_NONBLOCK");
?>
--FILE--
<?php
$sockets = array();
try {
	socket_create_pair(AF_UNIX, 11 | SOCK_CLOEXEC, 0, $sockets);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_create_pair(AF_UNIX, 11 | SOCK_NONBLOCK, 0, $sockets);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_create_pair(AF_UNIX, 11 | SOCK_CLOEXEC | SOCK_NONBLOCK, 0, $sockets);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
var_dump(socket_create_pair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0, $sockets));
?>
--EXPECTF--
socket_create_pair(): Argument #2 ($type) must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, or SOCK_RDM%A
socket_create_pair(): Argument #2 ($type) must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, or SOCK_RDM%A
socket_create_pair(): Argument #2 ($type) must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, or SOCK_RDM%A
bool(true)
