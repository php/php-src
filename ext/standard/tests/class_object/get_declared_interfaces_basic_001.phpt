--TEST--
Test get_declared_interfaces() function : basic functionality
--FILE--
<?php
/* Prototype  : proto array get_declared_interfaces()
 * Description: Returns an array of all declared interfaces.
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */


echo "*** Testing get_declared_interfaces() : basic functionality ***\n";

// Zero arguments
echo "\n-- Testing get_declared_interfaces() function with Zero arguments --\n";
var_dump(get_declared_interfaces());

foreach (get_declared_interfaces() as $interface) {
	if (!interface_exists($interface)) {
		echo "Error: $interface is not a valid interface.\n";
	}
}

echo "\n-- Ensure userspace classes are not listed --\n";
Class C {}
var_dump(in_array('C', get_declared_interfaces()));

echo "\n-- Ensure userspace interfaces are listed --\n";
Interface I {}
var_dump(in_array('I', get_declared_interfaces()));

echo "Done";
?>
--EXPECTF--
*** Testing get_declared_interfaces() : basic functionality ***

-- Testing get_declared_interfaces() function with Zero arguments --
array(%d) {
%a
}

-- Ensure userspace classes are not listed --
bool(false)

-- Ensure userspace interfaces are listed --
bool(true)
Done
