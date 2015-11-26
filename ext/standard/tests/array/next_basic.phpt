--TEST--
Test next() function : basic functionality 
--FILE--
<?php
/* Prototype  : mixed next(array $array_arg)
 * Description: Move array argument's internal pointer to the next element and return it 
 * Source code: ext/standard/array.c
 */

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
===DONE===
--EXPECTF--
*** Testing next() : basic functionality ***
0 => zero
string(3) "one"
1 => one
string(3) "two"
2 => two
bool(false)
===DONE===
