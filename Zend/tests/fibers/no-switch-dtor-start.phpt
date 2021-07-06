--TEST--
Cannot start fiber within destructor
--FILE--
<?php

return new class () {
    public function __destruct() {
        $fiber = new Fiber(fn () => null);
        $fiber->start();
    }
};

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot switch fibers in current execution context in %sno-switch-dtor-start.php:%d
Stack trace:
#0 %sno-switch-dtor-start.php(%d): Fiber->start()
#1 %sno-switch-dtor-start.php(%d): class@anonymous->__destruct()
#2 {main}
  thrown in %sno-switch-dtor-start.php on line %d
