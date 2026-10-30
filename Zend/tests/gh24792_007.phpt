--TEST--
GH-24792: Dimension write on a by-reference assignment result
--FILE--
<?php
$b = [1];
($a =& $b)[0] = 2;
?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
