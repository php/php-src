--TEST--
Prevent switching fibers when async signals are enabled
--EXTENSIONS--
pcntl
posix
--FILE--
<?php

pcntl_async_signals(true);

pcntl_signal(SIGUSR1, function (): void {
    if (Fiber::getCurrent() !== null) {
        Fiber::suspend();
    }
});

$fiber = new Fiber(function (): void {
    echo "Fiber start\n";
    posix_kill(posix_getpid(), SIGUSR1);
    time_nanosleep(1);
    echo "Fiber end\n";
});

$fiber->start();

?>
--EXPECTF--
Fiber start

Fatal error: Uncaught FiberError: Cannot switch fibers in current execution context in %ssignal-async.php:%d
Stack trace:
#0 %ssignal-async.php(%d): Fiber::suspend()
#1 %ssignal-async.php(%d): {closure}(%d, Array)
#2 [internal function]: {closure}()
#3 %ssignal-async.php(%d): Fiber->start()
#4 {main}
  thrown in %ssignal-async.php on line %d
