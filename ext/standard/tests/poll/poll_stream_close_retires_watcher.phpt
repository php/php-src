--TEST--
Io\Poll: closing a watched stream retires its watcher (GH-22844)
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

// Closing the underlying stream unregisters the watcher at close time (while
// its fd is still valid), so wait() never surfaces the retired watcher and a
// later remove() reports it as inactive rather than dereferencing freed memory.
$ctx = pt_new_stream_poll();
[$r, $w] = pt_new_socket_pair();

$watcher = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read]);
fwrite($w, "ping");
fclose($r);
fclose($w);

echo "events: "; var_dump(count($ctx->wait(0, 0)));

try {
    $watcher->remove();
    echo "remove: no exception\n";
} catch (\Throwable $e) {
    echo "remove: ", $e::class, "\n";
}

echo "done\n";
?>
--EXPECT--
events: int(0)
remove: Io\Poll\InactiveWatcherException
done
