--TEST--
Valid union type variance
--FILE--
<?php

class X {}
class Y extends X {}

class A {
    public X|Y $prop;

    public function method(int $a): int|float {}
    public function method2(B|string $a): A|string {}
}
class B extends A {
    public X $prop;

    public function method(int|float $a): int {}
    public function method2(A|string $a): B|string {}
}

?>
===DONE===
--EXPECT--
===DONE===
