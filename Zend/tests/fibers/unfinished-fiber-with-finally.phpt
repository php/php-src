--TEST--
Test unfinished fiber with finally block
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
finally
done
