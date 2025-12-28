--TEST--
socket_sendto() with invalid port
--EXTENSIONS--
sockets
--FILE--
<?php
    $s_c = socket_create_listen(0);
    try {
        $s_w = socket_sendto($s_c, "foo", 0, MSG_OOB, '127.0.0.1', 65536);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        $s_w = socket_sendto($s_c, "foo", 0, MSG_OOB, '127.0.0.1', -1);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    socket_close($s_c);
?>
--EXPECT--
socket_sendto(): Argument #6 ($port) must be between 0 and 65535
socket_sendto(): Argument #6 ($port) must be between 0 and 65535
