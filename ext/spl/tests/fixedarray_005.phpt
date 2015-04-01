--TEST--
SPL: FixedArray: Trying to instantiate passing object to constructor parameter
--FILE--
<?php

$b = new stdClass;

try {
	$a = new SplFixedArray($b);
}
catch(InvalidArgumentException $iae) {
	echo "Ok - ".$iae->getMessage().PHP_EOL;
}


?>
--EXPECTF--
Ok - SplFixedArray::__construct() expects parameter 1 to be integer, object given
