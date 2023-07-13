--TEST--
assert() - basic - Test that bailout works
--INI--
assert.active = 1
assert.warning = 1
assert.callback = f1
assert.bail = 0
assert.exception=0
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
var_dump($r2 = assert(0 != 0));
echo "If this is printed BAIL hasn't worked";
?>
--EXPECTF--
Deprecated: PHP Startup: assert.callback INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: assert.exception INI setting is deprecated in Unknown on line 0

Deprecated: Constant ASSERT_BAIL is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
int(0)
f1 called

Warning: assert(): assert(0 != 0) failed in %s on line %d
