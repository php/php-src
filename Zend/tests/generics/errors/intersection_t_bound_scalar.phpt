--TEST--
Errors: T in intersection position with scalar bound
--FILE--
<?php
class Foo {}
function x<T : string>(): T & Foo {}
?>
--EXPECTF--
Fatal error: Type parameter T with bound string cannot be part of an intersection type; use an object-shaped bound (e.g. T: object) in %s on line %d
