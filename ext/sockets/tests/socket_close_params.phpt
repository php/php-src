--TEST--
ext/sockets - socket_close - test with empty parameters
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
    // wrong parameter count
    $s_c = socket_close();
    var_dump($s_c);  
?>
--EXPECTF--

Warning: socket_close() expects exactly 1 parameter, 0 given in %s on line %i
NULL
