--TEST--
assert() - basic - Test that bailout works
--INI--
assert.active = 1
assert.warning = 1
assert.callback = f1
assert.bail = 0
assert.exception=0
--FILE--
<?php
function f1()
{
    echo "f1 called\n";
}

//bail out on error
var_dump($rao=assert_options(ASSERT_BAIL, 1));
var_dump($r2=assert(0 != 0));
echo "If this is printed BAIL hasn't worked";
?>
--EXPECTF--
int(0)
f1 called

Warning: assert(): assert(0 != 0) failed in %s on line %d
