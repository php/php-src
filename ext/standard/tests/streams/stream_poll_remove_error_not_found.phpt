--TEST--
Stream polling - remove not found error
--FILE--
<?php
require_once __DIR__ . '/stream_poll.inc';

list($socket1r, $socket1w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

try {
    stream_poll_remove($poll_ctx, $socket1w);
} catch (StreamPollException $e) {
    echo "ERROR: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
ERROR: Stream not found
