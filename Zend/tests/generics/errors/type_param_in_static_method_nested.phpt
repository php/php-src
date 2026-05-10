--TEST--
Errors: class type parameter cannot be nested inside a generic type in a static method signature
--FILE--
<?php
final readonly class Optional<U> {
    public function __construct(public null|U $value) {}
}

class Example<T> {
    public static function unwrap(Optional<T> $value): mixed { return $value->value; }
}
?>
--EXPECTF--
Fatal error: Type parameter T is bound to Example and cannot be used in static context in %s on line %d
