--TEST--
Suspend in force-closed fiber, catching exception thrown from destructor
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
} catch (FiberError $exception) {
    echo $exception->getMessage(), "\n";
}

echo "done\n";

?>
--EXPECT--
Cannot suspend in a force-closed fiber
done
