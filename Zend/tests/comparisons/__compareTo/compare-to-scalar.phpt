--TEST--
__compareTo: Compare against scalar values
--FILE--
<?php
include("Comparable.inc");

/**
 * Compare against scalar values, which are supported by our comparable class.
 */
var_dump(new Comparable(1) <=> 0);      // 1
var_dump(new Comparable(1) <=> 1);      // 0
var_dump(new Comparable(1) <=> '1');    // 0
var_dump(new Comparable(1) <=> 1.0);    // 0
var_dump(new Comparable(1) <=> 1.5);    // -1

/**
 * We didn't define comparison against non-numeric strings.
 */
var_dump(new Comparable(1) <=> 'a');

?>
--EXPECTF--
int(1)
int(0)
int(0)
int(0)
int(-1)

Fatal error: Uncaught Exception: Failed to compare in %s:%d
Stack trace:
#0 %s: Comparable->__compareTo('a')
#1 {main}
  thrown in %s on line %d
