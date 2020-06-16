--TEST--
SPL: FixedArray: accessing uninitialized array
--FILE--
<?php

try {
    $a = new SplFixedArray('');
} catch (TypeError $iae) {
    echo "Ok - ".$iae->getMessage().PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
Ok - SplFixedArray::__construct(): Argument #1 ($size) must be of type int, string given
Done
