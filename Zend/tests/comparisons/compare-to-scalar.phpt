--TEST--
__compareTo: Compare against scalar values
--FILE--
<?php
include("setup.inc");

/**
 * Compare against scalar values, which are supported by the comparable class.
 */
var_dump(new Comparable(1) <=> 0);
var_dump(new Comparable(1) <=> 1.0);
var_dump(new Comparable(1) <=> 1.5);

/**
 * We didn't implement something that compares against strings, so __compareTo
 * will return null, which will be converted to 0, and therefore equal.
 */
var_dump(new Comparable(1) <=> 'a');
var_dump(new Comparable(1) <=> 'b');
var_dump(new Comparable(1) <=> 'c');


?>
--EXPECTF--
int(1)
int(0)
int(-1)
int(0)
int(0)
int(0)
