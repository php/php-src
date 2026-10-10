--TEST--
Io\Poll: StreamPollWeakHandle can be re-added to a Context after manual Watcher::remove()
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$handle = StreamPollWeakHandle::create($r);

$watcher1 = $ctx->add($handle, [Io\Poll\Event::Read], 'first');
$watcher1->remove();
var_dump($watcher1->isActive());

$watcher2 = $ctx->add($handle, [Io\Poll\Event::Read], 'second');
var_dump($watcher2->isActive());
var_dump($watcher2->getData());

$watcher2->remove();
fclose($r);
fclose($w);
echo "done\n";
?>
--EXPECT--
bool(false)
bool(true)
string(6) "second"
done
