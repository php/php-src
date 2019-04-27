--TEST--
Test get_declared_classes() function : basic functionality
--FILE--
<?php
/* Prototype  : proto array get_declared_classes()
 * Description: Returns an array of all declared classes.
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */


echo "*** Testing get_declared_classes() : basic functionality ***\n";

// Zero arguments
echo "\n-- Testing get_declared_classes() function with Zero arguments --\n";
var_dump(get_declared_classes());

foreach (get_declared_classes() as $class) {
	if (!class_exists($class)) {
		echo "Error: $class is not a valid class.\n";
	}
}

echo "\n-- Ensure userspace classes are listed --\n";
Class C {}
var_dump(in_array('C', get_declared_classes()));

echo "\n-- Ensure userspace interfaces are not listed --\n";
Interface I {}
var_dump(in_array( 'I', get_declared_classes()));

echo "Done";
?>
--EXPECTF--
*** Testing get_declared_classes() : basic functionality ***

-- Testing get_declared_classes() function with Zero arguments --
array(%d) {
%a
}

-- Ensure userspace classes are listed --
bool(true)

-- Ensure userspace interfaces are not listed --
bool(false)
Done
