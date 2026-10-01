--TEST--
socket_set_option() attach/detach round trip for reuseport CBPF filters
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined("SO_ATTACH_REUSEPORT_CBPF") || !defined("SO_DETACH_REUSEPORT_BPF")) {
	die('SKIP on platforms not supporting reuseport CBPF filters');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

var_dump(socket_set_option($socket, SOL_SOCKET, SO_REUSEPORT, true));
var_dump(socket_bind($socket, '127.0.0.1', 0));
var_dump(socket_listen($socket));

var_dump(socket_set_option($socket, SOL_SOCKET, SO_ATTACH_REUSEPORT_CBPF, SKF_AD_CPU));
var_dump(socket_set_option($socket, SOL_SOCKET, SO_DETACH_REUSEPORT_BPF, 1));
var_dump(socket_set_option($socket, SOL_SOCKET, SO_DETACH_REUSEPORT_BPF, 1));

var_dump(socket_set_option($socket, SOL_SOCKET, SO_ATTACH_REUSEPORT_CBPF, SKF_AD_QUEUE));
var_dump(socket_set_option($socket, SOL_SOCKET, SO_ATTACH_REUSEPORT_CBPF, 0));

socket_close($socket);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: socket_set_option(): Unable to set socket option [%d]: %s in %s on line %d
bool(false)
bool(true)
bool(true)
