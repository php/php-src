--TEST--
Test unfinished fiber with finally block — under test_scheduler
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

unset($fiber); // Destroy fiber object, executing finally block.

echo "done\n";

?>
--EXPECT--
fiber
done
finally
