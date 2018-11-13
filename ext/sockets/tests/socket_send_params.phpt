--TEST--
ext/sockets - socket_send - test with incorrect parameters
--SKIPIF--
<?php
    if (!extension_loaded('sockets')) {
        die('skip sockets extension not available.');
    }
?>
--FILE--
<?php
    $rand = rand(1,999);
    $s_c = socket_create_listen(31330+$rand);
    $s_w = socket_send($s_c, "foo", -1, MSG_OOB);
    socket_close($s_c);
?>
--EXPECTF--
Warning: socket_send(): Length cannot be negative in %s on line %i
