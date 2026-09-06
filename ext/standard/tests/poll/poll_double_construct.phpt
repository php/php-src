--TEST--
Io\Poll: calling __construct() twice throws instead of leaking
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();

$handle = new StreamPollHandle($r);
try {
    $handle->__construct($r);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$ctx = pt_new_stream_poll();
try {
    $ctx->__construct();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "done\n";
?>
--EXPECT--
Error: StreamPollHandle object is already constructed
Error: Io\Poll\Context object is already constructed
done
