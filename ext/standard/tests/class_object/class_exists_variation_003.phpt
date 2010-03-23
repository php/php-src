--TEST--
Test class_exists() function : usage variations  - case sensitivity
--FILE--
<?php
/* Prototype  : proto bool class_exists(string classname [, bool autoload])
 * Description: Checks if the class exists 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

class caseSensitivityTest {}
var_dump(class_exists('casesensitivitytest'));

echo "Done"
?>
--EXPECTF--
bool(true)
Done