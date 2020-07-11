--TEST--
ReflectionParameter::isDefaultValueAvailable() should also work for parameters of internal functions
--FILE--
<?php
$class = new ReflectionClass('DateTime');
$method = $class->getMethod('setTime');

foreach ($method->getParameters() as $parameter) {
    var_dump($parameter->isDefaultValueAvailable());
}

echo "----------\n";

$class = new ReflectionClass('DateTimeZone');
$method = $class->getMethod('listIdentifiers');

foreach ($method->getParameters() as $parameter) {
    var_dump($parameter->isDefaultValueAvailable());
}
?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
----------
bool(true)
bool(true)
