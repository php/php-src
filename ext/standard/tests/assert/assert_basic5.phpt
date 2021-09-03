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
int(0)
f1 called

Warning: assert(): assert(0 != 0) failed in %s on line %d
bool(false)
bool(true)
int(1)
