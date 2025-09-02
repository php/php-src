--TEST--
A SensitiveParameterValue does not allow dynamic properties.
--FILE--
<?php

$v = new SensitiveParameterValue('secret');

$v->foo = 'bar';

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot create dynamic property SensitiveParameterValue::$foo in %ssensitive_parameter_value_no_dynamic_property.php:5
Stack trace:
#0 {main}
  thrown in %ssensitive_parameter_value_no_dynamic_property.php on line 5
