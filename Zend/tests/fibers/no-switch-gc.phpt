--TEST--
Context switches are prevented during GC collect cycles
--FILE--
<?php

$fiber = new Fiber(function () {
    call_user_func(function () {
        $a = new class { public $next; };

        $b = new class {
            public $next;
            public function __destruct() {
                Fiber::suspend();
            }
        };

        $a->next = $b;
        $b->next = $a;
    });

    gc_collect_cycles();
});

$fiber->start();

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot switch fibers in current execution context in %sno-switch-gc.php:%d
Stack trace:
#0 %sno-switch-gc.php(%d): Fiber::suspend()
#1 [internal function]: class@anonymous->__destruct()
#2 %sno-switch-gc.php(%d): gc_collect_cycles()
#3 [internal function]: {closure}()
#4 %sno-switch-gc.php(%d): Fiber->start()
#5 {main}
  thrown in %sno-switch-gc.php on line %d
