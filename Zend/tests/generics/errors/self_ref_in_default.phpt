--TEST--
Errors: top-level self-reference in default is rejected
--FILE--
<?php
class C<T = T> {}
?>
--EXPECTF--
Fatal error: Type parameter T cannot reference itself in its own bound or default outside of a generic type argument in %s on line %d
