--TEST--
misc assert() tests tests
--FILE--
<?php
/* Assert not active */
assert_options(ASSERT_ACTIVE, 0);
assert(1);
 
 
/* Wrong parameter count in assert */
assert_options(ASSERT_ACTIVE, 1);
assert(2,3);
 
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
Warning: Wrong parameter count for assert() in %s on line %d

Warning: Wrong parameter count for assert_options() in %s on line %d

Warning: assert_options(): Unknown value 0 in %s on line %d

Warning: assert(): Assertion failed in %s on line %d

Warning: assert(): Assertion failed in %s on line %d
