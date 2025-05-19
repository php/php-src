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


try {
	$s = new SocketSoSplice();
	socket_set_option($socket, SOL_SOCKET, SO_SPLICE, $s);
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	$s = new SocketSoSplice();
	$s->max = 1024;
	$s->socket = $todrain;
	$s->time = ["invalid" => 0, "usec" => 0];
	socket_set_option($socket, SOL_SOCKET, SO_SPLICE, $s);
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	$s = new SocketSoSplice();
	$s->max = 1024;
	$s->socket = $todrain;
	$s->time = ["sec" => 0, "nosec" => 0];
	socket_set_option($socket, SOL_SOCKET, SO_SPLICE, $s);
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

var_dump(socket_bind($todrain, '127.0.0.1'));
var_dump(socket_listen($todrain, 1));

socket_getsockname($todrain, $addr, $port);
socket_connect($socket, $addr, $port);
$peer = socket_accept($todrain);


$s1 = new SocketSoSplice();
$s1->max = 5;
$s1->socket = $peer;
$s1->time = ["sec" => 1, "usec" => 1];

var_dump(socket_set_option($socket, SOL_SOCKET, SO_SPLICE, $s1));
var_dump(socket_get_option($socket, SOL_SOCKET, SO_SPLICE));
socket_write($socket, "HELLO", 5);
var_dump(socket_read($peer, 5, PHP_NORMAL_READ));

socket_close($peer);
socket_close($todrain);
socket_close($socket);

?>
--EXPECTF--
socket_set_option(): Argument #4 ($value) must be of type SocketSoSplice, Socket given
socket_set_option(): Argument #4 ($value) must be of type SocketSoSplice, badClass given
socket_set_option(): Argument #4 ($value) "max" key must be greater than or equal to 0
Typed property SocketSoSplice::$socket must not be accessed before initialization
socket_set_option(): Argument #4 ($value) time must have key "sec"
socket_set_option(): Argument #4 ($value) time must have key "usec"
bool(true)
bool(true)
bool(true)
int(%d)
string(5) "HELLO"
