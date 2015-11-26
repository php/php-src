--TEST--
Test class_exists() function : basic functionality 
--FILE--
<?php
/* Prototype  : proto bool class_exists(string classname [, bool autoload])
 * Description: Checks if the class exists 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing class_exists() : basic functionality ***\n";

function __autoload($className) {
	echo "In __autoload($className)\n";
}

echo "Calling class_exists() on non-existent class with autoload explicitly enabled:\n";
var_dump( class_exists('C', true) );
echo "\nCalling class_exists() on existing class with autoload explicitly enabled:\n";
var_dump( class_exists('stdclass', true) );

echo "\nCalling class_exists() on non-existent class with autoload explicitly enabled:\n";
var_dump( class_exists('D', false) );
echo "\nCalling class_exists() on existing class with autoload explicitly disabled:\n";
var_dump( class_exists('stdclass', false) );

echo "\nCalling class_exists() on non-existent class with autoload unspecified:\n";
var_dump( class_exists('E') );
echo "\nCalling class_exists() on existing class with autoload unspecified:\n";
var_dump( class_exists('stdclass') );

echo "Done";
?>
--EXPECTF--
*** Testing class_exists() : basic functionality ***
Calling class_exists() on non-existent class with autoload explicitly enabled:
In __autoload(C)
bool(false)

Calling class_exists() on existing class with autoload explicitly enabled:
bool(true)

Calling class_exists() on non-existent class with autoload explicitly enabled:
bool(false)

Calling class_exists() on existing class with autoload explicitly disabled:
bool(true)

Calling class_exists() on non-existent class with autoload unspecified:
In __autoload(E)
bool(false)

Calling class_exists() on existing class with autoload unspecified:
bool(true)
Done