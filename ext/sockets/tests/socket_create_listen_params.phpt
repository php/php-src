--TEST--
ext/sockets - socket_create_listen - test for empty parameters
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
    $rand = rand(1,999);
    $s_c_l = socket_create_listen();
    var_dump($s_c_l);
    if ($s_c_l !== false) {
        @socket_close($s_c_l);
    }
?>
--EXPECTF--
Warning: socket_create_listen() expects at least 1 parameter, 0 given in %s on line %d
NULL
