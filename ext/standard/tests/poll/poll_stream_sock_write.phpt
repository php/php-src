--TEST--
Poll stream - socket write
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $socket1w, [Io\Poll\Event::Write], "socket_data");

pt_expect_events($poll_ctx->wait(0), [
    ['events' => [Io\Poll\Event::Write], 'data' => 'socket_data']
]);

?>
--EXPECT--
Events matched - count: 1
