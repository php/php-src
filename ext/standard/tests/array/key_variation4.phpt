--TEST--
Test key() function : usage variations
--FILE--
<?php
/* Prototype  : mixed key(array $array_arg)
 * Description: Return the key of the element currently pointed to by the internal array pointer
 * Source code: ext/standard/array.c
 */

/*
 * Test how key() behaves with muti-dimensional and recursive arrays
 */

echo "*** Testing key() : usage variations ***\n";

echo "\n-- Two Dimensional Array --\n";
$multi_array = array ('zero', array (1, 2, 3), 'two');
echo "Initial Position: ";
var_dump(key($multi_array));

echo "Next Position:    ";
next($multi_array);
var_dump(key($multi_array));

echo "End Position:     ";
end($multi_array);
var_dump(key($multi_array));

echo "\n-- Access an Array Within an Array --\n";
//accessing an array within an array
echo "Initial Position: ";
var_dump(key($multi_array[1]));

echo "\n-- Recursive, Multidimensional Array --\n";
//create a recursive array
$multi_array[] = &$multi_array;

//See where internal pointer is after adding more elements
echo "Current Position: ";
var_dump(key($multi_array));

//see if internal pointer is in same position as referenced array
var_dump(key($multi_array[3][3][3]));
// see if internal pointer is in the same position from when accessing this inner array
var_dump(key($multi_array[3][3][3][1]));
?>
===DONE===
--EXPECT--
*** Testing key() : usage variations ***

-- Two Dimensional Array --
Initial Position: int(0)
Next Position:    int(1)
End Position:     int(2)

-- Access an Array Within an Array --
Initial Position: int(0)

-- Recursive, Multidimensional Array --
Current Position: int(2)
int(2)
int(0)
===DONE===
