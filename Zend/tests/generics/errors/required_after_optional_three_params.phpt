--TEST--
Errors: required type parameter following two optional ones names the first optional
--FILE--
<?php
class C<T = int, U = string, V> {}
?>
--EXPECTF--
Fatal error: Optional type parameter T cannot be declared before required type parameter V in %s on line %d
