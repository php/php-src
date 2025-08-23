--TEST--
Stream polling - socket read
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket1r, STREAM_POLL_READ, "socket_data");

fwrite($socket1w, "test data");
pt_print_events(stream_poll_wait($poll_ctx, 100), true);

?>
--EXPECT--
Events count: 1
Event[0]: 1, user data: socket_data, read data: 'test data'
