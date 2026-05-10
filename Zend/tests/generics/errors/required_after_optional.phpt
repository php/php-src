--TEST--
Errors: required type parameter cannot follow an optional one
--FILE--
<?php
class C<T = int, U> {}
?>
--EXPECTF--
Fatal error: Optional type parameter T cannot be declared before required type parameter U in %s on line %d
