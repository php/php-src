--TEST--
Bug #71978 (Existence of return type hint affects other compatibility rules)
--FILE--
<?php
class A1 {
    function foo(int $a): int {}
}
class B1 extends A1 {
    function foo(string $a): int {}
}
?>
--EXPECTF--
Fatal error: Declaration of B1::foo(string $a): int must be compatible with A1::foo(int $a): int in %s on line %d
