--TEST--
__compareTo: Compare against scalar values
--FILE--
<?php
include("Comparable.inc");

/**
 * Compare against scalar values, which are supported by the comparable class.
 */
var_dump(new Comparable(1) <=> 0);
var_dump(new Comparable(1) <=> 1.0);
var_dump(new Comparable(1) <=> 1.5);

?>
--EXPECTF--
int(1)
int(0)
int(-1)
