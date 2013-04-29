--TEST--
Indirect method call by array - Invalid class name
--FILE--
<?php

$arr = array('a', 'b');
$arr();

?>
--EXPECTF--
Fatal error: Class 'a' not found in %s on line %d
