--TEST--
Disallow fetching properties in constant expressions on non-enums
--FILE--
<?php

class A {
    public $prop = 42;
}

const A_prop = (new A)->prop;

?>
--EXPECTF--
Fatal error: Uncaught Error: Fetching properties on non-enums in constant expressions is not allowed in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
