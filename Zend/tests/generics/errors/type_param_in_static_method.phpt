--TEST--
Errors: class type parameter cannot be used in a static method signature
--FILE--
<?php
class A<T> {
    public static function foo(T $a): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T is bound to A and cannot be used in static context in %s on line %d
