--TEST--
GH-24792: Property write on an assignment result
--FILE--
<?php
($a = new stdClass)->p = 1;
?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
