--TEST--
Test parameter handling in socket_listen().
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
--FILE--
<?php
$socket = socket_create(AF_UNIX, SOCK_STREAM, 0);
var_dump(socket_listen($socket));
?>
--EXPECTF--
Warning: socket_listen(): unable to listen on socket [%d]: Invalid argument in %s on line %d
bool(false)
--CREDITS--
Till Klampaeckel, till@php.net
Berlin TestFest 2009
