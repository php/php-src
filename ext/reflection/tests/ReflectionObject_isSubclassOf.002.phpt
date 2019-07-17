--TEST--
ReflectionObject::isSubclassOf() - bad arguments
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {}
$ro = new ReflectionObject(new C);

echo "\n\nTest bad arguments:\n";
try {
	var_dump($ro->isSubclassOf());
} catch (TypeError $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($ro->isSubclassOf('C', 'C'));
} catch (TypeError $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($ro->isSubclassOf(null));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($ro->isSubclassOf('ThisClassDoesNotExist'));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($ro->isSubclassOf(2));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Test bad arguments:
ReflectionClass::isSubclassOf() expects exactly 1 parameter, 0 given
ReflectionClass::isSubclassOf() expects exactly 1 parameter, 2 given
Parameter one must either be a string or a ReflectionClass object
Class ThisClassDoesNotExist does not exist
Parameter one must either be a string or a ReflectionClass object
