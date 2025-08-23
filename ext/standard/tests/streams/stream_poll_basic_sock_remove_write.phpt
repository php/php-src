--TEST--
Stream polling - socket remove write
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
list($socket2r, $socket2w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket1w, STREAM_POLL_WRITE, "socket_data_1");
stream_poll_add($poll_ctx, $socket2w, STREAM_POLL_WRITE, "socket_data_2");

pt_print_events(stream_poll_wait($poll_ctx, 0));

stream_poll_remove($poll_ctx, $socket1w);

pt_print_events(stream_poll_wait($poll_ctx, 0));

// check that both streams are still usable
var_dump(fwrite($socket1w, "test 1"));
var_dump(fwrite($socket2w, "test 2"));
var_dump(fread($socket1r, 100));
var_dump(fread($socket2r, 100));

?>
--EXPECT--
Events count: 2
Event[0]: 2, user data: socket_data_1
Event[1]: 2, user data: socket_data_2
Events count: 1
Event[0]: 2, user data: socket_data_2
int(6)
int(6)
string(6) "test 1"
string(6) "test 2"

