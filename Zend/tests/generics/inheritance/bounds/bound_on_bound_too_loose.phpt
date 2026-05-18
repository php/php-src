--TEST--
Bound check: forwarded T-ref where child's bound is looser than parent's is rejected
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

interface A<T : Dog> {}
interface B<Y : Animal> extends A<Y> {}
?>
--EXPECTF--
Fatal error: Type argument 1 to extends A in B does not satisfy the bound Dog on parameter T, Y given in %s on line %d
