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

echo "\n\nIncorrect number of args:\n";
ReflectionProperty::export();
ReflectionProperty::export('TestClass', "nonExistentProperty", true, false);

?>
--EXPECTF--
Non-existent class:

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
Class NonExistentClass does not exist

Wrong property parameter type:

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
The parameter class is expected to be either a string or an object

Non-existent property:

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
Property TestClass::$nonExistentProperty does not exist

Incorrect number of args:

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d

Warning: ReflectionProperty::export() expects at least 2 parameters, 0 given in %s on line %d

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d

Warning: ReflectionProperty::export() expects at most 3 parameters, 4 given in %s on line %d
