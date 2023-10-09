--TEST--
assert() asserting multiple with callback
--INI--
assert.active = 1
assert.warning = 1
assert.bail = 0
assert.exception=1
--FILE--
<?php
assert_options(ASSERT_CALLBACK, function ($f) {});
try {
    assert(false);
} catch (Throwable) {}
try {
    assert(false);
} catch (Throwable) {}
try {
    assert(false);
} catch (Throwable) {}
try {
    assert(false);
} catch (Throwable) {}
try {
    assert(false);
} catch (Throwable) {}
try {
    assert(false);
} catch (Throwable) {}
try {
    assert(false);
} catch (Throwable) {}
?>
DONE
--EXPECTF--
Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
DONE
