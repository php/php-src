--TEST--
Displaying function arguments in errors
--INI--
display_error_function_args=On
--FILE--
<?php

// A function that sets its own parameters in docref call, to compare
unlink('/');
// Something with sensitive parameters that exists in a minimal build,
// and also doesn't set anything in the docref call
// XXX: May be better to provide a new zend_test func?
password_hash("test", PASSWORD_BCRYPT, array("salt" => "123456789012345678901" . chr(0)));

ini_set("display_error_function_args", "Off");

unlink('/');
password_hash("test", PASSWORD_BCRYPT, array("salt" => "123456789012345678901" . chr(0)));

?>
--EXPECTF--
Warning: unlink('/'): %s in %s on line %d

Warning: password_hash(Object(SensitiveParameterValue), '2y', Array): The "salt" option has been ignored, since providing a custom salt is no longer supported in %s on line %d

Warning: unlink(/): %s in %s on line %d

Warning: password_hash(): The "salt" option has been ignored, since providing a custom salt is no longer supported in %s on line %d
