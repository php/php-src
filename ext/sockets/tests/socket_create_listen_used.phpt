--TEST--
ext/sockets - socket_create_listen - test for used socket
--CREDITS--
Florian Anderiasch
fa@php.net
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('skip - sockets extension not available.');
}
?>
--FILE--
<?php
$s_c_l = socket_create_listen(0);
var_dump($s_c_l);

socket_getsockname($s_c_l, $addr, $port);

$s_c_l2 = socket_create_listen($port);
var_dump($s_c_l2);
socket_close($s_c_l);

?>
--EXPECTF--
object(Socket)#%d (0) {
}

Warning: socket_create_listen(): unable to bind to given address [%i]: %a in %s on line %d
bool(false)
