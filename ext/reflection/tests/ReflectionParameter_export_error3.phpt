--TEST--
ReflectionParameter::export() with incorrect second parameter
--CREDITS--
Stefan Koopmanschap <stefan@stefankoopmanschap.nl>
--FILE--
<?php
function ReflectionParameterTest($test, $test2 = null) {
	echo $test;
}
$reflect = new ReflectionFunction('ReflectionParameterTest');
$params = $reflect->getParameters();
foreach($params as $key => $value) {
	ReflectionParameter::export('ReflectionParameterTest', 'incorrect_parameter');
}
--EXPECTF--
Fatal error: Uncaught ReflectionException: The parameter specified by its name could not be found in %s.php:%d
Stack trace:
#0 [internal function]: ReflectionParameter->__construct('ReflectionParam...', 'incorrect_param...')
#1 %s.php(%d): ReflectionParameter::export('ReflectionParam...', 'incorrect_param...')
#2 {main}
  thrown in %s.php on line %d
