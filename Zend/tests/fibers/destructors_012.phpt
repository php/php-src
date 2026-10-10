--TEST--
Fibers in destructors 012: GC destructor fiber running during exception unwinding does not skip the enclosing catch block
--INI--
zend.enable_gc=1
--FILE--
<?php

class D {
    public function __destruct() {
        echo "D::__destruct\n";
    }
}

class Cycle {
    public $self;
    public function __construct() {
        $this->self = $this;
    }
    public function __destruct() {
        try {
            throw new Exception('Cycle::__destruct');
        } catch (Exception) {
            echo "Cycle::__destruct\n";
        }
    }
}

$objects = [];
for ($i = 0; $i < 50000; $i++) {
    $objects[] = new stdClass();
}

function f() {
    global $objects;
    $d = new D();
    // Releasing these copies during unwinding fills the GC root buffer
    $copies = [...$objects];
    new Cycle();
    throw new Exception('f');
}

function c() {
    try {
        f();
    } catch (Exception $e) {
        echo 'caught: ', $e->getMessage(), "\n";
    }
}

$fiber = new Fiber(function () {
    try {
        c();
    } catch (Throwable $e) {
        echo 'escaped: ', $e->getMessage(), "\n";
    }
});
$fiber->start();

?>
--EXPECT--
D::__destruct
Cycle::__destruct
caught: f
