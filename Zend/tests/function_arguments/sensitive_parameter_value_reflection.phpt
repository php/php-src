--TEST--
A SensitiveParameterValue's value is accessible using reflection.
--FILE--
<?php

$v = new SensitiveParameterValue('secret');

$r = new ReflectionClass($v);
$p = $r->getProperty('value');

var_dump($p->getValue($v));

?>
--EXPECTF--
string(6) "secret"
