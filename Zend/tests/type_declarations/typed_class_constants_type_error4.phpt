--TEST--
Typed class constants (type not allowed; callable)
--FILE--
<?php
class A {
    public const callable CONST1 = 1;
}
?>
--EXPECTF--
Fatal error: Class constant A::CONST1 cannot have type callable in %s on line %d
