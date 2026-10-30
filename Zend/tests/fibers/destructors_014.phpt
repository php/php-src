--TEST--
Fibers in destructors 014: Exception thrown by a destructor called by the GC during exception unwinding is chained
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

$objects = [];
for ($i = 0; $i < 20000; $i++) {
    $objects[] = new stdClass();
}

function f() {
    global $objects;
    // Releasing these copies during unwinding fills the GC root buffer
    $copies = [...$objects];
    new Cycle();
    throw new Exception('f');
}

function g() {
    try {
        f();
        echo "Not reached\n";
    } catch (Exception $e) {
        echo 'Caught: ', $e->getMessage(), "\n";
        echo 'Previous: ', $e->getPrevious()->getMessage(), "\n";
    }
}

$fiber = new Fiber(function () {
    try {
        g();
    } catch (Exception $e) {
        echo 'Escaped: ', $e->getMessage(), "\n";
    }
});
$fiber->start();

?>
--EXPECT--
Cycle::__destruct
Caught: Cycle::__destruct
Previous: f
