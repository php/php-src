--TEST--
Test if socket_set_option() works, option:SO_SNDTIMEO
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
$retval_1 = socket_set_option( $socket, SOL_SOCKET, SO_SNDTIMEO, array());

//set/get comparison
$options = array("sec" => 1, "usec" => 0);
$retval_2 = socket_set_option( $socket, SOL_SOCKET, SO_SNDTIMEO, $options);
$retval_3 = socket_get_option( $socket, SOL_SOCKET, SO_SNDTIMEO);

var_dump($retval_2);
var_dump($retval_3 === $options);
socket_close($socket);
?>
--EXPECTF--
Warning: socket_set_option(): no key "sec" passed in optval in %s on line %d
bool(true)
bool(true)
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-10
