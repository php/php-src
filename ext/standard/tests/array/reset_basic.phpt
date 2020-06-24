--TEST--
Test reset() function : basic functionality
--FILE--
<?php
/*
 * Test basic functionality of reset()
 */

echo "*** Testing reset() : basic functionality ***\n";

$array = array('zero', 'one', 200 => 'two');

echo "\n-- Initial Position: --\n";
echo key($array) . " => " . current($array) . "\n";

echo "\n-- Call to next() --\n";
var_dump(next($array));

echo "\n-- Current Position: --\n";
echo key($array) . " => " . current($array) . "\n";

echo "\n-- Call to reset() --\n";
var_dump(reset($array));
?>
--EXPECT--
*** Testing reset() : basic functionality ***

-- Initial Position: --
0 => zero

-- Call to next() --
string(3) "one"

-- Current Position: --
1 => one

-- Call to reset() --
string(4) "zero"
