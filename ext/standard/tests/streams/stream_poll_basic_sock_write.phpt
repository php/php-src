--TEST--
Stream polling basic functionality - socket write
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket1w, STREAM_POLL_WRITE, "socket_data");

$events = stream_poll_wait($poll_ctx, 0);
pt_print_events($events);

?>
--EXPECT--
Events count: 1
Event[0]: 2, user data: socket_data
