--TEST--
Closure type covariance
--FILE--
<?php

class Foo {
    function a(): \Closure(): mixed {}
    function b(): \Closure(int) {}
    function c(\Closure(): int $x) {}
    function d(\Closure(mixed) $x) {}
    function e(\Closure(): \Closure(\Closure(int)): string $x) {}
}

class Bar extends Foo {
    function a(): \Closure(): int {}
    function b(): \Closure(mixed) {}
    function c(\Closure(): mixed $x) {}
    function d(\Closure(int) $x) {}
    function e(\Closure(): \Closure(\Closure(mixed)): string $x) {}
}

?>
--EXPECT--
