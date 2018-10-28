--TEST--
Test reset() function : usage variations - Referenced variables
--FILE--
<?php
/* Prototype  : mixed reset(array $array_arg)
 * Description: Set array argument's internal pointer to the first element and return it
 * Source code: ext/standard/array.c
 */

/*
 * Reference two arrays to each other then call reset() to test position of
 * internal pointer in both arrays
 */

echo "*** Testing reset() : usage variations ***\n";

$array1 = array ('zero', 'one', 'two');

echo "\n-- Initial position of internal pointer --\n";
var_dump(current($array1));

// Test that when two variables are referenced to one another
// the internal pointer is the same for both
$array2 = &$array1;

next($array1);

echo "\n-- Position after calling next() --\n";
echo "\$array1: ";
var_dump(current($array1));
echo "\$array2: ";
var_dump(current($array2));

echo "\n-- Position after calling reset() --\n";
var_dump(reset($array1));
echo "\$array1: ";
var_dump(current($array1));
echo "\$array2: ";
var_dump(current($array2));
?>
===DONE===
--EXPECT--
*** Testing reset() : usage variations ***

-- Initial position of internal pointer --
string(4) "zero"

-- Position after calling next() --
$array1: string(3) "one"
$array2: string(3) "one"

-- Position after calling reset() --
string(4) "zero"
$array1: string(4) "zero"
$array2: string(4) "zero"
===DONE===
