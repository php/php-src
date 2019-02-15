--TEST--
ext/sockets - socket_listen - test with empty parameters
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
    // wrong parameter count
    $s_c = socket_listen();
    var_dump($s_c);
?>
--EXPECTF--
Warning: socket_listen() expects at least 1 parameter, 0 given in %s on line %d
NULL
