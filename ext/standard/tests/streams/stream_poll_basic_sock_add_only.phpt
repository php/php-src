--TEST--
Stream polling basic functionality - only add
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
if ($sockets === false) {
    echo "Cannot create socket pair\n";
    exit(0);
}
list($socket1, $socket2) = $sockets;

$poll_ctx = new_stream_poll();

stream_poll_add($poll_ctx, $socket2, STREAM_POLL_WRITE, "socket2_data");

fclose($socket1);
fclose($socket2);

var_dump($poll_ctx);

?>
--EXPECT--
object(StreamPollContext)#1 (0) {
}
