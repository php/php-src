--TEST--
Test get_parent_class() function : variation - case sensitivity
--FILE--
<?php
/* Prototype  : proto string get_parent_class([mixed object])
 * Description: Retrieves the parent class name for object or class or current scope.
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

//  Note: basic use cases in Zend/tests/010.phpt

echo "*** Testing get_parent_class() : variation ***\n";

class caseSensitivityTest {}
class caseSensitivityTestChild extends caseSensitivityTest {}

var_dump(get_parent_class('CasesensitivitytestCHILD'));
var_dump(get_parent_class(new CasesensitivitytestCHILD));

echo "Done";
?>
--EXPECTF--
*** Testing get_parent_class() : variation ***
string(19) "caseSensitivityTest"
string(19) "caseSensitivityTest"
Done
