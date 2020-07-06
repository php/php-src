--TEST--
Typed class constants (type not allowed; void)
--FILE--
<?php
class A {
    public const void A = 1;
}
?>
--EXPECTF--
Fatal error: Class constant A::A cannot have type void in %s on line %d
