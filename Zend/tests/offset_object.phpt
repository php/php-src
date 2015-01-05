--TEST--
accessing object dimension
--FILE--
<?php

$object = new stdClass;
var_dump($object[1]);

?>
--EXPECTF--	
Fatal error: Cannot use object of type stdClass as array in %s on line %d
