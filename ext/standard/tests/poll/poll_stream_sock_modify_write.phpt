--TEST--
Poll stream - socket modify write
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1, $socket2) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

$watcher = pt_stream_poll_add($poll_ctx, $socket2, POLL_EVENT_WRITE, "socket_data");
$watcher->modify(POLL_EVENT_WRITE, "modified_data");

pt_expect_events($poll_ctx->wait(0), [
    ['events' => POLL_EVENT_WRITE, 'data' => 'modified_data']
]);
?>
--EXPECT--
Events matched - count: 1
