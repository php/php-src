--TEST--
Recursive bounds: covariant marker in bound position is still rejected (bounds are invariant)
--FILE--
<?php
class Box<out T> {}
class Pair<out T: Box<U>, out U: Box<T>> {}
?>
--EXPECTF--
Fatal error: Type parameter %s declared covariant (out %s) cannot appear in invariant position in %s on line %d
