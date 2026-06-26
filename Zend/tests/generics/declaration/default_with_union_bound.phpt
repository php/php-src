--TEST--
Declaration: default checked against union bound when types are concrete
--FILE--
<?php
class Box<T : int | string = float> {}
?>
--EXPECTF--
Fatal error: Default float for type parameter T does not satisfy its bound string|int in %s on line %d
