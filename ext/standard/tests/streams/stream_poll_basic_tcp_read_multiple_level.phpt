--TEST--
Stream polling - TCP read write level
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($clients, $servers) = pt_new_tcp_socket_connections(20);
$poll_ctx = pt_new_stream_poll();

for ($i = 0; $i < count($servers); $i++) {
    stream_poll_add($poll_ctx, $servers[$i], STREAM_POLL_READ, "server{$i}_data");
}

pt_expect_events(stream_poll_wait($poll_ctx, 0), []);

for ($i = 0; $i < count($clients); $i++) {
    pt_write_sleep($clients[$i], "test $i data");
}

// Build expected events for all 20 connections
$expected_events = [];
for ($i = 0; $i < 20; $i++) {
    $expected_events[] = ['events' => STREAM_POLL_READ, 'data' => "server{$i}_data", 'read' => "test $i data"];
}
pt_expect_events(stream_poll_wait($poll_ctx, 100), $expected_events);

pt_write_sleep($clients[1], "more data");
pt_write_sleep($clients[2], "more data");
pt_expect_events(stream_poll_wait($poll_ctx, 100), [
    ['events' => STREAM_POLL_READ, 'data' => 'server1_data', 'read' => 'more data'],
    ['events' => STREAM_POLL_READ, 'data' => 'server2_data', 'read' => 'more data']
]);

pt_expect_events(stream_poll_wait($poll_ctx, 100), []);

?>
--EXPECT--
Events matched - count: 0
Events matched - count: 20
Events matched - count: 2
Events matched - count: 0
