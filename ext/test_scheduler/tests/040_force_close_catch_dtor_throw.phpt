--TEST--
Suspend in force-closed fiber: the exception is thrown after the script, past the catch — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/fibers/suspend-in-force-close-fiber-catching-exception.phpt. Under
a scheduler the fiber runs as a coroutine, and destroying the unfinished fiber
cancels it: the graceful exit is delivered at its Fiber::suspend() when the
scheduler next runs the coroutine, here after the script. TrueAsync's core
branch expects the same order in its copy of the upstream test. The FiberError
of the second Fiber::suspend() is therefore thrown after "done", where the
script's catch no longer stands, and the request ends with it.
--SKIPIF--
<?php
if (!function_exists("TestScheduler\\spawn")) die("skip test_scheduler runtime required");
?>
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

try {
    (function (): void {
        $fiber = new Fiber(function (): void {
            try {
                Fiber::suspend();
            } finally {
                Fiber::suspend();
            }
        });

        $fiber->start();
    })();
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

echo "done\n";

?>
--EXPECTF--
done

Fatal error: Uncaught FiberError: Cannot suspend in a force-closed fiber in %s:%d
Stack trace:
#0 %s(%d): Fiber::suspend()
#1 [internal function]: {closure:%s:%d}()
#2 {main}
  thrown in %s on line %d
