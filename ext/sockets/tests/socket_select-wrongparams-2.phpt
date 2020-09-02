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

try {
    socket_select($sockets, $write, $except, $time);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECTF--
socket_select(): At least one array argument must be passed
--CREDITS--
Till Klampaeckel, till@php.net
Berlin TestFest 2009
