--TEST--
Test next() function : basic functionality
--FILE--
<?php
/*
 * Test basic functionality of next()
 */

echo "*** Testing next() : basic functionality ***\n";

$array = array('zero', 'one', 'two');
echo key($array) . " => " . current($array) . "\n";
var_dump(next($array));

echo key($array) . " => " . current($array) . "\n";
var_dump(next($array));

echo key($array) . " => " . current($array) . "\n";
var_dump(next($array));
?>
--EXPECT--
*** Testing next() : basic functionality ***
0 => zero
string(3) "one"
1 => one
string(3) "two"
2 => two
bool(false)
