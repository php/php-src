--TEST--
Io\Poll: a fired one-shot watcher stays active and is re-armed by modifyEvents()
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$handle = new StreamPollHandle($r);
$watcher = $ctx->add($handle, [Io\Poll\Event::Read, Io\Poll\Event::OneShot], "data");

fwrite($w, "a");
echo "Events count: ", count($ctx->wait(Time\Duration::fromMicroseconds(100000))), "\n";
var_dump(fread($r, 10));

fwrite($w, "b");
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
var_dump($watcher->isActive());

try {
    $ctx->add($handle, [Io\Poll\Event::Read]);
} catch (Io\Poll\HandleAlreadyWatchedException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$watcher->modifyEvents([Io\Poll\Event::Read, Io\Poll\Event::OneShot]);
$events = $ctx->wait(Time\Duration::fromMicroseconds(100000));
echo "Events count: ", count($events), "\n";
var_dump($events[0] === $watcher);
var_dump(fread($r, 10));

$watcher->remove();
var_dump($watcher->isActive());
?>
--EXPECT--
Events count: 1
string(1) "a"
Events count: 0
bool(true)
Io\Poll\HandleAlreadyWatchedException: Handle already added
Events count: 1
bool(true)
string(1) "b"
bool(false)
