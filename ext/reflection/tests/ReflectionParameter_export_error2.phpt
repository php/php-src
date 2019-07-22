--TEST--
ReflectionParameter::export() with incorrect first parameter
--CREDITS--
Stefan Koopmanschap <stefan@stefankoopmanschap.nl>
--FILE--
<?php
function ReflectionParameterTest($test, $test2 = null) {
	echo $test;
}
$reflect = new ReflectionFunction('ReflectionParameterTest');
$params = $reflect->getParameters();
try {
	foreach($params as $key => $value) {
		ReflectionParameter::export($reflect, $key);
	}
}
catch (ReflectionException $e) {
	echo $e->getMessage() . "\n";
}
try {
	foreach($params as $key => $value) {
		ReflectionParameter::export(42, $key);
	}
}
catch (ReflectionException $e) {
	echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
Deprecated: Function ReflectionParameter::export() is deprecated in %s on line %d
Method ReflectionFunction::__invoke() does not exist

Deprecated: Function ReflectionParameter::export() is deprecated in %s on line %d
The parameter class is expected to be either a string, an array(class, method) or a callable object
