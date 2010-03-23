--TEST--
Test is_subclass_of() function : usage variations  - case sensitivity
--FILE--
<?php
/* Prototype  : proto bool is_subclass_of(object object, string class_name)
 * Description: Returns true if the object has this class as one of its parents 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing is_subclass_of() : usage variations ***\n";

echo "*** Testing is_a() : usage variations ***\n";

class caseSensitivityTest {}
class caseSensitivityTestChild extends caseSensitivityTest {}

var_dump(is_subclass_of('caseSensitivityTestCHILD', 'caseSensitivityTEST'));

echo "Done"
?>
--EXPECTF--
*** Testing is_subclass_of() : usage variations ***
*** Testing is_a() : usage variations ***
bool(true)
Done