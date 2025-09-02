--TEST--
A SensitiveParameterValue is clonable.
--FILE--
<?php

$v1 = new SensitiveParameterValue('secret');
$v2 = clone $v1;

var_dump($v1->getValue());
var_dump($v2->getValue());

?>
--EXPECTF--
string(6) "secret"
string(6) "secret"
