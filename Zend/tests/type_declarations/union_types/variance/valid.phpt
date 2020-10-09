--TEST--
Valid union type variance
--FILE--
<?php

class X {}
class Y extends X {}

class A {
    public X|Y $prop;
    public iterable $prop2;

    public function method(int $a): int|float {}
    public function method2(B|string $a): A|string {}
    public function method3(Y|B $a): X|A {}
    public function method4(Traversable|X $a): iterable|X {}
}
class B extends A {
    public X $prop;
    public array|Traversable $prop2;

    public function method(int|float $a): int {}
    public function method2(A|string $a): B|string {}
    public function method3(A|X $a): B|Y {}
    public function method4(iterable|X $a): Traversable|X {}
}

?>
===DONE===
--EXPECT--
===DONE===
