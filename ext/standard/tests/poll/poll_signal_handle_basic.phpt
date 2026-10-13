--TEST--
Io\Poll\SignalHandle: signals as events, blocked while a context watches the handle
--EXTENSIONS--
posix
pcntl
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsSignalHandles()) die("skip no signal handle source on this platform");
?>
--FILE--
<?php
try {
    new Io\Poll\SignalHandle([]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    new Io\Poll\SignalHandle([0]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$ctx = new Io\Poll\Context();
$handle = new Io\Poll\SignalHandle([SIGUSR1, SIGUSR2]);
var_dump($handle->getSignals() === [SIGUSR1, SIGUSR2]);

// Not watched yet: nothing blocked
pcntl_sigprocmask(SIG_UNBLOCK, [SIGWINCH], $blocked);
var_dump(in_array(SIGUSR1, $blocked), in_array(SIGUSR2, $blocked));

try {
    $ctx->add($handle, [Io\Poll\Event::Notify]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$watcher = $ctx->add($handle, [Io\Poll\Event::Signal]);
// Watched: the handle blocked its signals
pcntl_sigprocmask(SIG_UNBLOCK, [SIGWINCH], $blocked);
var_dump(in_array(SIGUSR1, $blocked), in_array(SIGUSR2, $blocked));
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));

posix_kill(posix_getpid(), SIGUSR2);
$events = $ctx->wait(Time\Duration::fromSeconds(2));
var_dump(count($events), $events[0]->getTriggeredEvents());
var_dump($handle->getDelivered() === [SIGUSR2], $handle->getDelivered());

// Level: nothing pending, nothing reported
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));

// Both at once
posix_kill(posix_getpid(), SIGUSR1);
posix_kill(posix_getpid(), SIGUSR2);
$events = $ctx->wait(Time\Duration::fromSeconds(2));
var_dump(count($events), count($handle->getDelivered()));

$watcher->remove();

// Removed: the signals are unblocked again while the handle lives on
pcntl_sigprocmask(SIG_UNBLOCK, [SIGWINCH], $blocked);
var_dump(in_array(SIGUSR1, $blocked), in_array(SIGUSR2, $blocked));
?>
--EXPECTF--
Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) must not be empty
Io\Poll\SignalHandle::__construct(): Argument #1 ($signals) signals must be between 1 and %d
bool(true)
bool(false)
bool(false)
Io\Poll\Context::add(): Argument #2 ($events) must be Event::Signal for a SignalHandle
bool(true)
bool(true)
int(0)
int(1)
array(1) {
  [0]=>
  enum(Io\Poll\Event::Signal)
}
bool(true)
array(0) {
}
int(0)
int(1)
int(2)
bool(false)
bool(false)
