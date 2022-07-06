--TEST--
ReflectionParameter::__toString()
--CREDITS--
Stefan Koopmanschap <stefan@stefankoopmanschap.nl>
--FILE--
<?php
function ReflectionParameterTest($test, $test2 = null) {
    echo $test;
}
$reflect = new ReflectionFunction('ReflectionParameterTest');
foreach($reflect->getParameters() as $key => $value) {
    echo new ReflectionParameter('ReflectionParameterTest', $key), "\n";
}
?>
--EXPECT--
Parameter #0 [ <required> $test ]
Parameter #1 [ <optional> $test2 = NULL ]
