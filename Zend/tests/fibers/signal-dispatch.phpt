--TEST--
Prevent switching fibers when dispatching pending signals
--EXTENSIONS--
pcntl
posix
--FILE--
<?php

pcntl_signal(SIGUSR1, function (): void {
    if (Fiber::getCurrent() !== null) {
        Fiber::suspend();
    }
});

$fiber = new Fiber(function (): void {
    echo "Fiber start\n";

    posix_kill(posix_getpid(), SIGUSR1);

    try {
        pcntl_signal_dispatch();
    } catch (FiberError $e) {
        Fiber::suspend($e);
    }

    echo "Fiber end\n";
});

$e = $fiber->start();

echo $e, "\n";

$fiber->resume();

?>
--EXPECTF--
Fiber start
FiberError: Cannot switch fibers in current execution context in %ssignal-dispatch.php:%d
Stack trace:
#0 %ssignal-dispatch.php(%d): Fiber::suspend()
#1 [internal function]: {closure}(%d, Array)
#2 %ssignal-dispatch.php(%d): pcntl_signal_dispatch()
#3 [internal function]: {closure}()
#4 %ssignal-dispatch.php(%d): Fiber->start()
#5 {main}
Fiber end
