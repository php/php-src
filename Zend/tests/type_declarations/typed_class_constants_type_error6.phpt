--TEST--
Typed class constants (type not allowed; never)
--FILE--
<?php
class A {
    public const never CONST1 = 1;
}
?>
--EXPECTF--
Fatal error: Class constant A::CONST1 cannot have type never in %s on line %d
