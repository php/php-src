--TEST--
assert() - Define null as assert callback
--INI--
assert.active=1
--FILE--
<?php

function f1()
{
    echo "foo\n";
}

assert_options(ASSERT_CALLBACK);
try {
    assert(false);
} catch (AssertionError $exception) {
    echo $exception->getMessage() . "\n";
}

assert_options(ASSERT_CALLBACK, null);
try {
    assert(false);
} catch (AssertionError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
assert(false)

Warning: Invalid callback , no array or string given in %s on line %d
assert(false)
