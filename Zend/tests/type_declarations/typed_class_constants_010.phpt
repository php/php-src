--TEST--
Typed class constants (type not allowed; class name)
--FILE--
<?php
class A {
    public const self A = 1;
}
?>
--EXPECTF--
Fatal error: Class constant A::A cannot have type self in %s on line %d
