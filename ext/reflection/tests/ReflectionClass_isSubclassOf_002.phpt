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
    $rc->isSubclassOf();
} catch (ArgumentCountError $e) {
    echo $e->getMessage() . "\n";
}
try {
    $rc->isSubclassOf('C', 'C');
} catch (ArgumentCountError $e) {
    echo $e->getMessage() . "\n";
}
try {
    $rc->isSubclassOf(null);
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}
try {
    $rc->isSubclassOf('ThisClassDoesNotExist');
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}
try {
    $rc->isSubclassOf(2);
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Test bad arguments:
ReflectionClass::isSubclassOf() expects exactly 1 argument, 0 given
ReflectionClass::isSubclassOf() expects exactly 1 argument, 2 given
Class "" does not exist
Class "ThisClassDoesNotExist" does not exist
Class "2" does not exist
