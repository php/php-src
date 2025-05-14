--TEST--
SO_SPLICE usage with socket_set_option
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!defined('SO_SPLICE')) {
    die('skip SO_SPLICE not available.');
}

?>
--FILE--
<?php
class badClass {}
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$todrain = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

try {
	socket_set_option($socket, SOL_SOCKET, SO_SPLICE, $todrain);
} catch (\TypeError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	socket_set_option($socket, SOL_SOCKET, SO_SPLICE, new badClass());
} catch (\TypeError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	$s = new SocketSoSplice();
	$s->max = -1;
	$s->socket = $todrain;
	$s->time = ["sec" => 0, "usec" => 0];
	socket_set_option($socket, SOL_SOCKET, SO_SPLICE, $s);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

socket_close($todrain);
socket_close($socket);
?>
--EXPECT--
socket_set_option(): Argument #4 ($value) must be of type SocketSoSplice, Socket given
socket_set_option(): Argument #4 ($value) must be of type SocketSoSplice, badClass given
socket_set_option(): Argument #4 ($value) "max" key must be greater than or equal to 0
