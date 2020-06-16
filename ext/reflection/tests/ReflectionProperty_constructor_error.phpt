--TEST--
Test ReflectionProperty class constructor errors.
--FILE--
<?php

class TestClass {
}

$a = 5;

echo "Non-existent class:\n";
try {
    $propInfo = new ReflectionProperty("NonExistentClass", "prop");
}
catch(Exception $e) {
    echo $e->getMessage();
}

echo "\n\nWrong property parameter type:\n";
try {
    $propInfo = new ReflectionProperty($a, 'TestClass');
}
catch(ReflectionException $e) {
    echo $e->getMessage();
}

echo "\n\nNon-existent property:\n";
try {
    $propInfo = new ReflectionProperty('TestClass', "nonExistentProperty");
}
catch(Exception $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Non-existent class:
Class NonExistentClass does not exist

Wrong property parameter type:
ReflectionProperty::__construct(): Argument #1 ($class) must be of type object|string, int given

Non-existent property:
Property TestClass::$nonExistentProperty does not exist
