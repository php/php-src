--TEST--
Test reset() function : usage variations - unset first element
--FILE--
<?php
/*
 * Unset first element of an array and test behaviour of reset()
 */

echo "*** Testing reset() : usage variations ***\n";

$array = array('a', 'b', 'c');

echo "\n-- Initial Position: --\n";
echo current($array) . " => " . key($array) . "\n";

echo "\n-- Unset First element in array and check reset() --\n";
unset($array[0]);
var_dump(reset($array));
?>
--EXPECT--
*** Testing reset() : usage variations ***

-- Initial Position: --
a => 0

-- Unset First element in array and check reset() --
string(1) "b"
