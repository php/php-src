--TEST--
Test unfinished fiber with suspend in finally — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/fibers/unfinished-fiber-with-suspend-in-finally.phpt. Under a
scheduler the fiber runs as a coroutine, and destroying the unfinished fiber
cancels it: the graceful exit is delivered at its Fiber::suspend() when the
scheduler next runs the coroutine, here after the script. TrueAsync's core
branch expects the same order in its copy of the upstream test. So "done"
comes before the outer finally block.
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

$fiber = new Fiber(function (): object {
    try {
        try {
            echo "fiber\n";
            return new \stdClass;
        } finally {
            echo "inner finally\n";
            Fiber::suspend();
            echo "after await\n";
        }
    } catch (Throwable $exception) {
        echo "exit exception caught!\n";
    } finally {
        echo "outer finally\n";
    }

    echo "end of fiber should not be reached\n";
});

$fiber->start();

unset($fiber); // Destroy fiber object; the outer finally block runs after the script.

echo "done\n";

?>
--EXPECT--
fiber
inner finally
done
outer finally
