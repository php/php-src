--TEST--
Create an SplFixedArray using an SplFixedArray object.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
try {
    $array = new SplFixedArray(new SplFixedArray(3));
} catch (TypeError $iae) {
    echo "Ok - ".$iae->getMessage().PHP_EOL;
}

?>
--EXPECT--
Ok - SplFixedArray::__construct(): Argument #1 ($size) must be of type int, SplFixedArray given
