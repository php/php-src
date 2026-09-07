--TEST--
Io\Poll: calling __construct() twice throws instead of leaking
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();

$handle = new StreamPollHandle($r);
try {
    $handle->__construct($r);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$ctx = pt_new_stream_poll();
try {
    $ctx->__construct();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "done\n";
?>
--EXPECT--
StreamPollHandle object is already constructed
Io\Poll\Context object is already constructed
done
