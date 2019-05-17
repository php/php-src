--TEST--
ext/sockets - socket_set_block - basic test
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
    socket_set_nonblock($s_c_l);
    var_dump($s_c_l);
    #socket_accept($s_c_l);
    socket_close($s_c_l);
?>
--EXPECTF--
resource(%i) of type (Socket)
