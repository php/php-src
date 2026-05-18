--TEST--
Bound error message: link-time bound violation renders NAMED_WITH_ARGS with its type arguments
--FILE--
<?php
interface Comparable<T> {}

interface Bag<T : Comparable<T>> {}

class IntBag implements Bag<int> {}
?>
--EXPECTF--
Fatal error: Type argument 1 to implements Bag in IntBag does not satisfy the bound Comparable<T> on parameter T, int given in %s on line %d
