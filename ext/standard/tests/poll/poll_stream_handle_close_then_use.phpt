--TEST--
Io\Poll: StreamPollHandle operations are safe when the stream is closed first
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$handle = new StreamPollHandle($r);
$watcher = $ctx->add($handle, [Io\Poll\Event::Read]);

fclose($r);

var_dump($handle->isValid());
var_dump(is_resource($handle->getStream()));

try {
    $watcher->modifyEvents([Io\Poll\Event::Read, Io\Poll\Event::Write]);
} catch (Io\Poll\InvalidHandleException $e) {
    echo $e->getMessage(), "\n";
}

echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";

$watcher->remove();
var_dump($watcher->isActive());

fclose($w);
echo "ok\n";
?>
--EXPECT--
bool(false)
bool(false)
Invalid handle for polling
Events count: 0
bool(false)
ok
