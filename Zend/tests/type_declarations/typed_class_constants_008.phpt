--TEST--
Typed class constants (type not allowed; callable)
--FILE--
<?php
class A {
    public const callable A = 1;
}
?>
--EXPECTF--
Fatal error: Class constant A::A cannot have type callable in %s on line %d
