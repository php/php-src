--TEST--
Static methods: class type parameter is allowed nested inside a generic type in a static method signature
--FILE--
<?php
final readonly class Optional<U> {
    public function __construct(public null|U $value) {}
}

class Example<T> {
    public static function unwrap(Optional<T> $value): mixed { return $value->value; }
}
echo "ok\n";
?>
--EXPECT--
ok
