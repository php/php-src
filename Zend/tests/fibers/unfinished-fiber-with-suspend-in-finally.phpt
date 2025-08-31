--TEST--
Test unfinished fiber with suspend in finally
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

unset($fiber); // Destroy fiber object, executing finally block.

echo "done\n";

?>
--EXPECT--
fiber
inner finally
outer finally
done
