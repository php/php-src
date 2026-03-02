--TEST--
Poll stream - socket write / read close
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $socket1r, [Io\Poll\Event::Read], "socket1_data");
pt_stream_poll_add($poll_ctx, $socket1w, [Io\Poll\Event::Write], "socket2_data");

fwrite($socket1w, "test data");

fclose($socket1r);
pt_expect_events($poll_ctx->wait(0, 100000), [
    [
        'events' => [
            'default' => [Io\Poll\Event::Write, Io\Poll\Event::Error, Io\Poll\Event::HangUp],
            'Kqueue|EventPorts' => [Io\Poll\Event::Write, Io\Poll\Event::HangUp],
        ],
        'data' => 'socket2_data'
    ]
], $poll_ctx);

fclose($socket1w);
pt_expect_events($poll_ctx->wait(0, 100000), []);

?>
--EXPECT--
Events matched - count: 1
Events matched - count: 0
