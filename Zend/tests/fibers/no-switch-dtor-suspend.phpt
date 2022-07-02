--TEST--
Cannot suspend fiber within destructor
--FILE--
<?php

$fiber = new Fiber(function () {
    $a = new class () {
        public function __destruct() {
            Fiber::suspend();
        }
    };
});

$fiber->start();

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot switch fibers in current execution context in %sno-switch-dtor-suspend.php:%d
Stack trace:
#0 %sno-switch-dtor-suspend.php(%d): Fiber::suspend()
#1 [internal function]: class@anonymous->__destruct()
#2 %sno-switch-dtor-suspend.php(%d): Fiber->start()
#3 {main}
  thrown in %sno-switch-dtor-suspend.php on line %d
