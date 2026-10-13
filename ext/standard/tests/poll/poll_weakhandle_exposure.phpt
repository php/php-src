--TEST--
Io\Poll: StreamPollWeakHandle is exposed through create(), one per stream, invalid once it is closed
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$handle = StreamPollWeakHandle::create($r);
var_dump($handle->isExposed(), $handle->isValid(), $handle === StreamPollWeakHandle::create($r));
var_dump($handle->getStream() === $r);

$ctx = pt_new_stream_poll();
$watcher = $ctx->add($handle, [Io\Poll\Event::Read]);
var_dump($watcher->getHandle() === $handle);

fclose($r);
var_dump($handle->isValid(), $handle->getStream(), $handle->isExposed(), $watcher->isActive());
fclose($w);
echo "done\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
NULL
bool(true)
bool(false)
done
