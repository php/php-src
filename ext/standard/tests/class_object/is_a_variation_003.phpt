--TEST--
Test is_a() function : usage variations  - case sensitivity
--FILE--
<?php
/* Description: Returns true if the object is of this class or has this class as one of its parents
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

echo "*** Testing is_a() : usage variations ***\n";

class caseSensitivityTest {}
class caseSensitivityTestChild extends caseSensitivityTest {}

var_dump(is_a(new caseSensitivityTestChild, 'caseSensitivityTEST'));

echo "Done";
?>
--EXPECT--
*** Testing is_a() : usage variations ***
bool(true)
Done
