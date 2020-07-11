--TEST--
Subtype cannot remove nullable parameter (covariance)
--FILE--
<?php

interface A {
    function method(?int $p);
}

class B implements A {
    function method(int $p) { }
}
--EXPECTF--
Fatal error: Declaration of B::method(int $p) must be compatible with A::method(?int $p) in %s on line %d
