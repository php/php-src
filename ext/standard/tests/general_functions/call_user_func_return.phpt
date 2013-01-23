--TEST--
call_user_func() and return value
--FILE--
<?php

$t1 = 'test1';

function test1($arg1, $arg2)
{
	global $t1;
	echo "$arg1 $arg2\n";
	return $t1;
}

$t2 = 'test2';

function & test2($arg1, $arg2)
{
	global $t2;
	echo "$arg1 $arg2\n";
	return $t2;
}

function test($func)
{
	debug_zval_dump($func('Direct', 'Call'));
	debug_zval_dump(call_user_func_array($func, array('User', 'Func')));
}

test('test1');
test('test2');

?>
===DONE===
--EXPECTF--
Direct Call
string(5) "test1" refcount(1)
User Func
string(5) "test1" refcount(1)
Direct Call
string(5) "test2" refcount(2)
User Func
string(5) "test2" refcount(1)
===DONE===
