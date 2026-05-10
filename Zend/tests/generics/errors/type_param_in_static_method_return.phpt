--TEST--
Errors: class type parameter cannot be used as a static method return type
--FILE--
<?php
class A<T> {
    public static function foo(): T {}
}
?>
--EXPECTF--
Fatal error: Type parameter T is bound to A and cannot be used in static context in %s on line %d
