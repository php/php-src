--TEST--
Check type hint compatibility in overrides with array hints.
--FILE--
<?php
Class C { function f(array $a) {} }

echo "No hint, should be array.\n";
Class D extends C { function f($a) {} }
?>
==DONE==
--EXPECTF--
Warning: Declaration of D::f($a) should be compatible with C::f(array $a) in %s on line 5
No hint, should be array.
==DONE==
