--TEST--
ext/sockets - socket_read- test with empty parameters
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
    $s_c = socket_read();
    $s_c = socket_read(14);
    $s_c_l = socket_create_listen(31330+$rand);
    $s_c = socket_read($s_c_l, 25);
    socket_close($s_c_l);
?>
--EXPECTF--

Warning: socket_read() expects at least 2 parameters, 0 given in %s on line %i

Warning: socket_read() expects at least 2 parameters, 1 given in %s on line %i

Warning: socket_read(): unable to read from socket [%i]: Transport endpoint is not connected in %s on line %i
