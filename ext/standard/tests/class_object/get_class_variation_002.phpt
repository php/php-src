--TEST--
Test get_class() function : usage variations  - ensure class name case is preserved.
--FILE--
<?php
/* Prototype  : proto string get_class([object object])
 * Description: Retrieves the class name 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

class caseSensitivityTest {}
var_dump(get_class(new casesensitivitytest));

echo "Done";
?>
--EXPECTF--
string(19) "caseSensitivityTest"
Done