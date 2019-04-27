--TEST--
Test ReflectionProperty::export() errors.
--FILE--
<?php

class TestClass {
}

$a = 5;

echo "Non-existent class:\n";
try {
    ReflectionProperty::export("NonExistentClass", "prop", true);
}
catch(Exception $e) {
    echo $e->getMessage();
}

echo "\n\nWrong property parameter type:\n";
try {
    ReflectionProperty::export($a, 'TestClass', false);
}
catch(ReflectionException $e) {
    echo $e->getMessage();
}

echo "\n\nNon-existent property:\n";
try {
    ReflectionProperty::export('TestClass', "nonExistentProperty", true);
}
catch(Exception $e) {
    echo $e->getMessage();
}

?>
--EXPECTF--
Non-existent class:
Class NonExistentClass does not exist

Wrong property parameter type:
The parameter class is expected to be either a string or an object

Non-existent property:
Property TestClass::$nonExistentProperty does not exist
