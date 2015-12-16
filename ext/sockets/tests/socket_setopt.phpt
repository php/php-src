--TEST--
bool socket_setopt( resource $socket , int $level , int $optname , mixed $optval ) ;
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--DESCRIPTION--
-Bind to loopback 'lo' device (should exist)
-Bind to unexisting device
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
  die('SKIP sockets extension not available.');
}
if (!defined("SO_BINDTODEVICE")) {
  die('SKIP SO_BINDTODEVICE not supported on this platform.');
}
if (!function_exists("posix_getuid") || posix_getuid() != 0) {
  die('SKIP SO_BINDTODEVICE requires root permissions.');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
  die('Unable to create AF_INET socket [socket]');
}
var_dump(socket_setopt( $socket, SOL_SOCKET, SO_BINDTODEVICE, "lo"));
?>
--CLEAN--
<?php
unset(socket);
socket_close($socket);
?>
--EXPECT--
bool(true)
