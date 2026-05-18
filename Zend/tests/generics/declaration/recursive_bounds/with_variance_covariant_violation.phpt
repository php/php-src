--TEST--
Recursive bounds: covariant marker in bound position is still rejected (bounds are invariant)
--FILE--
<?php
class Box<+T> {}
class Pair<+T: Box<U>, +U: Box<T>> {}
?>
--EXPECTF--
Fatal error: Type parameter %s declared covariant (+%s) cannot appear in invariant position in %s on line %d
