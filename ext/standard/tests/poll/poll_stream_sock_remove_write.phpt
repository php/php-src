--TEST--
Poll stream - socket remove write
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
list($socket2r, $socket2w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

$watcher1w = pt_stream_poll_add($poll_ctx, $socket1w, [Io\Poll\Event::Write], "socket_data_1");
pt_stream_poll_add($poll_ctx, $socket2w, [Io\Poll\Event::Write], "socket_data_2");

pt_expect_events($poll_ctx->wait(0), [
    ['events' => [Io\Poll\Event::Write], 'data' => 'socket_data_1'],
    ['events' => [Io\Poll\Event::Write], 'data' => 'socket_data_2']
]);

$watcher1w->remove();

pt_expect_events($poll_ctx->wait(0), [
    ['events' => [Io\Poll\Event::Write], 'data' => 'socket_data_2']
]);

// check that both streams are still usable
var_dump(fwrite($socket1w, "test 1"));
var_dump(fwrite($socket2w, "test 2"));
var_dump(fread($socket1r, 100));
var_dump(fread($socket2r, 100));

?>
--EXPECT--
Events matched - count: 2
Events matched - count: 1
int(6)
int(6)
string(6) "test 1"
string(6) "test 2"
