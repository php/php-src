--TEST--
Testing dynamic call with invalid value for method name
--FILE--
<?php 

$a = new stdClass;

$a::$a();

?>
--EXPECTF--
Fatal error: Uncaught Error: Function name must be a string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
