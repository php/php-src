--TEST--
Test if socket_set_option() works, option:SO_BINDTODEVICE
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
// wrong params
$retval_1 = socket_set_option( $socket, SOL_SOCKET, SO_BINDTODEVICE, "lo");
var_dump($retval_1);
$retval_2 = socket_set_option( $socket, SOL_SOCKET, SO_BINDTODEVICE, "ethIDONOTEXIST");
var_dump($retval_2);

socket_close($socket);
?>

--EXPECTF--
bool(true)

Warning: socket_set_option(): unable to set socket option [19]: No such device in %s on line %d
bool(false)
--CREDITS--
Damjan Cvetko, foreach.org
