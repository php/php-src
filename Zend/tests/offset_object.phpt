--TEST--
accessing object dimension
--FILE--
<?php

$object = new StdClass;
var_dump($object[1]);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use object of type StdClass as array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
