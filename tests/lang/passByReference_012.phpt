--TEST--
Test pass by reference semantics
--FILE--
<?php
error_reporting(E_ALL | E_STRICT | E_NOTICE);

// Simplified array_shift_variation5.phpt
// Showing warning:
// "Only variables should be passed by reference in %s on line %d"
$stack = array ( array ( 'two' ));
var_dump(array_shift(array_shift($stack)));

// This should show the identical warning 
$original = array ( array ( 'one' ));
$stack = $original;
var_dump(array_shift(array_shift($stack)));
?>
===DONE===
--EXPECTF--
Strict Standards: Only variables should be passed by reference in %s on line %d
string(3) "two"

Strict Standards: Only variables should be passed by reference in %s on line %d
string(3) "one"
===DONE===
