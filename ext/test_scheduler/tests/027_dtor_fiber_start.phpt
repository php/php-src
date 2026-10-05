--TEST--
Fibers in destructors 002: Start in destructor — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/fibers/destructors_002.phpt. Under a scheduler the destructor phase
of gc_collect_cycles() runs in a coroutine, and a Fiber started there runs as
a coroutine too: start() queues its body and yields. While the first
destructor waits for its fiber, the collector goes on with the remaining
destructors in a new coroutine, so the second destructor starts before the
first one ends.
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
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
        $f = new Fiber(function () { });
        $f->start();
        printf("%d: End destruct\n", $id);
    }
}

new Cycle();
new Cycle();
gc_collect_cycles();

?>
--EXPECT--
0: Start destruct
1: Start destruct
0: End destruct
1: End destruct
Shutdown
