--TEST--
Io\Poll\SignalHandle: the signals stay blocked until the last context stops watching the handle
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsSignalHandles()) die("skip no signal handle source on this platform");
?>
--FILE--
<?php
use Io\Poll\{Context, Event, SignalHandle};

function usr1_blocked(): bool {
    pcntl_sigprocmask(SIG_UNBLOCK, [SIGWINCH], $blocked);
    return in_array(SIGUSR1, $blocked);
}

pcntl_async_signals(true);
pcntl_signal(SIGUSR1, function () { echo "handler\n"; });

// In no context the handle does nothing: the handler runs
$h = new SignalHandle([SIGUSR1]);
var_dump(usr1_blocked());
posix_kill(posix_getpid(), SIGUSR1);
var_dump($h->getDelivered());

// Two contexts: blocked until both removed
$a = new Context();
$b = new Context();
$wa = $a->add($h, [Event::Signal]);
$wb = $b->add($h, [Event::Signal]);
var_dump(usr1_blocked());
$wa->remove();
var_dump(usr1_blocked());
posix_kill(posix_getpid(), SIGUSR1);
var_dump(count($b->wait(Time\Duration::fromSeconds(2))), $h->getDelivered() === [SIGUSR1]);

// A context destroyed with the handle in it counts as a removal
unset($wb, $b);
var_dump(usr1_blocked());

// Two handles for the signal: blocked until the last watched one is removed
$h2 = new SignalHandle([SIGUSR1]);
$c = new Context();
$w1 = $c->add($h, [Event::Signal]);
$w2 = $c->add($h2, [Event::Signal]);
$w1->remove();
var_dump(usr1_blocked());
$w2->remove();
var_dump(usr1_blocked());

// A cycle through the watcher data, collected with the handle still watched
$d = new Context();
$d->add($h, [Event::Signal], $d);
var_dump(usr1_blocked());
unset($d);
gc_collect_cycles();
var_dump(usr1_blocked());
?>
--EXPECT--
bool(false)
handler
array(0) {
}
bool(true)
bool(true)
int(1)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
