--TEST--
Create an SplFixedArray using an SplFixedArray object.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
try {
	$array = new SplFixedArray(new SplFixedArray(3));
} catch (TypeException $iae) {
	echo "Ok - ".$iae->getMessage().PHP_EOL;
}

?>
--EXPECTF--
Ok - SplFixedArray::__construct() expects parameter 1 to be integer, object given
