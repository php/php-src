--TEST--
Asymmetric visibility: private(namespace) private(set) is invalid (reversed hierarchy)
--FILE--
<?php

namespace Test;

// This should fail: C[private] ⊈ C[ns] (set is more restrictive than get)
class A {
    private(namespace) private(set) string $prop1;
}

?>
--EXPECTF--
Fatal error: Visibility of property Test\A::$prop1 must not be weaker than set visibility in %s on line %d
