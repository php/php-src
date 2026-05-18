--TEST--
Bound check: implements with a concrete arg violating the interface bound
--FILE--
<?php
class Animal {}
interface Bag<T : Animal> {}

class IntBag implements Bag<int> {}
?>
--EXPECTF--
Fatal error: Type argument 1 to implements Bag in IntBag does not satisfy the bound Animal on parameter T, int given in %s on line %d
