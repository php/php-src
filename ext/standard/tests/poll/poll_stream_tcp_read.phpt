--TEST--
Poll stream - socket read
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1r, $socket1w) = pt_new_tcp_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $socket1r, POLL_EVENT_READ, "socket_data");

pt_write_sleep($socket1w, "test data");
pt_expect_events($poll_ctx->wait(100), [
    ['events' => POLL_EVENT_READ, 'data' => 'socket_data', 'read' => 'test data']
]);

?>
--EXPECT--
Events matched - count: 1
