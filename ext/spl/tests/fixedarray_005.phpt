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
===DONE===
--EXPECT--
Ok - SplFixedArray::__construct() expects parameter 1 to be int, object given
Ok - SplFixedArray::__construct() expects parameter 1 to be int, string given
Ok - SplFixedArray::__construct() expects parameter 1 to be int, string given
===DONE===
