--TEST--
SplFixedArray::__construct() with string passed as parameter.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php
try {
	$array = new SplFixedArray( "string" );
} catch (TypeError $iae) {
	echo "Ok - ".$iae->getMessage().PHP_EOL;
}


?>
--EXPECTF--
Ok - SplFixedArray::__construct() expects parameter 1 to be integer, string given
