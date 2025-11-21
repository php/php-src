--TEST--
Asymmetric visibility with incompatible modifiers (protected and private(namespace))
--FILE--
<?php

namespace Test;

// This should fail: protected and private(namespace) are incomparable
class A {
    protected private(namespace)(set) string $prop1;
}

?>
--EXPECTF--
Fatal error: Property Test\A::$prop1 has incompatible visibility modifiers: protected and private(namespace) operate on different axes (inheritance vs namespace) and cannot be combined in asymmetric visibility in %s on line %d
