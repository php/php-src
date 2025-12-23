--TEST--
Poll stream - socket write close
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();
list($socket2r, $socket2w) = pt_new_socket_pair();

pt_stream_poll_add($poll_ctx, $socket1w, [Io\Poll\Event::Write], "socket1w_data");
pt_stream_poll_add($poll_ctx, $socket2w, [Io\Poll\Event::Write], "socket2w_data");

fclose($socket1w);
fclose($socket2w);
pt_expect_events($poll_ctx->wait(100), []);

?>
--EXPECT--
Events matched - count: 0
