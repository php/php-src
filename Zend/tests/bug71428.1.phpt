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
?>
--EXPECTF--
Fatal error: Declaration of B::m(array $a = []) must be compatible with A::m(?array $a = null) in %s on line %d
