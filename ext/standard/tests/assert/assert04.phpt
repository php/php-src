--TEST--
misc assert() tests tests
--INI--
assert.active = 1
assert.warning = 1
assert.callback =
assert.bail = 0
assert.exception=0
--FILE--
<?php
/* Assert not active */
assert_options(ASSERT_ACTIVE, 0);
assert(1);

/* Wrong parameter count in assert */
assert_options(ASSERT_ACTIVE, 1);

/* Assert false */
assert(0);


/* Assert false and bail*/
assert_options(ASSERT_BAIL, 1);
assert(0);

echo "not reached\n";

?>
--EXPECTF--
Deprecated: PHP Startup: assert.exception INI setting is deprecated in Unknown on line 0

Deprecated: Constant ASSERT_ACTIVE is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d

Deprecated: Constant ASSERT_ACTIVE is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d

Warning: assert(): assert(0) failed in %s on line %d

Deprecated: Constant ASSERT_BAIL is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d

Warning: assert(): assert(0) failed in %s on line %d
