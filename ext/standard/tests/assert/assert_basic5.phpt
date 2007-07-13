--TEST--
assert() - basic - check switch on warnings using assert_options.
--INI--
assert.active = 1
assert.warning = 0
assert.callback = f1
assert.bail = 0
assert.quiet_eval = 0
--FILE--
<?php
function f1() 
{
	echo "f1 called\n";
}

//switch warning on and test return value
var_dump($rao=assert_options(ASSERT_WARNING, 1));
$sa = "0 != 0";
var_dump($r2=assert($sa));
$sa = "0 == 0";
var_dump($r2=assert($sa));

//switch warning on and test return value
var_dump($rao=assert_options(ASSERT_WARNING, 0));
--EXPECTF--
int(0)
f1 called

Warning: assert(): Assertion "0 != 0" failed in %s on line 10
NULL
bool(true)
int(1)
