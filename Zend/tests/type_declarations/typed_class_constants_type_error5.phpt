--TEST--
Typed class constants (type not allowed; void)
--FILE--
<?php
class A {
    public const void CONST1 = 1;
}
?>
--EXPECTF--
Fatal error: Class constant A::CONST1 cannot have type void in %s on line %d
