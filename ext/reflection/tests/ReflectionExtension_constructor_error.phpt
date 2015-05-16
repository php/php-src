--TEST--
ReflectionExtension::__construct()
--CREDITS--
Gerrit "Remi" te Sligte <remi@wolerized.com>
Leon Luijkx <leon@phpgg.nl>
--FILE--
<?php
try {
	$obj = new ReflectionExtension();
} catch (TypeError $re) {
	echo "Ok - ".$re->getMessage().PHP_EOL;
}

try {
	$obj = new ReflectionExtension('foo', 'bar');
} catch (TypeError $re) {
	echo "Ok - ".$re->getMessage().PHP_EOL;
}

try {
	$obj = new ReflectionExtension([]);
} catch (TypeError $re) {
	echo "Ok - ".$re->getMessage().PHP_EOL;
}


?>
==DONE==
--EXPECTF--
Ok - ReflectionExtension::__construct() expects exactly %d parameter, %d given
Ok - ReflectionExtension::__construct() expects exactly %d parameter, %d given
Ok - ReflectionExtension::__construct() expects parameter 1 to be string, array given
==DONE==
