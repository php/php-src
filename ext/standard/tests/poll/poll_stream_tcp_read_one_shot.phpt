--TEST--
Poll stream - TCP read write oneshot
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($client1, $server1) = pt_new_tcp_socket_pair();
list($client2, $server2) = pt_new_tcp_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $client1, [Io\Poll\Event::Read, Io\Poll\Event::OneShot], "client1_data");
pt_stream_poll_add($poll_ctx, $server1, [Io\Poll\Event::Read, Io\Poll\Event::OneShot], "server1_data");
pt_stream_poll_add($poll_ctx, $client2, [Io\Poll\Event::Read, Io\Poll\Event::OneShot], "client2_data");
pt_stream_poll_add($poll_ctx, $server2, [Io\Poll\Event::Read, Io\Poll\Event::OneShot], "server2_data");

pt_expect_events($poll_ctx->wait(0), []);

pt_write_sleep($client1, "test data");
pt_write_sleep($client2, "test data");
pt_expect_events($poll_ctx->wait(0, 100000), [
    ['events' => [Io\Poll\Event::Read], 'data' => 'server1_data', 'read' => 'test data'],
    ['events' => [Io\Poll\Event::Read], 'data' => 'server2_data', 'read' => 'test data']
]);

pt_write_sleep($client1, "more data");
pt_write_sleep($client2, "more data");
pt_expect_events($poll_ctx->wait(0, 100000), []);

?>
--EXPECT--
Events matched - count: 0
Events matched - count: 2
Events matched - count: 0
