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
var_dump($r2 = assert("0 != 0"));
echo "If this is printed BAIL hasn't worked";
--EXPECTF--
int(0)

Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d

Warning: assert(): Assertion "0 != 0" failed in %s on line 9

Fatal error: Uncaught ArgumentCountError: Too few arguments to function f1(), 3 passed and exactly 4 expected in %sassert_error2.php:2
Stack trace:
#0 [internal function]: f1('%s', 9, '0 != 0')
#1 %sassert_error2.php(9): assert('0 != 0')
#2 {main}
  thrown in %sassert_error2.php on line 2
