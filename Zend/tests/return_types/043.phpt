--TEST--
Some magic methods can declare mixed return type
--FILE--
<?php
class Foo {
    public function __get($name): bool {}
    public function __call($name, $args): string {}
    public static function __callStatic($name, $args): self {}
    public function __invoke(): self {}
}

class Bar {
    public function __get($name): string|array {}
    public function __call($name, $args): int|float {}
    public static function __callStatic($name, $args): ?object {}
    public function __invoke(): Foo|int {}
}

class Baz {
    public function __get($name): mixed {}
    public function __call($name, $args): mixed {}
    public static function __callStatic($name, $args): mixed {}
    public function __invoke(): mixed {}
}

echo 'Okay!';
?>
--EXPECT--
Okay!
