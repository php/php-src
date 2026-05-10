--TEST--
Declaration: default checked against intersection bound when types are concrete
--FILE--
<?php
class Box<T : Traversable & Countable = int> {}
?>
--EXPECTF--
Fatal error: Default int for type parameter T does not satisfy its bound Traversable&Countable in %s on line %d
