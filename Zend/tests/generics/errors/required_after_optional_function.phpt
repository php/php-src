--TEST--
Errors: required type parameter cannot follow an optional one
--FILE--
<?php
function f<T = int, U>(): void {}
?>
--EXPECTF--
Fatal error: Optional type parameter T cannot be declared before required type parameter U in %s on line %d
