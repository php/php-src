--TEST--
Check type hint compatibility in overrides with array hints.
--FILE--
<?php
Class C { function f(SomeClass $a) {} }

echo "Array hint, should be class.\n";
Class D extends C { function f(array $a) {} }
?>
==DONE==
--EXPECTF--
Strict Standards: Declaration of D::f() should be compatible with C::f(SomeClass $a) in %s on line 5
Array hint, should be class.
==DONE==
