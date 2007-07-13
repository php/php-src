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
Warning: Wrong parameter count for assert_options() in %s on line 14
NULL

Warning: assert_options(): Unknown value 0 in %s on line 18
bool(false)

Warning: Wrong parameter count for assert() in %s on line 22
NULL
