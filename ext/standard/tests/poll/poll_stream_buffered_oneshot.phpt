--TEST--
Io\Poll: a one-shot watcher reported from buffered data is disarmed until re-armed
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$a, $b] = pt_new_socket_pair();
fwrite($b, "first\nsecond\n");
var_dump(fgets($a));

$ctx = pt_new_stream_poll();
$w = pt_stream_poll_add($ctx, $a, [Io\Poll\Event::Read, Io\Poll\Event::OneShot], 'a');

// Fires once from the buffer
$events = $ctx->wait(Time\Duration::fromSeconds(0));
var_dump(count($events), $events[0] === $w);

// Disarmed: neither the buffer nor new data on the socket reports it
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));
fwrite($b, "third\n");
var_dump(count($ctx->wait(Time\Duration::fromMilliseconds(50))));
var_dump($w->isActive());

// Re-armed: the buffer reports it at once, again once
$w->modifyEvents([Io\Poll\Event::Read, Io\Poll\Event::OneShot]);
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));

// Drained, re-armed: the descriptor reports it
var_dump(fgets($a), fgets($a));
$w->modifyEvents([Io\Poll\Event::Read, Io\Poll\Event::OneShot]);
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));
fwrite($b, "fourth\n");
$events = $ctx->wait(Time\Duration::fromMilliseconds(100));
var_dump(count($events), $events[0]->hasTriggered(Io\Poll\Event::Read));

$w->remove();
var_dump($w->isActive());
?>
--EXPECT--
string(6) "first
"
int(1)
bool(true)
int(0)
int(0)
bool(true)
int(1)
int(0)
string(7) "second
"
string(6) "third
"
int(0)
int(1)
bool(true)
bool(false)
