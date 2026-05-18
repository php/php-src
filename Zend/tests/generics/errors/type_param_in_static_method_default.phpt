--TEST--
Errors: class type parameter cannot be used as a default for a static method's own type parameter
--FILE--
<?php
class A<T> {
    public static function foo<U = T>(U $x): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T is bound to A and cannot be used in static context in %s on line %d
