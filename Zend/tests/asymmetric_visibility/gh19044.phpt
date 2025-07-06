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
--EXPECT--
int(43)
