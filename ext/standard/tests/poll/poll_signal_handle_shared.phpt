--TEST--
Io\Poll\SignalHandle: of two handles for one signal only the one that took the delivery reports it
--EXTENSIONS--
posix
pcntl
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsSignalHandles()) die("skip no signal handle source on this platform");
?>
--FILE--
<?php
use Io\Poll\{Context, Event, SignalHandle};
$a = new SignalHandle([SIGUSR1]);
$b = new SignalHandle([SIGUSR1]);
$c = new Context;
$c->add($a, [Event::Signal], 'a');
$c->add($b, [Event::Signal], 'b');
posix_kill(posix_getpid(), SIGUSR1);
$events = $c->wait(Time\Duration::fromSeconds(2));
var_dump(count($events));
var_dump($events[0]->getHandle()->getDelivered() === [SIGUSR1]);
var_dump(count($c->wait(Time\Duration::fromSeconds(0))));
?>
--EXPECT--
int(1)
bool(true)
int(0)
