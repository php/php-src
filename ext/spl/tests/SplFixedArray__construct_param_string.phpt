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
--EXPECT--
Ok - SplFixedArray::__construct() expects argument #1 ($size) to be of type int, string given
