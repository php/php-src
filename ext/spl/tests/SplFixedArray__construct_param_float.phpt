--TEST--
SplFixedArray::__construct() with float passed as parameter.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplFixedArray( 3.141 );

echo $array->getSize();

?>
--EXPECT--
3