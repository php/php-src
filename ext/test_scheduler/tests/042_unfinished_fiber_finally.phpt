--TEST--
Test unfinished fiber with finally block — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/fibers/unfinished-fiber-with-finally.phpt. Under a scheduler the
fiber runs as a coroutine, and destroying the unfinished fiber cancels it: the
graceful exit is delivered at its Fiber::suspend() when the scheduler next
runs the coroutine, here after the script. TrueAsync's core branch expects the
same order in its copy of the upstream test. So "done" comes before the
finally block.
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
        echo "fiber\n";
        echo Fiber::suspend();
        echo "after suspend\n";
    } catch (Throwable $exception) {
        echo "exit exception caught!\n";
    } finally {
        echo "finally\n";
    }

    echo "end of fiber should not be reached\n";
});

$fiber->start();

unset($fiber); // Destroy fiber object; its finally block runs after the script.

echo "done\n";

?>
--EXPECT--
fiber
done
finally
