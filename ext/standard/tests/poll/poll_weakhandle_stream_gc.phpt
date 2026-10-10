--TEST--
Io\Poll: StreamPollWeakHandle watcher auto-removed and GC'd when stream is collected
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$handle = StreamPollWeakHandle::create($r);
$watcher = $ctx->add($handle, [Io\Poll\Event::Read]);

$weakHandle  = WeakReference::create($handle);
$weakWatcher = WeakReference::create($watcher);

// Refs to handle and watcher; context still holds a ref to
// watcher (and watcher holds a ref to handle), but only until the stream dies.
unset($handle, $watcher);

// Stream is still alive: watcher should still be in the context.
var_dump($weakHandle->get() !== null);   // handle alive (held by watcher in context)
var_dump($weakWatcher->get() !== null);  // watcher alive (held by context)

// Freeing the stream notifies handle and context
fclose($r);
fclose($w);

// Context no longer holds the watcher ref; watcher held no external refs either.
gc_collect_cycles();

var_dump($weakHandle->get() === null);   // handle freed
var_dump($weakWatcher->get() === null);  // watcher freed
echo "done\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
done
