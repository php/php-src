--TEST--
Io\Poll: Watcher operations are safe after its Context is destroyed
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$watcher = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read], "data");

// Drop the Context while still holding the Watcher it returned.
unset($ctx);
gc_collect_cycles();

var_dump($watcher->isActive());

try {
    $watcher->remove();
} catch (Io\Poll\InactiveWatcherException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $watcher->modifyEvents([Io\Poll\Event::Write]);
} catch (Io\Poll\InactiveWatcherException $e) {
    echo $e->getMessage(), "\n";
}

echo "done\n";
?>
--EXPECT--
bool(false)
Cannot remove inactive watcher
Cannot modify inactive watcher
done
