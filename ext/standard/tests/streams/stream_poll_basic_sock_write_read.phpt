--TEST--
Stream polling basic functionality - socket write / read
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1, $socket2) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket1, STREAM_POLL_READ, "socket1_data");
stream_poll_add($poll_ctx, $socket2, STREAM_POLL_WRITE, "socket2_data");

$events = stream_poll_wait($poll_ctx, 0);
pt_print_events($events);

fwrite($socket2, "test data");
$events = stream_poll_wait($poll_ctx, 100); // 100ms timeout
pt_print_events($events, true);

?>
--EXPECT--
Events count: 1
Event[0]: 2, user data: socket2_data
Events count: 2
Event[0]: 2, user data: socket2_data
Event[1]: 1, user data: socket1_data, read data: 'test data'
