--TEST--
SplFixedArray::__construct() with array passed as integer.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplFixedArray( array("string", 1) );

?>
--EXPECTF--
Warning: SplFixedArray::__construct() expects parameter 1 to be long, array given in %s on line %d