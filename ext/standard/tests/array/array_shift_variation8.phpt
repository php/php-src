--TEST--
Test array_shift() function : usage variations - maintaining referenced elements
--FILE--
<?php
/*
 * From a comment left by Traps on 09-Jul-2007 on the array_shift documentation page:
 * For those that may be trying to use array_shift() with an array containing references
 * (e.g. working with linked node trees), beware that array_shift() may not work as you expect:
 * it will return a *copy* of the first element of the array,
 * and not the element itself, so your reference will be lost.
 * The solution is to reference the first element before removing it with array_shift():
 */

echo "*** Testing array_shift() : usage variations ***\n";

// using only array_shift:
echo "\n-- Reference result of array_shift: --\n";
$a = 1;
$array = array(&$a);
$b =& array_shift($array);
$b = 2;
echo "a = $a, b = $b\n";

// solution: referencing the first element first:
echo "\n-- Reference first element before array_shift: --\n";
$a = 1;
$array = array(&$a);
$b =& $array[0];
array_shift($array);
$b = 2;
echo "a = $a, b = $b\n";

echo "Done";
?>
--EXPECTF--
*** Testing array_shift() : usage variations ***

-- Reference result of array_shift: --

Notice: Only variables should be assigned by reference in %s on line %d
a = 1, b = 2

-- Reference first element before array_shift: --
a = 2, b = 2
Done
