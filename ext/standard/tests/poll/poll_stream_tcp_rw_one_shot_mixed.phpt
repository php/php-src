--TEST--
Poll stream - TCP read write oneshot combined and mixed
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($client, $server) = pt_new_tcp_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $client, POLL_EVENT_READ | POLL_EVENT_WRITE | POLL_EVENT_ONESHOT, "client_data");
pt_stream_poll_add($poll_ctx, $server, POLL_EVENT_READ | POLL_EVENT_WRITE | POLL_EVENT_ONESHOT, "server_data");

pt_write_sleep($client, "test data");
pt_expect_events($poll_ctx->wait(0), [
    ['events' => POLL_EVENT_WRITE, 'data' => 'client_data'],
    ['events' => POLL_EVENT_READ|POLL_EVENT_WRITE, 'data' => 'server_data']
]);

pt_write_sleep($client, "test data");
pt_expect_events($poll_ctx->wait(100), []);

?>
--EXPECT--
Events matched - count: 2
Events matched - count: 0
