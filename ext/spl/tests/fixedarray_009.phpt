--TEST--
SPL: FixedArray: Trying to instantiate passing string to construtor parameter
--FILE--
<?php

try {
	$a = new SplFixedArray('FOO');
} catch (TypeError $iae) {
	echo "Ok - ".$iae->getMessage().PHP_EOL;
}
?>
--EXPECTF--
Ok - SplFixedArray::__construct() expects parameter 1 to be integer, string given
