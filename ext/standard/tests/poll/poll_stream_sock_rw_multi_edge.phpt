--TEST--
Poll stream - socket write / read multiple times with edge triggering
--SKIPIF--
<?php
require_once __DIR__ . '/poll.inc';
pt_skip_for_backend(['poll', 'wsapoll'], 'does not support edge triggering')
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $socket1r, POLL_EVENT_READ | POLL_EVENT_ET, "socket1_data");
pt_stream_poll_add($poll_ctx, $socket1w, POLL_EVENT_WRITE | POLL_EVENT_ET, "socket2_data");

pt_expect_events($poll_ctx->wait(0), [
    ['events' => POLL_EVENT_WRITE, 'data' => 'socket2_data']
]);

pt_expect_events($poll_ctx->wait(0), []);

fwrite($socket1w, "test data");
pt_expect_events($poll_ctx->wait(100), [
    ['events' => POLL_EVENT_READ, 'data' => 'socket1_data', 'read' => 'test data']
]);

fwrite($socket1w, "more data");
pt_expect_events($poll_ctx->wait(100), [
    ['events' => POLL_EVENT_WRITE, 'data' => 'socket2_data'],
    ['events' => POLL_EVENT_READ, 'data' => 'socket1_data']
]);

pt_expect_events($poll_ctx->wait(100), []);

fwrite($socket1w, " and even more data");
pt_expect_events($poll_ctx->wait(100), [
    ['events' => POLL_EVENT_READ, 'data' => 'socket1_data', 'read' => 'more data and even more data']
]);

fclose($socket1r);
pt_expect_events($poll_ctx->wait(100), [
    [
        'events' => ['default' => POLL_EVENT_WRITE|POLL_EVENT_HUP, 'poll' => POLL_EVENT_HUP],
        'data' => 'socket2_data'
    ]
], $poll_ctx);

fclose($socket1w);
pt_expect_events($poll_ctx->wait(100), []);

?>
--EXPECT--
Events matched - count: 1
Events matched - count: 0
Events matched - count: 1
Events matched - count: 2
Events matched - count: 0
Events matched - count: 1
Events matched - count: 1
Events matched - count: 0
