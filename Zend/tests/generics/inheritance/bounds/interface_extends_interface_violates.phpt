--TEST--
Bound check: interface extends another generic interface with violating arg
--FILE--
<?php
class Animal {}
interface A<T : Animal> {}

interface B extends A<int> {}
?>
--EXPECTF--
Fatal error: Type argument 1 to extends A in B does not satisfy the bound Animal on parameter T, int given in %s on line %d
