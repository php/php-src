--TEST--
Fibers in destructors 001: Fiber::suspend() in a destructor throws — under test_scheduler
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

/* The scheduler-adapted counterpart of Zend/tests/fibers/destructors_001.phpt.
 * With a scheduler active the destructor phase runs in a dedicated coroutine,
 * which is not a fiber, so the suspend throws instead of parking the flow. */

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
            try {
                Fiber::suspend(new stdClass);
                printf("%d: suspend returned\n", $id);
            } catch (FiberError $e) {
                printf("%d: %s: %s\n", $id, get_class($e), $e->getMessage());
            }
        }
        printf("%d: End destruct\n", $id);
    }
}

$f = new Fiber(function () {
    new Cycle();
    new Cycle();
    new Cycle();
    gc_collect_cycles();
    printf("fiber: done\n");
});

$f->start();

echo "==DONE==\n";
?>
--EXPECT--
0: Start destruct
0: FiberError: Cannot suspend outside of a fiber
0: End destruct
1: Start destruct
1: End destruct
2: Start destruct
2: End destruct
fiber: done
==DONE==
Shutdown
