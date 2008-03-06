--TEST--
Test method_exists() function : variation - Case sensitivity
--FILE--
<?php
/* Prototype  : proto bool method_exists(object object, string method)
 * Description: Checks if the class method exists 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing method_exists() : variation ***\n";

Class caseSensitivityTest {
	public function myMethod() {}
}

var_dump(method_exists(new casesensitivitytest, 'myMetHOD'));
var_dump(method_exists('casesensiTivitytest', 'myMetHOD'));

echo "Done";
?>
--EXPECTF--
*** Testing method_exists() : variation ***
bool(true)
bool(true)
Done