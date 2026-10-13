--TEST--
Io\Poll: StreamPollWeakHandle auto-removed from multiple Contexts when stream is collected
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$ctx1 = pt_new_stream_poll();
$ctx2 = pt_new_stream_poll();

$handle = StreamPollWeakHandle::create($r);
$watcher1 = $ctx1->add($handle, [Io\Poll\Event::Read], 'ctx1');
$watcher2 = $ctx2->add($handle, [Io\Poll\Event::Read], 'ctx2');

var_dump($watcher1->isActive());
var_dump($watcher2->isActive());

$weakWatcher1 = WeakReference::create($watcher1);
$weakWatcher2 = WeakReference::create($watcher2);
unset($handle, $watcher1, $watcher2);

fclose($r);
fclose($w);
gc_collect_cycles();

// Both watchers removed from their contexts and freed.
var_dump($weakWatcher1->get() === null);
var_dump($weakWatcher2->get() === null);
echo "done\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
done
