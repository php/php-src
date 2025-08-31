--TEST--
Test array_shift() function : usage variations - position of internal pointer
--FILE--
<?php
/*
 * Test that the internal pointer is reset after calling array_shift()
 */

echo "*** Testing array_shift() : usage variations ***\n";

$stack = array ('one' => 'un', 'two' => 'deux');

echo "\n-- Call array_shift() --\n";
var_dump($result = array_shift($stack));

echo "\n-- Position of Internal Pointer in Passed Array: --\n";
echo key($stack) . " => " . current ($stack) . "\n";

echo "Done";
?>
--EXPECT--
*** Testing array_shift() : usage variations ***

-- Call array_shift() --
string(2) "un"

-- Position of Internal Pointer in Passed Array: --
two => deux
Done
