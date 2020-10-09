--TEST--
Invalid property inheritance where one direction is valid and the other unresolved
--FILE--
<?php
class A {
    public X|B $prop;
}
class B extends A {
    public B $prop;
}
?>
--EXPECTF--
Fatal error: Type of B::$prop must be X|B (as in class A) in %s on line %d
