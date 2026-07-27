--TEST--
pcntl_signal_dispatch() stops dispatching after handler throws exception
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

posix_kill(posix_getpid(), SIGUSR1);
posix_kill(posix_getpid(), SIGUSR2);

try {
    pcntl_signal_dispatch();
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

echo "Handlers called: " . implode(', ', $called) . "\n";

?>
--EXPECT--
Exception in signal handler
Handlers called: SIGUSR1
