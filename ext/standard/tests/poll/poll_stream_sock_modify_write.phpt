--TEST--
Poll stream - socket modify write
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1, $socket2) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

$watcher = pt_stream_poll_add($poll_ctx, $socket2, [Io\Poll\Event::Write], "socket_data");
$watcher->modify([Io\Poll\Event::Write], "modified_data");

pt_expect_events($poll_ctx->wait(0), [
    ['events' => [Io\Poll\Event::Write], 'data' => 'modified_data']
]);
?>
--EXPECT--
Events matched - count: 1
