--TEST--
Stream polling - TCP read write level combined
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($client, $server) = pt_new_tcp_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $client, STREAM_POLL_READ | STREAM_POLL_WRITE, "client_data");
stream_poll_add($poll_ctx, $server, STREAM_POLL_READ | STREAM_POLL_WRITE, "server_data");

pt_expect_events(stream_poll_wait($poll_ctx, 0), [
    ['events' => STREAM_POLL_WRITE, 'data' => 'client_data'],
    ['events' => STREAM_POLL_WRITE, 'data' => 'server_data']
]);

fwrite($client, "test data");
usleep(10000);
pt_expect_events(stream_poll_wait($poll_ctx, 100), [
    ['events' => STREAM_POLL_WRITE, 'data' => 'client_data'],
    ['events' => STREAM_POLL_READ | STREAM_POLL_WRITE, 'data' => 'server_data', 'read' => 'test data']
]);

?>
--EXPECT--
Events matched - count: 2
Events matched - count: 2
