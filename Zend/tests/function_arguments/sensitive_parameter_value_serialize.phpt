--TEST--
A SensitiveParameterValue may not be serialized.
--FILE--
<?php

$v = new SensitiveParameterValue('secret');

var_dump(serialize($v));

?>
--EXPECTF--
Fatal error: Uncaught Exception: Serialization of 'SensitiveParameterValue' is not allowed in %ssensitive_parameter_value_serialize.php:5
Stack trace:
#0 %ssensitive_parameter_value_serialize.php(5): serialize(Object(SensitiveParameterValue))
#1 {main}
  thrown in %ssensitive_parameter_value_serialize.php on line 5
