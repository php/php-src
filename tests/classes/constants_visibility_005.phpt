--TEST--
Static constants are not allowed
--FILE--
<?php
class A {
    static const X = 1;
}
?>
--EXPECTF--
Fatal error: Constant A::X cannot be declared static in %s on line %d
