--TEST--
Stream polling - socket write / read close
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket1r, STREAM_POLL_READ, "socket1_data");
stream_poll_add($poll_ctx, $socket1w, STREAM_POLL_WRITE, "socket2_data");

fwrite($socket1w, "test data");

fclose($socket1r);
pt_expect_events(stream_poll_wait($poll_ctx, 100), [
    [
        'events' => ['default' => STREAM_POLL_WRITE|STREAM_POLL_HUP, 'poll' => STREAM_POLL_HUP],
        'data' => 'socket2_data'
    ]
], $poll_ctx);

fclose($socket1w);
pt_expect_events(stream_poll_wait($poll_ctx, 100), []);

?>
--EXPECT--
Events matched - count: 1
Events matched - count: 0
