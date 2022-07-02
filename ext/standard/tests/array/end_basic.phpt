--TEST--
Test end() function : basic functionality
--FILE--
<?php
/*
 * Test basic functionality of end()
 */

echo "*** Testing end() : basic functionality ***\n";

$array = array('zero', 'one', 200 => 'two');

echo "\n-- Initial Position: --\n";
echo key($array) . " => " . current($array) . "\n";

echo "\n-- Call to end() --\n";
var_dump(end($array));

echo "\n-- Current Position: --\n";
echo key($array) . " => " . current($array) . "\n";

echo "\n-- Add a new element to array --\n";
$array[2] = 'foo';
var_dump(end($array));
?>
--EXPECT--
*** Testing end() : basic functionality ***

-- Initial Position: --
0 => zero

-- Call to end() --
string(3) "two"

-- Current Position: --
200 => two

-- Add a new element to array --
string(3) "foo"
