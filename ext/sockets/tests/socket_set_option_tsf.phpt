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
// TCP/RACK and other alternate stacks are not present by default, BBR should be available.
var_dump(socket_set_option( $socket, SOL_TCP, TCP_FUNCTION_BLK, "bbr"));
$bef = socket_get_option( $socket, SOL_TCP, TCP_BBR_ALGORITHM);
var_dump($bef);
// not failed, but unchanged
var_dump(socket_set_option( $socket, SOL_TCP, TCP_BBR_ALGORITHM, 1024));
$aft = socket_get_option( $socket, SOL_TCP, TCP_BBR_ALGORITHM);
var_dump($bef === $aft);
var_dump(socket_set_option( $socket, SOL_TCP, TCP_BBR_ALGORITHM, 0));
var_dump(socket_get_option( $socket, SOL_TCP, TCP_FUNCTION_BLK));
var_dump(socket_get_option( $socket, SOL_TCP, TCP_BBR_ALGORITHM));
socket_close($socket);
?>
--EXPECTF--
Warning: socket_set_option(): Unable to set socket option [2]: No such file or directory in %s on line %d
bool(true)
int(%d)
bool(true)
bool(true)
bool(true)
array(2) {
  ["function_set_name"]=>
  string(3) "bbr"
  ["pcbcnt"]=>
  int(%d)
}
int(%d)
