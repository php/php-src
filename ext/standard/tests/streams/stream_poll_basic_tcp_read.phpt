--TEST--
Stream polling - socket read
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1r, $socket1w) = pt_new_tcp_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket1r, STREAM_POLL_READ, "socket_data");

pt_write_sleep($socket1w, "test data");
pt_expect_events(stream_poll_wait($poll_ctx, 100), [
    ['events' => STREAM_POLL_READ, 'data' => 'socket_data', 'read' => 'test data']
]);

?>
--EXPECT--
Events matched - count: 1
