--TEST--
accessing object dimension
--FILE--
<?php

$object = new stdClass;
var_dump($object[1]);

?>
--EXPECTF--	
Fatal error: Uncaught Error: Cannot use object of type stdClass as array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
