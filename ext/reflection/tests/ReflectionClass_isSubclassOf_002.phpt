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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $rc->isSubclassOf('C', 'C');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $rc->isSubclassOf(null);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $rc->isSubclassOf('ThisClassDoesNotExist');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $rc->isSubclassOf(2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Test bad arguments:
ArgumentCountError: ReflectionClass::isSubclassOf() expects exactly 1 argument, 0 given
ArgumentCountError: ReflectionClass::isSubclassOf() expects exactly 1 argument, 2 given

Deprecated: ReflectionClass::isSubclassOf(): Passing null to parameter #1 ($class) of type ReflectionClass|string is deprecated in %s on line %d
ReflectionException: Class "" does not exist
ReflectionException: Class "ThisClassDoesNotExist" does not exist
ReflectionException: Class "2" does not exist
