--TEST--
Test if socket_set_option() works, option:SO_RCVTIMEO
--DESCRIPTION--
-wrong params
-set/get params comparison
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
        die('SKIP sockets extension not available.');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
        die('Unable to create AF_INET socket [socket]');
}
socket_set_block($socket);

//wrong params
$retval_1 = socket_set_option( $socket, SOL_SOCKET, SO_RCVTIMEO, array());

//set/get comparison
$options = array("sec" => 1, "usec" => 0);
$retval_2 = socket_set_option( $socket, SOL_SOCKET, SO_RCVTIMEO, $options);
$retval_3 = socket_get_option( $socket, SOL_SOCKET, SO_RCVTIMEO);

var_dump($retval_2);
var_dump($retval_3 === $options);
socket_close($socket);
?>
--EXPECTF--
Warning: socket_set_option(): No key "sec" passed in optval in %s on line %d
bool(true)
bool(true)
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-10
