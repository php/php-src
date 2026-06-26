--TEST--
Declaration: class type-parameter default that does not satisfy its bound is rejected
--FILE--
<?php
class Animal {}
class Box<T : Animal = int> {}
?>
--EXPECTF--
Fatal error: Default int for type parameter T does not satisfy its bound Animal in %s on line %d
