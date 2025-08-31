--TEST--
Fibers in destructors 004: Suspend and throw in destructor
--FILE--
<?php

register_shutdown_function(function () {
    printf("Shutdown\n");
});

class Cycle {
    public static $counter = 0;
    public $self;
    public function __construct() {
        $this->self = $this;
    }
    public function __destruct() {
        $id = self::$counter++;
        printf("%d: Start destruct\n", $id);
        if ($id === 0) {
            global $f2;
            $f2 = Fiber::getCurrent();
            Fiber::suspend(new stdClass);
        }
        printf("%d: End destruct\n", $id);
        throw new \Exception(sprintf("%d exception", $id));
    }
}

$f = new Fiber(function () {
    global $f2;
    new Cycle();
    new Cycle();
    new Cycle();
    try {
        gc_collect_cycles();
    } catch (\Exception $e) {
        echo $e, "\n";
    }
    $f2->resume();
});

$f->start();

?>
--EXPECTF--
0: Start destruct
1: Start destruct
1: End destruct
2: Start destruct
2: End destruct
Exception: 1 exception in %s:%d
Stack trace:
#0 [internal function]: Cycle->__destruct()
#1 [internal function]: gc_destructor_fiber()
#2 %s(%d): gc_collect_cycles()
#3 [internal function]: {closure:%s:%d}()
#4 %s(%d): Fiber->start()
#5 {main}

Next Exception: 2 exception in %s:%d
Stack trace:
#0 [internal function]: Cycle->__destruct()
#1 [internal function]: gc_destructor_fiber()
#2 %s(%d): gc_collect_cycles()
#3 [internal function]: {closure:%s:%d}()
#4 %s(%d): Fiber->start()
#5 {main}
0: End destruct

Fatal error: Uncaught Exception: 0 exception in %s:%d
Stack trace:
#0 [internal function]: Cycle->__destruct()
#1 [internal function]: gc_destructor_fiber()
#2 %s(%d): Fiber->resume()
#3 [internal function]: {closure:%s:%d}()
#4 %s(%d): Fiber->start()
#5 {main}
  thrown in %s on line %d
Shutdown
