--TEST--
Check type hint compatibility in overrides with array hints.
--FILE--
<?php
Class C { function f(array $a) {} }

// Compatible hint.
Class D1 extends C { function f(array $a) {} }

// Class hint, should be array.
Class D2 extends C { function f(SomeClass $a) {} }
?>
--EXPECTF--
Fatal error: Declaration of D2::f(SomeClass $a) must be compatible with C::f(array $a) in %s on line 8
