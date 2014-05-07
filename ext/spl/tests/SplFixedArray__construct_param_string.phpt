--TEST--
SplFixedArray::__construct() with string passed as parameter.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplFixedArray( "string" );

?>
--EXPECTF--
Warning: SplFixedArray::__construct() expects parameter 1 to be integer, %unicode_string_optional% given in %s on line %d
