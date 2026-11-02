--TEST--
Io\Poll: modifyEvents() switching OneShot on and off takes effect on the kept filter
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

use Io\Poll\Event;

[$r, $w] = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$watcher = pt_stream_poll_add($ctx, $r, [Event::Read]);
fwrite($w, "a");

echo "Level: ", count($ctx->wait(Time\Duration::fromSeconds(0))), " ",
    count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";

// Read stays registered, only the one-shot flag changes
$watcher->modifyEvents([Event::Read, Event::OneShot]);
echo "One-shot: ", count($ctx->wait(Time\Duration::fromSeconds(0))), " ",
    count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
var_dump($watcher->isActive());

// Back to level-triggered, which re-arms the watcher
$watcher->modifyEvents([Event::Read]);
echo "Level: ", count($ctx->wait(Time\Duration::fromSeconds(0))), " ",
    count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";

// Add write to a one-shot read, only one event is reported for the watcher
$watcher->modifyEvents([Event::Read, Event::OneShot]);
$watcher->modifyEvents([Event::Read, Event::Write, Event::OneShot]);
$events = $ctx->wait(Time\Duration::fromSeconds(0));
echo "Read+write one-shot: ", count($events), " ",
    count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";
var_dump($events[0] === $watcher);

$watcher->remove();
?>
--EXPECT--
Level: 1 1
One-shot: 1 0
bool(true)
Level: 1 1
Read+write one-shot: 1 0
bool(true)
