--TEST--
Declaration: function type-parameter default that does not satisfy its bound is rejected
--FILE--
<?php
class Animal {}
function id<T : Animal = int>(): void {}
?>
--EXPECTF--
Fatal error: Default int for type parameter T does not satisfy its bound Animal in %s on line %d
