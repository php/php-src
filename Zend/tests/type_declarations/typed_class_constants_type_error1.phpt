--TEST--
Typed class constants (type mismatch; simple)
--FILE--
<?php
class A {
    public const string CONST1 = 1;
}
?>
--EXPECTF--
Fatal error: Cannot use int as value for class constant A::CONST1 of type string in %s on line %d
