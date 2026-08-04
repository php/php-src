--TEST--
Io\Poll: Context, Watcher and StreamPollHandle are not cloneable
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$handle = new StreamPollHandle($r);
$watcher = $ctx->add($handle, [Io\Poll\Event::Read]);

foreach ([$ctx, $handle, $watcher] as $obj) {
    try {
        clone $obj;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "done\n";
?>
--EXPECT--
Trying to clone an uncloneable object of class Io\Poll\Context
Trying to clone an uncloneable object of class StreamPollHandle
Trying to clone an uncloneable object of class Io\Poll\Watcher
done
