--TEST--
Errors: class type parameter cannot appear in an intersection type on a static method
--FILE--
<?php
class A<T: Iterator> {
    public static function foo(T&Countable $x): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T is bound to A and cannot be used in static context in %s on line %d
