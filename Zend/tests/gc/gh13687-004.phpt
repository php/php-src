--TEST--
GH-13687 004: Result operand may leak if GC is triggered before consumption
--ENV--
func=call_user_func
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
getenv('func')(fn (...$args) => gc_collect_cycles(), a: $c->get());

?>
==DONE==
--EXPECT--
==DONE==
