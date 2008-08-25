--TEST--
call_user_func(): Wrong parameters
--FILE--
<?php

call_user_func(array('Foo', 'bar'));
call_user_func(array(NULL, 'bar'));
call_user_func(array('stdclass', NULL));

?>
--EXPECTF--
Warning: call_user_func() expects parameter 1 to be a valid callback, class 'Foo' not found in %s on line %d

Warning: call_user_func() expects parameter 1 to be a valid callback, first array member is not a valid class name or object in %s on line %d

Warning: call_user_func() expects parameter 1 to be a valid callback, second array member is not a valid method in %s on line %d
