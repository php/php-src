--TEST--
Bound error message: declaration-time default-vs-bound error renders NAMED_WITH_ARGS in the bound
--FILE--
<?php
interface Comparable<T> {}
class Box<T : Comparable<T> = int> {}
?>
--EXPECTF--
Fatal error: Default int for type parameter T does not satisfy its bound Comparable<T> in %s on line %d
