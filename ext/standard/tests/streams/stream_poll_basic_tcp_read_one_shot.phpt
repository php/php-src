--TEST--
Stream polling - TCP read write oneshot
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($client1, $server1) = pt_new_tcp_socket_pair();
list($client2, $server2) = pt_new_tcp_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $client1, STREAM_POLL_READ | STREAM_POLL_ONESHOT, "client1_data");
stream_poll_add($poll_ctx, $server1, STREAM_POLL_READ | STREAM_POLL_ONESHOT, "server1_data");
stream_poll_add($poll_ctx, $client2, STREAM_POLL_READ | STREAM_POLL_ONESHOT, "client2_data");
stream_poll_add($poll_ctx, $server2, STREAM_POLL_READ | STREAM_POLL_ONESHOT, "server2_data");

pt_expect_events(stream_poll_wait($poll_ctx, 0), []);

pt_write_sleep($client1, "test data");
pt_write_sleep($client2, "test data");
pt_expect_events(stream_poll_wait($poll_ctx, 100), [
    ['events' => STREAM_POLL_READ, 'data' => 'server1_data', 'read' => 'test data'],
    ['events' => STREAM_POLL_READ, 'data' => 'server2_data', 'read' => 'test data']
]);

pt_write_sleep($client1, "more data");
pt_write_sleep($client2, "more data");
pt_expect_events(stream_poll_wait($poll_ctx, 100), []);

?>
--EXPECT--
Events matched - count: 0
Events matched - count: 2
Events matched - count: 0
