--TEST--
Poll stream - TCP read write level combined
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($client, $server) = pt_new_tcp_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $client, [Io\Poll\Event::Read, Io\Poll\Event::Write], "client_data");
pt_stream_poll_add($poll_ctx, $server, [Io\Poll\Event::Read, Io\Poll\Event::Write], "server_data");

pt_expect_events($poll_ctx->wait(0), [
    ['events' => [Io\Poll\Event::Write], 'data' => 'client_data'],
    ['events' => [Io\Poll\Event::Write], 'data' => 'server_data']
]);

fwrite($client, "test data");
usleep(10000);
pt_expect_events($poll_ctx->wait(0, 100000), [
    ['events' => [Io\Poll\Event::Write], 'data' => 'client_data'],
    ['events' => [Io\Poll\Event::Read, Io\Poll\Event::Write], 'data' => 'server_data', 'read' => 'test data']
]);

?>
--EXPECT--
Events matched - count: 2
Events matched - count: 2
