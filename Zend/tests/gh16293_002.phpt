--TEST--
GH-16293: Exception in assert() callback with bail enabled
--FILE--
<?php

@assert_options(ASSERT_EXCEPTION, 0);
@assert_options(ASSERT_BAIL, 1);
@assert_options(ASSERT_CALLBACK, function () {
    throw new Exception('Boo');
});
assert(false);

?>
--EXPECTF--
Warning: assert(): assert(false) failed in %s on line %d

Warning: Uncaught Exception: Boo in %s:%d
Stack trace:
%a
