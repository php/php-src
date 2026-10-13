--TEST--
Io\Poll\TimerHandle: timers and descriptors in one context, timers reported first
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$ctx = pt_new_stream_poll();
[$r, $w] = pt_new_socket_pair();

$sw = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read], "stream");
$tw = $ctx->add(new Io\Poll\TimerHandle(Time\Duration::fromMilliseconds(20)), [Io\Poll\Event::Timer], "timer");

// Data first: the stream is reported, the timer keeps waiting
fwrite($w, "x");
$events = $ctx->wait(Time\Duration::fromSeconds(1));
echo "Events count: ", count($events), "\n";
var_dump($events[0]->getData());
fread($r, 10);

// Then the timer fires on its own
$events = $ctx->wait(Time\Duration::fromSeconds(1));
echo "Events count: ", count($events), "\n";
var_dump($events[0]->getData());

// Both due at once: the timer comes first
$tw->modifyEvents([Io\Poll\Event::Timer]);
usleep(30000);
fwrite($w, "y");
$events = $ctx->wait(Time\Duration::fromSeconds(0));
echo "Events count: ", count($events), "\n";
var_dump($events[0]->getData(), $events[1]->getData());

// Destroying the context with an armed timer is fine
$tw->modifyEvents([Io\Poll\Event::Timer]);
unset($ctx);
var_dump($tw->isActive(), $sw->isActive());
echo "done\n";
?>
--EXPECT--
Events count: 1
string(6) "stream"
Events count: 1
string(5) "timer"
Events count: 2
string(5) "timer"
string(6) "stream"
bool(false)
bool(false)
done
