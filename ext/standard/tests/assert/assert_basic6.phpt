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
--EXPECT--
string(2) "f1"
foo
assert(false)

NULL
assert(false)
