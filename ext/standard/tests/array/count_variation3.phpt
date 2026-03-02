--TEST--
Test count() function : usage variations - Infinitely recursive array
--FILE--
<?php
/*
 * Pass count() an infinitely recursive array as $var argument
 * This will stop the script before it reaches the end.
 */

echo "*** Testing count() : usage variations ***\n";

$array1 = array (1, 2, 'three');
// get an infinitely recursive array
$array1[] = &$array1;

echo "\n-- \$mode not set: --\n";
var_dump(count ($array1));

echo "\n-- \$mode = 1: --\n";
var_dump(count ($array1, 1));

echo "Done";
?>
--EXPECTF--
*** Testing count() : usage variations ***

-- $mode not set: --
int(4)

-- $mode = 1: --

Warning: count(): Recursion detected in %s on line %d
int(4)
Done
