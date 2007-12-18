--TEST--
Test ReflectionProperty class constructor errors.
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
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
The parameter class is expected to be either a string or an object

Non-existent property:
Property TestClass::$nonExistentProperty does not exist
