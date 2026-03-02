--TEST--
Abstract constants are not allowed
--FILE--
<?php
class A {
    abstract const X = 1;
}
?>
--EXPECTF--
Fatal error: Cannot use the abstract modifier on a class constant in %s on line %d
