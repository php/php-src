--TEST--
Magic Methods inheritance rules on a non-trivial class hierarchy
--FILE--
<?php
class A {
    public function __get(string|array $name): mixed {} // valid
}

class B extends A {
    public function __get(string|array|object $name): int {} // also valid
}

class C extends B {
    public function __get(string|array $name): int {} // this is invalid
}
?>
--EXPECTF--
Fatal error: Declaration of C::__get(array|string $name): int must be compatible with B::__get(object|array|string $name): int in %s on line %d
