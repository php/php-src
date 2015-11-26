--TEST--
assert() - basic - Test that bailout works
--INI--
assert.active = 1
assert.warning = 1
assert.callback = f1
assert.quiet_eval = 1
assert.bail = 0
--FILE--
<?php
function f1($message) 
{
	echo "f1 called\n";
}

//bail out on error
var_dump($rao = assert_options(ASSERT_BAIL, 1));
$sa = "0 != 0";
var_dump($r2 = assert($sa, "0 is 0"));
echo "If this is printed BAIL hasn't worked";
--EXPECTF--
int(0)
f1 called

Warning: assert(): 0 is 0: "0 != 0" failed in %s on line 10

