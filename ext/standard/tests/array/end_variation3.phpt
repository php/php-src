--TEST--
Test end() function : usage variations - Referenced variables
--FILE--
<?php
/*
 * Test how the internal pointer is affected when two variables are referenced to each other
 */

echo "*** Testing end() : usage variations ***\n";

$array1 = array ('zero', 'one', 'two');

echo "\n-- Initial position of internal pointer --\n";
var_dump(current($array1));
end($array1);

// Test that when two variables are referenced to one another
// the internal pointer is the same for both
$array2 = &$array1;
echo "\n-- Position after calling end() --\n";
echo "\$array1: ";
var_dump(current($array1));
echo "\$array2: ";
var_dump(current($array2));
?>
--EXPECT--
*** Testing end() : usage variations ***

-- Initial position of internal pointer --
string(4) "zero"

-- Position after calling end() --
$array1: string(3) "two"
$array2: string(3) "two"
