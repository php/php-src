--TEST--
Suspend in force-closed fiber after shutdown
--FILE--
<?php

$fiber = new Fiber(function (): void {
    try {
        Fiber::suspend();
    } finally {
        Fiber::suspend();
    }
});

$fiber->start();

echo "done\n";

?>
--EXPECTF--
done

Fatal error: Uncaught FiberError: Cannot suspend in a force-closed fiber in %ssuspend-in-force-close-fiber-after-shutdown.php:%d
Stack trace:
#0 %ssuspend-in-force-close-fiber-after-shutdown.php(%d): Fiber::suspend()
#1 [internal function]: {closure:%s:%d}()
#2 {main}
  thrown in %ssuspend-in-force-close-fiber-after-shutdown.php on line %d
