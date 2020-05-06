--TEST--
Test class_exists() function : usage variations  - case sensitivity
--FILE--
<?php
/* Description: Checks if the class exists
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

class caseSensitivityTest {}
var_dump(class_exists('casesensitivitytest'));

echo "Done"
?>
--EXPECT--
bool(true)
Done
