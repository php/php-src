--TEST--
Fibers in destructors 002: Start in destructor — under test_scheduler
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
