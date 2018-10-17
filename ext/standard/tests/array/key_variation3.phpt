--TEST--
Test key() function : usage variations
--FILE--
<?php
/* Prototype  : mixed key(array $array_arg)
 * Description: Return the key of the element currently pointed to by the internal array pointer
 * Source code: ext/standard/array.c
 */

/*
 * Test how the internal pointer is affected when two variables are referenced to each other
 */

echo "*** Testing key() : usage variations ***\n";

$array1 = array ('zero', 'one', 'two');

echo "\n-- Initial position of internal pointer --\n";
var_dump(key($array1));

// Test that when two variables are referenced to one another
// the internal pointer is the same for both
$array2 = &$array1;

next($array1);

echo "\n-- Position after calling next() --\n";
echo "\$array1: ";
var_dump(key($array1));
echo "\$array2: ";
var_dump(key($array2));
?>
===DONE===
--EXPECT--
*** Testing key() : usage variations ***

-- Initial position of internal pointer --
int(0)

-- Position after calling next() --
$array1: int(1)
$array2: int(1)
===DONE===
