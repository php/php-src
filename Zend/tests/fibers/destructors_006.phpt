--TEST--
Fibers in destructors 006: multiple GC runs
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
    }
}

$f = new Fiber(function () {
    new Cycle();
    new Cycle();
    gc_collect_cycles();
});

$f->start();

new Cycle();
new Cycle();
gc_collect_cycles();

$f2->resume();

?>
--EXPECT--
0: Start destruct
1: Start destruct
1: End destruct
2: Start destruct
2: End destruct
3: Start destruct
3: End destruct
0: End destruct
Shutdown
