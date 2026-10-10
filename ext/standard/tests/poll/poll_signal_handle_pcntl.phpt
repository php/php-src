--TEST--
Io\Poll\SignalHandle: pcntl_sigprocmask() and pcntl_signal() leave a watched signal blocked
--EXTENSIONS--
posix
pcntl
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsSignalHandles()) die("skip no signal handle source on this platform");
?>
--FILE--
<?php
$ctx = new Io\Poll\Context();
$handle = new Io\Poll\SignalHandle([SIGUSR1]);
$watcher = $ctx->add($handle, [Io\Poll\Event::Signal]);

// Neither an unblock nor a new mask takes the watched signal out of the mask
pcntl_sigprocmask(SIG_UNBLOCK, [SIGUSR1]);
pcntl_sigprocmask(SIG_SETMASK, [], $blocked);
var_dump(in_array(SIGUSR1, $blocked));
pcntl_sigprocmask(SIG_BLOCK, [SIGUSR2], $blocked);
var_dump(in_array(SIGUSR1, $blocked));

// A pcntl handler installed meanwhile does not run: the handle takes the signal
pcntl_signal(SIGUSR1, function () { echo "pcntl handler ran\n"; });
posix_kill(posix_getpid(), SIGUSR1);
pcntl_signal_dispatch();
$events = $ctx->wait(Time\Duration::fromSeconds(2));
var_dump(count($events), $handle->getDelivered() === [SIGUSR1]);

// The last removal unblocks it and the handler takes over
$watcher->remove();
posix_kill(posix_getpid(), SIGUSR1);
pcntl_signal_dispatch();
echo "done\n";
?>
--EXPECT--
bool(true)
bool(true)
int(1)
bool(true)
pcntl handler ran
done
