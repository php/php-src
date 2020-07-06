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
Fatal error: Property B::$prop must be of type X|B to be compatible with overridden property A::$prop in %s on line %d
