--TEST--
Test pass by reference semantics
--FILE--
<?php
// Simplified array_shift_variation5.phpt
// Showing warning:
// "Only variables should be passed by reference in %s on line %d"
$stack = array ( array ( 'two' ));
try {
	var_dump(array_shift(array_shift($stack)));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}

// This should show the identical warning
$original = array ( array ( 'one' ));
$stack = $original;
try {
	var_dump(array_shift(array_shift($stack)));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Exception: Cannot pass parameter 1 by reference
Exception: Cannot pass parameter 1 by reference
