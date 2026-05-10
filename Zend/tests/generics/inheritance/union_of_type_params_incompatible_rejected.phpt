--TEST--
Inheritance: a union of bounded type parameters erases to a precise union that rejects incompatible overrides
--FILE--
<?php
class A {}
class B {}
interface TI<T> {
    public function c(mixed $v): T;
}
final class U<Tl: A, Tr: B> implements TI<Tl|Tr> {
    public function c(mixed $v): int { return 0; }
}
?>
--EXPECTF--
Fatal error: Declaration of U::c(mixed $v): int must be compatible with TI::c(mixed $v): A|B in %s on line %d
