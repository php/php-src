--TEST--
ext/sockets - socket_sendto - test with incorrect parameters
--EXTENSIONS--
sockets
--FILE--
<?php
    $s_c = socket_create_listen(0);
    try {
        $s_w = socket_sendto($s_c, "foo", -1, MSG_OOB, '127.0.0.1');
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    socket_close($s_c);
?>
--EXPECT--
ValueError: socket_sendto(): Argument #3 ($length) must be greater than or equal to 0
