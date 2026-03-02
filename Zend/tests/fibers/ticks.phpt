--TEST--
Prevent switching fibers in tick function
--FILE--
<?php

declare(ticks=1);

register_tick_function(function (): void {
    if (Fiber::getCurrent() !== null) {
        Fiber::suspend();
    }
});

$fiber = new Fiber(function (): void {
    echo "1\n";
    echo "2\n";
    echo "3\n";
});

$fiber->start();

?>
--EXPECTF--
1

Fatal error: Uncaught FiberError: Cannot switch fibers in current execution context in %sticks.php:%d
Stack trace:
#0 %sticks.php(%d): Fiber::suspend()
#1 %s(%d): {closure:%s:%d}()
#2 [internal function]: {closure:%s:%d}()
#3 %sticks.php(%d): Fiber->start()
#4 {main}
  thrown in %sticks.php on line %d
