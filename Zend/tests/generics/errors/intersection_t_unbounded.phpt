--TEST--
Errors: T in intersection position with unbounded T
--FILE--
<?php
class Foo {}
function x<T>(): T & Foo {}
?>
--EXPECTF--
Fatal error: Type parameter T with bound mixed cannot be part of an intersection type; use an object-shaped bound (e.g. T: object) in %s on line %d
