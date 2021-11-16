--TEST--
Test unfinished fiber with nested try/catch blocks
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
        }
    } finally {
        echo "outer finally\n";
    }

    echo "unreached\n";

    try {
        echo Fiber::suspend();
    } finally {
        echo "unreached\n";
    }

    echo "end of fiber should not be reached\n";
});

$fiber->start();

unset($fiber); // Destroy fiber object, executing finally block.

echo "done\n";

?>
--EXPECT--
fiber
inner finally
outer finally
done
