--TEST--
GH-24792: Dimension write on an assignment result
--FILE--
<?php
($a = [1])[0] = 2;
?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
