--TEST--
Errors: trait type parameter cannot be used in a static method signature
--FILE--
<?php
trait Bag<T> {
    public static function make(T $x): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T is bound to Bag and cannot be used in static context in %s on line %d
