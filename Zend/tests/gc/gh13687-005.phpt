--TEST--
GH-13687 005: Result operand may leak if GC is triggered before consumption
--FILE--
<?php

class A {
    public $cycle;
    public function __construct() {
        $this->cycle = $this;
        $a = $this;
        unset($a);
    }
}
class B {
    public function get() {
        return new A();
    }
}

$c = new B();
$objs = [];

$rc = new ReflectionClass(A::class);

while (gc_status()['roots']+1 < gc_status()['threshold']) {
    $obj = new stdClass;
    $objs[] = $obj;
}

var_dump($rc->newInstance());

?>
--EXPECTF--
object(A)#%d (1) {
  ["cycle"]=>
  *RECURSION*
}
