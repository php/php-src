--TEST--
Fibers in destructors 003: Resume in destructor
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
        global $f;
        $f->resume();
        printf("%d: End destruct\n", $id);
    }
}

$f = new Fiber(function () {
    while (true) {
        Fiber::suspend();
    }
});
$f->start();

new Cycle();
new Cycle();
gc_collect_cycles();

?>
--EXPECT--
0: Start destruct
0: End destruct
1: Start destruct
1: End destruct
Shutdown
