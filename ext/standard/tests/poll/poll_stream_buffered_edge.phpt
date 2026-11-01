--TEST--
Io\Poll: buffered data does not fire an edge-triggered watcher
--SKIPIF--
<?php
require_once __DIR__ . '/poll.inc';
pt_skip_for_backend(['Poll', 'WSAPoll', 'EventPorts'], 'does not support edge triggering')
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$a, $b] = pt_new_socket_pair();
fwrite($b, "first\nsecond\n");
var_dump(fgets($a));

$ctx = pt_new_stream_poll();
$w = pt_stream_poll_add($ctx, $a, [Io\Poll\Event::Read, Io\Poll\Event::EdgeTriggered], 'a');

// Buffered data is state, an edge watcher reports transitions only
var_dump(count($ctx->wait(Time\Duration::fromMilliseconds(100))));

// A new edge on the descriptor is reported as usual
fwrite($b, "third\n");
$events = $ctx->wait(Time\Duration::fromMilliseconds(100));
var_dump(count($events), $events[0] === $w);
?>
--EXPECT--
string(6) "first
"
int(0)
int(1)
bool(true)
