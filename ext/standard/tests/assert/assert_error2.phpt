--TEST--
assert() - basic - Test that bailout works
--INI--
assert.active = 1
assert.warning = 1
assert.callback = f1
assert.quiet_eval = 1
assert.bail = 0
error_reporting = -1
display_errors = 1
--FILE--
<?php
function f1($script, $line, $message, $user_message) 
{
	echo "f1 called\n";
}

//bail out on error
var_dump($rao = assert_options(ASSERT_BAIL, 1));
$sa = "0 != 0";
var_dump($r2 = assert($sa));
echo "If this is printed BAIL hasn't worked";
--EXPECTF--
int(0)

Warning: Missing argument 4 for f1() in %s on line 2
f1 called

Warning: assert(): Assertion "0 != 0" failed in %s on line 10

