--TEST--
GH-13687 003: Result operand may leak if GC is triggered before consumption
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
class Debug {
    public function __debugInfo() {
        gc_collect_cycles();
        return [];
    }
}

$c = new B();
var_dump($c->get(), new Debug);

?>
==DONE==
--EXPECTF--
object(A)#%d (1) {
  ["cycle"]=>
  *RECURSION*
}
object(Debug)#%d (0) {
}
==DONE==
