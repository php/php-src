--TEST--
Test ReflectionProperty class constructor errors.
--FILE--
<?php

class TestClass {
}

$a = 5;

echo "Non-existent class:\n";
try {
    new ReflectionProperty("NonExistentClass", "prop");
} catch (ReflectionException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\nWrong property parameter type:\n";
try {
    new ReflectionProperty($a, 'TestClass');
}
catch(ReflectionException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\nNon-existent property:\n";
try {
    new ReflectionProperty('TestClass', "nonExistentProperty");
}
catch(ReflectionException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Non-existent class:
ReflectionException: Class "NonExistentClass" does not exist

Wrong property parameter type:
ReflectionException: Class "5" does not exist

Non-existent property:
ReflectionException: Property TestClass::$nonExistentProperty does not exist
