--TEST--
Io\Poll: a level watcher reports Read while the stream layer holds buffered data
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$a, $b] = pt_new_socket_pair();
fwrite($b, "first\nsecond\n");
var_dump(fgets($a)); // "second\n" is now in $a's read buffer, the socket is empty

$ctx = pt_new_stream_poll();
$w = pt_stream_poll_add($ctx, $a, [Io\Poll\Event::Read], 'a');

// Returned at once although the descriptor is idle
$start = hrtime(true);
$events = $ctx->wait(Time\Duration::fromSeconds(5));
var_dump(count($events), $events[0] === $w, $events[0]->hasTriggered(Io\Poll\Event::Read));
var_dump(hrtime(true) - $start < 2_000_000_000);
var_dump(stream_get_meta_data($a)['unread_bytes']);

// Level: reported on every wait while the data stays buffered
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));

var_dump(fgets($a));
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));

// Write interest is not concerned with read data
fwrite($b, "third\nfourth\n");
var_dump(fgets($a));
$w->modifyEvents([Io\Poll\Event::Write]);
$events = $ctx->wait(Time\Duration::fromSeconds(0));
var_dump(count($events), $events[0]->hasTriggered(Io\Poll\Event::Write), $events[0]->hasTriggered(Io\Poll\Event::Read));

// The buffer counts as soon as Read interest is back
$w->modifyEvents([Io\Poll\Event::Read]);
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));
?>
--EXPECT--
string(6) "first
"
int(1)
bool(true)
bool(true)
bool(true)
int(7)
int(1)
string(7) "second
"
int(0)
string(6) "third
"
int(1)
bool(true)
bool(false)
int(1)
