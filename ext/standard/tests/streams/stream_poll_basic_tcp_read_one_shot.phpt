--TEST--
Stream polling - TCP read write level
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($client1, $server1) = pt_new_tcp_socket_pair();
list($client2, $server2) = pt_new_tcp_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $client1, STREAM_POLL_READ | STREAM_POLL_ONESHOT, "client1_data");
stream_poll_add($poll_ctx, $server1,  STREAM_POLL_READ  | STREAM_POLL_ONESHOT, "server1_data");
stream_poll_add($poll_ctx, $client2,  STREAM_POLL_READ  | STREAM_POLL_ONESHOT, "client2_data");
stream_poll_add($poll_ctx, $server2,  STREAM_POLL_READ  | STREAM_POLL_ONESHOT, "server2_data");

pt_print_events(stream_poll_wait($poll_ctx, 0));
pt_write_sleep($client1, "test data");
pt_write_sleep($client2, "test data");
pt_print_events(stream_poll_wait($poll_ctx, 100), true);
pt_write_sleep($client1, "more data");
pt_write_sleep($client2, "more data");
pt_print_events(stream_poll_wait($poll_ctx, 100), true);

?>
--EXPECT--
Events count: 0
Events count: 2
Event[0]: 1, user data: server1_data, read data: 'test data'
Event[1]: 1, user data: server2_data, read data: 'test data'
Events count: 0
