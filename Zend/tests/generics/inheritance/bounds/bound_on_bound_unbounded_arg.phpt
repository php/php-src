--TEST--
Bound check: forwarded T-ref where child has no bound is rejected when parent requires one
--FILE--
<?php
class Animal {}
interface A<T : Animal> {}
interface B<W> extends A<W> {}
?>
--EXPECTF--
Fatal error: Type argument 1 to extends A in B does not satisfy the bound Animal on parameter T, W given in %s on line %d
