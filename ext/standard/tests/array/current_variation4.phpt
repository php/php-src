--TEST--
Test current() function : usage variations - multi-dimensional arrays
--FILE--
<?php
/* Prototype  : mixed current(array $array_arg)
 * Description: Return the element currently pointed to by the internal array pointer
 * Source code: ext/standard/array.c
 * Alias to functions: pos
 */

/*
 * Test how current() behaves with muti-dimensional and recursive arrays
 */

echo "*** Testing current() : usage variations ***\n";

echo "\n-- Two Dimensional Array --\n";
$multi_array = array ('zero', array (1, 2, 3), 'two');
echo "Initial Position: ";
var_dump(current($multi_array));

echo "Next Position:    ";
next($multi_array);
var_dump(current($multi_array));

echo "End Position:     ";
end($multi_array);
var_dump(current($multi_array));

echo "\n-- Access an Array Within an Array --\n";
//accessing an array within an array
echo "Initial Position: ";
var_dump(current($multi_array[1]));

echo "\n-- Recursive, Multidimensional Array --\n";
//create a recursive array
$multi_array[] = &$multi_array;

//See where internal pointer is after adding more elements
echo "Current Position: ";
var_dump(current($multi_array));

//see if internal pointer is in same position as referenced array
var_dump(current($multi_array[3][3][3]));
// see if internal pointer is in the same position from when accessing this inner array
var_dump(current($multi_array[3][3][3][1]));
?>
===DONE===
--EXPECTF--
*** Testing current() : usage variations ***

-- Two Dimensional Array --
Initial Position: string(4) "zero"
Next Position:    array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
End Position:     string(3) "two"

-- Access an Array Within an Array --
Initial Position: int(1)

-- Recursive, Multidimensional Array --
Current Position: string(3) "two"
string(3) "two"
int(1)
===DONE===
