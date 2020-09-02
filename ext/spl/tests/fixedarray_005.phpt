--TEST--
SPL: FixedArray: Invalid arguments
--FILE--
<?php

try {
    $a = new SplFixedArray(new stdClass);
} catch (TypeError $iae) {
    echo "Ok - ".$iae->getMessage().PHP_EOL;
}

try {
    $a = new SplFixedArray('FOO');
} catch (TypeError $iae) {
    echo "Ok - ".$iae->getMessage().PHP_EOL;
}

try {
    $a = new SplFixedArray('');
} catch (TypeError $iae) {
    echo "Ok - ".$iae->getMessage().PHP_EOL;
}

?>
--EXPECT--
Ok - SplFixedArray::__construct(): Argument #1 ($size) must be of type int, stdClass given
Ok - SplFixedArray::__construct(): Argument #1 ($size) must be of type int, string given
Ok - SplFixedArray::__construct(): Argument #1 ($size) must be of type int, string given
