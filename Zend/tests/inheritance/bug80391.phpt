--TEST--
Iterable not covariant to mixed
--FILE--
<?php

class A {
    public function method1(): mixed {}
    public function method2(): array|object {}
    public function method3(iterable $x) {}
    public function method4(iterable $x) {}
}

class B extends A {
    public function method1(): iterable {}
    public function method2(): iterable {}
    public function method3(mixed $x) {}
    public function method4(array|object $x) {}
}

?>
===DONE===
--EXPECT--
===DONE===
