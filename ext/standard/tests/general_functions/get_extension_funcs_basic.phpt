--TEST--
Test get_extension_funcs() function: basic test
--FILE--
<?php
/* Prototype  : array get_extension_funcs  ( string $module_name  )
 * Description: Returns an array with the names of the functions of a module.
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

echo "Simple testcase for get_extension_funcs() function\n";

$result = get_extension_funcs("standard");
var_dump(gettype($result));
var_dump(in_array("cos", $result));

?>
===DONE===
--EXPECTF--
Simple testcase for get_extension_funcs() function
string(5) "array"
bool(true)
===DONE===
