--TEST--
Test if socket_set_option() works, option:TCP_FUNCTION_BLK
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!defined("TCP_FUNCTION_BLK")) {
	die('SKIP on platforms not supporting TCP_FUNCTION_BLK');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

if (!$socket) {
        die('Unable to create AF_INET socket [socket]');
}
socket_set_option( $socket, SOL_TCP, TCP_FUNCTION_BLK, "nochancetoexist");
// TCP/RACK and other alternate stacks are not present by default, at least `freebsd` is.
var_dump(socket_set_option( $socket, SOL_TCP, TCP_FUNCTION_BLK, "freebsd"));
var_dump(socket_get_option( $socket, SOL_TCP, TCP_FUNCTION_BLK));
socket_close($socket);
?>
--EXPECTF--
Warning: socket_set_option(): Unable to set socket option [2]: No such file or directory in %s on line %d
bool(true)
array(2) {
  ["function_set_name"]=>
  string(7) "freebsd"
  ["pcbcnt"]=>
  int(%d)
}
