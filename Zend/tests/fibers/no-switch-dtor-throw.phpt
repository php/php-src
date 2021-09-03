--TEST--
Cannot resume fiber within destructor
--FILE--
<?php

$fiber = new Fiber(function () {
    Fiber::suspend();
});
$fiber->start();

return new class ($fiber) {
    private $fiber;

    public function __construct(Fiber $fiber) {
        $this->fiber = $fiber;
    }

    public function __destruct() {
        $this->fiber->throw(new Error());
    }
};

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot switch fibers in current execution context in %sno-switch-dtor-throw.php:%d
Stack trace:
#0 %sno-switch-dtor-throw.php(%d): Fiber->throw(Object(Error))
#1 %sno-switch-dtor-throw.php(%d): class@anonymous->__destruct()
#2 {main}
  thrown in %sno-switch-dtor-throw.php on line %d
