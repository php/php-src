--TEST--
ReflectionObject::isSubclassOf() - bad arguments
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class A {}
$rc = new ReflectionClass('A');

echo "\n\nTest bad arguments:\n";
try {
    var_dump($rc->isSubclassOf());
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->isSubclassOf('C', 'C'));
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->isSubclassOf(null));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->isSubclassOf('ThisClassDoesNotExist'));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($rc->isSubclassOf(2));
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Test bad arguments:
ReflectionClass::isSubclassOf() expects exactly 1 parameter, 0 given
ReflectionClass::isSubclassOf() expects exactly 1 parameter, 2 given
ReflectionClass::isSubclassOf(): Argument #1 ($class) must be of type ReflectionClass|string, null given
Class ThisClassDoesNotExist does not exist
ReflectionClass::isSubclassOf(): Argument #1 ($class) must be of type ReflectionClass|string, int given
