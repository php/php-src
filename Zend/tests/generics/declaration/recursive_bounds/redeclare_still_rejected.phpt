--TEST--
Recursive bounds: duplicate parameter names are still rejected
--FILE--
<?php
class Foo<T: int, T: string> {}
?>
--EXPECTF--
Fatal error: Cannot redeclare type parameter T in %s on line %d
