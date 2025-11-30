--TEST--
Asymmetric visibility with incompatible modifiers (private(namespace) and protected(set))
--FILE--
<?php

namespace Test;

// This should fail: private(namespace) and protected(set) are incomparable
class A {
    private(namespace) protected(set) string $prop1;
}

?>
--EXPECTF--
Fatal error: Property Test\A::$prop1 has incompatible visibility modifiers: protected and private(namespace) operate on different axes (inheritance vs namespace) and cannot be combined in asymmetric visibility in %s on line %d
