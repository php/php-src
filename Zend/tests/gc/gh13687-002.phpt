--TEST--
GH-13687 002: Result operand may leak if GC is triggered before consumption
--FILE--
<?php

class A {
    public $cycle;
    public function __construct() { $this->cycle = $this; }
    public function __toString() { return __CLASS__; }
}
class B {
    public function get() {
        return new A();
    }
}

$root = new stdClass;
gc_collect_cycles();

$objs = [];
while (gc_status()['roots']+2 < gc_status()['threshold']) {
    $obj = new stdClass;
    $objs[] = $obj;
}

$a = [new A, $root][0]::class;

?>
==DONE==
--EXPECT--
==DONE==
