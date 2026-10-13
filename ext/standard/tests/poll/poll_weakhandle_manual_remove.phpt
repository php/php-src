--TEST--
Io\Poll: StreamPollWeakHandle watcher manual remove from single Context
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$handle = StreamPollWeakHandle::create($r);
$watcher = $ctx->add($handle, [Io\Poll\Event::Read], 'my-data');

var_dump($watcher->isActive());
var_dump($watcher->getData());

$watcher->remove();

var_dump($watcher->isActive());

try {
    $watcher->remove();
} catch (Io\Poll\InactiveWatcherException $e) {
    echo $e->getMessage(), "\n";
}

fclose($r);
fclose($w);
echo "done\n";
?>
--EXPECT--
bool(true)
string(7) "my-data"
bool(false)
Cannot remove inactive watcher
done
