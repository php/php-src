--TEST--
bug #71428.1: inheritance with null default values
--FILE--
<?php
class A {
    public function m(array $a = null) {}
}
class B extends A {
    public function m(array $a = []) {}
}
--EXPECTF--
Warning: Declaration of B::m(array $a = Array) should be compatible with A::m(?array $a = NULL) in %sbug71428.1.php on line 7

