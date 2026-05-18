--TEST--
Errors: class type parameter cannot appear in a nullable type on a static method
--FILE--
<?php
class A<T> {
    public static function foo(?T $x): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T is bound to A and cannot be used in static context in %s on line %d
