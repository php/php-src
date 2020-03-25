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
Fatal error: Declaration of B::m(array $a = Array) must be compatible with A::m(?array $a = NULL) in %sbug71428.1.php on line 6
