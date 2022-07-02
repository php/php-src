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
    echo $e->getMessage();
}

echo "\n\nWrong property parameter type:\n";
try {
    new ReflectionProperty($a, 'TestClass');
}
catch(ReflectionException $e) {
    echo $e->getMessage();
}

echo "\n\nNon-existent property:\n";
try {
    new ReflectionProperty('TestClass', "nonExistentProperty");
}
catch(ReflectionException $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Non-existent class:
Class "NonExistentClass" does not exist

Wrong property parameter type:
Class "5" does not exist

Non-existent property:
Property TestClass::$nonExistentProperty does not exist
