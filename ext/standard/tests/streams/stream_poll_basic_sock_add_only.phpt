--TEST--
Stream polling basic functionality - only add
--SKIPIF--
<?php
if (!function_exists('stream_poll_create')) {
    die("skip stream polling not available");
}
?>
--FILE--
<?php

$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
if ($sockets === false) {
    echo "Cannot create socket pair\n";
    exit(0);
}
list($socket1, $socket2) = $sockets;

$poll_ctx = stream_poll_create();

stream_poll_add($poll_ctx, $socket2, STREAM_POLL_WRITE, "socket2_data");

fclose($socket1);
fclose($socket2);

var_dump($poll_ctx);

?>
--EXPECT--
object(StreamPollContext)#1 (0) {
}
