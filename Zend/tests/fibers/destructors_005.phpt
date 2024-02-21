--TEST--
Fibers in destructors 005: Suspended and not resumed destructor
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
        try {
            if ($id === 0) {
                Fiber::suspend(new stdClass);
            }
        } finally {
            printf("%d: End destruct\n", $id);
        }
    }
}

$f = new Fiber(function () {
    new Cycle();
    new Cycle();
    gc_collect_cycles();
});

$f->start();

?>
--EXPECT--
0: Start destruct
1: Start destruct
1: End destruct
Shutdown
0: End destruct
