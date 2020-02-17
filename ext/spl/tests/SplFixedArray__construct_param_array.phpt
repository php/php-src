--TEST--
SplFixedArray::__construct() with array passed as integer.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

try {
    $array = new SplFixedArray( array("string", 1) );
} catch (TypeError $iae) {
    echo "Ok - ".$iae->getMessage().PHP_EOL;
}

?>
--EXPECT--
Ok - SplFixedArray::__construct() expects argument #1 ($size) to be of type int, array given
