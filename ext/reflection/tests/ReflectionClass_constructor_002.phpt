--TEST--
ReflectionClass::__constructor() - bad arguments
--FILE--
<?php
try {
	var_dump(new ReflectionClass());
} catch (TypeError $e) {
	echo $e->getMessage() . "\n";
}

try {
	var_dump(new ReflectionClass(null));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

try {
	var_dump(new ReflectionClass(true));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

try {
	var_dump(new ReflectionClass(1));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

try {
	var_dump(new ReflectionClass(array(1,2,3)));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

try {
	var_dump(new ReflectionClass("stdClass", 1));
} catch (TypeError $e) {
	echo $e->getMessage() . "\n";
}

try {
	var_dump(new ReflectionClass("X"));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
ReflectionClass::__construct() expects exactly 1 parameter, 0 given
Class  does not exist
Class 1 does not exist
Class 1 does not exist

Warning: Array to string conversion in %s on line %d
Class Array does not exist
ReflectionClass::__construct() expects exactly 1 parameter, 2 given
Class X does not exist
