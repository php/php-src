--TEST--
ext/sockets - socket_write - test with empty parameters
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
    $rand = rand(1,999);
    // wrong parameter count
    $s_w = socket_write();
    $s_c = socket_create_listen(31330+$rand);
    $s_w = socket_write($s_c);
    $s_w = socket_write($s_c, "foo");
    $s_w = socket_write($s_c, "foo", -1);
    socket_close($s_c);
?>
--EXPECTF--
Warning: socket_write() expects at least 2 parameters, 0 given in %s on line %i

Warning: socket_write() expects at least 2 parameters, 1 given in %s on line %i

Warning: socket_write(): unable to write to socket [%i]: %a in %s on line %i

Warning: socket_write(): Length cannot be negative in %s on line %i
