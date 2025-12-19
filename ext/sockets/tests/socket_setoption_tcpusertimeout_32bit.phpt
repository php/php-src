--TEST--
Test if socket_set_option() works, option:TCP_USER_TIMEOUT
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined('TCP_USER_TIMEOUT')) { die('skip TCP_USER_TIMEOUT is not defined'); }
if (PHP_INT_SIZE != 4) { die("skip 32-bit only"); }
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
        die('Unable to create AF_INET socket [socket]');
}
socket_set_block($socket);

try {
	socket_setopt($socket, SOL_TCP, TCP_USER_TIMEOUT, -1);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

$timeout = 200;
$retval_2 = socket_set_option($socket, SOL_TCP, TCP_USER_TIMEOUT, $timeout);
$retval_3 = socket_get_option($socket, SOL_TCP, TCP_USER_TIMEOUT);
var_dump($retval_2);
var_dump($retval_3 === $timeout);
socket_close($socket);
?>
--EXPECTF--
socket_setopt(): Argument #4 ($value) must be of between 0 and %d
bool(true)
bool(true)
