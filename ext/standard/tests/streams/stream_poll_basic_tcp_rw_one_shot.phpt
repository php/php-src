--TEST--
Stream polling - TCP read write oneshot combined
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($client, $server) = pt_new_tcp_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $client, STREAM_POLL_READ | STREAM_POLL_WRITE | STREAM_POLL_ONESHOT, "client_data");
stream_poll_add($poll_ctx, $server, STREAM_POLL_READ | STREAM_POLL_WRITE | STREAM_POLL_ONESHOT, "server_data");

pt_expect_events(stream_poll_wait($poll_ctx, 0), [
    ['events' => STREAM_POLL_WRITE, 'data' => 'client_data'],
    ['events' => STREAM_POLL_WRITE, 'data' => 'server_data']
]);

pt_write_sleep($client, "test data");
pt_expect_events(stream_poll_wait($poll_ctx, 100), []);

pt_write_sleep($client, "test data");
pt_expect_events(stream_poll_wait($poll_ctx, 100), []);

?>
--EXPECT--
Events matched - count: 2
Events matched - count: 0
Events matched - count: 0
