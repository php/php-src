--TEST--
Test unfinished fiber with suspend in finally — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/fibers/unfinished-fiber-with-throw-in-finally.phpt. Under a
scheduler the fiber runs as a coroutine, and destroying the unfinished fiber
cancels it: the graceful exit is delivered at its Fiber::suspend() when the
scheduler next runs the coroutine, here after the script. TrueAsync's core
branch expects the same order in its copy of the upstream test. So "done"
comes before the finally blocks.
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

$fiber = new Fiber(function (): void {
    try {
        try {
            try {
                echo "fiber\n";
                echo Fiber::suspend();
                echo "after await\n";
            } catch (Throwable $exception) {
                echo "inner exit exception caught!\n";
            }
        } catch (Throwable $exception) {
            echo "exit exception caught!\n";
        } finally {
            echo "inner finally\n";
            throw new \Exception("finally exception");
        }
    } catch (Throwable $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    } finally {
        echo "outer finally\n";
    }

    try {
        echo Fiber::suspend();
    } catch (Throwable $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }
});

$fiber->start();

unset($fiber); // Destroy fiber object; its finally blocks run after the script.

echo "done\n";

?>
--EXPECT--
fiber
done
inner finally
Exception: finally exception
outer finally
FiberError: Cannot suspend in a force-closed fiber
