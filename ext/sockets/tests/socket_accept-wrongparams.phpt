--TEST--
Test parameter handling in socket_accept()
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
--FILE--
<?php
var_dump(socket_accept(null));
--CREDITS--
Till Klampaeckel, till@php.net
Berlin TestFest 2009
--EXPECTF--
Warning: socket_accept() expects parameter 1 to be resource, null given in %s on line %d
NULL
