--TEST--
Bound check: forwarded T-ref where child's bound is at least as tight as parent's
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

interface A<T : Animal> {}
interface B<Y : Dog> extends A<Y> {}
interface C<Z : Animal> extends A<Z> {}

class IntBox<T : int> {}
class IntSubBox<X : int> extends IntBox<X> {}

echo "OK\n";
?>
--EXPECT--
OK
