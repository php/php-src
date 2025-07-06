--TEST--
GH-19044: Protected properties must be scoped according to their prototype (protected(set) on non-hooked property)
--FILE--
<?php

class P {
    public mixed $foo { get => 42; }
}

class C1 extends P {
    public protected(set) mixed $foo = 1;
}

class C2 extends P {
    public protected(set) mixed $foo;

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
