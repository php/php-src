--TEST--
Test current() function : basic functionality
--FILE--
<?php
/* Prototype  : mixed current(array $array_arg)
 * Description: Return the element currently pointed to by the internal array pointer
 * Source code: ext/standard/array.c
 */

/*
 * Test basic functionality of current()
 */

echo "*** Testing current() : basic functionality ***\n";

$array = array ('zero', 'one', 'two', 'three' => 3);
var_dump(current($array));
next($array);
var_dump(current($array));
end($array);
var_dump(current($array));
next($array);
var_dump(current($array));
?>
===DONE===
--EXPECT--
*** Testing current() : basic functionality ***
string(4) "zero"
string(3) "one"
int(3)
bool(false)
===DONE===
