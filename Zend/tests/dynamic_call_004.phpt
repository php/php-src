--TEST--
Testing dynamic call with undefined variables
--FILE--
<?php 

$a::$b();

?>
--EXPECTF--
Notice: Undefined variable: a in %s on line %d

Fatal error: Class name must be a valid object or a string in %s on line %d
