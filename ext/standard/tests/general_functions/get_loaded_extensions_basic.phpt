--TEST--
Test get_loaded_extensions() function : basic functionality
--FILE--
<?php
/* Description:  Returns an array with the names of all modules compiled and loaded
 * Source code: Zend/zend_builtin_functions.c
 */

echo "*** Testing get_loaded_extensions() : basic functionality ***\n";

echo "Get loaded extensions\n";
var_dump(get_loaded_extensions());

?>
--EXPECTF--
*** Testing get_loaded_extensions() : basic functionality ***
Get loaded extensions
array(%d) {
%a
}
