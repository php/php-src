--TEST--
Final constants are not allowed
--FILE--
<?php
class A {
    final const X = 1;
}
?>
--EXPECTF--
Fatal error: Constant A::X cannot be declared final in %s on line %d
