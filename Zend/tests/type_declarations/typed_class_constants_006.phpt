--TEST--
Typed class constants (type not allowed; object)
--FILE--
<?php
class A {
    public const object A = 1;
}
?>
--EXPECTF--
Fatal error: Class constant A::A cannot have type object in %s on line %d
