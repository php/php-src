--TEST--
Test array_shift() function : usage variations - call recursively
--FILE--
<?php
/* Prototype  : mixed array_shift(array &$stack)
 * Description: Pops an element off the beginning of the array 
 * Source code: ext/standard/array.c
 */

/*
 * Use the result of one call to array_shift 
 * as the $stack argument of another call to array_shift()
 * When done in one statement causes strict error messages. 
 */

echo "*** Testing array_shift() : usage variations ***\n";

$stack = array ( array ( array ('zero', 'one', 'two'), 'un', 'deux'), 'eins', 'zwei');

// not following strict standards
echo "\n-- Incorrect Method: --\n";
var_dump(array_shift(array_shift(array_shift($stack))));

$stack = array (array( array('zero', 'one', 'two'), 'un', 'deux'), 'eins', 'zwei');
// correct way of doing above:
echo "\n-- Correct Method: --\n";
$result1 = array_shift($stack);
$result2 = array_shift($result1);
var_dump(array_shift($result2));

echo "Done";
?>
--EXPECTF--
*** Testing array_shift() : usage variations ***

-- Incorrect Method: --

Notice: Only variables should be passed by reference in %s on line %d

Notice: Only variables should be passed by reference in %s on line %d
string(4) "zero"

-- Correct Method: --
string(4) "zero"
Done
