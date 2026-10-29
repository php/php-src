--TEST--
GH-24792: Compound assignment on a dimension of an assignment result
--FILE--
<?php
($a = [1])[0] += 2;
?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
