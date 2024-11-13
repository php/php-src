--TEST--
Static constants are not allowed
--FILE--
<?php
class A {
    static const X = 1;
}
?>
--EXPECTF--
Fatal error: Cannot use the static modifier on a class constant in %s on line %d
