--TEST--
Io\Poll\SignalHandle refuses the signal the execution timeout runs on
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die("skip Linux only");
if (!PHP_ZTS && php_uname('m') === 'aarch64') die("skip SIGPROF timer platforms only");
?>
--FILE--
<?php
// POSIX execution timers (ZTS on Linux) use SIGRTMIN, setitimer() uses SIGPROF
$sig = PHP_ZTS ? SIGRTMIN : SIGPROF;
try {
    new Io\Poll\SignalHandle([SIGUSR1, $sig]);
} catch (ValueError $e) {
    echo $e->getMessage() === "Io\\Poll\\SignalHandle::__construct(): Argument #1 (\$signals) must not contain signal " . $sig . ", which the execution timeout uses" ? "refused\n" : $e->getMessage() . "\n";
}

// The timeout still fires with a handle on other signals
$h = new Io\Poll\SignalHandle([SIGUSR1]);
set_time_limit(1);
while (true) {}
?>
--EXPECTF--
refused

Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
