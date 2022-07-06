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
    $ro->isSubclassOf();
} catch (ArgumentCountError $e) {
    echo $e->getMessage() . "\n";
}
try {
    $ro->isSubclassOf('C', 'C');
} catch (ArgumentCountError $e) {
    echo $e->getMessage() . "\n";
}
try {
    $ro->isSubclassOf(null);
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}
try {
    $ro->isSubclassOf('ThisClassDoesNotExist');
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}
try {
    $ro->isSubclassOf(2);
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
