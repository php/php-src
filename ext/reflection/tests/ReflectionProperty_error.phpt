--TEST--
Test ReflectionProperty class errors.
--FILE--
<?php

class C {
    public static $p;
}

try {
	new ReflectionProperty();
} catch (TypeError $re) {
	echo "Ok - ".$re->getMessage().PHP_EOL;
}
try {
	new ReflectionProperty('C::p');
} catch (TypeError $re) {
	echo "Ok - ".$re->getMessage().PHP_EOL;
}

try {
	new ReflectionProperty('C', 'p', 'x');
} catch (TypeError $re) {
	echo "Ok - ".$re->getMessage().PHP_EOL;
}

?>
--EXPECT--
Ok - ReflectionProperty::__construct() expects exactly 2 parameters, 0 given
Ok - ReflectionProperty::__construct() expects exactly 2 parameters, 1 given
Ok - ReflectionProperty::__construct() expects exactly 2 parameters, 3 given
