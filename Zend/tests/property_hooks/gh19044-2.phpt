--TEST--
GH-19044: Protected properties must be scoped according to their prototype (common ancestor does not have a setter)
--FILE--
<?php

abstract class P {
    abstract public mixed $foo { get; }
}

class C1 extends P {
    public mixed $foo { get => 1; }
}

class GrandC1 extends C1 {
    public protected(set) mixed $foo { get => 2; set {} }
}

class C2 extends C1 {
    static function foo($c) { return $c->foo += 1; }
}

var_dump(C2::foo(new GrandC1));

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify protected(set) property GrandC1::$foo from scope C2 in %s:%d
Stack trace:
#0 %s(%d): C2::foo(Object(GrandC1))
#1 {main}
  thrown in %s on line %d
