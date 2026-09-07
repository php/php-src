--TEST--
ext/sockets - socket_send - test with incorrect parameters
--EXTENSIONS--
sockets
--FILE--
<?php
    $s_c = socket_create_listen(0);
    try {
        $s_w = socket_send($s_c, "foo", -1, MSG_OOB);
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    socket_close($s_c);
?>
--EXPECT--
ValueError: socket_send(): Argument #3 ($length) must be greater than or equal to 0
