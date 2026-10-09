--TEST--
Exception thrown by a GC destructor that already has the exception of a previous, suspended destructor in its previous chain
--FILE--
<?php
class Cycle {
    public static ?Throwable $pending = null;
    public $self;

    public function __construct() {
        $this->self = $this;
    }

    public function __destruct() {
        if (self::$pending === null) {
            try {
                Fiber::suspend();
            } finally {
                self::$pending = new RuntimeException("pending");
                throw self::$pending;
            }
        }
        throw new LogicException("thrown", 0, self::$pending);
    }
}

$fiber = new Fiber(function () {
    new Cycle;
    new Cycle;
    try {
        gc_collect_cycles();
    } catch (Throwable $t) {
        for (; $t !== null; $t = $t->getPrevious()) {
            echo $t->getMessage(), "\n";
        }
    }
});
$fiber->start();
?>
--EXPECT--
thrown
pending
