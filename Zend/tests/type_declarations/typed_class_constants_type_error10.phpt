--TEST--
Typed class constants (type error)
--FILE--
<?php
class A1 {
    const ?B1 C = null;
}

class A2 extends A1 {
    const ?B2 C = null;
}

?>
--EXPECTF--
Fatal error: Type of A2::C must be compatible with A1::C of type ?B1 in %s on line %d
