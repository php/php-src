--TEST--
Throw in multiple destroyed fibers after shutdown
--FILE--
<?php

$fiber = new Fiber(function (): void {
    $fiber1 = new Fiber(function (): void {
        try {
            Fiber::suspend();
        } finally {
            throw new Exception('test1');
        }
    });

    $fiber1->start();

    $fiber2 = new Fiber(function (): void {
        try {
            Fiber::suspend();
        } finally {
            throw new Exception('test2');
        }
    });

    $fiber2->start();

    Fiber::suspend();
});

$fiber->start();

echo "done\n";

?>
--EXPECTF--
done

Fatal error: Uncaught Exception: test1 in %sthrow-in-multiple-destroyed-fibers-after-shutdown.php:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}()
#1 {main}

Next Exception: test2 in %sthrow-in-multiple-destroyed-fibers-after-shutdown.php:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}()
#1 {main}
  thrown in %sthrow-in-multiple-destroyed-fibers-after-shutdown.php on line %d
