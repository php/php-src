--TEST--
Testing dynamic call with invalid value for method name
--FILE--
<?php 

$a = new stdClass;

$a::$a();

?>
--EXPECTF--
Fatal error: Function name must be a string in %s on line %d
