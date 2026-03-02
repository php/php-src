--TEST--
A SensitiveParameterValue may not be converted to a string.
--FILE--
<?php

$v = new SensitiveParameterValue('secret');

var_dump((string)$v);

?>
--EXPECTF--
Fatal error: Uncaught Error: Object of class SensitiveParameterValue could not be converted to string in %ssensitive_parameter_value_to_string.php:5
Stack trace:
#0 {main}
  thrown in %ssensitive_parameter_value_to_string.php on line 5
