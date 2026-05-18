--TEST--
Bound check: use trait with a concrete arg violating the trait bound
--FILE--
<?php
class Animal {}
trait Holder<T : Animal> { public T $val; }

class IntHolder { use Holder<int>; }
?>
--EXPECTF--
Fatal error: Type argument 1 to use Holder in IntHolder does not satisfy the bound Animal on parameter T, int given in %s on line %d
