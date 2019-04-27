--TEST--
Test parameter handling in socket_create_pair()
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

var_dump(socket_create_pair(AF_INET, null, null, $sockets));

var_dump(socket_create_pair(31337, null, null, $sockets));

var_dump(socket_create_pair(AF_INET, 31337, 0, $sockets));
--EXPECTF--
bool(true)

Warning: socket_create_pair(): invalid socket domain [31337] specified for argument 1, assuming AF_INET in %s on line %d
bool(true)

Warning: socket_create_pair(): invalid socket type [31337] specified for argument 2, assuming SOCK_STREAM in %s on line %d
bool(true)
--CREDITS--
Till Klampaeckel, till@php.net
Berlin TestFest 2009
