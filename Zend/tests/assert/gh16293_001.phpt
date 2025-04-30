--TEST--
GH-16293: Exception in assert() callback with bail enabled
--FILE--
<?php

@assert_options(ASSERT_EXCEPTION, 0);
@assert_options(ASSERT_BAIL, 1);
@assert_options(ASSERT_CALLBACK, 'f1');
assert(false);

?>
--EXPECTF--
Warning: assert(): assert(false) failed in %s on line %d

Warning: Uncaught Error: Invalid callback f1, function "f1" not found or invalid function name in %s:%d
Stack trace:
#0 %s(%d): assert(false, 'assert(false)')
#1 {main}
  thrown in %s on line %d
