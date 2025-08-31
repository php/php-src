--TEST--
Check type hint compatibility in overrides with array hints.
--FILE--
<?php
Class C { function f(SomeClass $a) {} }

// Array hint, should be class.
Class D extends C { function f(array $a) {} }
?>
--EXPECTF--
Fatal error: Declaration of D::f(array $a) must be compatible with C::f(SomeClass $a) in %s on line 5
