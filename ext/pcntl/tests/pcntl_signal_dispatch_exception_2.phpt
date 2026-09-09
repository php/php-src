--TEST--
pcntl_signal_dispatch() keeps the signals left in the queue by a throwing handler
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

posix_kill(posix_getpid(), SIGUSR1);
posix_kill(posix_getpid(), SIGUSR2);
posix_kill(posix_getpid(), SIGHUP);

try {
    pcntl_signal_dispatch();
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

echo "Handlers called: " . implode(', ', $called) . "\n";

pcntl_signal_dispatch();

echo "Handlers called: " . implode(', ', $called) . "\n";

?>
--EXPECT--
Exception in signal handler
Handlers called: SIGUSR1
Handlers called: SIGUSR1, SIGUSR2, SIGHUP
