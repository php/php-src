--TEST--
Typed class constants (type mismatch; compile-time)
--FILE--
<?php
class A {
    public const string A = 1;
}
?>
--EXPECTF--
Fatal error: Cannot use int as value for class constant A::A of type string in %s on line %d
