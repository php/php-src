--TEST--
Stream polling - TCP read write level
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($client, $server) = pt_new_tcp_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $client, STREAM_POLL_READ | STREAM_POLL_WRITE | STREAM_POLL_ONESHOT, "client_data");
stream_poll_add($poll_ctx, $server,  STREAM_POLL_READ | STREAM_POLL_WRITE | STREAM_POLL_ONESHOT, "server_data");

pt_print_events(stream_poll_wait($poll_ctx, 0));
pt_write_sleep($client, "test data");
pt_print_events(stream_poll_wait($poll_ctx, 100), true);
pt_write_sleep($client, "test data");
pt_print_events(stream_poll_wait($poll_ctx, 100), true);

?>
--EXPECT--
Events count: 2
Event[0]: 2, user data: client_data
Event[1]: 2, user data: server_data
Events count: 0
Events count: 0
