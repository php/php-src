--TEST--
method_exists() on non-existent class, with __autoload().
--FILE--
<?php
/* Prototype  : proto bool is_subclass_of(object object, string class_name)
 * Description: Returns true if the object has this class as one of its parents 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

function __autoload($name) {
	echo "In __autoload($name)\n"; 
} 

var_dump(method_exists('UndefC', 'func'));

echo "Done";
?>
--EXPECTF--
In __autoload(UndefC)
bool(false)
Done