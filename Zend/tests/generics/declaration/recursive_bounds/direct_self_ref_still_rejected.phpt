--TEST--
Recursive bounds: direct self-reference in a bound (not inside a type argument) is still rejected
--FILE--
<?php
class Foo<T: T> {}
?>
--EXPECTF--
Fatal error: Type parameter T cannot reference itself in its own bound or default outside of a generic type argument in %s on line %d
