--TEST--
assert() - error - invalid params
--INI--
assert.active = 1
assert.warning = 1
assert.callback = f1
assert.quiet_eval = 1
assert.bail = 0
--FILE--
<?php
function f1() 
{
	echo "f1 called\n";
}
function handler($errno, $errstr) {
        echo "in handler()\n\n";
        assert(E_RECOVERABLE_ERROR === $errno);
        var_dump($errstr);
}

//Wrong number of parameters for assert_options()
assert_options(ASSERT_WARNING, 1);
var_dump($rao=assert_options(ASSERT_CALLBACK,"f1",1));


//Unknown option for assert_options()
var_dump($rao=assert_options("F1","f1"));

//Wrong number of parameters for  assert()
$sa="0 != 0";
var_dump($r2=assert($sa,1));


//Catch recoverable error with handler
var_dump($rc=assert('aa=sd+as+safsafasfaçsafçsafç'));
--EXPECTF--
Warning: assert_options() expects at most 2 parameters, 3 given in %s on line %d
NULL

Warning: assert_options() expects parameter 1 to be long, string given in %s on line %d
NULL

Warning: assert() expects exactly 1 parameter, 2 given in %s on line %d
NULL
