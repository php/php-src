--TEST--
Check type hint compatibility in overrides with array hints.
--FILE--
<?php
Class C { function f(array $a) {} }

echo "Compatible hint.\n";
Class D1 extends C { function f(array $a) {} }

echo "Class hint, should be array.\n";
Class D2 extends C { function f(SomeClass $a) {} }
?>
==DONE==
--EXPECTF--
Fatal error: Declaration of D2::f() must be compatible with that of C::f() in %s on line 8