--TEST--
ReflectionParameter::__toString() should display default values for internal functions as well
--FILE--
<?php
$class = new ReflectionClass('DateTime');
$method = $class->getMethod('setTime');

foreach ($method->getParameters() as $k => $parameter) {
    echo $parameter . "\n";
}

echo "----------\n";

$class = new ReflectionClass('DateTimeZone');
$method = $class->getMethod('listIdentifiers');

foreach ($method->getParameters() as $parameter) {
    echo $parameter . "\n";
}
?>
--EXPECT--
Parameter #0 [ <required> int $hour ]
Parameter #1 [ <required> int $minute ]
Parameter #2 [ <optional> int $second = 0 ]
Parameter #3 [ <optional> int $microsecond = 0 ]
----------
Parameter #0 [ <optional> int $timezoneGroup = DateTimeZone::ALL ]
Parameter #1 [ <optional> ?string $countryCode = null ]
