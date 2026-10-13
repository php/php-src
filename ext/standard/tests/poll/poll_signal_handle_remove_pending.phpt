--TEST--
Io\Poll\SignalHandle: deliveries taken while watched are kept across the last removal, nothing is discarded
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!Io\Poll\Backend::Auto->supportsSignalHandles()) die("skip no signal handle source on this platform");
?>
--FILE--
<?php
pcntl_async_signals(true);
pcntl_signal(SIGUSR2, function ($signo) { echo "handler ", $signo === SIGUSR2 ? "SIGUSR2" : $signo, "\n"; });

$ctx = new Io\Poll\Context();
$h = new Io\Poll\SignalHandle([SIGUSR1, SIGUSR2, SIGTERM]);
$watcher = $ctx->add($h, [Io\Poll\Event::Signal]);
posix_kill(posix_getpid(), SIGUSR1);
posix_kill(posix_getpid(), SIGUSR2);
posix_kill(posix_getpid(), SIGTERM);
echo "pending\n";
$watcher->remove();
pcntl_signal_dispatch();
echo "alive\n";

// Taken into the record before the unblock, none of them ran its handler or default action
$delivered = $h->getDelivered();
sort($delivered);
$expected = [SIGUSR1, SIGUSR2, SIGTERM];
sort($expected);
var_dump($delivered === $expected);

// Unblocked again: a later delivery has its normal disposition
posix_kill(posix_getpid(), SIGUSR2);
pcntl_signal_dispatch();
var_dump($h->getDelivered());
var_dump(pcntl_sigprocmask(SIG_UNBLOCK, [SIGCHLD], $old), in_array(SIGUSR1, $old), in_array(SIGTERM, $old));
?>
--EXPECT--
pending
alive
bool(true)
handler SIGUSR2
array(0) {
}
bool(true)
bool(false)
bool(false)
