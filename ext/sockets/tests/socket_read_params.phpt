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
    $s_c_l = socket_create_listen(31330+$rand);
    $s_c = socket_read($s_c_l, 25);
    socket_close($s_c_l);
?>
--EXPECTF--
Warning: socket_read(): unable to read from socket [%i]: %a in %s on line %d
