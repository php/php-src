--TEST--
GH-13687 001: Result operand may leak if GC is triggered before consumption
--FILE--
<?php

class A {
    public $cycle;
    public function __construct() { $this->cycle = $this; }
}
class B {
    public function get() {
        return new A();
    }
}

$c = new B();
$objs = [];

while (gc_status()['roots']+2 < gc_status()['threshold']) {
    $obj = new stdClass;
    $objs[] = $obj;
}

var_dump($c->get());

?>
--EXPECTF--
object(A)#%d (1) {
  ["cycle"]=>
  *RECURSION*
}
