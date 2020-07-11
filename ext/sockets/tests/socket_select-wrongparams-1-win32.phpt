--TEST--
Test parameter handling in socket_select().
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
	die('skip.. Not valid for non Windows');
}
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
--FILE--
<?php
$sockets = array();
$domain = AF_INET;
socket_create_pair($domain, SOCK_STREAM, 0, $sockets);

$write  = null;
$except = null;
$time   = -1;
var_dump(socket_select($sockets, $write, $except, $time));
--EXPECT--
int(0)
--CREDITS--
Till Klampaeckel, till@php.net
Berlin TestFest 2009
