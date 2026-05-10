--TEST--
Errors: class type parameter cannot be nested inside a generic type on a static property
--FILE--
<?php
final class Container<U> {
    public function __construct(public U $value) {}
}

class A<T> {
    public static ?Container<T> $box = null;
}
?>
--EXPECTF--
Fatal error: Type parameter T is bound to A and cannot be used in static context in %s on line %d
