--TEST--
Testing call_user_func() with autoload and passing invalid params
--FILE--
<?php

spl_autoload_register(function ($class) {
	var_dump($class);
});

call_user_func(array('foo', 'bar'));
call_user_func(array('', 'bar'));
call_user_func(array($foo, 'bar'));
call_user_func(array($foo, ''));

?>
--EXPECTF--
string(3) "foo"

Warning: call_user_func() expects parameter 1 to be a valid callback, class 'foo' not found in %s on line %d

Warning: call_user_func() expects parameter 1 to be a valid callback, class '' not found in %s on line %d

Notice: Undefined variable: foo in %s on line %d

Warning: call_user_func() expects parameter 1 to be a valid callback, first array member is not a valid class name or object in %s on line %d

Notice: Undefined variable: foo in %s on line %d

Warning: call_user_func() expects parameter 1 to be a valid callback, first array member is not a valid class name or object in %s on line %d
