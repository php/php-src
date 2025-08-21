--TEST--
Stream polling basic functionality - only add
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1, $socket2) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket2, STREAM_POLL_WRITE, "socket2_data");

var_dump($poll_ctx);

?>
--EXPECT--
object(StreamPollContext)#1 (0) {
}
