--TEST--
assert() - error - invalid params
--INI--
assert.active = 1
assert.warning = 1
assert.callback = f1
assert.bail = 0
--FILE--
<?php
function f1()
{
	echo "f1 called\n";
}

//Wrong number of parameters for assert_options()
assert_options(ASSERT_WARNING, 1);
var_dump($rao = assert_options(ASSERT_CALLBACK, "f1", 1));


//Unknown option for assert_options()
var_dump($rao=assert_options("F1", "f1"));

//Wrong number of parameters for  assert()
var_dump($r2 = assert(0 != 0, "message", 1));

?>
--EXPECTF--
Warning: assert_options() expects at most 2 parameters, 3 given in %s on line %d
NULL

Warning: assert_options() expects parameter 1 to be int, string given in %s on line %d
NULL

Warning: assert() expects at most 2 parameters, 3 given in %s on line %d
NULL
