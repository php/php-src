--TEST--
Poll stream - only add
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1, $socket2) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $socket2, [Io\Poll\Event::Write], "socket2_data");

var_dump($poll_ctx);

?>
--EXPECT--
object(Io\Poll\Context)#1 (0) {
}
