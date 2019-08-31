--TEST--
$var::class is not supported
--FILE--
<?php

$obj = new stdClass;
var_dump($obj::class);

?>
--EXPECTF--
Fatal error: Cannot use ::class with dynamic class name in %s on line %d
