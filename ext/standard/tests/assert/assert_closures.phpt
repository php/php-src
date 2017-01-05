--TEST--
assert() - basic - accept closures as callback.
--INI--
assert.active = 1
assert.warning = 1
assert.bail = 0
assert.quiet_eval = 0
--FILE--
<?php
assert_options(ASSERT_CALLBACK, function () { echo "Hello World!\n"; });
assert(0);
?>
--EXPECTF--
Hello World!

Warning: assert(): assert(0) failed in %s on line %d
