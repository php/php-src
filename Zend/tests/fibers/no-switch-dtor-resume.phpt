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
        $this->fiber->resume(1);
    }
};

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot switch fibers in current execution context in %sno-switch-dtor-resume.php:%d
Stack trace:
#0 %sno-switch-dtor-resume.php(%d): Fiber->resume(1)
#1 %sno-switch-dtor-resume.php(%d): class@anonymous->__destruct()
#2 {main}
  thrown in %sno-switch-dtor-resume.php on line %d
