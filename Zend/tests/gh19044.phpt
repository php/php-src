--TEST--
GH-19044: Protected properties must be scoped according to their prototype
--FILE--
<?php

abstract class P {
    protected $foo;
}

class C1 extends P {
    protected $foo = 1;
}

class C2 extends P {
    protected $foo = 2;
    
    static function foo($c) { return $c->foo; }
}

var_dump(C2::foo(new C2));
var_dump(C2::foo(new C1));

?>
--EXPECT--
int(2)
int(1)
