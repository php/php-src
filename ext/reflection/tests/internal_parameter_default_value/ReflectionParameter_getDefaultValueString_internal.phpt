--TEST--
ReflectionParameter::getDefaultValueString() returns the string representation of default values of internal functions
--FILE--
<?php
$class = new ReflectionClass('DateTime');
$method = $class->getMethod('setTime');

foreach ($method->getParameters() as $parameter) {
    try {
        var_dump($parameter->getDefaultValueString());
    } catch (ReflectionException $exception) {
        echo $exception->getMessage() . "\n";
    }
}

echo "----------\n";

$class = new ReflectionClass('DateTimeZone');
$method = $class->getMethod('listIdentifiers');

foreach ($method->getParameters() as $parameter) {
    try {
        var_dump($parameter->getDefaultValueString());
    } catch (ReflectionException $exception) {
        echo $exception->getMessage() . "\n";
    }
}

echo "----------\n";



?>
--EXPECT--
Internal error: Failed to retrieve the default value
Internal error: Failed to retrieve the default value
string(1) "0"
string(1) "0"
----------
string(17) "DateTimeZone::ALL"
string(4) "null"
----------
