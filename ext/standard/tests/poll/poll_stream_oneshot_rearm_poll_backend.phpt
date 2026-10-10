--TEST--
Io\Poll: a fired one-shot watcher stays registered and disarmed on the poll() backend
--SKIPIF--
<?php
if (!Io\Poll\Backend::Poll->isAvailable()) die("skip poll backend required");
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();
$ctx = new Io\Poll\Context(Io\Poll\Backend::Poll);
$handle = new StreamPollHandle($r);
$watcher = $ctx->add($handle, [Io\Poll\Event::Read, Io\Poll\Event::OneShot], "data");

fwrite($w, "a");
echo "Events count: ", count($ctx->wait(Time\Duration::fromMicroseconds(100000))), "\n";
var_dump(fread($r, 10));

// Disarmed: not reported although readable, and not even polled
fwrite($w, "b");
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
var_dump($watcher->isActive());

// Still registered: adding the handle again is refused
try {
    $ctx->add($handle, [Io\Poll\Event::Read]);
} catch (Io\Poll\HandleAlreadyWatchedException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Re-armed by modify, reports the pending byte at once
$watcher->modifyEvents([Io\Poll\Event::Read, Io\Poll\Event::OneShot]);
$events = $ctx->wait(Time\Duration::fromSeconds(0));
echo "Events count: ", count($events), "\n";
var_dump($events[0] === $watcher);
var_dump(fread($r, 10));

// A disarmed watcher does not report the peer closing either
fclose($w);
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
$watcher->modifyEvents([Io\Poll\Event::Read, Io\Poll\Event::OneShot]);
echo "Events count: ", count($ctx->wait(Time\Duration::fromMicroseconds(100000))), "\n";

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
Events count: 0
Events count: 1
bool(false)
