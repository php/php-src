--TEST--
Stream polling - socket write close
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();
list($socket2r, $socket2w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket1w, STREAM_POLL_WRITE, "socket1w_data");
stream_poll_add($poll_ctx, $socket2w, STREAM_POLL_WRITE, "socket2w_data");

fclose($socket1w);
fclose($socket2w);
pt_expect_events(stream_poll_wait($poll_ctx, 100), []);

?>
--EXPECT--
Events matched - count: 0
