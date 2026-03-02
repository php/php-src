--TEST--
ReflectionParameter::getDefaultValue() should also work for parameters of internal functions.
--FILE--
<?php
$class = new ReflectionClass('DateTime');
$method = $class->getMethod('setTime');

foreach ($method->getParameters() as $k => $parameter) {
    try {
        var_dump($parameter->getDefaultValue());
    } catch (ReflectionException $exception) {
        echo $exception->getMessage() . "\n";
    }
}

echo "----------\n";

$class = new ReflectionClass('DateTimeZone');
$method = $class->getMethod('listIdentifiers');

foreach ($method->getParameters() as $parameter) {
    try {
        var_dump($parameter->getDefaultValue());
    } catch (ReflectionException $exception) {
        echo $exception->getMessage() . "\n";
    }
}

?>
--EXPECT--
Internal error: Failed to retrieve the default value
Internal error: Failed to retrieve the default value
int(0)
int(0)
----------
int(2047)
NULL
