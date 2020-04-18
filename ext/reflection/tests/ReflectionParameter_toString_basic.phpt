--TEST--
ReflectionParameter::__toString()
--CREDITS--
Stefan Koopmanschap <stefan@stefankoopmanschap.nl>
--FILE--
<?php
function ReflectionParameterTest($test, $test2 = null, ...$test3) {
    echo $test;
}
$reflect = new ReflectionFunction('ReflectionParameterTest');
$params = $reflect->getParameters();
foreach($params as $key => $value) {
    echo $value->__toString() . "\n";
}
?>
--EXPECT--
Parameter #0 [ <required> $test ]
Parameter #1 [ <optional> $test2 = NULL ]
Parameter #2 [ <optional> ...$test3 ]
