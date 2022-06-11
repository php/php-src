--TEST--
Closure type variance
--FILE--
<?php

class Foo {
    function a(): fn(): mixed {}
    function b(): fn(int) {}
    function c(fn(): int $x) {}
    function d(fn(mixed) $x) {}
    function e(fn(): fn(fn(int)): string $x) {}
}

class Bar extends Foo {
    function a(): fn(): int {}
    function b(): fn(mixed) {}
    function c(fn(): mixed $x) {}
    function d(fn(int) $x) {}
    function e(fn(): fn(fn(mixed)): string $x) {}
}

?>
--EXPECT--
