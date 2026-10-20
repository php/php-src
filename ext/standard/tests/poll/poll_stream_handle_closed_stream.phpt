--TEST--
Io\Poll: handle operations after the stream has been closed
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();
$poll_ctx = pt_new_stream_poll();

$handle = new StreamPollHandle($r);
$watcher = $poll_ctx->add($handle, [Io\Poll\Event::Read]);
$not_added_yet = new StreamPollHandle($w);

fclose($r);
fclose($w);

var_dump($handle->isValid());
var_dump(get_debug_type($handle->getStream()));

try {
    $watcher->modifyEvents([Io\Poll\Event::Write]);
} catch (Io\Poll\InactiveWatcherException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $poll_ctx->add($not_added_yet, [Io\Poll\Event::Read]);
} catch (Io\Poll\InvalidHandleException $e) {
    echo $e->getMessage(), "\n";
}

echo "Events count: ", count($poll_ctx->wait(Time\Duration::fromSeconds(0))), "\n";

$watcher->remove();
var_dump($watcher->isActive());
?>
--EXPECT--
bool(false)
string(17) "resource (closed)"
Cannot modify inactive watcher
Invalid handle for polling
Events count: 0
bool(false)
