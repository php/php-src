--TEST--
pcntl_signal_dispatch() delivers the signals a throwing handler left behind once its exception is handled
--EXTENSIONS--
pcntl
posix
--FILE--
<?php

$called = [];

pcntl_signal(SIGUSR1, function ($signo) use (&$called) {
    $called[] = 'SIGUSR1';
    throw new \Exception('Exception in signal handler');
});

pcntl_signal(SIGUSR2, function ($signo) use (&$called) {
    $called[] = 'SIGUSR2';
});

pcntl_signal(SIGHUP, function ($signo) use (&$called) {
    $called[] = 'SIGHUP';
});

// Queued, not dispatched: asynchronous signals are off
posix_kill(posix_getpid(), SIGUSR1);
posix_kill(posix_getpid(), SIGUSR2);

pcntl_async_signals(true);

try {
    // Delivered asynchronously, so the whole queue is dispatched
    posix_kill(posix_getpid(), SIGHUP);
    echo "Not reached\n";
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

// No explicit dispatch: the engine delivers what the throwing handler left behind
// on its own, as soon as the exception has been handled
usleep(1000);

echo "Handlers called: " . implode(', ', $called) . "\n";

?>
--EXPECT--
Exception in signal handler
Handlers called: SIGUSR1, SIGUSR2, SIGHUP
