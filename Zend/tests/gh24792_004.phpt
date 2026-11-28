--TEST--
GH-24792: Unset of a dimension of an assignment result
--FILE--
<?php
unset(($a = [1])[0]);
?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
