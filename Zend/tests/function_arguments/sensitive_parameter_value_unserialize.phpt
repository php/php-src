--TEST--
A SensitiveParameterValue may not be unserialized.
--FILE--
<?php

$v = new SensitiveParameterValue('secret');

var_dump(unserialize(serialize($v)));

?>
--EXPECTF--
Fatal error: Uncaught Exception: Unserializing objects of class SensitiveParameterValue is not supported. in %ssensitive_parameter_value_unserialize.php:5
Stack trace:
#0 [internal function]: SensitiveParameterValue->__unserialize(Array)
#1 %ssensitive_parameter_value_unserialize.php(5): unserialize('O:23:"Sensitive...')
#2 {main}
  thrown in %ssensitive_parameter_value_unserialize.php on line 5
