--TEST--
Check type hint compatibility in overrides with array hints.
--FILE--
<?php
Class C { function f($a) {} }

echo "Array hint, should be nothing.\n";
Class D extends C { function f(array $a) {} }
?>
==DONE==
--EXPECTF--
Warning: Declaration of D::f(array $a) should be compatible with C::f($a) in %s on line 5
Array hint, should be nothing.
==DONE==
