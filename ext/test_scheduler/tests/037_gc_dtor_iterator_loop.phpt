--TEST--
OSS-Fuzz #471533782: a throwing finally after Fiber::suspend() in a destructor — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/fibers/oss-fuzz-471533782-001.phpt. Under a scheduler the
destructor phase of gc_collect_cycles() runs in a coroutine, not in a fiber,
so Fiber::suspend() in the destructor throws "Cannot suspend outside of a
fiber" instead of parking; the exception the finally block throws carries it
as its previous one. The scheduler RFC lists this behavior change as
"Fiber::suspend() inside a destructor throws".
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

class Cycle {
    public $self;
    public function __construct() {
        $this->self = $this;
    }
    public function __destruct() {
        try {
            Fiber::suspend();
        } finally {
            throw new Exception();
        }
    }
}

$f = new Fiber(function () {
    new Cycle();
    gc_collect_cycles();
});
$f->start();

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot suspend outside of a fiber in %s:%d
Stack trace:
#0 %s(%d): Fiber::suspend()
#1 [internal function]: Cycle->__destruct()
#2 {main}

Next Exception in %s:%d
Stack trace:
#0 [internal function]: Cycle->__destruct()
#1 {main}
  thrown in %s on line %d
