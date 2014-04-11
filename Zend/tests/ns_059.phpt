--TEST--
059: Constant arrays
--FILE--
<?php
const C = array();

var_dump(C);
?>
--EXPECTF--
Fatal error: Arrays are not allowed in constants at run-time in %sns_059.php on line 4

