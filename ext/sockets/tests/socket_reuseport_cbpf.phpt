--TEST--
Test if socket_set_option() works, option:SO_ATTACH_REUSEPORT_CBPF
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!defined("SO_ATTACH_REUSEPORT_CBPF")) {
	die('SKIP on platforms not supporting SO_ATTACH_REUSEPORT_CBPF');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

if (!$socket) {
        die('Unable to create AF_INET socket [socket]');
}
var_dump(socket_set_option( $socket, SOL_SOCKET, SO_REUSEADDR, true));
var_dump(socket_set_option( $socket, SOL_SOCKET, SO_REUSEPORT, true));
var_dump(socket_set_option( $socket, SOL_SOCKET, SO_ATTACH_REUSEPORT_CBPF, SKF_AD_CPU));
var_dump(socket_bind($socket, '0.0.0.0'));
socket_listen($socket);
socket_close($socket);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
