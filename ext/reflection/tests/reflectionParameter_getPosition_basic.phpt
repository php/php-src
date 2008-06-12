--TEST--
ReflectionParameter::getPosition()
--CREDITS--
Stefan Koopmanschap <stefan@stefankoopmanschap.nl>
#testfest roosendaal on 2008-05-10
--FILE--
<?php
function ReflectionParameterTest($test, $test2 = null) {
	echo $test;
}
$reflect = new ReflectionFunction('ReflectionParameterTest');
$params = $reflect->getParameters();
foreach($params as $key => $value) {
	var_dump($value->getPosition());
}
?>
==DONE==
--EXPECT--
int(0)
int(1)
==DONE==
