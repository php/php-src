--TEST--
UDP_SEGMENT setsockopt(), can't really test is as the kernel support might not be enabled.
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined('UDP_SEGMENT')) { die('skip UDP_SEGMENT is not defined'); }
?>
--FILE--
<?php
$src = socket_create(AF_UNIX, SOCK_DGRAM, 0);

try {
	socket_setopt($src, SOL_UDP, UDP_SEGMENT, -1);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	socket_setopt($src, SOL_UDP, UDP_SEGMENT, 65536);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
socket_setopt(): Argument #4 ($value) must be of between 0 and 65535
socket_setopt(): Argument #4 ($value) must be of between 0 and 65535
