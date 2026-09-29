--TEST--
Io\Poll\SignalHandle: a signal pending before the handle is watched is reported at once
--EXTENSIONS--
posix
pcntl
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsSignalHandles()) die("skip no signal handle source on this platform");
?>
--FILE--
<?php
// Blocked and delivered before any handle exists
pcntl_sigprocmask(SIG_BLOCK, [SIGUSR1, SIGUSR2]);
posix_kill(posix_getpid(), SIGUSR1);

$ctx = new Io\Poll\Context();
$handle = new Io\Poll\SignalHandle([SIGUSR1, SIGUSR2]);
$watcher = $ctx->add($handle, [Io\Poll\Event::Signal]);
// FreeBSD announces a nested kqueue's readiness from a kernel task, so a zero timeout may miss
// what add() triggered; a bounded wait returns as soon as it is reported
$events = $ctx->wait(Time\Duration::fromSeconds(1));
var_dump(count($events), $handle->getDelivered() === [SIGUSR1]);

// Consumed: nothing pending, nothing reported
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));

// Two signals of the set pending at once are both taken
posix_kill(posix_getpid(), SIGUSR1);
posix_kill(posix_getpid(), SIGUSR2);
$events = $ctx->wait(Time\Duration::fromSeconds(2));
$delivered = $handle->getDelivered();
sort($delivered);
var_dump(count($events), $delivered === [SIGUSR1, SIGUSR2]);
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));

// A pending signal outside the set is left alone
pcntl_sigprocmask(SIG_BLOCK, [SIGTERM]);
posix_kill(posix_getpid(), SIGTERM);
posix_kill(posix_getpid(), SIGUSR2);
$events = $ctx->wait(Time\Duration::fromSeconds(2));
var_dump(count($events), $handle->getDelivered() === [SIGUSR2]);
pcntl_sigprocmask(SIG_BLOCK, [SIGTERM], $blocked);
var_dump(in_array(SIGTERM, $blocked));
pcntl_signal(SIGTERM, function () { echo "SIGTERM handled\n"; });
pcntl_sigprocmask(SIG_UNBLOCK, [SIGTERM]);
pcntl_signal_dispatch();

$watcher->remove();
unset($handle, $watcher, $events);
// Blocked before the handle, so still blocked after it
pcntl_sigprocmask(SIG_BLOCK, [SIGTERM], $blocked);
var_dump(in_array(SIGUSR1, $blocked), in_array(SIGUSR2, $blocked));
?>
--EXPECT--
int(1)
bool(true)
int(0)
int(1)
bool(true)
int(0)
int(1)
bool(true)
bool(true)
SIGTERM handled
bool(true)
bool(true)
