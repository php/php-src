--TEST--
SPL: FixedArray: Trying to instantiate passing string to constructor parameter
--FILE--
<?php

try {
    $a = new SplFixedArray('FOO');
} catch (TypeError $iae) {
    echo "Ok - ".$iae->getMessage().PHP_EOL;
}
?>
--EXPECT--
Ok - SplFixedArray::__construct() expects argument #1 ($size) to be of type int, string given
