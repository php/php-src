--TEST--
Test count() function : Infinitely recursive array warning disaply
--FILE--
<?php
/**
 * Prototype  : int count(array|Countable|null $var [, int $mode])
 * Description: Count the number of elements in a variable (usually an array)
 * Source code: ext/standard/array.c
 */

/*
 * Pass count() an infinitely recursive array as $var argument
 * This will stop the script before it reaches the end.
 */

$array = [1, 2, 'three'];
// get an infinitely recursive array
$array[] = &$array;

echo "-- \$mode not set: --\n";
var_dump(count ($array));

echo "\n-- \$mode = COUNT_RECURSIVE: --\n";
var_dump(count ($array, COUNT_RECURSIVE));

?>
--EXPECTF--
-- $mode not set: --
int(4)

-- $mode = COUNT_RECURSIVE: --

Warning: count(): Recursion detected in %s on line %d
int(4)
