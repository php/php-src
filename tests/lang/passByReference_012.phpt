--TEST--
Test pass by reference semantics
--FILE--
<?php
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
--EXPECTF--
Notice: Only variables should be passed by reference in %s on line %d
string(3) "two"

Notice: Only variables should be passed by reference in %s on line %d
string(3) "one"
