--TEST--
SPL: FixedArray: Trying to instantiate passing object to constructor parameter
--FILE--
<?php

$b = new stdClass;

$a = new SplFixedArray($b);

?>
--EXPECTF--
Warning: SplFixedArray::__construct() expects parameter 1 to be long, object given in %s on line %d
