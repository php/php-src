--TEST--
Stream polling - add duplicite error
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

stream_poll_add($poll_ctx, $socket1w, STREAM_POLL_WRITE, "socket2_data");

try {
    stream_poll_add($poll_ctx, $socket1w, STREAM_POLL_WRITE, "socket2_data");
} catch (StreamPollException $e) {
    echo "ERROR: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
ERROR: Stream already added
