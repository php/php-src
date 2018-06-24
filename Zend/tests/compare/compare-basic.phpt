--TEST--
__compareTo: Basic comparison behaviour
--FILE--
<?php
include("Comparable.inc");

/**
 * Compare against instance of self, based on value. We're giving the second
 * object a higher decoy to make sure it'll break if default comparison is used.
 */
var_dump(new Comparable(1, 2) <=> new Comparable(0, 1));
var_dump(new Comparable(1, 2) <=> new Comparable(1, 1));
var_dump(new Comparable(1, 2) <=> new Comparable(2, 1));

?>
--EXPECTF--

int(1)
int(0)
int(-1)
