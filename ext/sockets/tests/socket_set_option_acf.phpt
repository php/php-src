--TEST--
Test if socket_set_option() works, option:SO_ACCEPTFILTER
--DESCRIPTION--
-wrong params
-set/get params comparison
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!defined("SO_ACCEPTFILTER")) {
	die('SKIP on platforms not supporting SO_ACCEPTFILTER');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

if (!$socket) {
        die('Unable to create AF_INET socket [socket]');
}
try {
	var_dump(socket_set_option( $socket, SOL_SOCKET, SO_ACCEPTFILTER, 1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
socket_listen($socket);
var_dump(socket_set_option( $socket, SOL_SOCKET, SO_ACCEPTFILTER, "httpready"));
var_dump(socket_get_option( $socket, SOL_SOCKET, SO_ACCEPTFILTER));
socket_close($socket);
?>
--EXPECTF--
Warning: socket_set_option(): Invalid filter argument type in %s on line %d
bool(false)
bool(true)
array(1) {
  ["af_name"]=>
  string(9) "httpready"
}
