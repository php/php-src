--TEST--
Poll stream - add duplicite error
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

pt_stream_poll_add($poll_ctx, $socket1w, POLL_EVENT_WRITE, "socket2_data");

try {
    pt_stream_poll_add($poll_ctx, $socket1w, POLL_EVENT_WRITE, "socket2_data");
} catch (PollException $e) {
    echo "ERROR: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
ERROR: Handle already added
