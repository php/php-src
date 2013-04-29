--TEST--
Indirect method call by array - Invalid method name
--FILE--
<?php

$arr = array('stdclass', 'b');
$arr();

?>
--EXPECTF--
Fatal error: Call to undefined method stdClass::b() in %s on line %d
