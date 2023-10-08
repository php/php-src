--TEST--
assert() - basic - check switch on warnings using assert_options.
--INI--
assert.active = 1
assert.warning = 0
assert.callback = f1
assert.bail = 0
assert.exception=0
--FILE--
<?php
function f1()
{
    echo "f1 called\n";
}

//switch warning on and test return value
var_dump($rao=assert_options(ASSERT_WARNING, 1));
var_dump($r2=assert(0 != 0));
var_dump($r2=assert(0 == 0));

//switch warning on and test return value
var_dump($rao=assert_options(ASSERT_WARNING, 0));
?>
--EXPECTF--
Deprecated: PHP Startup: assert.warning INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: assert.callback INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: assert.exception INI setting is deprecated in Unknown on line 0

Deprecated: Constant ASSERT_WARNING is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
int(0)
f1 called

Warning: assert(): assert(0 != 0) failed in %s on line %d
bool(false)
bool(true)

Deprecated: Constant ASSERT_WARNING is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
int(1)
