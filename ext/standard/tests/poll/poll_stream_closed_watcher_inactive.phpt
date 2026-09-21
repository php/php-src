--TEST--
Io\Poll: closing a watched stream deactivates its watcher and remove() stays harmless
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$watcher = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read]);

fwrite($w, "ping");
var_dump($watcher->isActive());

fclose($r);
var_dump($watcher->isActive());

echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";

$watcher->remove();
$watcher->remove();
echo "removed\n";
var_dump($watcher->isActive());

try {
    $watcher->modifyEvents([Io\Poll\Event::Write]);
} catch (Io\Poll\InactiveWatcherException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

fclose($w);
?>
--EXPECT--
bool(true)
bool(false)
Events count: 0
removed
bool(false)
Io\Poll\InactiveWatcherException: Cannot modify inactive watcher
