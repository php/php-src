--TEST--
Stream polling - socket write / read multiple times with level triggering
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket1r, STREAM_POLL_READ, "socket1_data");
stream_poll_add($poll_ctx, $socket1w, STREAM_POLL_WRITE, "socket2_data");

pt_expect_events(stream_poll_wait($poll_ctx, 0), [
    ['events' => STREAM_POLL_WRITE, 'data' => 'socket2_data']
]);

pt_expect_events(stream_poll_wait($poll_ctx, 0), [
    ['events' => STREAM_POLL_WRITE, 'data' => 'socket2_data']
]);

fwrite($socket1w, "test data");
pt_expect_events(stream_poll_wait($poll_ctx, 100), [
    ['events' => STREAM_POLL_WRITE, 'data' => 'socket2_data'],
    ['events' => STREAM_POLL_READ, 'data' => 'socket1_data', 'read' => 'test data']
]);

fwrite($socket1w, "more data");
pt_expect_events(stream_poll_wait($poll_ctx, 100), [
    ['events' => STREAM_POLL_WRITE, 'data' => 'socket2_data'],
    ['events' => STREAM_POLL_READ, 'data' => 'socket1_data']
]);

pt_expect_events(stream_poll_wait($poll_ctx, 100), [
    ['events' => STREAM_POLL_WRITE, 'data' => 'socket2_data'],
    ['events' => STREAM_POLL_READ, 'data' => 'socket1_data']
]);

fwrite($socket1w, " and even more data");
pt_expect_events(stream_poll_wait($poll_ctx, 100), [
    ['events' => STREAM_POLL_WRITE, 'data' => 'socket2_data'],
    ['events' => STREAM_POLL_READ, 'data' => 'socket1_data', 'read' => 'more data and even more data']
]);

fclose($socket1r);
pt_expect_events(stream_poll_wait($poll_ctx, 100), [
    ['events' => STREAM_POLL_WRITE|STREAM_POLL_HUP, 'data' => 'socket2_data']
]);

fclose($socket1w);
pt_expect_events(stream_poll_wait($poll_ctx, 100), []);

?>
--EXPECT--
Events matched - count: 1
Events matched - count: 1
Events matched - count: 2
Events matched - count: 2
Events matched - count: 2
Events matched - count: 2
Events matched - count: 1
Events matched - count: 0
