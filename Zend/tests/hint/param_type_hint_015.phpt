--TEST--
Parameter type hint - Testing with call_user_func()
--FILE--
<?php

function test(bool $b) {
	print "ok\n";
}

call_user_func('test', true);
call_user_func('test', false);
call_user_func('test', NULL);

?>
--EXPECTF--
ok
ok

Catchable fatal error: Argument 1 passed to test() must be of the type boolean, null given in %s on line %d
