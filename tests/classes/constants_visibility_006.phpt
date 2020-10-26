--TEST--
Abstract constants are not allowed
--FILE--
<?php
class A {
    abstract const X = 1;
}
?>
--EXPECTF--
Fatal error: Constant A::X cannot be declared abstract in %s on line %d
