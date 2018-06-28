--TEST--
__equals: Compare against scalar values
--FILE--
<?php
include("Equatable.inc");

/**
 * Compare against scalar values, which are supported by our equatable class.
 */
var_dump(new Equatable(1) == 0);    // false
var_dump(new Equatable(1) == 1);    // true
var_dump(new Equatable(1) == '1');  // true
var_dump(new Equatable(1) == 1.0);  // true
var_dump(new Equatable(1) == 1.5);  // false

/**
 * We didn't define comparison against non-numeric strings.
 */
var_dump(new Equatable(1) == 'a');

?>
--EXPECTF--
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)

Fatal error: Uncaught Exception: Failed to equate in %s:%d
Stack trace:
#0 %s: Equatable->__equals('a')
#1 {main}
  thrown in %s on line %d
