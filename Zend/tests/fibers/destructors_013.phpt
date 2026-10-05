--TEST--
Fibers in destructors 013: Exception thrown by a destructor called by the GC is thrown by the statement that triggered the GC
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
    try {
        $copies = [...$objects];
        new Cycle();
        // Releasing the copies fills the GC root buffer
        $copies = null;
        echo "Not reached\n";
    } catch (Exception $e) {
        echo 'Caught: ', $e->getMessage(), "\n";
    }
}

$fiber = new Fiber(function () {
    try {
        f();
    } catch (Exception $e) {
        echo 'Escaped: ', $e->getMessage(), "\n";
    }
});
$fiber->start();

?>
--EXPECT--
Cycle::__destruct
Caught: Cycle::__destruct
