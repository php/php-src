--TEST--
ReflectionParameter::getDefaultValueConstantName() should also work for parameters of internal functions.
--FILE--
<?php
$class = new ReflectionClass('DateTime');
$method = $class->getMethod('setTime');

foreach ($method->getParameters() as $parameter) {
    try {
        var_dump($parameter->getDefaultValueConstantName());
    } catch (ReflectionException $exception) {
        echo $exception->getMessage() . "\n";
    }
}

echo "----------\n";

$class = new ReflectionClass('DateTimeZone');
$method = $class->getMethod('getTransitions');

foreach ($method->getParameters() as $parameter) {
    try {
        var_dump($parameter->getDefaultValueConstantName());
    } catch (ReflectionException $exception) {
        echo $exception->getMessage() . "\n";
    }
}

echo "----------\n";

$class = new ReflectionClass('DateTimeZone');
$method = $class->getMethod('listIdentifiers');

foreach ($method->getParameters() as $parameter) {
    try {
        var_dump($parameter->getDefaultValueConstantName());
    } catch (ReflectionException $exception) {
        echo $exception->getMessage() . "\n";
    }
}

?>
--EXPECT--
Internal error: Failed to retrieve the default value
Internal error: Failed to retrieve the default value
NULL
NULL
----------
string(11) "PHP_INT_MIN"
string(11) "PHP_INT_MAX"
----------
string(17) "DateTimeZone::ALL"
NULL
