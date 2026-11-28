--TEST--
GH-24792: Dimension of an assignment result passed by reference
--FILE--
<?php
function f(&$x) {}
f(($a = [1])[0]);
?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
