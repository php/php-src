--TEST--
Poll stream - socket write / read few time only
--SKIPIF--
<?php
require_once __DIR__ . '/poll.inc';
pt_skip_for_backend(['poll', 'wsapoll'], 'does not support edge triggering')
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $socket1r, POLL_EVENT_READ | POLL_EVENT_ET, "socket1_data");
pt_stream_poll_add($poll_ctx, $socket1w, POLL_EVENT_WRITE | POLL_EVENT_ET, "socket2_data");

pt_expect_events($poll_ctx->wait(0), [
    ['events' => POLL_EVENT_WRITE, 'data' => 'socket2_data']
]);
fwrite($socket1w, "test data");
pt_expect_events($poll_ctx->wait(100), [
    ['events' => POLL_EVENT_READ, 'data' => 'socket1_data', 'read' => 'test data']
]);

?>
--EXPECT--
Events matched - count: 1
Events matched - count: 1
