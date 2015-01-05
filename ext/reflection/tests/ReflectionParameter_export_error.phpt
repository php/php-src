--TEST--
ReflectionParameter::export() without parameters
--CREDITS--
Stefan Koopmanschap <stefan@stefankoopmanschap.nl>
--FILE--
<?php
function ReflectionParameterTest($test, $test2 = null) {
	echo $test;
}
$reflect = new ReflectionFunction('ReflectionParameterTest');
foreach($reflect->getParameters() as $key => $value) {
	ReflectionParameter::export();
}
?>
==DONE==
--EXPECTF--

Warning: ReflectionParameter::export() expects at least 2 parameters, 0 given in %s.php on line %d

Warning: ReflectionParameter::export() expects at least 2 parameters, 0 given in %s.php on line %d
==DONE==
