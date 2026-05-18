--TEST--
Bound check: class extends with a concrete arg violating the parent bound
--FILE--
<?php
class Animal {}
class Box<T : Animal> {}

class IntBox extends Box<int> {}
?>
--EXPECTF--
Fatal error: Type argument 1 to extends Box in IntBox does not satisfy the bound Animal on parameter T, int given in %s on line %d
