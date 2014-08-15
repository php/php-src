--TEST--
misc assert() tests tests
--INI--
assert.active = 1
assert.warning = 1
assert.callback = 
assert.bail = 0
assert.quiet_eval = 0
--FILE--
<?php
/* Assert not active */
assert_options(ASSERT_ACTIVE, 0);
assert(1);
 
 
/* Wrong parameter count in assert */
assert_options(ASSERT_ACTIVE, 1);
assert(2, "failure", 3);
 
/* Wrong parameter count in assert_options */
assert_options(ASSERT_ACTIVE, 0, 2);
 
/* Wrong parameter name in assert_options */
$test="ASSERT_FRED";
assert_options($test, 1);
 
/* Assert false */
assert(0);
 
 
/* Assert false and bail*/
assert_options(ASSERT_BAIL, 1);
assert(0);
 
echo "not reached\n";
 
?>
--EXPECTF--
Warning: assert() expects at most 2 parameters, 3 given in %s on line %d

Warning: assert_options() expects at most 2 parameters, 3 given in %s on line %d

Warning: assert_options() expects parameter 1 to be integer, %unicode_string_optional% given in %s on line %d

Warning: assert(): Assertion failed in %s on line %d

Warning: assert(): Assertion failed in %s on line %d

