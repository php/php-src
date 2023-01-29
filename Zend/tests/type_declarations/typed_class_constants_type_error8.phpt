--TEST--
Typed class constants (type not allowed; static)
--FILE--
<?php
class A {
    public const static CONST1 = C;
}
?>
--EXPECTF--
Fatal error: Class constant A::CONST1 cannot have type static in %s on line %d
