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
Ok - SplFixedArray::__construct(): Argument #1 ($size) must be of type int, string given
