--TEST--
Test get_loaded_extensions() function : basic functionality
--FILE--
<?php
/* Prototype  : array get_loaded_extensions  ([ bool $zend_extensions= false  ] )
 * Description:  Returns an array with the names of all modules compiled and loaded
 * Source code: Zend/zend_builtin_functions.c
 */

echo "*** Testing get_loaded_extensions() : basic functionality ***\n";

echo "Get loaded extensions\n";
var_dump(get_loaded_extensions());

?>
===DONE===
--EXPECTF--
*** Testing get_loaded_extensions() : basic functionality ***
Get loaded extensions
array(%d) {
%a
}
===DONE===
