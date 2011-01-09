--TEST--
Test trait_exists() function : basic functionality 
--FILE--
<?php
/* Prototype  : proto bool trait_exists(string traitname [, bool autoload])
 * Description: Checks if the trait exists 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing trait_exists() : basic functionality ***\n";

function __autoload($traitName) {
	echo "In __autoload($traitName)\n";
}

trait MyTrait {}

echo "Calling trait_exists() on non-existent trait with autoload explicitly enabled:\n";
var_dump( trait_exists('C', true) );
echo "\nCalling trait_exists() on existing trait with autoload explicitly enabled:\n";
var_dump( trait_exists('MyTrait', true) );

echo "\nCalling trait_exists() on non-existent trait with autoload explicitly enabled:\n";
var_dump( trait_exists('D', false) );
echo "\nCalling trait_exists() on existing trait with autoload explicitly disabled:\n";
var_dump( trait_exists('MyTrait', false) );

echo "\nCalling trait_exists() on non-existent trait with autoload unspecified:\n";
var_dump( trait_exists('E') );
echo "\nCalling trait_exists() on existing trait with autoload unspecified:\n";
var_dump( trait_exists('MyTrait') );

echo "Done";
?>
--EXPECTF--
*** Testing trait_exists() : basic functionality ***
Calling trait_exists() on non-existent trait with autoload explicitly enabled:
In __autoload(C)
bool(false)

Calling trait_exists() on existing trait with autoload explicitly enabled:
bool(true)

Calling trait_exists() on non-existent trait with autoload explicitly enabled:
bool(false)

Calling trait_exists() on existing trait with autoload explicitly disabled:
bool(true)

Calling trait_exists() on non-existent trait with autoload unspecified:
In __autoload(E)
bool(false)

Calling trait_exists() on existing trait with autoload unspecified:
bool(true)
Done