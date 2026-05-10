--TEST--
Errors: class type parameter cannot be used in a static property type
--FILE--
<?php
class A<T> {
    public static null|T $a = null;
}
?>
--EXPECTF--
Fatal error: Type parameter T is bound to A and cannot be used in static context in %s on line %d
