--TEST--
Poll stream - socket write close
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();
list($socket2r, $socket2w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $socket1w, POLL_EVENT_WRITE, "socket1w_data");
pt_stream_poll_add($poll_ctx, $socket2w, POLL_EVENT_WRITE, "socket2w_data");

fclose($socket1w);
fclose($socket2w);
pt_expect_events($poll_ctx->wait(100), []);

?>
--EXPECT--
Events matched - count: 0
