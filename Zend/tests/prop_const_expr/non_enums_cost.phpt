--TEST--
Disallow fetching properties in constant expressions on non-enums even if lhs is other const
--FILE--
<?php

class A {
    public $prop = 42;
}

const A = new A();
const A_prop = A->prop;

?>
--EXPECTF--
Fatal error: Uncaught Error: Fetching properties on non-enums in constant expressions is not allowed in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
