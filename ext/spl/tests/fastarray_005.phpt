--TEST--
SPL: FastArray: Trying to instantiate passing object to constructor parameter
--FILE--
<?php

$b = new stdClass;

$a = new SplFastArray($b);

?>
--EXPECTF--
Warning: SplFastArray::__construct() expects parameter 1 to be long, object given in %s on line %d
