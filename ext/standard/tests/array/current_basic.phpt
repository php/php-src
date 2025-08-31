--TEST--
Test current() function : basic functionality
--FILE--
<?php
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
--EXPECT--
*** Testing current() : basic functionality ***
string(4) "zero"
string(3) "one"
int(3)
bool(false)
