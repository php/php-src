--TEST--
ext/sockets - socket_create_listen - test for used socket
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
    // wrong parameter count
    $s_c_l = socket_create_listen(31330+$rand);
    var_dump($s_c_l);
    // default invocation
    $s_c_l2 = socket_create_listen(31330+$rand);
    var_dump($s_c_l2);
    socket_close($s_c_l2);
    socket_close($s_c_l);
?>
--EXPECTF--
resource(%i) of type (Socket)

Warning: socket_create_listen(): unable to bind to given address [%i]: %a in %s on line %d
bool(false)

Warning: socket_close() expects parameter 1 to be resource, bool given in %s on line %d
