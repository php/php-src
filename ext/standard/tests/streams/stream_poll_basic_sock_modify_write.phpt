--TEST--
Stream polling - socket modify write
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1, $socket2) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket2, STREAM_POLL_WRITE, "socket_data");
stream_poll_modify($poll_ctx, $socket2, STREAM_POLL_WRITE, "modified_data");

pt_print_events(stream_poll_wait($poll_ctx, 0));
?>
--EXPECT--
Events count: 1
Event[0]: 2, user data: modified_data
