--TEST--
ext/sockets - socket_create - test with empty parameters
--CREDITS--
Florian Anderiasch
fa@php.net
--SKIPIF--
<?php
    if (!extension_loaded('sockets')) {
        die('skip sockets extension not available.');
    }
?>
--FILE--
<?php
    $s_w = socket_create();
    $s_w = socket_create(AF_INET);
    $s_w = socket_create(AF_INET, SOCK_STREAM);
?>
--EXPECTF--

Warning: socket_create() expects exactly 3 parameters, 0 given in %s on line %i

Warning: socket_create() expects exactly 3 parameters, 1 given in %s on line %i

Warning: socket_create() expects exactly 3 parameters, 2 given in %s on line %i
