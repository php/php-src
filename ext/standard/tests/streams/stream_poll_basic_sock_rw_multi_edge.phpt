--TEST--
Stream polling - socket write / read multiple times with edger triggering
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket1r, STREAM_POLL_READ | STREAM_POLL_ET, "socket1_data");
stream_poll_add($poll_ctx, $socket1w, STREAM_POLL_WRITE | STREAM_POLL_ET, "socket2_data");

pt_print_events(stream_poll_wait($poll_ctx, 0));
pt_print_events(stream_poll_wait($poll_ctx, 0));
fwrite($socket1w, "test data");
pt_print_events(stream_poll_wait($poll_ctx, 100), true);
fwrite($socket1w, "more data");
pt_print_events(stream_poll_wait($poll_ctx, 100));
pt_print_events(stream_poll_wait($poll_ctx, 100));
fwrite($socket1w, " and even more data");
pt_print_events(stream_poll_wait($poll_ctx, 100), true);
fclose($socket1r);
pt_print_events(stream_poll_wait($poll_ctx, 100));
fclose($socket1w);
pt_print_events(stream_poll_wait($poll_ctx, 100));

?>
--EXPECT--
Events count: 1
Event[0]: 2, user data: socket2_data
Events count: 0
Events count: 1
Event[0]: 1, user data: socket1_data, read data: 'test data'
Events count: 2
Event[0]: 2, user data: socket2_data
Event[1]: 1, user data: socket1_data
Events count: 0
Events count: 1
Event[0]: 1, user data: socket1_data, read data: 'more data and even more data'
Events count: 1
Event[0]: 10, user data: socket2_data
Events count: 0
