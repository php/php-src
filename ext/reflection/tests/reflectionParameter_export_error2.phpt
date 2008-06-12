--TEST--
ReflectionParameter::export() with incorrect first parameter
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
	ReflectionParameter::export($reflect, $key);
}
?>
--EXPECTF--

Fatal error: Uncaught exception 'ReflectionException' with message 'The parameter class is expected to be either a string or an array(class, method)' in %s.php:%d
Stack trace:
#0 [internal function]: ReflectionParameter->__construct(Object(ReflectionFunction), 0)
#1 %s.php(%d): ReflectionParameter::export(Object(ReflectionFunction), 0)
#2 {main}
  thrown in %s.php on line %d
