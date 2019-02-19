--TEST--
Test parameter handling in socket_select().
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
--FILE--
<?php
$sockets = null;
$write   = null;
$except  = null;
$time    = 0;
var_dump(socket_select($sockets, $write, $except, $time));
--EXPECTF--
Warning: socket_select(): no resource arrays were passed to select in %s on line %d
bool(false)
--CREDITS--
Till Klampaeckel, till@php.net
Berlin TestFest 2009
