--TEST--
ext/sockets - socket_sendto - test with incorrect parameters
--SKIPIF--
<?php
    if (!extension_loaded('sockets')) {
        die('skip sockets extension not available.');
    }
?>
--FILE--
<?php
    $s_c = socket_create_listen(0);
    $s_w = socket_sendto($s_c, "foo", -1, MSG_OOB, '127.0.0.1');
    socket_close($s_c);
?>
--EXPECTF--
Warning: socket_sendto(): Length cannot be negative in %s on line %i
