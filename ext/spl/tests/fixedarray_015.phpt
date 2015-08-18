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
--EXPECTF--	
Ok - SplFixedArray::__construct() expects parameter 1 to be integer, string given
Done
