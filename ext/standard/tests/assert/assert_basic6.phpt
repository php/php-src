--TEST--
assert() - Remove the assert callback
--INI--
assert.active=1
--FILE--
<?php

function f1()
{
    echo "foo\n";
}

assert_options(ASSERT_CALLBACK, "f1");
var_dump(assert_options(ASSERT_CALLBACK));

try {
    assert(false);
} catch (AssertionError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "\n";

assert_options(ASSERT_CALLBACK, null);
var_dump(assert_options(ASSERT_CALLBACK));

try {
    assert(false);
} catch (AssertionError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d

Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
string(2) "f1"
foo
assert(false)


Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d

Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
NULL
assert(false)
