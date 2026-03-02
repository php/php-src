--TEST--
Poll stream - socket write / read few time only
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $socket1r, [Io\Poll\Event::Read], "socket1_data");
pt_stream_poll_add($poll_ctx, $socket1w, [Io\Poll\Event::Write], "socket2_data");

pt_expect_events($poll_ctx->wait(0), [
    ['events' => [Io\Poll\Event::Write], 'data' => 'socket2_data']
]);
fwrite($socket1w, "test data");
pt_expect_events($poll_ctx->wait(0, 100000), [
    ['events' => [Io\Poll\Event::Write], 'data' => 'socket2_data'],
    ['events' => [Io\Poll\Event::Read], 'data' => 'socket1_data', 'read' => 'test data']
]);

?>
--EXPECT--
Events matched - count: 1
Events matched - count: 2
