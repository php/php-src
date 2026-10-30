--TEST--
GH-19044: Protected properties must be scoped according to their prototype (abstract parent has implicit set hook)
--FILE--
<?php

abstract class GP {
    public abstract mixed $foo { get; }
}

class P extends GP {
    public protected(set) mixed $foo { get => $this->foo; }
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
int(2)
