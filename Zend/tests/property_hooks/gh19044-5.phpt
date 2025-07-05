--TEST--
GH-19044: Protected properties must be scoped according to their prototype (abstract parent sets protected(set) with not having grandparent a setter - one inherits from grandparent)
--FILE--
<?php

abstract class GP {
    abstract mixed $foo { get; }
}

abstract class P extends GP {
    abstract protected(set) mixed $foo { get; set; }
}

class C1 extends P {
    public protected(set) mixed $foo { get => 2; set {} }
}

class C2 extends GP {
    public mixed $foo = 1;

    static function foo($c) { return $c->foo += 1; }
}

var_dump(C2::foo(new C1));

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify protected(set) property C1::$foo from scope C2 in %s:%d
Stack trace:
#0 %s(%d): C2::foo(Object(C1))
#1 {main}
  thrown in %s on line %d
