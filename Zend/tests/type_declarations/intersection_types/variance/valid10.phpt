--TEST--
Valid intersection type variance
--FILE--
<?php

interface X {}
interface Y {}
interface Z {}

class TestParent implements X, Y, Z {}
class TestChild implements Z {}

class A {
    public X&Y|null $prop;

    public function method1(X&Y&Z $a): X&Y|null{}
    public function method2(X&Y $a): ?X {}
}
class B extends A {
    public X&Y|null $prop;

    public function method1(X&Y|null $a): X&Y&Z {}
    public function method2(?X $a): X&Y {}
}

?>
===DONE===
--EXPECT--
===DONE===
