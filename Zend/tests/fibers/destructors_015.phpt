--TEST--
Fibers in destructors 015: Exception thrown by a destructor called by the GC from internal code
--INI--
zend.enable_gc=1
--FILE--
<?php

class Cycle {
    public $self;
    public function __construct() {
        $this->self = $this;
    }
    public function __destruct() {
        echo "Cycle::__destruct\n";
        throw new Exception('Cycle::__destruct');
    }
}

$fiber = new Fiber(function () {
    try {
        new Cycle();
        gc_collect_cycles();
        echo "Not reached\n";
    } catch (Exception $e) {
        echo 'Caught: ', $e->getMessage(), "\n";
    }
});
$fiber->start();

$objects = [];
for ($i = 0; $i < 20000; $i++) {
    $objects[] = new stdClass();
}

function create_fiber() {
    return new Fiber(function () {
        new Cycle();
        echo "Return\n";
    });
}

$fiber = create_fiber();
try {
    // Arguments are released after the fiber call, which fills the GC root buffer
    $fiber->start([...$objects]);
    echo "Not reached\n";
} catch (Exception $e) {
    echo 'Caught: ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Cycle::__destruct
Caught: Cycle::__destruct
Return
Cycle::__destruct
Caught: Cycle::__destruct
